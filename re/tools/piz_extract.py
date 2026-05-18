"""
Reader/writer for Mashed `.piz` archives.

Format (XeNTaX-documented; cross-verified against
re/prior_art/MashedFileExtractor/FileFormats/PIZ/ and against the six real
archives in original/TOASTART/Common/ on 2026-05-17):

    Header (2048 bytes):
        +0x00  4B   magic         "PIZ\\0"
        +0x04  4B   version       uint32  (3 in all known files)
        +0x08  4B   count         uint32
        +0x0C  4B   appendix2     four identical bytes, either 0x00 or 0xCC
                                  (Font36.piz, Perm.piz are 0xCCCCCCCC; the
                                  rest observed are 0x00000000)
        +0x10  -    pad           null bytes through 0x800

    Per-entry (128 bytes), `count` of them, starting at 0x800:
        +0x00  115B name          null-padded ASCII (byte at +0x73 is also
                                  null in every observed entry, so practical
                                  max is 114 chars + terminator)
        +0x74  4B   offset        uint32   raw byte offset within the archive
        +0x78  4B   size          uint32   raw byte length
        +0x7C  4B   id            uint32   purpose unconfirmed (likely a CRC
                                  of name or a vfs-id; not derivable from data)

    Entry-table region (count * 128 bytes) is itself zero-padded to the next
    2048-byte boundary before the first file blob.

    File data: each blob's offset is sector-aligned (2048-byte boundary).  Each
    blob is followed by zero padding out to the next 2048-byte boundary.
    Entries in the table are stored in ascending-offset order.

The writer additionally maintains a sidecar manifest (`_piz_manifest.json`) so
extract -> pack is byte-identical: the manifest preserves the original entry
order, the `id` field per entry, and the appendix2 variant.  Without the
manifest the writer can still produce a valid .piz, but it will not match the
original byte-for-byte (id is non-derivable; entry order would default to
alphabetical).
"""
import argparse
import fnmatch
import json
import os
import struct
import sys
import zlib
from pathlib import Path

HEADER_SIZE = 2048
ENTRY_SIZE = 128
SECTOR = 2048
MAGIC = b"PIZ\x00"
VERSION = 3
NAME_FIELD_LEN = 0x74  # name occupies bytes [0x00..0x74); SciLor caps at 0x73
                       # but the 0x73 byte is the terminator and is always null
                       # in observed entries.
APPENDIX2_OFFSET = 0x0C
APPENDIX2_LEN = 4
MANIFEST_NAME = "_piz_manifest.json"


# ---------------------------------------------------------------------------
# Header / entry parsing (shared by all subcommands)
# ---------------------------------------------------------------------------


def parse_header(buf: bytes):
    if len(buf) < HEADER_SIZE:
        raise ValueError(f"file too small ({len(buf)} bytes) -- not a .piz")
    magic = buf[:4]
    if magic != MAGIC:
        raise ValueError(f"bad magic {magic!r} -- not a .piz")
    version, count = struct.unpack_from("<II", buf, 4)
    appendix2 = buf[APPENDIX2_OFFSET : APPENDIX2_OFFSET + APPENDIX2_LEN]
    return version, count, appendix2


def parse_entry(buf: bytes, off: int):
    name_raw = buf[off : off + NAME_FIELD_LEN]
    name = name_raw.split(b"\x00", 1)[0].decode("ascii", errors="replace")
    file_off, length, file_id = struct.unpack_from("<III", buf, off + 0x74)
    return name, file_off, length, file_id


def detect_offset_mode(data: bytes, entries):
    """Return 'raw' or 'sector' based on first entry's offset+length sanity.

    All real Mashed archives observed are 'raw'.  The sector branch is kept as
    a fallback in case SciLor's parser was anticipating it; we have not seen
    one in the wild.
    """
    name, off, length, _ = entries[0]
    if off + length <= len(data):
        return "raw"
    if (off * SECTOR) + length <= len(data):
        return "sector"
    raise ValueError(
        f"first entry {name!r} offset={off:#x} length={length:#x} doesn't fit either raw or sector mode "
        f"(file size {len(data):#x})"
    )


def read_archive(path: Path):
    """Parse a .piz into (data, version, appendix2, entries, mode).

    Always resolves entry offsets to raw bytes.
    """
    data = path.read_bytes()
    version, count, appendix2 = parse_header(data)
    entries = [parse_entry(data, HEADER_SIZE + i * ENTRY_SIZE) for i in range(count)]
    mode = detect_offset_mode(data, entries)
    if mode == "sector":
        entries = [(n, o * SECTOR, l, i) for (n, o, l, i) in entries]
    return data, version, appendix2, entries, mode


