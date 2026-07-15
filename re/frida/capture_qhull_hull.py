# B5b — capture the ORIGINAL's qhull hull-build I/O at track load (bit-identity oracle).
#
# Warps the stock MASHED.exe into a race (same mechanism as scenario_launch.py:
# setup() writes the selection globals, launch() pokes DAT_00771968=2, the engine
# loads the track + spawns cars). The 4-body collision-hull build FUN_0047d3c0 runs
# once during physics-world init (gated DAT_0086caa0==0x7b) and calls the qhull bridge
# FUN_0057ca30 four times. That bridge is LOAD-TIME (not per-frame) so an Interceptor
# on it is safe (CLAUDE.md hot-path rule N/A here).
#
# For each of the 4 calls we capture:
#   numpoints (arg0), the point cloud (arg1 -> numpoints*3 float32), and the returned
#   packed collision-hull slab (retval -> slab[0]=total size -> that many bytes).
# Saved to log/b5b_qhull_capture/body<k>.{cloud.bin, slab.bin, meta.json}.
#
# The offline replay (b5b_qhull_selftest.exe --replay body<k>.cloud.bin) then rebuilds
# the hull with the VENDORED qhull + ported bridge; a byte/crc diff vs slab.bin is the
# B5b C4 bit-identity acceptance.
#
# Spawn+attach (NEVER frida.spawn). Kills ONLY the pid it launches.
# Usage: py -3.12 re/frida/capture_qhull_hull.py [--track 0] [--mode 10] [--cars 4] [--hold 20]
import os, sys, time, json, base64, argparse, subprocess
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
GAME_ROOT = Path(os.environ.get("MASHED_ROOT", ROOT))
EXE = GAME_ROOT / "original" / "MASHED.exe"
OUTDIR = ROOT / "log" / "b5b_qhull_capture"

