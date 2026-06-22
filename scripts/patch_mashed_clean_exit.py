# CLEAN-EXIT boot/shutdown fix (2026-06-21).
#
# When MASHED quits (WinMain returns), the CRT exit path crashes with eip=0 (a jmp/call
# THROUGH a null pointer inside doexit's cleanup — a game atexit/static-dtor calling a
# null function pointer; doexit's own table loops are guarded `if ptr!=0`, so it's a
# callee, not a table entry). Root-caused via Ghidra + a HW write-watchpoint
# (re/frida/watch_corruption.py): the crash is NOT a stack smash (doexit's return slot is
# never zeroed) — it is a null indirect transfer in the exit cleanup. The exact dtor is
# unresolvable (eip=0 carries no faulting instruction; the quit is non-deterministic).
# See [[project-replay-determinism-and-boot-patches]].
#
# doexit = FUN_004a3258(code, quick, retcaller) is the common dispatcher for ALL exit
# paths (exit/_exit/_cexit/abort). Since every doexit call means the process is shutting
# down and the cleanup is exactly what crashes, redirect doexit's entry to
# ExitProcess(code) — a clean OS-level terminate that skips the crashing CRT cleanup.
# The original MASHED.exe is a dev DIFF REFERENCE (not shipped), and its exit path is
# never diffed, so dropping CRT exit-cleanup is benign; it just stops the dirty-exit
# crash dumps.
#
# Patch (12 bytes at doexit entry 0x4a3258):
#     push dword ptr [esp+4]            ; ff 74 24 04   ; param_1 = exit code
#     call dword ptr [0x5cc1b8]         ; ff 15 b8 c1 5c 00  ; ExitProcess(code) — no return
#     ret                              ; c3            ; safety (unreachable)
#     int3                             ; cc            ; pad to the 12-byte instr boundary
# Overwrites: push ebp; mov ebp,esp; push esi; push edi; push 8; call __lock(8).
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_clean_exit.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.precleanexitpatch"

IMG  = 0x00400000
SITE = 0x004a3258                                       # doexit (FUN_004a3258) entry
ORIG = bytes.fromhex("558bec56576a08e81b460000")        # push ebp;mov ebp,esp;push esi;push edi;push 8;call __lock
PATCH= bytes.fromhex("ff742404" + "ff15b8c15c00" + "c3" + "cc")   # push [esp+4]; call [ExitProcess]; ret; int3
def f(rva): return rva - IMG


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    assert len(PATCH) == len(ORIG) == 12, (len(PATCH), len(ORIG))
    b = bytearray(EXE.read_bytes())
    cur = bytes(b[f(SITE):f(SITE)+len(ORIG)])
    if cur == PATCH:
        print("already patched (doexit -> ExitProcess)"); return 0
    if cur != ORIG:
        print(f"ERROR: bytes at 0x{SITE:08x} = {cur.hex()} != expected {ORIG.hex()} — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(SITE):f(SITE)+len(PATCH)] = PATCH
    EXE.write_bytes(bytes(b))
    print("patched: doexit (FUN_004a3258) -> ExitProcess(code); CRT exit-cleanup skipped (clean exit)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
