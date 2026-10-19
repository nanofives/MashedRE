"""u9760 amendments 1-3 tracker transaction (re-classify, no C-level change): U-9760 RESOLVED, new U-9761, hooks.csv note on
00467650, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers2.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9760_20261019/RESULT_u9760.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9761 | semantic | `Vehicle/Integrate2.cpp` (A6a, `FUN_00467650`): `MASHED_A6A_LUT` against D2, and car 1\'s call-1 omega '
       '`+0x9c0` | `MASHED_A6A_LUT` (A6a\'s nine lengths as the original\'s LUT `FUN_004c3ac0`) makes car 1\'s call-1 force words and '
       'velocity exact and brings (b) regime B to 3/3, but on the current default (`c43ebeba1`) the player\'s D2 slip1500 reads '
       '0.1877, below the band 0.18855-0.19635 (original 0.1943); on the pre-flip default it was 0.1904. Separately, car 1\'s '
       'first differing field with the knob is omega `+0x9c0` at call 1 (-8.73517183e-06 / -8.73517274e-06); the port\'s A6a '
       'torque chain (`l_60`, `l_d0`, `m15`, `m16`, `r`) is `double` (U-A6A-FLOAT10) where the original runs x87 at PC_24. ' + R +
       ' section 5 | Which remaining port-side term moves slip1500 once the lengths are faithful (a compensating value elsewhere, '
       'or the torque chain\'s precision), and whether the float form of the torque chain fixes the call-1 omega | Port the '
       'torque chain at float precision behind a knob, measure the call-1 omega and D2 slip1500 with and without '
       '`MASHED_A6A_LUT` | D3 (b) regime B 3/3; D2 bounds |')
N9760 = (' **RESOLVED 2026-10-19 (u9760 amendments 1-3, ' + R + ' section 5):** A5 is exact (new diagnostic `MASHED_FORCE_SPLIT`); '
         'the gap was A6a\'s addend, from its lengths: the original takes all nine with the LUT `FUN_004c3ac0` (`0x0046766e` .. '
         '`0x004686a4`), the port with exact sqrt. `MASHED_A6A_LUT` (default OFF) makes the addend, the call-1 force words and '
         'the call-1 velocity exact; (b) A 2/3 B 3/3 C 0/3; D2 slip1500 0.1877 OUT on the current default, so not a default '
         'candidate (U-9761).')
NOTES = {
    '00467650': 'u9760 2026-10-19: all nine lengths are FUN_004c3ac0 (LUT) at 0x0046766e/0x00467680/0x004680f6/0x0046820a/0x0046833e/0x004684bb/0x004684d7/0x004685b7/0x004686a4; ported as MASHED_A6A_LUT (default OFF); the wheel-force addend 0x00468320..0x00468337',
}


def uncert(s, nl):
    key = '| U-9760 | semantic |'
    assert s.count(key) == 1 and '| U-9761 |' not in s
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    row = row.replace(key, '| U-9760 | ~~semantic~~ **RESOLVED 2026-10-19** (u9760: A6a lengths, FUN_004c3ac0) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9760 + ' |'
    return s[:i] + ROW + nl + row + s[j:]


def hooks(s, nl):
    lines = s.split(nl)
    hdr = next(csv.reader([lines[0]])); ni = hdr.index('notes'); done = set()
    for k, ln in enumerate(lines[1:], 1):
        rva = ln.split(',', 1)[0]
        if rva in NOTES:
            r = next(csv.reader([ln])); assert len(r) == len(hdr), rva
            r[ni] = (r[ni] + ' | ' if r[ni] else '') + NOTES[rva]
            out = io.StringIO(newline=''); csv.writer(out, lineterminator='').writerow(r); lines[k] = out.getvalue(); done.add(rva)
    assert done == set(NOTES), set(NOTES) - done
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00467650  u9760  U-9760 RESOLVED: A5 exact, A6a\'s nine lengths are the LUT FUN_004c3ac0; MASHED_A6A_LUT '
            '(default OFF) makes car 1\'s call-1 force words and velocity exact, (b) B 3/3, but D2 slip1500 0.1877 OUT on the '
            'current default (new U-9761, with the call-1 omega lead). Diagnostic MASHED_FORCE_SPLIT added. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
