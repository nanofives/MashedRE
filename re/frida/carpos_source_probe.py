# carpos_source_probe.py — find which per-car position copies feed the
# HUD/effect elements (shadow, "!" marker, powerup icon) that still step at
# tick rate under MASHED_INTERP.
#
# Method (QOL_PATCH_PLAN_2026-08.md "now-cheap iteration method"): attach to an
# EXPLICIT --pid (never guess), hook the render entry FUN_00492e90 (0x00492e90,
# ~60/s — Interceptor-safe), and for one candidate at a time add +80 to the Y of
# that candidate position during the render only (restore in onLeave). Then
# trigger an on-demand backbuffer dump via the d3d9 shim's MASHED_ORIG_BBDUMP_REQ
# request file. The element that FLOATS in the dump reads that source.
#
# Candidates are auto-discovered: scan each car record (base 0x008815a0, stride
# 0xd04) and the fixed 0x0063d000..0x00640000 global window for float triplets
# matching the car's active render-matrix position (0x00881ec8 + i*0xd04 +
# act(+0x9A8)*0x40, pos at +0x30 — layout cited from FUN_0046d4a0 @0x0046d4a0).
#
# Usage:
#   py -3.12 re\frida\carpos_source_probe.py --pid <pid> --req <reqfile> --out verify\carpos_probe
#
# The target process must have been spawned with MASHED_ORIG_BBDUMP_REQ=<reqfile>.
import argparse, os, struct, sys, time

import frida

