# menu_draw_burst.py — capture K CONSECUTIVE frontend frames of the ORIGINAL's
# Im2D draw stream (raw vertex blobs + retaddr chains), for the parity harness.
#
# Standing capture tool (parity harness, re/analysis/parity_tooling.md) — the
# multi-frame sibling of menu_draw_dump.py. Differences from that tool:
#   * captures a BURST of consecutive frames (animation phase becomes a
#     comparable per-frame number, not an eyeball judgement);
#   * stores the RAW vertex blob (hex of count*0x1c bytes) per draw, the same
#     "v" schema the standalone's MASHED_DBG_DRAWSTREAM dump emits, so
#     re/tools/drawlist_diff.py decodes both sides with one decoder (incl. UVs);
#   * reads the draw call's actual args — verts ptr args[1], count args[2]
#     (RwIm2DRenderPrimitive(prim, verts, num); the prim arg and the count are
#     both 4 for the quad pipes) — instead of the fixed scratch 0x00898a20, so
#     non-scratch pipes (text) dump real geometry instead of stale bytes.
#
# Frame delimiting matches menu_draw_dump.py's armframe: one frontend frame =
# the span between two consecutive entries to ShellA FUN_0042e3a0.
#
# Hot-path note (CLAUDE.md): the device-draw Interceptor stays attached but
# only does work while `collecting` — the same pattern menu_draw_dump.py uses
# at the menu without destabilizing. Keep bursts short (default 4 frames) and
# the total session under ~60 s.
#
# Usage:
#   py -3.12 re/frida/menu_draw_burst.py [--screen N] [--frames K]
#       [--label NAME] [--out log/menu_draw_burst.json] [--settle SECS]
#
#   --screen N   push nav screen N via FUN_0043d2a0(N, 0) before capturing
#                (omit = capture at whatever screen is up, i.e. the title).
#   --frames K   consecutive frames to capture (default 4).
#
# Output: { "<label>_f<i>": [ {"v": "<hex>", "r": ["0x..rva", ...]}, ... ] }
import argparse
import json
import os
import struct
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
# Worktree-friendly: original/ lives only in the main checkout; override with
# MASHED_ROOT when running this script from a worktree copy.
if not (ROOT / "original" / "MASHED.exe").exists() and os.environ.get("MASHED_ROOT"):
    ROOT = Path(os.environ["MASHED_ROOT"])
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_SHELLA=0x0042e3a0, RVA_PHASE=0x0067eca4;
const VTBL=0x007d3ff8, VBUF=0x00898a20, STRIDE=0x1c;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8;
let nav=null, collecting=0, frames={}, order=[], cur=null, drawHooked=0;
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
  armburst:function(label,k){
    if(!drawHooked){
      const vt=abs(VTBL).readU32();
      const drawFn=ptr(vt).add(0x30).readU32();
      Interceptor.attach(ptr(drawFn), { onEnter(args){
        if(collecting===0 || cur===null) return;
        // RwIm2DRenderPrimitive(prim, verts, num): real verts ptr + count.
        let verts=args[1]; let n=args[2].toInt32();
        if(n<1||n>64){ n=4; }
        if(verts.isNull()){ verts=abs(VBUF); }
        let h='';
        try{
          const u=new Uint8Array(verts.readByteArray(n*STRIDE));
          for(let i=0;i<u.length;i++){ h+=('0'+u[i].toString(16)).slice(-2); }
        }catch(e){ return; }
        let rets=[];
        try{
          rets=Thread.backtrace(this.context, Backtracer.FUZZY).slice(0,5)
               .map(function(a){ return a.sub(DELTA).toString(); });
        }catch(e){}
        frames[cur].push({v:h, r:rets});
      }});
      drawHooked=1;
    }
    let seen=0;
    const h=Interceptor.attach(abs(RVA_SHELLA), { onEnter(args){
      seen++;
      if(seen<=k){
        cur=label+'_f'+(seen-1);
        frames[cur]=[]; order.push(cur);
        collecting=1;
      } else {
        collecting=0; cur=null; h.detach();
      }
    }});
    return 1;
  },
  done:function(){ return (collecting===0 && order.length>0) ? order.length : 0; },
  report:function(){
    const out={}; order.forEach(function(l){ out[l]=frames[l]; });
    return JSON.stringify(out);
  }
};
'''


def preview(rec):
    raw = bytes.fromhex(rec["v"])
    n = len(raw) // 0x1c
    xs, ys, col = [], [], 0
    for i in range(n):
        x, y, _z, _w, c = struct.unpack_from("<ffffI", raw, i * 0x1c)
        xs.append(x); ys.append(y)
        if i == 0:
            col = c
    return (f"x={min(xs):7.2f} y={min(ys):7.2f} w={max(xs)-min(xs):7.2f} "
            f"h={max(ys)-min(ys):6.2f} col={col:08x} verts={n}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--screen", type=int, default=None)
    ap.add_argument("--frames", type=int, default=4)
    ap.add_argument("--label", default=None)
    ap.add_argument("--out", default=str(ROOT / "log" / "menu_draw_burst.json"))
    ap.add_argument("--settle", type=float, default=2.5,
                    help="seconds to let the slide anim settle after a push")
    args = ap.parse_args()
    label = args.label or (f"scr{args.screen}" if args.screen is not None
                           else "title")

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT)
    scr.on("message", lambda m, d: None)
    scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)

    if args.screen is not None:
        print(f"push {args.screen} -> depth", E.push(args.screen))
        time.sleep(args.settle)

    E.armburst(label, args.frames)
    end = time.time() + 15
    while time.time() < end and E.done() == 0:
        time.sleep(0.2)
    data = json.loads(E.report())

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(data, indent=1))
    print("->", out)
    for lbl in data:
        rows = data[lbl]
        print(f"--- {lbl}: {len(rows)} draws ---")
        for r in rows[:10]:
            print("  " + preview(r))
    try:
        dev.kill(pid)
    except Exception:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
