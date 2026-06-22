# SKIP-TEARDOWN clean-exit fix (2026-06-22). The real fix for the esp=0x1afe50 crash.
#
# Root cause (Ghidra static analysis): WinMain = FUN_00492370 runs, after the game loop
# (FUN_00492290) returns OR after boot-init (FUN_00402750) conditionally FAILS, a TEARDOWN
# sequence at 0x4923de..0x49242d:
#     0x4923de  log("AppDestroy")
#     0x4923eb  call FUN_00402a40   ; AppDestroy
#     0x492416  call FUN_004938c0   ; SoftwareTidyUpBeforeExiting (via thunk 0x493550)
#     0x492428  call FUN_004954f0   ; HardwareExitApplication     (via thunk 0x493560)
#     0x49242d  call FUN_00499cc0   ; DestroyWindow + return
# The crash (eip -> garbage 64KB-aligned region base, esp=0x1afe50, varies run-to-run) is a
# transfer through an uninitialized/freed object pointer DEEP in this teardown (a D3D/input
# release on a garbage COM object). It is LAYOUT-SENSITIVE (vanishes under TTD/Frida — proven:
# 15/15 boots reached menu under tttracer), so the exact pointer can't be confirmed dynamically.
# The boot "lottery" is the SAME crash: FUN_00402750 fails ~67% (layout-sensitive) -> game loop
# skipped -> teardown runs on a half-init state -> crash. See [[project-replay-determinism-and-boot-patches]].
#
# Fix: at the teardown entry 0x4923de, call ExitProcess(0) directly. Both the boot-fail path
# (je 0x4923de) and the normal-quit path (game loop returns to 0x4923de) hit it, so the crashing
# teardown NEVER runs and the process exits cleanly in every case. The OS reclaims D3D/handles
# on ExitProcess; MASHED persists saves explicitly during play (not in teardown), so skipping the
# game teardown is safe. (Does NOT fix the ~67% boot-init failure itself — those now EXIT CLEANLY
# instead of crashing; successful boots quit cleanly with no crash dump.)
#
# Patch (10 bytes at 0x004923de): push 0 ; call dword ptr [ExitProcess]  (IAT 0x5cc1b8) ; nop;nop
# Overwrites: push 0x5cf700 ; call 0x4963e0 (the "AppDestroy" log).
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_skip_teardown.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.preteardownpatch"

IMG  = 0x00400000
SITE = 0x004923de                                  # WinMain teardown entry
ORIG = bytes.fromhex("6800f75c00" + "e8f83f0000")  # push 0x5cf700 ; call 0x4963e0
PATCH= bytes.fromhex("6a00" + "ff15b8c15c00" + "9090")  # push 0 ; call [ExitProcess] ; nop nop
def f(rva): return rva - IMG


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    assert len(PATCH) == len(ORIG) == 10, (len(PATCH), len(ORIG))
    b = bytearray(EXE.read_bytes())
    cur = bytes(b[f(SITE):f(SITE)+len(ORIG)])
    if cur == PATCH:
        print("already patched (WinMain teardown -> ExitProcess)"); return 0
    if cur != ORIG:
        print(f"ERROR: bytes at 0x{SITE:08x} = {cur.hex()} != expected {ORIG.hex()} — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(SITE):f(SITE)+len(PATCH)] = PATCH
    EXE.write_bytes(bytes(b))
    print("patched: WinMain teardown entry (0x4923de) -> ExitProcess(0); crashing teardown skipped")
    return 0


if __name__ == "__main__":
    sys.exit(main())
