# Scenario-attach lane probe (2026-06-12): quantify how much of the C2 pool's
# live state is populated by attaching at RACE state instead of menu.
#
# Reads hooks.csv, selects the scenario-attach candidates from the C2 gate
# audit (C2 + first-party + state-rich subsystem + notes cite DAT_* globals),
# extracts every cited DAT_ address, then drives the ORIGINAL into a Quick
# Battle race (race_refs.py recipe: FUN_00497310 input-override, depth/phase
# polling, setsel cursor write) and snapshots all addresses at BOTH states.
# A candidate is "unlocked" by the lane when >=1 of its globals goes
# zero-at-menu -> non-zero-in-race.
#
# Usage: py -3.12 re/frida/scenario_attach_probe.py
import csv
import json
import os
import re
import shutil
import subprocess
import sys
import time
from collections import defaultdict
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
HOOKS_CSV = ROOT / "hooks.csv"
OUT_JSON = ROOT / "log" / "scenario_attach_probe.json"

RACE_SUBSYSTEMS = {"gameplay", "vehicle", "ai", "camera", "race", "hud", "particle"}
DAT_RX = re.compile(r"DAT_(00[4-9a-fA-F][0-9a-fA-F]{4,5})")

# Stride-aware probe window (2026-06-16): read this many dwords past each cited
# base. The single-dword read undercounted per-slot arrays whose base element
# stays 0 in-race while later slots (stride 0x14..0x4e) populate — see the lane
# doc's caveat. A flip anywhere in the window unlocks the candidate; the vet
# step confirms the function actually reads the flipped offset (windows can
# spill into an adjacent global = candidate-grade, not proof).
WINDOW_DWORDS = 24

# Shared nav agent (single copy — also used by run_diff.py scenario:'race').
AGENT = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")


def candidates_and_globals():
    with open(HOOKS_CSV, newline="", encoding="utf-8", errors="replace") as f:
        rows = list(csv.DictReader(f))
    cand = {}
    for r in rows:
        if (r.get("confidence") or "").strip() != "C2":
            continue
        sub = (r.get("subsystem") or "").strip()
        if "third-party" in sub or sub not in RACE_SUBSYSTEMS:
            continue
        dats = sorted({int(m, 16) for m in DAT_RX.findall(r.get("notes") or "")})
        dats = [a for a in dats if 0x00400000 <= a < 0x00A00000]
        if dats:
            cand[r["rva"]] = dats
    return cand


def main():
    cand = candidates_and_globals()

    def windowed(base):
        return [base + 4 * k for k in range(WINDOW_DWORDS)]
    addrs = sorted({wa for v in cand.values() for a in v for wa in windowed(a)})
    print(f"{len(cand)} candidate C2 rows cite "
          f"{len({a for v in cand.values() for a in v})} DAT_* globals -> "
          f"{len(addrs)} probe addresses (window={WINDOW_DWORDS} dwords)")

    # Spawn EXACTLY like run_diff.py (subprocess.Popen + attach, NOT frida
    # dev.spawn): a Frida-controlled suspended spawn does not get the WIN98RTM
    # AppCompat layer applied the same way and crashes MASHED at boot (2026-06-16).
    # Enforce the windowed videocfg first so we don't fight fullscreen.
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists():
        shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = {**os.environ, "MASHED_RE_NO_AUTO_HOOK": "1"}
    _CREATE_NEW_PROCESS_GROUP = 0x00000200
    _DETACHED_PROCESS = 0x00000008
    dev = frida.get_local_device()

    def spawn_attach():
        # Attach FAST (0.2 s) like run_diff.py — suspend MASHED before any
        # residual boot-crash window. Intermittent boot AVs are retried by
        # respawning (post-EMULATEHEAP-fix boot is ~stable but not 100%).
        p = subprocess.Popen(
            [str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=_CREATE_NEW_PROCESS_GROUP | _DETACHED_PROCESS)
        time.sleep(0.2)
        try:
            return p, dev.attach(p.pid)
        except Exception as e:
            try: p.kill()
            except Exception: pass
            return p, None

    proc = sess = None
    for attempt in range(4):
        proc, sess = spawn_attach()
        if sess is not None:
            break
        print(f"  spawn/attach attempt {attempt+1} failed (boot crash window) — retrying")
        time.sleep(1.0)
    if sess is None:
        print("attach failed after 4 spawn retries (MASHED boot unstable)")
        return 4
    scr = sess.create_script(AGENT)
    scr.on("message", lambda m, d: None)
    scr.load()
    scr.exports_sync.init()
    E = scr.exports_sync

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred():
                return True
            time.sleep(0.1)
        print(f"timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False

    def press(c, ms=180):
        E.press(c, ms)
        time.sleep(ms / 1000.0 + 0.3)

    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target:
                return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0, f"d{target}"):
                return True
        return E.depth() >= target

    try:
        print("booting to menu...")
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 25, "title"):
            return 2
        time.sleep(1.5)
        menu_snap = E.snap(addrs)
        print(f"menu snapshot taken ({sum(1 for v in menu_snap if v == 0)} of "
              f"{len(addrs)} globals are zero at menu)")

        # race_refs.py drive recipe (verified 2026-06-10): Quick Battle, Arctic.
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4)
        confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3:
                break
            press(4); time.sleep(1.5)
        in_race = E.phase() != 3
        print(f"in race? phase={E.phase()} ({'YES' if in_race else 'NO'})")
        if not in_race:
            return 3
        time.sleep(4)                    # countdown + first seconds of the round
        race_snap_a = E.snap(addrs)
        time.sleep(8)                    # mid-round, scores/laps ticking
        race_snap_b = E.snap(addrs)
    finally:
        try:
            proc.kill()
        except Exception:
            pass

    race = [b if (b not in (0, None)) else a
            for a, b in zip(race_snap_a, race_snap_b)]
    val = dict(zip(addrs, zip(menu_snap, race)))
    flipped = {a: (m, rcv) for a, (m, rcv) in val.items()
               if m == 0 and rcv not in (0, None)}
    flipset = set(flipped)
    unlocked = {}
    for rva, bases in cand.items():
        hits = sorted({base + off for base in bases
                       for off in range(0, WINDOW_DWORDS * 4, 4)
                       if (base + off) in flipset})
        if hits:
            unlocked[rva] = hits

    print(f"\nglobals zero at menu:            {sum(1 for m, _ in val.values() if m == 0)}")
    print(f"of those, NON-ZERO in race:      {len(flipped)}")
    print(f"candidate rows with >=1 flip:    {len(unlocked)} / {len(cand)}")

    OUT_JSON.parent.mkdir(exist_ok=True)
    OUT_JSON.write_text(json.dumps({
        "candidates": len(cand), "globals": len(addrs),
        "flipped": {f"0x{a:08x}": [m, r] for a, (m, r) in val.items() if a in flipped},
        "unlocked_rvas": {k: [f"0x{a:08x}" for a in v] for k, v in unlocked.items()},
    }, indent=1), encoding="utf-8")
    print(f"detail -> {OUT_JSON}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
