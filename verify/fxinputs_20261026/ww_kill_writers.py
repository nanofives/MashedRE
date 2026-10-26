"""Summarise a scenario_launch --watch-write 4,9b0 capture around each kill of the watched car.

usage: py -3.12 ww_kill_writers.py <watch.csv> [window_rows]

Columns: seq,pc,ret0,ret1,ret2,w0,w1,bt (w0 = rec+0x4 as float bits, w1 = rec+0x9b0 float).
A kill row = the watched rec+0x4 dword becoming 0 (w0 == 0.0). Prints: pc histogram overall,
pc histogram of rows while rec+0x4 == 0, and the first rows after each kill with their
accurate backtraces.
"""
import collections, csv, sys

path = sys.argv[1]
win = int(sys.argv[2]) if len(sys.argv) > 2 else 12
rows = list(csv.DictReader(open(path, newline='')))
print(f'{len(rows)} rows')
allc = collections.Counter((r['pc'], r['ret0']) for r in rows)
print('-- all writes (pc, ret0) top 15')
for k, n in allc.most_common(15):
    print(f'  {n:7d} {k}')
dead = []
kills = []
prev_alive = None
for i, r in enumerate(rows):
    alive = float(r['w0']) != 0.0
    if prev_alive is True and not alive:
        kills.append(i)
    if not alive:
        dead.append(r)
    prev_alive = alive
print(f'-- kills (rec+0x4 -> 0): {len(kills)} at rows {kills[:20]}')
dc = collections.Counter((r['pc'], r['ret0'], r['ret1']) for r in dead)
print(f'-- writes while rec+0x4 == 0: {len(dead)} (pc, ret0, ret1) top 20')
for k, n in dc.most_common(20):
    print(f'  {n:7d} {k}')
nz = collections.Counter((r['pc'], r['ret0']) for r in dead if float(r['w1']) == 0.0)
print('-- of those, writes leaving +0x9b0 == 0.0, (pc, ret0)')
for k, n in nz.most_common(10):
    print(f'  {n:7d} {k}')
for ki in kills[:6]:
    print(f'== kill at row {ki}')
    for r in rows[max(0, ki - 2): ki + win]:
        print(f"  {r['seq']:>7} pc={r['pc']} r={r['ret0']},{r['ret1']},{r['ret2']} "
              f"w0={float(r['w0']):.3g} vx={float(r['w1']):.6g} bt={r['bt'][:120]}")
