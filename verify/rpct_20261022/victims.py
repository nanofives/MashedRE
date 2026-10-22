r"""rpct: per-round elimination victims from a MASHED_ELIM_TRACE csv. A victim is a car whose alive flag goes 1 -> 0 between two
consecutive rows of the same round (the trace row n is the race-tail update index). Prints round, row n, car, and the four
stand-in (pct) and slot (o_pct) values on the last row before the kill.
usage: py -3.12 victims.py <elim.csv> [...]"""
import csv, sys

for path in sys.argv[1:]:
    rows = list(csv.DictReader(open(path, newline='')))
    print(path)
    prev = None
    for r in rows:
        if prev is not None and prev['round'] == r['round']:
            for i in range(4):
                if prev[f'alive{i}'] == '1' and r[f'alive{i}'] == '0':
                    print(f"  round={r['round']} n={r['n']} victim=car{i} zoom_prev={prev['zoom_hex']} "
                          + " ".join(f"c{j}:{float(prev[f'pct{j}']):.3f}/{float(prev[f'o_pct{j}']):.3f}" for j in range(4)))
        prev = r
    rs = [r['round'] for r in rows]
    print(f"  rounds={sorted(set(rs), key=int)} rows={len(rows)}")
