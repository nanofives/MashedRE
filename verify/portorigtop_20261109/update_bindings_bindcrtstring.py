"""BIND-CRT-STRING: strchr / strrchr / wcsncpy bound to the exe CRT (Match/PortTop_CrtString.cpp).
0x005a7b40 stays unbound (identity open). No-chain convention: bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_bindcrtstring.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'BIND-CRT-STRING: '
EV = ' (verify/bindcrtstring_20261112/RESULT_BIND-CRT-STRING.md)'
ROWS = [
    (0x004a2fd0, 'strchr', 'MashedCrt_strchr_004a2fd0', 'std::strchr',
     'libid strchr.obj DIFFERS on bytes only (image add edx,1 for DDK inc edx); normalized listing equal, CF-safe, 4000/4000 unicorn runs'),
    (0x004a3480, 'strrchr', 'MashedCrt_strrchr_004a3480', 'std::strrchr',
     'libid strrchr.obj DIFFERS on bytes only (3 inc/dec as add/sub 1); normalized listing equal, CF-safe, 4000/4000 unicorn runs'),
    (0x004a3bdf, 'wcsncpy', 'MashedCrt_wcsncpy_004a3bdf', 'std::wcsncpy', 'libid wcsncpy.obj identical'),
]
FIXED = {rva: (name, 'crt', sym + ' (Match/PortTop_CrtString.cpp) -> ' + tgt, T + idn + '; pure, no CRT state' + EV, 5, 0)
         for rva, name, sym, tgt, idn in ROWS}
if __name__ == '__main__':
    apply(T, FIXED, {})
