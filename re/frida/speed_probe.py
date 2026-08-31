r"""speed_probe.py — measure Mashed's game-clock rate against the wall clock.

`re/analysis/QOL_PATCH_PLAN_2026-08.md:90` cites a "speed_probe" as the acceptance
method for MASHED_DECOUPLE (race clock DAT_007f0ff4 vs wall clock, phase 3), but no
script implemented it. This is that script, and it is now also the acceptance test
for MASHED_SPEED.

Method: drive into a race THROUGH THE FRONTEND with action-4 confirms, stop pressing
once the session phase at 0x00771968 reads 3, then sample the race clock and
time.perf_counter() together for N seconds and least-squares the rate.

Reference numbers from the plan: decoupled at 165 fps gives a median 2994 units/s =
0.998x real time; the clock runs in 1/3000 s units, so STOCK RATE IS 3000 units/s.
A speed multiplier m should give 3000*m.

The probe reads memory only -- no Interceptor on the clock -- so the hot-path rule in
CLAUDE.md does not apply. It spawns its own MASHED and kills ONLY that pid.
"""
import argparse
import json
import os
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"

RACE_CLOCK = 0x007F0FF4     # race clock, 1/3000 s units
SESSION_PHASE = 0x00771968  # U8: 1=menu 2=load+spawn 3=race
TICK_UNITS = 0x007F1000     # units in / N*0x32 ticks out
EFFECTS_CLOCK = 0x007F0FF0

AGENT = """
const RVA_RESOLVER = 0x00497310;
let force = {player: -1, action: -1};
let fired = 0;
let armed = false;
let tickN = 0;
rpc.exports = {
  arm: function () {
    if (armed) return 1;
    Interceptor.attach(ptr(RVA_RESOLVER), {
      onEnter(a) { const sp = this.context.esp;
                   this.p = sp.add(4).readS32(); this.c = sp.add(8).readS32(); },
      onLeave(ret) {
        if (force.action >= 0 && this.p === force.player && this.c === force.action) {
          ret.replace(ptr(0xff)); force.action = -1; fired++;
        }
      }
    });
    armed = true; return 1;
  },
  press: function (p, a) { force = {player: p, action: a}; return 1; },
  fired: function () { return fired; },
  disarm: function () { Interceptor.detachAll(); armed = false; return 1; },
  counttick: function () {
    Interceptor.attach(ptr(0x004111c0), { onEnter(){ tickN++; } });
    return 1;
  },
  ticks: function () { return tickN; },
  sample: function () {
    return {clock: ptr(%d).readS32(), phase: ptr(%d).readU8(),
            units: ptr(%d).readS32(), fx: ptr(%d).readS32(),
            acc: ptr(0x007719d4).readS32(), dt: ptr(0x007f1004).readFloat(),
            gstate: ptr(0x0063ba8c).readS32()};
  }
};
""" % (RACE_CLOCK, SESSION_PHASE, TICK_UNITS, EFFECTS_CLOCK)


def fit_rate(samples):
    """Rate of the race clock, EXCLUDING intervals where it is frozen.

    FUN_0040fc00 @0x0040fe46: the clock update is skipped entirely when
    DAT_0063ba8c == 7 (states 10..11 are gated the same way earlier in that
    function). So a naive fit over a fixed wall window under-reports by exactly
    the fraction of that window spent in a frozen state -- which is why the same
    2.0x configuration measured 1.21x .. 1.54x across runs and produced a false
    "speed-up saturates" conclusion on 2026-08-30.

    Sum only consecutive pairs where BOTH endpoints are in a counting state and
    the clock did not reset.
    """
    FROZEN = {7, 10, 11}
    dc = dt = 0.0
    frozen_pairs = 0
    for (t0, c0, g0), (t1, c1, g1) in zip(samples, samples[1:]):
        if g0 in FROZEN or g1 in FROZEN:
            frozen_pairs += 1
            continue
        if c1 < c0:            # clock reset (new round)
            continue
        dc += (c1 - c0)
        dt += (t1 - t0)
    if dt <= 0:
        return None, 0, frozen_pairs
    return dc / dt, len(samples), frozen_pairs


