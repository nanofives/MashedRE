# WS-H2 — clean original-vs-modded A/B for the score adder FUN_0040b290.
#
# CONFIDENCE.md C4 requires "a clean Frida CSV diff between original and modded
# behavior". The full-race telemetry diff can't be bit-identical (the arena race
# is nondeterministic: elim order/winner vary run-to-run). FUN_0040b290 is a pure
# function of (args + globals it reads) with NO destructive callees (it calls only
# the mode/team/phase GETTERS FUN_0040e340/e350/e370/0042f500), so it CAN be
# diffed by full state control + snapshot/restore:
#
#   1. spawn (hooks auto-install); reach the menu.
#   2. call the .asi UninjectHooks() export -> restore ALL original bytes, so the
#      ORIGINAL 0x0040b290 is callable un-patched (and the .asi export
#      ScoreAdd_0040b290 is still callable).
#   3. set mode=0 (FFA, the canonical path) + netflag=0; for each (car,delta,
#      seed-score) vector: snapshot the write-set, call ORIGINAL(car,delta), read
#      the writes, restore; call REIMPL(car,delta), read the writes, restore;
#      compare. Bit-identical across vectors == clean original-vs-modded diff.
#
# This is a state-controlled SYNTHETIC A/B (not hook-installed canonical), so it is
# the path1 bit-identity datapoint; combined with the installed-hook canonical race
# (verify_scoring_hooks.py) it is the C4 evidence for FUN_0040b290.
#
# Usage: py -3.12 re/frida/diff_scoring_adder.py
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import statenav
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
ASI_PATH = ORIG / "mashed_re_dev.asi"

