"""a4order part 2 tracker transaction (re-classify, no C-level change). Writes with newline="" so CRLF/LF stay as found.
New U-9748, U-9749 (newest-first under the table header), a note on U-9746, hooks.csv notes on 00470c70 / 00470670 /
004111c0, one CHANGELOG line under <!-- ENTRIES -->. Asserts every anchor. usage: py -3.12 apply_trackers_p2.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/a4order_20261017/RESULT_a4order.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROWS = [
    "| U-9749 | semantic | `Vehicle/VehicleControl.cpp` / `Integrate2.cpp` (A4 -> A5 -> A6a) on the first frame a car's A4 runs again after a `+0xaec` skip (`A4ContactGate`, `0x0047101b..0x00471048`) | Under `MASHED_A4_ORDER` + d3bB2's `S` stack (" + R + ", `B_SAP_K1` against `o_pw1` / `o_c2r1`, frames aligned on car 2's window) the player matches the original bit for bit through the 9-frame coast after the first car<->car contact (x 0.3035, `+0x9e4` 1237.92 at k = 31), then on the first A4 frame (k = 32) the original's `+0x9e4` drops 23.81 and the port's 6.56; car 2's first A4 frame after its coast (k = 33) gains +26.0 on the original and +34.5 on the port. The player then runs 0.05 left of the original's line (x 0.0946 against 0.1402 at k = 59), so the original's second player<->car 2 contact (both `+0xaec` armed 480 on the aligned frame, o_c2r1 874 / o_pw1 905; car 2 coasts at 1341.140 on k = 61..69) does not happen. | Which A4 / A5 / A6a term differs on the first frame after a skipped run (record state the skip leaves: wheel / torque / timer fields) | Dump the player's record on the original (`o_pw1.msd`) and the port on k = 31..33 and diff the fields A4 / A5 / A6a read; port the difference behind a knob | G-COAST2 of " + R + "; D3 (b) regime B after window call 32 |",
    "| U-9748 | structural | `Vehicle/VehiclePhysicsRun.cpp` A4 block (`g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf` on every `StepCar` call) vs `DAT_007f101c` | The original increments `DAT_007f101c` once per call of `FUN_004111c0` (`inc dword ptr [0x007f101c]` at `0x004111c1`, unconditional at entry); its other writers are `0x0040ff17` (`FUN_004111c0`) and `0x00492784` (`FUN_00492770`), none inside `FUN_00470c70` / `FUN_00470670`, so every car's A4 reads one phase per physics call (`0x00470776`). The port advances the phase once per car step, so with 4 cars each car writes 4 of the 16 slots of its `+0x1ac` / `+0x270` rings (A4 `0x00470841` / `0x00470848`). The port's only reader of the ring is the car-part steer visual (`FUN_0046bda0`, mean of the 16 `+0x1ac` floats x `[0x005cd058]`, `TrackRenderer.cpp`); the original's readers were not enumerated in this lane [UNCERTAIN U-9748]. Solo runs (D2) advance once per frame and are unaffected. `datarefs_7f101c.txt`. | The original's readers of `+0x1ac..+0x1e8` / `+0x270..+0x2ac`; whether `C27-CAR-PART-OFFSET` rows change with one phase per frame | Advance the phase once per race frame (all cars share it) behind a knob; re-run the 13 tracks and read the C27 car-part rows | car-part steer visual with more than one car; `C27-CAR-PART-OFFSET` |",
]


def uncert(s, nl):
    hdr = '| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |' + nl + '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    for u in ('U-9748', 'U-9749'):
        assert ('| ' + u + ' |') not in s, u
    s = s.replace(hdr, hdr + nl.join(ROWS) + nl, 1)
    key = '| U-9746 |'
    assert s.count(key) == 1
    i = s.index(key)
    j = s.index(nl, i)
    note = (' **2026-10-17 (a4order, ' + R + '):** ported behind `MASHED_A4_ORDER` (default OFF; implies `MASHED_AI_TICK_FIRST`): '
            'the AI cars\' gate + A4 run before the player\'s step, their own step runs the substeps only. Car 2 now coasts at '
            '1050.255 on window calls 24..32 (original 1050.409, `S` 1038.302); the second coast at call 61 still does not happen '
            '(U-9749). (b) against the T control: A 1/3 (car 2 passes, car 3 turns to FAIL), B 2/3, C 0/3; D2 3/3 in bounds '
            '(solo recipe, identical to the default); 13 tracks 867 rows 0 RED (two C27 car-part rows move). Residual: a car '
            'eliminated or spun out later in the frame keeps its pre-phase A4 (1 frame in `B_A_K1`). Not flipped (a passing car fails).')
    row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


NOTES = {
    '00470c70': 'a4order 2026-10-17: A4 loop for all cars before the substeps ported behind MASHED_A4_ORDER (default OFF; VehiclePhysics_A4Pre), U-9746',
    '00470670': 'a4order 2026-10-17: reads DAT_007f101c at 0x00470776, one value per physics call in the original; the port advances it per car (U-9748)',
    '004111c0': 'a4order 2026-10-17: inc [0x007f101c] at 0x004111c1, unconditional at entry (the torque ring phase, U-9748)',
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
    line = ('2026-10-17  00470c70  a4order part 2  every car\'s A4 before any substep behind MASHED_A4_ORDER (default OFF, '
            'U-9746), no C-level change. Car 2 coast 1050.255 vs 1050.409; second coast still missing (U-9749); (b) A 1/3 B 2/3 '
            'C 0/3 with A trading car 3 for car 2; D2 3/3; 867 rows 0 RED; knob-off G-CTRL 490c4b2d/1fdd0ea6/46088ac4. New U-9748 '
            '(DAT_007f101c per tick vs per car), U-9749. d3bB2\'s CCET arms ran without MASHED_AI_TICK_FIRST (' + R + ').')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
