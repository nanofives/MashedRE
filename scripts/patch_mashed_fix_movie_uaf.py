# FIX-MOVIE-UAF patch (2026-06-22). Fixes a latent use-after-free in MASHED's
# DirectShow movie teardown that crashes the game when a degenerate/short movie
# (e.g. the 1-frame intro-skip empties) fails video-format negotiation.
#
# Root cause (Ghidra RE, this session):
#   The TextureRenderer creates two D3D textures DAT_00771a10 / DAT_00771a14 in
#   FUN_00494820 (SetMediaType) via device->CreateTexture, but ONLY when the movie's
#   video format negotiates (it requires format 0x16/0x19). The per-movie teardown
#   FUN_00494320 then RELEASES those two textures. Its six sibling COM releases are
#   all gated+nulled ( if(x){ x=0; x->Release(); } ); a10/a14 are released UNGATED
#   and are NOT nulled afterward:
#       004943b0  MOV EAX,[0x771a10]      ; no null-check
#       004943b5  MOV EDX,[EAX]           ; deref — if stale/freed, EDX = garbage vtable
#       004943b8  CALL [EDX+8]            ; Release -> jump to garbage = eip=0 crash
#   A movie that fails negotiation never recreates a10/a14, so the next teardown
#   releases a DANGLING texture pointer -> use-after-free -> nondeterministic, delayed
#   crash (manifests as the esp=0x1afe50 / eip=0 corruption sink). Well-formed movies
#   always renegotiate, so they never dangle -> the original never trips this in normal
#   play; only degenerate movies (the empties) expose it.
#
# Fix: gate a10/a14 exactly like their siblings. Keep the MOV EAX,[a10] at 0x4943b0
# (it is a jump-join target), redirect 0x4943b5 -> a 0xCC code cave at RVA 0x508bd5
# that does the gated+nulled releases for a10 then a14, then POP ESI; RET (matching
# the real epilogue). ESI=0 throughout (XOR ESI,ESI in the prologue) and is callee-
# saved across COM Release, so the cave reuses it for the null stores and gate compares.
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_fix_movie_uaf.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE  = ROOT / "original" / "MASHED.exe"
BAK  = ROOT / "original" / "MASHED.exe.premovieuaf"

IMG  = 0x00400000
def f(rva): return rva - IMG

# --- Region 1: redirect at 0x4943b5 + NOP the old ungated a10-tail/a14 release ------
REDIR_RVA  = 0x004943b5
# original 17 bytes 0x4943b5..0x4943c5:
#   8b10        MOV EDX,[EAX]          (a10 release tail)
#   50          PUSH EAX
#   ff5208      CALL [EDX+8]
#   a1141a7700  MOV EAX,[0x771a14]     (a14 release, ungated)
#   8b08        MOV ECX,[EAX]
#   50          PUSH EAX
#   ff5108      CALL [ECX+8]
REDIR_ORIG = bytes.fromhex("8b1050ff5208a1141a77008b0850ff5108")
# JMP 0x508c95 (rel32 = 0x508c95 - (0x4943b5+5) = 0x000748db) ; then 12x NOP
# (cave 0x508c95, NOT 0x508bd5 — the latter is used by patch_mashed_fix_joypad.py)
REDIR_PATCH = bytes.fromhex("e9db480700" + "90" * 12)

# --- Region 2: the code cave (39 bytes of gated releases) --------------------------
CAVE_RVA  = 0x00508c95
CAVE_ORIG = bytes.fromhex("cc" * 39)             # int3 padding (joypad-guard not applied)
#   3bc6              CMP EAX,ESI           ; EAX = a10 (loaded inline at 0x4943b0)
#   740c              JZ  +0x0c             ; skip if a10 == 0
#   8935101a7700      MOV [0x771a10],ESI    ; null it (ESI = 0)
#   8b10              MOV EDX,[EAX]
#   50                PUSH EAX
#   ff5208            CALL [EDX+8]          ; a10->Release
#   a1141a7700        MOV EAX,[0x771a14]
#   3bc6              CMP EAX,ESI
#   740c              JZ  +0x0c             ; skip if a14 == 0
#   8935141a7700      MOV [0x771a14],ESI
#   8b08              MOV ECX,[EAX]
#   50                PUSH EAX
#   ff5108            CALL [ECX+8]          ; a14->Release
#   5e                POP ESI
#   c3                RET
CAVE_PATCH = bytes.fromhex(
    "3bc6740c8935101a77008b1050ff5208"
    "a1141a77003bc6740c8935141a77008b08"
    "50ff51085ec3"
)


def apply_region(b, rva, orig, patch, name):
    o = f(rva)
    cur = bytes(b[o:o + len(orig)])
    if cur == patch:
        print(f"  [{name}] already patched"); return "done"
    if cur != orig:
        print(f"  [{name}] ERROR @0x{rva:08x}: {cur.hex()} != expected {orig.hex()}"); return "bad"
    b[o:o + len(patch)] = patch
    print(f"  [{name}] patched @0x{rva:08x} ({len(patch)} bytes)"); return "ok"


def main():
    assert len(REDIR_PATCH) == len(REDIR_ORIG) == 17, (len(REDIR_PATCH), len(REDIR_ORIG))
    assert len(CAVE_PATCH) == 39, len(CAVE_PATCH)

    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    b = bytearray(EXE.read_bytes())
    # Pre-flight: verify both regions are in a known state (orig or already-patched).
    for rva, orig, patch, name in (
        (REDIR_RVA, REDIR_ORIG, REDIR_PATCH, "redir"),
        (CAVE_RVA,  CAVE_ORIG,  CAVE_PATCH,  "cave"),
    ):
        o = f(rva); cur = bytes(b[o:o + len(orig)])
        if cur != orig and cur != patch:
            print(f"ERROR: {name} @0x{rva:08x} unexpected bytes {cur.hex()} — aborting (cave taken?)")
            return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")

    r1 = apply_region(b, CAVE_RVA,  CAVE_ORIG,  CAVE_PATCH,  "cave")   # write cave FIRST
    r2 = apply_region(b, REDIR_RVA, REDIR_ORIG, REDIR_PATCH, "redir")  # then the jump
    if "bad" in (r1, r2):
        print("aborted; no write"); return 3
    EXE.write_bytes(bytes(b))
    print("patched: FUN_00494320 a10/a14 release now gated+nulled (movie-teardown UAF fixed)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
