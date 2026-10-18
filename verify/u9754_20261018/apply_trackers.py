"""u9754 tracker transaction (re-classify, no C-level change): U-9754 RESOLVED, new U-9755, hooks.csv notes on 0046cc40 /
0046f6c0 / 0046d400, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9754_20261018/RESULT_u9754.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9755 | semantic | `Vehicle/VehiclePhysicsRun.cpp` spawn pose / `VehiclePhysics_SeedSpawnPose`, original `FUN_004704c0` '
       '(`param_3 - FUN_0046d400(...)`) | With the spawn wheel normal (U-9754) and the spawn basis (`MASHED_SPAWN_BASIS_RW`) '
       'both the original\'s, car 1\'s z still leaves the original\'s at window call 4 (2.4e-7) and reaches 1.2e-4 at call 21 '
       '(the lookahead flip) under `S`. Remaining call-0 differences against o_wobj2: the spawn y `+0x95c` 0.468502462 / '
       '0.468502671 (7 ulp) and the wheel depths it feeds (`+0x194 / +0x258 / +0x31c / +0x3e0`), and the `+0x968` half, '
       'which the original holds one substep apart and the port sets to the same pose (U-9152 deviation). ' + R + ' | '
       'Which of the two moves the drift | Port `FUN_0046d400` (vertical RW line query, `FUN_00538c80` + `FUN_0046d3a0`) for the '
       'spawn y behind a knob and re-run `posdrift.py` / `newdiff.py`; then the `+0x968` half | D3 (b) car-level parity before '
       'the first contacts; D2 first-frame state |')
N9754 = (' **RESOLVED 2026-10-18 (u9754, ' + R + '):** the seed is written by the classifier `FUN_0046cc40` at `0x0046cff8` '
         '(watched on o_seed1, value 0.9994620680809021, return chain ending `0x0046f827` after the walk call `0x0046f817`): the '
         'walk\'s LUT-normalized face normal of the spawn triangle. The port\'s value came from `ReassertContacts`, which '
         'writes the port-only `ProbeGround` soup normal over the classifier\'s at spawn. `MASHED_REASSERT_KEEP_NORMAL` '
         '(default OFF) keeps the classifier\'s: seed bit for bit, refresh at calls 15 / 22 as the original. Car 1\'s drift '
         'is unchanged (U-9755). Not default: with `MASHED_SPAWN_BASIS_RW` D2 driving-median 1997.89 / 2002.84 / 1996.54 '
         '(> 1982.44); alone it fails (b) B car 1.')

NOTES = {
    '0046cc40': 'u9754 2026-10-18: 0x0046cff8 writes the wheel normal (+0x204) from the batch face normal (watched on o_seed1, spawn frame); exe ReassertContacts overwrote it (MASHED_REASSERT_KEEP_NORMAL, default OFF)',
    '0046f6c0': 'u9754 2026-10-18: the spawn wheel-normal seed is its classifier pass (0x0046f817 walk -> 0x0046cff8); U-9754 resolved',
    '0046d400': 'u9754 2026-10-18: FUN_004704c0 spawn y = param_3 - this (7 ulp from the exe on car 1); unported (U-9755)',
}


def uncert(s, nl):
    key = '| U-9754 |'
    assert s.count(key) == 1 and '| U-9755 |' not in s
    i = s.index(key)
    j = s.index(nl, i); row = s[i:j]
    old = '| U-9754 | semantic |'
    assert row.startswith(old)
    row = row.replace(old, '| U-9754 | ~~semantic~~ **RESOLVED 2026-10-18** (u9754: classifier `0x0046cff8`; port `ReassertContacts`) |', 1)
    assert row.rstrip().endswith('|')
    row = row.rstrip()[:-1].rstrip() + N9754 + ' |'
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
    line = ('2026-10-18  0046cc40  u9754  U-9754 RESOLVED: the spawn wheel normal is the classifier\'s walk normal (0x0046cff8, '
            'watched); the port\'s ReassertContacts overwrote it with ProbeGround\'s. MASHED_REASSERT_KEEP_NORMAL (default OFF) '
            'seeds it bit for bit; car 1 drift unchanged (new U-9755: spawn y FUN_0046d400, +0x968 half). Not default: D2 '
            'driving-median out. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
