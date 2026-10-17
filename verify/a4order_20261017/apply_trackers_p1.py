"""a4order part 1 tracker transaction (re-classify, no C-level change). Writes with newline="" so CRLF/LF stay as found.
A note on U-9744 (MASHED_AI_COUNTDOWN_CLOCK default ON by user decision), a hooks.csv note on 0040fc00, one CHANGELOG
line under <!-- ENTRIES -->. Asserts every anchor exactly once. usage: py -3.12 apply_trackers_p1.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF = '490c4b2d / 1fdd0ea6 / 46088ac4'
OPT = '4c81db89 / aa03f7ae / 7d3af534'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9744 |'
    assert s.count(key) == 1
    i = s.index(key)
    j = s.index(nl, i)
    note = (' **2026-10-17 (a4order part 1):** default ON by user decision (regime A trades car 2 for car 3, A stays 1/3); '
            '`MASHED_NO_AI_COUNTDOWN_CLOCK=1` opts out. G-CTRL default `' + DEF + '` x2, opt-out `' + OPT + '` (= dflip).')
    row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


NOTES = {
    '0040fc00': 'a4order 2026-10-17: exe countdown clock add (0x0040fe5e) default ON by user decision; MASHED_NO_AI_COUNTDOWN_CLOCK=1 opts out (U-9744)',
}


def hooks(s, nl):
    lines = s.split(nl)
    hdr = next(csv.reader([lines[0]]))
    ni = hdr.index('notes')
    done = set()
    for k, ln in enumerate(lines[1:], 1):
        rva = ln.split(',', 1)[0]
        if rva in NOTES:
            r = next(csv.reader([ln]))
            assert len(r) == len(hdr), (rva, len(r))
            r[ni] = (r[ni] + ' | ' if r[ni] else '') + NOTES[rva]
            out = io.StringIO(newline='')
            csv.writer(out, lineterminator='').writerow(r)
            lines[k] = out.getvalue()
            done.add(rva)
    assert done == set(NOTES), set(NOTES) - done
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-17  0040fc00  a4order part 1  MASHED_AI_COUNTDOWN_CLOCK default ON (user decision; opt-out '
            'MASHED_NO_AI_COUNTDOWN_CLOCK=1), no C-level change. G-CTRL default ' + DEF.replace(' ', '') + ' x2, opt-out '
            + OPT.replace(' ', '') + '; d3bB2 arm CC: 13 tracks 867 rows 0 RED, D2 3/3, (b) A 1/3 B 2/3 C 0/3 (U-9744).')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
