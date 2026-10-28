r"""D5-PARTMORPH witness W1: the existing original captures (verify/librwrest_checks_20261016/orig_*,
camera_probe.py --carparts, taken 2026-10-16) against FUN_0041f770's gate.

Per arm: rows, car-rows by rec+0x298 ("lod"), and part 0x14's (key "20") modelling matrix on every car-row:
the rotation must be identity and the position the streamed one when rec+0x298 != 0 (the body did not run).
Also lists part 0x12's (key "18") modelling, which the morph does not touch (it writes the atomic
interpolator, not the frame). Prints one line per arm and a total.
"""
import glob
import gzip
import json
import os

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'librwrest_checks_20261016')
tot = {'rows': 0, 'carrows': 0, 'state0': 0, 'p20_nonident': 0, 'p18_nonident': 0}
for arm in sorted(glob.glob(os.path.join(ROOT, 'orig_*'))):
    p = os.path.join(arm, 'trace.csv.carparts.json.gz')
    if not os.path.exists(p):
        continue
    d = json.load(gzip.open(p))
    lod = {}
    ni20 = ni18 = n = 0
    for r in d['rows']:
        for c in r['s']['cars']:
            n += 1
            lod[c['lod']] = lod.get(c['lod'], 0) + 1
            for key, cnt in (('20', 'p20'), ('18', 'p18')):
                q = c['p'].get(key)
                if not q:
                    continue
                m = q['m']
                rot = m[:9]
                ident = all(abs(rot[k] - (1.0 if k in (0, 4, 8) else 0.0)) < 1e-6 for k in range(9))
                if not ident:
                    if cnt == 'p20':
                        ni20 += 1
                    else:
                        ni18 += 1
    s0 = lod.get(0, 0)
    print(f'{os.path.basename(arm)}: rows={len(d["rows"])} car_rows={n} rec+0x298={dict(sorted(lod.items()))} '
          f'part0x14_non_identity={ni20} part0x12_non_identity={ni18}')
    tot['rows'] += len(d['rows'])
    tot['carrows'] += n
    tot['state0'] += s0
    tot['p20_nonident'] += ni20
    tot['p18_nonident'] += ni18
print('TOTAL', tot)
