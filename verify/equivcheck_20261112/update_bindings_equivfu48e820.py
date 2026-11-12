"""EQUIV-FU-48E820: 0x0048e820 near-match -> matched (re/match/s15/0048e820.c BYTE-IDENTICAL, 664 B, relocheck OK 48/48).
No-chain BINDINGS.tsv row (bindings_rows.apply, HB-GENFILES-NOCONFLICT)."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'portorigtop_20261109'))
from bindings_rows import apply

EV = ' (verify/equivcheck_20261112/RESULT_EQUIV-FU-48E820.md)'
FIXED = {
    0x0048e820: ('FUN_0048e820', 'matched', 'Orig_0048e820 (via PortTop_PS3_Call0048e820)',
                 'EQUIV-FU-48E820: Match/PortTop_0048e820.cpp = re/match/s15/0048e820.c, 100% BYTE-IDENTICAL; 0x0048cee0 '
                 'is called with the emitter index (FUN_0048cee0(i)), the PARTICLE-3-SWAP index bridge is removed' + EV,
                 4, 0x0040bbb0),
}
APPEND = {}

if __name__ == '__main__':
    apply('EQUIV-FU-48E820: ', FIXED, APPEND)
