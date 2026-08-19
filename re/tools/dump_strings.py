# dump_strings.py — exact string extraction from a PE, addressed by VA.
#
# WHY THIS EXISTS: re/analysis/strings.txt (v1) was produced by a printable-run
# scanner. That is subtly wrong for name lookup and it cost real work on
# 2026-08-18: the Lua binding harvest resolved names through v1 and silently
# missed two, because v1 records
#
#     0x001ce207  ascii  ?EgyptPushColumn2
#
# The code pushes 0x005ce208 — one byte later, past the '?'. A run scanner has no
# way to know where the *referenced* string starts, so a lookup keyed on the
# recorded address fails and the name vanishes without an error.
#
# THE ROBUST PRIMITIVE IS `--at <VA>`: read forward from an exact address to the
# first NUL. That is what the code itself does, so it cannot disagree with the
# code. Prefer it over any table lookup when resolving a pointer to a string.
#
# The bulk dump is still useful for surveying, so it is emitted too — but as
# NUL-delimited chunks with exact start addresses, not as printable runs, and
# each row carries its VA so callers never have to convert offsets by hand.
#
# Usage:
#   py -3.12 re/tools/dump_strings.py original/MASHED.exe.unpatched -o out.tsv
#   py -3.12 re/tools/dump_strings.py original/MASHED.exe.unpatched --at 0x005ce208
#   py -3.12 re/tools/dump_strings.py original/MASHED.exe.unpatched --at 0x005cde58 --at 0x005cdb28
import argparse
import pathlib
import re
import sys

import pefile

# \n and \r are legitimate inside a C string literal — the RenderWare Physics $Id
# banners and the Lua version banner both embed them. Excluding them made --at
# reject real strings.
PRINTABLE = bytes(range(0x20, 0x7F)) + b"\t\n\r"


def _sections(pe, base):
    out = []
    for s in pe.sections:
        name = s.Name.rstrip(b"\x00").decode(errors="replace")
        vsize = max(s.Misc_VirtualSize, s.SizeOfRawData)
        out.append((name, base + s.VirtualAddress, vsize, s.PointerToRawData,
                    s.SizeOfRawData))
    return out


def va_to_off(secs, va):
    for name, vstart, vsize, raw, rawsize in secs:
        if vstart <= va < vstart + vsize:
            off = raw + (va - vstart)
            return off if off < raw + rawsize else None
    return None


def read_cstr(data, secs, va, maxlen=512):
    """Read forward from an exact VA to the first NUL. This is what the code does."""
    off = va_to_off(secs, va)
    if off is None:
        return None
    end = data.find(b"\x00", off, off + maxlen)
    if end < 0:
        return None
    raw = data[off:end]
    if not raw or any(b not in PRINTABLE for b in raw):
        return None
    return raw.decode("ascii", errors="replace")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("image")
    ap.add_argument("-o", "--out", type=pathlib.Path)
    ap.add_argument("--at", action="append", default=[],
                    help="resolve the string at this exact VA (repeatable); "
                         "this is the lookup the code performs")
    ap.add_argument("--min-len", type=int, default=4)
    ap.add_argument("--sections", default=".rdata,.data",
                    help="comma list, or 'all'")
    args = ap.parse_args()

    data = pathlib.Path(args.image).read_bytes()
    pe = pefile.PE(args.image, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    secs = _sections(pe, base)

    if args.at:
        for a in args.at:
            va = int(a, 16) if a.lower().startswith("0x") else int(a, 16)
            s = read_cstr(data, secs, va)
            print(f"0x{va:08x}\t{s if s is not None else '<not a printable NUL-terminated string>'}")
        return 0

    want = None if args.sections == "all" else set(args.sections.split(","))
    rows = []
    for name, vstart, vsize, raw, rawsize in secs:
        if want is not None and name not in want:
            continue
        blob = data[raw:raw + rawsize]
        # NUL-delimited chunks: the start of each chunk is an exact address, which
        # is what a printable-run scanner gets wrong.
        pos = 0
        while pos < len(blob):
            end = blob.find(b"\x00", pos)
            if end < 0:
                end = len(blob)
            chunk = blob[pos:end]
            if len(chunk) >= args.min_len and all(b in PRINTABLE for b in chunk):
                rows.append((vstart + pos, raw + pos, name, "ascii", len(chunk),
                             chunk.decode("ascii")))
            pos = end + 1
        # UTF-16LE, same idea on 2-byte units
        for m in re.finditer(rb"(?:[\x20-\x7e]\x00){%d,}" % args.min_len, blob):
            txt = m.group(0).decode("utf-16le")
            rows.append((vstart + m.start(), raw + m.start(), name, "utf-16le",
                         len(txt), txt))

    rows.sort()
    lines = ["va\tfile_off\tsection\tencoding\tlength\ttext"]
    lines += [f"0x{va:08x}\t0x{off:06x}\t{sec}\t{enc}\t{ln}\t{txt}"
              for va, off, sec, enc, ln, txt in rows]
    body = "\n".join(lines) + "\n"
    if args.out:
        args.out.write_text(body, encoding="utf-8")
        print(f"{len(rows)} strings -> {args.out}")
    else:
        sys.stdout.write(body)
    return 0


if __name__ == "__main__":
    sys.exit(main())
