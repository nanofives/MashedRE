"""Compare two PE files section by section; report diffs outside the
header TimeDateStamp/CheckSum and the debug directory (+ the CodeView record it points at)."""
import struct, sys


def parse(b):
    pe = struct.unpack_from('<I', b, 0x3c)[0]
    n = struct.unpack_from('<H', b, pe + 6)[0]
    opt = struct.unpack_from('<H', b, pe + 20)[0]
    o = pe + 24
    dd = o + 96  # PE32 data directories
    dbg_rva, dbg_sz = struct.unpack_from('<II', b, dd + 6 * 8)
    secs = []
    for i in range(n):
        nm, vs, va, rs, ro = struct.unpack_from('<8sIIII', b, o + opt + 40 * i)
        secs.append((nm.rstrip(b'\0').decode(), va, vs, ro, rs))
    def r2o(rva):
        for _, va, vs, ro, rs in secs:
            if va <= rva < va + rs:
                return ro + rva - va
    ign = set(range(pe + 8, pe + 12)) | set(range(o + 64, o + 68))
    if dbg_rva:
        d0 = r2o(dbg_rva)
        ign |= set(range(d0, d0 + dbg_sz))
        for k in range(dbg_sz // 28):
            # IMAGE_DEBUG_DIRECTORY: chars, stamp, major/minor, type, SizeOfData, rva, PointerToRawData
            sz, _, ptr = struct.unpack_from('<III', b, d0 + 28 * k + 16)
            if ptr:
                ign |= set(range(ptr, ptr + sz))
    return secs, ign


a = open(sys.argv[1], 'rb').read(); b = open(sys.argv[2], 'rb').read()
sa, ia = parse(a); sb, ib = parse(b)
print('size', len(a), len(b), 'sections', [s[0] for s in sa] == [s[0] for s in sb])
ign = ia | ib
hdr_end = sa[0][3]
for label, lo, hi in [('headers', 0, hdr_end)] + [(s[0], s[3], s[3] + s[4]) for s in sa]:
    d = [i for i in range(lo, min(hi, len(a), len(b))) if a[i] != b[i]]
    real = [i for i in d if i not in ign]
    print(f'{label:<8} ndiff={len(d)} outside_stamp_debug={len(real)}' + (f' first=0x{real[0]:x}' if real else ''))
