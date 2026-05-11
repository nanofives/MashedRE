# On-disk patch: make Mashed's video-mode dropdown include [Window] modes.
#
# The selector populator FUN_00498d60 filters out any mode where the flags
# field (mode+0xc) is zero — and windowed modes have flags=0, so they never
# appear in the dropdown. Patch:
#
#   0x00498dbc  75 44      JNZ 0x00498e02   <-- skip if flags==0
#
# becomes
#
#   0x00498dbc  90 90      NOP NOP          <-- never skip on flags alone
#
# After the patch, the width/height filters (>=640, >=480) are still in
# effect, so the dropdown shows all "decent-size" modes including [Window]
# variants. User picks a windowed entry → game launches windowed → no
# monitor flicker between launches.
#
# This patch is INDEPENDENT of the powerups skip (different file offset).
# Safe to combine with scripts/patch_mashed_skip_powerups.py.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

# Patch site: 0x00498dbc. .text section maps RVA→file offset 1:1 in this
# binary (verified by the powerups patch at 0x295d landing correctly).
PATCH_FILE_OFFSET = 0x98dbc
PRE_PATCH  = bytes([0x75, 0x44])   # JNZ +0x44
POST_PATCH = bytes([0x90, 0x90])   # NOP NOP
ANCHOR_SHA = 'bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e'


def main():
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")

    data = MASHED_EXE.read_bytes()
    cur_sha = hashlib.sha256(data).hexdigest()
    print(f"current MASHED.exe sha256: {cur_sha}")

    site = data[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 2]
    if site == POST_PATCH:
        print(f"  already patched at 0x{PATCH_FILE_OFFSET:x} (NOP NOP)")
        return 0
    if site != PRE_PATCH:
        sys.exit(f"  unexpected bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (expected {PRE_PATCH.hex()})")

    print(f"pre-patch bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (JNZ filter on windowed)")

    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"  backed up to {BACKUP.name}")
    else:
        print(f"  backup already exists at {BACKUP.name}")

    patched = bytearray(data)
    patched[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 2] = POST_PATCH
    MASHED_EXE.write_bytes(bytes(patched))

    post_sha = hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()
    print(f"\npatched MASHED.exe sha256: {post_sha}")
    print(f"  2 bytes at 0x{PATCH_FILE_OFFSET:x} -> NOP NOP")
    print(f"  windowed modes will now appear in the Select Video Mode dropdown")
    print(f"  restore: copy {BACKUP.name} over MASHED.exe")
    return 0


if __name__ == '__main__':
    sys.exit(main())
