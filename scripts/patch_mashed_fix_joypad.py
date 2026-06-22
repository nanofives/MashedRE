# JOYPAD-POLL boot fix (2026-06-21). Companion to patch_mashed_fix_fopen.py.
#
# With a game controller connected, boot AVs ~4 s in at 0x00495883 inside the
# per-device input poll FUN_00495870 (read access, faulting addr 0x3eb33333):
#
#   0x495870  sub  esp,0x6c
#   0x495873  mov  eax,[0x616038]     ; /GS cookie
#   ...
#   0x49587d  mov  edi,[esi]          ; edi = device-interface pointer (slot+0)
#   0x495883  mov  eax,[edi]          ; <<< AV: deref garbage vtable
#   0x495886  call [eax+0x64]         ; IDirectInputDevice8::Poll
#
# The caller (0x00495fe0) loops [0x772fac] devices over the 4-slot array at
# 0x771e88 (stride 0x448), calling FUN_00495870 with esi = &slot[i]. One slot's
# device pointer is a GARBAGE, DETERMINISTIC value 0x3eb33333 — which is exactly
# the float 0.35f (0x3EB33333) reinterpreted as a pointer, i.e. that array memory
# overlaps/was written with a 0.35f and the device count counts it as live. The
# original code assumes every in-range slot holds a valid COM interface and
# dereferences it unconditionally, so the bad slot AVs. (Pure keyboard boots fine,
# which is why the crash is "intermittent" = present only when a pad is attached.)
#
# Fix: guard the poll. Redirect FUN_00495870's entry (jmp) to a code cave that
# re-runs the relocated prologue, loads the slot device pointer, and validates it:
#   - low-2-bits aligned (a real COM object is >=4-aligned; 0x...33 is not), and
#   - IsBadReadPtr(ptr,4)==FALSE (readable).
# If either fails, the function returns immediately (poll skipped; the slot's
# zero-init state buffer reads as "no input"). The /GS cookie register (eax) is
# preserved across the probe via push/pop, and on the skip path we never enter the
# cookie-checked frame (plain `add esp,0x6c; ret`, matching the real epilogue at
# 0x495edd which is a plain ret).
#
# Residual: IsBadReadPtr(ptr,4) only proves ptr[0..3] readable; a readable-but-bogus
# object could still mis-call. The observed garbage (0.35f) is both unaligned AND
# unmapped, so the alignment+readability pair catches it deterministically.
#
# .text maps 1:1 (file off = RVA - 0x400000). Reversible (backup), idempotent.
# Usage: py -3.12 scripts/patch_mashed_fix_joypad.py [--restore]
import sys, struct, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "original" / "MASHED.exe"
BAK = ROOT / "original" / "MASHED.exe.prejoypadpatch"

IMG       = 0x00400000
FUN_RVA   = 0x00495870     # FUN_00495870 entry (per-device input poll)
CONT_RVA  = 0x00495878     # resume point (after relocated `sub esp,0x6c; mov eax,cookie`)
CAVE_RVA  = 0x00508bd5     # 43-byte 0xCC cave (file 0x108bd5); stub is 39 B
IBR_IAT   = 0x005cc168     # KERNEL32!IsBadReadPtr import slot
COOKIE    = 0x00616038     # __security_cookie
ENTRY_SIG = bytes.fromhex("83ec6ca138606100")   # sub esp,0x6c ; mov eax,[0x616038]

def f(rva): return rva - IMG   # file offset (1:1 .text)


def build_stub():
    """39-byte guard stub. bad: label sits at offset 34."""
    BAD = 34
    s = bytearray()
    s += bytes.fromhex("83ec6c")              # 0:  sub  esp,0x6c          (relocated)
    s += bytes.fromhex("a138606100")          # 3:  mov  eax,[0x616038]    (relocated cookie)
    s += bytes.fromhex("50")                  # 8:  push eax               (save cookie)
    s += bytes.fromhex("8b06")                # 9:  mov  eax,[esi]         (device ptr, no deref)
    s += bytes.fromhex("a803")                # 11: test al,3              (alignment)
    s += b"\x75" + bytes([BAD - 15])          # 13: jnz  bad
    s += bytes.fromhex("6a04")                # 15: push 4
    s += bytes.fromhex("50")                  # 17: push eax
    s += b"\xff\x15" + struct.pack("<I", IBR_IAT)  # 18: call [IsBadReadPtr]
    s += bytes.fromhex("85c0")                # 24: test eax,eax
    s += b"\x75" + bytes([BAD - 28])          # 26: jnz  bad
    s += bytes.fromhex("58")                  # 28: pop  eax               (restore cookie)
    cont_rel = CONT_RVA - (CAVE_RVA + 34)     # jmp rel32 from next-ins (offset 34)
    s += b"\xe9" + struct.pack("<i", cont_rel)# 29: jmp  0x495878
    assert len(s) == BAD, len(s)
    s += bytes.fromhex("58")                  # 34: pop  eax  (bad: discard saved cookie)
    s += bytes.fromhex("83c46c")              # 35: add  esp,0x6c
    s += bytes.fromhex("c3")                  # 38: ret
    return bytes(s)


def main():
    if "--restore" in sys.argv:
        if BAK.exists():
            shutil.copy2(BAK, EXE); print(f"restored {EXE.name} from {BAK.name}"); return 0
        print("no backup"); return 1

    b = bytearray(EXE.read_bytes())
    stub = build_stub()
    redirect = b"\xe9" + struct.pack("<i", CAVE_RVA - (FUN_RVA + 5))
    print(f"FUN_00495870 -> cave 0x{CAVE_RVA:08x} ({len(stub)}B stub); IsBadReadPtr@[0x{IBR_IAT:08x}]")

    if bytes(b[f(FUN_RVA):f(FUN_RVA)+1]) == b"\xe9":
        print("already patched (FUN_00495870 -> cave)"); return 0
    if bytes(b[f(FUN_RVA):f(FUN_RVA)+8]) != ENTRY_SIG:
        print(f"ERROR: entry bytes {bytes(b[f(FUN_RVA):f(FUN_RVA)+8]).hex()} != expected {ENTRY_SIG.hex()} — aborting"); return 2
    if bytes(b[f(CAVE_RVA):f(CAVE_RVA)+len(stub)]) != b"\xcc" * len(stub):
        print("ERROR: cave not free (0xCC) — aborting"); return 2

    if not BAK.exists():
        shutil.copy2(EXE, BAK); print(f"backed up -> {BAK.name}")
    b[f(CAVE_RVA):f(CAVE_RVA)+len(stub)] = stub
    b[f(FUN_RVA):f(FUN_RVA)+len(redirect)] = redirect
    EXE.write_bytes(bytes(b))
    print("patched: per-device poll skips slots with an unaligned/unreadable device pointer")
    return 0


if __name__ == "__main__":
    sys.exit(main())
