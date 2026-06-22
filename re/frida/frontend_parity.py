# Robust frontend parity harness.
#
# Drives BOTH the original MASHED.exe and the standalone mashed_re.exe through the
# SAME reachable, non-race frontend screen list and reports per-screen
# faithfulness (pixel diff + side-by-side composite). This is the comparison
# foundation for precise frontend porting: every screen is captured identically
# on both sides (the post-Present client backbuffer, 640x480, no chrome) so a
# divergence is a real rendering difference, not a capture artifact.
#
#   RE: launched with MASHED_PARITY=1 — it self-walks the nav SM over the screen
#       list (RunParityWalk) and dumps verify/parity/re_s<scr>.bmp for each, then
#       quits.
#   ORIGINAL: Frida drives FUN_0043d2a0 over the SAME list (+ writes the
#       current-screen global so the title layer doesn't composite over it) and
#       captures via the d3d9-shim on-demand req -> verify/parity/orig_s<scr>.bmp.
#
# Then per-screen: a normalized pixel diff (% of pixels differing beyond a
# tolerance) + a labeled side-by-side, and a verdict table. Screens over the
# threshold are flagged NON-FAITHFUL for follow-up.
#
# Usage: py -3.12 re/frida/frontend_parity.py [scr ...]   (default: full list)
import os, sys, time
from pathlib import Path
import frida
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
RE_EXE = ROOT / "mashedmod" / "build" / "mashed_re.exe"
PARITY = ROOT / "verify" / "parity"
REQ = ROOT / "log" / "parity_dump.req"
SCREENS = [1, 2, 3, 4, 6, 7, 8, 15, 16, 18, 19, 24, 29, 30, 31, 32, 33]

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
const RVA_CURSCREEN=0x0067ecb0;
// Frontend backdrop composers (all cdecl, plain RET — verified in Ghidra
// 2026-06-14): FUN_00473c20 video/logo quad, FUN_00474890 preview crossfade
// wash, FUN_00473ee0 arc-wash + checker "race flag" grid. Patching each entry
// to RET (0xc3) neutralizes the non-deterministic video/preview/arc backdrop so
// the original exposes ONLY the static chrome + content on black — matching the
// RE side's MASHED_PARITY chrome-on-black render for an apples-to-apples diff.
const RVA_BACKDROP=[0x00473c20,0x00474890,0x00473ee0];
let nav=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  nopBackdrop:function(){
    RVA_BACKDROP.forEach(function(r){
      Memory.patchCode(abs(r),1,function(code){ code.writeU8(0xc3); });
    });
    return RVA_BACKDROP.length;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  depth:function(){ return abs(RVA_DEPTH).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); }
};
'''


def run_original(screens):
    PARITY.mkdir(parents=True, exist_ok=True)
    REQ.parent.mkdir(parents=True, exist_ok=True)
    if REQ.exists():
        REQ.unlink()
    dev = frida.get_local_device()
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env["MASHED_ORIG_BBDUMP_REQ"] = str(REQ)
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    scr.exports_sync.nop_backdrop()      # chrome-on-black: kill video/preview/arc
    dev.resume(pid)
    E = scr.exports_sync
    end = time.time() + 30
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    base = E.depth()
    got = []
    for s in screens:
        for _ in range(8):
            if E.depth() <= base:
                break
            E.pop(); time.sleep(0.12)
        E.push(s); time.sleep(2.4)             # settle the slide-in (let the animation finish)
        out = PARITY / f"orig_s{s}.bmp"
        if out.exists():
            out.unlink()
        REQ.write_text(str(out) + "\n", encoding="utf-8")
        t = time.time() + 4
        while time.time() < t and not out.exists():
            time.sleep(0.1)
        got.append(s if out.exists() else -s)
        E.pop(); time.sleep(0.15)
    try: dev.kill(pid)
    except Exception: pass
    return got


def run_re(screens):
    import subprocess
    PARITY.mkdir(parents=True, exist_ok=True)
    for s in screens:
        f = PARITY / f"re_s{s}.bmp"
        if f.exists():
            f.unlink()
    env = dict(os.environ); env["MASHED_PARITY"] = "1"
    p = subprocess.Popen([str(RE_EXE)], cwd=str(ROOT), env=env)
    # the parity walk self-quits; give it time (≈ screens * 1.1s dwell + boot)
    end = time.time() + 65
    while time.time() < end and p.poll() is None:
        time.sleep(0.3)
    if p.poll() is None:
        p.kill()
    return [s if (PARITY / f"re_s{s}.bmp").exists() else -s for s in screens]


def diff_pct(a_path, b_path):
    # Chrome-focused metric: the menu's text + bar borders are DARK ink; the
    # frontend video backdrop is mostly brighter and differs frame-to-frame
    # between the two captures, so a raw pixel diff is dominated by the video.
    # Instead compare the "ink" maps (brightness < 90) — the chrome STRUCTURE
    # (where text/borders are) — and report the fraction of ink pixels that
    # disagree, normalized by the ink area. This barely sees the video.
    import numpy as np
    a = Image.open(a_path).convert("RGB")
    b = Image.open(b_path).convert("RGB")
    if a.size != b.size:
        b = b.resize(a.size, Image.LANCZOS)
    ai = np.asarray(a).astype(int).sum(axis=2) < 90
    bi = np.asarray(b).astype(int).sum(axis=2) < 90
    union = (ai | bi).sum()
    if union == 0:
        return 0.0
    return float((ai ^ bi).sum() * 100.0 / union)  # % of ink pixels disagreeing


def compare(screens, thresh=12.0):
    PARITY.joinpath("cmp").mkdir(parents=True, exist_ok=True)
    rows = []
    for s in screens:
        o = PARITY / f"orig_s{s}.bmp"; r = PARITY / f"re_s{s}.bmp"
        if not o.exists() or not r.exists():
            rows.append((s, None, "MISSING(%s%s)" % ("" if o.exists() else "o", "" if r.exists() else "r")))
            continue
        pct = diff_pct(o, r)
        oi = Image.open(o).convert("RGB"); ri = Image.open(r).convert("RGB")
        H = 360; oi = oi.resize((int(oi.width*H/oi.height), H)); ri = ri.resize((int(ri.width*H/ri.height), H))
        cv = Image.new("RGB", (oi.width + ri.width + 12, H + 24), (24, 24, 24))
        cv.paste(oi, (0, 24)); cv.paste(ri, (oi.width + 12, 24))
        d = ImageDraw.Draw(cv)
        verdict = "FAITHFUL" if pct <= thresh else "DIVERGENT"
        d.text((4, 6), f"s{s}  ORIGINAL | STANDALONE   diff={pct:.1f}%  {verdict}",
               fill=(0, 255, 120) if pct <= thresh else (255, 120, 120))
        cv.save(PARITY / "cmp" / f"s{s}.png")
        rows.append((s, pct, verdict))
    print("\n=== FRONTEND PARITY REPORT ===")
    for s, pct, v in rows:
        print(f"  s{s:<3} {('%.1f%%' % pct) if pct is not None else '   -  '}  {v}")
    bad = [s for s, pct, v in rows if v == "DIVERGENT"]
    print(f"\n{sum(1 for _,_,v in rows if v=='FAITHFUL')}/{len(rows)} faithful; "
          f"divergent: {bad}")
    print("composites -> verify/parity/cmp/")
    return rows


def main():
    screens = [int(a) for a in sys.argv[1:]] or SCREENS
    print("[1/3] RE parity walk...");   print("  re:", run_re(screens))
    print("[2/3] original parity walk..."); print("  orig:", run_original(screens))
    print("[3/3] compare...");           compare(screens)
    return 0


if __name__ == "__main__":
    sys.exit(main())
