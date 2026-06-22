# SKIP-INTRO-LOGOS patch (2026-06-22). Auto-skips the 4 boot logo videos
# (empire/supersonic/renderware/intro) for a fast boot, WITHOUT touching the D3D pump.
#
# The intro loop FUN_00495350 (0x495350) plays 4 logos: it calls FUN_00494a80(0,idx,0) to
# START each movie, then spins a render/present loop (FUN_004671a0/FUN_00499710/...) until
# the movie finishes (DAT_00771a04 cleared) or is skipped, then starts the next. NOPping the
# whole intro loop crashes boot (it also pumps D3D), but NOPping only the movie-START is safe:
# the loop then sees "no movie active" (DAT_00771a04 stays 0) and advances through all 4
# instantly while still pumping the render loop.
#
# FUN_00494a80 (0x494a80) is __cdecl (plain ret; caller cleans args) and called ONLY from the
# intro loop (verified). It does: alloc DAT_00771a18, build+run the movie graph (FUN_004944c0),
# set DAT_00771a04=1. Patching its entry to `ret` skips all of that — no graph, no DirectShow,
# DAT_00771a04 stays 0 -> the loop flies through. The 3 pushed args are cleaned by the caller
# (cdecl) so the stack stays balanced. See [[project-launch-joystick-video-config]].
#
# (Complements patch_mashed_skip_movies.py which skips small.mpg FUN_00494c80. Together they
# skip ALL boot videos. The intro was already keypress-skippable; this makes it automatic.)
#
# Patch (5 bytes at 0x494a80): ret ; nop*4   (over `push 0x84`).
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_skip_intro_logos.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.preintrologospatch"

IMG  = 0x00400000
SITE = 0x00494a80                          # FUN_00494a80 entry (intro movie-start)
ORIG = bytes.fromhex("6884000000")         # push 0x84
PATCH= bytes.fromhex("c3" + "90909090")    # ret ; nop nop nop nop
def f(rva): return rva - IMG


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    assert len(PATCH) == len(ORIG) == 5, (len(PATCH), len(ORIG))
    b = bytearray(EXE.read_bytes())
    cur = bytes(b[f(SITE):f(SITE)+len(ORIG)])
    if cur == PATCH:
        print("already patched (intro movie-start -> ret)"); return 0
    if cur != ORIG:
        print(f"ERROR: bytes at 0x{SITE:08x} = {cur.hex()} != expected {ORIG.hex()} — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(SITE):f(SITE)+len(PATCH)] = PATCH
    EXE.write_bytes(bytes(b))
    print("patched: FUN_00494a80 -> ret  (4 intro logos auto-skipped; D3D pump intact)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
