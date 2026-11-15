"""List E8 rel32 calls to <target> inside [lo, hi) of the anchored MASHED.exe.unpatched, with the 12 instructions before
each (capstone). usage: py -3.12 calls_to.py <exe> <lo> <hi> <target>"""
import struct, sys
import capstone

exe, lo, hi, tgt = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), int(sys.argv[4], 16)
b = open(exe, "rb").read()
pe = struct.unpack_from("<I", b, 0x3c)[0]
nsec = struct.unpack_from("<H", b, pe + 6)[0]
optsz = struct.unpack_from("<H", b, pe + 20)[0]
base = struct.unpack_from("<I", b, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + optsz + 40 * i
    vsz, va, rsz, raw = struct.unpack_from("<IIII", b, o + 8)
    secs.append((base + va, vsz, raw, rsz))


def foff(v):
    for sva, vsz, raw, rsz in secs:
        if sva <= v < sva + max(vsz, rsz):
            return raw + (v - sva)
    raise ValueError(hex(v))


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
code = b[foff(lo):foff(hi)]
ins = list(md.disasm(code, lo))
for k, i in enumerate(ins):
    if i.mnemonic == "call" and i.op_str.startswith("0x") and int(i.op_str, 16) == tgt:
        print("call at 0x%08x" % i.address)
        for j in ins[max(0, k - 12):k + 3]:
            print("   0x%08x  %s %s" % (j.address, j.mnemonic, j.op_str))
