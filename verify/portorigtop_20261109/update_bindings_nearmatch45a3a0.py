"""NEARMATCH-45A3A0: 0x0045a3a0 runs its near-match source on the MASHED_ORIG_TOP path (L-115/L-117).

No-chain convention (HB-GENFILES-NOCONFLICT): only this row's delta, through bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_nearmatch45a3a0.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'NEARMATCH-45A3A0: '
EV = ' (verify/portorigtop_20261109/RESULT_NEARMATCH-45A3A0.md)'

FIXED = {
    0x0045a3a0: ('FUN_0045a3a0', 'near-match', 'Orig_0045a3a0 (via PortTop_PWI_Call0045a3a0)',
                 T + 'Match/PortTop_0045a6b0.cpp runs the unedited re/match/s09/0045a3a0.c (81.0%, register-allocation '
                 'residual), L-115/L-117, byte table RESULT_L115-REGCHOICE-45A3A0.md; clears 10 fields of the 0x0068bd00 '
                 'table and sets record +0x0c = 1.0f' + EV,
                 4, 0x0045a6b0),
}
APPEND = {
    0x0045a6b0: ' ' + T + 'callee 0x0045a3a0 now runs its near-match body (was a logging stand-in)' + EV,
}

if __name__ == '__main__':
    apply(T, FIXED, APPEND)
