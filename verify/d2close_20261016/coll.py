"""Collateral (d2bsp section 3 recipe): RESULT_U9684.md 3.3 recipe.
cut600.py on A = o_p2b, floor A = o_p0a (verify/d3_car2pickup_20261013) and on the B arm + its repeat, then
collateral.py --mode banded --speed s.v2.rec_9e4 --anchor s.v2.substate:ge:6 --bands 1000-1500,1500-2000,2000-2600.
usage: py -3.12 coll.py <arm> [<arm> ...]     (arm = H_W, reads H_W.step.csv and H_W-2.step.csv)
Prints '<arm>: <k> of <n> paired fields exceed their floor'."""
import os, re, subprocess, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.environ.get('D2C_OUT', os.path.join(W, r'verify\d2close_20261016'))
SP = r'C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed\76721d71-18eb-476b-a501-1567b00879d7\scratchpad'
CUT = os.path.join(W, r'verify\d3_u9684_20261013\cut600.py')
COLL = os.path.join(W, r're\tools\statediff\collateral.py')
ORIG = os.path.join(W, r'verify\d3_car2pickup_20261013')


def cut(src, dst):
    if not os.path.exists(dst):
        subprocess.run([sys.executable, CUT, src, dst], check=True)
    return dst


a = cut(os.path.join(ORIG, 'o_p2b.msd.aistep.csv'), os.path.join(SP, 'cut_o_p2b.csv'))
fa = cut(os.path.join(ORIG, 'o_p0a.msd.aistep.csv'), os.path.join(SP, 'cut_o_p0a.csv'))
for arm in sys.argv[1:]:
    b = cut(os.path.join(OUT, arm + '.step.csv'), os.path.join(OUT, 'cut_' + arm + '.csv'))
    fb = cut(os.path.join(OUT, arm + '-2.step.csv'), os.path.join(OUT, 'cut_' + arm + '-2.csv'))
    rep = os.path.join(OUT, 'COLLATERAL_CROSS_' + arm + '.txt')
    p = subprocess.run([sys.executable, COLL, '--mode', 'banded', '--a', 'csv:' + a, '--floor-a', 'csv:' + fa,
                        '--b', 'csv:' + b, '--floor-b', 'csv:' + fb, '--speed', 's.v2.rec_9e4',
                        '--anchor', 's.v2.substate:ge:6', '--bands', '1000-1500,1500-2000,2000-2600',
                        '--out', rep], capture_output=True, text=True)
    open(os.path.join(OUT, 'COLLATERAL_CROSS_' + arm + '_stdout.txt'), 'w').write(p.stdout + p.stderr)
    m = re.search(r'\((\d+) of (\d+) paired fields exceed their floor', p.stdout)
    print(f'{arm}: ' + (f'{m.group(1)} of {m.group(2)} paired fields exceed their floor' if m else
                        'NO SUMMARY LINE (see _stdout.txt)'))
