"""d3bB2 tracker transaction (re-classify, no C-level change). Writes with newline="" so CRLF/LF stay as found.
New rows U-9744..U-9747 (newest-first block under the table header), a note on U-9684, hooks.csv notes on 5 RVAs,
one CHANGELOG line under <!-- ENTRIES -->. Asserts every anchor exactly once. usage: py -3.12 apply_trackers.py"""
import csv, io, os

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d3bB2_20261017/RESULT_d3bB2.md'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    t = fn(s, '\r\n' if '\r\n' in s else '\n')
    open(p, 'w', encoding='utf-8', newline='').write(t)


ROWS = [
    "| U-9747 | structural | `D3d9Render/TrackRenderer.cpp` countdown block + `Race/PathProgStep.cpp` `PathProg_FrameTick` (FUN_0040da50's FUN_00408610 loop) under `MASHED_AI_TICK_FIRST` | With the AI tick moved before the player's physics (`FUN_00425a40` order: `0x00425a73` before `0x00425a8d`), the first racing AI ticks read path progress the exe has not converged: the AI rows of `0x008a96e8` read 0.0 on the first producer call, 29.507563 for all three cars on the next, and the original's at-rest 29.73 / 29.457 / 29.18 (`o_tg1`) only on the third (`B_SLP_K1`). Running the producer on every countdown frame does not help: the AI cars are skipped there (`VehicleSlotGetter`, 0x0046c7b0, is not 1 before their first racing step). Consequence: `FUN_00414a70` returns 1 for car 2 at call 1 (original 0), G-ORDER of " + R + " fails. | Which exe write makes the AI records' `+0x4` / alive word 1, and when the original's is 1 (pre-GO hold ticks run `FUN_0046baa0` for every alive car) | Make the AI cars count as alive for `FUN_0040da50` on the countdown frames as the original's are (find the exe writer), then re-run G-ORDER under `MASHED_AI_TICK_FIRST` | the `MASHED_AI_TICK_FIRST` default; D3 (b) regime B car 2 (call 1) |",
    "| U-9746 | structural | `Vehicle/VehiclePhysicsRun.cpp` `VehiclePhysics_StepCar` (A4 `VehicleControlIntegrate` per car, then that car's substeps) vs `FUN_00470c70` | The original runs the A4 loop for EVERY car (`0x0047101b..0x00471099`, `FUN_00470670` at `0x00471076`) and the speed refresh (`0x004710a1..0x004710da`) before ANY car's substeps (`FUN_004709a0` at `0x00471137`). The exe steps one car at a time (A4 then substeps), so a car<->car impulse applied in an earlier car's substeps (the player's, `0x00470bcd`) reaches a later car BEFORE its A4 of the same frame. Measured on regime B car 2 (full `S` stack of " + R + "): post-contact speed 1045.466 then 1038.302 against the original's frozen 1050.409 (velocity (45.75, -57.41, -1047.75) for 9 coast frames, `o_c2r1`), which moves car 2's line by 0.02 by window call 33 and loses the original's second coast at call 61. The `[d2bsp]` comment's 'identical while remI <= 0x32' does not hold across a car-car contact. | None from the bytes; it is a structural port difference | Run every car's A4 + refresh at the frame boundary (as the +0xaec gate already is, `A4ContactGate`), then each car's substeps, behind a knob; it changes the player's physics order, so score the three D2 metrics | D3 (b) regime B car 2 after call 23; any car<->car contact frame |",
    "| U-9745 | semantic | `Collision/CarCarContacts.cpp` (tail of `0x00469df0`), `Ai/AiStandalone.cpp` `Ai_ContactEventPush` / `Ai_ContactEventDrain` (`MASHED_AI_CONTACT_EVENT`, default OFF) | `FUN_00469df0` pushes a type-4 ring entry on every contact unless `FUN_0040e350()` is 1..5 (`0x0046ae97..0x0046aec3`, `0x0046b04c..0x0046b078`); the count asymmetry only orders the pair. `FUN_00422ba0` (after the physics, `FUN_00425a40`) drains it and calls `FUN_00417370(car)` for each car with `FUN_0040e470(car) != 1` when `FUN_0046cbb0`'s first word is 0 for both: AI override timer `[0x0089a4fc + v*0x74] = 0xb4 - 0xb4*__ftol([0x0089a360] - 10.0)` = 1440, replayed by `FUN_00418560`'s tail for 29 AI ticks (watched: `o_ov1`, `0x0041739b`, `0x004187ee`). The port had omitted the push as dev-viz only. Ported behind the knob: car 2's coast heading +0.146 deg/frame against the original's +0.1475 (G-COAST). Alone on the default it passes regime-B car 2 but fails B cars 1 and 3 and A / C, because the exe has AI<->AI contacts in the window (pairs 1-2, 1-3, 2-3) and each now freezes both commands. Not ported: `FUN_004922e0` and the pair tables `0x008995b0..` (no exe reader). | Whether the original has any AI<->AI contact in the (b) windows (U-9185 / U-9186 say not for 1-3) | Fix the exe's extra AI<->AI contacts (U-9185 / U-9186), then re-score the knob; port the pair tables if an exe reader appears | D3 (b) all regimes once enabled; the knob's default |",
    "| U-9744 | semantic | `D3d9Render/TrackRenderer.cpp` default countdown freeze vs `FUN_0040fc00` (`0x0040fe4f..0x0040fe5e`); fix behind `MASHED_AI_COUNTDOWN_CLOCK` (default OFF), `Ai/AiStandalone.cpp` | The original's AI clock `DAT_007f0ff4` counts 50 per tick through phase 3 and states 4 / 5 (sub-state 7 excepted, `0x0040fe46`) and reads 38850..46400 at GO on 31/31 labelled captures. The exe's countdown advanced no clock, so it reached GO at 0 and `FUN_00416250`'s fresh-steer arm (`0x00416608..0x00416621`, el >= 0xc8) failed on racing calls 1-2: every AI car wrote c0 = 0 where the original writes err*speed*_DAT_005cd0e8 (car 2: 11, 23). With the knob the k = 1, 2 commands equal the original's on all 9 car-regime pairs (G-RAMP). (b): A 1/3 (car 3 passes, car 2 turns to FAIL `c0#` 41), B 2/3, C 0/3; 13 tracks 867 rows 0 RED (the 3 C27 car-part rows of dflip drop out); G-CTRL `490c4b2d / 1fdd0ea6 / 46088ac4`. | None for the clock; the (b) trade comes from downstream divergences (U-9745, U-9746) | User decision on the default; re-score after U-9746 | the knob's default; D3 (b) |",
]