AGENT = r'''
'use strict';
const IMG = 0x400000;
let M = null;
function modBase(){ if(!M) M = Process.findModuleByName('MASHED.exe'); return M ? M.base : null; }
function ga(addr){ const b = modBase(); return b ? b.add(addr - IMG) : null; }

const PHASE      = 0x00771968;
const TRACK_ENG  = 0x0063ba7c, TRACK_MENU = 0x0067f17c;
const MODE       = 0x0067e9fc, RULE = 0x007f0fd0, CAR_P0 = 0x0067ea98;
const DIFFICULTY = 0x0067ea7c, POWERUPS = 0x0067ea80, TEAM = 0x0067ea64;
const ACTIVATE   = 0x0040e480;
const QHULL_RVA  = 0x0057ca30;   // FUN_0057ca30 RwpQHullWrapper (the bridge)

const caps = [];        // one entry per FUN_0057ca30 call
let qhullArmed = false;
function armQhull(){
  if (qhullArmed) return 'already';
  try {
    Interceptor.attach(ga(QHULL_RVA), {
      onEnter(){
        const sp = this.context.esp;             // cdecl: [esp+4]=numpoints [esp+8]=points ptr
        this.n   = sp.add(4).readS32();
        this.pts = sp.add(8).readPointer();
      },
      onLeave(ret){
        let cloud = null, slab = null, size = 0;
        try {
          if (this.n > 0 && !this.pts.isNull())
            cloud = this.pts.readByteArray(this.n * 3 * 4);   // n*3 float32
        } catch(e){}
        try {
          const rp = ptr(ret.toInt32());
          if (!rp.isNull()) { size = rp.readU32(); if (size > 0 && size < 0x400000) slab = rp.readByteArray(size); }
        } catch(e){}
        caps.push({ n: this.n, size: size,
                    cloud: cloud ? Array.from(new Uint8Array(cloud)) : null,
                    slab:  slab  ? Array.from(new Uint8Array(slab))  : null });
        send({kind:'qhull', idx: caps.length-1, n: this.n, size: size,
              haveCloud: !!cloud, haveSlab: !!slab});
      }
    });
    qhullArmed = true; return 'armed FUN_0057ca30';
  } catch(e){ return 'ERR '+e; }
}
armQhull();   // arm immediately at load, before the race triggers the build

rpc.exports = {
  ready: function(){ return modBase() ? 1 : 0; },
  phase: function(){ try { return ga(PHASE).readU8(); } catch(e){ return -1; } },
  count: function(){ return caps.length; },
  grab: function(i){ return JSON.stringify(caps[i] || null); },
  setup: function(cfg){
    try {
      ga(TRACK_ENG).writeS32(cfg.track); ga(TRACK_MENU).writeS32(cfg.track);
      ga(MODE).writeS32(cfg.mode); ga(RULE).writeS32(cfg.rule);
      ga(CAR_P0).writeS32(cfg.car); ga(TEAM).writeS32(cfg.team);
      if (cfg.difficulty >= 0) ga(DIFFICULTY).writeS32(cfg.difficulty);
      if (cfg.powerups   >= 0) ga(POWERUPS).writeS32(cfg.powerups);
      const e480 = new NativeFunction(ga(ACTIVATE), 'void', ['int','int'], 'mscdecl');
      for (let s = 0; s < 4; s++) e480(s, s === 0 ? 1 : (s < cfg.cars ? 2 : 0));
      return 'setup track='+cfg.track+' mode='+cfg.mode+' cars='+cfg.cars;
    } catch(e){ return 'ERR '+e; }
  },
  launch: function(){ try { ga(PHASE).writeU8(2); return 1; } catch(e){ return 'ERR '+e; } }
};
send({kind:'ready'});
'''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, default=0)
    ap.add_argument("--mode", type=int, default=10)
    ap.add_argument("--cars", type=int, default=4)
    ap.add_argument("--car", type=int, default=0)
    ap.add_argument("--rule", type=int, default=0)
    ap.add_argument("--team", type=int, default=0)
    ap.add_argument("--powerups", type=int, default=-1)
    ap.add_argument("--difficulty", type=int, default=-1)
    ap.add_argument("--fps", default="60")
    ap.add_argument("--hold", type=int, default=15)
    args = ap.parse_args()

    if not EXE.exists():
        print(f"error: {EXE} not found"); return 2
    OUTDIR.mkdir(parents=True, exist_ok=True)

    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"     # stock original, no installed hooks

    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    pid = proc.pid
    print(f"=== capture_qhull_hull  pid={pid}  track={args.track} mode={args.mode} cars={args.cars} ===")

    sess = None
    for _ in range(200):
        try: sess = dev.attach(pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print("  error: could not attach");
        try: proc.kill()
        except Exception: pass
        return 3

    got = []
    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "qhull":
            print(f"  [qhull] call#{p['idx']} n={p['n']} size={p['size']} "
                  f"cloud={p['haveCloud']} slab={p['haveSlab']}")
            got.append(p['idx'])
        elif p.get("kind") == "ready":
            print("  [agent] ready")

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    E = scr.exports_sync

    def wait_phase(target, timeout, label):
        end = time.time() + timeout; last = None
        while time.time() < end:
            if psutil and not psutil.pid_exists(pid):
                print(f"  game exited while waiting for {label}"); return None
            try: ph = E.phase()
            except Exception: ph = None
            if ph != last: print(f"    phase={ph}  (waiting for {label})"); last = ph
            if ph == target: return ph
            time.sleep(0.25)
        print(f"  TIMEOUT waiting for {label} (last phase={last})"); return None

    rc = 1
    try:
        if wait_phase(1, 45, "menu (phase 1)") is None: raise SystemExit
        time.sleep(0.5)
        cfg = {"track": args.track, "mode": args.mode, "cars": args.cars, "car": args.car,
               "rule": args.rule, "team": args.team,
               "difficulty": args.difficulty, "powerups": args.powerups}
        print("  [setup]", E.setup(cfg)); time.sleep(0.2)
        print("  [launch] DAT_00771968=2 ->", E.launch())
        # the 4-body build fires on the first physics tick after phase 3. Track load
        # (phase 2 -> 3) can be slow; poll longer and capture as soon as calls arrive.
        end = time.time() + 120
        last = None
        while time.time() < end:
            if psutil and not psutil.pid_exists(pid):
                print("  game exited during load"); break
            try: ph = E.phase()
            except Exception: ph = None
            if ph != last: print(f"    phase={ph}"); last = ph
            if E.count() >= 4: break
            time.sleep(0.5)
        # hold a bit more for straggler build calls
        for _ in range(int(args.hold)):
            time.sleep(1.0)
            if E.count() >= 4: break
        n = E.count()
        print(f"\n  captured {n} FUN_0057ca30 call(s)")
        saved = 0
        for i in range(n):
            rec = json.loads(E.grab(i))
            if not rec: continue
            meta = {"call": i, "n": rec["n"], "size": rec["size"],
                    "haveCloud": rec["cloud"] is not None, "haveSlab": rec["slab"] is not None}
            (OUTDIR / f"body{i}.meta.json").write_text(json.dumps(meta, indent=2))
            if rec["cloud"] is not None:
                # cloud.bin format matching b5b_qhull_selftest --replay: [int32 n][n*3 float32]
                import struct
                cb = bytes(rec["cloud"])
                with open(OUTDIR / f"body{i}.cloud.bin", "wb") as f:
                    f.write(struct.pack("<i", rec["n"])); f.write(cb)
            if rec["slab"] is not None:
                (OUTDIR / f"body{i}.slab.bin").write_bytes(bytes(rec["slab"]))
                saved += 1
            print(f"    body{i}: n={rec['n']} size={rec['size']} saved")
        print(f"\n  wrote {saved} slab(s) + clouds to {OUTDIR}")
        rc = 0 if n > 0 else 1
    except SystemExit:
        pass
    finally:
        try: scr.unload()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        try: proc.kill(); print(f"  killed pid={pid}")
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
