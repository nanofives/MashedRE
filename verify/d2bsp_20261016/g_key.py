"""G-KEY (PREREG_d2bsp.md section 3): the +0x34 key (entry dword 13) of every port-side batch entry, per arm.
usage: py -3.12 g_key.py <P_ arm .batch.csv> [...]
PASS for a fix-W arm = every key is one of Training's five collision colours; for a stand-in arm = every key 0.
Also prints the query radius histogram (U-9181: the port's +0x4a4) and the entry-count histogram."""
import collections, csv, sys

csv.field_size_limit(1 << 30)
TRAINING = {'ffc88080', 'ff008000', 'ffb48080', 'ff010101', 'ffff0080'}
for p in sys.argv[1:]:
    keys, rad, cnt = collections.Counter(), collections.Counter(), collections.Counter()
    n = 0
    with open(p, newline='') as f:
        for r in csv.DictReader(f):
            n += 1
            rad[r['r']] += 1
            cnt[int(r['count'])] += 1
            for e in (r['entries'].split('|') if r['entries'] else []):
                keys[e.split(';')[13]] += 1
    allt = set(keys) <= TRAINING and bool(keys)
    all0 = set(keys) == {'00000000'}
    print(f'{p}: queries {n}, entries {sum(keys.values())}')
    print(f'   keys {dict(keys.most_common())}')
    print(f'   all Training colours: {allt}   all zero: {all0}')
    print(f'   query radius {dict(rad.most_common(3))}')
    print(f'   count histogram {sorted(cnt.items())[:24]}')
