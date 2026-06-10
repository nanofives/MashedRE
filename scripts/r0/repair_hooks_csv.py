#!/usr/bin/env python3
"""R0 structural repair for hooks.csv (Phase R0, 2026-06-09).

Repairs, all mechanical / lossless:
  1. Remove the duplicate header line embedded in the body.
  2. Merge note-spill continuation lines (lines starting with optional
     whitespace + '|') into the preceding data row's notes field.
  3. Normalize the rva field: lowercase, strip '0x' prefix (file convention
     per header comment: "no prefix in CSV").
  4. Re-emit every data row through csv.writer so quoting is normalized.
  5. Rows parsing to >9 fields (unquoted commas in notes): tail folded into
     the notes field. Rows with <9 fields: padded with empty fields and
     reported.

Refuses to write if the resulting row count or per-row field count is wrong.
Backup written to log/backups/hooks.csv.pre_r0 before any write.
"""
import csv
import io
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CSV_PATH = ROOT / "hooks.csv"
BACKUP = ROOT / "log" / "backups" / "hooks.csv.pre_r0"

HEADER = "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes"


def main() -> int:
    raw = CSV_PATH.read_text(encoding="utf-8")
    lines = raw.splitlines()

    stats = {"dup_header": 0, "spill_merged": 0, "prefix_norm": 0,
             "overlong_folded": 0, "short_padded": 0, "blank_dropped": 0}

    # Pass 1: drop duplicate headers / blanks, merge spill lines.
    logical: list[str] = []   # data lines only
    comments: list[str] = []  # '#' comment lines, preserved in original order
    assert lines[0] == HEADER, "first line is not the expected header"
    for ln in lines[1:]:
        if ln.strip() == "":
            stats["blank_dropped"] += 1
            continue
        if ln == HEADER:
            stats["dup_header"] += 1
            continue
        if ln.startswith("#"):
            comments.append(ln)
            continue
        if ln.lstrip().startswith("|"):
            if not logical:
                print(f"FATAL: spill line with no preceding row: {ln[:80]}")
                return 1
            prev = logical[-1]
            # If prev ends with a closing quote, splice inside it.
            if prev.endswith('"'):
                logical[-1] = prev[:-1] + " " + ln.strip() + '"'
            else:
                logical[-1] = prev + " " + ln.strip()
            stats["spill_merged"] += 1
            continue
        logical.append(ln)

    # Pass 2: csv-parse each logical row, repair field counts, normalize rva.
    out_rows: list[list[str]] = []
    for ln in logical:
        row = next(csv.reader(io.StringIO(ln)))
        if len(row) > 9:
            row = row[:8] + [",".join(row[8:])]
            stats["overlong_folded"] += 1
        elif len(row) < 9:
            print(f"WARN short row ({len(row)} fields): {ln[:100]}")
            row = row + [""] * (9 - len(row))
            stats["short_padded"] += 1
        rva = row[0].strip()
        if rva.lower().startswith("0x"):
            row[0] = rva[2:].lower()
            stats["prefix_norm"] += 1
        else:
            row[0] = rva.lower()
        out_rows.append(row)

    # Invariants.
    expected = len(logical)
    if len(out_rows) != expected:
        print(f"FATAL: row count drifted {expected} -> {len(out_rows)}")
        return 1
    if any(len(r) != 9 for r in out_rows):
        print("FATAL: a row does not have exactly 9 fields after repair")
        return 1

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(CSV_PATH, BACKUP)

    buf = io.StringIO()
    w = csv.writer(buf, lineterminator="\n")
    buf.write(HEADER + "\n")
    for c in comments:
        buf.write(c + "\n")
    for r in out_rows:
        w.writerow(r)
    CSV_PATH.write_text(buf.getvalue(), encoding="utf-8")

    # Re-read and validate.
    with open(CSV_PATH, newline="", encoding="utf-8") as f:
        n = sum(1 for row in csv.reader(f)
                if row and not row[0].startswith("#") and row[0] != "rva")
    print(f"OK rows={n} stats={stats} backup={BACKUP}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
