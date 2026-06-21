# BOOT FIX (2026-06-20): neutralize MASHED's broken logging subsystem.
#
# After a ~2026-06-10 Windows update, MASHED's boot AVs ~4 s in: 0xC0000005 in
# RtlEnterCriticalSection (ntdll+0x542f0, ecx=0x5477). Root cause (see
# [[project-boot-crash-static-init-not-heap]]): the log FILE* global DAT_00772fbc
# (PE-init 0) holds garbage 0x5453 at boot (layout-dependent; 0 under Frida), so the
# log functions fputs() to a bad FILE* and crash entering its critical section. This
# is NOT heap, and no compat shim (EMULATEHEAP / page heap) fixes it.
#
# Fix = make the three boot-path log functions no-ops (logging to mashed.log is
# cosmetic). All guard `if (DAT_00772fbc != 0)`; we either return at entry or force
# the skip branch. Verified: MASHED boots to the main menu on direct launch.
#
#   FUN_00496490 @0x496490 (log-open, WM_CREATE)  prologue `sub esp,0x114` -> xor eax,eax;ret
#   FUN_00496400 @0x496400 (log-printf)           prologue `sub esp,0x204` first byte -> ret (cdecl)
#   FUN_004963e0 @0x4963e7 (log-write)            `je` (74) -> `jmp` (eb): always skip fputs
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible via full backup. Idempotent.
#
# Usage:
#   py -3.12 scripts/patch_mashed_disable_log.py            # apply
#   py -3.12 scripts/patch_mashed_disable_log.py --restore
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.prelogpatch"

# (name, file_offset, original_bytes, patched_bytes, context_check_offset, context_bytes)
PATCHES = [
    ("FUN_00496490 log-open -> ret0", 0x96490,
     bytes.fromhex("81ec14010000"), bytes.fromhex("31c0c3909090"), None, None),
    ("FUN_00496400 log-printf -> ret", 0x96400,
     bytes.fromhex("81"), bytes.fromhex("c3"),
     0x96401, bytes.fromhex("ec04020000")),     # ensure it's the sub esp,0x204 prologue
    ("FUN_004963e0 log-write je->jmp", 0x963e7,
     bytes.fromhex("74"), bytes.fromhex("eb"),
     0x963e0, bytes.fromhex("a1bc2f770085c0")),  # ensure mov eax,[0x772fbc];test eax,eax precedes
]


def main():
    b = bytearray(EXE.read_bytes())

    if "--restore" in sys.argv:
        if not BAK.exists():
            print("no backup (MASHED.exe.prelogpatch) to restore"); return 1
        shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0

    # validate every patch site first
    for name, off, orig, patched, coff, cbytes in PATCHES:
        cur = bytes(b[off:off+len(orig)])
        if cur == patched:
            continue
        if cur != orig:
            print(f"ERROR [{name}] @0x{off:x}: expected {orig.hex()} or {patched.hex()}, found {cur.hex()} — aborting")
            return 2
        if coff is not None and bytes(b[coff:coff+len(cbytes)]) != cbytes:
            print(f"ERROR [{name}] context @0x{coff:x} mismatch — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")

    applied = 0
    for name, off, orig, patched, coff, cbytes in PATCHES:
        if bytes(b[off:off+len(patched)]) == patched:
            print(f"  [already] {name}")
            continue
        b[off:off+len(patched)] = patched
        applied += 1
        print(f"  [patched] {name} @0x{off:x}")
    if applied:
        EXE.write_bytes(bytes(b))
    print(f"done: {applied} patch(es) applied; MASHED boots to menu on direct launch")
    return 0


if __name__ == "__main__":
    sys.exit(main())
