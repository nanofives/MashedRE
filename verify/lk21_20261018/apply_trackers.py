"""lk21 tracker transaction (re-classify, no C-level change): new U-9754, U-9156 note, hooks.csv notes on 0046c5f0 /
004704c0 / 0046f6c0, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/lk21_20261018/RESULT_lk21.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9754 | semantic | `Collision/WheelContactSolver.cpp` (`FUN_0046f6c0`), `Vehicle/VehiclePhysicsRun.cpp` spawn '
       'settle; original `FUN_004704c0` -> `FUN_0046f6c0(0)` (`0x00470638`) | The per-wheel ground normal a car carries from '
       'spawn (record `+0x200..+0x208`, stride 0xc4) is (0, 0.999462068, -0.0312347524) on the original (o_wobj2 car 1, '
       'calls 0..~13 of the (b) window) and (0, 0.999512017, -0.0312363114) on the port; from ~call 18 both carry the walk\'s '
       '(0, 0.999512076, -0.0312347524). The original\'s seed differs from the walk\'s in y only (5.0e-5). It sets the wheel '
       'force (`+0x210..+0x21c`, ratio 1.00005 at call 1), which is where car 1\'s position drift starts (z one float step '
       'at call 4, 1.2e-4 at call 21, the lookahead flip at 21 under `S`; ' + R + '). | Which code inside the spawn pass '
       'writes the seed and how its y is formed | Disassemble `FUN_0046f6c0`\'s branches reached from `FUN_004704c0` '
       '(substep 0) for the writes to `+0x204` and the y computation; port it behind a knob and re-run `recdiff1.py` / '
       '`posdrift.py` against o_wobj2 | D3 (b) car-level parity before the first contacts (every car); D2 first-frame '
       'state |')
N9156 = (' **2026-10-18 (lk21, ' + R + '):** record `+0x9c8..+0x9d0` is written by `FUN_0046c5f0` at `0x0046f7b0` (ESI = '
         'record + 0x9c8, EAX / ECX / EDX = transformed suspension points `0x0088e620` / `0x0088e644` / `0x0088e638`): the '
         'LUT-sqrt normal of the body\'s suspension plane. The off-unit magnitude this row read as an ortho-normalized row '
         'is the LUT error. Ported behind `MASHED_UP_TRINORMAL` (default OFF): `+0x9c8` / `+0x9d0` match o_wobj2, `+0x9cc` '
         '0.999992728 against 0.999988973 (the transformed points still differ).')

NOTES = {
    '0046c5f0': 'lk21 2026-10-18: writes state: 0x0046f7b0 ESI=record+0x9c8 (LUT normal of transformed susp points 0/3/2); 0x0046fd57 ESI=stack local (3-grounded branch); 0x0046db60 in FUN_0046da80 ESI=record+0x9c8; exe MASHED_UP_TRINORMAL ports the first (default OFF)',
    '004704c0': 'lk21 2026-10-18: spawn basis via FUN_004c4d20(m,(0,1,0),deg,1) 0x0047061c/0x0047062e (FUN_004c4a50 elements; exe MASHED_SPAWN_BASIS_RW, default OFF); spawn y = param_3 - FUN_0046d400 (RW line query, unported); FUN_0046f6c0(0) seeds the per-wheel normal (U-9754)',
    '0046f6c0': 'lk21 2026-10-18: FUN_0046c5f0 call at 0x0046f7b0 writes +0x9c8 (MASHED_UP_TRINORMAL); spawn-pass per-wheel normal seed open (U-9754)',
}


def uncert(s, nl):
    key = '| U-9753 |'
    assert s.count(key) == 1 and '| U-9754 |' not in s
    i = s.index(key)
    s = s[:i] + ROW + nl + s[i:]
    key = '| U-9156 |'
    assert s.count(key) == 1
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + N9156 + ' |' + s[j:]


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
    line = ('2026-10-18  0046c5f0  lk21  car 1\'s call-21 lookahead flip is position drift (replay exact); drift enters '
            'through the per-wheel force at call 1 from the spawn wheel normal seed (U-9754 new). Default-OFF knobs '
            'MASHED_SPAWN_BASIS_RW (FUN_004c4d20 basis, G-BASIS pass) and MASHED_UP_TRINORMAL (+0x9c8 = FUN_0046c5f0 LUT '
            'normal, U-9156 note); neither moves the drift. No default change, no C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
