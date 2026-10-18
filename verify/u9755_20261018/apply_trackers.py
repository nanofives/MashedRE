"""u9755 tracker transaction (re-classify, no C-level change): U-9755 RESOLVED, new U-9756, hooks.csv notes on 0046d400 /
00538d60 / 00408b00 / 004704c0, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9755_20261018/RESULT_u9755.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROW = ('| U-9756 | semantic | `Vehicle/VehiclePhysicsRun.cpp` spawn settle (`VehiclePhysics_SpawnSettle` / the StepCar settle block), '
       'original `FUN_004704c0` -> `FUN_0046f6c0(0)` | With the spawn pose bit for bit the original\'s (`MASHED_SPAWN_Y_LINE`), '
       'the port\'s spawn settle returns car 1 at physics y 0.468762839 (caller 0.506262839), the same from spawn y 0.471709967 '
       'or 0.471710175; the original\'s frame-2 y after `FUN_0046f6c0(0)` is 0.468762636 (o_sdrop1), held to GO, 0.468502462 '
       'after the first racing step (port 0.468502671). This 7-ulp gap is car 1\'s call-0 y difference and the remaining '
       'call-0 candidate for its z drift (U-9755). ' + R + ' | Which term of the spawn pass differs (wheel contact positions, '
       'depths `+0x194..`, the y correction `pos -= mean(fv * n)`) | Record-dump the port\'s settle step and diff it against '
       'o_sdrop1 frame 2 field by field (`recdiff1.py`), then port the differing term | D3 (b) car-level parity before the first '
       'contacts; D2 first-frame state |')
N9755 = (' **RESOLVED 2026-10-18 (u9755, ' + R + '):** the spawn y is ported (`MASHED_SPAWN_Y_LINE`, default OFF): `FUN_0046d400` '
         'over the replica (line walk `FUN_00538d60` generated verbatim, callback `FUN_0046d3a0`), param_3 = `FUN_00408b00`\'s '
         'grid point (x / y / z in its order), exact y through `SeedSpawnPose`; spawn pose of all four cars = o_sdrop1 bit for '
         'bit. The premise is refuted: the call-0 y gap does not come from the spawn y (the spawn settle returns the same y '
         'from either); it is made in `FUN_0046f6c0(0)`\'s spawn pass (U-9756). Default-ON candidate: (b) pass set kept, D2 3/3, '
         '13 tracks 0 RED; G-CTRL `b7534619 / fb690fb1 / d3a20f4c`.')

NOTES = {
    '0046d400': 'u9755 2026-10-18: ported as CollWorld_SpawnY (line (x,y+3,z)->(x,y-3,z), best 6.0, best-3) with callback 0x0046d3a0; probed on the original (--spawn-drop-probe, o_sdrop1): caller 0x00470517, best bits equal; MASHED_SPAWN_Y_LINE (default OFF)',
    '00538d60': 'u9755 2026-10-18: verbatim naked-asm replica BW_538d60 (re/tools/naked_specs/collworld_line_u9755.json), FUN_00538c80 case 1 in the standalone',
    '00408b00': 'u9755 2026-10-18: slot 0..3 grid point x/y/z in its op order (RoundGrid_GridPoint); the port place() combined the lateral terms (car 2 z 1 ulp off)',
    '004704c0': 'u9755 2026-10-18: spawn y = param_3 - FUN_0046d400 ported (MASHED_SPAWN_Y_LINE); the FUN_0046f6c0(0) spawn pass y is 7 ulp off on the port (U-9756)',
}


def uncert(s, nl):
    key = '| U-9755 |'
    assert s.count(key) == 1 and '| U-9756 |' not in s
    i = s.index(key)
    j = s.index(nl, i); row = s[i:j]
    old = '| U-9755 | semantic |'
    assert row.startswith(old)
    row = row.replace(old, '| U-9755 | ~~semantic~~ **RESOLVED 2026-10-18** (u9755: spawn y ported; the call-0 gap is the spawn solver pass, U-9756) |', 1)
    assert row.rstrip().endswith('|')
    row = row.rstrip()[:-1].rstrip() + N9755 + ' |'
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
    line = ('2026-10-18  0046d400  u9755  U-9755 RESOLVED: spawn y ported (FUN_0046d400 over the replica, line walk 0x00538d60 '
            'verbatim, FUN_00408b00 grid point in op order) behind MASHED_SPAWN_Y_LINE (default OFF); spawn pose of all four cars '
            'bit for bit (o_sdrop1). Call-0 y gap is the FUN_0046f6c0(0) spawn pass (new U-9756). Candidate: (b) pass set kept, '
            'D2 3/3, 13 tracks 869 rows 0 RED. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
