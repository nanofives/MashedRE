"""One-shot INT3 coverage census: which hooks.csv functions the ORIGINAL executes in each phase of
the canonical boot->menu and race scenarios.

    py -3.12 re/frida/coverage_census.py [--runs 3] [--menu-settle 20] [--race-hold 45] [--track 0]

Modelled on SimGolf re/tools/coverage_census.py. Mashed has no coverage shim, so the INT3 work is
done from the Frida agent with an exception handler (poll_attach_catch_crash.py style), NOT the
Interceptor: every hooks.csv entry gets a 0xCC; the first hit restores the original byte, rewinds
EIP to the entry and records the RVA. A site therefore costs one exception per phase and nothing
after it, so hot paths (FastSqrt ~2,700 calls/s at the menu) stay within the "Frida overhead on
hot paths" rule in CLAUDE.md.

Scenario (one spawned MASHED.exe per run, explicit PID, killed by PID only):
  phase menu       spawn suspended -> arm all sites -> resume; boot until DAT_00771968 == 1
                   (menu), then --menu-settle seconds at the main menu.
  phase race_load  re-arm all sites, then in the same RPC poke DAT_00771968 = 2 (the
                   scenario_launch.py warp: selection globals + FUN_0040e480 slot activation).
                   Lasts until DAT_00771968 == 3.
  phase race_run   re-arm all sites at phase 3, hold --race-hold seconds. No input is injected
                   (the scenario_launch press pulses truncate phase 3, and the input resolver
                   0x00497310 is too hot for the Interceptor); slot 0 sits parked, slots 1..3 are AI.

MASHED_RACE_DEMO / MASHED_GOTO drive the STANDALONE (exe_main.cpp:1449) and do not exist in
MASHED.exe, so the original-side race uses the scenario_launch.py warp instead. Consequence: the
frontend race-setup screens (car/track select) are not exercised by either phase.

Known blind spots, by construction:
  * the re-arm and the phase poke share one RPC, but the menu frame in flight finishes after it, so
    at most one menu frame of functions can be attributed to race_load;
  * functions that only run when the player drives, finishes a race, or opens a menu other than
    the main menu are unreached here.

Writes re/coverage/reach.tsv (one row per hooks.csv RVA, spelled as in hooks.csv):
  rva, reached_menu, reached_race, reached_race_load, reached_race_run, runs_menu, runs_race, va
reached_* = 1 when the site was hit in a strict majority of runs. runs_* = "k/N".
Per-run raw hit lists go to log/coverage/census_<stamp>_run<i>.json.
"""
from __future__ import annotations

import argparse
import collections
import csv
import hashlib
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

try:
    import psutil
except ImportError:
    sys.exit("psutil required: py -3.12 -m pip install psutil")

ROOT = Path(__file__).resolve().parent.parent.parent
IMG = 0x00400000
ANCHOR_SHA256 = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
PHASES = ("menu", "race_load", "race_run")


def game_root() -> Path:
    """original/ lives only in the main checkout (worktrees never junction it)."""
    if os.environ.get("MASHED_ROOT"):
        return Path(os.environ["MASHED_ROOT"])
    if (ROOT / "original" / "MASHED.exe").exists():
        return ROOT
    common = subprocess.run(["git", "-C", str(ROOT), "rev-parse", "--git-common-dir"],
                            capture_output=True, text=True).stdout.strip()
    return Path(common).resolve().parent


