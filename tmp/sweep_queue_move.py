#!/usr/bin/env python3
"""Move a Queued row from re/SCRIBE_QUEUE.md to Drained, appending drained-by suffix.

Usage:
    py -3.12 tmp/sweep_queue_move.py <session_id_tag> <bucket_name> <drain_suffix>

Example:
    py -3.12 tmp/sweep_queue_move.py batch-u-s3 cluster_0055_first_pass \
       "drained-by=sweep-20260518-0514; 60 plates, 60 bookmarks, 0 renames; bucket-split=4 sub-dirs (rtfs_manager/anim_channel/vector_path/vector_font/) — see CHANGELOG; subsystem CONFIRMED MULTI[rtfs-manager,vector-path,vector-font]"
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
QFILE = ROOT / "re" / "SCRIBE_QUEUE.md"


def main():
    if len(sys.argv) != 4:
        print(__doc__)
        sys.exit(1)
    tag = sys.argv[1]
    bucket = sys.argv[2]
    suffix = sys.argv[3]

    text = QFILE.read_text(encoding="utf-8")
    # Split file at "## Drained"
    drained_marker = "\n## Drained\n"
    if drained_marker not in text:
        print(f"ERROR: missing '## Drained' header")
        sys.exit(2)
    queued_section, drained_section = text.split(drained_marker, 1)

    # Find the row in queued_section.
    # Each row sits inside a ``` ... ``` block. Match the block whose row line includes "<tag>  bucket=<bucket>".
    # Strategy: find the block that contains both substrings on a single line.
    # Find each ```\n ... ```\n fenced block and check if it contains the row line.
    fence_pat = re.compile(r"\n```\n(.+?)\n```\n", re.DOTALL)
    m = None
    for cand in fence_pat.finditer(queued_section):
        body = cand.group(1)
        if tag in body and f"bucket={bucket}" in body:
            m = cand
            break
    if not m:
        print(f"ERROR: could not locate queued row tag='{tag}' bucket='{bucket}'")
        sys.exit(3)
    row_body = m.group(1).strip()
    # Remove the matched block (including a trailing blank line if present)
    start, end = m.span(0)
    # Also consume an immediate trailing "\n" if present so we don't leave double blanks
    new_queued = queued_section[:start] + "\n" + queued_section[end:]

    # Build drained line: original row line + "  " + suffix
    drained_line = row_body.rstrip() + "  " + suffix

    # Append to existing batch-u drained code block if present (the most recent code block in Drained
    # that lists batch-u-* rows is the most natural target). Simpler approach: open a fresh ``` block
    # at the very top of the Drained section to keep batch_u rows grouped.
    # Find first ``` in drained_section and inject before it.
    first_fence = drained_section.find("\n```\n")
    if first_fence < 0:
        # No existing block; just append.
        new_drained = drained_section + "\n```\n" + drained_line + "\n```\n"
    else:
        # Inject our row INSIDE the first fenced block if its first line is a batch-u row,
        # else create a new block immediately before.
        block_start = first_fence + len("\n")
        # Find end of this block
        block_end = drained_section.find("\n```\n", block_start + len("```\n"))
        if block_end < 0:
            block_end = len(drained_section)
        first_block = drained_section[block_start:block_end]  # includes opening ``` and content
        if "batch-u-" in first_block:
            # Append our drained_line as a new line inside this block (before its closing fence)
            # The block runs from a ```\n line through its content up to a ```\n closing line
            # Inject at the end of the block content (before the closing fence)
            # block_end points to "\n```\n" closing
            new_drained = (
                drained_section[:block_end] + "\n" + drained_line + drained_section[block_end:]
            )
        else:
            # Create a new block before the first existing block
            inject = "\n```\n" + drained_line + "\n```\n"
            new_drained = drained_section[:first_fence] + inject + drained_section[first_fence:]

    new_text = new_queued + drained_marker + new_drained
    QFILE.write_text(new_text, encoding="utf-8")
    print(f"OK moved tag={tag} bucket={bucket} -> drained")


if __name__ == "__main__":
    main()