def uncert(s, nl):
    hdr = '| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |' + nl + '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    for u in ('U-9744', 'U-9745', 'U-9746', 'U-9747'):
        assert ('| ' + u + ' |') not in s, u
    s = s.replace(hdr, hdr + nl.join(ROWS) + nl, 1)
    key = '| U-9684 |'
    i = s.index(key)
    j = s.index(nl, i)
    note = (' **2026-10-17 (d3bB2, ' + R + '):** regime-B car 2 fails (b) through this brake. With the U-9684 chain + '
            '`MASHED_GRID_DIR` + `MASHED_AI_COUNTDOWN_CLOCK` + `MASHED_AI_CONTACT_EVENT` + `MASHED_AI_TICK_FIRST` the exe\'s car 2 '
            'enters mode 3 at window call 23 and its commands equal the original\'s on every call 0..38 except call 1 (U-9747); '
            'the contact frame is decided by a 3e-4 SAT margin (`sat.py`). Remaining: U-9746 (A4 order), U-9747.')
    row = s[i:j]
    assert row.rstrip().endswith('|')
    s = s[:i] + row.rstrip()[:-1].rstrip() + note + ' |' + s[j:]
    return s


NOTES = {
    '00469df0': 'd3bB2 2026-10-17: tail pushes type-4 ring entry on every contact unless FUN_0040e350() in 1..5 (0x0046ae97/0x0046b04c); NOT dev-viz; exe event behind MASHED_AI_CONTACT_EVENT (U-9745)',
    '00417370': 'd3bB2 2026-10-17: AI override timer [0x0089a4fc+v*0x74] = 0xb4 - 0xb4*__ftol([0x0089a360]-10.0) = 1440 on Training (watched o_ov1); exe body Ai/AiStandalone.cpp AiOverrideTimerSet behind MASHED_AI_CONTACT_EVENT (U-9745); no C-level change',
    '00422ba0': 'd3bB2 2026-10-17: type-4 arm FUN_00417370 effect ported in Ai_ContactEventDrain (MASHED_AI_CONTACT_EVENT); FUN_004922e0 + pair tables not ported (U-9745)',
    '0040fc00': 'd3bB2 2026-10-17: clock add 0x0040fe5e runs on pre-GO ticks too (38850..46400 at GO); exe countdown stand-in behind MASHED_AI_COUNTDOWN_CLOCK (U-9744)',
    '00470c70': 'd3bB2 2026-10-17: A4 loop for all cars (0x0047101b..0x00471099) before any substep (0x00471137); exe steps per car, see U-9746',
}


def hooks(s, nl):
    # line-based: only the target rows are re-serialised, every other byte stays as found
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
    line = ('2026-10-17  d3bB2  regime-B car 2 cause chain (' + R + '): AI clock at GO (FUN_0040fc00 0x0040fe5e; '
            'MASHED_AI_COUNTDOWN_CLOCK, U-9744), type-4 contact event -> FUN_00417370 AI override (MASHED_AI_CONTACT_EVENT, U-9745), '
            'AI tick before physics (MASHED_AI_TICK_FIRST, U-9747), A4 order (U-9746). All default OFF, no C-level change; '
            'G-CTRL knobs off 4c81db89/aa03f7ae/7d3af534, 13 tracks 870 rows 0 RED.')
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
