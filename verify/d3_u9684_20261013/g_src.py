"""G-SRC (PREREG_U9684.md section 2): the original's car-2 post-contact command, checked row by row.

usage: py -3.12 g_src.py <aistep.csv> [<aistep.csv> ...]   (car 2 rows; read-only)

Contact row = the first car-2 row whose rec_9e4 drops by more than 1000 from the previous row (the FUN_00469df0
impulse). Run = the maximal block of consecutive rows with ret14a70 == 1 that starts within 2 rows before that.
Per run row k = 1..N (row 0 = the row before the run):
  M  ai_mode == 3 and ret14a70 == 1 and ret16060 == 1      (0x0041639c, 0x004163ab, 0x004163f6)
  E  180 < err < 330, err = hist_d8 on the high branch (hist_dc == 0)   (0x00416863..0x00416885)
  B  c1 == c4 == c5 == 255                                  (0x0041687d / 0x00416881 / 0x00416885)
  Z  c0 == int((200 - (clk_k - clk_0)) * 0.005 * c0_0) while clk_k - clk_0 < 200, else c0 == 0
     (0x00416738..0x00416758; c0_0 = row 0's c0, the value the LO band stored at 0x0089a4f4)
The row after the run is printed as a control (mode back to 0 -> E/B should no longer hold).
"""
import csv, sys

def f(x):
    try: return float(x)
    except ValueError: return None

for path in sys.argv[1:]:
    r = [x for x in csv.DictReader(open(path, newline='')) if x['v'] == '2']
    drop = next(i for i in range(1, len(r))
                if f(r[i - 1]['rec_9e4']) is not None and f(r[i]['rec_9e4']) is not None
                and f(r[i - 1]['rec_9e4']) - f(r[i]['rec_9e4']) > 1000.0)
    s = next(i for i in range(max(0, drop - 2), len(r)) if r[i]['ret14a70'] == '1')
    e = s
    while e + 1 < len(r) and r[e + 1]['ret14a70'] == '1': e += 1
    r0 = r[s - 1]; c00 = int(r0['c0']); clk0 = int(r0['clk_0ff4'])
    n = e - s + 1; ok = {'M': 0, 'E': 0, 'B': 0, 'Z': 0}
    print(f'== {path}  drop row {drop} (frame {r[drop]["frame"]})  run rows {s}..{e} frames '
          f'{r[s]["frame"]}..{r[e]["frame"]}  N={n}  row0 c0={c00} clk0={clk0}')
    for i in range(s, e + 2):
        x = r[i]; err = f(x['hist_d8']) if f(x['hist_dc']) == 0.0 else f(x['hist_dc'])
        el = int(x['clk_0ff4']) - clk0
        pz = int((200 - el) * 0.005 * c00) if el < 200 else 0
        m = x['ai_mode'] == '3' and x['ret14a70'] == '1' and x['ret16060'] == '1'
        ee = err is not None and 180.0 < err < 330.0
        b = x['c1'] == '255' and x['c4'] == '255' and x['c5'] == '255'
        z = int(x['c0']) == pz
        tag = 'run' if i <= e else 'after'
        if i <= e:
            ok['M'] += m; ok['E'] += ee; ok['B'] += b; ok['Z'] += z
        print(f'  {tag:5s} row {i} frame {x["frame"]} mode {x["ai_mode"]} ret {x["ret14a70"]}/{x["ret16060"]} '
              f'err {err:.3f} c0 {x["c0"]} (pred {pz}) c1 {x["c1"]} c4 {x["c4"]} c5 {x["c5"]}  M{int(m)} E{int(ee)} B{int(b)} Z{int(z)}')
    print('  totals: ' + '  '.join(f'{k} {v}/{n}' for k, v in ok.items()))
