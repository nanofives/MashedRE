"""2026-10-12 parent: a real fixture for 0x00430910 MenuOptionSlotGet (key menu_option_slot_get).

Its old vectors (arg_type none, quiescent menu) never reached the case the row-movers branch fixed (selection 11
in modes 6..9 returns 0). Bytes (MASHED.exe.unpatched):
  0x00430910 mov eax,[0x67e9fc]; add eax,-3; cmp eax,6; ja 0x00430999 (ret: modes outside 3..9 return mode-3)
  jump table 0x0043099c via byte index 0x004309a4: modes 3,4,5 -> arm A 0x0043092b; modes 6,7,8,9 -> arm B 0x0043095a
  arm A returns 0 for selection [0x67f184] in {0,3,4,6,7,8,9,10,11}; arm B for {0,1,2,5,6,7,4,11}
  otherwise eax = [0x7f0a40 + (sel + row*12)*4], row = [0x67f17c] (0x00430986..0x00430992).
Every vector seeds the whole 13x12 table with a distinct value per cell, so a wrong index or stride returns a
different number. Return folded; the function writes nothing.
"""

FIXES = {}

_TBL = 0x007f0a40


def _vec(mode, sel, row):
    s = [{'addr': 0x0067e9fc, 'val': mode}, {'addr': 0x0067f184, 'val': sel}, {'addr': 0x0067f17c, 'val': row}]
    for i in range(13 * 12):
        s.append({'addr': _TBL + 4 * i, 'val': 0x5A000100 + i})
    return {'args': [], 'seed': s}


_VECS = [
    # arm A (modes 3..5): zero list, then table reads
    _vec(3, 0, 0), _vec(4, 3, 1), _vec(5, 10, 2), _vec(3, 11, 0),
    _vec(3, 1, 0), _vec(4, 2, 5), _vec(5, 5, 12),
    # arm B (modes 6..9): zero list incl. the fixed case (sel 11), then table reads
    _vec(6, 11, 0), _vec(7, 11, 3), _vec(8, 4, 1), _vec(9, 0, 0),
    _vec(6, 3, 2), _vec(9, 8, 7), _vec(7, 10, 4),
    # outside 3..9: returns mode-3
    _vec(2, 1, 0), _vec(10, 1, 0),
]

FIXES['menu_option_slot_get'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int32', 'args': []},
    'fold_ret': True, 'observe': [], 'freeze_main': True,
    'path1_tests': _VECS, 'path2_tests': [_VECS[7], _VECS[4], _VECS[14]],
}
