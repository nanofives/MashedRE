"""PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): BINDINGS.tsv rows for the particle stand-ins swapped onto their
matched bodies on the MASHED_ORIG_TOP path, and their exe-only matched callees 0x004892c0 / 0x004893a0 / 0x00487150. Runs
update_bindings_otrender.py first (which runs the othud chain and applies the GAMEPLAY, VEHICLE and RENDER tables), then
writes the rows below. Idempotent. A new row goes right after its caller's row, not at the end of the file.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otother.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import update_bindings_otrender as R  # noqa: E402

T = 'PORT-OT-OTHER: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00489290: (None, 'matched', 'Orig_00489290',
                 T + 'Match/PortTop_00489290.cpp = re/match/s14/00489290.c: the 100 pools at 0x00703118 get m_54 = 0, '
                 'm_58 = 1. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS14_00489290.cpp', 3, 0x0040bbb0),
    0x0048a460: (None, 'matched', 'Orig_0048a460',
                 T + 'Match/PortTop_0048a460.cpp = re/match/s14/0048a460.c: dwords +0 / +4 = 0 per 0x330-byte record '
                 '0x00709238..0x00713198. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS14_0048a460.cpp', 3, 0x0040bbb0),
    0x00487e00: (None, 'matched', 'Orig_00487e00',
                 T + 'Match/PortTop_00487e00.cpp = re/match/s14/00487150.c: 0x00487150 builds the record 0x00703068 '
                 '(0x004770c0 0x80c, 0x00476cb0 5, 6)', 3, 0x0040bbb0),
    0x00487150: (None, 'matched', 'Orig_00487150',
                 T + 'Match/PortTop_00487e00.cpp = re/match/s14/00487150.c (static in the source)', 4, 0x00487e00),
    0x00487140: (None, 'matched', 'Orig_00487140',
                 T + 'Match/PortTop_00487140.cpp = re/match/s14/00487140.c: 0x004768c0(0x00703068). ORIGTOP-ONLY '
                 'CROSS-TARGET vs .asi-only Match/MatchS14_00487140.cpp', 3, 0x0040bd00),
    0x00485d90: (None, 'matched', 'Orig_00485d90',
                 T + 'Match/PortTop_00485d90.cpp = re/match/s14/00485d90.c: four 0x004892c0 pools into the records at '
                 '0x006fde10', 3, 0x0040bbb0),
    0x004892c0: (None, 'matched', 'Orig_004892c0',
                 T + 'Match/PortTop_004892c0.cpp = re/match/s14/004892c0.c: first free pool at 0x00703118 (m_58 != 0), '
                 '0x004770c0 + 0x00476cb0 + 0x00474db0 items. Exe-only', 4, 0x00485d90),
    0x004893a0: (None, 'matched', 'Orig_004893a0',
                 T + 'Match/PortTop_004893a0.cpp = re/match/s14/004893a0.c: 0x00476cb0(p + 0x10, a, b). ORIGTOP-ONLY '
                 'CROSS-TARGET vs .asi-only Match/MatchS14_004893a0.cpp', 4, 0x00485d90),
    0x00485e10: (None, 'matched', 'Orig_00485e10',
                 T + 'Match/PortTop_00485e10.cpp = re/match/s14/00485e10.c: 0x004768c0(pool + 0x10) + RwGlobals +0x10c on '
                 'the four pools 0x00485d90 takes', 3, 0x0040bd00),
    0x004862d0: (None, 'matched', 'Orig_004862d0',
                 T + 'Match/PortTop_004862d0.cpp = re/match/s14/004862d0.c: 150 records at 0x006fe790 cleared, '
                 '0x004770c0 records 0x006fe700 / 0x006fe740', 3, 0x0040bbb0),
    0x00486350: (None, 'matched', 'Orig_00486350',
                 T + 'Match/PortTop_00486350.cpp = re/match/s14/00486350.c: 0x004768c0 on 0x006fe700 / 0x006fe740', 3,
                 0x0040bd00),
    0x0048eac0: (None, 'matched', 'Orig_0048eac0',
                 T + 'Match/PortTop_0048eac0.cpp = re/match/s15/0048eac0.c: nine 0x004770c0 records 0x007668f8..0x007706d8',
                 3, 0x0040bbb0),
    0x0048fdd0: (None, 'matched', 'Orig_0048fdd0',
                 T + 'Match/PortTop_0048fdd0.cpp = re/match/s15/0048fdd0.c: 0x004768c0 on the nine records 0x0048eac0 '
                 'builds', 3, 0x0040bd00),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_otrender.py')])
    rows = R.apply(FIXED)
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
