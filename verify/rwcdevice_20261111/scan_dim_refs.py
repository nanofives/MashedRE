"""RWC-DEVICE: list every instruction byte site in MASHED.exe.unpatched's .text that encodes the absolute address
0x00616028 or 0x0061602c (the cells behind the screen-dim getters 0x00498bc0 / 0x00498bd0).

    py -3.12 verify/rwcdevice_20261111/scan_dim_refs.py [path to MASHED.exe.unpatched]

Prints: target, address of the 4-byte operand, the two bytes before it (opcode / ModRM). A raw byte scan: it lists
every occurrence of the 4 address bytes in .text, so it cannot miss an absolute reference; a match inside other data
would show up as an extra row.
"""
import re
import struct
import sys

DEFAULT = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    so = struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    for i in range(nsec):
        o = pe + 24 + so + i * 40
        name = d[o:o + 8].rstrip(b"\0").decode()
        vs, va, rs, ro = struct.unpack_from("<IIII", d, o + 8)
        if name != ".text":
            continue
        t = d[ro:ro + rs]
        for tgt in (0x00616028, 0x0061602C):
            for m in re.finditer(re.escape(struct.pack("<I", tgt)), t):
                a = base + va + m.start()
                print("0x%08x  operand@0x%08x  prev=%s" % (tgt, a, t[m.start() - 2:m.start()].hex()))


if __name__ == "__main__":
    main()
