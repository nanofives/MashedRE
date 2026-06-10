# Dump float32 values at .rdata VAs from the anchored MASHED.exe.unpatched.
# Used to harvest camera/elimination constants for the verbatim port
# (FUN_00446520 / FUN_00410d10). Deterministic: byte reads at section-mapped
# file offsets of the SHA-256-anchored binary.
#
# Usage: py -3.12 re/tools/dump_rdata_floats.py 0x005cc320 0x005cc358 ...
import struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "original" / "MASHED.exe.unpatched"


def parse_pe(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    sec0 = pe + 24 + opt_size
    secs = []
    for i in range(nsec):
        s = sec0 + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode()
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    return base, secs


def main():
    data = EXE.read_bytes()
    base, secs = parse_pe(data)
    for arg in sys.argv[1:]:
        va = int(arg, 16)
        rva = va - base
        hit = None
        for name, vaddr, vsz, roff, rsz in secs:
            if vaddr <= rva < vaddr + max(vsz, rsz):
                off = roff + (rva - vaddr)
                if rva - vaddr < rsz:
                    raw = struct.unpack_from("<I", data, off)[0]
                    f = struct.unpack_from("<f", data, off)[0]
                    hit = (name, f"0x{raw:08x}", f"{f!r}", f"i32={raw if raw < 0x80000000 else raw - (1 << 32)}")
                else:
                    hit = (name, "uninitialized (.bss range)", "", "")
                break
        if hit:
            print(f"0x{va:08x}  {hit[0]:8s} raw={hit[1]:12s} f32={hit[2]:16s} {hit[3]}")
        else:
            print(f"0x{va:08x}  NOT IN IMAGE")


if __name__ == "__main__":
    sys.exit(main())
