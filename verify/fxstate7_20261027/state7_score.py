r"""D4-FX-STATE7: per-car summary of probe F (orig_fx_state7.py <rep>.F.rows.bin), split by DAT_0063ba8c state.
Per state and car: frames, rec+0x4 alive frames, FUN_0046e9e0 entries, 0x0046eaf2 hits (the +0xcfc rewrite reached),
frames with rec+0xcfc != 0, frames whose last FUN_0046e9e0 block was 0x007f19b8 / a slot block, histograms of rec+0x10,
rec+0x14, rec+0x9f0, and the last-block bytes 4 / 5 on frames with an entry. Then the state-6 -> 7 boundary: per car
rec+0xcfc on the last state-6 frame and the first 3 state-7 frames, and the first state-7 frame where it changes.
usage: py -3.12 state7_score.py <rep>.F.rows.bin [...]"""
import collections, struct, sys

ROW = 136

def rows(path):
    b = open(path, 'rb').read()
    for i in range(len(b) // ROW):
        r = b[i * ROW:(i + 1) * ROW]
        cars = []
        for c in range(4):
            q = r[8 + c * 32: 8 + c * 32 + 32]
            cfc, r10, r14, r9f0 = struct.unpack_from('<iiii', q, 0)
            ne, nw = struct.unpack_from('<HH', q, 16)
            v2, = struct.unpack_from('<f', q, 24)
            cars.append(dict(cfc=cfc, r10=r10, r14=r14, r9f0=r9f0, ne=ne, nw=nw, b4=q[20], b5=q[21], null=q[22],
                             bslot=q[23], v2=v2, s4=q[28], s5=q[29], slot=struct.unpack_from('<b', q, 30)[0]))
        yield dict(st=r[0], ph=r[1], alive=r[2], poked=r[3], n4=r[4], n5=r[5], cars=cars)

for path in sys.argv[1:]:
    R = list(rows(path))
    print('== %s  (%d rows)' % (path.replace('\\', '/').split('/')[-1], len(R)))
    nb = collections.Counter((r['n4'], r['n5']) for r in R)
    print('  block 0x007f19b8 bytes (4,5) over all rows: %s' % dict(nb))
    by = collections.defaultdict(list)
    for r in R: by[r['st']].append(r)
    for st in sorted(by):
        rs = by[st]
        print('  [state %d] %d frames' % (st, len(rs)))
        for c in range(4):
            cs = [r['cars'][c] for r in rs]
            al = sum(1 for r in rs if r['alive'] >> c & 1)
            ent = [x for x in cs if x['ne']]
            print('    car %d alive=%d e9e0=%d eaf2=%d cfc!=0=%d entry-frames=%d null-blk=%d slot-blk=%d r10=%s r14=%s r9f0=%s blk45=%s'
                  % (c, al, sum(x['ne'] for x in cs), sum(x['nw'] for x in cs), sum(1 for x in cs if x['cfc']), len(ent),
                     sum(1 for x in ent if x['null']), sum(1 for x in ent if x['bslot'] != 0xff),
                     dict(collections.Counter(x['r10'] for x in cs)), dict(collections.Counter(x['r14'] for x in cs)),
                     dict(collections.Counter(x['r9f0'] for x in cs)),
                     dict(collections.Counter((x['b4'], x['b5']) for x in ent))))
    i7 = next((i for i, r in enumerate(R) if r['st'] == 7), None)
    if i7 is None:
        print('  no state-7 frame'); continue
    print('  first state-7 row %d (poked flag on it: %d)' % (i7, R[i7]['poked']))
    for c in range(4):
        seq = [R[j]['cars'][c]['cfc'] for j in range(max(0, i7 - 1), min(len(R), i7 + 3))]
        ch = next((j - i7 for j in range(i7 + 1, len(R)) if R[j]['cars'][c]['cfc'] != R[j - 1]['cars'][c]['cfc']), None)
        x = R[i7]['cars'][c]
        print('    car %d cfc[last6, first 3 of 7]=%s first change at +%s  alive@7=%d r10=%d r14=%d e9e0@7=%d eaf2@7=%d'
              % (c, seq, ch, R[i7]['alive'] >> c & 1, x['r10'], x['r14'], x['ne'], x['nw']))
