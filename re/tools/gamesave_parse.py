"""
Reader/writer for Mashed `gamesave.bin` save files.

Format (from re/analysis/structs/gamesave_layout.md; first-pass session
save_gamesave_d3-20260511):

    File offset  Size          Content
    -----------  ----------    -------
    0x0000       4             Magic sentinel 0xDEADBEEF (LE uint32)
                               First-write RVA: 0x00404F37
                               MOV DWORD PTR [0x803358], 0xDEADBEEF
    0x0004       0x2443C       Profile data block — REP MOVSD from
                               *DAT_008A94A8 (0x928F dwords).
                               Internal layout TBD (UNCERTAIN U-3560);
                               treated as opaque bytes here.
    0x24440      0xB60         Tail — not written by Save::SerializeToBuffer;
                               shipped with non-zero defaults.
                               Who writes it is unresolved (UNCERTAIN U-3559);
                               treated as opaque bytes here.

    Total: 0x24FA0 (151,456) bytes.

Key serialize / deserialize RVAs (original MASHED.exe):
    0x00404EE0  Save::SerializeToBuffer   game state -> save_buf
    0x00404E80  Save::DeserializeFromBuffer save_buf -> game state
    0x00404F50  SAVE_WRITE_FN             save_buf -> gamesave.bin
    0x00404E50  SAVE_LOAD_FN              gamesave.bin -> save_buf

Reference file: original/gamesave.bin (shipped with game; never mutate it
directly — copy to a work path first).
"""
import argparse
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

# ---- Layout constants (from gamesave_layout.md) ----------------------------

MAGIC_VALUE: int = 0xDEADBEEF
MAGIC_OFFSET: int = 0x0000
MAGIC_SIZE: int = 4

PROFILE_OFFSET: int = 0x0004
PROFILE_SIZE: int = 0x2443C  # 150,076 bytes; 0x928F dwords via REP MOVSD

TAIL_OFFSET: int = 0x24440
TAIL_SIZE: int = 0xB60      # 2,912 bytes; UNCERTAIN U-3559

FILE_SIZE: int = 0x24FA0    # 151,456 bytes total


# ---- Parsed structure -------------------------------------------------------


@dataclass
class GameSave:
    """Decomposed gamesave.bin.

    Fields documented in re/analysis/structs/gamesave_layout.md:

    magic         (uint32 LE)  Must equal 0xDEADBEEF.
                               First-write RVA 0x00404F37.
    profile_block (bytes)      0x2443C-byte profile block.
                               Written by Save::SerializeToBuffer (RVA 0x00404EE0)
                               via REP MOVSD from *DAT_008A94A8.
                               Internal layout is opaque (UNCERTAIN U-3560).
    tail          (bytes)      0xB60-byte tail region [0x24440..0x24F9F].
                               Not written by Save::SerializeToBuffer.
                               Who writes it is unresolved (UNCERTAIN U-3559).
                               Contains sparse championship-progression flags,
                               track-ID sequences, float arrays, and byte-flag
                               runs as documented in gamesave_layout.md §Tail.
    """

    magic: int          # uint32 LE; always 0xDEADBEEF
    profile_block: bytes  # opaque; 0x2443C bytes
    tail: bytes           # opaque; 0xB60 bytes


# ---- Core parse / build  ----------------------------------------------------


def parse(buf: bytes) -> GameSave:
    """Decompose a raw gamesave.bin buffer into a :class:`GameSave`.

    Raises ``ValueError`` on any structural violation (wrong size, bad magic).
    """
    if len(buf) != FILE_SIZE:
        raise ValueError(
            f"gamesave.bin must be exactly {FILE_SIZE:#x} ({FILE_SIZE}) bytes; "
            f"got {len(buf):#x} ({len(buf)}) bytes"
        )

    (magic,) = struct.unpack_from("<I", buf, MAGIC_OFFSET)
    if magic != MAGIC_VALUE:
        raise ValueError(
            f"bad magic at offset {MAGIC_OFFSET:#x}: "
            f"expected {MAGIC_VALUE:#010x}, got {magic:#010x}"
        )

    profile_block = buf[PROFILE_OFFSET : PROFILE_OFFSET + PROFILE_SIZE]
    tail = buf[TAIL_OFFSET : TAIL_OFFSET + TAIL_SIZE]

    return GameSave(magic=magic, profile_block=profile_block, tail=tail)


