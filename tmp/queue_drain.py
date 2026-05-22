"""Move a single Queued row in re/SCRIBE_QUEUE.md into the Drained block.

Usage:
    py -3.12 tmp/queue_drain.py <session_short_id> <bucket_name> <append_suffix>

The append_suffix is appended to the row (after a separator) when it's moved
to Drained. Example:
    drained-by=sweep-20260518-1448; 80 plates, 80 bookmarks, 0 renames
"""

from __future__ import annotations
import sys
import os

QUEUE_PATH = os.path.join(os.path.dirname(__file__), "..", "re", "SCRIBE_QUEUE.md")


def main():
    if len(sys.argv) != 3:
        print("usage: queue_drain.py <bucket_name> <append_suffix>", file=sys.stderr)
        sys.exit(2)
    bucket = sys.argv[1]
    suffix = sys.argv[2]

    with open(QUEUE_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    # Find Queued / Drained section markers
    q_idx = None
    d_idx = None
    for i, ln in enumerate(lines):
        s = ln.strip()
        if s == "## Queued":
            q_idx = i
        elif s == "## Drained":
            d_idx = i

    if q_idx is None or d_idx is None:
        print("ERROR: could not find both ## Queued and ## Drained sections", file=sys.stderr)
        sys.exit(3)

    # Find the row inside Queued referencing this bucket
    row_idx = None
    needle = f"bucket={bucket}"
    for i in range(q_idx + 1, d_idx):
        if needle in lines[i]:
            row_idx = i
            break
    if row_idx is None:
        print(f"ERROR: no Queued row found for bucket {bucket}", file=sys.stderr)
        sys.exit(4)

    row = lines[row_idx].rstrip("\n").rstrip()
    # Append suffix on same line
    new_row = row + "  " + suffix + "\n"

    # Remove row from Queued
    del lines[row_idx]
    # Adjust d_idx if it was past row_idx
    if d_idx > row_idx:
        d_idx -= 1

    # Insert new_row after the "## Drained" header. Find the ``` fence after it.
    # The Drained block uses ```...``` fences. We insert as a new line after the
    # opening ``` (i.e., as the first row inside the block).
    insert_at = d_idx + 1
    # Skip blank line after "## Drained"
    while insert_at < len(lines) and lines[insert_at].strip() == "":
        insert_at += 1
    # If the next line is ```, insert after it
    if insert_at < len(lines) and lines[insert_at].strip().startswith("```"):
        insert_at += 1
    lines.insert(insert_at, new_row)

    with open(QUEUE_PATH, "w", encoding="utf-8", newline="\n") as f:
        f.writelines(lines)
    print(f"moved bucket={bucket} from Queued to Drained")


if __name__ == "__main__":
    main()
