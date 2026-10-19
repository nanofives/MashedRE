"""u9769 tracker transaction (re-classify, no C-level change): U-9769 RESOLVED (its car-2 / player part; the rest moves to
U-9770), new U-9770, one CHANGELOG line. Line endings kept. usage: py -3.12 apply_trackers.py"""
import os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/u9769_20261019/RESULT_u9769.md'
U69 = ('| U-9769 | ~~semantic~~ **RESOLVED 2026-10-19** (u9769: the player-side gap is the first car<->car contact) | the first '
       'per-car divergence on the d3bc full stack | **RESOLVED 2026-10-19 (u9769, ' + R + '):** the player\'s record equals the '
       'original through window call 21 and splits at 22, the car-0 / car-2 contact (VehicleCarCarContact `0x00469df0`, call '
       '`0x00470bcd`). Three causes: FUN_004c4dc0 was an empty stub (`Rw_MatrixDerive`), so the spin tail read an uninitialised '
       'local (`MASHED_CC_MATRIX_INVERT`); car 2 entered before its own A4 (FUN_00470c70 runs FUN_00470670 for all records first, '
       '`0x00471076`; `MASHED_A4_ORDER`, U-9746); sqrtf and doubles where the original runs the RW LUT bodies at PC_24 '
       '(`MASHED_CC_PC24`). With all three the contact is bit-exact and car 2 runs exact to row 138 in C. All default OFF; '
       '(b) A regresses (car 2) with `MASHED_CC_MATRIX_INVERT` alone. Car 1 row 149 and car 3 move to U-9770. | (none) | (none) | '
       'D3 (b) car-level parity |')
U70 = ('| U-9770 | semantic | the next per-car divergences on `EXL7AFSM7PCA` (d3bc full stack + `MASHED_CC_PC24` + `MASHED_A4_ORDER`) | '
       'Car 3: record call 7, `+0x9b8` `-1964.98584` (original) vs `-1964.98596`, calls 0..6 exact; commands equal through row 57. '
       'Car 1: row 149 (A and C), the original\'s FUN_00414c30 returns 0, the port\'s 2, stored error and record equal through 148. '
       'Also measured: the default build\'s car<->car spin reads an uninitialised stack local, so a frame change of '
       'VehicleCarCarContact alone moved `B_DEF_K0` `6efb49ed` -> `ddddf961`. ' + R + ' | The term of car 3\'s call-7 velocity '
       'that differs; the FUN_00414c30 input that changes at row 149 | `MASHED_FORCE_SPLIT` on slot 3 against an `--a6a-probe-wheel` '
       'original capture of car 3; a callee-entry probe of FUN_00414c30 at row 149 | D3 (b) car-level parity |')


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def uncert(s, nl):
    assert '| U-9770 |' not in s and s.count('| U-9769 |') == 1
    i = s.index('| U-9769 |'); j = s.index(nl, i)
    return s[:i] + U70 + nl + U69 + s[j:]


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1
    line = ('2026-10-19  00469df0  u9769  U-9769 RESOLVED: the player\'s call-22 split is the first car<->car contact. '
            'FUN_004c4dc0 was an empty stub in the port\'s contact deps (S-3442 is marked resolved for the function, not for this '
            'call site), so the default build\'s spin tail reads uninitialised stack (MASHED_CC_MATRIX_INVERT); car 2 entered '
            'before its A4 (MASHED_A4_ORDER); LUT + PC_24 arithmetic (MASHED_CC_PC24). Replay 64/64, in-run contact bit-exact; '
            'all default OFF; new U-9770. No C-level change. ' + R)
    return s.replace(m, m + line + nl, 1)


rw('UNCERTAINTIES.md', uncert)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