# ---------------------------------------------------------------------------
# list / extract
# ---------------------------------------------------------------------------


def list_archive(path: Path) -> int:
    data, version, appendix2, entries, mode = read_archive(path)
    print(
        f"{path.name}  version={version}  entries={len(entries)}  size={len(data)}  "
        f"appendix2={appendix2.hex()}  mode={mode}"
    )
    print(f"  {'name':<60} {'offset':>10} {'size':>10} {'id':>10}")
    for name, off, length, fid in entries:
        print(f"  {name:<60} {off:>#10x} {length:>#10x} {fid:>#10x}")
    return 0


def extract_archive(path: Path, out_dir: Path, filter_glob: str | None) -> int:
    data, version, appendix2, entries, mode = read_archive(path)
    out_dir.mkdir(parents=True, exist_ok=True)

    written = 0
    manifest_entries = []
    for idx, (name, off, length, fid) in enumerate(entries):
        if filter_glob and not fnmatch.fnmatch(name, filter_glob):
            continue
        if off + length > len(data):
            print(
                f"  SKIP {name}: out of bounds ({off:#x}+{length:#x} > {len(data):#x})",
                file=sys.stderr,
            )
            continue
        blob = data[off : off + length]
        # Path inside .piz uses backslashes for nested directories on some
        # archives (Perm.piz); normalize to OS separator on extract.
        rel = name.replace("\\", "/").replace("/", os.sep)
        dest = out_dir / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(blob)
        written += 1
        manifest_entries.append(
            {
                "order": idx,
                "name": name,                       # preserve original separators
                "id": fid,
                "size": length,
                "offset": off,                      # informational; pack recomputes
                "crc32": f"{zlib.crc32(blob):08x}",
                "extracted_path": rel.replace(os.sep, "/"),
            }
        )

    # Only write the sidecar if the extract was complete (no glob filter).
    # A partial extract cannot be repacked into a byte-identical archive.
    if filter_glob is None:
        manifest = {
            "format": "piz",
            "version": version,
            "appendix2": appendix2.hex(),
            "source_archive": path.name,
            "source_size": len(data),
            "entries": manifest_entries,
        }
        (out_dir / MANIFEST_NAME).write_text(
            json.dumps(manifest, indent=2), encoding="utf-8"
        )

    print(f"extracted {written}/{len(entries)} entries to {out_dir}")
    return 0


# ---------------------------------------------------------------------------
# pack
# ---------------------------------------------------------------------------


def _build_manifest_from_dir(src_dir: Path) -> dict:
    """Synthesize a manifest from a directory that has none.

    Used when packing user-prepared content (not a round-trip).  We have no
    way to recover the original `id` field or appendix2; we default to:
      - appendix2 = 0x00 (most archives)
      - id        = crc32(name) (placeholder; will NOT match originals)
      - order     = alphabetical
    """
    files = []
    for p in sorted(src_dir.rglob("*")):
        if not p.is_file() or p.name == MANIFEST_NAME:
            continue
        rel = p.relative_to(src_dir).as_posix()
        files.append(rel)

    entries = []
    for idx, rel in enumerate(files):
        # Mashed entries use the bare name (or backslash-separated path in
        # archives like Perm.piz); we keep the as-extracted path but flip
        # forward slashes to backslashes which is the dominant convention
        # observed in original archives.
        stored_name = rel.replace("/", "\\")
        entries.append(
            {
                "order": idx,
                "name": stored_name,
                "id": zlib.crc32(stored_name.encode("ascii")),
                "extracted_path": rel,
            }
        )

    return {
        "format": "piz",
        "version": VERSION,
        "appendix2": "00000000",
        "source_archive": None,
        "entries": entries,
    }


