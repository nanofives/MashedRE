"""G-SETTLE / G-SPAWNY / G-PROG scorer (PREREG_d2posmodel.md section 3).
usage: py -3.12 g_settle.py <arm> [<arm> ...]      (reads <arm>.settle.txt, <arm>.ptrace.log, <arm>.pathprog.csv)
Original targets (verify/d2settle_20261016/PREREG_d2settle.md 0.3, o_p0a.msd player):
  frame 1 pos (0.50093675, 0.43742681, -1.99903715), frame 2 (0.50093675, 0.43447936, -1.99894500).
"""
import os, re, struct, sys
D = os.path.dirname(os.path.abspath(__file__))
F = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
O1 = (F(0x3f003d64), F(0x3edff668), -1.9990371465682983)    # x, y bit patterns: PREREG_d2settle 0.3 / amendment 2
O2 = (F(0x3f003d64), 0.43447936, -1.9989449977874756)       # frame-2 y known to 8 digits only (error <= 5e-9)
ODZ = -1.9989449977874756 - -1.9990371465682983             # +0.0000921487808
ODY = O2[1] - O1[1]                                          # -0.0029474455
VEC = r'\(([-0-9.eE+]+),([-0-9.eE+]+),([-0-9.eE+]+)\)'


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


for arm in sys.argv[1:]:
    p = lambda ext: os.path.join(D, arm + ext)
    st = open(p('.settle.txt')).read() if os.path.exists(p('.settle.txt')) else ''
    m = re.search(r'SETTLE slot=0 in\.pos=' + VEC + r'.*?s\.pos=' + VEC + r'.*?goff=([-0-9.eE+]+) physy0=([-0-9.eE+]+)', st)
    pt = open(p('.ptrace.log')).read() if os.path.exists(p('.ptrace.log')) else ''
    m1 = re.search(r'^f=1 .*?pos=' + VEC, pt, re.M)
    print(f'== {arm}')
    if not m or not m1:
        print('   MISSING settle or ptrace line'); continue
    pre = [float(m.group(i)) for i in (1, 2, 3)]; sp = [float(m.group(i)) for i in (4, 5, 6)]
    goff, phy0 = float(m.group(7)), float(m.group(8))
    post = [float(m1.group(i)) for i in (1, 2, 3)]
    mv = [post[i] - pre[i] for i in range(3)]; smv = [sp[i] - pre[i] for i in range(3)]
    print(f'   pre(spawn)={pre}  settle.s.pos={sp}  ptrace f=1={post}')
    print(f'   goff={goff!r} physy0={phy0!r}  (original frame-1 y {O1[1]!r}; diff {phy0 - O1[1]:+.3e})')
    print(f'   settle-step move  = ({smv[0]:+.9g}, {smv[1]:+.9g}, {smv[2]:+.9g})')
    print(f'   first-StepCar move= ({mv[0]:+.9g}, {mv[1]:+.9g}, {mv[2]:+.9g})   original (0, {ODY:+.9g}, {ODZ:+.9g})')
    gx = f32(post[0]) == f32(pre[0])
    gz = abs(mv[2] - ODZ) <= 2.4e-7
    gy = abs(mv[1] - ODY) <= 1.0e-5
    print(f'   G-SETTLE: x bit-identical {"PASS" if gx else "FAIL"} | dz {mv[2] - ODZ:+.3e} vs 2.4e-7 '
          f'{"PASS" if gz else "FAIL"} | dy {mv[1] - ODY:+.3e} vs 1e-5 {"PASS" if gy else "FAIL"} '
          f'=> {"PASS" if gx and gz and gy else "FAIL"}')
    if os.path.exists(p('.pathprog.csv')):
        rows = [l.split(',') for l in open(p('.pathprog.csv')).read().splitlines()[1:]]
        v0 = [(int(r[0]), float(r[4])) for r in rows if len(r) > 4 and r[1] == '0']
        vals = sorted({v for _, v in v0})
        last = v0[-1] if v0 else None
        ok = last is not None and abs(last[1] - 29.999977) <= 1e-5
        print(f'   G-PROG: v=0 distinct prog {vals[:8]} last frame {last} => {"PASS" if ok else "FAIL"}')
