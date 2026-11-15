#!/usr/bin/env python3
"""tag_lint.py: flag re/match/**/*.c sources whose `// MATCH:` tag is not at line start.

Two readers see the tag differently:
  match.py / match_permute.py   unanchored  r"//\\s*MATCH:\\s*(?:MASHED\\.exe\\s+)?(0x..)\\s+(\\S+)"
  ddkobj.py (MATCH_RE)          anchored    r"^\\s*//\\s*MATCH:..." with re.M (only leading whitespace allowed)
A tag that follows code on the same line (`int f(void);// MATCH: 0x... FUN_...`) is found by the first and missed
by the second, so a unit can match under match.py and still be invisible to ddkobj (MATCH-RWT-PORT-FOLLOWUPS,
0x004e4c30, commit e018bf5bf). This lint reports every source where the two readers disagree.

Usage: py -3.12 re/tools/match/tag_lint.py [--root <dir>] [--quiet]
Exit code 1 when any source is flagged, 0 otherwise. Read-only.
"""
import argparse
import re
import sys
from pathlib import Path

TAIL = r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]+)\s+(\S+)"
LOOSE = re.compile(TAIL)                      # match.py's reader
ANCHORED = re.compile(r"^\s*" + TAIL, re.M)   # ddkobj.py's reader


def lint_text(text):
    """List of (line_no, va, text_before_tag) for tags the anchored reader does not see."""
    bad = []
    for m in LOOSE.finditer(text):
        ls = text.rfind(chr(10), 0, m.start()) + 1
        if not text[ls:m.start()].strip():
            continue    # only whitespace before the tag on its line: ddkobj's ^\s* form reads it too
        line_no = text.count("\n", 0, m.start()) + 1
        bad.append((line_no, "0x" + m.group(1).lower(), text[ls:m.start()].strip()))
    return bad


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=str(Path(__file__).resolve().parents[2] / "match"))
    ap.add_argument("--quiet", action="store_true", help="counts only")
    a = ap.parse_args(argv)
    root = Path(a.root)
    files = sorted(root.rglob("*.c"))
    flagged = {}
    untagged = 0
    for f in files:
        t = f.read_text(errors="replace")
        if not LOOSE.search(t):
            untagged += 1
            continue
        bad = lint_text(t)
        if bad:
            flagged[f] = bad
    print(f"tag_lint: scanned {len(files)} .c files under {root}; no tag at all: {untagged}; "
          f"tag not at line start: {len(flagged)} files, {sum(len(v) for v in flagged.values())} tags")
    if not a.quiet:
        for f, bad in flagged.items():
            for line_no, va, before in bad:
                try:
                    rel = f.relative_to(root.parent.parent)
                except ValueError:
                    rel = f
                print(f"  {str(rel).replace(chr(92), '/')}:{line_no}: {va} after `{before[:70]}`")
    return 1 if flagged else 0


if __name__ == "__main__":
    sys.exit(main())
