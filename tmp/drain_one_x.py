"""drain_one_x.py — move one batch_x bucket row from anywhere in SCRIBE_QUEUE.md
into the Drained section, append the drained-by suffix, and write the
CHANGELOG line.

Handles BOTH cases:
  - row in '## Queued' (s2/s3/s4/s6) → normal move
  - row already under '## Drained' but without 'drained-by=' suffix
    (s1/s5 misfiled by their agents) → strip from current location,
    re-insert under Drained header WITH the drained-by= tag

Usage:
  py -3.12 tmp/drain_one_x.py <bucket_name> <plates> <bookmarks> <renames> <sid>
"""

import re
import sys
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
QUEUE = ROOT / "re" / "SCRIBE_QUEUE.md"
CHLOG = ROOT / "re" / "analysis" / "CHANGELOG.md"


def main():
    bucket = sys.argv[1]
    plates = int(sys.argv[2])
    bookmarks = int(sys.argv[3])
    renames = int(sys.argv[4])
    sid = sys.argv[5]

    text = QUEUE.read_text(encoding="utf-8")
    lines = text.splitlines(keepends=True)

    # Match batch-x-s\d+ rows for this bucket
    bucket_re = re.compile(
        r"^2026-\d{2}-\d{2}\s+batch-x-s\d+\b.*\bbucket=(re/analysis/)?"
        + re.escape(bucket)
        + r"\b"
    )
    row_idx = None
    for i, ln in enumerate(lines):
        if bucket_re.match(ln):
            row_idx = i
            break

    if row_idx is None:
        print(f"ERROR: row for bucket={bucket} not found")
        sys.exit(1)

    row = lines[row_idx].rstrip("\r\n")

    # If row already has drained-by= it was actually drained; bail.
    if "drained-by=" in row:
        print(f"ERROR: row for bucket={bucket} already has drained-by= tag")
        sys.exit(1)

    suffix = f"  drained-by={sid}; {plates} plates, {bookmarks} bookmarks, {renames} renames"
    drained_row = row + suffix + "\n"

    # Remove original row
    new_lines = lines[:row_idx] + lines[row_idx + 1 :]
    # Collapse blank-line runs
    cleaned = []
    blank_run = 0
    for ln in new_lines:
        if ln.strip() == "":
            blank_run += 1
            if blank_run <= 1:
                cleaned.append(ln)
        else:
            blank_run = 0
            cleaned.append(ln)

    # Insert under ## Drained header
    new_drained_idx = None
    for i, ln in enumerate(cleaned):
        if ln.startswith("## Drained"):
            new_drained_idx = i
            break
    insert_at = new_drained_idx + 1
    while insert_at < len(cleaned) and cleaned[insert_at].strip() == "":
        insert_at += 1
    cleaned = cleaned[:insert_at] + [drained_row] + cleaned[insert_at:]

    QUEUE.write_text("".join(cleaned), encoding="utf-8")

    date = datetime.utcnow().strftime("%Y-%m-%d")
    chrow = (
        f"{date}  {sid}  scribe-release  bucket={bucket}  writes={plates}  "
        f"renames={renames}  errors=0\n"
    )
    with CHLOG.open("a", encoding="utf-8") as f:
        f.write(chrow)

    print(f"drained bucket={bucket} plates={plates} bookmarks={bookmarks} renames={renames}")


if __name__ == "__main__":
    main()
