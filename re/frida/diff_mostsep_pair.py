# WS-H2 — clean original-vs-modded A/B for FUN_0040e180 (MostSeparatedPair),
# the camera/elimination cluster's 3D most-separated-pair finder.
#
# Same proven infra as diff_scoring_adder.py: Module.load the .asi (robust vs the
# flaky dinput8 loader), uninject so the ORIGINAL 0x0040e180 is callable un-patched,
# then full state-controlled A/B. FUN_0040e180 only WRITES its two out-params (it
# does not mutate car state), so no snapshot/restore is needed — just set the car
# state, call original then reimpl, compare the (inner, outer) pair.
#
# Synthetic car state (offsets decompiled from the getters this fn forwards to):
#   slot-active : *(int*)(*(int*)0x5f2770 + 0x34 + i*4)   (we point 0x5f2770 at our buf)
#   alive[i]    : *(int*)(0x008815a4 + i*0xd04) = 1
#   dead[i]     : *(int*)(0x00881f90 + i*0xd04) = 0
#   idx[i]      : *(int*)(0x00881f48 + i*0xd04) = 0   (so CarPtr base = 0x881ec8 + i*0xd04)
#   pos[i]      : float @ 0x00881ec8 + i*0xd04 + 0x30/0x34/0x38
#
# Usage: py -3.12 re/frida/diff_mostsep_pair.py
import json, os, struct, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import statenav
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
ASI_PATH = ORIG / "mashed_re_dev.asi"

