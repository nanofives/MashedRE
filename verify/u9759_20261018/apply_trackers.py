"""u9759 tracker transaction (re-classify, no C-level change): U-9759 RESOLVED, new U-9760, hooks.csv notes on 004c3d90,
004dc370 and 0046c5f0, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9759_20261018/RESULT_u9759.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9760 | semantic | car 1\'s call-1 velocity `+0x9b0`, omega `+0x9c0` and wheel force words `+0x214 / +0x21c` | With the '
       'spawn pose, spawn normal, spawn basis, `_DAT_005cea60`, the `+0x968` half, the forward `+0x9d4`, the wheel points and '
       'the up vector all the original\'s (u9759 `UTBQ1SSE`, plus `MASHED_RING_DBUF` / `MASHED_FWD_AT` for the half and the '
       'forward), car 1\'s record still differs at call 1: `+0x9b0` -0.00108456612 / -0.00108465552, `+0x9c0`, `+0x214` '
       '-2.8865118 / -2.88652229, `+0x21c` -2385.21582 / -2385.22363; drift 1.202e-4 at call 21. Leads: (1) the A4 torque ring '
       '`rec + 0x1ac / 0x270 + (DAT_007f101c & 0xf)*4` (`pc 0x00470848 / 0x0047090d`, watch o_w1d4) holds the call-1 value at '
       'index 13 in the original and 10 in the port (`g_torqueRingPhase`, `Vehicle/VehicleControl.cpp:158-159`); its consumer '
       'is not identified. (2) never set by the port at call 0: `+0x044 / +0x048` (-1 / 0) and `+0x13c / +0x140` (-0.16622 / '
       '0.03475 vs 0). ' + R + ' | Which term of the call-1 force words differs, and whether the ring phase or the call-0 '
       'constants feed it | Write-watch car 1 `+0x214` on the original (return chain = the writer), find the consumer of the '
       '`+0x1ac` ring, then compare that term with the port | D3 (b) car-level parity before the first contacts |')
N9759 = (' **RESOLVED 2026-10-18 (u9759, ' + R + '):** the writer is the tri-normal `FUN_0046c5f0` (watch o_up1), whose port '
         '(`MASHED_UP_TRINORMAL`) is exact on the original\'s points. The points differed: the live RW slot `+0xc` is the SSE '
         '`0x004dc370`, `(z*at + y*up) + (x*right + pos)`, not the installer\'s `0x004c3670` (live slots read in game, o_bq3). '
         '`MASHED_RW_SSE_POINTS` (default OFF) makes the points exact; with `MASHED_UP_TRINORMAL` too, `+0x9cc` is exact at '
         'calls 0 and 1. Drift unchanged (not the driver). Default candidate: (b) pass set kept, D2 3/3, 13 tracks 0 RED, '
         'G-CTRL `bb05716c / 47b28f48 / ac62cd14`. Next driver U-9760.')
NOTES = {
    '004c3d90': 'u9759 2026-10-18: the slot +0xc it dispatches is live 0x004dc370 (SSE), not the installer default 0x004c3670 (0x004c3ebf); read in game: slots +8/+0xc/+0x10/+0x14 = 0x004dc4a0/0x004dc370/0x004dc530/0x004dc410',
    '004dc370': 'u9759 2026-10-18: live RW points transform (slot +0xc); per component (z*m[8+c] + y*m[4+c]) + (x*m[c] + m[12+c]) at 0x004dc3d2..0x004dc3e7; ported as MASHED_RW_SSE_POINTS (default OFF)',
    '0046c5f0': 'u9759 2026-10-18: sole writer of record +0x9c8..+0x9d0 (pc 0x0046c6a8/0x0046c6b1, watch o_up1); MASHED_UP_TRINORMAL is exact on the original points',
}


def uncert(s, nl):
    key = '| U-9759 | semantic |'
    assert s.count(key) == 1 and '| U-9760 |' not in s
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    row = row.replace(key, '| U-9759 | ~~semantic~~ **RESOLVED 2026-10-18** (u9759: SSE points transform 0x004dc370) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9759 + ' |'
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
    line = ('2026-10-18  004dc370  u9759  U-9759 RESOLVED: the live RW points transform (slot +0xc) is the SSE 0x004dc370, '
            'not 0x004c3670; MASHED_RW_SSE_POINTS (default OFF) makes car 1\'s wheel points and (with MASHED_UP_TRINORMAL) the up '
            'vector exact; drift unchanged; new U-9760 (call-1 velocity / force words, torque ring phase). SSE candidate: (b) '
            'pass set kept, D2 3/3, 13 tracks 0 RED, G-CTRL bb05716c/47b28f48/ac62cd14. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
