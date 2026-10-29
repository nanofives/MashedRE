r"""D2-BAND-42S band builder (PREREG_D2-BAND-42S.md section 3). Reads score_d2_v2.py `orig` output lines and writes a band
file with the same schema as verify/d2rebase_20261019/band.json. Rule: mean +/- max(3h, 2% of mean), h = half-range.
usage: py -3.12 build_band.py <scores.txt> <out.json> <label> <run> [<run> ...]   (run = msd basename without .msd)"""
import json, re, sys

RULE = 'mean +/- max(3h, 2% of mean), h = half-range of the runs (d81a8df6 / re/analysis/D2_REOPEN_2026-09-29.md 3a)'


def main():
    src, out, label, runs = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4:]
    vals = {}
    for line in open(src, encoding='utf-8'):
        m = re.match(r'(\S+)\.msd: slip1500=([\d.]+)\S* \S* ?slip2000=([\d.]+)\S* \S* ?drvmed=([\d.]+)', line)
        if not m:
            m = re.match(r'(\S+)\.msd: slip1500=([\d.]+)(?: \w+)? slip2000=([\d.]+)(?: \w+)? drvmed=([\d.]+)', line)
        if m:
            vals[m.group(1).replace('\\', '/').split('/')[-1]] = dict(slip1500=float(m.group(2)), slip2000=float(m.group(3)),
                                                                      drvmed=float(m.group(4)))
    missing = [r for r in runs if r not in vals]
    if missing:
        sys.exit(f'missing scores for {missing}')
    b = {'rule': RULE, 'label': label, 'runs': {r: vals[r] for r in runs}}
    for k, nd in (('slip1500', 6), ('slip2000', 6), ('drvmed', 4)):
        xs = [vals[r][k] for r in runs]
        mean = sum(xs) / len(xs)
        h = (max(xs) - min(xs)) / 2
        two = 0.02 * mean
        tol = max(3 * h, two)
        b[k] = {'mean': round(mean, nd), 'min': min(xs), 'max': max(xs), 'h': round(h, 6), '3h': round(3 * h, 6),
                '2pct': round(two, 6), 'tol': round(tol, 6), 'width_set_by': '3h' if 3 * h >= two else '2%',
                'band': [round(mean - tol, 5 if k != 'drvmed' else 2), round(mean + tol, 5 if k != 'drvmed' else 2)]}
    json.dump(b, open(out, 'w'), indent=1)
    for k in ('slip1500', 'slip2000', 'drvmed'):
        print(k, b[k])


if __name__ == '__main__':
    main()
