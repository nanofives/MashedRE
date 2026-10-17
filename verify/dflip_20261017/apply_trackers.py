"""dflip tracker transaction (re-classify, no C-level change), user decisions 2026-10-17.
Run once from the worktree root: py -3.12 verify/dflip_20261017/apply_trackers.py
Every edit asserts exactly one match; files keep their CRLF line endings."""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dflip_20261017/RESULT_dflip.md'


def edit(rel, fn):
    p = os.path.join(W, rel)
    raw = open(p, encoding='utf-8', newline='').read()
    crlf = '\r\n' in raw
    t = raw.replace('\r\n', '\n')
    t = fn(t)
    open(p, 'w', encoding='utf-8', newline='').write(t.replace('\n', '\r\n') if crlf else t)


def one(t, old, new):
    assert t.count(old) == 1, (old[:70], t.count(old))
    return t.replace(old, new)


U9743 = ('| U-9743 | structural | `FUN_00424eb0` @ `0x00424eb0` (the `FUN_0046c7d0` loop, call at `0x00425a82`), '
         '`Race/HeadingAssist.cpp` `HeadingAssist_Frame` (AI slots 1..3 only) | The original runs the steer-0 heading assist '
         '`FUN_0046c7d0` for every live car, the player (slot 0) included. The port runs it for the AI slots only (mask 0x1, '
         'default ON since dflip 2026-10-17); the player form is not ported. It changes the player\'s heading on steer-0 frames, '
         'which the D2 recipe (player solo, `MASHED_MEASURE_SOLO`) measures. Unmeasured. (d3b proposed this row as "U-9729"; '
         'that id was taken.) | the player\'s per-frame assist angle in the original on the D2 recipe (the `--watch-write` / '
         'lat-bracket probes of `verify/d3b_20261016` on slot 0), and the three D2 metrics with the player form on. | add a '
         'mask bit for slot 0 in `HeadingAssist_Frame`, re-run the D2 recipe x3 (`score_d2.py`, bounds `d81a8df6`, H1) and '
         'G-CTRL, user decision on the default. | D2 player heading on steer-0 frames (D2 watch list); no C-level gate |')


def unc(t):
    # new U-9743 above the newest row
    t = one(t, '\n| U-9742 | structural |', '\n' + U9743 + '\n| U-9742 | structural |')
    # U-9191: cause found and the fix default ON -> resolved
    t = one(t, '| U-9191 | semantic **— STILL OPEN after 2026-10-05\'s arm leg',
            '| U-9191 | ~~semantic~~ **RESOLVED 2026-10-17** (cause found by d3b 2026-10-16: the car-1 heading residual is '
            'GENERATED on steer-0 frames by the unported steer-0 heading assist `FUN_0046c7d0`, called by `FUN_00424eb0` '
            'between the AI tick `0x00425a73` and the physics frame `0x00425a8d`, -0.19 .. -0.26 deg per frame on car 1, '
            'writer proven by hardware watchpoint (`verify/d3b_20261016/RESULT_d3b.md` section 2, `o_ww2`). Ported for the '
            'AI slots (`Race/HeadingAssist.cpp`), default ON by user decision 2026-10-17 (`' + R + '`, opt-out '
            '`MASHED_NO_HEADING_ASSIST=1`); car-1 heading residual 1.033 -> 0.402 deg. The player form is U-9743.) '
            '**History:** semantic **— STILL OPEN after 2026-10-05\'s arm leg')
    # U-9736: I-NAME rule decided and implemented
    t = one(t, '| U-9736 | structural | `verify/d1r1_20261016/check_gates.py` I-NAME',
            '| U-9736 | ~~structural~~ **RESOLVED 2026-10-17** (dflip, user decision: a `#WxH:<D3DFORMAT>` raster name counts '
            'as named on a row when the same name is drawn on the other side of that row; a `#` name on one side only stays '
            'unnamed. `check_gates.py` `is_named` / `peer_names`, `run13.py score` passes the peer. 13 tracks x 3 frames: '
            'I-NAME true on all 78 gate lines, Neustein 1169 standalone 59/59 and original 77/77, Highway 928 standalone 77/77, '
            'INSTRUMENT GATES PASS (`' + R + '`). The STATE-row label instability in this row is not addressed by the rule and '
            'stays a known artifact, memory `drawlist-instrument-row-artifacts`.) | `verify/d1r1_20261016/check_gates.py` I-NAME')
    # U-9738: user accepted the exit approximation, the row stays open
    t = one(t, 'then re-run the pontoon replay with the exit rows | MASHED_PONTOON_CARRY default (stays OFF) |',
            'then re-run the pontoon replay with the exit rows. **2026-10-17 (dflip): the user ACCEPTED this exit '
            'approximation for Warzone; MASHED_PONTOON_CARRY is default ON (opt-out MASHED_NO_PONTOON_CARRY=1). The row '
            'stays open: the exe still has no RWP car mode.** | nothing (accepted by the user 2026-10-17; was: '
            'MASHED_PONTOON_CARRY default) |')
    # U-9730 / U-9741: deferred by the user
    t = one(t, '| U-9741 | structural | two exe bodies', '| U-9741 | structural **DEFERRED 2026-10-17 (D-11079)** | two exe bodies')
    t = one(t, '| U-9730 | structural | `FUN_00420420`', '| U-9730 | structural **DEFERRED 2026-10-17 (D-11078)** | `FUN_00420420`')
    return t


