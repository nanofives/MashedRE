"""cev tracker transaction (re-classify, no C-level change): U-9745 and U-9684 notes, hooks.csv notes on 00408610 /
00414a70, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/cev_20261018/RESULT_cev.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def append_note(s, nl, key, note):
    assert s.count(key) == 1, key
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


N9745 = (' **2026-10-18 (cev, ' + R + '):** the open question is answered: the original has AI<->AI contacts in the (b) '
         'windows at fixed calls (override-timer onsets, both AI timers on one frame): A (13/13) cars 2+3 at 205, B (3/3) '
         'cars 1+3 at 159, C (2/2) none; car 2\'s player contacts at 23 and 60 in all. The port\'s first extra contact under '
         'the knob (cars 1+3 at call 63, every regime) is car 3\'s missing `FUN_00414a70` brake chain (U-9684). Under the '
         'ported chain (`S`) the AI cars\' progress `0x008a96e8` was one tick stale (`PathProg_FrameTick` ran before the '
         'opponents\' physics; the original runs `FUN_00425a40` `0x0040fc6c` before `FUN_0040da50` `0x0040fc75`); '
         '`MASHED_PATHPROG_LATE` (default OFF) fixes it: `FUN_00414a70` returns equal the original\'s through call 59, the '
         'call-63 contact is gone, C\'s schedule is the original\'s, B has 1+3 at 169 plus 2+3 at 189, A misses 2+3 at 205; '
         '(b) `S` + knob 0/3 x3 (car 1\'s lookahead node flip at call 21 on a 0.3 speed difference; car 2\'s post-contact '
         'progress, U-9746). The event knob alone on today\'s default: A 3/3, B 2/3, C 0/3, with the call-63 contact.')
N9684 = (' **2026-10-18 (cev, ' + R + '):** car 3\'s brake chain is exact under `S` + `MASHED_PATHPROG_LATE` (return 1 '
         'on 39..41, 2 on 44..57, as o_reg_r13); `S` alone ran it one call late because the AI cars\' progress was one '
         'tick stale (port call k = original call k-1, o_wobj2). First `FUN_00414a70` divergence moves to calls 134..190.')

NOTES = {
    '00408610': 'cev 2026-10-18: the per-frame loop (FUN_0040da50 0x0040fc75) runs after the physics frame FUN_00425a40 (0x0040fc6c); exe UpdateRace site left the AI rows one tick stale; MASHED_PATHPROG_LATE (default OFF) moves it (U-9745)',
    '00414a70': 'cev 2026-10-18: returns equal the original\'s through window call 59 for cars 1..3 under S + MASHED_PATHPROG_LATE (o_reg_r13 / o_wobj2); car 3 brake chain exact (U-9684)',
}


def uncert(s, nl):
    s = append_note(s, nl, '| U-9745 |', N9745)
    return append_note(s, nl, '| U-9684 |', N9684)


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
    line = ('2026-10-18  00408610  cev  U-9745: the original\'s AI<->AI contact schedule per regime (A 2+3@205, B 1+3@159, C '
            'none); the port\'s extra 1+3@63 = car 3\'s missing FUN_00414a70 brake chain; MASHED_PATHPROG_LATE (default OFF) '
            'removes a one-tick progress lag under S, FUN_00414a70 exact through call 59; (b) S+knob 0/3 x3, no default change. '
            'No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
