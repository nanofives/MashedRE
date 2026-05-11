# On-disk patch: skip the Select Controllers dialog at boot.
#
# Same pattern as skip_selector: FUN_00498510(param_1, param_2) shows a
# DialogBoxParamA when param_1 != 0. The caller FUN_00495150 currently
# passes the global [0x006147c0] as param_1:
#
#   0x004951aa  a1 c0 47 61 00     MOV EAX, [0x006147c0]
#   0x004951af  50                 PUSH EAX
#   0x004951b0  e8 5b 33 00 00     CALL 0x00498510
#
# Force EAX = 0 so the dialog never fires:
#
#   0x004951aa  33 c0              XOR EAX, EAX
#   0x004951ac  90 90 90           NOP NOP NOP
#
# The controller config loader (FUN_004971b0) still runs and tries to read
# contcfg*.bin files. If they don't exist the function returns 0; the parent
# falls back to defaults. Either way, no dialog.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

PATCH_FILE_OFFSET = 0x951aa
PRE_PATCH  = bytes([0xa1, 0xc0, 0x47, 0x61, 0x00])   # MOV EAX, [0x006147c0]
POST_PATCH = bytes([0x33, 0xc0, 0x90, 0x90, 0x90])   # XOR EAX,EAX ; NOP NOP NOP


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
    print(f"pre-patch bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (MOV EAX,[0x6147c0])")
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
    print(f"  controller selector dialog now SKIPPED")
    return 0


if __name__ == '__main__':
    sys.exit(main())
