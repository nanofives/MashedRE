"""dcphys2 amendment 1 (p2flip) tracker transaction (re-classify): the P2 stack default ON (Mariano's decision);
A6b 0x00468980 / A5 0x0046ddb0 / A4 0x00470670 C2 -> C4 on the default body. Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcphys2_20261022/RESULT_dcphys2.md'
D = '2026-10-22'
V = 'mashedmod/src/mashed_re/Vehicle/'

COMMON = ("p2flip " + D + ": the P2 stack default ON (Mariano's decision; Core/FaithDefaults.cpp, the .asi seeds the same; "
          "opt-out MASHED_NO_<name>=1). C4 on the DEFAULT shared body: hook installed live, canonical race, in-process A/B "
          "against the live original over the whole 0xd04 record (+ scratch / matrix), no-op control 0 on every call, no "
          "stub in the body. New exe default G-CTRL c3bfcef3 / 4ed4849b / 4c3548a1 x2, B 4985eb3b / b6e3d073 / 6717c15e, "
          "(b) 9/9, (e) 3/3, D2 3/3, 13 tracks 878 rows 0 RED; opt-out = the old 23ac657a trio. " + R + " | ")
ROWS = {
    '00468980': ('AeroStabilize.cpp', "A6b: 402/402 on the final .asi (242 boosted airborne) apart from the matrix pad words "
                 "(npad); coverage U-9786 (state != 0, natural air). "),
    '0046ddb0': ('ForceIntegrator.cpp', "A5: 3214/3214 on the final .asi (334 airborne incl. 14 natural partial-ground); "
                 "inputs U-9784 / U-9785, coverage U-9786 (random surface). "),
    '00470670': ('VehicleControl.cpp', "A4 (whole function incl. its dispatch): 5403/5403 on the final .asi; input "
                 "U-9784. "),
}


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def hooks(s, nl):
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    hit = set()
    for li, line in enumerate(lines[1:], 1):
        k = line[:8]
        if k not in ROWS or line[8] != ',':
            continue
        ef, note = ROWS[k]
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == 'C2', k
        assert r[ix['file']] == r[ix['exe_file']] == V + ef, k
        r[ix['confidence']] = 'C4'
        r[ix['notes']] = COMMON + note + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit.add(k)
    assert hit == set(ROWS), hit
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  p2flip:' not in s
    tail = (" p2flip: the P2 stack default ON (Mariano's decision, dcphys2 PREREG amendment 1). New default G-CTRL c3bfcef3 / "
            "4ed4849b / 4c3548a1 x2, B 4985eb3b / b6e3d073 / 6717c15e, (b) 9/9, (e) 3/3, D2 3/3, 13 tracks 878 rows 0 RED; "
            "opt-out = 23ac657a / 7bf2f27f / 3186dd09. " + R)
    lines = [D + "  00470670  VehicleControlUpdate  C2->C4  default body 5403/5403 (whole function)." + tail,
             D + "  0046ddb0  VehicleWheelForceIntegrator  C2->C4  default body 3214/3214." + tail,
             D + "  00468980  VehicleAeroStabilizer  C2->C4  default body 402/402 (pad words apart)." + tail]
    return s.replace(m, m + nl.join(lines) + nl, 1)


rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
