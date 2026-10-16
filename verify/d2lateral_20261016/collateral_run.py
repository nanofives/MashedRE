"""Collateral (PREREG_d2lateral.md section 5), recipe of verify/d3_u9684_20261013/RESULT_U9684.md 3.3 as d2posmodel
ran it: collateral.py --mode banded, A = o_p2b (floor o_p0a), 600 frames from s.v2.substate >= 6 (cut600.py),
--speed s.v2.rec_9e4 --anchor s.v2.substate:ge:6 --bands 1000-1500,1500-2000,2000-2600.
usage: py -3.12 collateral_run.py <armB> <armB-floor> [<tag>]    e.g. H_B H_B-2 B"""
import os, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
W = os.path.dirname(os.path.dirname(D))
M = r'C:\Users\maria\Desktop\Proyectos\Mashed'
SCR = r'C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed\e3a88210-98a6-42d2-aeaa-b0cc1a3255e9\scratchpad'
CUT = os.path.join(W, r'verify\d3_u9684_20261013\cut600.py')
COL = os.path.join(W, r're\tools\statediff\collateral.py')
b, bf = sys.argv[1], sys.argv[2]
tag = sys.argv[3] if len(sys.argv) > 3 else b[2:]
for o, name in (('o_p2b', 'cut_o_p2b.csv'), ('o_p0a', 'cut_o_p0a.csv')):
    if not os.path.exists(os.path.join(SCR, name)):
        subprocess.run([sys.executable, CUT, os.path.join(M, rf'verify\d3_car2pickup_20261013\{o}.msd.aistep.csv'),
                        os.path.join(SCR, name)], check=True)
for arm in (b, bf):
    subprocess.run([sys.executable, CUT, os.path.join(D, arm + '.step.csv'), os.path.join(D, f'cut_{arm}.csv')], check=True)
out = os.path.join(D, f'COLLATERAL_CROSS_H_{tag}.txt')
r = subprocess.run([sys.executable, COL, '--mode', 'banded',
                    '--a', 'csv:' + os.path.join(SCR, 'cut_o_p2b.csv'), '--floor-a', 'csv:' + os.path.join(SCR, 'cut_o_p0a.csv'),
                    '--b', 'csv:' + os.path.join(D, f'cut_{b}.csv'), '--floor-b', 'csv:' + os.path.join(D, f'cut_{bf}.csv'),
                    '--speed', 's.v2.rec_9e4', '--anchor', 's.v2.substate:ge:6',
                    '--bands', '1000-1500,1500-2000,2000-2600', '--out', out], capture_output=True, text=True)
open(os.path.join(D, f'COLLATERAL_CROSS_H_{tag}_stdout.txt'), 'w').write(r.stdout + r.stderr)
for line in r.stdout.splitlines():
    if line.startswith('#') or 'exceed their floor' in line:
        print(line)
