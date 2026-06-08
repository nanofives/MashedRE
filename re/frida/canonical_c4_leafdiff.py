# Canonical-scenario behavioral DIFF for pure-leaf frontend getters -> the C4 gate
# (CONFIDENCE.md: "clean Frida CSV diff between original and modded behavior on a canonical
# scenario"). Unlike the navigate harness (install+survive+exercise = prerequisite only),
# this captures the leaf's OBSERVABLE BEHAVIOR in two runs and compares:
#   OFF run: original (MASHED_HOOK_ONLY=__none__). Interceptor.attach(rva); capture behavior.
#   ON  run: modded   (MASHED_HOOK_ONLY=<names>). Attach the .asi EXPORT (where the installed
#            inline-JMP tail-jumps to); capture OUR reimpl's behavior on the same scenario.
# Clean iff the captured behavior SETS are identical and non-empty -> C4 evidence.
#
# Both runs navigate to the same stable menu screen (FUN_00497310 override; default 4,4 ->
# Game Type Select, idle) so the read globals are in identical canonical state -> deterministic.
#
# Per-leaf capture "kind":
#   ret_eax   : pure getter returning a 32-bit value in EAX (e.g. AspectRatioGlobalGet:
#               MOV EAX,[0x771a18]; RET). Capture retval bits.
#   out2x2    : void(param_1*,param_2*) writing 2 dwords to each (IntroVideoDimGetter). Capture
#               the 4 written dwords from the output buffers in onLeave.
#   writeflag : naked write-leaf using implicit EAX ptr (MenuDimSet: [eax+3]=0x30; [0x8990e4]=1).
#               Capture (byte at eax+3, dword at 0x008990e4) after the call.
#
# Usage: py -3.12 re/frida/canonical_c4_leafdiff.py [--nav 4,4] [--settle 7000] [--seconds 20]
import json, os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
LOG  = ROOT / "log"

# leaves to diff (pure leaves only). rva -> {name (=.asi export), kind}
# kind ret_eax_src: getter returning [src] in EAX; the global may be run-varying (pointer/
# handle), so cross-run value compare is invalid -> instead verify, IN THE SAME RUN, that the
# return equals the source global it claims to read. Both orig and reimpl read [src] -> both
# record 'ret==[src]'. A reimpl that returns something else records MISMATCH.
LEAVES = {
    "0x00493fc0": {"name": "AspectRatioGlobalGet", "kind": "ret_eax_src", "src": 0x00771a18},
    "0x00493f80": {"name": "IntroVideoDimGetter",  "kind": "out2x2"},
    "0x0042aad0": {"name": "MenuDimSet",           "kind": "writeflag"},
    # AspectRatioSnapshot: DAT_00771a50 = AspectRatioGlobalGet(); DAT_00771a54 = DAT_00771a50.
    # Only callee (0x00493fc0) is now C4 -> no stub. Same-run check: d1==d2==[src] after call.
    "0x00494f30": {"name": "AspectRatioSnapshot",  "kind": "snap2src",
                   "src": 0x00771a18, "d1": 0x00771a50, "d2": 0x00771a54},
}

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;
let NAV = [], SETTLE = 7000, DWELL = 1500, t0 = Date.now(), lastSlot = -99;
let GATE = 0;                         // only capture after nav completes (canonical screen)
const SAMP = {};                      // key -> Set of behavior-signature strings
const LEAVES = {};                    // key -> {kind}
let MODE = 'off';

function abs(rva){ return ptr(rva + DELTA); }
function navControl(){ const tt=Date.now()-t0-SETTLE; if(tt<0||!NAV.length) return -1;
  const k=Math.floor(tt/DWELL); if(k>=NAV.length) return -1; return NAV[k]; }

function record(key, sig){
  if (Date.now() < GATE) return;      // pre-nav-complete -> ignore (not canonical screen)
  if (!SAMP[key]) SAMP[key] = {};
  SAMP[key][sig] = (SAMP[key][sig]||0) + 1;
}

