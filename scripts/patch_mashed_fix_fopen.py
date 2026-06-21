# ROBUST-FOPEN boot fix (2026-06-20). Root-level companion to patch_mashed_disable_log.py.
#
# Post-2026-06-10, MASHED's fopen wrapper FUN_004a4541 = _fsopen("file","mode",0x40)
# returns a GARBAGE FILE* (~0x5453, i.e. < 0x10000) on direct launch, to ALL six
# callers (boot is a lottery over which caller runs first). Valid FILE*s are always
# >= 0x10000 (the static _iob in .data and heap allocs are high; the null page is
# 0..0x10000). So: validate the result and return NULL when it's < 0x10000; every
# caller already guards `if (fp != NULL)`, so they skip cleanly instead of crashing.
#
# FUN_004a4541 has no trailing slack, so we redirect it (jmp) to a code cave at RVA
# 0x50f4c4 (0xCC padding) holding: the original _fsopen call + the < 0x10000 check.
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
#
# Usage: py -3.12 scripts/patch_mashed_fix_fopen.py [--restore]
import sys, struct, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.prefopenpatch"

IMG = 0x00400000
FOPEN_RVA = 0x004a4541          # FUN_004a4541 (fopen wrapper)
CAVE_RVA  = 0x0050f6c4          # 60-byte 0xCC cave (file 0x10f6c4)
def f(rva): return rva - IMG    # file offset (1:1 .text)


def build():
    b = bytearray(EXE.read_bytes())
    # sanity: fopen wrapper original prologue `6a 40 ff 74 24 0c`
    if bytes(b[f(FOPEN_RVA):f(FOPEN_RVA)+6]) == bytes.fromhex("6a40ff7424"+"0c"):
        pass
    # resolve __fsopen absolute from the wrapper's original `call rel32` at FOPEN+10
    call_site = FOPEN_RVA + 10
    rel = struct.unpack_from("<i", b, f(call_site) + 1)[0]
    fsopen_abs = call_site + 5 + rel

    # cave stub
    cave_call_site = CAVE_RVA + 10
    cave_call_rel = fsopen_abs - (cave_call_site + 5)
    stub = (bytes.fromhex("6a40") + bytes.fromhex("ff74240c") + bytes.fromhex("ff74240c")
            + b"\xe8" + struct.pack("<i", cave_call_rel)
            + bytes.fromhex("83c40c")            # add esp,0xC
            + bytes.fromhex("3d00000100")        # cmp eax,0x10000
            + bytes.fromhex("7302")              # jae +2
            + bytes.fromhex("31c0")              # xor eax,eax
            + bytes.fromhex("c3"))               # ret
    # redirect: jmp cave at fopen entry
    jmp_rel = CAVE_RVA - (FOPEN_RVA + 5)
    redirect = b"\xe9" + struct.pack("<i", jmp_rel)
    return b, stub, redirect, fsopen_abs


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    b, stub, redirect, fsopen_abs = build()
    print(f"__fsopen resolved @0x{fsopen_abs:08x}; cave @0x{CAVE_RVA:08x} ({len(stub)}B stub)")

    if bytes(b[f(FOPEN_RVA):f(FOPEN_RVA)+1]) == b"\xe9":
        print("already patched (fopen -> cave)"); return 0
    # cave must be free (0xCC)
    if bytes(b[f(CAVE_RVA):f(CAVE_RVA)+len(stub)]) != b"\xcc" * len(stub):
        print("ERROR: cave not free (0xCC) — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(CAVE_RVA):f(CAVE_RVA)+len(stub)] = stub
    b[f(FOPEN_RVA):f(FOPEN_RVA)+len(redirect)] = redirect
    EXE.write_bytes(bytes(b))
    print(f"patched: fopen(FUN_004a4541) -> cave; returns NULL when _fsopen result < 0x10000")
    return 0


if __name__ == "__main__":
    sys.exit(main())
