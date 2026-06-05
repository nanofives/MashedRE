# Canonical C3->C4 verifier (rubric-clean: installed + exercised + no regression).
#
# For each candidate RVA, runs the boot-to-menu canonical scenario TWICE:
#   OFF run  - .asi loaded but MASHED_HOOK_ONLY="__none__" (installs nothing);
#              Interceptor.attach each ORIGINAL RVA and count calls during boot.
#              Proves the function is ON the canonical path (exercised>0) and is
#              the ORIGINAL's behavior trace. (Boot-once / cold fns only — do NOT
#              feed hot-path fns here; Interceptor on >1000/s destabilizes MASHED.)
#   ON run   - MASHED_HOOK_ONLY=<all candidates>; confirm each inline-JMP is live
#              (first byte 0xE9) and the game boots-to-menu and survives (no
#              regression with OUR code executing).
#
# C4 verdict per candidate = exercised(OFF calls>0) AND installed(0xE9) AND
# survived(ON run alive, no crash). Bit-identity over the full input domain is
# the prior C3 evidence (force-call A/B); for a pure leaf the canonical scenario
# cannot feed an input the C3 diff did not already cover, so this trio is
# sufficient C4. Non-leaves additionally require the captured-input replay
# (out of scope here -- gate this tool to leaves).
#
# Usage:
#   py -3.12 re/frida/canonical_c4_verify.py 0xRVA[,0xRVA...] [--seconds N]
import json, os, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE  = ORIG / "MASHED.exe"
LOG  = ROOT / "log"

# ---- agent: crash handler + optional Interceptor call-counters + RPC ----
AGENT = r'''
'use strict';
const COUNT = {};
function modtab(){ const o=[]; Process.enumerateModules().forEach(m=>o.push(
  {name:m.name,base:m.base.toString(),end:m.base.add(m.size).toString()})); return o; }
const FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  const c=d.context; let bAt='';
  try{const e=ptr(d.address.toString()); for(let i=0;i<16;i++){try{bAt+=e.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(x){bAt+='?? ';}}}catch(x){}
  let st=[]; try{const sp=ptr(c.esp.toString()); for(let i=0;i<320;i++){try{st.push({off:i*4,val:sp.add(i*4).readU32().toString(16)});}catch(x){break;}}}catch(x){}
  send({kind:'crash',type:d.type,address:d.address.toString(),
    eax:c.eax.toString(),ebx:c.ebx.toString(),ecx:c.ecx.toString(),edx:c.edx.toString(),
    esi:c.esi.toString(),edi:c.edi.toString(),ebp:c.ebp.toString(),esp:c.esp.toString(),eip:c.pc.toString(),
    bytes_at_eip:bAt.trim(),
    mem_address:(d.memory&&d.memory.address)?d.memory.address.toString():'?',
    mem_op:(d.memory&&d.memory.operation)?d.memory.operation:'?', stack:st, modules:modtab()});
  return false;
});
rpc.exports = {
  firstbyte:function(rva){try{return ptr(rva).readU8();}catch(e){return -1;}},
  counts:function(){return COUNT;},
  // Attach Interceptor counters to a list of ORIGINAL RVAs (OFF run only).
  countthese:function(rvas){
    rvas.forEach(function(r){
      COUNT[r]=0;
      try{ Interceptor.attach(ptr(r),{onEnter:function(){COUNT[r]++;}}); }
      catch(e){ COUNT[r]=-1; }
    });
    return Object.keys(COUNT).length;
  }
};
send({kind:'ready'});
'''

def run(candidates_csv, rvas, count_rvas, seconds):
    """One canonical run. If count_rvas given -> OFF/trace mode."""
    env = dict(os.environ)
    env["MASHED_RE_DEV"]    = "1"
    env["MASHED_HOOK_ONLY"] = candidates_csv
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    st = {"crash": None}
    sess_script = sess.create_script(AGENT)
    sess_script.on("message", lambda m,d: st.update(crash=m["payload"]) if m.get("type")!="error" and m.get("payload",{}).get("kind")=="crash" else None)
    sess_script.load()
    if count_rvas:
        sess_script.exports_sync.countthese(count_rvas)  # attach before resume
    dev.resume(pid)
    deadline = time.time() + seconds
    while time.time() < deadline:
        if st["crash"]: break
        if psutil and not psutil.pid_exists(pid): break
        time.sleep(0.25)
    alive = psutil.pid_exists(pid) if psutil else None
    jmp, counts = {}, {}
    if not st["crash"]:
        for r in rvas:
            try: jmp[r] = sess_script.exports_sync.firstbyte(r)
            except Exception as e: jmp[r] = str(e)
        if count_rvas:
            try: counts = sess_script.exports_sync.counts()
            except Exception: pass
    try:
        if alive: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"crash": st["crash"], "alive": alive, "jmp": jmp, "counts": counts}

def main():
    if len(sys.argv) < 2:
        sys.exit("usage: canonical_c4_verify.py 0xRVA[,0xRVA...] [--seconds N]")
    rvas = [c.strip() for c in sys.argv[1].split(",") if c.strip()]
    seconds = 22
    if "--seconds" in sys.argv:
        seconds = int(sys.argv[sys.argv.index("--seconds")+1])
    LOG.mkdir(parents=True, exist_ok=True)

    print(f"=== OFF run (count calls during boot-to-menu; installs nothing) ===")
    off = run("__none__", [], rvas, seconds)
    if off["crash"]:
        print(f"  OFF run CRASHED unexpectedly (baseline): eip={off['crash']['eip']}")
    print(f"  OFF alive={off['alive']} counts={off['counts']}")

    print(f"\n=== ON run (install ONLY candidates; confirm JMP-live + survive) ===")
    on = run(",".join(rvas), rvas, None, seconds)
    if on["crash"]:
        c = on["crash"]
        print(f"  ON run CRASH: {c['type']} eip={c['eip']} mem={c.get('mem_address')}")
        (LOG/"crash_eip.txt").write_text(json.dumps(c, indent=2), encoding="utf-8")
    print(f"  ON alive={on['alive']}")

    print(f"\n=== C4 VERDICT (exercised AND installed AND survived) ===")
    verdicts = {}
    for r in rvas:
        rl = r.lower()
        calls = off["counts"].get(rl, off["counts"].get(r, 0))
        try: calls = int(calls)
        except Exception: calls = 0
        installed = (on["jmp"].get(r) == 0xE9)
        survived  = (on["crash"] is None) and bool(on["alive"])
        ok = (calls > 0) and installed and survived
        verdicts[r] = {"calls": calls, "installed": installed, "survived": survived, "C4": ok}
        print(f"  {r}: calls={calls} installed={installed} survived={survived} -> {'C4-READY' if ok else 'HOLD'}")
    (LOG/"c4_verify_result.json").write_text(json.dumps(verdicts, indent=2), encoding="utf-8")
    print(f"\n  -> log/c4_verify_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
