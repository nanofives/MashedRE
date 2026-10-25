r"""D3-CF-BOX box-state sequence reducer (PREREG_D3-CF-BOX.md section 5, P2 / P3).

usage: py -3.12 boxseq.py <orig .elim.csv> [<port .box.csv> ...]

Original rows: fn = boxset (a0 idx, a1 val, a2 return address, a3 previous), box3 (a0 slot, a1 [EDI+0x18], a2 previous),
kill (a0 victim). Port rows: frame,class,slot,value,prev.
Classes by return address: 0x0040ff70 / 0x0040ffa1 start (FUN_004111c0), 0x0040eeb3 grid (FUN_0040e590),
0x0042300a kill (FUN_00422fd0), 0x00425134 / 0x004251cd respawn (FUN_00424eb0), 0x0040c57d event (FUN_0040be50).
Span (P3a, amendment 1): the original capture's own rows; the port trace is cut to that many rows. Extra .elim.csv
arguments are original repeats compared row for row.
"""
import csv, sys

RA = {'0x40ff70': 'start', '0x40ffa1': 'start', '0x40eeb3': 'grid', '0x42300a': 'kill',
      '0x425134': 'respawn', '0x4251cd': 'respawn', '0x40c57d': 'event'}


def span(seq):
    k = next((i for i, e in enumerate(seq) if e[0] == 'kill'), None)
    if k is None: return seq
    g = next((i for i in range(k, len(seq)) if seq[i][0] == 'grid'), None)
    if g is None: return seq
    while g + 1 < len(seq) and seq[g + 1][0] == 'grid': g += 1
    return seq[:g + 1]


def orig(path):
    seq, p2 = [], []
    rows = list(csv.DictReader(open(path, newline='')))
    for i, r in enumerate(rows):
        if r['fn'] == 'boxset':
            cls = RA.get(r['a2'], 'UNKNOWN:' + r['a2'])
            seq.append((cls, int(r['a0']), int(r['a1']), r['frame'], r['ba8c']))
        elif r['fn'] == 'box3':
            seq.append(('3', int(r['a0']), 3, r['frame'], r['ba8c']))
        elif r['fn'] == 'kill':
            nxt = next((q for q in rows[i + 1:] if q['fn'] == 'boxset'), None)
            ok = nxt is not None and nxt['a2'] == '0x42300a' and nxt['a0'] == r['a0'] and nxt['a1'] == '2'
            p2.append((r['a0'], r['frame'], ok))
    return seq, p2


def port(path):
    return [(r['class'], int(r['slot']), int(r['value']), r['frame'], '') for r in csv.DictReader(open(path, newline=''))]


def show(name, seq):
    print(f'--- {name}: {len(seq)} rows')
    for e in seq: print('   ', e[0], 'slot', e[1], 'val', e[2], 'frame', e[3], ('ba8c ' + e[4]) if e[4] else '')


o, p2 = orig(sys.argv[1])
show('original (all)', o)
unk = [e for e in o if e[0].startswith('UNKNOWN')]
gap = [e for e in o if e[0] in ('respawn', 'event')]
print(f'P2: unknown return addresses {len(unk)}; kill rows {len(p2)}, followed by a 0x0042300a write of 2 to the same slot: '
      f'{sum(1 for k in p2 if k[2])}/{len(p2)}; slice-1 gap rows (respawn / event) {len(gap)}')
os_ = [e[:3] for e in o]
print(f'P3a span = the original rows: {len(os_)}')
for pp in sys.argv[2:]:
    if pp.endswith('.elim.csv'):
        q = [e[:3] for e in orig(pp)[0]]
        print(f'O-repeat {pp}: {len(q)} rows -> {"EQUAL" if q == os_ else "DIFFER"}')
        continue
    q = port(pp)
    show(pp, q)
    qs = [e[:3] for e in q][:len(os_)]
    eq = os_ == qs
    print(f'P3a {pp}: original {len(os_)} / port cut {len(qs)} -> {"EQUAL" if eq else "DIFFER"}')
    if not eq:
        for i in range(max(len(os_), len(qs))):
            a = os_[i] if i < len(os_) else None; b = qs[i] if i < len(qs) else None
            if a != b:
                print(f'   first difference at {i}: original {a} port {b}'); break
