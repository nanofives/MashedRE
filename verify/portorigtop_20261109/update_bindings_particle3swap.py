"""PARTICLE-3-SWAP: 0x0048e820 near-match (L-115/L-117/L-118), 0x0048cee0 and 0x0048cf70 matched, MASHED_ORIG_TOP.
No-chain convention: only this row's delta, through bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_particle3swap.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'PARTICLE-3-SWAP: '
EV = ' (verify/portorigtop_20261109/RESULT_PARTICLE-3-SWAP.md)'
FIXED = {
    0x0048e820: ('FUN_0048e820', 'near-match', 'Orig_0048e820 (via PortTop_PS3_Call0048e820)',
                 T + 'Match/PortTop_0048e820.cpp runs the unedited re/match/s15/0048e820.c (85.5%); L-115/L-117/L-118, '
                 'byte table RESULT_PORT-OT-PARTICLE-3.md. The EDI emitter index reaches 0x0048cee0 through the '
                 '0x004b6480 bridge (i = (p0 - 0x00722250) / 0x20)' + EV, 3, 0x0040bbb0),
    0x0048cee0: ('FUN_0048cee0', 'matched', 'Orig_0048cee0 (via PortTop_PS3_Cee0_0048cee0)',
                 T + 'Match/PortTop_0048cee0.cpp = re/match/s14/0048cee0.c; builds record i of the 17 at 0x00722470 '
                 '(0x004770c0, 0x00476cb0)' + EV, 4, 0x0048e820),
    0x0048cf70: ('FUN_0048cf70', 'matched', 'Orig_0048cf70 (via PortTop_PS3_Call0048cf70)',
                 T + 'Match/PortTop_0048cf70.cpp = re/match/s15/0048cf70.c; 0x004768c0 on the 17 records' + EV, 3, 0x0040bd00),
}
if __name__ == '__main__':
    apply(T, FIXED, {})
