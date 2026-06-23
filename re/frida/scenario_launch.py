# SCENARIO LAUNCHER (2026-06-23) — warp the ORIGINAL MASHED.exe straight into a race,
# bypassing the menu, by writing the selection-state globals and poking the session-phase
# state machine. Build spec: investigation 2026-06-23 (Ghidra-verified).
#
# How it works (re/analysis/game_state/0x004929d0.md):
#   The session is a global state machine FUN_004929d0 switching on the byte DAT_00771968:
#     phase 1 = menu/lobby
#     phase 2 = LOAD TRACK + SPAWN CARS (calls FUN_0040d440 = Course::LoadCurrent, then ->3)
#     phase 3 = race running
#   There is NO callable StartRace(cfg). We set the globals the menu would have built, then
#   write DAT_00771968 = 2 and the engine's own loop loads the track + spawns every car.
#
# Increment 1 (this file): drop into a race on a chosen track and confirm phase->3 + a car
#   spawned. No warp / no input injection yet (those are increment 2/3).
#
# Spawn+attach (NEVER frida.spawn — perturbs boot layout, project_replay_deterministic_clock).
# Kills ONLY the pid it launches. No OS input injection.
#
# Usage:
#   py -3.12 re/frida/scenario_launch.py [--track 0] [--mode 10] [--players 1] [--fps 60] [--hold 20]
import os, sys, time, argparse, subprocess
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
EXE  = ROOT / "original" / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG = 0x400000;
let M = null;
function modBase(){ if(!M) M = Process.findModuleByName('MASHED.exe'); return M ? M.base : null; }
function ga(addr){ const b = modBase(); return b ? b.add(addr - IMG) : null; }   // global VA -> live ptr

// --- selection-state globals (Ghidra-verified build spec 2026-06-23) ---
const PHASE      = 0x00771968;   // session-phase enum (U8): 1=menu 2=load+spawn 3=race
const TRACK_ENG  = 0x0063ba7c;   // engine track idx (FUN_0040d440 loads this)
const TRACK_MENU = 0x0067f17c;   // menu-side track idx (keep consistent)
const MODE       = 0x0067e9fc;   // game-mode 2..10 (10=QuickRace, 2=TimeTrial)
const RULE       = 0x007f0fd0;   // race-rule
const CAR_P0     = 0x0067ea98;   // player-0 car/character cursor
const DIFFICULTY = 0x0067ea7c;   // RaceConfig.difficulty
const POWERUPS   = 0x0067ea80;   // RaceConfig.powerUps
const SLOT0      = 0x007f1a14;   // per-slot car-index array (stride 0x10; -1=inactive)
const PCOUNT     = 0x008a94d0;   // player count 1..4
const TEAM       = 0x0067ea64;   // team-game flag
const CARREC     = 0x008815a0;   // player car record base (stride 0xd04)
const SPAWN_RVA  = 0x0046b540;   // VehicleSpawnInit (one-shot spawn confirm; fires once/car)
const ACTIVATE   = 0x0040e480;   // FUN_0040e480(slot,val) cdecl: writes the per-slot array the
                                 // spawn loop (FUN_004111c0 case DAT_0063ba8c==1) reads at
                                 // PTR_PTR_005f2770+0x34 to decide which slots get VehicleSpawnInit

let spawnFired = 0, spawnArmed = false;
function armSpawn(){ if(spawnArmed) return; spawnArmed = true;
  try { Interceptor.attach(ga(SPAWN_RVA), { onEnter(){ spawnFired++; } }); } catch(e){ send({kind:'err', msg:'armSpawn '+e}); } }

// In-process control injection (nav_agent.js method): override FUN_00497310's return for
// player-0 control `pressCtrl` to 0xff (pressed) while pressUntil is in the future. Control 4
// = confirm/accelerate -> used to skip the race-start intro, keep the race driving, and
// continue between rounds. No OS input injection.
const RES_RVA = 0x00497310;
let pressCtrl = -1, pressUntil = 0, inputArmed = false;
function armInput(){ if(inputArmed) return; inputArmed = true;
  try { Interceptor.attach(ga(RES_RVA), {
    onEnter(){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
    onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }
  }); } catch(e){ send({kind:'err', msg:'armInput '+e}); } }

