"""Helper for ghidra-sweep: parse first bullet of ## Mechanical description from a plate .md.

Usage:
    py -3.12 tmp/sweep_helper.py <bucket_dir> <rva_hex_no_0x>

Outputs: a single line, the C1 plate text:
    "[C1 YYYY-MM-DD] <first bullet, truncated at word boundary <=120 chars + '...' if cut>"

Filename convention is tolerant: tries both "0x<rva>.md" and "<rva>.md".
Returns non-zero exit + stderr if the file is missing.
"""

from __future__ import annotations
import sys
import os
import re
from datetime import datetime, timezone

TRUNCATE = 120
TODAY = datetime.now(timezone.utc).strftime("%Y-%m-%d")


def find_plate(bucket_dir: str, rva: str) -> str | None:
    """Return path to plate file, trying both naming conventions. None if missing."""
    candidates = [
        os.path.join(bucket_dir, f"0x{rva}.md"),
        os.path.join(bucket_dir, f"{rva}.md"),
    ]
    for c in candidates:
        if os.path.isfile(c):
            return c
    return None


def first_bullet_of_mech_desc(md_path: str) -> str | None:
    """Extract first bullet (first '- ' line) under '## Mechanical description'.

    Bullet may continue across indented continuation lines (e.g. '  - sub bullet'
    or unindented text). We grab from the first '- ' to either the next top-level
    '- ' line that starts in column 0, or the next '##' heading, or end.
    """
    with open(md_path, "r", encoding="utf-8") as f:
        text = f.read()

    # Find the heading
    m = re.search(r"^##\s+Mechanical\s+description\s*$", text, flags=re.MULTILINE)
    if not m:
        return None
    body_start = m.end()
    # Find next ## heading
    n = re.search(r"^##\s+", text[body_start:], flags=re.MULTILINE)
    body_end = body_start + n.start() if n else len(text)
    body = text[body_start:body_end]

    # Find first bullet
    lines = body.splitlines()
    out = []
    started = False
    for ln in lines:
        stripped = ln.rstrip()
        if not started:
            if stripped.startswith("- "):
                started = True
                out.append(stripped[2:].strip())
            # else skip blank/intro lines
        else:
            # Stop on next top-level bullet (starts at col 0 with '- ')
            if stripped.startswith("- "):
                break
            # Stop on blank-followed-by-non-indented (defensive: end of bullet group)
            if stripped == "":
                # peek: blank line is a soft break; include if more continuation follows
                # Actually, for the FIRST bullet we just stop at first blank to be safe.
                # But many plates have multi-line bullets without blanks; so only stop
                # on blank lines that aren't immediately followed by an indented line.
                # Simpler heuristic: just stop at first blank line.
                break
            # Continuation: indented sub-line or wrapped text
            out.append(stripped.strip())

    if not out:
        return None
    return " ".join(out).strip()


def truncate_at_word(s: str, limit: int = TRUNCATE) -> str:
    """Truncate s at the last word boundary <= limit, appending '...' if cut."""
    if len(s) <= limit:
        return s
    cut = s[:limit]
    # Find last whitespace
    sp = cut.rfind(" ")
    if sp <= 0:
        # No good word boundary; just hard cut
        return cut.rstrip() + "..."
    return cut[:sp].rstrip() + "..."


def main():
    if len(sys.argv) != 3:
        print("usage: sweep_helper.py <bucket_dir> <rva_hex_no_0x>", file=sys.stderr)
        sys.exit(2)
    bucket_dir = sys.argv[1]
    rva = sys.argv[2].lower().lstrip("0x").rjust(8, "0")
    # Try both: original input and stripped
    plate = find_plate(bucket_dir, rva)
    if not plate:
        # Try original arg too (some files may have shorter forms)
        plate = find_plate(bucket_dir, sys.argv[2].lower())
    if not plate:
        print(f"MISSING: no plate file for rva {rva} in {bucket_dir}", file=sys.stderr)
        sys.exit(3)
    bullet = first_bullet_of_mech_desc(plate)
    if not bullet:
        print(f"MISSING: no '## Mechanical description' first bullet in {plate}", file=sys.stderr)
        sys.exit(4)
    text = f"[C1 {TODAY}] {bullet}"
    text = truncate_at_word(text)
    # Print exactly, no trailing newline normalization beyond what print adds.
    sys.stdout.write(text)


if __name__ == "__main__":
    main()