RVA = 0x0040e180
STRIDE = 0xd04
ALIVE, DEAD, IDX, POSB = 0x008815a4, 0x00881f90, 0x00881f48, 0x00881ec8
SLOTPTR = 0x005f2770

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0; let ASI=null;
let origFn=null, reimplFn=null; let SLOTBUF=null; let OUTA=null, OUTB=null;
const RVA_PHASE=0x0067eca4;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG; return DELTA; },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  findasi:function(){ const mods=Process.enumerateModules();
    for(const m of mods){ if(/mashed_re_dev\.asi/i.test(m.name)){ ASI=m; return m.name+'@'+m.base; } }
    return null; },
  loadasi:function(path){ try{ ASI=Module.load(path); return 'loaded:'+ASI.name; }
    catch(e){ const mods=Process.enumerateModules();
      for(const m of mods){ if(/mashed_re_dev\.asi/i.test(m.name)){ ASI=m; return 'already:'+m.name; } }
      return 'err:'+e; } },
  byte0:function(rva){ try{ return abs(parseInt(rva,16)).readU8(); }catch(e){ return -1; } },
  uninject:function(){ const e=ASI.findExportByName('UninjectHooks');
    if(!e) return 'no-export'; new NativeFunction(e,'void',[])(); return 'ok'; },
  resolve:function(rva, expname){
    origFn = new NativeFunction(abs(parseInt(rva,16)), 'void', ['pointer','pointer']);
    let e = ASI.findExportByName(expname) || ASI.findExportByName('_'+expname);
    if(!e){ ASI.enumerateExports().forEach(function(x){ if(!e && x.name.indexOf(expname)>=0) e=x.address; }); }
    if(!e) return 'no-reimpl-export';
    reimplFn = new NativeFunction(e, 'void', ['pointer','pointer']);
    OUTA=Memory.alloc(4); OUTB=Memory.alloc(4);
    // own the slot-active table: point *(0x5f2770) at our 0x60-byte buffer
    SLOTBUF=Memory.alloc(0x60);
    abs(0x005f2770).writePointer(SLOTBUF);
    return 'ok'; },
  setcar:function(i, active, alive, dead, px, py, pz){
    SLOTBUF.add(0x34 + i*4).writeS32(active|0);                 // slot-active
    abs(0x008815a4 + i*0xd04).writeS32(alive|0);                // alive
    abs(0x00881f90 + i*0xd04).writeS32(dead|0);                 // dead
    abs(0x00881f48 + i*0xd04).writeS32(0);                      // idx -> 0
    const base=abs(0x00881ec8 + i*0xd04);
    base.add(0x30).writeFloat(px); base.add(0x34).writeFloat(py); base.add(0x38).writeFloat(pz);
    return 1; },
  callpair:function(which){ OUTA.writeS32(-99); OUTB.writeS32(-99);
    if(which==='orig') origFn(OUTA,OUTB); else reimplFn(OUTA,OUTB);
    return [OUTA.readS32(), OUTB.readS32()]; }
};
send({kind:'ready'});
'''


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    E = scr.exports_sync
    dev.resume(pid)
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.3)
    print(f"  menu reached: phase={E.phase()}")
    time.sleep(1.0)

    asi = E.findasi() or E.loadasi(str(ASI_PATH).replace("\\", "/"))
    print(f"  .asi: {asi}")
    if not asi or str(asi).startswith("err"):
        dev.kill(pid); return 5
    b0 = E.byte0(hex(RVA)) & 0xff
    if b0 == 0xE9:
        print(f"  uninject: {E.uninject()}"); time.sleep(0.3); b0 = E.byte0(hex(RVA)) & 0xff
    print(f"  byte0@0x{RVA:08x}: 0x{b0:02x} (expect NOT E9)")
    if b0 == 0xE9:
        print("  ABORT: original still patched."); dev.kill(pid); return 5
    r = E.resolve(hex(RVA), "MostSepPair_0040e180")
    print(f"  resolve: {r}")
    if r != "ok":
        dev.kill(pid); return 5

    # test vectors: list of 4 cars, each (active, alive, dead, x, y, z)
    P = lambda *c: list(c)
    vectors = [
        # spread along X — pair (0,3) most separated
        [P(1,1,0, 0,0,0), P(1,1,0, 10,0,0), P(1,1,0, 20,0,0), P(1,1,0, 30,0,0)],
        # square — diagonal pair max
        [P(1,1,0, 0,0,0), P(1,1,0, 10,0,0), P(1,1,0, 10,0,10), P(1,1,0, 0,0,10)],
        # one car far on Z
        [P(1,1,0, 0,0,0), P(1,1,0, 1,0,0), P(1,1,0, 2,0,0), P(1,1,0, 0,0,50)],
        # 3D spread
        [P(1,1,0, -5,3,-5), P(1,1,0, 5,-2,5), P(1,1,0, 0,10,0), P(1,1,0, 1,1,1)],
        # car 2 DEAD -> excluded; max among {0,1,3}
        [P(1,1,0, 0,0,0), P(1,1,0, 8,0,0), P(1,1,1, 100,0,0), P(1,1,0, 4,0,0)],
        # car 1 not-alive -> excluded
        [P(1,0,0, 50,0,0), P(1,1,0, 0,0,0), P(1,1,0, 7,0,0), P(1,1,0, 3,0,0)],
        # car 3 inactive slot -> excluded
        [P(1,1,0, 0,0,0), P(1,1,0, 6,0,0), P(1,1,0, 2,0,0), P(0,1,0, 99,0,0)],
        # near-tie: (0,1) vs (2,3) almost equal
        [P(1,1,0, 0,0,0), P(1,1,0, 10,0,0), P(1,1,0, 0,0,0.001), P(1,1,0, 10,0,0.001)],
        # all same position -> degenerate (mag 0); tail fixups
        [P(1,1,0, 5,5,5), P(1,1,0, 5,5,5), P(1,1,0, 5,5,5), P(1,1,0, 5,5,5)],
        # only one active car
        [P(1,1,0, 3,3,3), P(0,0,0, 0,0,0), P(0,0,0, 0,0,0), P(0,0,0, 0,0,0)],
        # no active cars -> (0,0)/(0,?) tail
        [P(0,0,0, 0,0,0), P(0,0,0, 0,0,0), P(0,0,0, 0,0,0), P(0,0,0, 0,0,0)],
        # negative coords
        [P(1,1,0, -30,-30,-30), P(1,1,0, 30,30,30), P(1,1,0, 0,0,0), P(1,1,0, -1,2,-3)],
    ]
    mismatches = []; rows = []
    for idx, cars in enumerate(vectors):
        for i, c in enumerate(cars):
            E.setcar(i, c[0], c[1], c[2], float(c[3]), float(c[4]), float(c[5]))
        ow = E.callpair("orig")
        for i, c in enumerate(cars):
            E.setcar(i, c[0], c[1], c[2], float(c[3]), float(c[4]), float(c[5]))
        rw = E.callpair("reimpl")
        match = (ow == rw)
        rows.append({"vec": idx, "orig": ow, "reimpl": rw, "match": match})
        print(f"   vec{idx:2d}: orig={ow}  reimpl={rw}  {'OK' if match else 'MISMATCH'}")
        if not match: mismatches.append((idx, ow, rw))

    verdict = "GREEN" if not mismatches else "RED"
    print(f"\n  A/B VERDICT (FUN_0040e180 MostSeparatedPair): {verdict}  "
          f"({len(vectors)-len(mismatches)}/{len(vectors)} bit-identical)")
    (ROOT/"log"/"diff_mostsep_pair.json").write_text(json.dumps(
        {"verdict": verdict, "vectors": len(vectors), "mismatches": len(mismatches), "rows": rows}, indent=1))
    print("  -> log/diff_mostsep_pair.json")
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0 if verdict == "GREEN" else 1


if __name__ == "__main__":
    sys.exit(main())
