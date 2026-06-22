# SKIP-MOVIES boot fix (2026-06-21; CORRECTED 2026-06-22). Companion to
# patch_mashed_skip_intro.py. Skips ONLY the small.mpg DirectShow player so the boot
# path does not build/exercise that graph, while the intro loop keeps running.
#
# The movie subsystem is rooted at TWO calls inside boot-init FUN_00402750:
#     0x402838  call FUN_00495350          ; intro loop  <- KEEP (also pumps the D3D
#                                          ;   present/render loop later boot code needs;
#                                          ;   NOP'ing it AVs ~0x402879, observed 5/5)
#     0x40283d  push 0                     ; <- KEEP (see below)
#     0x40283f  call FUN_00494c80          ; small.mpg player  <- the ONLY thing we NOP
#     0x402844  push 0x5cc41c; call FUN_00495280("...font36.piz")   ; normal init resumes
#     ...
#     0x402888  add esp, 0x24              ; deferred batch cleanup of accumulated args
#
# *** CALLING-CONVENTION CORRECTION (2026-06-22, root-caused via Ghidra) ***
# The ORIGINAL version of this patch NOP'd 7 bytes (`push 0` + `call`), on the assumption
# that FUN_00494c80 is __stdcall and the `push 0` was its argument (cleaned by the call).
# That is WRONG: FUN_00494c80 ends in a plain `ret` (c3 @ 0x494ed8) — it is cdecl/void and
# cleans NOTHING. The `push 0` at 0x40283d is NOT its argument; it is a deferred-cleanup
# stack slot that boot-init pops later in ONE batch `add esp, 0x24` @ 0x402888 (and the
# code uses esp-relative locals `lea [esp+0x24]`/`[esp+0x2c]` just before that). Removing
# the `push 0` shifts esp by 4 for every later esp-relative access AND makes `add esp,0x24`
# over-pop by 4 -> a DETERMINISTIC 4-byte stack imbalance -> corrupted return -> eip=0 crash
# a few seconds into the menu (proven 2026-06-22: with this NOP the game crashes
# eip=0 esp=0x1afe50 every run; without it the menu is stable). The crash was independent
# of which movies are present (real or 1-frame empties) — it was purely the stack imbalance.
#
# CORRECT patch: NOP ONLY the 5-byte `call FUN_00494c80` at 0x40283f. KEEP the `push 0`.
# The deferred cleanup then balances exactly as in the original; small.mpg's graph is never
# built. The intro loop (0x402838) and all later init are untouched.
#
# NOTE: patch_mashed_skip_intro.py (1-frame empties) is the preferred intro-skip — it lets
# FUN_00494c80 run normally (small.mpg plays instantly) so nothing is skipped at all. This
# binary skip is only for when you want small.mpg's DirectShow graph specifically NOT built.
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_skip_movies.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.premoviepatch"

IMG  = 0x00400000
SITE = 0x0040283f                                   # the `call FUN_00494c80` ONLY (push 0 kept)
ORIG = bytes.fromhex("e83c240900")                  # call FUN_00494c80 (small.mpg), 5 bytes
NOPS = b"\x90" * len(ORIG)                            # 5 NOPs
# Guard: the `push 0` at 0x40283d MUST remain (6a00). If a binary still has the OLD broken
# 7-byte NOP applied (push 0 also NOP'd), this script detects it and refuses / re-fixes.
OLD_BROKEN = bytes.fromhex("90909090909090")         # old 7-NOP form over 0x40283d..0x402843
def f(rva): return rva - IMG


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    b = bytearray(EXE.read_bytes())

    # Detect & repair the OLD broken 7-byte NOP (push 0 wrongly removed -> stack imbalance).
    old = bytes(b[f(0x0040283d):f(0x0040283d)+7])
    if old == OLD_BROKEN:
        if not BAK.exists():
            print("ERROR: old broken patch present but no backup to restore push 0 — aborting"); return 4
        print("detected OLD broken 7-NOP form (stack-imbalance bug); restoring then re-applying correct fix")
        shutil.copy2(BAK, EXE); b = bytearray(EXE.read_bytes())

    cur = bytes(b[f(SITE):f(SITE)+len(ORIG)])
    push0 = bytes(b[f(0x0040283d):f(0x0040283d)+2])
    if cur == NOPS:
        print("already patched (small.mpg call NOP'd; push 0 intact)"); return 0
    if push0 != bytes.fromhex("6a00"):
        print(f"ERROR: expected `push 0` (6a00) at 0x40283d, found {push0.hex()} — aborting"); return 3
    if cur != ORIG:
        print(f"ERROR: bytes at 0x{SITE:08x} = {cur.hex()} != expected {ORIG.hex()} — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(SITE):f(SITE)+len(NOPS)] = NOPS
    EXE.write_bytes(bytes(b))
    print("patched: small.mpg `call FUN_00494c80` NOP'd (push 0 KEPT; stack balanced; intro intact)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
