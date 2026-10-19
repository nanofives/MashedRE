"""u9758 tracker transaction (re-classify, no C-level change): U-9758 RESOLVED, new U-9759, hooks.csv note on 0046ddb0, one
CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9758_20261018/RESULT_u9758.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9759 | semantic | the body up vector record `+0x9c8..+0x9d0` (`Vehicle/VehiclePhysicsRun.cpp` seeds it with the basis up '
       'row; `MASHED_UP_TRINORMAL` writes the original\'s tri-normal `FUN_0046f7b0` in `Collision/WheelContactSolver.cpp`) | With '
       'the spawn pose, spawn normal, spawn basis, `_DAT_005cea60`, the `+0x968` half and the forward `+0x9d4` all the '
       'original\'s, car 1\'s only call-0 float difference against o_wobj2 is `+0x9cc`: 0.999988973 original, 1 port, '
       '0.999989033 under `MASHED_UP_TRINORMAL`. At call 1 the velocity `+0x9b0` (-0.00108456612 / -0.00108465552) and omega '
       '`+0x9c0` differ; car 1\'s drift is 1.202e-4 at call 21. ' + R + ' | Which input of the tri-normal differs (the three '
       'transformed suspension points `DAT_0088e620 / 38 / 44`, or the `FUN_004c3b30` LUT magnitude), and whether the up '
       'vector is what moves the call-1 velocity | Write-watch car 1 `+0x9cc` and `DAT_0088e624` on the original at the first '
       'racing frame, compare with the port under `MASHED_UP_TRINORMAL`, then re-run `posdrift.py` | D3 (b) car-level parity '
       'before the first contacts |')
N9758 = (' **RESOLVED 2026-10-18 (u9758, ' + R + '):** the writer is `FUN_004c3df0(rec + 0x9d4, &DAT_00614708 = (0, 0, 1), 1, '
         'matrix)`, i.e. the matrix\'s `at` row, from A5 `0x0046ddc9` (A4\'s `[+0x9a8]` half, before the substeps) and '
         '`FUN_0046d510` `0x0046d53a` (`rec + 0x928`; AI callers before physics, `FUN_004252c0` `0x0042536f` after it); none in '
         'the substep loop (write watch o_fw1). `MASHED_FWD_AT` (default OFF) ports it: the field is exact at call 0, the drift '
         'is unchanged (not the driver). Default candidate: (b) pass set kept, D2 3/3, 13 tracks 0 RED, G-CTRL '
         '`ded760d5 / fa7a6390 / 27a97e09`. Next driver the up vector (U-9759).')
NOTES = {
    '0046ddb0': 'u9758 2026-10-18: 0x0046ddb0..0x0046ddc9 FUN_004c3df0(rec+0x9d4, &DAT_00614708=(0,0,1), 1, arg2 = A4 [+0x9a8] half): record forward = the matrix at row; also FUN_0046d510 (rec+0x928); ported as MASHED_FWD_AT (default OFF)',
}


def uncert(s, nl):
    key = '| U-9758 | semantic |'
    assert s.count(key) == 1 and '| U-9759 |' not in s
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    row = row.replace(key, '| U-9758 | ~~semantic~~ **RESOLVED 2026-10-18** (u9758: the at row, FUN_004c3df0) |', 1)
    row = row.rstrip()[:-1].rstrip() + N9758 + ' |'
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
    line = ('2026-10-18  0046ddb0  u9758  U-9758 RESOLVED: record +0x9d4 is the matrix at row (FUN_004c3df0 with (0,0,1) from A5 '
            '0x0046ddc9 and FUN_0046d510); MASHED_FWD_AT (default OFF) makes it exact at call 0; car 1 drift unchanged; new U-9759 '
            '(up vector +0x9cc). FA candidate: (b) pass set kept, D2 3/3, 13 tracks 0 RED, G-CTRL ded760d5/fa7a6390/27a97e09. '
            'No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
