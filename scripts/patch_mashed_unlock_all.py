# Unlock-everything patch for MASHED.exe — makes all vehicles and all tracks
# available so a race can be started from a fresh save.
#
# The fresh/shipped gamesave.bin is all-zero (no progression), so the game
# gates content via two getters that read the (empty) unlock/championship
# state:
#
#   0x0042ef40  VehicleUnlockFlagGet  — returns 1 iff the car's unlock byte
#                                       (DAT_007f0e50 + idx*0xc) == 1. 5 callers,
#                                       all car-select / HUD availability checks.
#   0x00430830  TrackAvailGet         — returns a championship-table dword
#                                       (DAT_007f0a40, per game-mode column); the
#                                       track-select code treats NON-ZERO as
#                                       "available" (test eax; jne accept). 9
#                                       callers, all track-availability checks.
#
# Both are __cdecl (plain RET, caller cleans args). We overwrite each function
# entry with:
#     B8 01 00 00 00   MOV EAX, 1
#     C3               RET
# so every car reads as unlocked and every track reads as available, in every
# game mode. The rest of each function becomes dead code (never reached).
#
# This does NOT touch the boot patches; it is a separate, reversible overlay.
# Backup: original/MASHED.exe.preunlock_bak (the pre-unlock exe). Restore with
# `--restore`. The version anchor original/MASHED.exe.unpatched is never touched.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.preunlock_bak'

ALWAYS_RETURN_1 = bytes([0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3])  # MOV EAX,1 ; RET

# file_offset : (pre-patch first-6-bytes, label)  — .text maps RVA-0x400000 here.
SITES = {
    0x2ef40: (bytes.fromhex('8b4c240833c0'), 'VehicleUnlockFlagGet 0x0042ef40 (all cars)'),
    0x30830: (bytes.fromhex('a1fce9670083'), 'TrackAvailGet 0x00430830 (all tracks)'),
}


def main(argv):
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    restore = '--restore' in argv
    data = bytearray(MASHED_EXE.read_bytes())

    if restore:
        if not BACKUP.exists():
            sys.exit(f"no backup at {BACKUP.name}; cannot restore")
        shutil.copy2(BACKUP, MASHED_EXE)
        print(f"restored MASHED.exe from {BACKUP.name}")
        print(f"  sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()[:16]}")
        return 0

    # Idempotency / state check.
    pristine = all(bytes(data[off:off+6]) == pre for off, (pre, _) in SITES.items())
    patched  = all(bytes(data[off:off+6]) == ALWAYS_RETURN_1 for off in SITES)
    if patched:
        print("already unlocked (both getters patched) — no change.")
        return 0
    if not pristine:
        bad = [hex(off) for off, (pre, _) in SITES.items()
               if bytes(data[off:off+6]) not in (pre, ALWAYS_RETURN_1)]
        sys.exit(f"unexpected bytes at {bad} — different build or mid-patch? Abort.")

    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"backed up pre-unlock exe to {BACKUP.name}")

    for off, (pre, label) in SITES.items():
        data[off:off+6] = ALWAYS_RETURN_1
        print(f"  patched {label}  @ file 0x{off:x} -> MOV EAX,1 ; RET")

    MASHED_EXE.write_bytes(bytes(data))
    print(f"\nUNLOCK-ALL applied. MASHED.exe sha256: {hashlib.sha256(bytes(data)).hexdigest()[:16]}")
    print("  all vehicles unlocked + all tracks available in every mode.")
    print("  revert with: py -3.12 scripts/patch_mashed_unlock_all.py --restore")
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