def run(speed, hold, fps_cap, maxpress=26):
    env = dict(os.environ)
    # mashed_re_dev.asi installs RH_ScopedInstall(FpsDiscretise, 0x00493480), a verbatim
    # port that REPLACES the quantizer and calls FUN_00493390 directly -- bypassing the
    # call site mashed_qol.asi retargets. Both .asi load, the dev one wins, and
    # MASHED_SPEED silently does nothing. Disable the dev hooks for this measurement.
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env["MASHED_QOL"] = "1"
    env["MASHED_QOL_LOG"] = "1"
    env["MASHED_FPS_LOG"] = "1"
    env["MASHED_DECOUPLE"] = "1"
    env["MASHED_WIN_POS"] = "left-bl"
    env["MASHED_FPS_CAP"] = str(fps_cap)
    env["MASHED_FPS_CAP_RACE"] = str(fps_cap)
    if speed is not None:
        env["MASHED_SPEED"] = str(speed)
    else:
        env.pop("MASHED_SPEED", None)

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  spawned pid={pid}  MASHED_SPEED={speed if speed is not None else '(unset)'}")
    out = {"speed": speed, "pid": pid, "fps_cap": fps_cap}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        dev.resume(pid)
        time.sleep(30)

        scr.exports_sync.arm()
        interval = 1.8 / (speed if speed else 1.0)
        interval = max(1.2, min(interval, 5.0))
        for k in range(maxpress):
            st = scr.exports_sync.sample()
            print(f'      press {k}: phase={st["phase"]} clock={st["clock"]} units={st["units"]}')
            if st["phase"] == 3:
                print(f"    in race after {k} confirms")
                break
            scr.exports_sync.press(0, 4)
            time.sleep(interval)
        else:
            out["error"] = "never reached phase 3"
            return out
        scr.exports_sync.disarm()          # no input at all during measurement
        time.sleep(2.0)
        scr.exports_sync.counttick()
        t_tick0, n_tick0 = time.perf_counter(), scr.exports_sync.ticks()
        time.sleep(1.0)

        samples, units_seen, t0 = [], set(), time.perf_counter()
        acc_seen, dt_seen = set(), set()
        while time.perf_counter() - t0 < hold:
            st = scr.exports_sync.sample()
            if st["phase"] != 3:
                break
            samples.append((time.perf_counter() - t0, st["clock"], st["gstate"]))
            units_seen.add(st["units"]); acc_seen.add(st["acc"]); dt_seen.add(round(st["dt"],6))
            time.sleep(0.05)
        n_tick1, t_tick1 = scr.exports_sync.ticks(), time.perf_counter()
        rate, n, frozen = fit_rate(samples)
        real_ticks = (n_tick1 - n_tick0) / (t_tick1 - t_tick0)
        out.update({"rate_units_per_s": rate, "n": n,
                    "units_seen": sorted(units_seen),
                    "acc_seen": sorted(acc_seen), "dt_seen": sorted(dt_seen),
                    "ratio_vs_stock": (rate / 3000.0) if rate else None,
                    "real_ticks_per_s": real_ticks, "frozen_pairs": frozen,
                    "clock_ticks_per_s": (rate / 50.0) if rate else None})
        print(f"    rate={rate:.1f} units/s  ratio={rate/3000.0:.3f}x  n={n}"
              if rate else f"    NO FIT (n={n})")
        return out
    finally:
        try:
            dev.kill(pid)
            print(f"  killed pid={pid}")
        except Exception as e:
            print(f"  could not kill pid={pid}: {e}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--speeds", default="1.0",
                    help="comma list of MASHED_SPEED values; 'none' = unset (control)")
    ap.add_argument("--hold", type=float, default=14.0, help="measurement seconds")
    ap.add_argument("--fps", type=int, default=60)
    ap.add_argument("--out", default=str(ROOT / "log" / "speed_probe.json"))
    a = ap.parse_args()

    results = []
    for tok in a.speeds.split(","):
        tok = tok.strip()
        if not tok:
            continue
        speed = None if tok.lower() == "none" else float(tok)
        print(f"--- MASHED_SPEED={tok}")
        results.append(run(speed, a.hold, a.fps))

    Path(a.out).write_text(json.dumps(results, indent=2), encoding="utf-8")
    print(f"\nwrote {a.out}\n")
    print("%14s %10s %8s %9s  %s" % ("MASHED_SPEED","units/s","ratio","expected","units seen"))
    for r in results:
        sp = r["speed"]
        exp = 1.0 if sp is None else sp
        rate = r.get("rate_units_per_s")
        ratio = r.get("ratio_vs_stock")
        print("%14s %10s %8s %9s  %s" % (
            sp, "n/a" if rate is None else round(rate,1),
            "n/a" if ratio is None else round(ratio,3), exp,
            r.get("units_seen") or r.get("error")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
