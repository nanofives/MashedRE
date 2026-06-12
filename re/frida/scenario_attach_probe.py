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
    addrs = sorted({a for v in cand.values() for a in v})
    print(f"{len(cand)} candidate C2 rows cite {len(addrs)} distinct DAT_* globals")

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT)
    scr.on("message", lambda m, d: None)
    scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
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
            dev.kill(pid)
        except Exception:
            pass

    race = [b if (b not in (0, None)) else a
            for a, b in zip(race_snap_a, race_snap_b)]
    val = dict(zip(addrs, zip(menu_snap, race)))
    flipped = {a: (m, rcv) for a, (m, rcv) in val.items()
               if m == 0 and rcv not in (0, None)}
    unlocked = {rva: [a for a in dats if a in flipped]
                for rva, dats in cand.items()}
    unlocked = {k: v for k, v in unlocked.items() if v}

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
