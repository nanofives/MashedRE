# Nav-driven diff-original for FUN_00473ee0 (animated logo overlay) — the
# draw-SEQUENCE variant of the on-game-thread A/B (menu_anim_diff.py
# precedent, 2026-06-10): the overlay's observable output is its Im2D
# submissions, so a pre-installed Interceptor on the device draw fn
# (*(*(0x7d3ff8)+0x30)) records (vert-scratch bytes) per draw while a
# collector flag is set. Per sample, inside the replaced 0x00473ee0 on the
# game thread:
#   1. sync the port's fade pair to the original's globals
#      (DAT_0086ecc8/DAT_0086eccc) via LogoOverlayFadeSet
#   2. snapshot fade globals; collect A = original(args...)
#   3. restore fade globals;  collect B = twin(args...)
#   4. GREEN iff sequence lengths + all vert bytes match
#
# Usage: py -3.12 re/frida/logo_overlay_diff.py
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
ASI = ORIG / "mashed_re_dev.asi"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_LOGO=0x00473ee0, RVA_PHASE=0x0067eca4;
const VTBL=0x007d3ff8, VBUF=0x00898a20, VLEN=4*0x1c;
const FADE_T=0x0086ecc8, FADE_C=0x0086eccc;
let twin=null, fadeSet=null, results=[], want=0, origPtr=null;
let collecting=0, seqA=[], seqB=[];
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(asiPath){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    const asi=Module.load(asiPath);
    twin=new NativeFunction(asi.findExportByName('LogoOverlayTwin'),'void',
        ['int','int','float','float','int','int','int','int','int','int']);
    fadeSet=new NativeFunction(asi.findExportByName('LogoOverlayFadeSet'),
        'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  arm:function(n){
    want=n; results=[];
    const vt=abs(VTBL).readU32();
    const drawFn=ptr(vt).add(0x30).readU32();
    // real collector: hex of the vert scratch at each draw
    Interceptor.attach(ptr(drawFn), { onEnter(args){
      if(collecting===0) return;
      const bytes=abs(VBUF).readByteArray(VLEN);
      const u=new Uint8Array(bytes);
      let h='';
      for(let i=0;i<VLEN;i++){ h+=('0'+u[i].toString(16)).slice(-2); }
      if(collecting===1) seqA.push(h); else seqB.push(h);
    }});
    origPtr=Interceptor.replaceFast(abs(RVA_LOGO), new NativeCallback(function(
        p1,p2,slide,p4,p5,bg,p7,logo,p9,alpha){
      const orig=new NativeFunction(origPtr,'void',
        ['int','int','float','float','int','int','int','int','int','int']);
      if(results.length>=want){ orig(p1,p2,slide,p4,p5,bg,p7,logo,p9,alpha); return; }
      const ft=abs(FADE_T).readS32(), fc=abs(FADE_C).readS32();
      fadeSet(ft, fc);
      seqA=[]; seqB=[];
      collecting=1;
      orig(p1,p2,slide,p4,p5,bg,p7,logo,p9,alpha);
      collecting=0;
      const ft2=abs(FADE_T).readS32(), fc2=abs(FADE_C).readS32();
      abs(FADE_T).writeS32(ft); abs(FADE_C).writeS32(fc);
      collecting=2;
      twin(p1,p2,slide,p4,p5,bg,p7,logo,p9,alpha);
      collecting=0;
      // leave the world as-if-original
      abs(FADE_T).writeS32(ft2); abs(FADE_C).writeS32(fc2);
      let ok=(seqA.length===seqB.length);
      let badi=-1;
      if(ok){ for(let i=0;i<seqA.length;i++){ if(seqA[i]!==seqB[i]){ ok=false; badi=i; break; } } }
      results.push({n:seqA.length, m:seqB.length, ok:ok, badi:badi,
                    a:badi>=0?seqA[badi]:null, b:badi>=0?seqB[badi]:null,
                    slide:slide});
    },'void',['int','int','float','float','int','int','int','int','int','int']));
    return 1;
  },
  done:function(){ return results.length; },
  report:function(){
    Interceptor.revert(abs(RVA_LOGO));
    return JSON.stringify(results);
  }
};
'''


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init(str(ASI))
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)
    print("arming 20 on-thread A/B samples (draw-sequence capture)...")
    E.arm(20)
    end = time.time() + 20
    while time.time() < end and E.done() < 20:
        time.sleep(0.3)
    rows = json.loads(E.report())
    n_green = sum(1 for r in rows if r["ok"])
    print(f"samples={len(rows)} GREEN={n_green}")
    for r in rows[:4]:
        print(" ", {k: r[k] for k in ("n", "m", "ok", "badi", "slide")})
    if rows and not rows[0]["ok"] and rows[0]["badi"] >= 0:
        print("  first mismatch A:", rows[0]["a"][:64])
        print("  first mismatch B:", rows[0]["b"][:64])
    out = ROOT / "log" / "diff_logo_overlay_navdriven.json"
    out.write_text(json.dumps({"samples": rows, "green": n_green}, indent=1))
    print("->", out)
    try: dev.kill(pid)
    except Exception: pass
    return 0 if rows and n_green == len(rows) else 1


if __name__ == "__main__":
    sys.exit(main())
