"""u9749 tracker transaction (re-classify, no C-level change). Writes with newline="" so CRLF/LF stay as found.
U-9749 RESOLVED (type cell struck, note), notes on U-9743 and U-9745, hooks.csv notes on 0046c7d0 / 00424eb0, one CHANGELOG
line under <!-- ENTRIES -->. Asserts every anchor. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9749_20261017/RESULT_u9749.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def append_note(s, nl, uid, note, retype=None):
    key = '| ' + uid + ' |'
    assert s.count(key) == 1, uid
    i = s.index(key)
    j = s.index(nl, i)
    row = s[i:j]
    assert row.rstrip().endswith('|')
    if retype:
        old, new = retype
        assert row.count(old) == 1, (uid, old)
        row = row.replace(old, new, 1)
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


def uncert(s, nl):
    s = append_note(s, nl, 'U-9749',
        ' **RESOLVED 2026-10-17 (u9749, ' + R + '):** no A4 / A5 / A6a term differs. MSD1 record dumps (`MASHED_RECDUMP`) '
        'against `o_pw1.msd`: at k = 31 the player\'s position and velocity match but its forward vector does not '
        '(`+0x9d4` -0.05019 / -0.07133); the yaw rate `+0x148` is bit-identical and from k = 24 the original turns a constant '
        '0.00264 per frame less, the steer-0 heading assist `FUN_0046c7d0` on car 0 (U-9743). The "-23.81 vs -6.56" read '
        '`+0x9e4`, which the original refreshes after A4 (`0x004710a1..0x004710da`) and the port by default does not; '
        'the velocity vectors agree to 0.3. With `MASHED_HEADING_ASSIST_PLAYER=1` on the `S` + A4-order stack the player\'s '
        'forward x tracks to 5e-5 through k = 36 and the second player<->car 2 contact happens: car 2 coasts 1340.509 on calls 61..69 '
        '(original 1341.140).',
        retype=('| U-9749 | semantic |', '| U-9749 | ~~semantic~~ **RESOLVED 2026-10-17** (u9749: the player form of FUN_0046c7d0, U-9743) |'))
    s = append_note(s, nl, 'U-9743',
        ' **2026-10-17 (u9749, ' + R + '):** ported behind `MASHED_HEADING_ASSIST_PLAYER=1` (mask 0x2, default OFF): '
        '`HeadingAssist_Player` runs `FUN_0046c7d0(0, dt)` before the player\'s step with the player\'s steer bytes. Measured: '
        'forward x on the coast after the first contact tracks the original to 5e-5; (b) A 2/3 (car 2 and car 3 pass), B 2/3, '
        'C 0/3 against the default\'s 1/3, 2/3, 0/3, no passing car turns to FAIL; D2 3/3 in bounds; 13 tracks 866 rows 0 RED '
        '(Training 884 loses its `C27-CAR-PART-OFFSET` row); G-CTRL `a34714b4 / 0f8ef6df / 852a255d`. Default awaits the user.')
    s = append_note(s, nl, 'U-9745',
        ' **2026-10-17 (u9749):** with the second player<->car 2 contact now in the original\'s frame (`S` + A4 order + player '
        'assist), car 2\'s steer during that override drifts (6, 4, 2, then -7 on the other byte) where the original holds 6 on '
        'calls 61..65; the port\'s override timer samples 50 lower than the original\'s on both contacts (1390 / 1440 at calls '
        '23 and 60).')
    return s


NOTES = {
    '0046c7d0': 'u9749 2026-10-17: player (car 0) form ported behind MASHED_HEADING_ASSIST_PLAYER (mask 0x2, default OFF); resolves U-9749, see U-9743',
    '00424eb0': 'u9749 2026-10-17: its FUN_0046c7d0 loop covers car 0; exe runs car 0 under MASHED_HEADING_ASSIST_PLAYER (U-9743)',
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
    line = ('2026-10-17  0046c7d0  u9749  player form of the steer-0 heading assist behind MASHED_HEADING_ASSIST_PLAYER '
            '(default OFF), no C-level change. U-9749 RESOLVED (the cause is the heading, not A4 / A5 / A6a). Knob alone: '
            '(b) A 2/3 B 2/3 C 0/3 (default 1/3, 2/3, 0/3), D2 3/3, 866 rows 0 RED, G-CTRL a34714b4/0f8ef6df/852a255d; '
            'knob off 490c4b2d/1fdd0ea6/46088ac4. New diagnostic MASHED_RECDUMP (MSD1 record dumps). ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
