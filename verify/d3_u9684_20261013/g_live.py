"""G-LIVE (PREREG_U9684.md section 2): the exe's post-contact AI command against the original's, per contact.

usage: py -3.12 g_live.py <orig aistep.csv> <exe step.csv> [n=16]

Contact call = the call on whose frame FUN_00469df0 fires. Both dumps show it as a rec_9e4 drop of more than 800
between consecutive rows of one car, but at different phases (memory aistep-probe-phase-and-ctrl-zeroing):
  original: the probe row of the contact frame + 1 already carries the new geometry and the drop shows one row later
            (o_p2a: contact frame 855 = RESULT_CAR2PICKUP, ret14a70 run from 856, drop at 857), so call 0 = drop - 1;
  exe:      AiStepDump samples after the tick and the physics, the drop shows on the contact call itself
            (C_DEF: contact step 24 = C_DEF.carcar.log, drop at row 24), so call 0 = drop + 1.
Car 2's first contact is scored against the original's; every other exe contact is listed (report only).
PASS (car 2): exe ret14a70==1 run starts at call 0 +- 1, length 16 +- 1, c1 equal on >= 14 of n aligned rows,
|c0 diff| <= 5 on >= 14 of n.
"""
import csv, sys
n = int(sys.argv[3]) if len(sys.argv) > 3 else 16

def load(p):
    return list(csv.DictReader(open(p, newline='')))

def fl(x):
    try: return float(x)
    except (TypeError, ValueError): return None

def drops(r):
    out = []
    for i in range(1, len(r)):
        a, b = fl(r[i - 1]['rec_9e4']), fl(r[i]['rec_9e4'])
        if a is not None and b is not None and a - b > 800.0: out.append(i)
    return out

def run_at(r, i0):
    """first ret14a70==1 run starting in [i0-1, i0+1]; returns (start, length) or (None, 0)"""
    for s in range(max(0, i0 - 1), min(len(r), i0 + 2)):
        if r[s].get('ret14a70') == '1':
            e = s
            while e + 1 < len(r) and r[e + 1].get('ret14a70') == '1': e += 1
            return s, e - s + 1
    return None, 0

O = [x for x in load(sys.argv[1]) if x['v'] == '2']
E_all = load(sys.argv[2])
has = 'ret14a70' in E_all[0]
print(f'exe dump {sys.argv[2]}: ret14a70 column {"present" if has else "ABSENT (TGT3 off)"}')
o0 = drops(O)[0] - 1
E = [x for x in E_all if x['v'] == '2']
e0 = drops(E)[0] + 1
os_, ol = run_at(O, o0); es, el = run_at(E, e0) if has else (None, 0)
print(f'car 2: orig call0 row {o0} (frame {O[o0]["frame"]}) run start {os_} len {ol}; '
      f'exe call0 row {e0} (frame {E[e0]["frame"]}) run start {es} len {el}')
c1ok = c0ok = 0
print(' k | orig mode ret c0 c1 c4 c5 err      | exe mode ret c0 c1 c4 c5 err')
for k in range(n + 2):
    a, b = O[o0 + k], E[e0 + k]
    ea = fl(a['hist_d8']) if fl(a['hist_dc']) == 0.0 else fl(a['hist_dc'])
    eb = fl(b['hist_d8']) if fl(b['hist_dc']) == 0.0 else fl(b['hist_dc'])
    if k < n:
        c1ok += a['c1'] == b['c1']; c0ok += abs(int(a['c0']) - int(b['c0'])) <= 5
    print(f'{k:2d} | {a["ai_mode"]:>4} {a.get("ret14a70",""):>3} {a["c0"]:>3} {a["c1"]:>3} {a["c4"]:>3} {a["c5"]:>3} {ea:8.3f} |'
          f' {b["ai_mode"]:>4} {b.get("ret14a70","-"):>3} {b["c0"]:>3} {b["c1"]:>3} {b["c4"]:>3} {b["c5"]:>3} {eb:8.3f}'
          + ('' if k < n else '  (after)'))
start_ok = es is not None and abs(es - e0) <= 1
len_ok = abs(el - ol) <= 1 and abs(el - 16) <= 1
v = start_ok and len_ok and c1ok >= 14 and c0ok >= 14
print(f'car 2 G-LIVE: start {"ok" if start_ok else "FAIL"} (exe {es} vs call0 {e0}), length {el} vs orig {ol} '
      f'{"ok" if len_ok else "FAIL"}, c1 equal {c1ok}/{n}, |dc0|<=5 {c0ok}/{n} -> {"PASS" if v else "FAIL"}')

print('\nregime-A car-1 / car-3 contacts (exe steps 64 and 129/130, RESULT_CAR2PICKUP.md section 4), report only:')
print('  rows 60..84 and 125..149 per car; mode digit, ret14a70 (1 = X), c1 == 255 (X)')
for v_ in ('1', '3'):
    R = [x for x in E_all if x['v'] == v_]
    for lo in (60, 125):
        seg = R[lo:lo + 25]
        md = ''.join(x['ai_mode'] if len(x['ai_mode']) == 1 else '?' for x in seg)
        rt = ''.join('X' if x.get('ret14a70') == '1' else ('2' if x.get('ret14a70') == '2' else '.') for x in seg)
        c1 = ''.join('X' if x['c1'] == '255' else '.' for x in seg)
        sp = ' '.join(f'{fl(x["rec_9e4"]):.0f}' for x in seg[:8])
        print(f'  car {v_} rows {lo}..{lo + 24}: mode {md}  ret1 {rt}  c1 {c1}  rec_9e4[{lo}..{lo + 7}] {sp}')
