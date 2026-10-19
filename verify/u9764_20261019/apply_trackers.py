"""u9764 tracker transaction (re-classify, no C-level change): new U-9764, one CHANGELOG line. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9764_20261019/RESULT_u9764.md'
ROW = ('| U-9764 | semantic | the steered wheel\'s forward axis y at A5 Phase 0 (`Vehicle/ForceIntegrator.cpp`, `Rw_MatrixFromAxisAngle` '
       'against the original\'s `RwMatrixRotate` before `0x0046de5b`) | On the current default car 1 is bit-exact against o_wobj2 '
       'through call 4 (`recpos.py`); the first difference is wheel 1\'s A6a addend at call 3 (`30.5746307` port, `30.5746327` '
       'original). The new probe `--a6a-probe-wheel` shows the original\'s `la8` = `2.66e-08` / `2.72e-08` at calls 2 / 3 where the '
       'port\'s is 0: the original\'s wheel-1 axis y (`rec + 0x2e8`, A5 Phase 0 at frame start) is ~-2.66e-08, the port\'s 0. ' + R +
       ' | Which term of the steer rotation matrix differs | Probe the original\'s rotation matrix before the vectors transform at '
       '`0x0046de5b` and compare with `Rw_MatrixFromAxisAngle` | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9764 |' not in s  # lane id
    i = s.index('| U-9763 |')
    return s[:i] + ROW + nl + s[i:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  u9764  car 1\'s call-3 gap is wheel 1\'s A6a addend: the original\'s steered axis y is ~-2.66e-08, '
            'the port\'s 0 (new --a6a-probe-wheel probe); MASHED_A6A_LC0 (default OFF, lc0 order) keeps (b) but is not the cause; '
            'new U-9764. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
