"""HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0): BINDINGS.tsv rows for the Rt2d open 0x00552b60 and its callees on exe
bodies under MASHED_ORIG_TOP with MASHED_ORIG_TOP_RWMOD=1, and the 0x00427ca0 call-site selector.

No chain (HB-GENFILES-NOCONFLICT): this script applies only this row's FIXED rows and APPEND notes to
BINDINGS.tsv in place, through verify/portorigtop_20261109/bindings_rows.py. Idempotent.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_hudinit_a.py
"""
import os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
T = 'HUDINIT-A: '
EV = ' (verify/portorigtop_20261109/RESULT_HUDINIT-A.md)'
RW = ', run only with MASHED_ORIG_TOP_RWMOD=1'
OPEN = 0x00552b60

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00427ca0: (None, 'stand-in', 'PortTop_00402750_Call00427ca0 -> PortTop_StandIn04E_00427ca0',
                 'PORT-TOP-04E: Match/PortTop_00402750_Pieces.cpp (logs its first call, returns 0). ' + T + 'the call site in '
                 'the matched 0x00402750 binds to the selector PortTop_00402750_Call00427ca0 (Match/PortTop_00552b60.cpp): with '
                 'MASHED_ORIG_TOP_RWMOD=1 it runs the Rt2d open 0x00552b60(0), the body\'s first call, then the stand-in; without '
                 'it the stand-in only. The body swaps in HUDINIT-B' + EV, 2, 0x00402750),
    OPEN: ('FontSys_InitSeq', 'matched', 'Orig_00552b60 (via PortTop_FontSysInit_00552b60)',
           T + 'Match/PortTop_00552b60.cpp = re/match/s24/00552b60.c (BYTE-IDENTICAL 45 B, relocheck OK). The entry keeps the '
           'camera argument the tail jmp at 0x00552b88 hands to 0x00552a60' + RW + '. Owner of the Rt2d globals '
           '0x00912500..0x0091320c and the freelist headers 0x007dc77c..0x007dc887 in the exe' + EV, 3, 0x00427ca0),
    0x00557250: ('FontSys_InitDataPools', 'matched', 'Orig_00557250', T + 'Match/PortTop_00557250.cpp = re/match/s24/00557250.c '
                 '(BYTE-IDENTICAL 83 B): freelists 0x007dc864 -> DAT_00912a34, 0x007dc840 -> DAT_00912a38' + EV, 4, OPEN),
    0x005540d0: ('FontCanvas_Init', 'matched', 'Orig_005540d0', T + 'Match/PortTop_005540d0.cpp = re/match/rwl04/005540d0.c '
                 '(BYTE-IDENTICAL 122 B): freelist 0x007dc77c -> DAT_00912a24, one entry through RwGlobals +0x118 -> '
                 'DAT_00912a28, 0x00553f40 twice' + EV, 4, OPEN),
    0x00556d20: ('FontSys_InitBuffers', 'matched', 'Orig_00556d20', T + 'Match/PortTop_00556d20.cpp = re/match/rwl05/00556d20.c '
                 '(BYTE-IDENTICAL 73 B): RwGlobals +0x108 0x2400 bytes -> DAT_00912a04, freelist 0x007dc7e8 (0x78-byte '
                 'entries) -> DAT_00912a00' + EV, 4, OPEN),
    0x005571e0: ('FontSys_InitFontPool', 'matched', 'Orig_005571e0', T + 'Match/PortTop_005571e0.cpp = re/match/s24/005571e0.c '
                 '(BYTE-IDENTICAL 49 B): freelist 0x007dc81c -> DAT_00912a2c' + EV, 4, OPEN),
    0x00555360: (None, 'stand-in', 'PortTop_StandInHIA_00555360', T + 'Match/PortTop_StandInsHIA.cpp (logs, returns 0). '
                 're/match/s24/00555360.c is 94.4%: residual = the three failed-check return blocks (own jne / ret vs one '
                 'shared xor eax, eax / ret) and one 16-bit store order in the index loop. Outside L-115 (SIB order only), so '
                 'the stand-in runs until L0 rules on the proof in the RESULT. Not written: 0x00912a08 / 0a0c / 0a10 / 0a14 / '
                 '0a18 / 0a1c / 0a20, 0x0091325c / 0x00913260 / 0x00913100, the 0x00912700 table' + EV, 4, OPEN),
    0x005c4ad0: (None, 'stand-in', 'PortTop_StandInHIA_005c4ad0', T + 'Match/PortTop_StandInsHIA.cpp (logs, returns 0). '
                 're/match/s28/005c4ad0.c is 96.7% (ops 100%): residual = the operand order of one imul (0x005c4b57 / '
                 '0x005c4b62 load +0x8, multiply by +0xc; the source loads +0xc first). Outside L-115, so the stand-in runs '
                 'until L0 rules on the proof in the RESULT. 0x00553f40 then returns 0 and leaves its node unchanged' + EV,
                 6, 0x00553f40),
    0x00552c10: ('FontSys_InitRenderState', 'matched', 'Orig_00552c10', T + 'Match/PortTop_00552c10.cpp = '
                 're/match/s24/00552c10.c (BYTE-IDENTICAL 254 B): matrix stack 0x00912a84 ([0] = RwMatrixCreate), 0x00912a44, '
                 '0x00912500. ORIGTOP-ONLY CROSS-TARGET vs .asi-only HUD/FontCtx.cpp' + EV, 4, OPEN),
    0x00552a60: ('FontSys_SetActiveCamera', 'matched', 'Orig_00552a60', T + 'Match/PortTop_00552a60.cpp = '
                 're/match/s24/00552a60.c (BYTE-IDENTICAL 236 B). Camera 0 on this path: DAT_00912c0c = 0, returns 0' + EV,
                 4, OPEN),
    0x00552b50: (None, 'matched', 'Orig_00552b50', T + 'Match/PortTop_00552b50.cpp = re/match/rt2d/00552b50.c (MATCH-RT2D-FONT source, '
                 'BYTE-IDENTICAL 6 B, relocheck OK 1/1): jmp dword ptr [0x00912c58]. Installed at camera +0x1c only for a '
                 'non-zero camera, so not called on this path' + EV, 5, 0x00552a60),
    0x00552750: ('FontCtx_ResetTransform', 'matched', 'Orig_00552750', T + 'Match/PortTop_00552750.cpp = '
                 're/match/s24/00552750.c (BYTE-IDENTICAL 238 B). ORIGTOP-ONLY CROSS-TARGET vs .asi-only HUD/TextCluster.cpp'
                 + EV, 5, 0x00552c10),
    0x00553f40: ('FontGlyph_UploadData', 'matched', 'Orig_00553f40', T + 'Match/PortTop_00553f40.cpp = '
                 're/match/rwl04/00553f40.c (BYTE-IDENTICAL 205 B). Its list create 0x005c4ad0 is a stand-in, so it returns 0 for '
                 'the 0x005540d0 node' + EV,
                 5, 0x005540d0),
    0x005c4c60: (None, 'stand-in', 'PortTop_StandInHIA_005c4c60', T + 'Match/PortTop_StandInsHIA.cpp (logs, returns 0): '
                 're/match/s28/005c4c60.c is 93.0%; 0x00553f40 calls it only for a node with a non-zero count, and the only '
                 'node on this path has 0' + EV, 6, 0x00553f40),
    0x0055deb0: (None, 'stand-in', 'PortTop_StandInHIA_0055deb0', T + 'Match/PortTop_StandInsHIA.cpp (logs, returns 0): no '
                 'matched source; 0x00553f40 calls it only for +0x4 == -1 inside a test that needs +0x4 != -1' + EV,
                 6, 0x00553f40),
    0x004cc9e0: ('thunk_FUN_004cc820', 'matched', 'Orig_004cc9e0', T + 'Match/PortTop_004cc9e0.cpp = '
                 're/match/rwl01/004cc9e0.c (BYTE-IDENTICAL 5 B): jmp 0x004cc820' + EV, 5, 0x00557250),
    0x004cc820: ('RwFreeListCreate', 'bound', 'PortTop_RwFreeListCreate_004cc820', T + 'Match/PortTop_004cc820.cpp: binding '
                 '(L0 ruling 2026-11-10: a binding, like a librw one, not a hand transcription) onto the exe freelist (RwGlobals +0x118 / +0x11c, one allocation per entry): header +0x0 rounded '
                 'entry size, +0x4, +0x8, +0xc alignment, +0x10 empty block list, +0x18 2 / 3. re/match/rwl01/004cc820.c is '
                 '74.5% and does not run. Deviations, no reader on this path: no block preallocation, no link into '
                 'DAT_007d45cc (BSS in the exe)' + EV, 6, 0x004cc9e0),
}

APPEND = {  # rva -> text appended to the existing note
    0x004c57a0: ' ' + T + 'also called by the matched 0x00552c10 (Rt2d open, matrix stack [0])' + EV,
    0x005c4d30: ' ' + T + 'also named by the matched 0x00553f40, on a branch that cannot run there' + EV,
}


def main():
    sys.path.insert(0, HERE)
    from bindings_rows import apply
    apply('HUDINIT-A: ', FIXED, APPEND)


if __name__ == '__main__':
    main()
