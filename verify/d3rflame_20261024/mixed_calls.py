"""Count dispatcher calls where the R_FLAME query site 0x45afcc returned BOTH a hit and a miss.

Only on such a call can the scan order decide which spark consumes the hit, so only such a call
can make the sort change a query count.
    py -3.12 mixed_calls.py <capture.pucontact.csv> [...]
"""
import csv, sys
from collections import defaultdict

for path in sys.argv[1:]:
    rets = defaultdict(list)
    with open(path, newline='') as f:
        for r in csv.DictReader(f):
            if r['ret_addr'].lower() == '0x45afcc':
                rets[int(r['call'])].append(int(r['ret']) != 0)
    multi = [c for c, v in rets.items() if len(v) >= 2]
    mixed = [c for c, v in rets.items() if any(v) and not all(v)]
    print(f'{path}: calls with a 0x45afcc query {len(rets)}, queries {sum(len(v) for v in rets.values())}, '
          f'hits {sum(sum(v) for v in rets.values())}, calls with 2+ queries {len(multi)}, '
          f'calls with a hit AND a miss {len(mixed)}: {sorted(mixed)}')
