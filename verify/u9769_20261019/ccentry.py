"""u9769: VehicleCarCarContact entry / leave records, original --carcar-probe rows vs the port's MASHED_CC_TRACE rows, paired by
order among the calls that returned 1 (the contacts). Prints differing dwords per side (self / other, in / out), skipping
the fields that also differ on the exact car 1. usage: ccentry.py <orig.carcar.tsv> <port.cctrace.tsv> [ncontacts]"""
import csv, struct, sys
csv.field_size_limit(10 ** 8)
f = lambda h, o: struct.unpack_from('<f', bytes.fromhex(h), o)[0]
noise = {0x44, 0x48, 0x13c, 0x140, 0x460, 0x994} | set(range(0xaf4, 0xb00, 4)) | {0xb14, 0xb1c} | set(range(0xb2c, 0xbec, 4)) | \
        {0x944, 0x954, 0x964, 0x984, 0x9a4, 0xd00, 0x9d4, 0x9dc} | set(range(0x1ac, 0x1ec, 4)) | set(range(0x270, 0x2b0, 4))
O = [r for r in csv.DictReader(open(sys.argv[1]), delimiter='\t') if r['ret'] == '1']
P = [r for r in csv.DictReader(open(sys.argv[2]), delimiter='\t') if r['ret'] == '1']
n = int(sys.argv[3]) if len(sys.argv) > 3 else 2
print('contacts: orig %d port %d' % (len(O), len(P)))
for i in range(min(n, len(O), len(P))):
    o, p = O[i], P[i]
    print('== contact %d  orig tick %s self %s other %s pass %s | port tick %s self %s other %s pass %s' % (
        i, o['tick'], o['self'], o['other'], o['pass'], p['tick'], p['self'], p['other'], p['pass']))
    for col in ('self_in', 'other_in', 'self_out', 'other_out'):
        a, b = bytes.fromhex(o[col]), bytes.fromhex(p[col])
        d = [x for x in range(0, 0xd04, 4) if a[x:x + 4] != b[x:x + 4] and x not in noise]
        print('  %-9s %3d  %s' % (col, len(d), ' '.join('+0x%x(%.7g|%.7g)' % (x, f(o[col], x), f(p[col], x)) for x in d[:9])))
