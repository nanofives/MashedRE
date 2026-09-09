#!/usr/bin/env python3
"""dualinstall_audit.py -- classify HookSystem DUAL-INSTALL refusals and name the dead copy.

    py -3.12 re/tools/dualinstall_audit.py [--log original/hook_dual_install.log]
                                           [--tail N]   # only the last N lines (one run)

HookSystem refuses to install a second hook over an RVA that already carries an E9, because
saving our own JMP as the "original prologue" would permanently corrupt the restore path.
Each refusal names a WINNER (whatever got there first) and a LOSER (refused, therefore DEAD
CODE for that run). This tool separates the two very different reasons a refusal happens:

  FOREIGN-E9    the RVA was already patched by something that is NOT our hook registry --
                a binary boot patch (scripts/patch_mashed_*.py) or another loaded .asi.
                Our hook is dead, but there is no duplicate implementation in our tree.
  DUP-REGISTRY  two of OUR OWN registered implementations claim the same RVA. One is dead
                code. This is the U-9065 / duplicate-rva-implementations-drift class.

For DUP-REGISTRY it reports which source file each copy lives in and, crucially, whether
hooks.csv's `file` column points at the WINNER or at the dead LOSER -- a tracker row that
describes the dead copy is worse than no row.
"""
import argparse
import collections
import csv
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from matchdiff_sweep import load_registrations, ROOT  # noqa: E402

LINE = re.compile(
    r"DUAL-INSTALL REFUSED at 0x([0-9A-Fa-f]{8}): already hooked by '([^']*)'; "
    r"NOT installing '([^']*)'")

# The ten documented binary boot patches (CLAUDE.md "Runtime state"). An E9 at one of these
# is expected and is not a tooling defect.
BOOT_PATCHES = {
    0x00498dbc: "show_windowed", 0x005bc750: "skip_audio_com", 0x004951f0: "skip_selector",
    0x004951aa: "skip_controller_dialog", 0x00498bc0: "fix_camera_res",
    0x00498bd0: "fix_camera_res", 0x004963e7: "disable_log", 0x00496400: "disable_log",
    0x00496490: "disable_log", 0x004a4541: "fix_fopen", 0x00495870: "fix_joypad",
    0x0040283f: "skip_movies", 0x004996d3: "no_focus_pause",
}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--log", default=str(ROOT / "original" / "hook_dual_install.log"))
    ap.add_argument("--tail", type=int, default=0,
                    help="consider only the last N lines (isolate ONE run; the log is append-mode "
                         "and accumulates across months of differing hook configurations)")
    a = ap.parse_args()

    lines = Path(a.log).read_text(encoding="utf-8", errors="replace").splitlines()
    if a.tail:
        lines = lines[-a.tail:]

    regs, _ = load_registrations()
    sym_file = {}
    for (sym, rva), (cpp, installed) in regs.items():
        sym_file.setdefault(sym, []).append((cpp.name, rva, installed))

    hooks = {}
    with open(ROOT / "hooks.csv", encoding="utf-8", errors="replace") as fh:
        for r in csv.DictReader(fh):
            try:
                hooks[int(r["rva"], 16)] = r
            except (ValueError, TypeError):
                pass

    seen, rows = set(), []
    for ln in lines:
        m = LINE.search(ln)
        if not m:
            continue
        rva = int(m.group(1), 16)
        winner, loser = m.group(2), m.group(3)
        if (rva, winner, loser) in seen:
            continue
        seen.add((rva, winner, loser))
        foreign = "not in registry" in winner
        klass = "FOREIGN-E9" if foreign else "DUP-REGISTRY"
        why = BOOT_PATCHES.get(rva, "") if foreign else ""
        h = hooks.get(rva)
        csv_file = (h["file"] if h else "")
        wfiles = ",".join(f for f, _r, _i in sym_file.get(winner, []))
        lfiles = ",".join(f for f, _r, _i in sym_file.get(loser, []))
        # does hooks.csv point at the dead copy?
        points_at = ""
        if klass == "DUP-REGISTRY" and csv_file.endswith(".cpp"):
            base = Path(csv_file).name
            if base in lfiles and base not in wfiles:
                points_at = "TRACKER-POINTS-AT-DEAD-COPY"
            elif base in wfiles:
                points_at = "tracker-ok"
            else:
                points_at = "tracker-elsewhere"
        rows.append(dict(rva="%08x" % rva, klass=klass, why=why, winner=winner, loser=loser,
                         winner_file=wfiles, loser_file=lfiles,
                         conf=(h["confidence"] if h else ""), csv_file=csv_file,
                         tracker=points_at))

    tally = collections.Counter(r["klass"] for r in rows)
    print("refusals considered: %d  (%s)" % (len(rows), "last %d lines" % a.tail if a.tail else "whole log"))
    for k, v in tally.most_common():
        print("  %-13s %d" % (k, v))

    print("\n--- FOREIGN-E9 (no duplicate in our tree; our hook is dead for that run) ---")
    for r in rows:
        if r["klass"] != "FOREIGN-E9":
            continue
        tag = ("boot patch: " + r["why"]) if r["why"] else "source UNKNOWN (another .asi?)"
        print("  %s  loser=%-28s %-14s [%s]" % (r["rva"], r["loser"][:28], r["conf"], tag))

    print("\n--- DUP-REGISTRY (two of OUR implementations; the loser is dead code) ---")
    for r in rows:
        if r["klass"] != "DUP-REGISTRY":
            continue
        flag = "  <<< " + r["tracker"] if r["tracker"] == "TRACKER-POINTS-AT-DEAD-COPY" else ""
        print("  %s %-4s WINS %-26s (%s)" % (r["rva"], r["conf"], r["winner"][:26], r["winner_file"]))
        print("           DEAD %-26s (%s)%s" % (r["loser"][:26], r["loser_file"], flag))

    bad = sum(1 for r in rows if r["tracker"] == "TRACKER-POINTS-AT-DEAD-COPY")
    print("\nhooks.csv rows whose `file` names the DEAD copy: %d" % bad)
    return 0


if __name__ == "__main__":
    sys.exit(main())
