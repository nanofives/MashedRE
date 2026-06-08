# On-disk patch (DEV/testing): stop MASHED pausing when its window loses focus.
#
# Background: the main-loop message pump FUN_00499690 (0x00499690) ends with
#   004996cc  MOV EAX,[0x0077391c]        ; DAT_0077391c = focus/active flag
#   004996d1  TEST EAX,EAX                ;   (set by WndProc WM_ACTIVATE, FUN_00499820)
#   004996d3  JNZ 0x004996db             ; 75 06  -> if focused, skip the wait
#   004996d5  CALL [0x005cc27c]           ; ff 15 7c c2 5c 00  -> WaitMessage()  <-- PAUSE
#   004996db  ...
# When the window is NOT focused, DAT_0077391c==0, so every loop iteration calls
# WaitMessage(), which blocks the thread until a window message arrives -> the game
# freezes until you click back on it. (DAT_0077391c has exactly one reader, so this is
# the only pause gate.)
#
# Fix: flip the conditional JNZ (75) to an unconditional JMP (EB) at 0x004996d3, so
# WaitMessage() is ALWAYS skipped regardless of focus. The game then keeps running at
# full speed when unfocused, so you can do other things while it's tested in the
# background. (Trade-off: the main loop spins a CPU core while unfocused. Acceptable for
# a dev/testing patch; reversible with --restore.)
#
# This lets in-process input testing (re/frida/input_*.py) work reliably: the frontend
# tick keeps advancing even when the terminal/Frida (not MASHED) holds foreground focus.
#
# RVA->file offset: single .text mapping, file offset = RVA - 0x400000 (same convention
# as the other patch_mashed_*.py scripts). Idempotent; reversible with --restore. Does
# NOT touch original/MASHED.exe.unpatched (the version anchor).
import hashlib
import sys
from pathlib import Path

ROOT       = Path(__file__).resolve().parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'

PATCH_FILE_OFFSET = 0x000996d3          # RVA 0x004996d3
JNZ = 0x75                              # original: JNZ 0x004996db
JMP = 0xEB                              # patched : JMP 0x004996db (always skip WaitMessage)

# guard signature: the MOV/TEST/JNZ/CALL block around the patch site, so we never flip
# the wrong byte. Spans 0x004996cc..0x004996da (file 0x996cc..0x996da), 15 bytes.
SIG_OFF = 0x000996cc
SIG_PRE = bytes.fromhex("a11c39770085c07506ff157cc25c00")   # ...75 06 (JNZ) ...
SIG_POST = bytes.fromhex("a11c39770085c0eb06ff157cc25c00")  # ...eb 06 (JMP) ...


def main():
    restore = "--restore" in sys.argv
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    data = bytearray(MASHED_EXE.read_bytes())
    sig = bytes(data[SIG_OFF:SIG_OFF + len(SIG_PRE)])
    cur = data[PATCH_FILE_OFFSET]

    if restore:
        if sig == SIG_PRE and cur == JNZ:
            print(f"  not patched (byte at 0x{PATCH_FILE_OFFSET:x} already JNZ 0x75) — nothing to restore")
            return 0
        if sig != SIG_POST or cur != JMP:
            sys.exit(f"  unexpected bytes; refusing to restore. sig={sig.hex()} byte=0x{cur:02x}")
        data[PATCH_FILE_OFFSET] = JNZ
        MASHED_EXE.write_bytes(bytes(data))
        print(f"  restored: 0x{PATCH_FILE_OFFSET:x} JMP -> JNZ (focus-pause re-enabled)")
        print(f"  MASHED.exe sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()}")
        return 0

    if sig == SIG_POST and cur == JMP:
        print(f"  already patched at 0x{PATCH_FILE_OFFSET:x} (JMP 0xeb) — no-op")
        return 0
    if sig != SIG_PRE or cur != JNZ:
        sys.exit(f"  unexpected bytes at 0x{PATCH_FILE_OFFSET:x}: sig={sig.hex()} byte=0x{cur:02x}\n"
                 f"  expected sig={SIG_PRE.hex()} byte=0x{JNZ:02x}")
    data[PATCH_FILE_OFFSET] = JMP
    MASHED_EXE.write_bytes(bytes(data))
    print(f"patched MASHED.exe: 0x{PATCH_FILE_OFFSET:x} JNZ(0x75) -> JMP(0xeb)")
    print(f"  WaitMessage() at 0x004996d5 now always skipped -> no pause on focus loss")
    print(f"  MASHED.exe sha256: {hashlib.sha256(MASHED_EXE.read_bytes()).hexdigest()}")
    print(f"  restore: py -3.12 scripts/patch_mashed_no_focus_pause.py --restore")
    return 0


if __name__ == '__main__':
    sys.exit(main())
