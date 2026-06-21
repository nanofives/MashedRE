# Guided-playthrough capture — ATTACH-AFTER-BOOT variant.
#
# Spawn-mode (record_session.py) can't be used post-2026-06-10: Frida-spawn perturbs
# MASHED's layout and it crashes before the menu (the broken-log wild write lands
# elsewhere; see [[project-boot-crash-static-init-not-heap]]). But DIRECT launch boots
# fine (scripts/patch_mashed_disable_log.py). So: YOU launch + play MASHED, and this
# attaches by PID and records passively.
#
# DirectInput8Create already fired by attach time, so we can't catch the device the
# spawn way. Instead we resolve IDirectInputDevice8::GetDeviceState (vtable slot 9) by
# scanning dinput8's read-only vtables for pointer-runs into its .text and hooking each
# candidate slot-9 read-only; only the keyboard poll (cbData==256) records. No OS input
# injection; never kills your game (detach only).
#
# Usage:
#   1) launch MASHED yourself (double-click original\MASHED.exe -> main menu)
#   2) py -3.12 re/frida/record_attach.py --pid <PID> --name ramp-airborne --mode physics --seconds 180
#      (find PID: the MASHED window title is "MASHED [D3D9] [Release]")
import os, sys, time, json, argparse, datetime
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

_here = Path(__file__).resolve().parent
sys.path.insert(0, str(_here))
from record_session import (write_artifacts, load_hooks_index, parse_cov_arg,
                            next_ordinal, sha256, MODE_COV, SCN_DIR,
                            MASHED_EXE, STANDALONE_EXE)