function attachLeaf(key, where, L){
  const kind=L.kind, src=L.src, d1=L.d1, d2=L.d2;
  try {
    Interceptor.attach(where, {
      onEnter(a){
        const sp=this.context.esp;
        if (kind==='out2x2'){ this.p1=sp.add(4).readPointer(); this.p2=sp.add(8).readPointer(); }
        else if (kind==='writeflag'){ this.eax=this.context.eax; }
      },
      onLeave(ret){
        try {
          if (kind==='ret_eax'){
            record(key, 'eax=0x'+ret.toUInt32().toString(16));
          } else if (kind==='ret_eax_src'){
            const v=ret.toUInt32(); const g=abs(src).readU32();
            record(key, v===g ? 'ret==[src]' : 'MISMATCH ret=0x'+v.toString(16)+' src=0x'+g.toString(16));
          } else if (kind==='snap2src'){
            const a1=abs(d1).readU32(), a2=abs(d2).readU32(), s=abs(src).readU32();
            record(key, (a1===a2 && a2===s) ? 'd1==d2==[src]' : 'MISMATCH d1=0x'+a1.toString(16)+' d2=0x'+a2.toString(16)+' s=0x'+s.toString(16));
          } else if (kind==='out2x2'){
            let s='';
            if(!this.p1.isNull()) s+='p1['+this.p1.readU32().toString(16)+','+this.p1.add(4).readU32().toString(16)+']';
            if(!this.p2.isNull()) s+='p2['+this.p2.readU32().toString(16)+','+this.p2.add(4).readU32().toString(16)+']';
            record(key, s);
          } else if (kind==='writeflag'){
            const b=this.eax.add(3).readU8(); const f=abs(0x008990e4).readU32();
            record(key, 'byte=0x'+b.toString(16)+' flag=0x'+f.toString(16));
          }
        } catch(e){}
      }
    });
    send({kind:'info', msg:'attached '+key+' ('+kind+') @'+where});
  } catch(e){ send({kind:'err', msg:'attach '+key+' '+e}); }
}

