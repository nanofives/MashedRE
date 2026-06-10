#!/usr/bin/env python3
"""R1 Phase A: demote suspect-C4 rows with no installable reimpl (2026-06-09).

Ground truth gathered 2026-06-09 by scanning mashedmod/src/mashed_re/**/*.cpp
for active (non-comment-prefixed) RH_ScopedInstall sites: of the 101
C4-EVIDENCE-SUSPECT rows, 16 have NO active install. Per re/CONFIDENCE.md the
C3 gate requires "hooked through RH_ScopedInstall and runtime-toggleable", so
C4 is unreachable for these rows regardless of observation evidence.
Precedents: 2026-06-05 (22 demotions, scripts/reclassify_c4_revalidate_demote.py)
and 2026-06-07 (disabled-install fails the C3 hook gate).

Two sub-populations, distinct markers:
  ABSENT  (11): no RH_ScopedInstall line at all in source.
  DISABLED (5): an RH_ScopedInstall line exists but is comment-prefixed
                (MASS-DISABLED era, never re-validated/re-enabled).

Also rewrites the row's C4-EVIDENCE-SUSPECT: token to
C4-EVIDENCE-SUSPECT-CLEARED(2026-06-09,demoted-r1a): so the open-population
grep converges (101 -> 85). Idempotent; refuses unexpected confidence values.
"""
import csv
import io
import sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"

ABSENT = [
    "00495120", "00495270", "00498c00", "004c2d90", "004c9eb0", "004c9f50",
    "004c9f60", "004cbc60", "004cbc70", "004cbc80", "004cc820",
]
DISABLED = ["00428590", "004c5a00", "004c5ae0", "004c5b50", "004c5c80"]

MARK_ABSENT = (
    "| DEMOTED C4->C2 (R1-A re-validation 2026-06-09): NOT a registered hook — "
    "no RH_ScopedInstall site in mashedmod/src at all -> fails C3 rubric "
    "'reimplementation written + hooked'. Prior C4 was in-window canonical "
    "observation of the ORIGINAL. Decomp-read analysis note exists -> C2. "
    "Needs a reimpl to re-earn C3/C4.")
MARK_DISABLED = (
    "| DEMOTED C4->C2 (R1-A re-validation 2026-06-09): reimpl source exists "
    "but its RH_ScopedInstall is comment-disabled (MASS-DISABLED era, never "
    "re-validated) -> fails C3 rubric 'hooked + runtime-toggleable' (per "
    "2026-06-07 precedent: disabled-install fails the C3 hook gate). "
    "Re-enable via clean diff-original to re-earn C3/C4.")


def parse(line):
    return next(csv.reader([line.rstrip("\n")]))


def emit(fields):
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(fields)
    return b.getvalue()


def main():
    hooks = REPO + r"\hooks.csv"
    targets = {t: MARK_ABSENT for t in ABSENT}
    targets.update({t: MARK_DISABLED for t in DISABLED})
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()
    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"):
            continue
        key = line.split(",", 1)[0].strip().lower()
        if key not in targets:
            continue
        fields = parse(line)
        if len(fields) != 9:
            sys.exit(f"row {i}: {len(fields)} cols")
        if fields[3] == "C2":
            continue  # idempotent
        if fields[3] != "C4":
            sys.exit(f"{key} is {fields[3]!r}, expected C4")
        if "C4-EVIDENCE-SUSPECT:" not in fields[8]:
            sys.exit(f"{key} is not suspect-tagged — refusing")
        fields[3] = "C2"
        fields[8] = (fields[8].replace(
            "C4-EVIDENCE-SUSPECT:",
            "C4-EVIDENCE-SUSPECT-CLEARED(2026-06-09,demoted-r1a):")
            + " " + targets[key]).strip()
        lines[i] = emit(fields) + "\n"
        done.append(key)
    missing = set(targets) - set(done)
    if missing:
        present = {l.split(",", 1)[0].strip().lower() for l in lines
                   if l and "," in l and not l.startswith("#")}
        truly = missing - present
        if truly:
            sys.exit(f"rows not found: {sorted(truly)}")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C4->C2 demoted "
          f"(absent={len([k for k in done if k in ABSENT])}, "
          f"disabled={len([k for k in done if k in DISABLED])})")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            kind = "no-install-site" if k in ABSENT else "install-disabled"
            f.write(f"2026-06-09  {k}  C4->C2  R1-A re-validation: "
                    f"{kind} (fails C3 hook gate)\n")
    print("CHANGELOG: appended")


if __name__ == "__main__":
    main()
