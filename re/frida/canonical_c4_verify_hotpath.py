# Canonical C3->C4 verifier for HOT-PATH leaf-math primitives (render transforms).
#
# Hot per-frame functions (RwV3dTransformPoint, Vec2Length, ...) must NOT be
# Interceptor-counted for the whole run (>1000/s destabilizes MASHED in ~6s,
# per CLAUDE.md). So this tool splits the evidence safely:
#   ON run    - MASHED_HOOK_ONLY=<candidates>; install (inline-JMP, ZERO overhead)
#               and survive the full render-heavy boot-to-menu scenario. No
#               Interceptor. Confirms installed (0xE9) + no regression.
#   COUNT run - installs nothing; resume, wait for the menu (SETTLE), THEN attach
#               Interceptor and count for a SHORT window (~2.5s, well under the
#               ~6s destabilization threshold), detach, kill. Confirms exercised.
#
# NO OS input is used anywhere (idle boot-to-menu renders a 3D scene + attract
# demo, which exercises render-math without navigation).
#
# C4 verdict = installed AND survived (ON) AND exercised>0 (COUNT), on top of the
# prior C3 full-domain force-call bit-identity. Leaf-math primitives only.
#
# Usage: py -3.12 re/frida/canonical_c4_verify_hotpath.py 0xRVA[,..] [--seconds N] [--settle S] [--window W]
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
  send({kind:'crash',type:d.type,address:d.address.toString(),eip:c.pc.toString(),
    eax:c.eax.toString(),ebx:c.ebx.toString(),ecx:c.ecx.toString(),edx:c.edx.toString(),
    esi:c.esi.toString(),edi:c.edi.toString(),ebp:c.ebp.toString(),esp:c.esp.toString(),
    bytes_at_eip:bAt.trim(),
    mem_address:(d.memory&&d.memory.address)?d.memory.address.toString():'?',
    mem_op:(d.memory&&d.memory.operation)?d.memory.operation:'?', stack:st, modules:modtab()});
  return false;
});
const HANDLES={};
rpc.exports = {
  firstbyte:function(rva){try{return ptr(rva).readU8();}catch(e){return -1;}},
  counts:function(){return COUNT;},
  attachcount:function(rvas){               // attach AFTER settle (short window)
    rvas.forEach(function(r){ COUNT[r]=0;
      try{ HANDLES[r]=Interceptor.attach(ptr(r),{onEnter:function(){COUNT[r]++;}}); }
      catch(e){ COUNT[r]=-1; } });
    return Object.keys(HANDLES).length;
  },
  detachall:function(){ for(const k in HANDLES){ try{HANDLES[k].detach();}catch(e){} } Interceptor.flush(); return 1; }
};
send({kind:'ready'});
'''

def spawn(candidates_csv):
    env = dict(os.environ); env["MASHED_RE_DEV"]="1"; env["MASHED_HOOK_ONLY"]=candidates_csv
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    st = {"crash": None}
    scr = sess.create_script(AGENT)
    scr.on("message", lambda m,d: st.update(crash=m["payload"]) if m.get("type")!="error" and m.get("payload",{}).get("kind")=="crash" else None)
    scr.load(); dev.resume(pid)
    return dev, pid, sess, scr, st

def survive_run(rvas, seconds):
    dev, pid, sess, scr, st = spawn(",".join(rvas))
    deadline = time.time()+seconds
    while time.time() < deadline:
        if st["crash"]: break
        if psutil and not psutil.pid_exists(pid): break
        time.sleep(0.25)
    alive = psutil.pid_exists(pid) if psutil else None
    jmp = {}
    if not st["crash"]:
        for r in rvas:
            try: jmp[r] = scr.exports_sync.firstbyte(r)
            except Exception as e: jmp[r] = str(e)
    try:
        if alive: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"crash": st["crash"], "alive": alive, "jmp": jmp}

def count_run(rvas, settle, window):
    dev, pid, sess, scr, st = spawn("__none__")
    # let it reach the menu/attract (renders the 3D scene) BEFORE attaching
    t = time.time()+settle
    while time.time() < t:
        if st["crash"] or (psutil and not psutil.pid_exists(pid)): break
        time.sleep(0.25)
    counts = {}
    if not st["crash"] and (not psutil or psutil.pid_exists(pid)):
        try:
            scr.exports_sync.attachcount(rvas)   # SHORT hot window starts here
            time.sleep(window)
            scr.exports_sync.detachall()
            counts = scr.exports_sync.counts()
        except Exception as e:
            counts = {"_err": str(e)}
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"crash": st["crash"], "counts": counts}

def main():
    if len(sys.argv) < 2: sys.exit("usage: ... 0xRVA[,..] [--seconds N] [--settle S] [--window W]")
    rvas = [c.strip() for c in sys.argv[1].split(",") if c.strip()]
    def arg(name, default):
        return type(default)(sys.argv[sys.argv.index(name)+1]) if name in sys.argv else default
    seconds = arg("--seconds", 22); settle = arg("--settle", 14.0); window = arg("--window", 2.5)
    LOG.mkdir(parents=True, exist_ok=True)

    print(f"=== ON run (install {len(rvas)} hot-path math hooks; survive {seconds}s, no Interceptor) ===")
    on = survive_run(rvas, seconds)
    if on["crash"]:
        c = on["crash"]; print(f"  ON CRASH: {c['type']} eip={c['eip']}")
        (LOG/"crash_eip.txt").write_text(json.dumps(c, indent=2), encoding="utf-8")
    print(f"  ON alive={on['alive']}  jmp={ {r:hex(b) if isinstance(b,int) and b>=0 else b for r,b in on['jmp'].items()} }")

    print(f"\n=== COUNT run (attach AFTER {settle}s settle; {window}s hot window) ===")
    cnt = count_run(rvas, settle, window)
    if cnt["crash"]: print(f"  COUNT run crashed (baseline): {cnt['crash']['eip']}")
    print(f"  counts={cnt['counts']}")

    print(f"\n=== C4 VERDICT (installed AND survived AND exercised) ===")
    verdicts={}
    for r in rvas:
        installed = (on["jmp"].get(r) == 0xE9)
        survived  = (on["crash"] is None) and bool(on["alive"])
        calls = cnt["counts"].get(r.lower(), cnt["counts"].get(r, 0))
        try: calls=int(calls)
        except Exception: calls=0
        ok = installed and survived and calls>0
        verdicts[r]={"installed":installed,"survived":survived,"calls":calls,"C4":ok}
        print(f"  {r}: installed={installed} survived={survived} calls={calls} -> {'C4-READY' if ok else 'HOLD'}")
    (LOG/"c4_hotpath_result.json").write_text(json.dumps(verdicts,indent=2),encoding="utf-8")
    print("\n  -> log/c4_hotpath_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
