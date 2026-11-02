# MATCH-PORTSTATE-RECORDS: list every instruction in a PE's executable sections whose displacement or
# immediate falls inside one of the original's record ranges (the exe's absolute readers / writers of them).
# Usage: py -3.12 absref_scan.py <pe> [--map <mapfile>]
import struct
import sys
import bisect

import capstone

RANGES = [
    ('veh_records', 0x008815a0, 0x008815a0 + 16 * 0xd04),   # 16 x 0xd04 (0x00881e80 = +0x8e0 of record 0)
    ('car_visual', 0x0063d9e0, 0x0063d9e0 + 4 * 0x2ac),      # 4 x 0x2ac
    ('score_blk', 0x008995bc, 0x008995bc + 16 * 0x138),      # 16 x 0x138 (count: PREREG)
]


def sections(pe):
    peo = struct.unpack_from('<I', pe, 0x3c)[0]
    nsec = struct.unpack_from('<H', pe, peo + 6)[0]
    optsz = struct.unpack_from('<H', pe, peo + 20)[0]
    base = struct.unpack_from('<I', pe, peo + 24 + 28)[0]
    so = peo + 24 + optsz
    out = []
    for i in range(nsec):
        o = so + i * 40
        name = pe[o:o + 8].rstrip(b'\0').decode('latin1')
        vsz, va, rsz, raw = struct.unpack_from('<IIII', pe, o + 8)
        ch = struct.unpack_from('<I', pe, o + 36)[0]
        out.append((name, base + va, vsz, raw, rsz, ch))
    return base, out


def load_map(path):
    syms = []
    for ln in open(path, encoding='latin1'):
        p = ln.split()
        if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
            try:
                syms.append((int(p[2], 16), p[1], p[-1]))
            except ValueError:
                pass
    syms.sort()
    return syms


def main():
    pe = open(sys.argv[1], 'rb').read()
    syms = load_map(sys.argv[sys.argv.index('--map') + 1]) if '--map' in sys.argv else []
    keys = [s[0] for s in syms]
    _, secs = sections(pe)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    hits = []
    for name, va, vsz, raw, rsz, ch in secs:
        if not ch & 0x20000000:
            continue
        code = pe[raw:raw + min(vsz, rsz)]
        for ins in md.disasm(code, va):
            if ins.mnemonic == '.byte':
                continue
            vals = []
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_IMM:
                    vals.append(op.imm & 0xffffffff)
                elif op.type == capstone.x86.X86_OP_MEM:
                    vals.append(op.mem.disp & 0xffffffff)
            for v in vals:
                for tag, lo, hi in RANGES:
                    if lo <= v < hi:
                        fn = ''
                        if syms:
                            k = bisect.bisect_right(keys, ins.address) - 1
                            if k >= 0:
                                fn = '%s (%s)' % (syms[k][1], syms[k][2])
                        hits.append((tag, ins.address, v, '%s %s' % (ins.mnemonic, ins.op_str), fn))
    for h in hits:
        print('%-11s %08x  ->%08x  %-48s %s' % h)
    print('total', len(hits))


if __name__ == '__main__':
    main()
