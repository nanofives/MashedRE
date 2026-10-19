"""u9760 tracker transaction (re-classify, no C-level change): U-9760 note (stays open), hooks.csv note on 0046ddb0, one
CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9760_20261019/RESULT_u9760.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def append(s, nl, key, note):
    assert s.count(key) == 1, key
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


N9760 = (' **2026-10-19 (u9760, ' + R + ', partial):** the call-1 force words are A5 plus an A6a addend (write watch o_f214): '
         'A5\'s all-grounded block writes the projected `x -13.935943603515625 / z -11923.7607421875` (`pc 0x0046e91a / '
         '0x0046e94f`), A6a adds a speed-scaled term (`0x00468320..0x00468337`, `+0x9e4` at `0x004682d7`) to `-2.88651180 / '
         '-2385.21582`. The port\'s A5 block uses `std::sqrt` and `double` where the original uses `FUN_004c3ac0` (LUT) and '
         'float steps with another dot order; `MASHED_A5_SUSP_LUT` (default OFF) ports it, (b) pass set kept, but car 1\'s '
         'dumped force words at call 1 do not move, so A5 / A6a shares are not split yet. Next: a port-side log of A5\'s '
         'projected force and A6a\'s addend against o_f214. Also on the full knob stack: `+0xb3c` -1.00002646 / +1 at call 0.')
NOTES = {
    '0046ddb0': 'u9760 2026-10-19: all-grounded suspension force 0x0046e885..0x0046e925 = (S*load)*0.5 * (n.x, n.y-1, n.z), len = FUN_004c3ac0 (LUT), float steps, dot (a.y*ay + a.x*ax) + a.z*az; ported as MASHED_A5_SUSP_LUT (default OFF)',
}


def uncert(s, nl):
    return append(s, nl, '| U-9760 |', N9760)


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
    line = ('2026-10-19  0046ddb0  u9760  U-9760 note (open): call-1 force words = A5 projected + A6a speed-scaled addend '
            '(watch o_f214); MASHED_A5_SUSP_LUT (default OFF) ports A5\'s LUT/float arithmetic, (b) pass set kept, force words '
            'at call 1 unchanged; next a port-side A5/A6a log. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