def build(gs: GameSave) -> bytes:
    """Recompose a :class:`GameSave` to raw bytes.

    The result is byte-identical to the original input provided ``gs`` was
    produced by :func:`parse` without modification.

    Raises ``ValueError`` if any field has an unexpected size.
    """
    if len(gs.profile_block) != PROFILE_SIZE:
        raise ValueError(
            f"profile_block must be {PROFILE_SIZE:#x} bytes; "
            f"got {len(gs.profile_block):#x}"
        )
    if len(gs.tail) != TAIL_SIZE:
        raise ValueError(
            f"tail must be {TAIL_SIZE:#x} bytes; got {len(gs.tail):#x}"
        )

    buf = bytearray(FILE_SIZE)
    struct.pack_into("<I", buf, MAGIC_OFFSET, gs.magic)
    buf[PROFILE_OFFSET : PROFILE_OFFSET + PROFILE_SIZE] = gs.profile_block
    buf[TAIL_OFFSET : TAIL_OFFSET + TAIL_SIZE] = gs.tail
    return bytes(buf)


# ---- CLI helpers  -----------------------------------------------------------


def _bytes_summary(data: bytes, label: str) -> str:
    """Return a human-readable summary for a large opaque byte field."""
    return f"<bytes len=0x{len(data):x} ({len(data)}) hex_prefix={data[:16].hex()}>"


def _gs_to_json_dict(gs: GameSave) -> dict:
    """Convert a GameSave to a JSON-serialisable dict (opaque bytes as summaries)."""
    return {
        "magic": f"{gs.magic:#010x}",
        "profile_block": _bytes_summary(gs.profile_block, "profile_block"),
        "tail": _bytes_summary(gs.tail, "tail"),
    }


# ---- CLI entry points  ------------------------------------------------------


def _cmd_parse(path: Path) -> int:
    buf = path.read_bytes()
    gs = parse(buf)
    d = _gs_to_json_dict(gs)
    # Print as a pseudo-JSON (real json.dumps would need special encoding; we
    # keep a readable flat format consistent with piz_extract.py style).
    print(f"file:          {path}")
    print(f"size:          {len(buf):#x} ({len(buf)})")
    print(f"magic:         {d['magic']}")
    print(f"profile_block: {d['profile_block']}")
    print(f"tail:          {d['tail']}")
    return 0


def _cmd_roundtrip(path: Path) -> int:
    original = path.read_bytes()
    gs = parse(original)
    rebuilt = build(gs)
    if original == rebuilt:
        print(f"[PASS] {path}: round-trip byte-identical ({len(original):#x} bytes)")
        return 0
    # Find the first differing byte to aid diagnostics.
    for i, (a, b) in enumerate(zip(original, rebuilt)):
        if a != b:
            print(
                f"[FAIL] {path}: first byte mismatch at offset {i:#x} "
                f"(original={a:#04x} rebuilt={b:#04x})",
                file=sys.stderr,
            )
            return 1
    if len(original) != len(rebuilt):
        print(
            f"[FAIL] {path}: length mismatch "
            f"(original={len(original):#x} rebuilt={len(rebuilt):#x})",
            file=sys.stderr,
        )
        return 1
    # Should not be reachable.
    print("[FAIL] bytes differ but no position found", file=sys.stderr)
    return 1


def main(argv=None):
    p = argparse.ArgumentParser(
        description="Mashed gamesave.bin parser and round-trip verifier"
    )
    sub = p.add_subparsers(dest="cmd", required=True)

    pp = sub.add_parser("parse", help="show structured fields from gamesave.bin")
    pp.add_argument("file", type=Path, metavar="file.bin")

    pr = sub.add_parser(
        "roundtrip",
        help="parse then re-encode; assert byte-identical to original",
    )
    pr.add_argument("file", type=Path, metavar="file.bin")

    args = p.parse_args(argv)
    if args.cmd == "parse":
        return _cmd_parse(args.file)
    if args.cmd == "roundtrip":
        return _cmd_roundtrip(args.file)
    raise AssertionError(f"unknown command {args.cmd!r}")


if __name__ == "__main__":
    sys.exit(main())
