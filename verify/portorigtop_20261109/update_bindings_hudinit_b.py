"""HUDINIT-B (L-111 step 1, chain 2 of 0x00427ca0): BINDINGS.tsv rows for the 0x00427ca0 body on its matched source under
MASHED_ORIG_TOP with MASHED_ORIG_TOP_RWMOD=1, the brush create 0x00556d70 and its callees, and 0x00556cc0.

No chain (HB-GENFILES-NOCONFLICT): this script applies only this row's FIXED rows and APPEND notes to
BINDINGS.tsv in place, through verify/portorigtop_20261109/bindings_rows.py. Idempotent. FIXED sets 0x00427ca0
whole, over the HUDINIT-A stand-in row.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_hudinit_b.py
"""
import os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from bindings_rows import apply  # noqa: E402

T = 'HUDINIT-B: '
EV = ' (verify/portorigtop_20261109/RESULT_HUDINIT-B.md)'
HUD = 0x00427ca0

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    HUD: (None, 'matched', 'FUN_00427ca0 (via PortTop_00402750_Call00427ca0)',
          T + 'Match/MatchS06_00427ca0.cpp = re/match/s06/00427ca0.c (100%, relocheck OK), its exe callees now bound by name '
          '(0x00552b60 PortTop_FontSysInit_00552b60, 0x00556cc0 Orig_00556cc0, 0x00556d70 Orig_00556d70, 0x00556e90 and '
          '0x00427880 stand-ins, 0x0042a6b0 FUN_0042a6b0). The selector PortTop_00402750_Call00427ca0 '
          '(Match/PortTop_00552b60.cpp) runs it with MASHED_ORIG_TOP_RWMOD=1, the stand-in PortTop_StandIn04E_00427ca0 '
          'otherwise' + EV, 2, 0x00402750),
    0x00556cc0: ('SetDat00912a20', 'matched', 'Orig_00556cc0', T + 'Match/PortTop_00556cc0.cpp = re/match/s24/00556cc0.c '
                 '(BYTE-IDENTICAL 10 B): DAT_00912a20 = the fgdc20.txd dictionary. ORIGTOP-ONLY DUP-IN-TARGET vs the hand '
                 'SetDat00912a20 (Frontend/BatchAA_s3.cpp, both targets), so no hand body runs on the knob path' + EV,
                 3, HUD),
    0x00556d70: (None, 'matched', 'Orig_00556d70', T + 'Match/PortTop_00556d70.cpp = re/match/rwl05/00556d70.c '
                 '(BYTE-IDENTICAL 195 B, relocheck OK 5/5): brush create, one 0x78-byte entry from DAT_00912a00 through '
                 'RwGlobals +0x118' + EV, 3, HUD),
    0x00557110: (None, 'matched', 'Orig_00557110', T + 'Match/PortTop_00557110.cpp = re/match/s24/00557110.c '
                 '(BYTE-IDENTICAL 169 B, relocheck OK 4/4). ORIGTOP-ONLY CROSS-TARGET vs .asi-only Util/PromoLoop_sessionB.cpp'
                 + EV, 4, 0x00556d70),
    0x00556e90: (None, 'stand-in', 'PortTop_StandInHIB_00556e90', T + 'Match/PortTop_StandInsHIB.cpp (logs, returns 0). '
                 're/match/s24/00556e90.c is 97.5%: 35 differing bytes, integer moves scheduled across x87 instructions, '
                 'outside L-115 (SIB order only). Brush colours stay zero' + EV, 3, HUD),
    0x00427880: (None, 'stand-in', 'PortTop_StandInHIB_00427880', T + 'Match/PortTop_StandInsHIB.cpp (logs, returns 0): the '
                 'font-file loader, chain C (its callee 0x00554390 is 33.5%). DAT_0067d838 stays 0' + EV, 3, HUD),
}

APPEND = {  # rva -> text appended to the existing note
    0x0042a6b0: ' ' + T + 'also called by the matched 0x00427ca0 (fgdc20.txd -> 0x00556cc0)' + EV,
    0x004c5a60: ' ' + T + 'also named by the matched 0x00556d70, on the +0x68 != 0 branch the zeroed entry never takes' + EV,
    0x00556e40: ' ' + T + 'the four handles 0x0067d83c..0x0067d848 are built by the matched 0x00427ca0 now, and the RWMOD '
                'boot takes the matched branch for each' + EV,
}


if __name__ == '__main__':
    apply(T, FIXED, APPEND)
