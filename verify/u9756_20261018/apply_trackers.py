"""u9756 tracker transaction (re-classify, no C-level change): U-9756 RESOLVED, U-9152 note, new U-9757, hooks.csv note on
0046f6c0, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9756_20261018/RESULT_u9756.md'


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


ROW = ('| U-9757 | semantic | `Collision/CarWorldContacts.cpp` (`kTer_VelScale2`, `ContactConstants.h:43`) | `_DAT_005cea44` is '
       '0.27777779f (`0x3e8e38e4`) in the anchored binary; the port held 0.277779f. `MASHED_FIX_5CEA44` (default OFF) uses the '
       'binary\'s value: (b) unchanged (A 2/3 B 2/3 C 0/3) but D2 leaves its bounds (slip 2000-2600 0.2549 > 0.25487, '
       'driving-median 1880.00 < 1904.70, H1 398/400) and the G-CTRL player trace moves (`dd428bdb`). ' + R + ' | Why the '
       'faithful constant moves the player out of the D2 band (a compensating port-side value elsewhere in the car-vs-world '
       'chain, or a band set on the wrong constant) | Trace the D2 run\'s car-vs-world contact responses with and without the '
       'knob against an original capture of the same recipe | D2 bounds; default fidelity of the car-vs-world response |')
N9756 = (' **RESOLVED 2026-10-18 (u9756, ' + R + '):** a mistranscribed constant: `_DAT_005cea60` is 0.2778f (`0x3e8e3bcd`), the '
         'port\'s `kWTorque` / `kProbeScale` held 0.277779f, which moved the wheel contact points 1.26e-5 (o_sp2) and the depth '
         '7e-7. With `MASHED_FIX_5CEA60` (default OFF) the spawn pass (contact x, depth, y 0.468762934) and car 1\'s call-0 y '
         '(0.468502462) equal the original\'s. The drift is unchanged; next driver the `+0x968` half (U-9152). Default-ON '
         'candidate: (b) dumps unchanged, D2 3/3, 13 tracks 0 RED, G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`.')
N9152 = (' **2026-10-18 (u9756, ' + R + '):** with the spawn pose, spawn normal, spawn basis and `_DAT_005cea60` all '
         'bit-exact, the first record fields that differ for car 1 are this half: `+0x99c` 0.468632549 (original, one substep '
         'behind) / 0.468502462 (port, current pose) at window call 0, `+0x970` 2.17e-5 / 4.33e-5 at call 1; the solver reads '
         'the `+0x9ac` half (= 1), so its contacts at call 1 come from a different pose (force words `+0x214` / `+0x21c`). '
         'This half is now the next driver of car 1\'s drift and its call-21 lookahead flip.')
NOTES = {
    '0046f6c0': 'u9756 2026-10-18: _DAT_005cea60 = 0.2778f (0x3e8e3bcd), the exe held 0.277779f (kWTorque, kProbeScale); MASHED_FIX_5CEA60 (default OFF) makes the spawn pass bit-exact (o_sp1/o_sp2); the hold-tick order is FUN_0046baa0 + FUN_00470c70 per tick',
}


def uncert(s, nl):
    old = '| U-9756 | semantic |'
    i = s.index('| U-9756 |')
    j = s.index(nl, i)
    row = s[i:j]
    assert row.startswith(old) and '| U-9757 |' not in s
    row = row.replace(old, '| U-9756 | ~~semantic~~ **RESOLVED 2026-10-18** (u9756: _DAT_005cea60 transcription) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9756 + ' |'
    s = s[:i] + ROW + nl + row + s[j:]
    return append(s, nl, '| U-9152 |', N9152)


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
    line = ('2026-10-18  0046f6c0  u9756  U-9756 RESOLVED: _DAT_005cea60 = 0.2778f, the exe held 0.277779f; MASHED_FIX_5CEA60 '
            '(default OFF) makes the spawn solver pass and car 1\'s call-0 y bit-exact; drift unchanged (next: the +0x968 half, '
            'U-9152 note). _DAT_005cea44 likewise mistranscribed; its fix moves D2 out of band (new U-9757). F60 candidate: (b) '
            'unchanged, D2 3/3, 13 tracks 0 RED. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
