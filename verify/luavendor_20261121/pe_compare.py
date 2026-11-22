"""LUA-VENDOR control: compare two PE files byte for byte, masking only the COFF TimeDateStamp, the optional-header
CheckSum and the debug-directory timestamps (link-time values). Prints section sizes and the count of other differing
bytes.  py -3.12 pe_compare.py <a.exe> <b.exe>"""
import struct
import sys


def masked(path):
    d = bytearray(open(path, "rb").read())
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    mask = set(range(pe + 8, pe + 12))                      # TimeDateStamp
    opt = pe + 24
    mask |= set(range(opt + 64, opt + 68))                  # CheckSum
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    secs = []
    o = opt + optsz
    for i in range(nsec):
        nm = bytes(d[o:o + 8]).rstrip(b"\0").decode()
        vs, va, rs, ra = struct.unpack_from("<IIII", d, o + 8)
        secs.append((nm, va, vs, ra, rs))
        o += 40
    dbg_va, dbg_sz = struct.unpack_from("<II", d, opt + 96 + 8 * 6)
    for nm, va, vs, ra, rs in secs:
        if dbg_va and va <= dbg_va < va + vs:
            fo = ra + dbg_va - va
            for k in range(dbg_sz // 28):
                e = fo + 28 * k
                mask |= set(range(e + 4, e + 8))            # debug entry TimeDateStamp
                ty, sz, _, ptr = struct.unpack_from("<IIII", d, e + 12)
                if ty == 2 and sz >= 24:                     # CodeView: RSDS GUID + age
                    mask |= set(range(ptr + 4, ptr + 24))
    for i in mask:
        if i < len(d):
            d[i] = 0
    return bytes(d), secs


a, sa = masked(sys.argv[1])
b, sb = masked(sys.argv[2])
print("sections a", [(n, hex(vs)) for n, _, vs, _, _ in sa])
print("sections b", [(n, hex(vs)) for n, _, vs, _, _ in sb])
print("size", len(a), len(b))
n = sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b))
print("differing bytes after mask:", n)
print("RESULT", "SAME" if n == 0 else "DIFFERENT")
