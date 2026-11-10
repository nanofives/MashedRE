"""rdf.py <va> [...]: print the dword at each VA of the unpatched image as hex, float and double (8 B)."""
import sys, struct, pefile
pe = pefile.PE(r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched", fast_load=True)
d = pe.get_memory_mapped_image()
for a in sys.argv[1:]:
    v = int(a, 16); o = v - 0x400000
    w = d[o:o+4]; q = d[o:o+8]
    sec = next((s.Name.rstrip(b'\0').decode() for s in pe.sections if s.VirtualAddress <= o < s.VirtualAddress + s.Misc_VirtualSize), '?')
    print(f"{v:08x} {sec} {w.hex()} f={struct.unpack('<f', w)[0]!r} i={struct.unpack('<i', w)[0]} d={struct.unpack('<d', q)[0]!r}")
