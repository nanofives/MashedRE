"""Read-only measure probe: what FUN_004282a0(id, scale) returns on the original at screen 32 (autosave toggle).

scr32 variant of ../arrow_fix/measure_probe.py (2026-10-13). Adds: the label call FUN_00431240 makes at
0x0043130b (entry hook on 0x004282a0 filtered on return address 0x00431310, detached after one hit; reads
the pushed id/scale, ESI = row*0x34 and the record float [ESI+0x898aa4] that 0x00431310 fadds), and the
live [0x008a95ac] that FUN_0040ad20 returns (the On/Off selector, 0x004313f5).

Spawns original\\MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1, stock code), waits for the menu, pushes nav
screen 32 the same way menu_draw_burst.py does, then on the GAME thread (inside one onEnter of the
frame shell 0x0042e3a0, detached after one call) calls the original's own FUN_004282a0 for each id
and records the ST0 return as a double. It also reads the inputs the function consumes, so the result
can be recomputed offline:
  [0x0067d830] float      fdiv operand        (0x004282f1)
  [0x0067d838] font ctx   FUN_005554d0 arg1  (0x004282dd)
  ctx+0x0c tracking, ctx+0x10 flags, ctx+0x24 ASCII LUT, ctx+0x124/0x128/0x12c ext table,
  [ctx+0x134] -> FUN_005c4d30 -> glyph table, record float at +0 (0x00555585)
  the message string: [0x0066d828 + id*4] + 0x0066d828 -> u16 count, u16 chars (FUN_00427780 /
  FUN_004277a0, see TextCtrlCodeRemap.cpp)
  The x87 control word is not read (a fnstcw stub AV'd inside Frida's call path); the precision
  the game thread ran at is inferred offline from the return bits instead.
No memory is written in the game's image or data. Kills only the PID it spawned.
"""
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
function abs(r){return ptr(r+DELTA);}
let nav=null, measure=null, cwfn=null, result=null, label=null;
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(0x0043d2a0),'void',['int','int']);
    measure=new NativeFunction(abs(0x004282a0),'double',['uint32','float']);
    return DELTA;
  },
  phase:function(){ return abs(0x0067eca4).readS32(); },
  push:function(scr){ nav(scr,0); abs(0x0067ecb0).writeS32(scr); return abs(0x0067e9f8).readS32(); },
  armlabel:function(){
    const want=abs(0x00431310);
    const h=Interceptor.attach(abs(0x004282a0), { onEnter(){
      if(!this.returnAddress.equals(want)) return;
      h.detach();
      const sp=this.context.esp; const esi=this.context.esi.toUInt32();
      label={id:sp.add(4).readU32(), scale_hex:sp.add(8).readU32().toString(16), scale:sp.add(8).readFloat(),
             esi:esi, row:esi/0x34, rec_x:abs(0x00898aa4+esi).readFloat(),
             rec_x_hex:abs(0x00898aa4+esi).readU32().toString(16), autosave:abs(0x008a95ac).readS32()};
    }});
    return 1;
  },
  label:function(){ return label===null ? '' : JSON.stringify(label); },
  arm:function(ids, scales){
    const h=Interceptor.attach(abs(0x0042e3a0), { onEnter(){
      h.detach();
      const out={autosave:abs(0x008a95ac).readS32(), d830:abs(0x0067d830).readFloat(),
                 d830_hex:abs(0x0067d830).readU32().toString(16), calls:[]};
      ids.forEach(function(id){ scales.forEach(function(s){
        out.calls.push({id:id, scale:s, ret:measure(id, s)});
      }); });
      if(label!==null){ out.label_ret=measure(label.id, label.scale); }
      result=out;
    }});
    return 1;
  },
  result:function(){ return result===null ? '' : JSON.stringify(result); },
  inputs:function(ids){
    const ctx=abs(0x0067d838).readPointer();
    const o={ctx:ctx.toString(), trk:ctx.add(0xc).readFloat(), trk_hex:ctx.add(0xc).readU32().toString(16),
             flags:ctx.add(0x10).readU8(), ext_base:ctx.add(0x124).readU32(), ext_count:ctx.add(0x128).readU32()};
    const holder=ctx.add(0x134).readPointer();
    const tbl=holder.add(4).readU32()!==0 ? holder.readPointer() : ptr(0);
    o.tbl=tbl.toString();
    o.strings=[];
    ids.forEach(function(id){
      const base=abs(0x0066d828);
      const sp=base.add(base.add(id*4).readU32());
      const n=sp.readS16(); const chars=[]; const glyphs=[];
      for(let i=0;i<n;i++){
        const c=sp.add(2+2*i).readU16(); chars.push(c);
        let gi=-1;
        if(c<0x80) gi=ctx.add(0x24+c*2).readS16();
        else { const k=c-o.ext_base; if(k>=0 && k<o.ext_count) gi=ctx.add(0x12c).readPointer().add(k*2).readS16(); }
        glyphs.push({c:c, gi:gi, w: gi>=0 ? tbl.add(gi*32).readFloat() : null,
                     w_hex: gi>=0 ? tbl.add(gi*32).readU32().toString(16) : null});
      }
      o.strings.push({id:id, n:n, text:String.fromCharCode.apply(null,chars), glyphs:glyphs});
    });
    return JSON.stringify(o);
  }
};
'''

def main():
    out = Path(sys.argv[1])
    # optional argv[2]: absolute .bmp path. The d3d9 shim's MASHED_ORIG_BBDUMP_REQ channel
    # dumps the settled screen-32 backbuffer there (read-only; no game memory written).
    bmp = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else None
    req = out.with_suffix(".bbreq").resolve()
    if req.exists(): req.unlink()
    ids = [0x59, 0x5a]
    scales = [0.7, 0.6, 0.8]
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if bmp: env["MASHED_ORIG_BBDUMP_REQ"] = str(req)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print("spawned PID", pid, flush=True)
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print(m)); scr.load()
        E = scr.exports_sync; E.init(); dev.resume(pid)
        end = time.time() + 25
        while time.time() < end and E.phase() != 3: time.sleep(0.2)
        time.sleep(1.5)
        print("push 32 -> depth", E.push(32), flush=True)
        time.sleep(6)
        E.armlabel()
        end = time.time() + 10; lab = ''
        while time.time() < end and not lab: time.sleep(0.2); lab = E.label()
        lab = json.loads(lab) if lab else None
        print("label", lab, flush=True)
        if bmp:
            if bmp.exists(): bmp.unlink()
            req.write_text(str(bmp) + chr(10))
            end = time.time() + 10
            while time.time() < end and req.exists(): time.sleep(0.1)
            print("bbdump ->", bmp, bmp.exists(), flush=True)
        if lab: ids = ids + [lab["id"]]
        E.arm(ids, scales)
        end = time.time() + 10; r = ''
        while time.time() < end and not r: time.sleep(0.2); r = E.result()
        res = json.loads(r) if r else None
        inp = json.loads(E.inputs(ids))
        data = {"pid": pid, "screen": 32, "label_call": lab, "game_thread": res, "inputs": inp}
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(data, indent=1))
        print(json.dumps(data, indent=1))
    finally:
        try: dev.kill(pid); print("killed PID", pid)
        except Exception as e: print("kill", pid, e)

if __name__ == "__main__":
    main()