rpc.exports = {
  setup:function(mode, leaves, nav, settle, dwell, gateAtSettle){
    MODE=mode; NAV=nav; SETTLE=settle; DWELL=dwell; t0=Date.now();
    // gateAtSettle: start capturing right after settle (DURING nav taps) so input-driven
    // branches (e.g. readiness=1 while a control is pending) are observed, not just idle.
    GATE = gateAtSettle ? (t0 + SETTLE) : (t0 + SETTLE + NAV.length*DWELL + 800);
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMGBASE;
    // nav driver
    try { Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){ const k=navControl(); if(k>=0&&this.p===0&&this.c===k){
        const slot=Math.floor((Date.now()-t0-SETTLE)/DWELL); if(slot===lastSlot) return;
        lastSlot=slot; ret.replace(ptr(0xff)); } }
    }); } catch(e){ send({kind:'err',msg:'navarm '+e}); }
    // attach leaves
    let asi=null;
    if (mode==='on'){
      for (const nm of ['mashed_re_dev.asi','MASHED_RE_DEV.ASI']){
        try{ asi=Process.findModuleByName(nm); if(asi) break; }catch(e){}
      }
      send({kind:'info', msg:'asi='+(asi?asi.name+'@'+asi.base:'NOT FOUND')});
    }
    for (const key in leaves){
      const L=leaves[key]; LEAVES[key]=L;
      if (mode==='off'){
        attachLeaf(key, abs(parseInt(key,16)), L);
      } else {
        // ON: attach the .asi export (installed inline-JMP tail-jumps here)
        let ex=null;
        if (asi){ try{ ex=asi.findExportByName(L.name); }catch(e){} }
        if (ex){ attachLeaf(key, ex, L); }
        else { send({kind:'err', msg:'no export '+L.name+' in .asi -> cannot capture ON'}); }
      }
    }
    return 1;
  },
  samples:function(){ return SAMP; },
  jmpbyte:function(rva){ try{ return abs(parseInt(rva,16)).readU8(); }catch(e){ return -1; } }
};
send({kind:'ready'});
'''

GATE_AT_SETTLE = False

def run(mode, nav, settle, dwell, seconds):
    env = dict(os.environ)
    if mode == "on":
        env["MASHED_RE_DEV"] = "1"; env["MASHED_HOOK_ONLY"] = ",".join(LEAVES.keys())
    else:
        env["MASHED_RE_DEV"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("   agent err:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"): print("   ", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.setup(mode, LEAVES, nav, settle, dwell, GATE_AT_SETTLE)
    dev.resume(pid)
    t = time.time() + seconds
    while time.time() < t:
        if psutil and not psutil.pid_exists(pid): print("   exited early"); break
        time.sleep(0.25)
    samples, jmp = {}, {}
    try: samples = scr.exports_sync.samples()
    except Exception: pass
    if mode == "on":
        for r in LEAVES:
            try: jmp[r] = scr.exports_sync.jmpbyte(r)
            except Exception: jmp[r] = -1
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"samples": samples, "jmp": jmp}

def main():
    global LEAVES, GATE_AT_SETTLE
    if "--readiness" in sys.argv:
        # exercise BOTH branches of the readiness checks by capturing the EAX return DURING
        # continuous down-taps (input pending -> 1-branch) AND idle gaps (0-branch).
        LEAVES = {
            "0x0042ae10": {"name": "MenuReadinessCheckA", "kind": "ret_eax"},
            "0x0042aeb0": {"name": "MenuReadinessCheckB", "kind": "ret_eax"},
        }
        GATE_AT_SETTLE = True
    nav = [int(x) for x in sys.argv[sys.argv.index("--nav")+1].split(",")] if "--nav" in sys.argv else [4,4]
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 7000
    dwell  = int(sys.argv[sys.argv.index("--dwell")+1])  if "--dwell"  in sys.argv else 1500
    seconds= int(sys.argv[sys.argv.index("--seconds")+1])if "--seconds"in sys.argv else 20
    LOG.mkdir(parents=True, exist_ok=True)
    print(f"  leaves={list(LEAVES.keys())} nav={nav}")

    print("\n=== OFF run (original behavior on canonical scenario) ===")
    off = run("off", nav, settle, dwell, seconds)
    for k in LEAVES: print(f"  {k} {LEAVES[k]['name']}: {off['samples'].get(k,{})}")

    print("\n=== ON run (modded behavior; .asi export) ===")
    on = run("on", nav, settle, dwell, seconds)
    for k in LEAVES: print(f"  {k} {LEAVES[k]['name']}: {on['samples'].get(k,{})}  jmp=0x{on['jmp'].get(k,-1):x}" if isinstance(on['jmp'].get(k),int) and on['jmp'].get(k)>=0 else f"  {k}: {on['samples'].get(k,{})}")

    print("\n=== CANONICAL-SCENARIO BEHAVIORAL DIFF VERDICT ===")
    out = {}
    for k in LEAVES:
        offset = set(off["samples"].get(k, {}).keys())
        onset  = set(on["samples"].get(k, {}).keys())
        installed = (on["jmp"].get(k) == 0xE9)
        clean = bool(offset) and bool(onset) and (offset == onset)
        verdict = "C4-CLEAN" if (clean and installed) else "HOLD"
        why = []
        if not offset: why.append("orig-not-observed")
        if not onset:  why.append("modded-not-observed")
        if offset and onset and offset != onset: why.append("BEHAVIOR-DIFF")
        if not installed: why.append("no-JMP")
        out[k] = {"name": LEAVES[k]["name"], "kind": LEAVES[k]["kind"],
                  "off": sorted(offset), "on": sorted(onset),
                  "installed": installed, "clean": clean, "verdict": verdict, "why": why}
        print(f"  {k} {LEAVES[k]['name']:<22} -> {verdict} {','.join(why)}")
        print(f"        orig={sorted(offset)}")
        print(f"        mod ={sorted(onset)}")
    (LOG/"c4_leafdiff_result.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\n  -> log/c4_leafdiff_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
