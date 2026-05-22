"""Round-trip test for gamesave_parse.py.

Verifies that parsing then re-encoding original/gamesave.bin produces a buffer
byte-identical to the original file.

Usage (from repo root):
    py -3.12 -m pytest re/tools/tests/test_gamesave_roundtrip.py -v
"""
import struct
import sys
from pathlib import Path

import pytest

# ---------------------------------------------------------------------------
# Path setup: import gamesave_parse from the sibling re/tools/ directory.
# ---------------------------------------------------------------------------

THIS_DIR = Path(__file__).resolve().parent
TOOLS_DIR = THIS_DIR.parent
REPO_ROOT = TOOLS_DIR.parent.parent
sys.path.insert(0, str(TOOLS_DIR))

import gamesave_parse as gp  # noqa: E402

# ---------------------------------------------------------------------------
# Fixture: the shipped gamesave.bin
# ---------------------------------------------------------------------------

REFERENCE_PATH = REPO_ROOT / "original" / "gamesave.bin"


def _resolve_reference() -> Path:
    """Locate original/gamesave.bin; try a few candidate roots for worktrees."""
    candidates = [
        REPO_ROOT / "original" / "gamesave.bin",
        REPO_ROOT.parent.parent / "original" / "gamesave.bin",
        Path("C:/Users/maria/Desktop/Proyectos/Mashed/original/gamesave.bin"),
    ]
    for c in candidates:
        if c.exists():
            return c
    return REFERENCE_PATH  # will fail later with a clear FileNotFoundError


@pytest.fixture(scope="module")
def original_bytes() -> bytes:
    path = _resolve_reference()
    if not path.exists():
        pytest.skip(f"reference file not found: {path}")
    return path.read_bytes()


@pytest.fixture(scope="module")
def parsed(original_bytes) -> gp.GameSave:
    return gp.parse(original_bytes)


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------


class TestFileStructure:
    """Structural checks against the layout documented in
    re/analysis/structs/gamesave_layout.md."""

    def test_file_size(self, original_bytes):
        """File must be exactly 0x24FA0 (151,456) bytes."""
        assert len(original_bytes) == gp.FILE_SIZE, (
            f"expected {gp.FILE_SIZE:#x} bytes, got {len(original_bytes):#x}"
        )

    def test_magic(self, parsed):
        """Magic sentinel at offset 0 must be 0xDEADBEEF (first-write RVA 0x00404F37)."""
        assert parsed.magic == gp.MAGIC_VALUE, (
            f"expected magic {gp.MAGIC_VALUE:#010x}, got {parsed.magic:#010x}"
        )

    def test_magic_bytes_le(self, original_bytes):
        """Magic must encode as EF BE AD DE (little-endian) at file offset 0."""
        assert original_bytes[:4] == b"\xef\xbe\xad\xde"

    def test_profile_block_size(self, parsed):
        """Profile block (offset 0x0004) must be 0x2443C bytes."""
        assert len(parsed.profile_block) == gp.PROFILE_SIZE

    def test_tail_size(self, parsed):
        """Tail region (offset 0x24440) must be 0xB60 bytes."""
        assert len(parsed.tail) == gp.TAIL_SIZE

    def test_tail_first_nonzero_offset(self, parsed):
        """First non-zero byte in tail must be at tail-relative offset 0x604
        (file offset 0x24A44), per gamesave_layout.md observation."""
        for i in range(0x604):
            assert parsed.tail[i] == 0, (
                f"expected tail[{i:#x}] == 0, got {parsed.tail[i]:#x}"
            )
        # Byte at tail offset 0x604 should be non-zero.
        assert parsed.tail[0x604] != 0, (
            "expected first non-zero byte at tail[0x604] but it is zero"
        )


class TestRoundTrip:
    """Core criterion 4 tests: parse -> build -> assert byte-identical."""

    def test_roundtrip_byte_identical(self, original_bytes, parsed):
        """build(parse(buf)) must equal buf exactly."""
        rebuilt = gp.build(parsed)
        assert rebuilt == original_bytes, (
            _roundtrip_diff_message(original_bytes, rebuilt)
        )

    def test_roundtrip_length(self, original_bytes, parsed):
        """Rebuilt buffer length must match original."""
        rebuilt = gp.build(parsed)
        assert len(rebuilt) == len(original_bytes)

    def test_roundtrip_magic_preserved(self, parsed):
        """Magic must survive a parse -> build cycle unchanged."""
        gs2 = gp.parse(gp.build(parsed))
        assert gs2.magic == parsed.magic

    def test_roundtrip_profile_block_preserved(self, parsed):
        """Profile block must survive a parse -> build cycle unchanged."""
        gs2 = gp.parse(gp.build(parsed))
        assert gs2.profile_block == parsed.profile_block

    def test_roundtrip_tail_preserved(self, parsed):
        """Tail must survive a parse -> build cycle unchanged."""
        gs2 = gp.parse(gp.build(parsed))
        assert gs2.tail == parsed.tail


class TestValidation:
    """parse() and build() must reject malformed inputs."""

    def test_parse_rejects_wrong_size_short(self):
        with pytest.raises(ValueError, match="must be exactly"):
            gp.parse(b"\x00" * (gp.FILE_SIZE - 1))

    def test_parse_rejects_wrong_size_long(self):
        with pytest.raises(ValueError, match="must be exactly"):
            gp.parse(b"\x00" * (gp.FILE_SIZE + 1))

    def test_parse_rejects_bad_magic(self):
        buf = bytearray(gp.FILE_SIZE)
        # Write a wrong magic value.
        struct.pack_into("<I", buf, 0, 0xCAFEBABE)
        with pytest.raises(ValueError, match="bad magic"):
            gp.parse(bytes(buf))

    def test_build_rejects_short_profile_block(self):
        gs = gp.GameSave(
            magic=gp.MAGIC_VALUE,
            profile_block=b"\x00" * (gp.PROFILE_SIZE - 1),
            tail=b"\x00" * gp.TAIL_SIZE,
        )
        with pytest.raises(ValueError, match="profile_block"):
            gp.build(gs)

    def test_build_rejects_short_tail(self):
        gs = gp.GameSave(
            magic=gp.MAGIC_VALUE,
            profile_block=b"\x00" * gp.PROFILE_SIZE,
            tail=b"\x00" * (gp.TAIL_SIZE - 1),
        )
        with pytest.raises(ValueError, match="tail"):
            gp.build(gs)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _roundtrip_diff_message(original: bytes, rebuilt: bytes) -> str:
    """Return a diagnostic string pointing at the first differing byte."""
    for i, (a, b) in enumerate(zip(original, rebuilt)):
        if a != b:
            return (
                f"first byte mismatch at offset {i:#x}: "
                f"original={a:#04x} rebuilt={b:#04x}"
            )
    if len(original) != len(rebuilt):
        return (
            f"length mismatch: original={len(original):#x} rebuilt={len(rebuilt):#x}"
        )
    return "buffers are identical (no diff — this should not happen)"
