"""sweep_drain_one.py — move one bucket row from Queued/orphan into Drained
section of SCRIBE_QUEUE.md, append the suffix, and append a CHANGELOG line.

Usage:
  py -3.12 scripts/sweep_drain_one.py <bucket_name> <plates> <bookmarks> <renames> <session_id>
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

    # Find section markers
    queued_idx = None
    drained_idx = None
    for i, ln in enumerate(lines):
        if ln.startswith("## Queued"):
            queued_idx = i
        elif ln.startswith("## Drained"):
            drained_idx = i
            break

    if queued_idx is None or drained_idx is None:
        print(f"ERROR: missing ## Queued or ## Drained section markers")
        sys.exit(1)

    # Locate the bucket row anywhere in the file (queued or orphan position)
    bucket_re = re.compile(
        r"^2026-\d{2}-\d{2}\s+batch[-_][a-zA-Z]+-?s\d+\b.*\bbucket=(re/analysis/)?"
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
    # Append drain suffix
    suffix = (
        f"  drained-by={sid}; {plates} plates, {bookmarks} bookmarks, {renames} renames"
    )
    drained_row = row + suffix + "\n"

    # Remove the original row (and a single trailing blank line if it was an
    # orphan after Drained, to keep formatting clean)
    new_lines = lines[:row_idx] + lines[row_idx + 1 :]
    # Remove trailing whitespace-only block left behind: collapse runs of
    # multiple blank lines at the previous row's position
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

    # Re-locate drained section in the cleaned list
    new_drained_idx = None
    for i, ln in enumerate(cleaned):
        if ln.startswith("## Drained"):
            new_drained_idx = i
            break

    # Insert drained_row right after the "## Drained" header (or after the
    # opening blank line if one follows it).
    insert_at = new_drained_idx + 1
    while insert_at < len(cleaned) and cleaned[insert_at].strip() == "":
        insert_at += 1
    cleaned = cleaned[:insert_at] + [drained_row] + cleaned[insert_at:]

    QUEUE.write_text("".join(cleaned), encoding="utf-8")

    # Append CHANGELOG entry
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
