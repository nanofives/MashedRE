#!/usr/bin/env python3
"""R1 Phase B: reconfirm the 85 installable suspect-C4 rows (2026-06-09).

Evidence: log/install_observe_r1b_20260609.txt — six subset-install canonical
boot-to-menu runs (canonical_install_observe.py), each hook verified inline-JMP
live (first byte 0xE9) with the .asi manifest confirming install, 25s survival,
no crash. This reproduces, with the loader demonstrably working, exactly the
evidence class the 2026-06-05 tag-don't-demote decision demanded
("installed-hook canonical re-validation").

For every row still C4 + C4-EVIDENCE-SUSPECT-tagged: rewrite the token to
C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect): and append the evidence
string. Refuses rows that are not C4. Idempotent.
"""
import csv
import io
import sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
EVIDENCE = ("| RECONFIRMED C4 (R1-B 2026-06-09): subset-install canonical "
            "boot-to-menu observe — installed 0xE9 live + manifest installed=1 "
            "+ 25s survival, no crash; log/install_observe_r1b_20260609.txt")


def parse(line):
    return next(csv.reader([line.rstrip("\n")]))


def emit(fields):
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(fields)
    return b.getvalue()


def main():
    hooks = REPO + r"\hooks.csv"
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()
    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"):
            continue
        if "C4-EVIDENCE-SUSPECT:" not in line:
            continue
        fields = parse(line)
        if len(fields) != 9:
            sys.exit(f"row {i}: {len(fields)} cols")
        if fields[3] != "C4":
            sys.exit(f"{fields[0]} suspect-tagged but {fields[3]!r} — "
                     f"run demote pass first")
        fields[8] = (fields[8].replace(
            "C4-EVIDENCE-SUSPECT:",
            "C4-EVIDENCE-RECONFIRMED(R1-B 2026-06-09, was suspect):")
            + " " + EVIDENCE).strip()
        lines[i] = emit(fields) + "\n"
        done.append(fields[0])
    if len(done) != 85:
        sys.exit(f"expected exactly 85 open suspect rows, found {len(done)} — "
                 f"aborting (nothing written)")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C4 rows reconfirmed")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        f.write("\n2026-06-09 r1-b(c4-revalidation): 85/85 installable suspect "
                "C4 rows RECONFIRMED via subset-install canonical boot-to-menu "
                "observe (6 runs, every hook 0xE9-live + manifest installed=1 + "
                "25s survival, 0 crashes; evidence "
                "log/install_observe_r1b_20260609.txt). Combined with the 16 "
                "R1-A demotions, the C4-EVIDENCE-SUSPECT open population is now "
                "ZERO. C4=106, all with installed-hook canonical evidence or "
                "post-fix diffs. R1 exit criterion MET.\n")
        for k in done:
            f.write(f"2026-06-09  {k}  C4 reconfirmed  R1-B install-observe "
                    f"(0xE9 live, 25s survival)\n")
    print("CHANGELOG: appended")


if __name__ == "__main__":
    main()
