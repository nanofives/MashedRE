# cam_frame_writer_watch.py — U-9058. Find WHO writes the RwCamera frame matrix
# during a TRAINING race (Quick Battle).
#
# [2026-10-05] THE QUESTION IN THE LINE ABOVE IS ANSWERED. U-9058 was RESOLVED on
# 2026-08-30, the same day it was filed: the frame basis is written by
# Camera::InitWithMatrix (0x00442a20 @ 0x00442a4e), a 16-dword copy of a matrix
# composed at 0x0089650c by FUN_00445aa0 (already C2); Camera::Apply also writes
# frame+0x10 and its output is OVERWRITTEN. Read the U-9058 row before using this
# script — it is kept as a live instrument for the camera frame, not as an open
# investigation. Found by re/tools/stale_uncertain_refs.py; see
# verify/d4_save_20261005/RESULT_STALESWEEP2.md for why that sweep exists.
#
# Drives the ORIGINAL into a race (same
# nav recipe as race_draw_burst.py), resolves the live frame object
#   cam    = *(DAT_00897fe0 + 0x84)     (RwCamera*)
#   frame  = *(cam + 0x04)              (RwFrame*)
#   modelling matrix at frame+0x10, LTM at frame+0x50 (RW 3.x)
# then arms an x86 HARDWARE write-watchpoint (DR0) on one 4-byte slot and uses
# Process.setExceptionHandler to record the PC of every writer over a short
# window. Two passes on the SAME process: modelling right.x (frame+0x10) then
# LTM right.x (frame+0x50). Aggregates pc->count in-agent (no per-write message
# flood) and returns a rebased-RVA histogram + one backtrace per distinct pc.
#
# Watchpoint mechanism copied from probe_watchpoint.py (survives ~90s). Hot-path
# rule: matrix writes are a few per frame, windows are short (default 2.5s).
#
# --pads (U-9676, 2026-10-12): instead of the frame passes, watch each RwMatrix pad slot
# (+0x68/+0x78/+0x88, i.e. matrix +0x1c/+0x2c/+0x3c of the matrix at entry+0x4c) of every live
# camera entry (0x008964c0 + k*0xd8, k < [0x00898994]), one slot per pass on DR0. Hits are keyed
# by writer PC and by which 0x004464c0 callee is on the stack (return 0x004464e5 = 0x00445aa0,
# 0x004464f5 = 0x00441d40, 0x00446505 = 0x00442440); ESI/EDI/ECX and the dword at ESI-4 are kept
# so a rep movsd source can be resolved.
#
# Usage: py -3.12 re/frida/cam_frame_writer_watch.py [--settle 3] [--window 2.5] [--pads]
import argparse, json, os, shutil, subprocess, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
def find_original(root):
    c = root / "original" / "MASHED.exe"
    if c.exists(): return c
    for p in root.parents:
        c2 = p / "original" / "MASHED.exe"
        if c2.exists(): return c2
    return c
EXE = find_original(ROOT); ORIG = EXE.parent
NAV = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")
NPG, DET, ABOVE = 0x00000200, 0x00000008, 0x00008000