D = ('| D-11078 | **Two-car original capture for one vehicle TXD per race (U-9730).** The original loads the vehicle dictionary '
     'only for the first car instantiated (`FUN_00420420`, load `0x004204c0` when `DAT_0063d7e8 == 0`, made current '
     '`0x004204ce`; freed by `FUN_0041ec00` when `DAT_0063e498` is 1); the standalone registers each model\'s own TXD. '
     'Whether the original ever instantiates two different models in one race is [UNCERTAIN U-9730]. | User decision '
     '2026-10-17 (dflip): deferred, not worked. | A race with two different car models is needed by a parity or D-phase gate, '
     'or a multi-car race row is attributed to the vehicle dictionary. Then take the original capture U-9730 names and bind '
     'cars 2..N to the first car\'s dictionary if the draws show it. | (D1-residue) |\n'
     '| D-11079 | **Unify the duplicate exe bodies of `0x004c4680` and `0x00532b80` (U-9741).** `Track/WarzonePontoon.cpp` '
     'OrthoNormalize vs `Vehicle/BodyOrientationIntegrate.cpp` MatrixOrthoNormalize (D2 path), and WarzonePontoon.cpp '
     'KfInterpolate vs `D3d9Render/TrackRenderer.cpp` LinearAnimMatrix (Dump train). Both pairs are allowlisted in '
     '`re/tools/dual_copy_allowlist.txt`. | User decision 2026-10-17 (dflip): deferred, not worked; unifying moves closed '
     'gates (D2 for `0x004c4680`, the Dump capfix rows for `0x00532b80`). | A D2 or Dump-row lane is open anyway, or the '
     'dual-copy lint allowlist is being drained. Then switch each caller to the listing body behind a knob and measure D2 '
     '(`score_d2.py`, bounds `d81a8df6`) and the Dump capfix rows; user decision on the default. | (D2-watch) |\n')


def dfr(t):
    i = t.index('\n| D-11077 |')
    j = t.index('\n', i + 1)
    return t[:j + 1] + D + t[j + 1:]


NOTES = {
    '0046c7d0': ' | dflip 2026-10-17: exe port Race/HeadingAssist.cpp (AI slots 1..3), DEFAULT ON (opt-out MASHED_NO_HEADING_ASSIST=1); '
                'reproduces the original\'s measured turn at car 1\'s first steer-0 frame to 4 digits (-0.23935 vs -0.2393 deg); '
                'player form not ported (U-9743); no Frida diff of the body, C-level unchanged; verify/d3b_20261016, ' + R,
    '004093b0': ' | dflip 2026-10-17: exe port inside Race/HeadingAssist.cpp (TrackHeading), default ON with the assist; '
                'C-level unchanged; verify/d3b_20261016',
    '00443dc0': ' | d3b 2026-10-16: exe LUT sqrt sites behind MASHED_AI_LUT=5 (default OFF), look replay 1384/1384 bit-exact on '
                'the (b) windows; C-level unchanged; verify/d3b_20261016',
    '00443300': ' | d3b 2026-10-16: exe Catmull-Rom operand order behind MASHED_AI_LUT bit 0x4 (=5 with the lookahead LUT, default '
                'OFF), look replay 1384/1384 bit-exact; C-level unchanged; verify/d3b_20261016',
}


def hooks(t):
    """Line-level: only the four target rows are re-serialised (csv, minimal quoting); every other line is untouched."""
    import csv, io
    lines = t.split(chr(10))
    for rva, note in NOTES.items():
        idx = [k for k, l in enumerate(lines) if l.startswith(rva + ',')]
        assert len(idx) == 1, (rva, idx)
        row = next(csv.reader([lines[idx[0]]]))
        assert len(row) == 10, (rva, len(row))
        row[8] = row[8] + note
        buf = io.StringIO(); csv.writer(buf, lineterminator='').writerow(row)
        lines[idx[0]] = buf.getvalue()
    return chr(10).join(lines)


CL = None   # set by main from RESULT numbers


def chg(t):
    return one(t, '<!-- ENTRIES -->\n', '<!-- ENTRIES -->\n' + CL + '\n')


if __name__ == '__main__':
    import sys
    CL = open(os.path.join(W, 'verify/dflip_20261017/changelog_line.txt'), encoding='utf-8').read().strip()
    edit('UNCERTAINTIES.md', unc)
    edit('DEFERRED.md', dfr)
    edit('hooks.csv', hooks)
    edit('re/analysis/CHANGELOG.md', chg)
    print('applied')