def pack_archive(src_dir: Path, out_path: Path) -> int:
    """Build a .piz from `src_dir`.  If `src_dir/_piz_manifest.json` exists, it
    is used to reproduce the original entry order, ids, and appendix2.
    Otherwise a manifest is synthesized (entry order alphabetical, id =
    crc32(name), appendix2 = 0x00000000) and the result will NOT round-trip
    byte-identical against an original archive.
    """
    manifest_path = src_dir / MANIFEST_NAME
    if manifest_path.exists():
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    else:
        manifest = _build_manifest_from_dir(src_dir)
        print(
            f"  WARNING: no {MANIFEST_NAME}; synthesizing manifest "
            "(ids and entry order will not match an original archive)",
            file=sys.stderr,
        )

    appendix2_hex = manifest.get("appendix2", "00000000")
    appendix2 = bytes.fromhex(appendix2_hex)
    if len(appendix2) != APPENDIX2_LEN:
        raise ValueError(
            f"manifest appendix2 {appendix2_hex!r} must encode {APPENDIX2_LEN} bytes"
        )

    # Preserve manifest-given entry order (matches original table order).
    entries = sorted(manifest["entries"], key=lambda e: e["order"])
    count = len(entries)

    # ---- Pre-compute layout (so we can fill the entry table before writing
    # data, mirroring how the original archives were laid out) ----
    table_region_end = HEADER_SIZE + count * ENTRY_SIZE
    data_start = ((table_region_end + SECTOR - 1) // SECTOR) * SECTOR

    blobs = []
    cur_off = data_start
    for e in entries:
        rel = e.get("extracted_path") or e["name"].replace("\\", "/")
        blob_path = src_dir / rel.replace("/", os.sep)
        if not blob_path.exists():
            raise FileNotFoundError(
                f"manifest references {rel!r} but {blob_path} is missing"
            )
        blob = blob_path.read_bytes()
        expected_size = e.get("size")
        if expected_size is not None and expected_size != len(blob):
            raise ValueError(
                f"size mismatch on {rel}: manifest={expected_size:#x} disk={len(blob):#x}"
            )
        expected_crc = e.get("crc32")
        if expected_crc is not None:
            got = f"{zlib.crc32(blob):08x}"
            if got != expected_crc:
                raise ValueError(
                    f"crc32 mismatch on {rel}: manifest={expected_crc} disk={got}"
                )
        blobs.append((e, blob, cur_off))
        next_off = cur_off + len(blob)
        rem = next_off % SECTOR
        if rem:
            next_off += SECTOR - rem
        cur_off = next_off

    final_size = cur_off  # already sector-aligned

    # ---- Assemble the buffer (pre-zeroed to satisfy all padding) ----
    buf = bytearray(final_size)
    buf[0:4] = MAGIC
    struct.pack_into("<II", buf, 4, VERSION, count)
    buf[APPENDIX2_OFFSET : APPENDIX2_OFFSET + APPENDIX2_LEN] = appendix2

    for i, (e, blob, off) in enumerate(blobs):
        row = HEADER_SIZE + i * ENTRY_SIZE
        name_bytes = e["name"].encode("ascii")
        if len(name_bytes) >= NAME_FIELD_LEN:
            raise ValueError(
                f"name {e['name']!r} exceeds {NAME_FIELD_LEN - 1} ASCII bytes"
            )
        buf[row : row + len(name_bytes)] = name_bytes
        struct.pack_into("<III", buf, row + 0x74, off, len(blob), int(e["id"]))

    for (e, blob, off) in blobs:
        buf[off : off + len(blob)] = blob

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(bytes(buf))
    print(f"packed {count} entries -> {out_path} ({final_size} bytes)")
    return 0


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def main(argv=None):
    p = argparse.ArgumentParser(description="Mashed .piz archive tool")
    sub = p.add_subparsers(dest="cmd", required=True)

    pl = sub.add_parser("list", help="list entries")
    pl.add_argument("archive", type=Path)

    pe = sub.add_parser(
        "extract",
        help="extract entries (writes a sidecar manifest unless --filter is used)",
    )
    pe.add_argument("archive", type=Path)
    pe.add_argument("-o", "--out", type=Path, required=True)
    pe.add_argument(
        "--filter",
        help="glob pattern, e.g. '*.tga'; suppresses the manifest sidecar",
    )

    pp = sub.add_parser(
        "pack",
        help="rebuild a .piz from an extracted directory (reads _piz_manifest.json)",
    )
    pp.add_argument("src_dir", type=Path)
    pp.add_argument("-o", "--out", type=Path, required=True)

    args = p.parse_args(argv)
    if args.cmd == "list":
        return list_archive(args.archive)
    if args.cmd == "extract":
        return extract_archive(args.archive, args.out, args.filter)
    if args.cmd == "pack":
        return pack_archive(args.src_dir, args.out)
    raise AssertionError(f"unknown command {args.cmd}")


if __name__ == "__main__":
    sys.exit(main())