WATCH = r'''
'use strict';
const IMG = 0x00400000;
let DELTA = 0;
let counts = {};        // pc_rva -> count
let samples = {};       // pc_rva -> {bt, val, esp, ...}
let armedTids = [];
let seq = [];           // last hits in order: pc|callee
const SEQ_MAX = 12;
let handlerInstalled = false;
let WADDR = null;
const DISPATCH_RETS = {0x4464e5: '445aa0', 0x4464f5: '441d40', 0x446505: '442440'};
function callerOf(esp){
  try { for (let k = 0; k < 512; k++){
    let x = (esp.add(k*4).readU32() >>> 0) - DELTA;
    if (DISPATCH_RETS[x] !== undefined) return {callee: DISPATCH_RETS[x], off: k*4};
  } } catch(e){}
  return {callee: 'none', off: -1};
}
const LO = 0x401000, HI = 0x995000;

function rvaOf(p){ return '0x' + ((p.toUInt32() - DELTA) >>> 0).toString(16); }
function absHex(p){ return '0x' + (p.toUInt32() >>> 0).toString(16); }
function btOf(esp){
  let bt = [];
  try { for (let k = 0; k < 40 && bt.length < 12; k++){
    let x = esp.add(k*4).readU32() >>> 0;
    if (x >= LO && x < HI) bt.push('0x' + (x - DELTA).toString(16));
  } } catch(e){}
  return bt;
}

function installHandler(){
  if (handlerInstalled) return;
  handlerInstalled = true;
  Process.setExceptionHandler(function(d){
    if (d.type === 'breakpoint' || d.type === 'single-step'){
      let c = d.context;
      let key = rvaOf(c.pc);
      let cal = callerOf(c.esp);
      key = key + '|' + cal.callee;
      counts[key] = (counts[key] || 0) + 1;
      seq.push(key); if (seq.length > SEQ_MAX) seq.shift();
      if (!samples[key]){
        let src = '?';
        try { src = '0x' + (c.esi.sub(4).readU32() >>> 0).toString(16); } catch(e){}
        let v = '?';
        try { v = '0x' + (WADDR.readU32() >>> 0).toString(16); } catch(e){}
        samples[key] = { pc_abs: absHex(c.pc), val: v, esp: absHex(c.esp),
                         eax: absHex(c.eax), ecx: absHex(c.ecx), edx: absHex(c.edx),
                         esi: absHex(c.esi), edi: absHex(c.edi), ebx: absHex(c.ebx),
                         ebp: absHex(c.ebp), bt: btOf(c.esp),
                         esi_m4: src, caller_off: cal.off };
      }
      return true;
    }
    return false;  // let real faults through
  });
}

rpc.exports = {
  setup: function(){
    let m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMG;
    let out = { delta: DELTA, base: absHex(m.base) };
    try {
      let ctrl = ptr(0x00897fe0 + DELTA);
      out.ctrl_addr = absHex(ctrl);
      let cam = ctrl.add(0x84).readPointer();
      out.cam = absHex(cam);
      let fr = cam.add(0x04).readPointer();
      out.frame = absHex(fr);
      out.modelling_addr = absHex(fr.add(0x10));
      out.ltm_addr = absHex(fr.add(0x50));
      // record[0] matrix source (0x0089650c = &DAT_008964c0 + 0x4c), the FIXED
      // global that Camera::InitWithMatrix (0x00442a20) copies into frame+0x10.
      // Watching this finds WHO composes the rolled record matrix per frame.
      out.record0_matrix = absHex(ptr(0x0089650c + DELTA));
      let rd = function(base){ let g = o => base.add(o).readFloat();
        return { right:[g(0),g(4),g(8)], up:[g(0x10),g(0x14),g(0x18)],
                 at:[g(0x20),g(0x24),g(0x28)], pos:[g(0x30),g(0x34),g(0x38)] }; };
      out.mat_modelling = rd(fr.add(0x10));
      out.mat_ltm = rd(fr.add(0x50));
    } catch(e){ out.error = String(e); }
    return out;
  },
  entries: function(){
    let n = ptr(0x00898994 + DELTA).readS32(); let t = [];
    for (let k = 0; k < Math.max(0, Math.min(n, 16)); k++) t.push(ptr(0x008964c0 + DELTA + k*0xd8 + 4).readS32());
    // 0x004c4a50 multiplies through call [ecx+edx+8] with ecx=[0x007d4028], edx=[0x007d3ff8] (0x004c4cd8)
    let mul = '?';
    try { mul = absHex(ptr(ptr(0x007d3ff8 + DELTA).readU32() + ptr(0x007d4028 + DELTA).readU32() + 8).readPointer()); } catch(e){ mul = String(e); }
    return { count: n, types: t, mul_fn: mul };
  },
  arm: function(addr_str){
    installHandler();
    WADDR = ptr(addr_str);
    counts = {}; samples = {}; seq = [];
    armedTids = [];
    Process.enumerateThreads().forEach(function(t){
      try { t.setHardwareWatchpoint(0, WADDR, 4, 'w'); armedTids.push(t.id); }
      catch(e){ send({kind:'armerr', tid:t.id, err:e.message}); }
    });
    return { armed: armedTids.length, addr: absHex(WADDR) };
  },
  snapshot: function(){
    let rows = [];
    for (let k in counts) rows.push({ pc: k, count: counts[k], sample: samples[k] });
    rows.sort((a,b) => b.count - a.count);
    let cur = '?';
    try { cur = '0x' + (WADDR.readU32() >>> 0).toString(16); } catch(e){}
    return { rows: rows, cur_val: cur, seq: seq.slice() };
  },
  disarm: function(){
    Process.enumerateThreads().forEach(function(t){
      try { t.unsetHardwareWatchpoint(0); } catch(e){}
    });
    return 1;
  }
};
'''


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--settle", type=float, default=3.5)
    ap.add_argument("--window", type=float, default=2.5)
    ap.add_argument("--mode-sel", type=int, default=1)
    ap.add_argument("--pads", action="store_true", help="U-9676: watch every camera entry's pad slots")
    ap.add_argument("--pad-list", default=None, help="entry indices in pass order, e.g. 4,5,3; a final "
                    "e0+0x68 pass is appended as the liveness control")
    ap.add_argument("--out", default=str(ROOT / "verify" / "camframe" / "writers.json"))
    args = ap.parse_args()

    out = Path(args.out).resolve(); out.parent.mkdir(parents=True, exist_ok=True)

    if not EXE.exists(): sys.exit(f"original MASHED.exe not found at {EXE}")
    if not (ORIG / "d3d9.dll").exists(): sys.exit(f"d3d9 shim missing at {ORIG/'d3d9.dll'}")
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if not canon.exists(): canon = ORIG.parent / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists(): shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"

    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET | ABOVE)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None: sys.exit("attach failed")
    print(f"  pid={proc.pid}")

    scr = sess.create_script(NAV); scr.on("message", lambda m, d: None)
    scr.load(); E = scr.exports_sync; E.init()

    def wait(pred, t):
        end = time.time() + t
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)
    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0): return True
        return E.depth() >= target

    result = {"writers": {}}
    rc = 3
    try:
        print("  booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30):
            sys.exit("never reached title")
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(args.mode_sel); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() == 3: sys.exit(f"NOT in race (phase={E.phase()})")
        print(f"  in race (phase={E.phase()}); settling {args.settle}s")
        time.sleep(args.settle)

        w = sess.create_script(WATCH); w.on("message", lambda m, d: print("  AGENT:", m))
        w.load(); W = w.exports_sync
        info = W.setup()
        print("  setup:", json.dumps(info, indent=2))
        result["setup"] = info
        if info.get("error"):
            sys.exit(f"resolve failed: {info['error']}")

        passes = [(label, info[key]) for label, key in (("modelling+0x10", "modelling_addr"),
                  ("ltm+0x50", "ltm_addr"), ("record0_matrix@0x0089650c", "record0_matrix"))]
        if args.pads:
            ent = W.entries(); result["entries"] = ent
            print(f"  entries: {ent}")
            ks = ([int(x) for x in args.pad_list.split(",")] if args.pad_list else list(range(len(ent["types"]))))
            passes = [(f"e{k}+0x{o:x}", hex(0x008964c0 + info['delta'] + k * 0xd8 + o))
                      for k in ks for o in (0x68, 0x78, 0x88)]
            if args.pad_list:
                passes.append(("control e0+0x68", hex(0x008964c0 + info['delta'] + 0x68)))
        for label, addr in passes:
            a = W.arm(addr)
            print(f"  === PASS {label}: armed DR0 on {a['armed']} threads at {a['addr']} ===")
            time.sleep(args.window)
            snap = W.snapshot()
            W.disarm()
            print(f"  {label}: cur_val={snap['cur_val']} distinct_writers={len(snap['rows'])} seq={snap.get('seq')}")
            for r in snap["rows"]:
                s = r["sample"]
                print(f"    PC={r['pc']} count={r['count']} val={s['val']} bt={s['bt']}")
            result["writers"][label] = snap
            time.sleep(0.3)
        rc = 0
    finally:
        out.write_text(json.dumps(result, indent=2), encoding="utf-8")
        print("  wrote", out)
        try: scr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        try: dev.kill(proc.pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
