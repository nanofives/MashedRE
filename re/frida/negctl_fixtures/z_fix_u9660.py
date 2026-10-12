"""negctl re-earn fix, U-9660 (2026-10-12): 0x0042ac90 MenuEntryGet (key menu_entry_get).

The registry entry ran arg_type 'none' at the quiescent main menu. The body (disasm of original/MASHED.exe.unpatched):

  0x0042ac90  mov ecx,[0x67e9f8] ; shl ecx,6            slot index * 0x40
  0x0042ac99  mov edx,[ecx+0x67ed38]                    table pointer of that slot
  0x0042ac9f  mov ecx,[ecx+0x67ed40]                    cursor of that slot
  0x0042aca7  cmp ecx,-1 ; je 0x0042acd2                -1 skips the item walk
  0x0042acac  inc ecx                                   cursor+1 passes, each: scan to the next 0xff040000
  0x0042acb0  cmp [edx+eax*4],0xff040000                (the current word is checked before the loop at 0x0042acc0)
  0x0042accd  inc eax ; dec ecx ; jne 0x0042acb0        step past it
  0x0042acd2  final scan, stops on 0xff050000 / 0xff140000 / 0xff060000 (0x0042acd5 / 0x0042ace0 / 0x0042ace8)
  0x0042acfd  cmp [edx+eax*4],0xff060000 ; jne -> 0x0042ad0a mov eax,[edx+eax*4+4] ; else or eax,-1

At the main menu with no seeded table, slot 0's pointer is NULL and both arms read [0 + eax*4] -> AV at 0x0 (U-9660).
This fixture points the slot's table pointer at a scratch word array it controls, seeds the cursor, and on the
multi-slot vectors seeds every slot 0..3 with a different table so a port reading the wrong slot reads a
different result. Pure read: the only observable is the return value (fold_ret). The game's main thread is frozen
for the whole run because the menu code reads and writes 0x0067e9f8 / 0x0067ed38.. itself.

Unseeded scratch words are 0xA5A5A5A5 (scratch_fill), which matches no sentinel.
"""

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'

T4 = 0xFF040000    # item tag            0x0042acb0 / 0x0042acc5
T5 = 0xFF050000    # action key A        0x0042acd5 / 0x0042acf5
T14 = 0xFF140000   # action key B        0x0042ace0
T6 = 0xFF060000    # group end -> -1     0x0042ace8 / 0x0042acfd

_SLOT = 0x0067e9f8
_PTR = 0x0067ed38
_CUR = 0x0067ed40
_TBL_STRIDE = 0x100   # one 64-dword table per slot in scratch


def _tbl(off, words):
    return [{'addr': {'scratch': off + i * 4}, 'val': w & 0xFFFFFFFF} for i, w in enumerate(words)]


def _v(cursor, words, slot=0, others=None):
    """slot: the active slot index. others: {slot: (cursor, words)} for the inactive slots."""
    s = [{'addr': _SLOT, 'val': slot}]
    tabs = dict(others or {})
    tabs[slot] = (cursor, words)
    for sl, (cur, ws) in sorted(tabs.items()):
        s.append({'addr': _PTR + sl * 0x40, 'val': {'scratch': sl * _TBL_STRIDE}})
        s.append({'addr': _CUR + sl * 0x40, 'val': cur & 0xFFFFFFFF})
        s += _tbl(sl * _TBL_STRIDE, ws)
    return {'args': [], 'seed': s}


# Three items; item 2 has no action key before its group end.
_MENU3 = [T4, 0x100, T5, 0xAAAA0001, T6,
          T4, 0x101, T14, 0xAAAA0002, T6,
          T4, 0x102, 0x33, T6, 0xAAAA0003]

# Same shape, different action codes: the inactive-slot decoy.
_DECOY = [T4, 0x900, T5, 0xDEC00001, T6,
          T4, 0x901, T5, 0xDEC00002, T6,
          T4, 0x902, T5, 0xDEC00003, T6]

_ALL_DECOYS = {k: (0, _DECOY) for k in range(4)}


def _multi(slot, cursor, words):
    o = {k: v for k, v in _ALL_DECOYS.items() if k != slot}
    return _v(cursor, words, slot, o)


_TESTS = [
    _v(-1, [T5, 0x11111111]),                              # 0  cursor -1, key at word 0 -> 0x11111111
    _v(-1, [0x1, 0x2, T14, 0x22222222]),                   # 1  cursor -1, T14 at word 2 -> 0x22222222
    _v(-1, [0x7, T6, 0x44]),                               # 2  cursor -1, group end first -> 0xffffffff
    _v(-1, [T4, 0x1, T14, 0x55555555]),                    # 3  final scan does NOT stop on T4 -> 0x55555555
    _v(-1, [T5, T6]),                                      # 4  returns the word after the key even if it is T6
    _v(0, _MENU3),                                         # 5  item 0, T5 -> 0xAAAA0001
    _v(1, _MENU3),                                         # 6  item 1, T14 -> 0xAAAA0002
    _v(2, _MENU3),                                         # 7  item 2, T6 before any key -> 0xffffffff
    _v(0, [0x5, T5, 0xBAD, T4, 0x200, T5, 0xC0DE]),        # 8  item walk skips a T5 before the first T4 -> 0xC0DE
    _v(0, [T4, T5, 0x77777777]),                           # 9  T4 at word 0 (check before the loop) -> 0x77777777
    _v(1, [T4, T4, T5, 0x88888888]),                       # 10 two T4 back to back -> 0x88888888
    _v(-1, [T6, 0x99]),                                    # 11 T6 at word 0 -> 0xffffffff
    _v(-1, [T5, 0x0]),                                     # 12 action code 0 -> 0
    _multi(2, 1, _MENU3),                                  # 13 slot 2 active, decoys in 0/1/3 -> 0xAAAA0002
    _multi(3, 0, _MENU3),                                  # 14 slot 3 active -> 0xAAAA0001
    _multi(1, -1, [0x3, T14, 0x12345678]),                 # 15 slot 1, cursor -1 -> 0x12345678
]

FIXES['menu_entry_get'] = {
    'arg_type': _SCAL,
    'signature': {'ret': 'uint32', 'args': []},
    'fold_ret': True,
    'observe': [],
    'freeze_main': True,
    'target_global': None,
    'scratch_size': 4 * _TBL_STRIDE,
    'scratch_fill': 0xA5,
    'path1_tests': _TESTS,
    'path2_tests': [_TESTS[5], _TESTS[13], _TESTS[7]],
}
