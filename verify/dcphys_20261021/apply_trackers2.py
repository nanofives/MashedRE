"""dcphys amendment 1 (dotflip) tracker transaction (re-classify): MASHED_A6A_DOT_ORDER default ON (Mariano's decision);
A6a 0x00467650 C2 -> C4 on the default body; U-9784 filed (the standalone's game-mode word). Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcphys_20261021/RESULT_dcphys.md'
D = '2026-10-22'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


A6A_NOTE = ("dotflip " + D + ": MASHED_A6A_DOT_ORDER default ON (Mariano's decision; FaithDefaults.cpp, and the .asi seed; "
            "opt-out MASHED_NO_A6A_DOT_ORDER=1). C4 on the DEFAULT shared body: hook installed live (A6a_Entry inline-JMP "
            "@0x00467650), canonical race, in-process A/B against the live original over the full 0xd04 record, .asi "
            "default with no env 1440/1440 bit-identical (plus 6551/6551 knob-ON, dcphys), no-op control 0 on every call; "
            "the opt-out reproduces the 174/1440 residual on the same race. Exe new default G-CTRL 23ac657a / 7bf2f27f / "
            "3186dd09 x2, B 0e3790c4 / 55b84dd8 / 2324715b, (b) 9/9, (e) 3/3, D2 3/3. Input caveat U-9784 (the exe's "
            "game-mode word is a stand-in). Airborne branch not sampled. " + R + " | ")


def hooks(s, nl):
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    hit = 0
    for li, line in enumerate(lines[1:], 1):
        if not line.startswith('00467650,'):
            continue
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == 'C2'
        assert r[ix['file']] == r[ix['exe_file']] == 'mashedmod/src/mashed_re/Vehicle/Integrate2.cpp'
        r[ix['confidence']] = 'C4'
        r[ix['notes']] = A6A_NOTE + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit += 1
    assert hit == 1
    return nl.join(lines)


def uncertainties(s, nl):
    assert '| U-9784 |' not in s
    hdr = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    row = ("| U-9784 | structural | the standalone's game-mode word `Fi_GameMode()` (`Vehicle/ForceIntegratorStubs.cpp:124`), "
           "read by A6a `0x00467650` (through `A6GameMode`), A4, `BodyOrientationIntegrate.cpp` and A5 | The original's "
           "`FUN_0040e350` returns the live `DAT_0063ba8c`; the dcphys A/B logs show it at 3, 4, 5 and 6 within one race "
           "(`gm=` field). The standalone returns 6 on every call except the d2engage pre-release hold tick (5, "
           "`VehiclePhysicsRun.cpp:1292`); `TrackRenderer.cpp:6948` states the standalone has no `DAT_0063ba8c` state "
           "machine. A6a branches on it at `0x00467cfe` (`== 6`, the `+0xbf4` decrement) and at its mode-7 tests. The .asi "
           "reads the live word, which is what A6a's C4 A/B measured. | whether the standalone runs A6a / A4 / A5 while the "
           "original's word is below 6, and with which value; a per-tick trace of `FUN_0040e350` on the original next to "
           "`Fi_GameMode()` on the port over the countdown | trace both over recipe F's countdown and first second "
           "(`scenario_launch.py --watch-write` on `0x0063ba8c`, and a port log), then bind `Fi_GameMode` to the "
           "standalone's sub-state where they differ | the exe's A6a / A4 / A5 behaviour on ticks where the original's "
           "mode word is not 6 (their inputs, not their bodies) |" + nl)
    return s.replace(hdr, hdr + row, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  dotflip  ' not in s
    line = (D + "  00467650  VehicleWheelDrivetrainUpdate  C2->C4  dotflip: MASHED_A6A_DOT_ORDER default ON (Mariano's "
            "decision, dcphys PREREG amendment 1, opt-out MASHED_NO_A6A_DOT_ORDER=1). Default shared body bit-identical "
            "to the original 1440/1440 (.asi default, hook live, full record, no-op control 0); opt-out = the old "
            "174/1440 residual. New default G-CTRL 23ac657a / 7bf2f27f / 3186dd09 x2 (opt-out f78de789 / 43bbe0fb / "
            "161b1a6a, NO_FAITH 997f4117 / e6288905 / a63bdcfa unchanged), B 0e3790c4 / 55b84dd8 / 2324715b, (b) 9/9, "
            "(e) 3/3, D2 3/3, 13 tracks 0 RED. U-9784 filed (standalone game-mode word). " + R)
    return s.replace(m, m + line + nl, 1)


rw('hooks.csv', hooks)
rw('UNCERTAINTIES.md', uncertainties)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
