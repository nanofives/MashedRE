# Canonical-scenario SIDE-EFFECT diff for non-leaf frontend functions -> the C4 gate for
# dispatchers/drawers whose behavior is side effects (calls + buffer writes), not a return value.
#
# Two capture kinds, both compared OFF (original) vs ON (modded, .asi export) on the SAME
# navigated screen (Game Type Select, idle) so state is identical -> deterministic:
#   calltrace : capture the ORDERED sequence of (callee_rva, arg0..argN) the TARGET invokes
#               during one call (scoped by an in-target flag so shared callees elsewhere are
#               ignored). The reimpl, calling the same callee RVAs with the same args, yields an
#               identical sequence iff behaviorally faithful.
#   buffer    : capture N bytes of an output buffer the target writes, after the call.
#
# Clean iff the captured behavior SETS match and are non-empty -> C4 evidence. One TARGET's
# callees are scoped to that target only; multiple targets run with independent flags.
#
# Usage: py -3.12 re/frida/canonical_c4_sidediff.py [--nav 4,4] [--settle 7000] [--seconds 20]
import json, os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
LOG  = ROOT / "log"

# targets. rva -> {name(.asi export), kind, ...}
#   calltrace: callees = [[rva, nargs], ...]
#   buffer:    buf, size
TARGETS = {
    # MenuChromeShellA: on GTS it calls MenusBodyA(DAT_0067ebc0-15000) with an ANIMATED arg
    # (DAT_0067ebc0 += frame delta each frame). Cross-run that arg differs by phase -> mask it
    # by recording the same-run delta (arg0 - ([0x0067ebc0]-15000)); a faithful reimpl -> d=0
    # always. The other 3 draw calls have constant args -> recorded literally.
    "0x0042e3a0": {"name": "MenuChromeShellA", "kind": "calltrace",
                   "callees": [["0x00472f40",5],["0x004730b0",5],["0x00472c60",5],
                               ["0x0042d5a0",1,{"rel":0x0067ebc0,"sub":15000}]]},
    "0x0042aae0": {"name": "MenuIm2DQuad", "kind": "buffer", "buf": 0x0067ec30, "size": 112},
}

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;
let NAV = [], SETTLE = 7000, DWELL = 1500, t0 = Date.now(), lastSlot = -99;
let GATE = 0;
const SAMP = {};            // targetKey -> {sig: count}
const CUR = {};             // targetKey -> current in-flight call seq (array) or null
let MODE = 'off';

function abs(rva){ return ptr(rva + DELTA); }
function navControl(){ const tt=Date.now()-t0-SETTLE; if(tt<0||!NAV.length) return -1;
  const k=Math.floor(tt/DWELL); if(k>=NAV.length) return -1; return NAV[k]; }
function record(key, sig){ if(Date.now()<GATE) return; if(!SAMP[key]) SAMP[key]={};
  SAMP[key][sig]=(SAMP[key][sig]||0)+1; }

function attachTargetCalltrace(key, where){
  Interceptor.attach(where, {
    onEnter(a){ CUR[key] = []; },
    onLeave(r){ if(CUR[key]!==null && CUR[key]!==undefined){ record(key, CUR[key].join(' ')); } CUR[key]=null; }
  });
}
function attachCallee(key, calleeAbs, nargs, rel){
  Interceptor.attach(calleeAbs, { onEnter(a){
    if (CUR[key]===null || CUR[key]===undefined) return;   // only when inside the target
    const sp=this.context.esp; let parts=['c@0x'+(calleeAbs.toUInt32()-DELTA).toString(16)];
    for(let i=0;i<nargs;i++){
      try{
        const v=sp.add(4+i*4).readU32();
        if(rel && i===0){ const exp=abs(rel.rel).readU32()-rel.sub; parts.push('d='+((v-exp)|0)); }
        else parts.push(v.toString(16));
      }catch(e){ parts.push('?'); }
    }
    CUR[key].push('['+parts.join(',')+']');
  }});
}
function attachBuffer(key, where, buf, size){
  Interceptor.attach(where, { onEnter(a){}, onLeave(r){
    try{ const bytes=abs(buf).readByteArray(size); const u8=new Uint8Array(bytes);
      let h=''; for(let i=0;i<u8.length;i++) h+=u8[i].toString(16).padStart(2,'0');
      record(key, 'buf:'+h); }catch(e){ record(key,'buf-err'); }
  }});
}

