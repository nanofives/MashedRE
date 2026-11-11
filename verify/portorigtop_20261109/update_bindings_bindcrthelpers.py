"""BIND-CRT-HELPERS: the six MSVC compiler helpers bound to the exe CRT through naked jmp thunks
(Match/PortTop_CrtHelpers.cpp). No-chain convention: bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_bindcrthelpers.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'BIND-CRT-HELPERS: '
EV = (' libid libcmt.lib %s (verify/libidentity_20261030/libidentity_functions.tsv); unit test 451/451 vs the DDK '
      'body (verify/bindcrthelpers_20261111) (verify/portorigtop_20261109/RESULT_BIND-CRT-HELPERS.md)')
ROWS = [(0x004a2c48, '_ftol2', 'MashedCrt_ftol2_004a2c48', 'ftol2.obj identical-unlisted', 'ST0 in, EDX:EAX out, truncation'),
        (0x004a4170, '_alldiv', 'MashedCrt_alldiv_004a4170', 'lldiv.obj identical-unlisted', 'stdcall int64 / int64'),
        (0x004a4220, '_allmul', 'MashedCrt_allmul_004a4220', 'llmul.obj identical-unlisted', 'stdcall int64 * int64'),
        (0x004a4e40, '_aulldiv', 'MashedCrt_aulldiv_004a4e40', 'ulldiv.obj identical', 'stdcall uint64 / uint64'),
        (0x004aa060, '_aullrem', 'MashedCrt_aullrem_004aa060', 'ullrem.obj identical', 'stdcall uint64 % uint64'),
        (0x004a3440, '_chkstk', 'MashedCrt_chkstk_004a3440', 'chkstk.obj identical-unlisted', 'EAX = bytes, ESP lowered')]
FIXED = {rva: (name, 'crt', sym + ' (Match/PortTop_CrtHelpers.cpp) -> VS2022 CRT ' + name,
               T + 'naked jmp thunk onto the exe CRT helper, ' + conv + '.' + EV % obj, 5, 0)
         for rva, name, sym, obj, conv in ROWS}
if __name__ == '__main__':
    apply(T, FIXED, {})
