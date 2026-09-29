"""hooks.csv schema migration: append `exe_file` as the LAST column.

Why (2026-09-29, user decision on re/analysis/DUAL_COPY_AUDIT_2026-09-29.md):
`hooks.csv` had ONE `file` column, which by convention names the `.asi` copy --
the one Frida C3/C4 evidence measures. `mashed_re.exe` compiles a DIFFERENT
source list (`mashedmod/exe_sources.rsp`), so an RVA can have a second, drifting
body that the tracker cannot see. Five such divergences shipped defects.
This also closes the schema half of DEFERRED row D-11069, which asked for exactly
this ("give hooks.csv a way to record per-target implementations").

Semantics of the new column:
  exe_file = <path>   the exe compiles a port of this RVA in that TU
  exe_file = <empty>  the exe has no port of this RVA
  exe_file == file    ONE shared TU serves both targets (the goal state)

Mechanics: the migration appends a single `,` to the header and to every
non-comment data line. That is byte-preserving for the nine existing fields --
including any quoting -- because a trailing comma outside a quoted field simply
adds one empty field. Comment lines (`#...`) are left untouched.

Idempotent: refuses to run twice (detects `exe_file` already in the header).

  py -3.12 scripts/add_exe_file_column.py [--check]
"""
import csv
import io
import pathlib
import shutil
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
CSV_PATH = ROOT / "hooks.csv"
BACKUP = ROOT / "log" / "hooks.csv.pre-exe_file.bak"

OLD_HEADER = "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes"
NEW_HEADER = OLD_HEADER + ",exe_file"


def split_rows(text):
    """Return (lines, parsed) where parsed[i] is None for comment/blank lines."""
    lines = text.split("\n")
    parsed = []
    for ln in lines:
        if not ln.strip() or ln.startswith("#"):
            parsed.append(None)
        else:
            parsed.append(next(csv.reader(io.StringIO(ln))))
    return lines, parsed


def main(argv):
    check_only = "--check" in argv
    text = CSV_PATH.read_text(encoding="utf-8")
    lines, parsed = split_rows(text)

    if lines[0] == NEW_HEADER:
        print("already migrated: header already carries exe_file")
        return 0
    if lines[0] != OLD_HEADER:
        print(f"FATAL: unexpected header: {lines[0][:120]!r}")
        return 1

    widths = {len(r) for r in parsed if r is not None}
    if widths != {9}:
        print(f"FATAL: pre-migration rows are not uniformly 9 fields: {sorted(widths)}")
        return 1
    n_data = sum(1 for r in parsed if r is not None) - 1
    n_comment = sum(1 for ln, r in zip(lines, parsed) if r is None and ln.strip())
    print(f"pre:  header + {n_data} data rows (all 9 fields) + {n_comment} comment lines")

    out = []
    for i, (ln, row) in enumerate(zip(lines, parsed)):
        if row is None:
            out.append(ln)              # comment / blank: untouched
        elif ln == OLD_HEADER:
            out.append(NEW_HEADER)      # header gets the column NAME
        else:
            out.append(ln + ",")        # data row gets one empty field
    new_text = "\n".join(out)

    # Invariant: re-parsing the new text must give exactly 10 fields per data row,
    # with fields 0..8 byte-identical to the old parse and field 9 empty.
    _, reparsed = split_rows(new_text)
    if len(reparsed) != len(parsed):
        print("FATAL: line count drifted")
        return 1
    for i, (old, new) in enumerate(zip(parsed, reparsed)):
        if (old is None) != (new is None):
            print(f"FATAL: line {i} changed comment/data class")
            return 1
        if old is None:
            continue
        if i == 0:
            if new != old + ["exe_file"]:
                print(f"FATAL: header did not round-trip: {new}")
                return 1
            continue
        if len(new) != 10 or new[:9] != old or new[9] != "":
            print(f"FATAL: line {i} did not round-trip: {new[:3]}")
            return 1
    print(f"post: header + {n_data} data rows, all 10 fields, "
          f"fields 0..8 byte-identical, field 9 empty  [VERIFIED]")

    if check_only:
        print("--check: not written")
        return 0

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(CSV_PATH, BACKUP)
    CSV_PATH.write_text(new_text, encoding="utf-8", newline="")
    print(f"written; backup at {BACKUP.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
