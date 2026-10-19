"""d3bc tracker transaction (re-classify, no C-level change): U-9768 RESOLVED, new U-9769, one CHANGELOG line. Line endings
kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/d3bc_20261019/RESULT_d3bc.md'
U68 = ('| U-9768 | ~~semantic~~ **RESOLVED 2026-10-19** (d3bc: the look point is FUN_00414c30\'s mode-7 target) | car 1\'s AI lookahead '
       'target at record call 104 (aistep row 105) on the five-knob stack | **RESOLVED 2026-10-19 (d3bc, ' + R + '):** the original\'s '
       'point `(-1.009332, -20.2197)` is the mode-7 candidate of FUN_00414c30 (call `0x004163c3`, return 2, `ai_mode 7`, `c4 64`); '
       'the same point opens mode 7 at row 100 in regimes A and C. The port\'s mode-7 arm is the default-OFF M4 set (`MASHED_REGISTRY` '
       '+ `MASHED_PICKUPS_ORIG` + `MASHED_REG_CARS` + `MASHED_REG_CALLBACKS` + `MASHED_MODE7`); with it on the d3bc stack car 1 takes '
       'mode 7 and runs exact to row 161 in B, 148 in A / C. | (none) | (none) | D3 (b) car-level parity |')
U69 = ('| U-9769 | semantic | the first per-car divergence on the d3bc full stack (u9767 stack + `MASHED_AI_LUT=7` + '
       '`MASHED_STEER_ACOS_CRT` + `MASHED_AI_FWD_ATROW` + `MASHED_STEER_SCALE_RDATA` + the M4 mode-7 set) | Car 1 row 149 (A and C): '
       'the original\'s FUN_00414c30 returns 0 (A then takes the FUN_004148b0 return, `c4 0 / c5 255`), the port\'s returns 2; '
       'stored error and record equal through 148. Car 2 row 23: own position equal at 22, `own_x` 1.00629162 vs 1.00472975 at 23, the '
       'call of its contact with the player (`ai_override` 1440 vs 1390). Car 3 row 39: both take FUN_00414a70\'s mode 3, own position '
       'equal, target `(-0.7680652, -2.8144028)` vs `(-0.7629314, -2.8055846)`. ' + R + ' | Which FUN_00414c30 input changes at row '
       '149; the parked player\'s state at calls 22..39 | Callee-entry probe of FUN_00414c30 at row 149 (o_aq0 recipe); a slot-0 '
       'record capture (`--statediff-car 0`) against the port\'s slot-0 dump over calls 0..40 | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9769 |' not in s and s.count('| U-9768 |') == 1
    i = s.index('| U-9768 |'); j = s.index(nl, i)
    return s[:i] + U69 + nl + U68 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00415e20  d3bc  U-9768 RESOLVED (the call-104 look point is FUN_00414c30\'s mode-7 target); new U-9769. '
            'FUN_00415e20 = LUT normalize + the CRT acos at PC_24 (FUN_004a70a5 keeps the precision bits) + the exact 0x005cc970, '
            '1054/1054 original rows offline (MASHED_STEER_ACOS_CRT); the AI forward is the record at row (MASHED_AI_FWD_ATROW); '
            '_DAT_005cd0e8 is 0.003f, not 0.0030034f (MASHED_STEER_SCALE_RDATA). All default OFF; (b) C still 0/3, B regresses on the '
            'default. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
