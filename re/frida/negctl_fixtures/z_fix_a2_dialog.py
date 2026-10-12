"""negctl follow-up a2-dialog (2026-10-12): 0x00498d20 ReadModeFromCombo_j5 and 0x0042f8d0 MenuMenusBC.

Both take an argument in a REGISTER, which is what kept them blind:

* 0x00498d20 takes hDlg in EAX (0x00498d27 push eax, before any other write to EAX). The old entry was arg_type
  'none' + crash_equal_ok, so GetDlgItem/SendMessageA saw no window and DAT_00773200 was 0 on every vector.
* 0x0042f8d0 takes the panel alpha in AL (0x0042f8d9 mov bl,al). b09 drove x/y/w/h but could not set AL, so neither
  colour (0x0042f8e7 shr cl,1 -> fill 0x{a>>1}146ef0; 0x0042f92f -> border 0x{a}1050b4) was observed.

Both now go through `eax_arg` (diff_template.js): `mov eax,[esp+4+4k]; jmp target`. That trampoline ALSO leaves the
value on the stack, so a port that read the argument from the stack instead of the register would read the right
value and pass. Each signature therefore carries one DECOY stack slot ahead of the register value, holding a value
the register-reading original never sees:

* 0x00498d20: args (decoy, hDlg), eax_arg 1. [esp+4] at entry = decoy, EAX = hDlg.
* 0x0042f8d0: args (x, y, w, h, decoy, alpha), eax_arg 5. [esp+0x14] at entry = decoy (where a 5-stack-arg port
  would look for alpha), EAX = alpha with garbage in the upper 24 bits on some vectors (only AL is read).

0x00498d20 observable: DAT_00773200 (pre-filled 0xA5), the return value (fold_ret; EAX at 0x00498d54 = the
CB_GETITEMDATA result), and the dialog readback. Each vector carries its own `dialog` tree (per-test override added
to scalars_to_scattered_globals for this fixture), so the selection / item data differ per vector. A decoy 0x3e8 combo
holds different item data, so a port using the wrong control id reads a different value.

0x0042f8d0 observable: every argument of every call to FUN_00472c60 (five direct E8 calls, 0x0042f911 / 0x0042f950 /
0x0042f978 / 0x0042f9a0 / 0x0042f9e9; cdecl, 5 dwords each, caller cleans: 0x0042f9b1 add esp,0x50 for four calls,
0x0042f9ee add esp,0x14 for the fifth). 0x00472c60 is replaced by the scalars_to_scattered_globals `recorder`
through `stub_at`, so nothing reaches RwGlobals and the full x/y/w/h/colour of all five quads, both colours
included, is in the fingerprint. The port calls 0x00472c60 through g_menuBc_Quad (an absolute address), so both arms
hit the same recorder.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

# ── 0x00498d20 ReadModeFromCombo_j5 ──────────────────────────────────────────────────────────────────────────────────
_CB = 0x200003                                   # CBS_DROPDOWNLIST | WS_VSCROLL (as z_fix_settings)
_DECOY_COMBO = {'id': 0x3e8, 'cls': 'COMBOBOX', 'style': _CB, 'items': [['d0', 0x111], ['d1', 0x222], ['d2', 0x333]],
                'sel': 1}


def _dlg(items, sel):
    c = {'id': 0x3e9, 'cls': 'COMBOBOX', 'style': _CB, 'items': items}
    if sel is not None:
        c['sel'] = sel
    return {'controls': [_DECOY_COMBO, c]}


_ITEMS2 = [['m0', 7], ['m1', 9]]
_ITEMS3 = [['x0', 0xDEADBEEF], ['x1', 0x80000000], ['x2', 0]]
_ITEMS10 = [['k%d' % k, 0x1000 + k * 0x11] for k in range(10)]


def _rm(dlg, decoy, hdlg):
    # args = (decoy on the stack, hDlg in EAX)
    return {'args': [decoy, hdlg], 'seed': [], 'dialog': dlg}


_P = {'dialog': 0}            # the parent window: GetDlgItem(parent, 0x3e9) finds the combo
_C = {'dialog': 0x3e9}        # the combo itself: it has no child 0x3e9, so GetDlgItem returns NULL
_D = {'dialog': 0x3e8}        # the decoy combo: no child 0x3e9 either

FIXES['read_mode_from_combo_j5'] = {
    '__replace__':    True,
    'rva':            0x00498d20,
    'export':         'ReadModeFromCombo_j5',
    'arg_type':       'scalars_to_scattered_globals',
    'signature':      {'ret': 'int32', 'args': ['uint32', 'uint32']},
    'eax_arg':        1,
    'fold_ret':       True,
    'freeze_main':    True,
    'dialog':         _dlg(_ITEMS2, 1),
    'observe':        [{'addr': 0x00773200, 'len': 4, 'fill': 0xA5}],
    'lut_root_delta': 0,
    'path1_tests': [
        _rm(_dlg(_ITEMS2, 1), 0, _P),             # data 9
        _rm(_dlg(_ITEMS2, 0), 0, _P),             # data 7
        _rm(_dlg(_ITEMS2, None), 0, _P),          # no selection: CB_GETCURSEL = CB_ERR, CB_GETITEMDATA(-1) = CB_ERR
        _rm(_dlg(_ITEMS3, 0), 0, _P),             # 0xDEADBEEF
        _rm(_dlg(_ITEMS3, 1), 0, _P),             # 0x80000000
        _rm(_dlg(_ITEMS3, 2), _P, _P),            # data 0, decoy = parent too
        _rm(_dlg([], None), 0, _P),               # empty combo
        _rm(_dlg(_ITEMS10, 7), 0, _P),            # 0x1077
        _rm(_dlg(_ITEMS10, 9), _C, _P),           # 0x1099, decoy = the combo
        _rm(_dlg(_ITEMS2, 1), _P, _C),            # EAX = combo: GetDlgItem NULL -> 0; the stack holds the parent
        _rm(_dlg(_ITEMS2, 1), _P, 0),             # EAX = NULL -> 0; the stack holds the parent
        _rm(_dlg(_ITEMS10, 3), _P, _D),           # EAX = decoy combo -> 0; the stack holds the parent
    ],
    'path2_tests': [_rm(_dlg(_ITEMS2, 1), 0, _P)],
}


# ── 0x0042f8d0 MenuMenusBC(x, y, w, h; alpha in AL) ──────────────────────────────────────────────────────────────────
_REC_OFF = 0
_REC_MAX = 6                                     # five calls expected; a sixth would show as count 6


def _bc(x, y, w, h, eax, decoy=0x33):
    return {'args': [x, y, w, h, decoy, eax], 'seed': []}


FIXES['menu_menus_bc'] = {
    '__replace__':    True,
    'rva':            0x0042f8d0,
    'export':         'MenuMenusBC',
    'arg_type':       'scalars_to_scattered_globals',
    'signature':      {'ret': 'void', 'args': ['float', 'float', 'float', 'float', 'uint32', 'uint32']},
    'eax_arg':        5,
    'freeze_main':    True,
    'scratch_size':   0x100,
    'scratch_fill':   0,
    'recorder':       {'args': 5, 'abi': 'mscdecl', 'off': _REC_OFF, 'max_calls': _REC_MAX},
    'stub_at':        [{'addr': 0x00472c60}],
    'observe':        [{'addr': {'scratch': _REC_OFF}, 'len': 4 + _REC_MAX * 5 * 4}],
    'lut_root_delta': 0,
    'path1_tests': [
        _bc(100.0, 50.0, 200.0, 30.0, 0xff),
        _bc(100.0, 50.0, 200.0, 30.0, 0x80),
        _bc(100.0, 50.0, 200.0, 30.0, 0x00, decoy=0xff),
        _bc(0.0, 0.0, 640.0, 480.0, 0x01),
        _bc(12.5, -7.25, 3.0, 4.0, 0x7f),
        _bc(300.0, 400.0, 1.0, 1.0, 0xDEADBE81),            # only AL (0x81) is read
        _bc(-20.0, 33.3, 0.1, 1.0e-7, 0x123456fe),           # h - 2.0 rounds; 0x0042f98d fst keeps it unrounded in ST0
        _bc(1.0e7, 0.3, 77.7, 3.0000002, 0x000000c3, decoy=0xc4),
        _bc(185.0, 140.0, 120.0, 28.0, 0xff, decoy=0x00),   # ability/team plate geometry (hooks.csv 0x0043a610)
    ],
    'path2_tests': [_bc(100.0, 50.0, 200.0, 30.0, 0xff)],
}
