"""Read-only whole-frame text-geometry probe on the original: every glyph quad of one frame, projected to pixels.

Usage: py -3.12 text_frame_probe.py <screen> <out.json> [bbdump.bmp]

Same spawn / push / settle as text_y_probe.py (screen pushed via FUN_0043d2a0 + 0x0067ecb0). Then one frame is
taken between two consecutive entries of the frame shell 0x0042e3a0. Inside it:
  - entry hooks on 0x00427e00 and 0x00428140 record each text call (args id, x, y, colour, scale, align) and
    its return address; submits that follow are tagged with the latest call.
  - entry hook on 0x004cd070 (Im3D transform, cdecl (verts, count, ltm, flags)), kept only when the return
    address lies inside FUN_00554940 (0x00554940..0x005551ca): verts (stride 0x24, xyz +0/+4/+8, uv
    +0x1c/+0x20), the 16 ltm floats, and the camera at [0x00912c0c] (view +0x20, raster +0x60 w/h).
  Pixels: p = local*ltm, p' = p*view, (p'.x/p'.z*W, p'.y/p'.z*H) (see orig_text_quads.py).
All hooks detach at the second frame-shell entry. Nothing is written into game memory. No key input.
Kills only the PID it spawned.
"""
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
function abs(r){return ptr(r+DELTA);}
function f16(p){ const o=[]; for(let i=0;i<16;i++) o.push(p.add(4*i).readFloat()); return o; }
let nav=null, result=null;
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(0x0043d2a0),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(0x0067eca4).readS32(); },
  push:function(scr){ nav(scr,0); abs(0x0067ecb0).writeS32(scr); return abs(0x0067e9f8).readS32(); },
  arm:function(){
    let state=0; const calls=[]; const subs=[]; let cam=null;
    const lo=abs(0x00554940), hi=abs(0x005551ca);
    const hs=[];
    function textHook(rva){
      return Interceptor.attach(abs(rva), { onEnter(){
        if(state!==1) return;
        const sp=this.context.esp;
        calls.push({fn:rva.toString(16), ret:this.returnAddress.sub(DELTA).toString(),
          id:sp.add(4).readU32(), x:sp.add(8).readFloat(), y:sp.add(12).readFloat(),
          colour:sp.add(16).readU32().toString(16), scale:sp.add(20).readFloat(), align:sp.add(24).readS32()});
      }});
    }
    hs.push(textHook(0x00427e00)); hs.push(textHook(0x00428140));
    hs.push(Interceptor.attach(abs(0x004cd070), { onEnter(){
      if(state!==1) return;
      const r=this.returnAddress; if(r.compare(lo)<0 || r.compare(hi)>0) return;
      const sp=this.context.esp;
      const vp=sp.add(4).readPointer(), n=sp.add(8).readU32(), ltm=sp.add(12).readPointer();
      const verts=[];
      for(let i=0;i<n && i<400;i++){ const b=vp.add(i*0x24);
        verts.push([b.readFloat(), b.add(4).readFloat(), b.add(8).readFloat(), b.add(0x1c).readFloat(), b.add(0x20).readFloat()]); }
      if(cam===null){ const c=abs(0x00912c0c).readPointer(); const ras=c.add(0x60).readPointer();
        cam={proj:c.add(0x14).readS32(), view:f16(c.add(0x20)), ras_w:ras.add(0xc).readS32(), ras_h:ras.add(0x10).readS32()}; }
      subs.push({call:calls.length-1, ltm:f16(ltm), verts:verts});
    }}));
    hs.push(Interceptor.attach(abs(0x0042e3a0), { onEnter(){
      if(state===0){ state=1; return; }
      if(state===1){ state=2; result={calls:calls, submits:subs, cam:cam}; hs.forEach(function(h){h.detach();}); }
    }}));
    return 1;
  },
  result:function(){ return result===null ? '' : JSON.stringify(result); }
};
'''

def main():
    screen = int(sys.argv[1]); out = Path(sys.argv[2])
    bmp = Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else None
    req = out.with_suffix(".bbreq").resolve()
    if req.exists(): req.unlink()
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if bmp: env["MASHED_ORIG_BBDUMP_REQ"] = str(req)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(EXE.parent), env=env)
    print("spawned PID", pid, flush=True)
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print(m)); scr.load()
        E = scr.exports_sync; E.init(); dev.resume(pid)
        end = time.time() + 25
        while time.time() < end and E.phase() != 3: time.sleep(0.2)
        time.sleep(1.5)
        print("push", screen, "-> depth", E.push(screen), flush=True)
        time.sleep(6)
        if bmp:
            if bmp.exists(): bmp.unlink()
            req.write_text(str(bmp) + chr(10))
            end = time.time() + 10
            while time.time() < end and req.exists(): time.sleep(0.1)
            print("bbdump ->", bmp, bmp.exists(), flush=True)
        E.arm()
        end = time.time() + 10; r = ''
        while time.time() < end and not r: time.sleep(0.2); r = E.result()
        res = json.loads(r) if r else None
        data = {"pid": pid, "screen": screen, "frame": res}
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(data))
        print("wrote", out, "calls", len(res["calls"]) if res else None, "submits", len(res["submits"]) if res else None)
    finally:
        try: dev.kill(pid); print("killed PID", pid)
        except Exception as e: print("kill", pid, e)

if __name__ == "__main__":
    main()
