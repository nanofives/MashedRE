"""u9749 flip transaction (re-classify, no C-level change): U-9743 RESOLVED (player form of FUN_0046c7d0 default ON by user
decision), hooks.csv note on 0046c7d0, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found."""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF = 'a34714b4 / 0f8ef6df / 852a255d'
OPT = '490c4b2d / 1fdd0ea6 / 46088ac4'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9743 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i)
    row = s[i:j]
    old = '| U-9743 | structural |'
    assert row.count(old) == 1
    row = row.replace(old, '| U-9743 | ~~structural~~ **RESOLVED 2026-10-17** (u9749: player form ported, default ON by user decision) |', 1)
    note = (' **2026-10-17 (u9749 flip):** mask 0x2 default ON by user decision; `MASHED_NO_HEADING_ASSIST_PLAYER=1` opts out. '
            'G-CTRL default `' + DEF + '` x2 (exe `2ca9a02b`), opt-out `' + OPT + '`.')
    assert row.rstrip().endswith('|')
    row = row.rstrip()[:-1].rstrip() + note + ' |'
    return s[:i] + row + s[j:]


def hooks(s, nl):
    lines = s.split(nl)
    hdr = next(csv.reader([lines[0]])); ni = hdr.index('notes'); done = False
    for k, ln in enumerate(lines[1:], 1):
        if ln.split(',', 1)[0] == '0046c7d0':
            r = next(csv.reader([ln])); assert len(r) == len(hdr)
            r[ni] = r[ni] + ' | u9749 flip 2026-10-17: car 0 form default ON (MASHED_NO_HEADING_ASSIST_PLAYER=1 opts out), U-9743 resolved'
            out = io.StringIO(newline=''); csv.writer(out, lineterminator='').writerow(r); lines[k] = out.getvalue(); done = True
    assert done
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  0046c7d0  u9749 flip  player form of the steer-0 heading assist default ON (user decision; opt-out '
            'MASHED_NO_HEADING_ASSIST_PLAYER=1), no C-level change, U-9743 RESOLVED. G-CTRL default ' + DEF.replace(' ', '') +
            ' x2, opt-out ' + OPT.replace(' ', '') + '; (b) A 2/3 B 2/3 C 0/3, D2 3/3, 866 rows 0 RED (verify/u9749_20261017/RESULT_u9749.md).')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
