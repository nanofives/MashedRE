"""Read-only text-geometry probe: the original's own glyph vertices for one FUN_00427e00 call.

Usage: py -3.12 text_y_probe.py <screen 32|19> <out.json> [bbdump.bmp]

Spawns original\\MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1), waits for menu phase 3, pushes nav screen <screen>
(FUN_0043d2a0 + 0x0067ecb0, the menu_draw_burst.py method), settles 6 s, optionally dumps the backbuffer via
the d3d9 shim's MASHED_ORIG_BBDUMP_REQ channel, then arms two entry hooks for ONE value-text call:
  - 0x00427e00 (FUN_00427e00) filtered on return address 0x00431426 (screen 32, call at 0x00431421) or
    0x00431202 (screen 19, call at 0x004311fd). Records the pushed args (id, x, y, colour, scale, align).
  - 0x004cd070 (Im3D transform, cdecl (verts, count, ltm, flags)) while that call is live. Records every
    vertex (stride 0x24: x,y,z at +0/+4/+8, u,v at +0x1c/+0x20), the 16 floats at ltm (FUN_00552e40's
    &0x00912b98), and the camera at [0x00912c0c]: projection +0x14, viewMatrix +0x20 (16 floats), raster
    +0x60 -> width +0xc / height +0x10, viewWindow +0x68, viewOffset +0x78, near/far +0x80/+0x84.
  Also reads [0x0067d828..0x0067d834], [0x00912be0], [0x00912b08]/[0x00912b0c], the 16 floats at 0x00912b18.
Both hooks detach after the first complete call. Nothing is written into game memory. No key input.
Kills only the PID it spawned.
"""
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe"
RET = {32: 0x00431426, 19: 0x00431202}

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
function abs(r){return ptr(r+DELTA);}
function f16(p){ const o=[]; for(let i=0;i<16;i++) o.push(p.add(4*i).readFloat()); return o; }
function h16(p){ const o=[]; for(let i=0;i<16;i++) o.push(p.add(4*i).readU32().toString(16)); return o; }
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
  arm:function(ret){
    const want=abs(ret);
    let live=false, cur=null, done=false, h2=null;
    const h1=Interceptor.attach(abs(0x00427e00), {
      onEnter(){
        if(done || !this.returnAddress.equals(want)) return;
        const sp=this.context.esp;
        cur={args:{id:sp.add(4).readU32(), x:sp.add(8).readFloat(), x_hex:sp.add(8).readU32().toString(16),
                   y:sp.add(12).readFloat(), y_hex:sp.add(12).readU32().toString(16),
                   colour:sp.add(16).readU32().toString(16), scale:sp.add(20).readFloat(),
                   scale_hex:sp.add(20).readU32().toString(16), align:sp.add(24).readS32()},
             d828:[0,1,2,3].map(function(i){return abs(0x0067d828+4*i).readFloat();}),
             d828_hex:[0,1,2,3].map(function(i){return abs(0x0067d828+4*i).readU32().toString(16);}),
             be0:abs(0x00912be0).readFloat(), b08:abs(0x00912b08).readFloat(), b0c:abs(0x00912b0c).readFloat(),
             submits:[]};
        live=true; this.mine=true;
      },
      onLeave(){
        if(!this.mine) return;
        live=false; done=true; cur.b18=f16(abs(0x00912b18)); result=cur;
        h1.detach(); if(h2) h2.detach();
      }
    });
    h2=Interceptor.attach(abs(0x004cd070), { onEnter(){
      if(!live) return;
      const sp=this.context.esp;
      const vp=sp.add(4).readPointer(), n=sp.add(8).readU32(), ltm=sp.add(12).readPointer(), fl=sp.add(16).readU32();
      const verts=[];
      for(let i=0;i<n && i<400;i++){
        const b=vp.add(i*0x24);
        verts.push([b.readFloat(), b.add(4).readFloat(), b.add(8).readFloat(), b.add(0x1c).readFloat(), b.add(0x20).readFloat()]);
      }
      const cam=abs(0x00912c0c).readPointer();
      const ras=cam.add(0x60).readPointer();
      cur.submits.push({n:n, flags:fl, ltm:f16(ltm), ltm_hex:h16(ltm), verts:verts,
        cam:{proj:cam.add(0x14).readS32(), view:f16(cam.add(0x20)), view_hex:h16(cam.add(0x20)),
             vw:[cam.add(0x68).readFloat(), cam.add(0x6c).readFloat()],
             voff:[cam.add(0x78).readFloat(), cam.add(0x7c).readFloat()],
             near:cam.add(0x80).readFloat(), far:cam.add(0x84).readFloat(),
             ras_w:ras.add(0xc).readS32(), ras_h:ras.add(0x10).readS32()}});
    }});
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
        E.arm(RET[screen])
        end = time.time() + 10; r = ''
        while time.time() < end and not r: time.sleep(0.2); r = E.result()
        data = {"pid": pid, "screen": screen, "ret": hex(RET[screen]), "call": json.loads(r) if r else None}
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(data, indent=1))
        print("wrote", out, "submits", len(data["call"]["submits"]) if data["call"] else None)
    finally:
        try: dev.kill(pid); print("killed PID", pid)
        except Exception as e: print("kill", pid, e)

if __name__ == "__main__":
    main()
