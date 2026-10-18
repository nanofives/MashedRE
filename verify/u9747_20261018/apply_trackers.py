"""u9747 tracker transaction (re-classify, no C-level change): U-9747 RESOLVED, hooks.csv notes on 00418860 / 004704c0, one
CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py <G-CTRL trio> <13-track summary>"""
import csv, io, os, sys

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9747_20261018/RESULT_u9747.md'
CTRL, TRACKS = sys.argv[1], sys.argv[2]


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def uncert(s, nl):
    key = '| U-9747 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    old = '| U-9747 | structural |'
    assert row.count(old) == 1
    row = row.replace(old, '| U-9747 | ~~structural~~ **RESOLVED 2026-10-18** (u9747: slot state + spawn pose on countdown frames) |', 1)
    note = (' **2026-10-18 (u9747, ' + R + '):** the stated cause was wrong: the alive word `FUN_0046c7b0` is 1 for every car '
            'through the countdown (the port\'s A3 writes `+0x4 = 1`, as `0x0046b6fc`). The failing gate is `IsCarSlotActive` '
            '(`0x0040e370`, slot state), set to 2 by `FUN_00418860`\'s alive-poke (`0x004188ae..0x004188db`), which the original '
            'runs on its pre-GO ticks and the port first ran on GO; and every record\'s pose was (0,0,0) until the car\'s first '
            'step (the original\'s `FUN_004704c0` writes both state matrices at race start, `0x004705fe` / `0x0047060a` / '
            '`0x0047061c` / `0x0047062e`). Ported behind `MASHED_COUNTDOWN_SPAWN=1` (default OFF): AI progress on every countdown '
            'frame 29.7277 / 29.4549 / 29.1828 (original 29.73 / 29.457 / 29.18), and under `S` car 2\'s `FUN_00414a70` returns 0 '
            'at window call 1 as on the original (G-ORDER). Alone: (b) A 2/3 B 2/3 C 0/3 (the default\'s pass set), D2 3/3, '
            '13 tracks ' + TRACKS + ', G-CTRL `' + CTRL + '`.')
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


NOTES = {
    '00418860': 'u9747 2026-10-18: alive-poke 0x004188ae..0x004188db (slot state 2) also runs on pre-GO ticks; exe countdown copy behind MASHED_COUNTDOWN_SPAWN (U-9747)',
    '004704c0': 'u9747 2026-10-18: both state matrices written at race start (0x004705fe/0x0047060a/0x0047061c/0x0047062e); exe VehiclePhysics_SeedSpawnPose behind MASHED_COUNTDOWN_SPAWN (U-9747)',
}


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
    line = ('2026-10-18  00418860  u9747  U-9747 RESOLVED: countdown slot state (FUN_00418860 alive-poke) + spawn pose '
            '(FUN_004704c0 matrices) behind MASHED_COUNTDOWN_SPAWN (default OFF); AI progress at GO = the original\'s; G-ORDER '
            'passes under S. Alone: (b) unchanged pass set, D2 3/3, 13 tracks ' + TRACKS + '. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
