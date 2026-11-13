"""NEARMATCH-555360 (L-111 step 1, L-115 / L-117 = (a)): BINDINGS.tsv rows for 0x00555360 on its unedited 94.4% source as a
near-match body under MASHED_ORIG_TOP with MASHED_ORIG_TOP_RWMOD=1, and the two callees it brings onto the path.

No chain (HB-GENFILES-NOCONFLICT): this script applies only this row's FIXED rows and APPEND notes to
BINDINGS.tsv in place, through verify/portorigtop_20261109/bindings_rows.py. Idempotent.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_nearmatch_555360.py
"""
import os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from bindings_rows import apply  # noqa: E402

T = 'NEARMATCH-555360: '
EV = ' (verify/portorigtop_20261109/RESULT_NEARMATCH-555360.md)'

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00555360: (None, 'near-match', 'Orig_00555360',
                 T + 'L-115/L-117, byte table RESULT_L115-HUDINIT-PROOFS.md. Match/PortTop_00555360.cpp = the unedited '
                 're/match/s24/00555360.c (94.4%, 55 differing bytes, all explained: return-block layout and its shift, '
                 'two-store order), port axis only, never C4: font freelists 0x00912a08 / 0x00912a14, path 0x00912a0c, index '
                 'table 0x00912700, dictionary 0x00912a1c / 0x00912a20, reader 0x00913100' + EV, 4, 0x00552b60),
    0x004c5890: ('RwTexDictionaryCreate', 'bound', 'RwCoreBind_TexDictionaryCreate_004c5890',
                 T + 'RwCore/RwCore_Bind_004c5890.cpp (RWC-BIND-4C5890, L-129 / L-130): the byte-matched body on the RW core '
                 'under MASHED_RWC_CORE=1, returns 0 with it off. 0x00555360 stores the result at 0x00912a1c / 0x00912a20 and '
                 'tests it nowhere. No librw binding (L-124)' + EV, 5, 0x00555360),
    0x00555910: (None, 'stand-in', 'PortTop_StandInNM_00555910',
                 T + 'Match/PortTop_00555360.cpp (logs, returns 0): the font-file reader 0x00555360 stores at 0x00913100, '
                 'not called on this path. re/match/s24/00555910.c is 100%, its callees are the font-read chain (HUDINIT-C)'
                 + EV, 5, 0x00555360),
}

APPEND = {  # rva -> text appended to the existing note
    0x004cc9e0: ' ' + T + 'also called by the near-match 0x00555360 (headers 0x007dc7a0, 0x007dc7c4)' + EV,
}


if __name__ == '__main__':
    apply(T, FIXED, APPEND)
