# Unlock-all-TRACKS patch for MASHED.exe.
#
# Why this is needed (the getter patch in patch_mashed_unlock_all.py is NOT enough):
# the championship/track-select renderers (e.g. FUN_00439210) read the availability
# table at DAT_007f0a40 DIRECTLY (0 = locked, nonzero = available), bypassing the
# TrackAvailGet getter (0x430830). So patching the getter does nothing for what the
# screens draw. Runtime proof (re/frida/read_unlock_state.py): at the menu, the 13x12
# int32 table at 0x7f0a40 is the FUN_004924f0 fresh pattern (137 zeros) -> locked.
#
# The game's OWN "everything completed/unlocked" state is the whole table = 2 (see
# FUN_004927c0's championship-completion path: `for (0x9c dwords) *p = 2`). The state
# initializer FUN_004924f0 (called from FUN_00492370 on every boot) is what populates
# the table at the menu, so we append a fill loop to its tail:
#
#   code cave @ 0x005caf00 (end-of-.text zero padding, executable):
#       push eax/ecx/edx           ; preserve regs so FUN_00431d00 sees identical state
#       mov eax, 0x007f0a40
#       mov ecx, 0x9c (156)        ; 13 rows x 12 cols int32 = exactly 0x270 bytes
#       mov edx, 2
#     loop: mov [eax], edx ; add eax,4 ; dec ecx ; jnz loop
#       pop edx/ecx/eax
#       call 0x00431d00            ; displaced original call
#       ret
#   redirect @ 0x0049267f: CALL 0x00431d00 -> CALL 0x005caf00
#
# Cars are already unlocked by default (FUN_004924f0 sets every 0x7f0e50 byte = 1), so
# this + the getter patch covers "everything". Reversible via --restore (backup is the
# pristine original\MASHED.exe.preunlock_bak shared with patch_mashed_unlock_all.py).
# Never touches the original\MASHED.exe.unpatched version anchor. File offset = VA-0x400000.
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.preunlock_bak'

# --- redirect site: CALL 0x431d00 inside FUN_004924f0's tail -----------------
REDIR_OFF  = 0x9267f                       # VA 0x0049267f
REDIR_PRE  = bytes.fromhex('e87cf6f9ff')   # CALL 0x00431d00
REDIR_POST = bytes.fromhex('e87c881300')   # CALL 0x005caf00

# --- code cave: end-of-.text zero padding ------------------------------------
CAVE_OFF   = 0x1caf00                       # VA 0x005caf00
CAVE_PRE   = bytes(35)                      # must be 35 zero bytes
# push eax;push ecx;push edx; mov eax,0x7f0a40; mov ecx,0x9c; mov edx,2;
# loop: mov [eax],edx; add eax,4; dec ecx; jnz loop; pop edx;pop ecx;pop eax;
# call 0x431d00 (rel from 0x5caf22 = 0xffe66dde); ret
CAVE_POST  = bytes.fromhex('505152'
                           'b8400a7f00'
                           'b99c000000'
                           'ba02000000'
                           '8910'
                           '83c004'
                           '49'
                           '75f8'
                           '5a5958'
                           'e8de6de6ff'
                           'c3')

assert len(CAVE_POST) == 35, len(CAVE_POST)


def main(argv):
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")

    if '--restore' in argv:
        if not BACKUP.exists():
            sys.exit(f"no backup at {BACKUP.name}; cannot restore")
        shutil.copy2(BACKUP, MASHED_EXE)
        print(f"restored MASHED.exe from {BACKUP.name}  (reverts BOTH unlock patches)")
        print(f"  sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()[:16]}")
        return 0

    data = bytearray(MASHED_EXE.read_bytes())
    redir = bytes(data[REDIR_OFF:REDIR_OFF+5])
    cave  = bytes(data[CAVE_OFF:CAVE_OFF+35])

    if '--check' in argv:
        print(f"redirect @0x{REDIR_OFF:x}: {redir.hex()}  "
              f"({'PATCHED' if redir==REDIR_POST else 'pristine' if redir==REDIR_PRE else 'UNKNOWN'})")
        print(f"cave     @0x{CAVE_OFF:x}: {cave.hex()}  "
              f"({'PATCHED' if cave==CAVE_POST else 'empty' if cave==CAVE_PRE else 'UNKNOWN'})")
        return 0

    if redir == REDIR_POST and cave == CAVE_POST:
        print("already track-unlocked — no change.")
        return 0

    # Pre-flight asserts (abort on any surprise — wrong build or offset drift).
    if redir not in (REDIR_PRE, REDIR_POST):
        sys.exit(f"redirect site 0x{REDIR_OFF:x} = {redir.hex()} (expected {REDIR_PRE.hex()}). Abort.")
    if cave not in (CAVE_PRE, CAVE_POST):
        sys.exit(f"code cave 0x{CAVE_OFF:x} not free (got {cave.hex()[:16]}...). Abort.")

    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"backed up pre-unlock exe to {BACKUP.name}")

    data[CAVE_OFF:CAVE_OFF+35]  = CAVE_POST
    data[REDIR_OFF:REDIR_OFF+5] = REDIR_POST
    MASHED_EXE.write_bytes(bytes(data))
    print(f"UNLOCK-TRACKS applied. sha256: {hashlib.sha256(bytes(data)).hexdigest()[:16]}")
    print("  championship/track table (0x7f0a40) now filled with 2 (all available) at boot.")
    print("  revert with: py -3.12 scripts/patch_mashed_unlock_tracks.py --restore")
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
