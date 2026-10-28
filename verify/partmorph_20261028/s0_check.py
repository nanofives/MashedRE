r"""D5-PARTMORPH-PRODUCER P3 / P4 / P5 checker (PREREG_D5-PARTMORPH-PRODUCER.md section 4).

usage: py -3.12 s0_check.py <arm>.s0.csv [--car0]

Replays every L row of MASHED_DBG_CAR_STATE0 through an independent model of PREREG section 1.3:
  tail = s in (7, -1) or (s == 6 and pre == 0 and n == 0)
  all0 = tail or (rule == 5 and cdone)
  car0 = not all0 and (mode == 2 or rule in (5, 10, 9))
and every D row against the decision it names:
  state = 0 when all0, 0 for slot 0 when car0, else the logged CarLodState value;
  the decision id = the latest L id written before the D row, minus 1 (the previous tick's decision);
  -1 before the second L row (amended after H_S0: the first draws precede any decision).
--car0: P5, also requires car0 on every L row without all0, and slots 1..3 never 0 on car0 decisions.
"""
import argparse
import collections


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('csv')
    ap.add_argument('--car0', action='store_true')
    a = ap.parse_args()
    L = {}
    bad_l = bad_d = bad_id = bad_n = 0
    last_l = -1
    d_rows = 0
    subs = collections.Counter()
    st0 = collections.Counter()
    all0_ids = []
    d_by_id = collections.defaultdict(list)
    first_bad = []
    with open(a.csv) as f:
        next(f)
        for line in f:
            p = line.strip().split(',')
            if p[0] == 'L':
                i, s, pre, n, rule, mode, cd, all0, car0 = (int(x) for x in p[1:10])
                tail = s in (7, -1) or (s == 6 and pre == 0 and n == 0)
                m_all0 = tail or (rule == 5 and cd == 1)
                m_car0 = (not m_all0) and (mode == 2 or rule in (5, 10, 9))
                if (s == 6 and pre == 0) != (n >= 0):
                    bad_n += 1
                if int(m_all0) != all0 or int(m_car0) != car0:
                    bad_l += 1
                    if len(first_bad) < 5: first_bad.append(line.strip())
                L[i] = (s, pre, n, rule, mode, cd, all0, car0)
                subs[(s, pre)] += 1
                if all0: all0_ids.append(i)
                last_l = i
            elif p[0] == 'D':
                i, slot, lod, st = (int(x) for x in p[1:5])
                d_rows += 1
                if i != max(last_l - 1, -1):   # before the first L row: no decision yet (-1)
                    bad_id += 1
                if i < 0:
                    exp = lod
                else:
                    _, _, _, _, _, _, all0, car0 = L[i]
                    exp = 0 if (all0 or (car0 and slot == 0)) else lod
                if st != exp:
                    bad_d += 1
                    if len(first_bad) < 5: first_bad.append(line.strip())
                if st == 0: st0[slot] += 1
                d_by_id[i].append((slot, lod, st))
    print(f'L rows={len(L)} D rows={d_rows}')
    print('L rows by (sub, pre):', dict(sorted(subs.items())))
    print(f'L model mismatches={bad_l}  n-consistency mismatches={bad_n}')
    print(f'D state mismatches={bad_d}  D decision-id mismatches={bad_id}')
    print('D rows with state 0 by slot:', dict(sorted(st0.items())))
    # all0 windows (consecutive L ids)
    win = []
    for i in all0_ids:
        if win and win[-1][1] == i - 1: win[-1][1] = i
        else: win.append([i, i])
    print(f'all0 windows={len(win)}', [(w0, w1, L[w0][0]) for w0, w1 in win[:12]])
    slots_all0 = set()
    for i in all0_ids:
        for slot, lod, st in d_by_id.get(i, []):
            if st == 0: slots_all0.add(slot)
    print('slots drawn with state 0 on all0 decisions:', sorted(slots_all0))
    ok = bad_l == 0 and bad_d == 0 and bad_id == 0 and bad_n == 0
    if a.car0:
        miss = sum(1 for i, v in L.items() if not v[6] and not v[7])
        other0 = sum(1 for i, v in L.items() if v[7] and not v[6]
                     for slot, lod, st in d_by_id.get(i, []) if slot != 0 and st == 0)
        s0rows = sum(1 for i, v in L.items() if v[7] and not v[6]
                     for slot, lod, st in d_by_id.get(i, []) if slot == 0 and st == 0)
        print(f'P5: L rows without all0 and without car0={miss}  slot-0 state-0 D rows on car0={s0rows}  '
              f'slots 1..3 state 0 on car0={other0}')
        ok = ok and miss == 0 and other0 == 0 and s0rows > 0
    for b in first_bad: print('  first bad:', b)
    print('PASS' if ok else 'FAIL')


if __name__ == '__main__':
    main()
