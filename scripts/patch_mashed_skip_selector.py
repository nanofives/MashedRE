# On-disk patch: skip the Select Video Mode selector dialog entirely, always
# use the videocfg.bin values for boot.
#
# FUN_004951f0 calls FUN_00499400(arg). FUN_00499400 has a gate:
#   if (arg == 0) silent_path_using_videocfg();
#   else          show_selector_dialog();
#
# The caller currently reads a global flag at 0x006147bc and passes it as arg:
#   0x004951f0  a1 bc 47 61 00     MOV EAX, [0x006147bc]
#   0x004951f5  50                 PUSH EAX
#   0x004951f6  e8 05 42 00 00     CALL 0x00499400
#
# We patch the MOV to force EAX = 0 unconditionally:
#   0x004951f0  33 c0              XOR EAX, EAX     ; EAX = 0 always
#   0x004951f2  90 90 90           NOP NOP NOP      ; pad to original 5 bytes
#
# Effect: FUN_00499400 always takes the silent path → load videocfg.bin →
# use whatever's pinned there → boot without dialog.
#
# Requires a valid pre-existing videocfg.bin (otherwise the silent path's
# fallback may misbehave). Use scripts/canonical/videocfg_windowed.bin or
# canonical/videocfg_640x480.bin.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

PATCH_FILE_OFFSET = 0x951f0
PRE_PATCH  = bytes([0xa1, 0xbc, 0x47, 0x61, 0x00])           # MOV EAX, [0x006147bc]
POST_PATCH = bytes([0x33, 0xc0, 0x90, 0x90, 0x90])           # XOR EAX,EAX ; NOP NOP NOP


def main():
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    data = MASHED_EXE.read_bytes()
    site = data[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 5]
    if site == POST_PATCH:
        print(f"  already patched at 0x{PATCH_FILE_OFFSET:x}")
        return 0
    if site != PRE_PATCH:
        sys.exit(f"  unexpected bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (expected {PRE_PATCH.hex()})")
    print(f"pre-patch bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (MOV EAX,[0x6147bc])")
    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"  backed up to {BACKUP.name}")
    else:
        print(f"  backup already exists at {BACKUP.name}")
    patched = bytearray(data)
    patched[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 5] = POST_PATCH
    MASHED_EXE.write_bytes(bytes(patched))
    print(f"\npatched MASHED.exe sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()}")
    print(f"  5 bytes at 0x{PATCH_FILE_OFFSET:x} -> XOR EAX,EAX ; NOP NOP NOP")
    print(f"  selector dialog is now SKIPPED — always loads videocfg.bin")
    print()
    print(f"make sure original/videocfg.bin exists and has valid mode values:")
    print(f"  cp scripts/canonical/videocfg_windowed.bin original/videocfg.bin")
    return 0


if __name__ == '__main__':
    sys.exit(main())