ATTACH_AGENT = r'''
'use strict';
let frameCount=0, t0=0, prevKey=null;
const EVENTS=[]; const COV=[]; const _toDetach=[];
let textLo=null, textHi=null;
function inText(p){ return textLo && p.compare(textLo)>=0 && p.compare(textHi)<0; }
function addText(mod){ mod.enumerateRanges('r-x').forEach(function(r){ const e=r.base.add(r.size);
  if(textLo===null||r.base.compare(textLo)<0)textLo=r.base; if(textHi===null||e.compare(textHi)>0)textHi=e; }); }

function onFrame(buf){
  frameCount++; if(t0===0) t0=Date.now();
  let down=[]; try{ for(let i=0;i<256;i++){ if(buf.add(i).readU8()&0x80) down.push(i); } }catch(e){}
  const s=down.join(','); if(s!==prevKey){ EVENTS.push({f:frameCount, t:Date.now()-t0, d:down}); prevKey=s; }
}
const seen={};
function hookGDS(addr){ const k=addr.toString(); if(seen[k]) return; seen[k]=1;
  Interceptor.attach(addr,{ onEnter(a){ this.cb=a[1].toInt32(); this.buf=a[2]; },
                            onLeave(){ if(this.cb===256) onFrame(this.buf); } }); }
function resolveGDS(){
  let armed=0;
  ['dinput8_real.DLL','DINPUT8.dll','dinput8.dll'].forEach(function(mn){
    const mod=Process.findModuleByName(mn); if(!mod) return; addText(mod);
    mod.enumerateRanges('r--').forEach(function(r){
      let a=r.base; const end=r.base.add(r.size).sub(4); let runStart=null, runLen=0;
      function flush(){ if(runStart!==null && runLen>=16){ try{ const g=runStart.add(9*4).readPointer();
        if(inText(g)){ hookGDS(g); armed++; } }catch(e){} } runStart=null; runLen=0; }
      while(a.compare(end)<0){
        let p=null; try{ p=a.readPointer(); }catch(e){}
        if(p && inText(p)){ if(runStart===null){runStart=a;runLen=0;} runLen++; } else flush();
        a=a.add(4);
      }
      flush();
    });
  });
  return armed;
}
const N_GDS = resolveGDS();

function armCoverage(modName, imgBase, rvas){
  const mod=Process.findModuleByName(modName); if(!mod){ send({kind:'err',msg:'cov: '+modName+' missing'}); return 0; }
  let n=0;
  rvas.forEach(function(rva){ try{ const addr=mod.base.add(rva-imgBase); const h={hit:false,rva:rva};
    h.L=Interceptor.attach(addr,{ onEnter(){ if(h.hit)return; h.hit=true; COV.push({rva:rva,f:frameCount,t:Date.now()-t0}); _toDetach.push(h.L); } }); n++;
  }catch(e){} });
  setInterval(function(){ if(_toDetach.length){ const d=_toDetach.splice(0); d.forEach(function(L){try{L.detach();}catch(e){}}); } },200);
  return n;
}
rpc.exports = { armcoverage:function(m,b,r){ return armCoverage(m,b,r); },
                dump:function(){ return {events:EVENTS, frames:frameCount, cov:COV, gds:N_GDS}; } };
send({kind:'ready', msg:'attach recorder armed; GetDeviceState vtable hooks='+N_GDS});
'''

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True, help="PID of the MASHED you launched")
    ap.add_argument("--name", required=True, help="scenario slug")
    ap.add_argument("--mode", choices=["physics", "generic"], default="generic")
    ap.add_argument("--cov", default="", help="candidate RVAs: comma list or @file (overrides --mode set)")
    ap.add_argument("--seconds", type=int, default=180)
    ap.add_argument("--binary", choices=["original", "standalone"], default="original")
    args = ap.parse_args()

    exe = MASHED_EXE if args.binary == "original" else STANDALONE_EXE
    img_base = 0x00400000 if args.binary == "original" else 0x00010000
    mod_name = exe.name
    cov = parse_cov_arg(args.cov) if args.cov else MODE_COV[args.mode]
    hooks_idx = load_hooks_index()
    ordinal = next_ordinal()
    out_dir = SCN_DIR / f"{ordinal:03d}-{args.name}"

    print(f"=== record_attach: {out_dir.name}  (pid={args.pid}) ===")
    if psutil and not psutil.pid_exists(args.pid):
        print(f"error: pid {args.pid} not running"); return 2

    dev = frida.get_local_device()
    try:
        sess = dev.attach(args.pid)
    except Exception as e:
        print(f"attach failed: {e}"); return 3

    meta = {
        "name": args.name, "ordinal": ordinal, "slug": out_dir.name, "mode": args.mode,
        "binary": args.binary, "exe": str(exe),
        "exe_sha256": sha256(exe) if exe.exists() else "",
        "image_base": "0x%08x" % img_base, "capture": "attach", "pid": args.pid,
        "date": datetime.datetime.now().isoformat(timespec="seconds"),
        "coverage_candidates": ["0x%08x" % r for r in cov],
    }

    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("ready", "err"): print("  [agent]", p.get("msg"))

    scr = sess.create_script(ATTACH_AGENT); scr.on("message", on_msg); scr.load()
    if cov:
        scr.exports_sync.armcoverage(mod_name, img_base, cov)
    print(f"\n  >>> PLAY NOW. Recording for up to {args.seconds}s. Flushing to {out_dir.name}/ every ~1s.")
    print("  >>> (this never closes your game; Ctrl-C or wait out the window to stop)\n")

    last = {"events": [], "frames": 0, "cov": [], "gds": 0}
    try:
        deadline = time.time() + args.seconds
        while time.time() < deadline:
            if psutil and not psutil.pid_exists(args.pid):
                print("  game process exited."); break
            try:
                last = scr.exports_sync.dump()
                write_artifacts(out_dir, last, meta, hooks_idx)
            except Exception:
                pass
            print(f"\r  gds-hooks={last.get('gds',0)}  frames={last['frames']}  input-events={len(last['events'])}"
                  f"  coverage={len(last['cov'])}/{len(cov)}   ", end="", flush=True)
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\n  stopped.")
    finally:
        try: last = scr.exports_sync.dump()
        except Exception: pass
        write_artifacts(out_dir, last, meta, hooks_idx)
        try: sess.detach()
        except Exception: pass

    print(f"\n\n=== captured {out_dir.name} ===")
    print(f"  GetDeviceState vtable-hooks={last.get('gds',0)}  frames={last['frames']}"
          f"  input-events={len(last['events'])}  coverage={len(last['cov'])}/{len(cov)}")
    if last.get("gds", 0) == 0:
        print("  WARNING: no GetDeviceState resolved — input trace will be empty (coverage still valid).")
    hit = sorted(c["rva"] for c in last["cov"])
    if cov:
        print("  coverage HIT : " + (", ".join("0x%08x" % r for r in hit) or "(none)"))
        print("  coverage MISS: " + (", ".join("0x%08x" % r for r in cov if r not in hit) or "(none)"))
    print(f"  artifacts -> {out_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
