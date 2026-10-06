# Boot the ORIGINAL to the menu and hold it open with an rpc nav controller,
# so an outside orchestrator can push/pop screens and take window screenshots
# between steps (side-by-side parity captures vs the standalone).
#
# Writes one line per state change to log/orig_nav_hold.state so the
# orchestrator can poll readiness:  READY <pid> / PUSH <scr> <depth> / ...
# Navigation is driven by a step file: log/orig_nav_hold.cmd — each line is
#   push <scr> | pop | reload | quit
# appended by the orchestrator; this script tails and executes them.
#
# Usage: py -3.12 re/frida/orig_nav_hold.py  (runs until 'quit' or 180s)
#
# [D4 2026-10-05] Two additions, both opt-in; the cmd-file contract above is
# unchanged and is still the default path.
#
#   MASHED_ROOT   point the spawn at a COPY of the install, exactly as
#                 scenario_launch.py:29-31 does. Needed to boot the original
#                 against a chosen gamesave.bin without writing into the repo's
#                 original/ (which CLAUDE.md protects).
#   --scan A-B    push each screen id in [A,B], peek after each, pop back, and
#                 report the first id at which a watched global changes. Added to
#                 find WHERE the frontend reads gamesave.bin: four legs in
#                 verify/d4_save_20261005 showed the save is never read in a
#                 warp-launched run NOR at the top menu, so the load must be
#                 deeper, and guessing screen ids is worse than scanning them.
#
#                 *** --scan IS NOT VALIDATED. DO NOT TRUST ITS OUTPUT YET. ***
#                 First run (2026-10-05, arm P, ids 1..24) reported "no-change" on
#                 every id -- but `depth` read a CONSTANT 2 immediately after every
#                 push, so push() never navigated and the sweep proves nothing
#                 about screens. Whoever picks this up must first establish that
#                 push() moves `depth`; until then a clean --scan run is an absence
#                 of navigation, not an absence of a trigger. The script also
#                 reported phase=3 while waiting for 1, so this agent's RVA_PHASE
#                 (0x0067eca4) does NOT mean what scenario_launch.py's phase means
#                 -- do not compare the two.
#   --peek        plain Memory reads, no Interceptor, same spec as
#                 scenario_launch.py --peek (rva:type, type in f/d/i/u).
import argparse, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
# [D4] honour MASHED_ROOT so the game can be booted from a copied install.
GAME_ROOT = Path(os.environ.get("MASHED_ROOT", ROOT))
ORIG = GAME_ROOT / "original"; EXE = ORIG / "MASHED.exe"
STATE = ROOT / "log" / "orig_nav_hold.state"
CMD   = ROOT / "log" / "orig_nav_hold.cmd"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
let nav=null;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  depth:function(){ return abs(RVA_DEPTH).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); },
  reload:function(scr){ nav(scr,2); return abs(RVA_DEPTH).readS32(); },
  // [D4 2026-10-05] plain global reads. NO Interceptor, no hook, no write --
  // same contract and same spec as scenario_launch.py's --peek.
  peek:function(spec){
    const out={};
    for (const part of spec.split(',')){
      if(!part) continue;
      const b=part.split(':'); const rva=parseInt(b[0],16); const ty=(b[1]||'u');
      try{
        const p=abs(rva);
        out[b[0]] = ty==='f'? p.readFloat() : ty==='d'? p.readDouble()
                  : ty==='i'? p.readS32() : p.readU32();
      }catch(e){ out[b[0]]='ERR'; }
    }
    return out;
  }
};
'''


def emit(line):
    with STATE.open("a", encoding="utf-8") as f:
        f.write(line + "\n")
    print(line, flush=True)


def scan_mode(E, dev, pid, lo, hi, spec):
    """[D4] Push each screen id in [lo,hi], peek, pop back. Report the first id at
    which any watched value changes from its menu baseline.

    Why a scan and not a guessed id: the frontend save-load site is unknown, and
    four legs have now shown the save is read neither in a warp-launched run nor at
    the top menu. A scan finds the trigger without inventing a screen map.
    """
    base = E.peek(spec)
    emit(f"SCAN baseline {base}")
    first = None
    for scr in range(lo, hi + 1):
        try:
            d = E.push(scr)
        except Exception as e:
            emit(f"SCAN {scr} push-ERR {e}")
            break
        time.sleep(0.45)
        try:
            v = E.peek(spec)
        except Exception as e:
            emit(f"SCAN {scr} peek-ERR {e} (process likely gone)")
            break
        changed = {k: (base[k], v[k]) for k in v if v[k] != base[k]}
        emit(f"SCAN {scr} depth={d} {'CHANGED ' + str(changed) if changed else 'no-change'}")
        if changed and first is None:
            first = (scr, changed)
        try:
            E.pop()
        except Exception:
            pass
        time.sleep(0.25)
    emit(f"SCAN RESULT first-change={first}")
    return first


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--scan", default="", help="[D4] screen-id range A-B to sweep")
    ap.add_argument("--peek", default="", help="[D4] rva:type,... plain reads, no hook")
    ap.add_argument("--menu", action="store_true",
                    help="[D4] wait for the MENU (phase 1) instead of phase 3")
    args = ap.parse_args()

    STATE.parent.mkdir(parents=True, exist_ok=True)
    STATE.write_text("", encoding="utf-8")
    CMD.write_text("", encoding="utf-8")
    emit(f"EXE {EXE}")
    dev = frida.get_local_device()
    # Hooks OFF: a bare spawn lets the dinput8 loader install the FULL hook
    # set, which has a known full-install instability — reference captures
    # must be stock behavior anyway.
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    E = scr.exports_sync

    want = 1 if (args.menu or args.scan) else 3
    end = time.time() + 30
    while time.time() < end and E.phase() != want:
        time.sleep(0.2)
    time.sleep(1.5)
    emit(f"READY {pid} phase={E.phase()} (wanted {want})")

    # [D4] one-shot scan mode: no cmd-file orchestration needed.
    if args.scan:
        lo, _, hi = args.scan.partition("-")
        try:
            scan_mode(E, dev, pid, int(lo), int(hi), args.peek)
        finally:
            try: dev.kill(pid)
            except Exception: pass
        return 0

    seen = 0
    deadline = time.time() + 600
    while time.time() < deadline:
        lines = CMD.read_text(encoding="utf-8").splitlines()
        for ln in lines[seen:]:
            seen += 1
            t = ln.strip().split()
            if not t:
                continue
            try:
                if t[0] == "push":
                    emit(f"PUSH {t[1]} {E.push(int(t[1]))}")
                elif t[0] == "pop":
                    emit(f"POP {E.pop()}")
                elif t[0] == "reload":
                    emit(f"RELOAD {E.reload(int(t[1]))}")
                elif t[0] == "quit":
                    emit("QUIT")
                    try: dev.kill(pid)
                    except Exception: pass
                    return 0
            except Exception as e:
                emit(f"ERR {e}")
        time.sleep(0.25)
    try: dev.kill(pid)
    except Exception: pass
    emit("TIMEOUT")
    return 0


if __name__ == "__main__":
    sys.exit(main())
