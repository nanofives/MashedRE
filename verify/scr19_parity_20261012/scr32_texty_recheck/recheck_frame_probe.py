"""Recheck probe (2026-10-13): one frame of the original's text geometry, read-only.

Usage: py -3.12 recheck_frame_probe.py <screen> <out.json> [bbdump.bmp]

Spawns original\\MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1), waits for [0x0067eca4] == 3, pushes <screen>
with FUN_0043d2a0(screen, 0) + [0x0067ecb0] = screen (no key input), settles 6 s, optionally requests a
backbuffer dump through the d3d9 shim (MASHED_ORIG_BBDUMP_REQ), then records ONE frame between two entries
of the frame shell 0x0042e3a0:
  - entry of all four text entry points that call FUN_00427680 (callers found by an E8 scan of .text:
    0x00427e9c, 0x00427f90, 0x0042809c, 0x004281b1, i.e. FUN_00427e00 / 00427f00 / 00427ff0 / 00428140):
    fn, return address, the first six stack dwords (raw hex + float).
  - FUN_00427680 entry: its own x, y, align, scale ([esp+4..+0x10]), ESI (out ptr), [0x0067d828..834],
    font = [0x0067d838] and [font+8]; leave: the pen it wrote, [esi] and [esi+4] (0x00427745 / 0x00427771).
  - Im3D 0x004cd070 entry, kept only when the return address lies in FUN_00554940 (0x00554940..0x005551ca):
    the vertices (stride 0x24: xyz +0/+4/+8, uv +0x1c/+0x20), the 16 ltm floats, and the camera at
    [0x00912c0c] (type +0x14, view matrix +0x20, raster +0x60 -> w +0xc / h +0x10, raw +0x70..+0x88).
Each submit is tagged with the latest text call. All hooks detach at the second frame-shell entry.
Nothing is written into game memory except the screen push. Kills only the PID it spawned.
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
function hx(v){ return ('00000000'+(v>>>0).toString(16)).slice(-8); }
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
    let state=0; const calls=[]; const pens=[]; const subs=[]; let cam=null;
    const lo=abs(0x00554940), hi=abs(0x005551ca);
    const hs=[];
    [0x00427e00,0x00427f00,0x00427ff0,0x00428140].forEach(function(rva){
      hs.push(Interceptor.attach(abs(rva), { onEnter(){
        if(state!==1) return;
        const sp=this.context.esp; const raw=[], fl=[];
        for(let i=1;i<=6;i++){ raw.push(hx(sp.add(4*i).readU32())); fl.push(sp.add(4*i).readFloat()); }
        calls.push({fn:rva.toString(16), ret:'0x'+this.returnAddress.sub(DELTA).toString(16), raw:raw, f:fl, pen:null});
      }}));
    });
    hs.push(Interceptor.attach(abs(0x00427680), {
      onEnter(){
        if(state!==1) return;
        const sp=this.context.esp; this.esi=this.context.esi; this.ok=1;
        const font=abs(0x0067d838).readPointer();
        this.rec={call:calls.length-1, x:sp.add(4).readFloat(), y:sp.add(8).readFloat(), align:sp.add(12).readS32(),
          scale:sp.add(16).readFloat(),
          k:[hx(abs(0x0067d828).readU32()),hx(abs(0x0067d82c).readU32()),hx(abs(0x0067d830).readU32()),hx(abs(0x0067d834).readU32())],
          c8:font.isNull()?null:font.add(8).readFloat(), c8hex:font.isNull()?null:hx(font.add(8).readU32())};
      },
      onLeave(){
        if(!this.ok) return;
        this.rec.pen=[this.esi.readFloat(), this.esi.add(4).readFloat()];
        this.rec.penhex=[hx(this.esi.readU32()), hx(this.esi.add(4).readU32())];
        pens.push(this.rec);
        if(this.rec.call>=0) calls[this.rec.call].pen=pens.length-1;
      }}));
    hs.push(Interceptor.attach(abs(0x004cd070), { onEnter(){
      if(state!==1) return;
      const r=this.returnAddress; if(r.compare(lo)<0 || r.compare(hi)>0) return;
      const sp=this.context.esp;
      const vp=sp.add(4).readPointer(), n=sp.add(8).readU32(), ltm=sp.add(12).readPointer();
      const verts=[];
      for(let i=0;i<n && i<400;i++){ const b=vp.add(i*0x24);
        verts.push([b.readFloat(), b.add(4).readFloat(), b.add(8).readFloat(), b.add(0x1c).readFloat(), b.add(0x20).readFloat()]); }
      if(cam===null){ const c=abs(0x00912c0c).readPointer(); const ras=c.add(0x60).readPointer();
        const rawc=[]; for(let i=0x60;i<0x90;i+=4) rawc.push(c.add(i).readFloat());
        cam={proj:c.add(0x14).readS32(), view:f16(c.add(0x20)), ras_w:ras.add(0xc).readS32(), ras_h:ras.add(0x10).readS32(), raw60:rawc}; }
      subs.push({call:calls.length-1, ltm:f16(ltm), verts:verts});
    }}));
    hs.push(Interceptor.attach(abs(0x0042e3a0), { onEnter(){
      if(state===0){ state=1; return; }
      if(state===1){ state=2; result={calls:calls, pens:pens, submits:subs, cam:cam}; hs.forEach(function(h){h.detach();}); }
    }}));
    return 1;
  },
  result:function(){ return result===null ? '' : JSON.stringify(result); }
};
'''

def main():
    screen = int(sys.argv[1]); out = Path(sys.argv[2]).resolve()
    bmp = Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else None
    req = out.with_suffix(".bbreq")
    if req.exists(): req.unlink()
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if bmp: env["MASHED_ORIG_BBDUMP_REQ"] = str(req)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(EXE.parent), env=env)
    print("spawned PID", pid, flush=True)
    status = "ok"
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print(m)); scr.load()
        E = scr.exports_sync; E.init(); dev.resume(pid)
        end = time.time() + 30
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
        print("wrote", out, "calls", len(res["calls"]) if res else None, "pens", len(res["pens"]) if res else None,
              "submits", len(res["submits"]) if res else None, flush=True)
    except Exception as e:
        status = f"error {e!r}"; print(status, flush=True)
    finally:
        try: dev.kill(pid); print("killed PID", pid)
        except Exception as e: print("kill", pid, e)
    if status != "ok": sys.exit(2)

if __name__ == "__main__":
    main()