AGENT = r"""
'use strict';
const REND    = ptr('0x00492e90');
const RECBASE = 0x008815a0, STRIDE = 0xd04;
const MATBASE = 0x00881ec8, IDXOFF = 0x00881f48;
const COUNT   = ptr('0x008a94d0');
const PHASE   = ptr('0x00771968');

let g_lift = null;          // {addrs: [NativePointer y-addrs], dy}
let g_saved = [];

function rf(a) { return a.readFloat(); }

rpc.exports = {
  status() {
    return { phase: PHASE.readU32(), cars: COUNT.readS32() };
  },
  // active render-matrix info per car (validates FUN_0046d4a0 modeling)
  actinfo() {
    const n = Math.min(COUNT.readS32(), 16), out = [];
    for (let i = 0; i < n; i++) {
      const act = ptr(IDXOFF + i * STRIDE).readS32();
      const m = ptr(MATBASE + i * STRIDE + act * 0x40);
      out.push({ car: i, act: act,
                 pos: [rf(m.add(0x30)), rf(m.add(0x34)), rf(m.add(0x38))] });
    }
    return out;
  },
  // scan car 0's record for float triplets ~= its active matrix pos
  scanrec() {
    const act = ptr(IDXOFF).readS32();
    const m = ptr(MATBASE + act * 0x40);
    const rx = rf(m.add(0x30)), ry = rf(m.add(0x34)), rz = rf(m.add(0x38));
    const hits = [];
    for (let off = 0; off <= STRIDE - 12; off += 4) {
      const p = ptr(RECBASE + off);
      const x = rf(p), y = rf(p.add(4)), z = rf(p.add(8));
      if (Math.abs(x - rx) < 0.75 && Math.abs(y - ry) < 0.75 &&
          Math.abs(z - rz) < 0.75)
        hits.push('0x' + off.toString(16));
    }
    return { pos: [rx, ry, rz], hits: hits };
  },
  // scan a fixed VA window for triplets ~= car 0's active matrix pos
  scanva(lo, hi) {
    const act = ptr(IDXOFF).readS32();
    const m = ptr(MATBASE + act * 0x40);
    const rx = rf(m.add(0x30)), ry = rf(m.add(0x34)), rz = rf(m.add(0x38));
    const hits = [];
    for (let a = lo; a <= hi - 12; a += 4) {
      let x, y, z;
      try { const p = ptr(a); x = rf(p); y = rf(p.add(4)); z = rf(p.add(8)); }
      catch (e) { a += 0xffc; continue; }   // skip unmapped pages
      if (Math.abs(x - rx) < 0.75 && Math.abs(y - ry) < 0.75 &&
          Math.abs(z - rz) < 0.75)
        hits.push('0x' + a.toString(16));
    }
    return { pos: [rx, ry, rz], hits: hits };
  },
  // arm a lift: recOff != null -> record-relative for ALL cars; else absolute VAs
  setlift(recOff, vas, dy) {
    if (recOff === null && (!vas || !vas.length)) { g_lift = null; return 0; }
    const addrs = [];
    if (recOff !== null) {
      const n = Math.min(COUNT.readS32(), 16);
      for (let i = 0; i < n; i++)
        addrs.push(ptr(RECBASE + i * STRIDE + recOff + 4));   // +4 = Y
    } else {
      for (const v of vas) addrs.push(ptr(v).add(4));
    }
    g_lift = { addrs: addrs, dy: dy };
    return addrs.length;
  },
  // root-frame pair: like pair() but lifts every car's ROOT RwFrame matrix.
  // which: 'model' = modelling +0x40 Y, 'ltm' = LTM +0x80 Y, 'both'
  pairroot(which, dy, reqPath, aPath, bPath, cPath) {
    const n = Math.min(COUNT.readS32(), 16), addrs = [];
    for (let i = 0; i < n; i++) {
      const f = carRootFrame(i);
      if (!f) continue;
      if (which === 'model' || which === 'both') addrs.push(f.add(0x10 + 0x30 + 4));
      if (which === 'ltm'   || which === 'both') addrs.push(f.add(0x50 + 0x30 + 4));
    }
    g_freeze = true;
    g_pair = { addrs: addrs, dy: dy, req: reqPath,
               a: aPath, b: bPath, c: cPath || null, state: -2 };
    return addrs.length;
  },
  // consecutive-frame pair: frame N renders with the lift + dumps to aPath,
  // frame N+1 renders clean + dumps to bPath (16ms apart -> scene motion tiny,
  // the +dy displacement dominates the diff)
  pair(recOff, vas, dy, reqPath, aPath, bPath, cPath) {
    const addrs = [];
    if (recOff !== null) {
      const n = Math.min(COUNT.readS32(), 16);
      for (let i = 0; i < n; i++)
        addrs.push(ptr(RECBASE + i * STRIDE + recOff + 4));
    } else {
      for (const v of vas) addrs.push(ptr(v).add(4));
    }
    g_freeze = true;
    g_pair = { addrs: addrs, dy: dy, req: reqPath, a: aPath, b: bPath,
               c: cPath || null, state: -2 };
    return addrs.length;
  },
};

// car RwFrame roots (same walk as mashed_qol.cpp interp): renderable
// *(0x0063da18 + i*0x2ac) -> frame *(rend+0x4) -> root *(frame+0xa0).
// modelling matrix +0x10 (pos +0x40), LTM +0x50 (pos +0x80).
function carRootFrame(i) {
  const rend = ptr(0x0063da18 + i * 0x2ac).readPointer();
  if (rend.compare(ptr(0x10000)) < 0) return null;
  const fa = rend.add(0x4).readPointer();
  if (fa.compare(ptr(0x10000)) < 0) return null;
  const root = fa.add(0xa0).readPointer();
  return root.compare(ptr(0x10000)) >= 0 ? root : fa;
}

// Keep the game rendering while unfocused: FUN_00499690 (0x00499690) blocks in
// WaitMessage() while the active flag DAT_0077391c == 0. Force it 1 (the
// WM_ACTIVATE handler rewrites 0 on focus loss, so re-force periodically) and
// post WM_NULL to every thread once to wake an already-blocked WaitMessage.
const ACTIVE_FLAG = ptr('0x0077391c');
const postThreadMsg = new NativeFunction(
  Process.getModuleByName('user32.dll').getExportByName('PostThreadMessageA'),
  'int', ['uint32', 'uint32', 'pointer', 'pointer'], 'stdcall');
function forceActive() {
  try { ACTIVE_FLAG.writeU32(1); } catch (e) {}
}
forceActive();
for (const t of Process.enumerateThreads())
  postThreadMsg(t.id, 0 /*WM_NULL*/, NULL, NULL);
setInterval(forceActive, 250);

let g_pair = null;
let g_freeze = false;
function writeReq(req, target) {
  const f = new File(req, 'w');
  f.write(target + '\n');
  f.close();
}

// Freeze game logic while a pair is armed: force the frame-time source
// FUN_00493390 (0x00493390) to report 0 elapsed units -> the tick quantizer
// emits 0 ticks -> logic static, render continues (0-tick frames are native
// behavior under the decouple). Scene motion between the pair frames drops to
// zero, so the diff isolates the lifted element.
Interceptor.attach(ptr('0x00493390'), {
  onLeave(ret) { if (g_freeze) ret.replace(ptr(0)); }
});

Interceptor.attach(REND, {
  onEnter(args) {
    g_saved = [];
    if (g_pair) {
      if (g_pair.state < 0) {          // settle frames under freeze
        g_pair.state++;
      } else if (g_pair.state === 0) {
        for (const a of g_pair.addrs) {
          try {
            const v = rf(a);
            g_saved.push([a, v]);
            a.writeFloat(v + g_pair.dy);
          } catch (e) {}
        }
        try { writeReq(g_pair.req, g_pair.a); } catch (e) {}
        g_pair.state = 1;
      } else if (g_pair.state === 1) {
        try { writeReq(g_pair.req, g_pair.b); } catch (e) {}
        g_pair.state = 2;
      } else if (g_pair.state === 2) {
        try { if (g_pair.c) writeReq(g_pair.req, g_pair.c); } catch (e) {}
        g_pair = null;
        g_freeze = false;
      }
      return;
    }
    if (!g_lift) return;
    for (const a of g_lift.addrs) {
      try {
        const v = rf(a);
        g_saved.push([a, v]);
        a.writeFloat(v + g_lift.dy);
      } catch (e) {}
    }
  },
  onLeave(ret) {
    for (const sv of g_saved) { try { sv[0].writeFloat(sv[1]); } catch (e) {} }
    g_saved = [];
  }
});
"""


