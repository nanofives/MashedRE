# Unlock-everything (PERMANENT) patch for MASHED.exe — patches the SAVE-STATE RESTORE.
#
# Root cause (proven live 2026-06-02, re/frida/force_unlock_live.py): the menu reads the
# WORKING arrays 0x007f0a40 (track/cup table, 13x12 int32; 0=locked, nonzero=available)
# and 0x007f0e50 (car flags, 156 bytes; 1=unlocked). On entering any game mode,
# FUN_00404e80 (the save-state restore) REP-MOVSDs 0x148 dwords from the loaded save
# snapshot DAT_00827d98 over 0x007f0a40..0x007f0f60 — which spans BOTH arrays — so a
# fresh all-zero save re-locks everything. Patching the boot init (FUN_004924f0) or the
# getters (0x42ef40/0x430830) does NOT help because this restore overwrites them and the
# renderers read the arrays directly. Forcing 0x7f0a40=2 and 0x7f0e50=1 live unlocked
# every screen (cars, tracks, challenges) — confirmed by the user.
#
# This patch redirects the restore's tail so it ALWAYS leaves the arrays unlocked:
#   redirect @ 0x00404eb4 (6B: MOV EDI,[0x008a94a8]) -> JMP cave2 (+ 1 NOP)
#   cave2 @ 0x005caf30 (end-of-.text zero padding, executable):
#       push eax/ecx/edx
#       fill 0x007f0a40 with 2  (156 int32)      ; tracks/cups available
#       fill 0x007f0e50 with 1  (156 bytes)      ; all cars unlocked
#       pop edx/ecx/eax
#       mov edi,[0x008a94a8]                      ; displaced original instruction
#       jmp 0x00404eba                            ; back into FUN_00404e80
# Flags are re-set by the TEST EDI,EDI at 0x404eba, so the fill's flags don't leak.
#
# Reversible via --restore (backup original\MASHED.exe.preunlock_bak, shared with the
# other unlock scripts). Never touches the original\MASHED.exe.unpatched anchor.
# Bonus Features (main-menu MenuOptionSlotGet 0x430910) is a SEPARATE gate, NOT covered.
# File offset = VA - 0x400000.
import hashlib, shutil, sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.preunlock_bak'

IMAGE_BASE = 0x400000
CAVE_VA    = 0x5caf30
REDIR_VA   = 0x404eb4
RET_VA     = 0x404eba                      # where the cave jumps back to (TEST EDI,EDI)

def va2off(va): return va - IMAGE_BASE

def rel32(frm_next, to):                   # signed 32-bit branch displacement
    return (to - frm_next) & 0xffffffff

# ---- cave2 machine code -----------------------------------------------------
# offsets within the cave are fixed; only the trailing JMP rel32 depends on CAVE_VA
cave = bytearray()
cave += bytes.fromhex('505152')                 # push eax;push ecx;push edx
cave += bytes.fromhex('b8400a7f00')             # mov eax,0x7f0a40
cave += bytes.fromhex('b99c000000')             # mov ecx,156
cave += bytes.fromhex('ba02000000')             # mov edx,2
cave += bytes.fromhex('8910')                   # L1: mov [eax],edx
cave += bytes.fromhex('83c004')                 #     add eax,4
cave += bytes.fromhex('49')                     #     dec ecx
cave += bytes.fromhex('75f8')                   #     jnz L1
cave += bytes.fromhex('b8500e7f00')             # mov eax,0x7f0e50
cave += bytes.fromhex('b99c000000')             # mov ecx,156
cave += bytes.fromhex('c60001')                 # L2: mov byte[eax],1
cave += bytes.fromhex('40')                     #     inc eax
cave += bytes.fromhex('49')                     #     dec ecx
cave += bytes.fromhex('75f9')                   #     jnz L2
cave += bytes.fromhex('5a5958')                 # pop edx;pop ecx;pop eax
cave += bytes.fromhex('8b3da8948a00')           # mov edi,[0x8a94a8]  (displaced)
jmp_at_next = CAVE_VA + len(cave) + 5           # addr after the E9 rel32
cave += b'\xe9' + rel32(jmp_at_next, RET_VA).to_bytes(4, 'little')   # jmp 0x404eba
CAVE_POST = bytes(cave)
CAVE_PRE  = bytes(len(CAVE_POST))               # expect that many zero bytes

# ---- redirect: 6 bytes at 0x404eb4 -> E9 rel32 (jmp cave) + 90 (nop) --------
REDIR_PRE  = bytes.fromhex('8b3da8948a00')      # MOV EDI,[0x008a94a8]
REDIR_POST = b'\xe9' + rel32(REDIR_VA + 5, CAVE_VA).to_bytes(4, 'little') + b'\x90'

CAVE_OFF  = va2off(CAVE_VA)
REDIR_OFF = va2off(REDIR_VA)


def main(argv):
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")

    if '--restore' in argv:
        if not BACKUP.exists():
            sys.exit(f"no backup at {BACKUP.name}; cannot restore")
        shutil.copy2(BACKUP, MASHED_EXE)
        print(f"restored MASHED.exe from {BACKUP.name} (reverts ALL unlock patches)")
        return 0

    data  = bytearray(MASHED_EXE.read_bytes())
    redir = bytes(data[REDIR_OFF:REDIR_OFF+6])
    cavev = bytes(data[CAVE_OFF:CAVE_OFF+len(CAVE_POST)])

    if '--check' in argv:
        print(f"redirect @0x{REDIR_OFF:x}: {redir.hex()} "
              f"({'PATCHED' if redir==REDIR_POST else 'pristine' if redir==REDIR_PRE else 'UNKNOWN'})")
        print(f"cave2    @0x{CAVE_OFF:x}: {cavev.hex()} "
              f"({'PATCHED' if cavev==CAVE_POST else 'empty' if cavev==CAVE_PRE else 'UNKNOWN'})")
        print(f"cave2 bytes to write ({len(CAVE_POST)}): {CAVE_POST.hex()}")
        print(f"redir bytes to write: {REDIR_POST.hex()}")
        return 0

    if redir == REDIR_POST and cavev == CAVE_POST:
        print("already restore-unlocked — no change.")
        return 0
    if redir not in (REDIR_PRE, REDIR_POST):
        sys.exit(f"redirect 0x{REDIR_OFF:x}={redir.hex()} (want {REDIR_PRE.hex()}). Abort.")
    if cavev not in (CAVE_PRE, CAVE_POST):
        sys.exit(f"cave 0x{CAVE_OFF:x} not free ({cavev.hex()[:16]}...). Abort.")

    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"backed up to {BACKUP.name}")

    data[CAVE_OFF:CAVE_OFF+len(CAVE_POST)] = CAVE_POST
    data[REDIR_OFF:REDIR_OFF+6]            = REDIR_POST
    MASHED_EXE.write_bytes(bytes(data))
    print(f"UNLOCK-RESTORE applied. sha256: {hashlib.sha256(bytes(data)).hexdigest()[:16]}")
    print("  save-restore now leaves 0x7f0a40=2 (tracks) + 0x7f0e50=1 (cars) on every mode-entry.")
    print("  revert: py -3.12 scripts/patch_mashed_unlock_restore.py --restore")
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