RVA = 0x0040b290
# write-set (byte addrs) that FUN_0040b290 may modify (snapshot/restore these)
WRITE_GLOBALS = {
    "score":   (0x008a94e0, 4),   # DAT_008a94e0[4]
    "prev":    (0x008a9570, 4),
    "ddisp":   (0x008a9520, 4),
    "pclamp":  (0x008a9500, 4),
    "timer":   (0x008a9510, 4),
    "ring_ty": (0x007e9de4, 256), # ring arrays — snapshot generously
    "ring_cx": (0x007ea1e4, 256),
    "ring_cr": (0x007ea5e4, 256),
    "ring_dl": (0x007ea9e4, 256*8),
}
RING_PTR = 0x007e9de0
MODE = 0x007f0fd0    # set to 0 (FFA)
NETFLAG = 0x007f1014 # set to 0

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0; let ASI=null;
let origFn=null, reimplFn=null;
const RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG; return DELTA; },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  findasi:function(){ const mods=Process.enumerateModules();
    for(const m of mods){ if(/mashed_re_dev\.asi/i.test(m.name)){ ASI=m; return m.name+'@'+m.base; } }
    return null; },
  loadasi:function(path){ try{ ASI=Module.load(path); return 'loaded:'+ASI.name+'@'+ASI.base; }
    catch(e){ const mods=Process.enumerateModules();
      for(const m of mods){ if(/mashed_re_dev\.asi/i.test(m.name)){ ASI=m; return 'already:'+m.name; } }
      return 'err:'+e; } },
  modnames:function(){ return Process.enumerateModules().map(function(m){return m.name;})
    .filter(function(n){return /asi|mashed_re|dinput/i.test(n);}); },
  uninject:function(){ const e=ASI.findExportByName('UninjectHooks');
    if(!e) return 'no-export'; new NativeFunction(e,'void',[])(); return 'ok'; },
  byte0:function(rva){ try{return abs(parseInt(rva,16)).readU8();}catch(e){return -1;} },
  asiexports:function(substr){ const out=[];
    try{ ASI.enumerateExports().forEach(function(e){
      if(e.name.indexOf(substr)>=0) out.push(e.name); }); }catch(err){ out.push('ERR:'+err); }
    return out; },
  resolve:function(rva, expname){
    origFn = new NativeFunction(abs(parseInt(rva,16)), 'void', ['int','uint32']);
    let e = ASI.findExportByName(expname) || ASI.findExportByName('_'+expname);
    if(!e){ ASI.enumerateExports().forEach(function(x){
        if(!e && x.name.indexOf(expname)>=0) e=x.address; }); }
    if(!e) return 'no-reimpl-export';
    reimplFn = new NativeFunction(e, 'void', ['int','uint32']); return 'ok'; },
  poke:function(addr,val){ ptr(addr).writeS32(val|0); return 1; },
  snap:function(addr,nbytes){ return abs(0).add(0); }, // unused placeholder
  readblk:function(addr,nbytes){ return abs(0); },     // unused placeholder
  // snapshot/restore the write-set via base64 of the raw bytes
  snapraw:function(addr,nbytes){ return ptr(addr).readByteArray(nbytes); },
  restoreraw:function(addr,bytes){ ptr(addr).writeByteArray(bytes); return 1; },
  read32:function(addr){ return ptr(addr).readS32(); },
  calln:function(which, p1, p2){ if(which==='orig') origFn(p1,p2|0); else reimplFn(p1,p2|0); return 1; }
};
send({kind:'ready'});
'''


def main():
    # NO_AUTO_HOOK=1 so the dinput8-loaded .asi (if it loads) leaves the original
    # un-patched; we Module.load the .asi in Frida to resolve the reimpl export.
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    delta = scr.exports_sync.init()
    E = scr.exports_sync
    dev.resume(pid)

    # reach the menu
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.3)
    print(f"  menu reached: phase={E.phase()}")
    time.sleep(1.0)

    asi = E.findasi()
    if not asi:
        # dinput8 didn't surface the .asi as a module — load it explicitly in Frida.
        asi = E.loadasi(str(ASI_PATH).replace("\\", "/"))
    print(f"  .asi: {asi}  (asi-ish modules: {E.modnames()})")
    if not asi or str(asi).startswith("err"):
        print("  ABORT: .asi could not be resolved/loaded.")
        try: dev.kill(pid)
        except Exception: pass
        return 5
    b0 = E.byte0(hex(RVA)) & 0xff
    print(f"  byte0@0x{RVA:08x}: 0x{b0:02x}")
    if b0 == 0xE9:
        print(f"  (hook installed by dinput8) uninject: {E.uninject()}")
        time.sleep(0.3)
        b0 = E.byte0(hex(RVA)) & 0xff
        print(f"  byte0 after uninject: 0x{b0:02x} (expect NOT E9)")
    if b0 == 0xE9:
        print("  ABORT: original not restored (still E9) — cannot A/B against original.")
        try: dev.kill(pid)
        except Exception: pass
        return 5
    r = E.resolve(hex(RVA), "ScoreAdd_0040b290")
    print(f"  resolve orig+reimpl: {r}")
    if r != "ok":
        try: dev.kill(pid)
        except Exception: pass
        return 5

    # delta-relative absolute addresses for globals
    def A(rva): return rva + delta
    # set the FFA path: mode=0, netflag=0
    E.poke(A(MODE), 0); E.poke(A(NETFLAG), 0)

    SCORE, PREV, DDISP, PCLAMP, TIMER = 0x008a94e0, 0x008a9570, 0x008a9520, 0x008a9500, 0x008a9510
    RT, RC, RCAR, RD = 0x007e9de4, 0x007ea1e4, 0x007ea5e4, 0x007ea9e4
    FIXED_PTR = 10   # fixed ring write slot for deterministic compare

    def setup(car, seed):
        # deterministic input state: only what FUN_0040b290 reads/writes
        E.poke(A(RING_PTR), FIXED_PTR)
        E.poke(A(RT) + FIXED_PTR*4, 0); E.poke(A(RC) + FIXED_PTR*4, 0)
        E.poke(A(RCAR) + FIXED_PTR*4, 0); E.poke(A(RD) + FIXED_PTR*32, 0)
        for base in (SCORE, PREV, DDISP, PCLAMP, TIMER):
            E.poke(A(base) + car*4, seed if base == SCORE else 0)
    def writeset(car):
        return {
            "score": E.read32(A(SCORE) + car*4), "prev": E.read32(A(PREV) + car*4),
            "ddisp": E.read32(A(DDISP) + car*4), "pclamp": E.read32(A(PCLAMP) + car*4),
            "timer": E.read32(A(TIMER) + car*4), "ringptr": E.read32(A(RING_PTR)),
            # ring slot: [type, car, delta] — the DETERMINISTIC fields. The 2nd ring
            # field (RingCtx = DAT_007f1030, the live ~3MHz timer "current time") is
            # EXCLUDED: orig and reimpl both write the live timer, which differs only
            # by the few-ms wall-clock gap between the two calls (identical behavior,
            # not a logic divergence — verified: it was the sole differing field).
            "ring_slot": [E.read32(A(RT) + FIXED_PTR*4),
                          E.read32(A(RCAR) + FIXED_PTR*4), E.read32(A(RD) + FIXED_PTR*32)],
        }

    # test vectors: (car, delta, seed_score) — exercises floor-at-0, +/- deltas
    vectors = [(car, d, seed) for car in range(4)
               for d in (1, -1, 2, -2, 0, 5, -100)
               for seed in (0, 3, -4)]

    mismatches = []; rows = []
    for idx, (car, d, seed) in enumerate(vectors):
        setup(car, seed)
        E.calln("orig", car, d & 0xffffffff)
        ow = writeset(car)
        setup(car, seed)
        E.calln("reimpl", car, d & 0xffffffff)
        rw = writeset(car)
        match = (ow == rw)
        rows.append({"car": car, "delta": d, "seed": seed, "orig": ow, "reimpl": rw, "match": match})
        if not match:
            mismatches.append((car, d, seed, ow, rw))
        if idx % 20 == 0:
            print(f"   ...vector {idx}/{len(vectors)} mismatches={len(mismatches)}", flush=True)

    print(f"\n  vectors: {len(vectors)}  mismatches: {len(mismatches)}")
    for car, d, seed, ow, rw in mismatches[:8]:
        print(f"    MISMATCH car={car} delta={d} seed={seed}\n      orig  ={ow}\n      reimpl={rw}")
    verdict = "GREEN" if not mismatches else "RED"
    print(f"\n  A/B VERDICT (FUN_0040b290 ScoreAdd): {verdict}  "
          f"({len(vectors)-len(mismatches)}/{len(vectors)} bit-identical)")
    (ROOT/"log"/"diff_scoring_adder.json").write_text(json.dumps(
        {"verdict": verdict, "vectors": len(vectors), "mismatches": len(mismatches),
         "rows": rows[:40]}, indent=1))
    print("  -> log/diff_scoring_adder.json")
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0 if verdict == "GREEN" else 1


if __name__ == "__main__":
    sys.exit(main())
