#!/usr/bin/env python3
"""C4 re-validation: RECONFIRM RwMatrixScale (hot-path render leaf) — stays C4.

Evidence (canonical_c4_verify_hotpath.py, 2026-06-06):
  - installed: inline-JMP live (0xE9) for all 5 RW-math candidates in the ON run
  - survived: booted to menu, survived 22s with hooks installed in the hot render
    path (no Interceptor overhead — install only)
  - exercised: 5376 calls in a 2.5s post-menu Interceptor window (OFF/count run)
  - output identity: pure deterministic leaf (no state/globals); the C3 force-call
    diff (log/diff_rw_matrix_scale.csv, 11/11 GREEN) covers ALL 3 mode branches
    (0/1/2) over identity/mixed/translation/scale matrices, so the canonical
    scenario cannot feed an uncovered branch.
Supersedes the broken-loader-window evidence. The other 4 RW-math suspects
(004c3730/004c3880/004c3bf0/004c3c60) logged 0 calls at the idle menu -> HOLD.
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SCEN = "boot_to_menu_install_observe_2026-06-06"
TARGETS = {"004c5010": ("RwMatrixScale", 5376)}
SUSPECT_TOKEN = "| C4-EVIDENCE-SUSPECT"

def parse(line): return next(csv.reader([line.rstrip("\n")]))
def emit(fields):
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(fields); return b.getvalue()

def main():
    hooks = REPO + r"\hooks.csv"
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()
    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"): continue
        key = line.split(",", 1)[0].strip().lower()
        if key in TARGETS:
            fields = parse(line)
            if fields[3] != "C4":
                sys.exit(f"{key} is {fields[3]!r}, expected C4")
            if "C4-RECONFIRMED" in fields[8]:
                continue
            name, n = TARGETS[key]
            notes = fields[8]
            idx = notes.find(SUSPECT_TOKEN)
            if idx != -1: notes = notes[:idx].rstrip()
            marker = (f"| C4-RECONFIRMED (canonical install-observe 2026-06-06, hot-path lane): "
                      f"inline-JMP LIVE (0xE9) + booted-to-menu & survived 22s with hook installed "
                      f"(no Interceptor on hot path) + exercised {n}x in a 2.5s post-menu window; "
                      f"pure-deterministic leaf, C3 diff covers ALL 3 mode branches (11/11 GREEN) so "
                      f"the canonical scenario cannot feed an uncovered branch. Supersedes broken-loader evidence.")
            fields[8] = (notes + " " + marker).strip()
            fields[6] = SCEN
            fields[7] = "log/c4_hotpath_result.json;log/diff_rw_matrix_scale.csv"
            lines[i] = emit(fields) + "\n"; done.append(key)
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C4 reconfirmed: {sorted(done)}")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-06  {k}  {TARGETS[k][0]}  C4-RECONFIRMED  canonical install-observe hot-path ({TARGETS[k][1]}x)  {SCEN}\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
