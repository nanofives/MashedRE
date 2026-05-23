# One-time on-disk patch of MASHED.exe to skip the HardwareShowIntroVideo playback.
#
# Background: MASHED plays multi-second intro/splash videos (supersonic.mpg,
# empire.mpg, intro.mpg) before reaching main menu. This makes
# canonical-observation Frida runs slower (BOOT_WAIT must clear intro), and
# slows manual testing.
#
# Approach: replace the entry byte of HardwareShowIntroVideo (FUN_00495350)
# with `C3` (RET). The function becomes a no-op — caller's stack stays clean
# (CALL pushed return addr, RET pops it; no prolog/epilog mismatch).
#
# Why not NOP the call site at 0x00402838: the next call (FUN_00494c80) is
# DirectShow filter-graph setup for the menu's small.mpg background video,
# which relies on COM apartment state initialized inside FUN_00495350's
# per-stage video loader (FUN_00494a80). NOPing just the intro call leaves
# the menu setup attempting CoCreateInstance against an uninitialized
# apartment and crashes. Replacing FUN_00495350 with a clean RET preserves
# the caller's flow while skipping all video playback.
#
# FUN_00495350 original prolog (16 bytes at file 0x95350):
#   55 8b ec 83 e4 f8 83 ec 40 53 55 56 57 33 db 53
#   = PUSH EBP; MOV EBP, ESP; AND ESP, ~7; SUB ESP, 0x40; PUSH EBX/EBP/ESI/EDI; XOR EBX, EBX; PUSH EBX
#
# Patch: write `C3` (RET) at file 0x95350. The function returns immediately.
# Per C2 attestation (analysis note re/analysis/promote_c2_launch_handshake/),
# FUN_00495350 is "void(void)" so the immediate RET is calling-convention safe.
#
# Result: MASHED still calls HardwareShowIntroVideo (no caller change), but
# the function returns immediately. Boot continues directly to font loading,
# menu setup, main menu — typically <1s vs ~8-15s with intro playback.
#
# Safety:
#   1. Original preserved at MASHED.exe.unpatched (created only on first run).
#   2. Pre-patch byte signature verified before writing; re-running is no-op.
#   3. Restore by removing the patched file and renaming .unpatched back.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

# File offset of FUN_00495350 entry. RVA 0x00495350 - imagebase 0x00400000
# = 0x00095350. .text is mapped 1:1 in this binary so file_off = RVA - 0x400000.
PATCH_OFFSET = 0x00095350  # in MASHED.exe file
PATCH_LEN    = 1

# Original prolog first byte: 0x55 (PUSH EBP).
# Replacement: 0xC3 (RET — function exits before doing anything).
PRE_PATCH  = bytes.fromhex('55')
POST_PATCH = bytes.fromhex('c3')

ANCHOR_SHA = 'bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e'


def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    h.update(path.read_bytes())
    return h.hexdigest()


def main() -> int:
    if not MASHED_EXE.exists():
        print(f'FATAL: {MASHED_EXE} not found.', file=sys.stderr)
        return 1

    # First-run backup. Don't overwrite an existing backup.
    if not BACKUP.exists():
        # Verify the source file matches the anchor SHA before backing it up.
        live = sha256_of(MASHED_EXE)
        if live != ANCHOR_SHA:
            print(
                f'WARNING: {MASHED_EXE.name} SHA-256 = {live}; expected anchor '
                f'{ANCHOR_SHA}. Refusing to back up — this might be a different '
                f'version OR a previously-patched binary missing its .unpatched.',
                file=sys.stderr,
            )
            return 2
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f'Backed up {MASHED_EXE.name} -> {BACKUP.name} (first run).')

    data = bytearray(MASHED_EXE.read_bytes())
    region = bytes(data[PATCH_OFFSET:PATCH_OFFSET + PATCH_LEN])

    if region == POST_PATCH:
        print(f'NOP: {MASHED_EXE.name} already patched at file 0x{PATCH_OFFSET:06x}; nothing to do.')
        return 0

    if region != PRE_PATCH:
        print(
            f'FATAL: {MASHED_EXE.name} at file 0x{PATCH_OFFSET:06x} has unexpected bytes:\n'
            f'  found:    {region.hex()}\n'
            f'  expected: {PRE_PATCH.hex()}\n'
            f'Refusing to patch — manually restore MASHED.exe.unpatched first.',
            file=sys.stderr,
        )
        return 3

    data[PATCH_OFFSET:PATCH_OFFSET + PATCH_LEN] = POST_PATCH
    MASHED_EXE.write_bytes(bytes(data))
    print(
        f'PATCHED: {MASHED_EXE.name} at file 0x{PATCH_OFFSET:06x} (RVA 0x{0x00400000 + PATCH_OFFSET:08x}):\n'
        f'  {PRE_PATCH.hex()}  ->  {POST_PATCH.hex()}\n'
        f'Intro/splash video calls are now NOPed; MASHED boots straight to menu.'
    )
    return 0


if __name__ == '__main__':
    sys.exit(main())
