"""Label an original run's (b) regime from its orig_rampwatch poll (PREREG_BREGIME.md §1). Read-only.
usage: py -3.12 label_regime.py <poll.csv>   -> prints 'A' or 'B' (and the evidence) on the last line"""
import csv, sys
rows = list(csv.DictReader(open(sys.argv[1])))
go = next((r for r in rows if r['substate'] == '6'), None)
if go is None: print('NO-GO'); sys.exit(1)
c0 = int(go['clk_0ff4'])
win = [r for r in rows if r['substate'] in ('6', '7') and 0 <= (int(r['clk_0ff4']) - c0) // 50 <= 220]
vals = [r['mode368'] for r in win]
nz = next(((int(r['clk_0ff4']) - c0) // 50, r['mode368']) for r in win if r['mode368'] not in ('0', '')) if any(r['mode368'] not in ('0', '') for r in win) else None
first1 = nz
from collections import Counter
print('window samples %d, mode368 %s, first ==1 at game frame %s' % (len(win), dict(Counter(vals)), first1))
print({None: 'A'}.get(nz, {'1': 'B', '2': 'C'}.get(nz[1] if nz else None, '?')) if nz else 'A')
