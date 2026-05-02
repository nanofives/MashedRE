"""
First-pass extractor for Mashed `.piz` archives.

Format (per XeNTaX thread on Mashed PIZ, cross-verified against
re/prior_art/MashedFileExtractor/FileFormats/ when present):

    Header (2048 bytes):
        +0x00  4B   magic     "PIZ\0"
        +0x04  4B   version   uint32  (3 in all known files)
        +0x08  4B   count     uint32
        +0x0C  -    pad       null bytes through 0x800

    Per-entry (128 bytes), `count` of them, starting at 0x800:
        +0x00  116B name      null-padded ASCII
        +0x74  4B   offset    uint32   <-- RAW bytes OR sector index; we try raw first
        +0x78  4B   length    uint32   raw bytes
        +0x7C  4B   id        uint32   purpose unconfirmed

    File data: each blob padded to 2048-byte boundary.

If raw-byte offsets read past EOF, we retry with offset * 2048 (sector index).
The first successful interpretation is assumed for the rest of the archive.
"""
import argparse
import fnmatch
import os
import struct
import sys
from pathlib import Path

HEADER_SIZE = 2048
ENTRY_SIZE = 128
SECTOR = 2048
MAGIC = b"PIZ\x00"


def parse_header(buf: bytes):
    if len(buf) < HEADER_SIZE:
        raise ValueError(f"file too small ({len(buf)} bytes) — not a .piz")
    magic = buf[:4]
    if magic != MAGIC:
        raise ValueError(f"bad magic {magic!r} — not a .piz")
    version, count = struct.unpack_from("<II", buf, 4)
    return version, count


def parse_entry(buf: bytes, off: int):
    name_raw = buf[off : off + 116]
    name = name_raw.split(b"\x00", 1)[0].decode("ascii", errors="replace")
    file_off, length, file_id = struct.unpack_from("<III", buf, off + 0x74)
    return name, file_off, length, file_id


def detect_offset_mode(data: bytes, entries):
    """Return 'raw' or 'sector' based on first entry's offset+length sanity."""
    name, off, length, _ = entries[0]
    if off + length <= len(data):
        return "raw"
    if (off * SECTOR) + length <= len(data):
        return "sector"
    raise ValueError(
        f"first entry {name!r} offset={off:#x} length={length:#x} doesn't fit either raw or sector mode "
        f"(file size {len(data):#x})"
    )


def list_archive(path: Path) -> int:
    data = path.read_bytes()
    version, count = parse_header(data)
    print(f"{path.name}  version={version}  entries={count}  size={len(data)}")
    entries = [parse_entry(data, HEADER_SIZE + i * ENTRY_SIZE) for i in range(count)]
    mode = detect_offset_mode(data, entries)
    print(f"  offset mode: {mode}")
    print(f"  {'name':<60} {'offset':>10} {'length':>10} {'id':>10}")
    for name, off, length, fid in entries:
        real_off = off if mode == "raw" else off * SECTOR
        print(f"  {name:<60} {real_off:>#10x} {length:>#10x} {fid:>#10x}")
    return 0


def extract_archive(path: Path, out_dir: Path, filter_glob: str | None) -> int:
    data = path.read_bytes()
    version, count = parse_header(data)
    entries = [parse_entry(data, HEADER_SIZE + i * ENTRY_SIZE) for i in range(count)]
    mode = detect_offset_mode(data, entries)
    out_dir.mkdir(parents=True, exist_ok=True)
    written = 0
    for name, off, length, _ in entries:
        if filter_glob and not fnmatch.fnmatch(name, filter_glob):
            continue
        real_off = off if mode == "raw" else off * SECTOR
        if real_off + length > len(data):
            print(f"  SKIP {name}: out of bounds ({real_off:#x}+{length:#x} > {len(data):#x})", file=sys.stderr)
            continue
        blob = data[real_off : real_off + length]
        # Path inside .piz uses backslashes; normalize.
        rel = name.replace("\\", os.sep)
        dest = out_dir / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(blob)
        written += 1
    print(f"extracted {written}/{count} entries to {out_dir}")
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(description="Mashed .piz archive tool")
    sub = p.add_subparsers(dest="cmd", required=True)
    pl = sub.add_parser("list", help="list entries")
    pl.add_argument("archive", type=Path)
    pe = sub.add_parser("extract", help="extract entries")
    pe.add_argument("archive", type=Path)
    pe.add_argument("-o", "--out", type=Path, required=True)
    pe.add_argument("--filter", help="glob pattern, e.g. '*.tga'")
    args = p.parse_args(argv)
    if args.cmd == "list":
        return list_archive(args.archive)
    return extract_archive(args.archive, args.out, args.filter)


if __name__ == "__main__":
    sys.exit(main())
