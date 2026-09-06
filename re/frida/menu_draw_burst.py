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
// Current-screen id global (FUN_0042b930 getter / FUN_0042b940 setter). The
// frame dispatcher FUN_00492e90 draws the TITLE layer (logo + press-button,
// FUN_00403050) iff this reads 0x21 — the title is nav screen 33. A bare
// FUN_0043d2a0 push does NOT update it, which left the title layer
// composited over every synthetically-pushed screen (polluted baselines,
// 2026-06-12). Writing it after the push is what the real screen-change
// writers (0x0043f386/0x0043f5b0/0x0043f7e7) do.
const RVA_CURSCREEN=0x0067ecb0;
// Modal request block. FUN_0042bf30 is the game's own POSTER (C3): it fills
// 0x0067eab4..0x0067ead0 and raises the flag. It sets the alpha ramp to 0, and
// FUN_0043d7c0 is what drives that ramp up per frame -- the renderer
// FUN_00433f40 early-outs until it reaches 0x28. So a caller posts, then polls
// ramp() rather than poking 0x0067eab8, which would be seeding a derived field.
const RVA_POST=0x0042bf30, RVA_MODAL_FLAG=0x0067eab0, RVA_MODAL_RAMP=0x0067eab8;
const RVA_MODAL_AUX=0x0067ea5c, RVA_MODAL_BIASON=0x0067ead4;
let nav=null, postFn=null, collecting=0, frames={}, texts={}, order=[], cur=null;
let drawHooked=0, textHooked=0;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    postFn=new NativeFunction(abs(RVA_POST),'void',
      ['uint32','uint32','uint32','uint32','uint32','uint32']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); abs(RVA_CURSCREEN).writeS32(scr); return abs(RVA_DEPTH).readS32(); },
  // 0x0067ea5c is the POSTER's own gate: FUN_0042bf30 raises DAT_0067ead4 (the
  // id-bias enable) iff it finds this non-zero, and clears it. Seeding it is
  // seeding the producer's input; writing DAT_0067ead4 directly would be poking
  // the derived field, which is the failure Finding 31 recorded.
  post:function(a,b,c,d,e,f,aux){
    if(aux) abs(RVA_MODAL_AUX).writeS32(aux);
    postFn(a,b,c,d,e,f);
    return abs(RVA_MODAL_FLAG).readS32();
  },
  bias:function(){ return abs(RVA_MODAL_BIASON).readS32(); },
  ramp:function(){ return abs(RVA_MODAL_RAMP).readS32(); },
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
  // Glyph draws do NOT reach the RwIm2DRenderPrimitive stream with the modal
  // renderer anywhere in the first five backtrace frames, so the vertex channel
  // cannot see a title/body/prompt regression at all. This records the ARGUMENTS
  // of the four text entry points instead. Interceptor is gated on `collecting`,
  // exactly like the draw hook, so the cost is bounded to the burst window --
  // 0x00427e00 fires for every label on a menu screen and is not something to
  // leave attached (see CLAUDE.md, Frida overhead on hot paths).
  texttrace:function(){
    if(textHooked) return 1;
    [[0x00427e00,'427e00',6],[0x004278d0,'4278d0',3],
     [0x00427be0,'427be0',3],[0x00427990,'427990',8]].forEach(function(p){
      Interceptor.attach(abs(p[0]), { onEnter(){
        if(collecting===0 || cur===null) return;
        const sp=this.context.esp; const w=[];
        for(let i=1;i<=p[2];i++){ w.push(sp.add(4*i).readU32()); }
        if(!texts[cur]) texts[cur]=[];
        texts[cur].push({f:p[1], a:w});
      }});
    });
    textHooked=1; return 1;
  },
  done:function(){ return (collecting===0 && order.length>0) ? order.length : 0; },
  report:function(){
    const out={}; order.forEach(function(l){ out[l]=frames[l]; });
    return JSON.stringify(out);
  },
  textreport:function(){
    const out={}; order.forEach(function(l){ out[l]=texts[l]||[]; });
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
    ap.add_argument("--text-trace", action="store_true",
                    help="also record the arguments of the four text entry points "
                         "(0x00427e00/0x004278d0/0x00427be0/0x00427990) during the "
                         "burst; written alongside --out as *.text.json")
    ap.add_argument("--modal-aux", type=lambda s: int(s, 0), default=0,
                    help="seed DAT_0067ea5c before posting, so the poster raises "
                         "the id-bias enable DAT_0067ead4 (title id 0x41 -> 0xb8, "
                         "body id -> DAT_0067eadc + id)")
    ap.add_argument("--post-modal", default=None, metavar="P1,..,P6",
                    help="post a modal through the game's own FUN_0042bf30 before "
                         "arming, then wait for the alpha ramp to pass 0x28. Six "
                         "ints; the rejection arms at 0x0043f3e2.. pass e.g. "
                         "'0xd6,0,1,0x2d,0,0'")
    args = ap.parse_args()
    label = args.label or (f"scr{args.screen}" if args.screen is not None
                           else "title")

    env = dict(os.environ)
    # Stock behaviour by default. A caller that wants the .asi's hooks LIVE
    # (hook-on vs hook-off draw-stream A/B for a ported renderer) exports
    # MASHED_RE_NO_AUTO_HOOK=0 itself, optionally with MASHED_HOOK_ONLY to
    # limit the installed set to the RVAs under test.
    env.setdefault("MASHED_RE_NO_AUTO_HOOK", "1")
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

    if args.post_modal:
        p = [int(t, 0) for t in args.post_modal.split(",")]
        if len(p) != 6:
            raise SystemExit(f"--post-modal needs 6 ints, got {len(p)}")
        print("post modal", [hex(v) for v in p], "-> flag", E.post(*p, args.modal_aux))
        if args.modal_aux:
            print(f"  id-bias enable DAT_0067ead4 = {E.bias()}")
        # Wait for SATURATION, not merely for the 0x28 gate. The ramp is the top
        # byte of all five modal colours, so sampling it mid-climb makes every
        # colour a function of when the burst armed and buries a real diff under
        # run-to-run noise. FUN_0043d7c0 pins it at 0xff (0x0043d81e).
        end = time.time() + 15
        prev = -1
        while time.time() < end:
            cur_r = E.ramp()
            if cur_r >= 0xff or cur_r == prev:
                break
            prev = cur_r
            time.sleep(0.4)
        print(f"  alpha ramp = 0x{E.ramp():02x} (renderer gates at 0x28, saturates at 0xff)")

    if args.text_trace:
        E.texttrace()

    E.armburst(label, args.frames)
    end = time.time() + 15
    while time.time() < end and E.done() == 0:
        time.sleep(0.2)
    data = json.loads(E.report())

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(data, indent=1))
    print("->", out)
    if args.text_trace:
        tout = out.with_suffix(".text.json")
        tdata = json.loads(E.textreport())
        tout.write_text(json.dumps(tdata, indent=1))
        print("->", tout, sum(len(v) for v in tdata.values()), "text calls")
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
