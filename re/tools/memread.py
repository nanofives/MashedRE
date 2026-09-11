"""Read raw bytes at a VA from the anchored MASHED.exe.unpatched.

Deterministic byte-level evidence: prints the containing section, the file offset,
and the dword/float/pointer interpretations, so a constant can be cited exactly.
Reports when a VA is in an uninitialised region (raw size short of virtual size),
because a static read there proves nothing about the runtime value.
"""
import struct, sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe.unpatched"

data = EXE.read_bytes()
pe = struct.unpack_from("<I", data, 0x3C)[0]
nsec = struct.unpack_from("<H", data, pe + 6)[0]
opt = struct.unpack_from("<H", data, pe + 20)[0]
base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    s = pe + 24 + opt + i * 40
    name = data[s:s + 8].rstrip(b"\0").decode()
    vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
    secs.append((name, vaddr, vsz, roff, rsz))

print(f"image base 0x{base:08x}   {EXE.name}  {len(data)} bytes")
for va_s in sys.argv[1:]:
    va = int(va_s, 16)
    rva = va - base
    hit = None
    for name, vaddr, vsz, roff, rsz in secs:
        if vaddr <= rva < vaddr + max(vsz, rsz):
            hit = (name, vaddr, vsz, roff, rsz)
            break
    print(f"\n=== {va_s} (rva 0x{rva:06x}) ===")
    if not hit:
        print("  NOT MAPPED in any section")
        continue
    name, vaddr, vsz, roff, rsz = hit
    delta = rva - vaddr
    print(f"  section {name}  vaddr 0x{vaddr:06x} vsz 0x{vsz:x} raw 0x{roff:x} rsz 0x{rsz:x}")
    if delta >= rsz:
        # NOT "proves nothing" - that was this tool's original wording and it was too
        # cautious, which cost a real answer once. A PE section whose VirtualSize
        # exceeds SizeOfRawData has its tail ZERO-FILLED by the loader, so the value
        # at process start is definitely 0. What is unknown is only whether code
        # writes it afterwards, and `decomp_pc.py --datarefs` answers exactly that.
        print("  BSS TAIL: past the section's raw data, so there are no bytes in the file.")
        print("  The PE loader zero-fills a section's virtual tail, so the value AT PROCESS")
        print("  START is 0. Whether anything writes it later is a separate question ->")
        print(f"  py -3.12 re/tools/decomp_pc.py {va_s} --datarefs")
        continue
    off = roff + delta
    b = data[off:off + 16]
    d = struct.unpack_from("<I", data, off)[0]
    f = struct.unpack_from("<f", data, off)[0]
    print(f"  file offset 0x{off:x}")
    print(f"  bytes  {' '.join(f'{x:02x}' for x in b)}")
    print(f"  dword  0x{d:08x}  ({d}, signed {struct.unpack('<i', struct.pack('<I', d))[0]})")
    print(f"  float  {f!r}")
