"""Extract printable ASCII / UTF-16LE runs from a PE binary.

Output: <run> <offset> <encoding>  per line, sorted by offset.
Filters:  --min N (default 6),  --grep PATTERN (regex; case-insensitive)
"""
import argparse
import re
import sys
from pathlib import Path

ASCII_PRINTABLE = set(bytes(range(0x20, 0x7F)) + b"\t")


def runs_ascii(data: bytes, min_len: int):
    start = None
    for i, c in enumerate(data):
        if c in ASCII_PRINTABLE:
            if start is None:
                start = i
        else:
            if start is not None and i - start >= min_len:
                yield start, data[start:i].decode("ascii", errors="replace"), "ascii"
            start = None
    if start is not None and len(data) - start >= min_len:
        yield start, data[start:].decode("ascii", errors="replace"), "ascii"


def runs_utf16le(data: bytes, min_len: int):
    # Two-byte stride; each pair must be (printable, 0x00)
    start = None
    i = 0
    while i + 1 < len(data):
        c = data[i]
        if data[i + 1] == 0 and c in ASCII_PRINTABLE:
            if start is None:
                start = i
            i += 2
        else:
            if start is not None and (i - start) // 2 >= min_len:
                s = data[start:i].decode("utf-16-le", errors="replace")
                yield start, s, "utf-16le"
            start = None
            i += 1
    if start is not None and (len(data) - start) // 2 >= min_len:
        s = data[start:].decode("utf-16-le", errors="replace")
        yield start, s, "utf-16le"


def main(argv=None):
    p = argparse.ArgumentParser()
    p.add_argument("binary", type=Path)
    p.add_argument("-o", "--out", type=Path, help="output file (default stdout)")
    p.add_argument("--min", type=int, default=6, help="minimum run length in chars")
    p.add_argument("--grep", help="regex filter (case-insensitive)")
    p.add_argument("--ascii-only", action="store_true")
    p.add_argument("--utf16-only", action="store_true")
    args = p.parse_args(argv)

    data = args.binary.read_bytes()
    pat = re.compile(args.grep, re.IGNORECASE) if args.grep else None

    items = []
    if not args.utf16_only:
        items.extend(runs_ascii(data, args.min))
    if not args.ascii_only:
        items.extend(runs_utf16le(data, args.min))
    items.sort(key=lambda x: x[0])

    out = args.out.open("w", encoding="utf-8") if args.out else sys.stdout
    try:
        for off, s, enc in items:
            if pat and not pat.search(s):
                continue
            print(f"0x{off:08x}  {enc:9s}  {s}", file=out)
    finally:
        if args.out:
            out.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
