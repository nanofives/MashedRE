"""u9745ov tracker transaction (re-classify, no C-level change). New U-9750, U-9751 (newest-first under the table header),
a note on U-9745, hooks.csv notes on 0040fc00 / 004177b0 / 0046ddb0, one CHANGELOG line. Writes with newline="" so CRLF/LF
stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9745ov_20261017/RESULT_u9745ov.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROWS = [
    "| U-9751 | structural | `Vehicle/ForceIntegrator.cpp:141` (A5 `FUN_0046ddb0` phase 3) reads `g_raceTimer` (`ForceIntegratorStubs.cpp:23`, initialised 0, no writer in the exe) | The original's A5 reads `DAT_007f0ff8` (`FILD [0x007f0ff8]` at `0x0046e0a9`) for the grip ramp: `local_6c` = 1 while `60000 - t >= 0` (`_DAT_005cea7c` 60000 = 20 s of 1/3000 units), then `1 + (60000 - t) * (-1/30000)` (`_DAT_005cea74`, floor `_DAT_005cea78` -30000), i.e. it falls from 1 at 20 s to 0 at 30 s and feeds the contact / rubber-band grip terms (`ForceIntegrator.cpp:141..169`). In the exe the timer reads 0 forever, so `local_6c` stays 1 for the whole race. Found in " + R + " (the `DAT_007f0ff8` reader list); not measured. | The original's `local_6c` and the grip terms after 20 s on a race capture; which consumers change with it | Point `g_raceTimer` at `DAT_007f0ff8` (with U-9750's add order) behind a knob; score D2 (90 s runs reach the ramp) and D3 (b) | A5 after 20 s of race time, every car |",
    "| U-9750 | structural | `Ai/AiStandalone.cpp` `Ai_AdvanceClock` (adds the budget to `DAT_007f0ff4` / `DAT_007f0ff8` before the AI tick); fix behind `MASHED_AI_CLOCK_LATE` (default OFF) | `FUN_0040fc00` stores `DAT_007f1008` (`0x0040fc63`), calls `FUN_00425a40` (`0x0040fc6c`: AI tick, every car's physics, the event drain) and the rest of the frame, and adds the budget to `DAT_007f0ff4` / `DAT_007f0ff8` only at its end (`0x0040fe4f..0x0040fe64`, RET `0x0040fe6c`), so every reader in a frame sees the value before that frame's add. The port read one budget more. Measured: the `FUN_004177b0` band-0 roll (`0x00417b2f`, prob 20 for row 2 from `0x005f30a0`) fired at window call 60 on the port and 61 on the original (o_reg_r13 / r15 / r18 / r19 / r20); under the knob 61, and car 2's commands equal o_reg_r13's on calls 57..92 (S + A4-order stack). Knob alone: (b) A 2/3 B 2/3 C 0/3 (same pass set as the default), D2 3/3, 13 tracks 866 rows 0 RED (identical score file), G-CTRL `dd822489 / 35f5fea0 / da12bbe0`. | None from the bytes; other `DAT_007f0ff8` readers (`FUN_00416250` `0x004168d2`, `FUN_00416a30`, `FUN_00417180`, `FUN_00417da0`, `FUN_00422ba0`, `FUN_004252c0`) move with it | User decision on the default | the knob's default; D3 (b) |",
]


def uncert(s, nl):
    hdr = '| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |' + nl + '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    for u in ('U-9750', 'U-9751'):
        assert ('| ' + u + ' |') not in s, u
    s = s.replace(hdr, hdr + nl.join(ROWS) + nl, 1)
    key = '| U-9745 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    note = (' **2026-10-17 (u9745ov, ' + R + '):** the u9749 residual (car 2 steering during the second override) was two things: '
            'the reference `o_c2r1` lost the one-shot `FUN_004177b0` band-0 roll while the port\'s K1 wins it (as o_reg_r13 does), and '
            'the port\'s roll fired one call early (U-9750). Against o_reg_r13 with `MASHED_AI_CLOCK_LATE` car 2\'s commands are equal '
            'on calls 57..92; the override replay itself (`0x004187d5..0x004187f0`) is not involved (`FUN_00416250` does not read '
            'the timer). The port\'s step dump reads the override timer 50 lower than the original\'s probe on every override call '
            '(1390 / 1440 on the first) while the commands are equal; where the port\'s dump reads it relative to the '
            '0x004187e8 decrement was not checked.')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


NOTES = {
    '0040fc00': 'u9745ov 2026-10-17: the DAT_007f0ff4/0ff8 add is at the end (0x0040fe4f..0x0040fe64), after FUN_00425a40 (0x0040fc6c); exe order behind MASHED_AI_CLOCK_LATE (U-9750)',
    '004177b0': 'u9745ov 2026-10-17: band-0 roll reads DAT_007f0ff8 at 0x00417b2f before that frame\'s add; fires at window call 61 on the original (U-9750)',
    '0046ddb0': 'u9745ov 2026-10-17: reads DAT_007f0ff8 at 0x0046e0a9 (grip ramp 60000 / -30000 / -1/30000); exe reads the stub g_raceTimer = 0 (U-9751)',
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
    line = ('2026-10-17  0040fc00  u9745ov  DAT_007f0ff4/0ff8 add moved to the end of the frame behind MASHED_AI_CLOCK_LATE '
            '(default OFF), no C-level change. The FUN_004177b0 roll now fires at call 61 as on the original; car 2 equals '
            'o_reg_r13 on calls 57..92; (b) unchanged pass set, D2 3/3, 866 rows 0 RED. New U-9750 (clock order), U-9751 '
            '(A5 grip ramp reads a stub race timer). ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
