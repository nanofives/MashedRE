"""D4-FX-INPUTS attempt 3: pair the AI step's ctrl byte 5 with the FX gate's read (MASHED_FX_PAIRLOG csv).

usage: py -3.12 pair_score.py <pair.csv> <step.csv>
Per AI slot, alive rows only (the step dump writes alive cars only): rows, c5_step == c5_fx, both > 0x40, step only,
fx only; and against the step dump: rows where c5_step equals the step.csv c5 of the same (frame, v).
"""
import csv, sys, collections
pair = list(csv.DictReader(open(sys.argv[1])))
step = {(int(r['frame']), int(r['v'])): int(r['c5']) for r in csv.DictReader(open(sys.argv[2]))}
for v in (1, 2, 3):
    rows = [r for r in pair if int(r['v']) == v and r['alive'] == '1']
    n = len(rows); same = sum(r['c5_step'] == r['c5_fx'] for r in rows)
    bs = sum(int(r['c5_step']) > 0x40 for r in rows); bf = sum(int(r['c5_fx']) > 0x40 for r in rows)
    both = sum(int(r['c5_step']) > 0x40 and int(r['c5_fx']) > 0x40 for r in rows)
    m = [r for r in rows if (int(r['step_frame']), v) in step]
    agree = sum(step[(int(r['step_frame']), v)] == int(r['c5_step']) for r in m)
    rng = sum(r['rng'] == '1' for r in rows)
    sb = collections.Counter(int(r['c5_step']) for r in rows).most_common(4)
    fb = collections.Counter(int(r['c5_fx']) for r in rows).most_common(4)
    print(f'v={v} alive gate rows={n} c5_step==c5_fx {same} | >0x40 step {bs} fx {bf} both {both} | rng {rng} '
          f'| step.csv match {agree}/{len(m)} | step top {sb} | fx top {fb}')