rpc.exports = {
  ready: function(){ return modBase() ? 1 : 0; },
  phase: function(){ try { return ga(PHASE).readU8(); } catch(e){ return -1; } },
  setup: function(cfg){
    try {
      ga(TRACK_ENG ).writeS32(cfg.track);
      ga(TRACK_MENU).writeS32(cfg.track);
      ga(MODE      ).writeS32(cfg.mode);
      ga(RULE      ).writeS32(cfg.rule);
      ga(CAR_P0    ).writeS32(cfg.car);
      ga(TEAM      ).writeS32(cfg.team);
      // difficulty / powerups: encoding [UNCERTAIN] — only write when explicitly given (>=0),
      // else leave the game default so an unknown value can't break the race.
      if (cfg.difficulty >= 0) ga(DIFFICULTY).writeS32(cfg.difficulty);
      if (cfg.powerups   >= 0) ga(POWERUPS  ).writeS32(cfg.powerups);
      // Activate the per-slot vehicles via FUN_0040e480(slot,val) — THIS is the array the
      // spawn loop reads. slot 0 = human player (1); slots 1..cars-1 = AI (2); rest = empty (0).
      // (The earlier raw DAT_007f1a14 write was the wrong array.) DAT_008a94d0 (player count)
      // is recomputed by the spawn loop, so we do NOT preset it.
      const e480 = new NativeFunction(ga(ACTIVATE), 'void', ['int','int'], 'mscdecl');
      for (let s = 0; s < 4; s++) e480(s, s === 0 ? 1 : (s < cfg.cars ? 2 : 0));
      armSpawn();
      armInput();
      return 'set track='+cfg.track+' mode='+cfg.mode+' cars='+cfg.cars+' car='+cfg.car
             +' rule='+cfg.rule+' team='+cfg.team
             +(cfg.difficulty>=0?' diff='+cfg.difficulty:'')+(cfg.powerups>=0?' powerups='+cfg.powerups:'');
    } catch(e){ return 'ERR '+e; }
  },
  launch: function(){ try { ga(PHASE).writeU8(2); return 1; } catch(e){ return 'ERR '+e; } },
  press: function(c, ms){ pressCtrl = c; pressUntil = Date.now() + ms; return 1; },
  boost: function(v){ try { ga(CARREC).add(0x9b4).writeFloat(v); return 1; } catch(e){ return 'ERR '+e; } },
  carinfo: function(){
    try { const r = ga(CARREC);
      return { spawnFired: spawnFired,
               grounded: r.add(0x9e0).readFloat(),
               pos_via_fwd: [r.add(0x9d4).readFloat(), r.add(0x9d8).readFloat(), r.add(0x9dc).readFloat()],
               vel: [r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat()],
               airflag: r.add(0xb20).readU32() };
    } catch(e){ return { err: ''+e }; }
  }
};
send({kind:'ready'});
'''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, default=0, help="engine track index (0=Arctic)")
    ap.add_argument("--mode", type=int, default=10, help="game-mode (10=QuickRace, 2=TimeTrial)")
    ap.add_argument("--cars", type=int, default=1, help="active car slots (slot 0 = player; rest AI)")
    ap.add_argument("--car", type=int, default=0, help="player car/character index (DAT_0067ea98)")
    ap.add_argument("--rule", type=int, default=0, help="race-rule sub-mode 0..10 (DAT_007f0fd0)")
    ap.add_argument("--team", type=int, default=0, help="team-game flag (DAT_0067ea64; 1=team)")
    ap.add_argument("--powerups", type=int, default=-1, help="power-up setting (DAT_0067ea80; -1=game default)")
    ap.add_argument("--difficulty", type=int, default=-1, help="difficulty (DAT_0067ea7c; -1=game default)")
    ap.add_argument("--boost", type=float, default=0,
                    help="upward vel-Y impulse per tick on the player car to FORCE it airborne "
                         "(grounded->0 => A6b airborne body runs). 0=off. Contrived state (C3-grade).")
    ap.add_argument("--fps", default="60")
    ap.add_argument("--hold", type=int, default=20, help="seconds to hold in the race after spawn")
    ap.add_argument("--hooks", default="",
                    help="comma .asi hook RVAs/names to install LIVE + turn on the physics A/B "
                         "self-test (MASHED_PHYS_C4_SELFTEST -> original/phys_c4_*_selftest.log). "
                         "Empty = stock original. e.g. 0x00468980 for A6b airborne capture.")
    args = ap.parse_args()

    if not EXE.exists():
        print(f"error: {EXE} not found"); return 2

    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    if args.hooks:
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_HOOK_ONLY"] = args.hooks
        env["MASHED_PHYS_C4_SELFTEST"] = "1"
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    else:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"     # stock original, no installed hooks
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    pid = proc.pid
    print(f"=== scenario_launch  pid={pid}  track={args.track} mode={args.mode} cars={args.cars} ===")
    print("  attaching ASAP...")
    sess = None
    for _ in range(200):
        try: sess = dev.attach(pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print("  error: could not attach")
        try: proc.kill()
        except Exception: pass
        return 3

    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("ready", "err"): print("  [agent]", p.get("msg") or "ready")

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    E = scr.exports_sync

    def wait_phase(target, timeout, label):
        end = time.time() + timeout
        last = None
        while time.time() < end:
            if psutil and not psutil.pid_exists(pid):
                print(f"  game exited while waiting for {label}"); return None
            try: ph = E.phase()
            except Exception: ph = None
            if ph != last:
                print(f"    phase={ph}  (waiting for {label})"); last = ph
            if ph == target: return ph
            time.sleep(0.25)
        print(f"  TIMEOUT waiting for {label} (last phase={last})"); return None

    rc = 1
    try:
        # 1) wait for the menu (main loop live, phase 1)
        if wait_phase(1, 40, "menu (phase 1)") is None: raise SystemExit
        time.sleep(0.5)
        # 2) write the selection globals
        cfg = {"track": args.track, "mode": args.mode, "cars": args.cars, "car": args.car,
               "rule": args.rule, "team": args.team,
               "difficulty": args.difficulty, "powerups": args.powerups}
        print("  [setup]", E.setup(cfg))
        time.sleep(0.2)
        # 3) poke the state machine into load+spawn
        print("  [launch] poke DAT_00771968 = 2 ->", E.launch())
        # 4) wait for the race to be running (phase 3)
        ph3 = wait_phase(3, 40, "race running (phase 3)")
        if ph3 is None: raise SystemExit
        print("\n  *** RACE RUNNING (phase 3) ***")
        # 5) confirm a car spawned + read its record
        for _ in range(8):       # give the spawn a moment to populate the record
            time.sleep(0.5)
            ci = E.carinfo()
            if ci.get("spawnFired", 0) > 0: break
        print(f"  car spawn fired: {ci.get('spawnFired')}   grounded={ci.get('grounded')}"
              f"  airflag={ci.get('airflag')}")
        print(f"  vel={ci.get('vel')}  fwd={ci.get('pos_via_fwd')}")
        if ci.get("spawnFired", 0) > 0:
            print("\n  VERDICT: launcher reached a running race and spawned a car. [OK]")
            rc = 0
        else:
            print("\n  VERDICT: phase 3 reached but VehicleSpawnInit never fired — spawn incomplete.")
        print(f"\n  racing {args.hold}s — pulsing control 4 (confirm/accel) to skip the start intro + continue rounds...")
        t0 = time.time(); t = t0 + args.hold; n = 0
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid): print("\n  game exited."); break
            try: E.press(4, 250)            # pulse: 250ms held, ~0.35s gap -> edges for round-end prompts
            except Exception: pass
            if args.boost:
                for _ in range(4):          # re-launch a few times per tick so it stays airborne
                    try: E.boost(args.boost)
                    except Exception: pass
                    time.sleep(0.08)
            n += 1
            if n % 8 == 0:
                try:
                    ci = E.carinfo()
                    print(f"\r    +{int(time.time()-t0):>3}s  spawnFired={ci.get('spawnFired')}"
                          f"  p0.grounded={ci.get('grounded')} airflag={ci.get('airflag')}"
                          f"  vel={[round(v,1) for v in ci.get('vel',[0,0,0])]}   ", end="", flush=True)
                except Exception: pass
            time.sleep(0.6)
        print()
    except SystemExit:
        pass
    finally:
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