AGENT = r'''
'use strict';
const IMG = 0x400000;
const PHASE_VA = 0x00771968;   // session-phase U8: 1=menu 2=load+spawn 3=race (scenario_launch.py)
const ACTIVATE = 0x0040e480;   // FUN_0040e480(slot,val) cdecl, per-slot vehicle activation
let base = null;
const sites = new Map();       // va -> {orig, armed}
let cur = 0;
const hits = [[], [], []];
let clobbered = 0, writeErr = 0;
function ga(va){ return base.add(va - IMG); }

function writeByte(p, b){
  try { p.writeU8(b); }
  catch (e) { writeErr++; Memory.patchCode(p, 1, function (c) { c.writeU8(b); }); }
}

Process.setExceptionHandler(function (d) {
  if (d.type !== 'breakpoint' || base === null) return false;
  let va = d.address.sub(base).toUInt32() + IMG;
  let s = sites.get(va);
  if (s === undefined) { va -= 1; s = sites.get(va); }   // tolerate EIP-after-INT3 reporting
  if (s === undefined) return false;
  const p = ga(va);
  if (s.armed) { writeByte(p, s.orig); s.armed = false; hits[cur].push(va); }
  d.context.pc = p;   // re-execute the restored entry instruction
  return true;
});

function armAll(phase){
  cur = phase;
  let n = 0;
  sites.forEach(function (s, va) {
    const p = ga(va);
    const b = p.readU8();
    if (s.armed) { if (b !== 0xcc) { clobbered++; s.armed = false; } return; }
    s.orig = b; writeByte(p, 0xcc); s.armed = true; n++;
  });
  return n;
}

rpc.exports = {
  init: function (vas) {
    const m = Process.getModuleByName('MASHED.exe');
    base = m.base;
    Process.enumerateRanges({ protection: 'r-x', coalesce: true }).forEach(function (r) {
      if (r.base.compare(m.base) >= 0 && r.base.compare(m.base.add(m.size)) < 0)
        Memory.protect(r.base, r.size, 'rwx');
    });
    vas.forEach(function (va) { sites.set(va, { orig: 0, armed: false }); });
    return base.toString();
  },
  arm: function (phase) { return armAll(phase); },
  armAndLaunch: function (phase) { const n = armAll(phase); ga(PHASE_VA).writeU8(2); return n; },
  phase: function () { try { return ga(PHASE_VA).readU8(); } catch (e) { return -1; } },
  // scenario_launch.py setup(): same globals, same FUN_0040e480 calls. The activation fn is a
  // census site, so it is disarmed for the duration of OUR call (not recorded as a hit).
  setup: function (cfg) {
    ga(0x0063ba7c).writeS32(cfg.track);   // TRACK_ENG
    ga(0x0067f17c).writeS32(cfg.track);   // TRACK_MENU
    ga(0x0067e9fc).writeS32(cfg.mode);    // MODE
    ga(0x007f0fd0).writeS32(cfg.rule);    // RULE
    ga(0x0067ea98).writeS32(cfg.car);     // CAR_P0
    ga(0x0067ea64).writeS32(cfg.team);    // TEAM
    const s = sites.get(ACTIVATE), p = ga(ACTIVATE);
    const wasArmed = s !== undefined && s.armed;
    if (wasArmed) { writeByte(p, s.orig); s.armed = false; }
    const e480 = new NativeFunction(p, 'void', ['int', 'int'], 'mscdecl');
    for (let i = 0; i < 4; i++) e480(i, i === 0 ? 1 : (i < cfg.cars ? 2 : 0));
    if (wasArmed) { writeByte(p, 0xcc); s.armed = true; }
    return 'track=' + cfg.track + ' mode=' + cfg.mode + ' cars=' + cfg.cars;
  },
  hits: function (phase) { return hits[phase]; },
  stats: function () {
    let armed = 0; sites.forEach(function (s) { if (s.armed) armed++; });
    return { armed: armed, clobbered: clobbered, writeErr: writeErr,
             hits: hits.map(function (h) { return h.length; }) };
  },
};
'''


def load_sites() -> tuple[list[dict], dict[int, list[str]]]:
    rows = [r for r in csv.DictReader(open(ROOT / "hooks.csv", newline="", encoding="utf-8-sig"))
            if r["rva"] and not r["rva"].startswith("#")]
    by_va: dict[int, list[str]] = collections.defaultdict(list)
    for r in rows:
        v = int(r["rva"], 16)
        by_va[v + IMG if v < IMG else v].append(r["rva"])   # 29 rows are stored base-relative
    return rows, by_va


def text_ranges(exe: Path) -> list[tuple[int, int]]:
    import pefile
    pe = pefile.PE(str(exe), fast_load=True)
    return [(IMG + s.VirtualAddress, IMG + s.VirtualAddress + s.Misc_VirtualSize)
            for s in pe.sections if s.Characteristics & 0x20000000]   # IMAGE_SCN_MEM_EXECUTE


