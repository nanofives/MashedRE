# On-disk patch: neutralize FUN_005bc750 (the audio COM init wrapper that
# crashes when its passed-in object pointer-to-pointer dereferences to NULL).
#
# Background: the natural spinning-disc crash on this Win11+RTX5070Ti machine
# turns out to be at 0x5bc76c (NOT inside the piz reader — that earlier
# diagnosis was Frida-Interceptor-induced). FUN_005bc750 calls
# (*(*(*param_1)))(...)  — a QueryInterface-style COM dispatch. When the
# caller passes `audio_obj + 0xa8` and that slot holds NULL, the inner
# `MOV ECX, [EAX]` with EAX=0 crashes.
#
# The simplest no-op patch: replace the function's first 3 bytes
#   0x005bc750  83 ec 34     SUB ESP, 0x34
# with
#   0x005bc750  33 c0        XOR EAX, EAX     (return value 0 = false)
#   0x005bc752  c3           RET
#
# This makes FUN_005bc750 immediately return false without touching ESP.
# Calling convention: stdcall-ish but the caller has `ADD ESP, 8` after
# the call (cleans up the 2 pushed args), so an early RET is balanced.
# The function's other 'success path' code at body_end is now unreachable
# but harmless.
#
# Effect: audio init "fails" gracefully on whichever object was un-init'd.
# Game should proceed past the spinning disc to wherever the next failure
# is (or to main menu).
import hashlib
import shutil
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
BACKUP     = ROOT / 'original' / 'MASHED.exe.unpatched'

PATCH_FILE_OFFSET = 0x1bc750   # RVA 0x005bc750 → file offset same (single .text section in this exe)
PRE_PATCH  = bytes([0x83, 0xec, 0x34])   # SUB ESP, 0x34
POST_PATCH = bytes([0x33, 0xc0, 0xc3])   # XOR EAX,EAX ; RET


def main():
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    data = MASHED_EXE.read_bytes()
    site = data[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 3]
    if site == POST_PATCH:
        print(f"  already patched at 0x{PATCH_FILE_OFFSET:x}")
        return 0
    if site != PRE_PATCH:
        sys.exit(f"  unexpected bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (expected {PRE_PATCH.hex()})")
    print(f"pre-patch bytes at 0x{PATCH_FILE_OFFSET:x}: {site.hex()} (SUB ESP, 0x34)")
    if not BACKUP.exists():
        shutil.copy2(MASHED_EXE, BACKUP)
        print(f"  backed up to {BACKUP.name}")
    else:
        print(f"  backup already exists at {BACKUP.name}")
    patched = bytearray(data)
    patched[PATCH_FILE_OFFSET:PATCH_FILE_OFFSET + 3] = POST_PATCH
    MASHED_EXE.write_bytes(bytes(patched))
    print(f"\npatched MASHED.exe sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()}")
    print(f"  3 bytes at 0x{PATCH_FILE_OFFSET:x} -> XOR EAX,EAX ; RET")
    print(f"  FUN_005bc750 now returns false without dereferencing arg")
    print(f"  restore: copy {BACKUP.name} over MASHED.exe")
    return 0


if __name__ == '__main__':
    sys.exit(main())
