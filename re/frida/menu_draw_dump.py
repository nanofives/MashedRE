# Dump the ORIGINAL menu draw-call stream (decoded Im2D quads: corner coords,
# color) for a set of screens — the deterministic geometry reference for the
# standalone's record-walk port (no window-capture pixel forensics).
#
# Same capture core as menu_drawloop_diff.py: Interceptor on the device draw
# fn (*(*(0x7d3ff8)+0x30)) reads the vert scratch 0x00898a20 (4 verts x 0x1c:
# x,y,z,rhw,argb,u,v) while a collecting flag is set inside a replaceFast host
# at the per-frame draw loop 0x0043c5b0. One sampled frame per screen.
#
# Usage: py -3.12 re/frida/menu_draw_dump.py
import json, os, struct, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_HOST=0x0043c5b0, RVA_PHASE=0x0067eca4;
const VTBL=0x007d3ff8, VBUF=0x00898a20, VLEN=4*0x1c;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8;
let nav=null, collecting=0, frames={}, pending=null, rawCalls=0, drawHooked=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  arm:function(label){
    if(!drawHooked){
      // device vtable exists only after D3D9 init -> hook at first arm
      const vt=abs(VTBL).readU32();
      const drawFn=ptr(vt).add(0x30).readU32();
      Interceptor.attach(ptr(drawFn), { onEnter(args){
        if(collecting===0) return;
        const b=abs(VBUF).readByteArray(VLEN);
        const u=new Uint8Array(b); let h='';
        for(let i=0;i<VLEN;i++){ h+=('0'+u[i].toString(16)).slice(-2); }
        // emitter identification: walk a few return addresses (module-relative)
        let rets=[];
        try{
          rets=Thread.backtrace(this.context, Backtracer.FUZZY).slice(0,5)
               .map(function(a){ return a.sub(DELTA).toString(); });
        }catch(e){}
        frames[pending].push({v:h, r:rets});
      }});
      drawHooked=1;
    }
    pending=label; frames[label]=[];
    const origPtr=Interceptor.replaceFast(abs(RVA_HOST), new NativeCallback(function(){
      rawCalls++;
      const orig=new NativeFunction(origPtr,'void',[]);
      if(frames[pending].length===0 && collecting===0){
        collecting=1; orig(); collecting=0;
        Interceptor.revert(abs(RVA_HOST));
        return;
      }
      orig();
    },'void',[]));
    return 1;
  },
  got:function(label){ return (frames[label]||[]).length; },
  report:function(){ return JSON.stringify(frames); }
};
'''


def decode(entry):
    hexstr = entry["v"] if isinstance(entry, dict) else entry
    rets = entry.get("r", []) if isinstance(entry, dict) else []
    raw = bytes.fromhex(hexstr)
    verts = []
    for i in range(4):
        x, y, z, rhw, argb = struct.unpack_from("<ffffI", raw, i * 0x1c)
        verts.append((round(x, 2), round(y, 2), argb))
    xs = [v[0] for v in verts]; ys = [v[1] for v in verts]
    return {"x": min(xs), "y": min(ys),
            "w": round(max(xs) - min(xs), 2), "h": round(max(ys) - min(ys), 2),
            "argb": f"{verts[0][2]:08x}",
            "corner_colors": [f"{v[2]:08x}" for v in verts],
            "rets": rets}


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)

    # Title first (no push): the press-button/logo/checker emitters live here.
    E.arm("title")
    end = time.time() + 10
    while time.time() < end and E.got("title") == 0:
        time.sleep(0.2)
    print(f"  title: {E.got('title')} draws")

    SCREENS = [1, 8, 19]
    for s in SCREENS:
        print(f"push {s} ->", E.push(s))
        time.sleep(2.5)                       # let slide anim fully settle
        E.arm(f"scr{s}")
        end = time.time() + 10
        while time.time() < end and E.got(f"scr{s}") == 0:
            time.sleep(0.2)
        print(f"  scr{s}: {E.got(f'scr{s}')} draws")
    data = json.loads(E.report())
    out = {}
    for label, hexes in data.items():
        out[label] = [decode(hx) for hx in hexes]
    path = ROOT / "log" / "menu_draw_dump.json"
    path.write_text(json.dumps(out, indent=1))
    print("->", path)
    for label, rows in out.items():
        print(f"--- {label}: {len(rows)} draws ---")
        for r in rows[:40]:
            print(f"  x={r['x']:7.2f} y={r['y']:7.2f} w={r['w']:7.2f} h={r['h']:6.2f} {r['argb']} corners={r['corner_colors']}")
    try: dev.kill(pid)
    except Exception: pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
