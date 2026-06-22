# FORCE-KEYBOARD-ONLY patch (2026-06-22). Makes MASHED ignore physically connected
# joysticks, so it never takes the joystick code path that crashes on this Win11 build.
#
# Root cause (Ghidra static analysis, this session): the joypad enumerator
# FUN_00496040 ("Searching for joypads") calls IDirectInput8::EnumDevices(class=4
# GAMECTRL, callback LAB_00495ee0, ctx=&DAT_00772fac); the callback increments
# DAT_00772fac (the joystick COUNT) per joypad. That count drives EVERYTHING:
#   - FUN_00498510 (device/binding assignment) makes players [0..count-1] joysticks
#     (device type 1) and player [count] the keyboard (type 2);
#   - the per-frame poll loop FUN_00495fe0 iterates `count` devices and calls
#     FUN_00495870 — the exact crash site (esp=0x1afe50 / WinMain-teardown class).
# count==0 => keyboard-only, no joystick poll, no crash (= unplugged / clean baseline).
# See [[project-replay-determinism-and-boot-patches]].
#
# Patch: replace FUN_00496040's entry (0x496040) with `mov dword [0x772fac],0; mov eax,1;
# ret` — skip the enum, force count 0, return success. Every caller treats 0 joypads as a
# valid keyboard-only state. The game then never enumerates/assigns/polls a joystick even
# with one connected. This is the clean fix the joypad-guard patch only band-aided.
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent. Toggle for
# A/B (apply vs --restore): keyboard-forced vs stock joystick behavior.
# Usage: py -3.12 scripts/patch_mashed_force_keyboard.py [--restore]
import sys, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.prekbdonlypatch"

IMG  = 0x00400000
SITE = 0x00496040                                         # FUN_00496040 entry (joypad enum)
ORIG = bytes.fromhex("5668bcff5c00e86527000083c404e8cd")  # push esi; push str; call log; add esp,4; call... (16B)
PATCH= bytes.fromhex("c705ac2f770000000000" + "b801000000" + "c3")  # mov [0x772fac],0 ; mov eax,1 ; ret (16B)
def f(rva): return rva - IMG


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    assert len(PATCH) == len(ORIG) == 16, (len(PATCH), len(ORIG))
    b = bytearray(EXE.read_bytes())
    cur = bytes(b[f(SITE):f(SITE)+len(ORIG)])
    if cur == PATCH:
        print("already patched (joypad enum -> force count 0)"); return 0
    if cur != ORIG:
        print(f"ERROR: bytes at 0x{SITE:08x} = {cur.hex()} != expected {ORIG.hex()} — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(SITE):f(SITE)+len(PATCH)] = PATCH
    EXE.write_bytes(bytes(b))
    print("patched: FUN_00496040 -> DAT_00772fac=0; return 1  (joysticks ignored, keyboard-only)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
