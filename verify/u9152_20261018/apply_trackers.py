"""u9152 tracker transaction (re-classify, no C-level change): U-9152 note, new U-9758, hooks.csv notes on 004709a0 and
00470c70, one CHANGELOG line. Writes with newline="" so CRLF/LF stay as found. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9152_20261018/RESULT_u9152.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


def append(s, nl, key, note):
    assert s.count(key) == 1, key
    i = s.index(key); j = s.index(nl, i); row = s[i:j]
    assert row.rstrip().endswith('|')
    return s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]


ROW = ('| U-9758 | semantic | `Vehicle/VehiclePhysicsRun.cpp` world forward `+0x9d4..+0x9dc` (`off::kForward`, written as '
       '`(cos yaw, 0, sin yaw)` at `:1209 / :1411 / :1774`) | With the spawn pose, spawn normal, spawn basis, `_DAT_005cea60` and '
       'the `+0x968` half all the original\'s (u9152 `SPLSBKNF60DBR1`), car 1\'s remaining call-0 float differences against '
       'o_wobj2 are `+0x9d4` (-8.74227766e-08 original, -4.37113883e-08 port, exactly 2x) and `+0x9cc` (up y, 0.999988973 / '
       '1, 0.999989033 with `MASHED_UP_TRINORMAL`). A6a reads the forward for the lateral velocity (`Integrate2.cpp:671-672`) '
       'and at call 1 the velocity `+0x9b0` differs (-0.00108456612 / -0.00108465552) with omega `+0x9c0`; car 1\'s drift is '
       '1.202e-4 at call 21. ' + R + ' | The original\'s writer of `+0x9d4..+0x9dc` and its formula (the port\'s cos / sin of the '
       'heading is not cited to an RVA) | `scenario_launch.py --watch-write` on car 1 `+0x9d4`, then port the writer behind a '
       'knob and re-run `posdrift.py` / `recdiff1.py` against o_wobj2 | D3 (b) car-level parity before the first contacts |')
N9152 = (' **2026-10-18 (u9152, ' + R + '):** the double buffer is decoded: A9 writes dst = `[+0x9ac]` from src = `[+0x9a8]` '
         '(`0x00470a6f..0x00470ad3`), the solver reads and corrects dst only (`0x0046f6d8`), a settled substep swaps the selectors '
         '(`0x00470c31..0x00470c41`), exhausted retries skip the swap (`0x00470ab3` -> `0x00470c47`), an odd frame ends with slot 0 '
         'copied over slot 1 (`0x00471195..0x004711af`). `MASHED_RING_DBUF` (default OFF) ports it: the half equals o_wobj2 at '
         'calls 0 and 1 (pads aside). **The u9756 premise above is refuted:** car 1\'s drift is bit-identical with and without it '
         '(the solver reads dst after A9 wrote it). The half\'s only in-substep readers are car-car (proximity test, '
         '`FUN_00469df0`); the port steps each car\'s whole frame before the next, so car j is at its frame-start pose where the '
         'original has it after substep 1, and the knob alone moves contacts (B car 1 PASS -> FAIL): not a default candidate '
         'until car-car pairing is per-substep. Next drift driver: the world forward `+0x9d4` (U-9758).')
NOTES = {
    '004709a0': 'u9152 2026-10-18: A9 EBX=dst=[+0x9ac] half, EDI=src=[+0x9a8] half (0x00470a6f..0x00470ad3); selector swap after a settled substep 0x00470c31..0x00470c41; retry>=2 exits to 0x00470c47 without the swap; ported as MASHED_RING_DBUF (default OFF)',
    '00470c70': 'u9152 2026-10-18: frame end 0x00471195..0x004711af: if +0x9a8 != 0 then +0x9ac=+0x9a8, +0x9a8=(+0x9a8-1)&1, rep movsd +0x928 -> +0x968',
}


def uncert(s, nl):
    assert '| U-9758 |' not in s
    i = s.index('| U-9757 |')
    s = s[:i] + ROW + nl + s[i:]
    return append(s, nl, '| U-9152 |', N9152)


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
    line = ('2026-10-18  004709a0  u9152  U-9152 note: the +0x928/+0x968 double buffer decoded (dst=[+0x9ac], swap 0x00470c31, '
            'frame-end fixup 0x00471195) and ported as MASHED_RING_DBUF (default OFF): the half equals o_wobj2, car 1 drift unchanged '
            '(premise refuted), car-car contacts move (B car 1 FAIL), not a default candidate. New U-9758 (world forward +0x9d4). '
            'No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