def one_run(exe: Path, vas: list[int], a, tag: str) -> dict:
    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = "60"
    env.setdefault("MASHED_MUTE", "1")
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"   # stock original: no inline-JMP hooks over the INT3 sites
    dev = frida.get_local_device()
    pid = dev.spawn([str(exe)], cwd=str(exe.parent), env=env)
    print(f"  [{tag}] spawned MASHED.exe pid={pid} (suspended)", flush=True)
    t0 = time.time()
    out = {"pid": pid, "phases": {}, "ok": False}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: print(f"  [{tag}] agent:", m.get("description") or m)
               if m.get("type") == "error" else None)
        scr.load()
        E = scr.exports_sync
        E.init(vas)
        print(f"  [{tag}] armed {E.arm(0)} sites; resuming", flush=True)
        dev.resume(pid)

        def wait_phase(target: int, timeout: float) -> bool:
            end = time.time() + timeout
            while time.time() < end:
                if not psutil.pid_exists(pid):
                    print(f"  [{tag}] game exited while waiting for phase {target}")
                    return False
                if E.phase() == target:
                    return True
                time.sleep(0.1)
            print(f"  [{tag}] TIMEOUT waiting for phase {target} (last {E.phase()})")
            return False

        if not wait_phase(1, a.boot_timeout):
            return out
        print(f"  [{tag}] menu at +{time.time() - t0:.1f}s; settling {a.menu_settle}s", flush=True)
        time.sleep(a.menu_settle)
        print(f"  [{tag}] setup:", E.setup({"track": a.track, "mode": a.mode, "cars": a.cars,
                                             "car": 0, "rule": 0, "team": 0}))
        print(f"  [{tag}] re-armed {E.arm_and_launch(1)} sites + DAT_00771968=2", flush=True)
        # Snapshot a phase only AFTER the switch: arm() moves the recording cursor under the
        # agent's JS lock, so no hit lands between the two lists.
        out["phases"]["menu"] = sorted(E.hits(0))
        if not wait_phase(3, a.load_timeout):
            return out
        rearmed = E.arm(2)
        out["phases"]["race_load"] = sorted(E.hits(1))
        print(f"  [{tag}] race running at +{time.time() - t0:.1f}s; re-armed {rearmed}; "
              f"holding {a.race_hold}s", flush=True)
        time.sleep(a.race_hold)
        if not psutil.pid_exists(pid):
            print(f"  [{tag}] game exited during the race hold")
            return out
        out["stats"] = E.stats()
        out["phases"]["race_run"] = sorted(E.hits(2))
        out["ok"] = True
        print(f"  [{tag}] stats {out['stats']}", flush=True)
        return out
    finally:
        try:
            dev.kill(pid)          # the PID this run spawned, never by name
        except Exception:
            pass


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--runs", type=int, default=3)
    ap.add_argument("--menu-settle", type=float, default=20.0)
    ap.add_argument("--race-hold", type=float, default=45.0)
    ap.add_argument("--boot-timeout", type=float, default=90.0)
    ap.add_argument("--load-timeout", type=float, default=90.0)
    ap.add_argument("--track", type=int, default=0)
    ap.add_argument("--mode", type=int, default=10, help="game mode (10 = QuickRace)")
    ap.add_argument("--cars", type=int, default=4, help="active slots: 0 = player, rest AI")
    ap.add_argument("--out", type=Path, default=ROOT / "re" / "coverage" / "reach.tsv")
    a = ap.parse_args()

    groot = game_root()
    exe = groot / "original" / "MASHED.exe"
    unpatched = groot / "original" / "MASHED.exe.unpatched"
    sha = hashlib.sha256(unpatched.read_bytes()).hexdigest().upper()
    if sha != ANCHOR_SHA256:
        print(f"error: {unpatched} SHA-256 {sha} != anchor; RVAs invalid, re-anchor first")
        return 2

    rows, by_va = load_sites()
    ranges = text_ranges(unpatched)
    bad = [hex(v) for v in by_va if not any(lo <= v < hi for lo, hi in ranges)]
    if bad:
        print(f"error: {len(bad)} sites outside executable sections: {bad[:10]}")
        return 2
    vas = sorted(by_va)
    print(f"census: {len(rows)} hooks.csv rows -> {len(vas)} distinct entry VAs; exe={exe}")

    stamp = time.strftime("%Y%m%d_%H%M%S")
    logdir = ROOT / "log" / "coverage"
    logdir.mkdir(parents=True, exist_ok=True)
    runs = []
    for i in range(a.runs):
        r = one_run(exe, vas, a, f"run{i + 1}")
        (logdir / f"census_{stamp}_run{i + 1}.json").write_text(json.dumps(r))
        if not r["ok"]:
            print(f"error: run {i + 1} did not complete; aborting (no reach.tsv written)")
            return 3
        runs.append({ph: set(v) for ph, v in r["phases"].items()})
        print(f"run {i + 1}: " + "  ".join(f"{ph}={len(runs[-1][ph])}" for ph in PHASES), flush=True)

    # Stability: per scenario, |union - intersection| / |union| across runs.
    print("\nrun agreement (|union - intersection| / |union|):")
    worst = 0.0
    for name, phs in (("menu", ("menu",)), ("race", ("race_load", "race_run")),
                      *((ph, (ph,)) for ph in PHASES[1:])):
        sets = [set().union(*(r[p] for p in phs)) for r in runs]
        uni, inter = set().union(*sets), set.intersection(*sets)
        frac = (len(uni) - len(inter)) / len(uni) if uni else 0.0
        if name in ("menu", "race"):
            worst = max(worst, frac)
        print(f"  {name:10} union={len(uni):5}  intersection={len(inter):5}  disagree={100 * frac:.2f}%")
    print(f"  worst scenario disagreement {100 * worst:.2f}% -> {'PASS' if worst <= 0.01 else 'FAIL'} (<= 1%)")

    n = len(runs)
    maj = n // 2 + 1
    a.out.parent.mkdir(parents=True, exist_ok=True)
    with open(a.out, "w", newline="") as f:
        f.write("rva\treached_menu\treached_race\treached_race_load\treached_race_run"
                "\truns_menu\truns_race\tva\n")
        for va in vas:
            k = {ph: sum(va in r[ph] for r in runs) for ph in PHASES}
            k_race = sum((va in r["race_load"]) or (va in r["race_run"]) for r in runs)
            for rva in by_va[va]:
                f.write(f"{rva}\t{int(k['menu'] >= maj)}\t{int(k_race >= maj)}"
                        f"\t{int(k['race_load'] >= maj)}\t{int(k['race_run'] >= maj)}"
                        f"\t{k['menu']}/{n}\t{k_race}/{n}\t{va:08x}\n")
    reached = sum(1 for va in vas if any(sum(va in r[ph] for r in runs) >= maj for ph in PHASES))
    print(f"\nreached (majority of {n} runs): {reached}/{len(vas)} entry VAs  -> {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
