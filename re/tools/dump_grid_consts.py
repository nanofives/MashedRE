# dump_grid_consts.py — read the float constants used by the ORIGINAL's race
# grid start-position calculator FUN_00408b00 (re/analysis/game_state_d5_cont2/
# 0x00408b00.md) straight out of MASHED.exe .rdata. No game spawn, pure file read.
#
# These are .rdata literals, unaffected by the boot patches, so either MASHED.exe
# or MASHED.exe.unpatched gives identical values; default to .unpatched (pristine).
import struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = None
for cand in (ROOT / "original" / "MASHED.exe.unpatched",
             ROOT / "original" / "MASHED.exe"):
    if cand.exists():
        EXE = cand; break
if EXE is None:
    for p in ROOT.parents:
        for name in ("MASHED.exe.unpatched", "MASHED.exe"):
            c = p / "original" / name
            if c.exists():
                EXE = c; break
        if EXE: break
if EXE is None:
    sys.exit("MASHED.exe not found")

data = EXE.read_bytes()
IMAGE_BASE = 0x00400000

# Minimal PE section parse to map VA -> file offset.
pe = data.find(b"PE\x00\x00", struct.unpack_from("<I", data, 0x3c)[0])
coff = pe + 4
num_sections = struct.unpack_from("<H", data, coff + 2)[0]
opt_size = struct.unpack_from("<H", data, coff + 16)[0]
sec_off = coff + 20 + opt_size
sections = []
for i in range(num_sections):
    o = sec_off + i * 40
    name = data[o:o+8].rstrip(b"\x00").decode("latin1")
    vsize, vaddr, rsize, raddr = struct.unpack_from("<IIII", data, o + 8)
    sections.append((name, vaddr, vsize, raddr, rsize))

def va_to_off(va):
    rva = va - IMAGE_BASE
    for name, vaddr, vsize, raddr, rsize in sections:
        if vaddr <= rva < vaddr + max(vsize, rsize):
            return raddr + (rva - vaddr)
    return None

def rf(va):
    off = va_to_off(va)
    if off is None:
        return None
    return struct.unpack_from("<f", data, off)[0]

consts = {
    "_DAT_005ccac0 (perp / forward-perp scale)": 0x005ccac0,
    "_DAT_005ccabc (lateral A, slot 1)":         0x005ccabc,
    "_DAT_005ccab8 (lateral B, slot 2)":         0x005ccab8,
    "_DAT_005ccab4 (lateral C, slot 3)":         0x005ccab4,
    "_DAT_005cc9bc (forward spacing)":           0x005cc9bc,
    "_DAT_005cc318 (2-car spacing)":             0x005cc318,
    "_DAT_005cc9c8 (3-car spacing)":             0x005cc9c8,
}
print(f"exe = {EXE}")
for label, va in consts.items():
    v = rf(va)
    raw = struct.unpack_from("<I", data, va_to_off(va))[0] if va_to_off(va) else 0
    print(f"  0x{va:08x}  {label:42s} = {v!r:>16}   (0x{raw:08x})")
