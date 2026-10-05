"""
Reader/writer for Mashed `gamesave.bin` save files.

Format (from re/analysis/structs/gamesave_layout.md; first-pass session
save_gamesave_d3-20260511):

    File offset  Size          Content
    -----------  ----------    -------
    0x0000       4             Magic sentinel 0xDEADBEEF (LE uint32)
                               First-write RVA: 0x00404F37
                               MOV DWORD PTR [0x803358], 0xDEADBEEF
    0x0004       0x24A3C       Profile data block — REP MOVSD from
                               *DAT_008A94A8 (0x928F dwords = 150,076 bytes).
                               Internal layout TBD (UNCERTAIN U-3560);
                               treated as opaque bytes here.
    0x24A40      0x520         Championship / unlock table — REP MOVSD of 0x148
                               dwords from DAT_007F0A40 (RVA 0x00404F19).
                               Its first 13 rows x 0x30 bytes are decoded by
                               re/tools/gamesave_edit.py; the remaining 0x2B0
                               bytes are the rest of the same table.
                               +0x4BC within it (file 0x24EFC) takes the
                               save-state counter DAT_008A95AC (RVA 0x00404F23).
    0x24F60      0x40          The ONLY region Save::SerializeToBuffer does not
                               write. 64 bytes, not 0xB60.

    Total: 0x24FA0 (151,456) bytes.

    [CORRECTED 2026-10-05.] This header previously read "0x0004 0x2443C Profile"
    and "0x24440 0xB60 Tail — not written by Save::SerializeToBuffer ...
    unresolved (UNCERTAIN U-3559)". BOTH claims were wrong and both had already
    been corrected in the trackers:
      * PROFILE_SIZE was the live constant 0x2443C (148,540) while its own
        comment said 150,076. 0x928F dwords is 150,076 = 0x24A3C — a transposed
        digit. The two sizes still summed to 0x24FA0, which is why it went
        unnoticed, but the profile/tail BOUNDARY was 0x600 too low, so every
        caller got 0x600 bytes of profile prepended to `tail`. U-3560 already
        recorded the fix: "HEX CORRECTED: previously recorded as 0x2443C ...
        the decimal was right and the hex was wrong."
      * U-3559 has been RESOLVED since 2026-05-22: the region IS written by
        Save::SerializeToBuffer, by the two stores now described above, and
        "no second writer exists".
    The stale header outlived both corrections and misled
    verify/d4_save_20261005/PREREG_SAVE.md and then RESULT_SPAN.md. Keep this
    block in sync with UNCERTAINTIES.md, not the other way round.

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
# [CORRECTED 2026-10-05] was 0x2443C (148,540) with a comment claiming 150,076.
# 0x928F dwords IS 150,076 = 0x24A3C; the hex had a transposed digit, so the
# profile/tail boundary sat 0x600 too low and `tail` carried 0x600 bytes of
# profile. Corroborated three ways: the running total at DAT_008A94AC is checked
# against 0x24a3c at 0x004113b0, probe_save_globals.py:44 names that global
# PROFILE_SIZE_GLOBAL, and the profile then ends exactly at the championship
# table base 0x24A40 that gamesave_edit.py:10 documents.
PROFILE_SIZE: int = 0x24A3C  # 150,076 bytes; 0x928F dwords via REP MOVSD

# [CORRECTED 2026-10-05] U-3559 has been RESOLVED since 2026-05-22: this region IS
# written by Save::SerializeToBuffer and has no second writer. It is the 0x520-byte
# championship/unlock table (0x148 dwords from DAT_007F0A40, RVA 0x00404F19) plus a
# 0x40-byte residual that genuinely is never written. Kept as one opaque field so
# callers do not break; see the module docstring for the split.
TAIL_OFFSET: int = 0x24A40
TAIL_SIZE: int = 0x560      # 1,376 bytes = 0x520 champ table + 0x40 unwritten

FILE_SIZE: int = 0x24FA0    # 151,456 bytes total


# ---- Parsed structure -------------------------------------------------------


@dataclass
class GameSave:
    """Decomposed gamesave.bin.

    Fields documented in re/analysis/structs/gamesave_layout.md:

    magic         (uint32 LE)  Must equal 0xDEADBEEF.
                               First-write RVA 0x00404F37.
    profile_block (bytes)      0x24A3C-byte profile block.
                               Written by Save::SerializeToBuffer (RVA 0x00404EE0)
                               via REP MOVSD from *DAT_008A94A8.
                               Internal layout is opaque (UNCERTAIN U-3560).
    tail          (bytes)      0x560-byte region [0x24A40..0x24F9F]. IS written by
                               Save::SerializeToBuffer — U-3559 RESOLVED 2026-05-22,
                               no second writer exists. Splits as:
                                 +0x000..+0x520  championship / unlock table, 0x148
                                                 dwords from DAT_007F0A40 (0x00404F19);
                                                 its first 13 rows x 0x30 are decoded
                                                 by re/tools/gamesave_edit.py.
                                 +0x4BC          save-state counter DAT_008A95AC
                                                 (0x00404F23), file offset 0x24EFC.
                                 +0x520..+0x560  the only never-written bytes, 0x40.
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
