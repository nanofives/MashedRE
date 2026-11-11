"""BIND-CRT-HEAP: malloc / free / delete / new bound together to the exe CRT heap (Match/PortTop_CrtHeap.cpp).
No-chain convention: bindings_rows.apply.  usage: py -3.12 verify/portorigtop_20261109/update_bindings_bindcrtheap.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'BIND-CRT-HEAP: '
EV = ' (verify/portorigtop_20261109/RESULT_BIND-CRT-HEAP.md)'
ROWS = [(0x004a45fb, '_malloc', 'MashedCrt_malloc_004a45fb', 'std::malloc', 'libid malloc.obj identical-unlisted'),
        (0x004a460d, '_free', 'MashedCrt_free_004a460d', 'std::free', 'libid free.obj identical-unlisted'),
        (0x004a3bda, 'operator_delete', 'MashedCrt_delete_004a3bda', 'std::free', 'libid delete.obj identical (jmp _free)'),
        (0x004a3c1c, 'operator_new', 'MashedCrt_new_004a3c1c', 'std::malloc (NULL on failure, no throw)',
         'libid identical-ambiguous (_iswupper bytes); code: calls __nh_malloc 0x004a45cf, so operator new')]
FIXED = {rva: (name, 'crt', sym + ' (Match/PortTop_CrtHeap.cpp) -> ' + tgt,
               T + idn + '; one heap (exe CRT) with librw defaultMemfuncs and RwGlobals +0x108/+0x10c' + EV, 5, 0)
         for rva, name, sym, tgt, idn in ROWS}
if __name__ == '__main__':
    apply(T, FIXED, {})