def diff_report(a_path, b_path):
    """Changed-pixel bbox + count between two same-size BMPs (PIL)."""
    try:
        from PIL import Image, ImageChops
        a = Image.open(a_path).convert("RGB")
        b = Image.open(b_path).convert("RGB")
        d = ImageChops.difference(a, b).convert("L")
        d = d.point(lambda v: 255 if v > 24 else 0)
        bbox = d.getbbox()
        n = sum(1 for v in d.getdata() if v)
        return {"bbox": bbox, "px": n}
    except Exception as e:
        return {"error": str(e)}


def dump_bmp(req_path, out_bmp, timeout=6.0):
    """Write the request file and wait for the shim to produce the BMP."""
    if os.path.exists(out_bmp):
        os.remove(out_bmp)
    with open(req_path, "w") as f:
        f.write(os.path.abspath(out_bmp) + "\n")
    t0 = time.time()
    while time.time() - t0 < timeout:
        if os.path.exists(out_bmp) and os.path.getsize(out_bmp) > 54:
            time.sleep(0.2)   # let the write finish
            return True
        time.sleep(0.1)
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True,
                    help="EXPLICIT pid of the probe-owned MASHED instance")
    ap.add_argument("--req", required=True,
                    help="request-file path (must equal the spawn's MASHED_ORIG_BBDUMP_REQ)")
    ap.add_argument("--out", default="verify\\carpos_probe")
    ap.add_argument("--dy", type=float, default=80.0)
    ap.add_argument("--settle", type=float, default=0.6)
    args = ap.parse_args()

    os.makedirs(args.out, exist_ok=True)
    sess = frida.attach(args.pid)
    script = sess.create_script(AGENT)
    script.load()
    rpc = script.exports_sync

    st = rpc.status()
    print(f"phase={st['phase']} cars={st['cars']}", flush=True)
    if st["phase"] != 3:
        print("NOT in race phase 3 — aborting (no live car records)", flush=True)
        return 2

    print("actinfo:", rpc.actinfo(), flush=True)

    rec = rpc.scanrec()
    print(f"record triplet hits (car0 pos {rec['pos']}): {rec['hits']}", flush=True)
    va = rpc.scanva(0x0063d000, 0x00640000)
    va2 = rpc.scanva(0x00770000, 0x007f2000)
    va3 = rpc.scanva(0x00890000, 0x008b0000)
    va["hits"] = va["hits"] + va2["hits"] + va3["hits"]
    print(f"fixed-global hits: {va['hits']}", flush=True)

    def probe_one(tag, rec_off, vas, dy=None):
        """Consecutive-frame lift/clean dump pair + diff bbox."""
        t0 = time.time()
        while rpc.status()["phase"] != 3 and time.time() - t0 < 90:
            time.sleep(1.0)
        if rpc.status()["phase"] != 3:
            print(f"{tag}: SKIP (phase != 3 for 90s)", flush=True)
            return
        on_p  = os.path.abspath(os.path.join(args.out, f"{tag}_on.bmp"))
        off_p = os.path.abspath(os.path.join(args.out, f"{tag}_off.bmp"))
        c_p   = os.path.abspath(os.path.join(args.out, f"{tag}_off2.bmp"))
        for attempt in range(3):
            for p in (on_p, off_p, c_p):
                if os.path.exists(p):
                    os.remove(p)
            n = rpc.pair(rec_off, vas, args.dy if dy is None else dy,
                         os.path.abspath(args.req), on_p, off_p, c_p)
            t0 = time.time()
            while time.time() - t0 < 6.0:
                if all(os.path.exists(p) and os.path.getsize(p) > 54
                       for p in (on_p, off_p, c_p)):
                    break
                time.sleep(0.1)
            time.sleep(0.2)
            if not all(os.path.exists(p) for p in (on_p, off_p, c_p)):
                print(f"{tag}: dump fail (attempt {attempt})", flush=True)
                continue
            noise = diff_report(off_p, c_p)
            sig   = diff_report(on_p, off_p)
            if "px" in noise and noise["px"] > 3000:
                print(f"{tag}: noisy scene (noise={noise['px']}), retrying", flush=True)
                time.sleep(2.0)
                continue
            verdict = ""
            if "px" in sig and "px" in noise:
                verdict = "RESPONDER" if sig["px"] > 3 * noise["px"] + 500 else "quiet"
            print(f"{tag}: lifted={n} signal={sig} noise={noise.get('px')} {verdict}",
                  flush=True)
            break

    probe_one("control_dy0", 0x958, [], dy=0.0)   # noise floor under freeze

    for off_s in rec["hits"]:
        probe_one(f"rec_{off_s}", int(off_s, 16), [])

    for va_s in va["hits"]:
        probe_one(f"va_{va_s}", None, [int(va_s, 16)])

    script.unload()
    sess.detach()
    print("DONE", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