rpc.exports = {
  setup:function(mode, targets, nav, settle, dwell){
    MODE=mode; NAV=nav; SETTLE=settle; DWELL=dwell; t0=Date.now();
    GATE = t0 + SETTLE + NAV.length*DWELL + 800;
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMGBASE;
    try { Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){ const k=navControl(); if(k>=0&&this.p===0&&this.c===k){
        const slot=Math.floor((Date.now()-t0-SETTLE)/DWELL); if(slot===lastSlot) return;
        lastSlot=slot; ret.replace(ptr(0xff)); } }
    }); } catch(e){ send({kind:'err',msg:'navarm '+e}); }
    let asi=null;
    if (mode==='on'){ try{ asi=Process.findModuleByName('mashed_re_dev.asi'); }catch(e){}
      send({kind:'info', msg:'asi='+(asi?asi.base:'NOT FOUND')}); }
    for (const key in targets){
      const T=targets[key]; CUR[key]=null;
      // target entry: original RVA (off) or .asi export (on)
      let tw=null;
      if (mode==='off') tw=abs(parseInt(key,16));
      else { if(asi){ try{ tw=asi.findExportByName(T.name); }catch(e){} } }
      if (!tw){ send({kind:'err', msg:'no target site '+T.name}); continue; }
      if (T.kind==='calltrace'){
        attachTargetCalltrace(key, tw);
        // callees are ALWAYS at original RVAs (reimpl calls them by absolute addr)
        T.callees.forEach(function(c){ attachCallee(key, abs(parseInt(c[0],16)), c[1], c[2]); });
      } else if (T.kind==='buffer'){
        attachBuffer(key, tw, T.buf, T.size);
      }
      send({kind:'info', msg:'armed '+T.name+' ('+T.kind+') @'+tw});
    }
    return 1;
  },
  samples:function(){ return SAMP; },
  jmpbyte:function(rva){ try{ return abs(parseInt(rva,16)).readU8(); }catch(e){ return -1; } }
};
send({kind:'ready'});
'''

def run(mode, nav, settle, dwell, seconds):
    env = dict(os.environ); env["MASHED_RE_DEV"]="1"
    env["MASHED_HOOK_ONLY"] = ",".join(TARGETS.keys()) if mode=="on" else "__none__"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type")=="error": print("   agent err:", m.get("description")); return
        p=m.get("payload",{})
        if p.get("kind") in ("info","err"): print("   ", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.setup(mode, TARGETS, nav, settle, dwell)
    dev.resume(pid)
    t=time.time()+seconds
    while time.time()<t:
        if psutil and not psutil.pid_exists(pid): print("   exited early"); break
        time.sleep(0.25)
    samples, jmp = {}, {}
    try: samples = scr.exports_sync.samples()
    except Exception: pass
    if mode=="on":
        for r in TARGETS:
            try: jmp[r]=scr.exports_sync.jmpbyte(r)
            except Exception: jmp[r]=-1
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"samples":samples, "jmp":jmp}

def main():
    nav = [int(x) for x in sys.argv[sys.argv.index("--nav")+1].split(",")] if "--nav" in sys.argv else [4,4]
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 7000
    dwell  = int(sys.argv[sys.argv.index("--dwell")+1])  if "--dwell"  in sys.argv else 1500
    seconds= int(sys.argv[sys.argv.index("--seconds")+1])if "--seconds"in sys.argv else 20
    LOG.mkdir(parents=True, exist_ok=True)
    print(f"  targets={[(k,TARGETS[k]['name']) for k in TARGETS]} nav={nav}")
    print("\n=== OFF (original) ===")
    off = run("off", nav, settle, dwell, seconds)
    print("\n=== ON (modded; .asi export) ===")
    on = run("on", nav, settle, dwell, seconds)
    print("\n=== SIDE-EFFECT DIFF VERDICT ===")
    out = {}
    for k in TARGETS:
        offset=set(off["samples"].get(k,{}).keys()); onset=set(on["samples"].get(k,{}).keys())
        installed = (on["jmp"].get(k)==0xE9)
        clean = bool(offset) and bool(onset) and (offset==onset)
        verdict = "C4-CLEAN" if (clean and installed) else "HOLD"
        why=[]
        if not offset: why.append("orig-not-observed")
        if not onset: why.append("modded-not-observed")
        if offset and onset and offset!=onset: why.append("BEHAVIOR-DIFF")
        if not installed: why.append("no-JMP")
        # truncate long sigs for printing
        def trunc(s): return {(x[:90]+'..' if len(x)>92 else x) for x in s}
        out[k]={"name":TARGETS[k]["name"],"kind":TARGETS[k]["kind"],"installed":installed,
                "clean":clean,"verdict":verdict,"why":why,
                "off_n":len(offset),"on_n":len(onset),
                "off":sorted(offset)[:4],"on":sorted(onset)[:4]}
        print(f"  {k} {TARGETS[k]['name']:<20} -> {verdict} {','.join(why)}  (off {len(offset)} sigs, on {len(onset)} sigs)")
        if verdict!="C4-CLEAN":
            print(f"        off={sorted(trunc(offset))}")
            print(f"        on ={sorted(trunc(onset))}")
    (LOG/"c4_sidediff_result.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\n  -> log/c4_sidediff_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
