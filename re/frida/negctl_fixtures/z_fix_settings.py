"""negctl re-earn fix, cluster settings (2026-10-12). Loads last; overrides b04 / registry entries by hook key.

All five rows act only through USER32 calls on dialog-control HWNDs (GetDlgItem / SendMessageA / SetWindowTextA /
SetFocus / EndDialog). With hDlg = NULL every call is a no-op returning 0, so each ABI defect in the cluster (combo
HWND read from EAX instead of the stack at 0x00498dd9, SetControlText's HWND from EAX instead of [esp+0x108] at
0x00499778, FUN_00498d60 called __fastcall from 0x00498f60 / 0x00499170) read GREEN. These fixtures use the
`dialog` option of scalars_to_scattered_globals (diff_template.js, added 2026-10-12): a real hidden STATIC popup with
the dialog-0x65 control ids as WS_CHILD windows is created on the A/B thread before each arm, an arg {dialog: id}
resolves to its HWND, and the fingerprint folds a readback of every control (text, BM_GETCHECK, combo items + item
data + selection, focused id). `eax_arg` delivers hDlg in EAX for 0x00498f60 / 0x00499170 (0x00498f64 / 0x00499179
mov esi,eax).

Mode tables live in the harness scratch: DAT_007731f8 -> int counts[2], DAT_00773408 -> modes*[2] (0x18-byte records
{w, h, depth, flags}), DAT_007731fc -> 0x50-stride subsystem names. Every flags word is nonzero: the deployed
reference carries the show_windowed patch at 0x00498dbc (75 44 -> 90 90), so the flags == 0 filter cannot be diffed.

0x00499170 reaches FUN_004c2e70 (sets the RW subsystem on the live engine) and FUN_004c2f00: both are replaced with
`stub_at` (the first records its argument at scratch+0x700, the second returns 3).
"""

FIXES = {}

_SCRATCH = 0x800
_REC = 0x700

_CONTROLS = [
    {'id': 1, 'cls': 'BUTTON', 'style': 0, 'text': 'b1'},
    {'id': 2, 'cls': 'BUTTON', 'style': 0, 'text': 'b2'},
    {'id': 0x41a, 'cls': 'STATIC', 'style': 0, 'text': 'lbl'},
    {'id': 0x417, 'cls': 'BUTTON', 'style': 3, 'text': 'cA'},                    # BS_AUTOCHECKBOX, unchecked
    {'id': 0x418, 'cls': 'BUTTON', 'style': 3, 'text': 'cB', 'check': 1},
    {'id': 0x419, 'cls': 'BUTTON', 'style': 3, 'text': 'cC', 'check': 1},
    {'id': 0x41b, 'cls': 'BUTTON', 'style': 3, 'text': 'cD'},
    {'id': 0x3e8, 'cls': 'COMBOBOX', 'style': 0x200003,                          # CBS_DROPDOWNLIST | WS_VSCROLL
     'items': [['subA', 0], ['subB', 1], ['subC', 1]], 'sel': 2},
    {'id': 0x3e9, 'cls': 'COMBOBOX', 'style': 0x200003, 'items': [['m0', 7], ['m1', 9]], 'sel': 1},
]
_DIALOG = {'controls': _CONTROLS}

# {w, h, depth, flags}: mixes the width (signed jl 0x280, 0x00498dc4) and height (jl 0x1e0, 0x00498dcd) filters,
# a negative width, and flag bits that pick different FormatDisplayModeString suffixes (0x00498a3a..0x00498b0b).
_MODES0 = [(640, 480, 16, 1), (320, 240, 16, 1), (800, 600, 32, 2), (1024, 768, 32, 3), (639, 480, 16, 1),
           (640, 479, 16, 1), (0xFFFFFD80, 480, 16, 1), (1280, 1024, 32, 0x101), (1600, 1200, 32, 4),
           (800, 400, 16, 0x200)]
_MODES1 = [(1024, 768, 16, 1), (640, 480, 32, 0x100), (512, 384, 16, 1), (1152, 864, 32, 2)]
_NAMES = ['Direct3D9 HAL', 'Reference Rasterizer', 'Sub Three']


def _str(off, s):
    b = s.encode('ascii') + b'\0'
    b += b'\0' * (-len(b) % 4)
    return [{'addr': {'scratch': off + i}, 'val': int.from_bytes(b[i:i + 4], 'little')} for i in range(0, len(b), 4)]


def _tables(sub, n0=len(_MODES0), n1=len(_MODES1), nsub=3, cur_mode=0, chk=(0, 0, 0, 0)):
    s = [{'addr': 0x007731f8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0}, 'val': n0 & 0xffffffff},
         {'addr': {'scratch': 4}, 'val': n1 & 0xffffffff},
         {'addr': 0x00773408, 'val': {'scratch': 0x10}}, {'addr': {'scratch': 0x10}, 'val': {'scratch': 0x40}},
         {'addr': {'scratch': 0x14}, 'val': {'scratch': 0x200}},
         {'addr': 0x0077340c, 'val': sub & 0xffffffff},
         {'addr': 0x007731fc, 'val': {'scratch': 0x400}}, {'addr': 0x00773410, 'val': nsub & 0xffffffff},
         {'addr': 0x00773200, 'val': cur_mode},
         {'addr': 0x007733a8, 'val': chk[0]}, {'addr': 0x007733a4, 'val': chk[1]},
         {'addr': 0x007733ac, 'val': chk[2]}, {'addr': 0x007733b0, 'val': chk[3]}]
    for base, modes in ((0x40, _MODES0), (0x200, _MODES1)):
        for k, m in enumerate(modes):
            for j, v in enumerate(m):
                s.append({'addr': {'scratch': base + k * 0x18 + j * 4}, 'val': v})
    for k, nm in enumerate(_NAMES):
        s += _str(0x400 + k * 0x50, nm)
    return s


_MAP = {'addr': 0x00773418, 'len': 0x40, 'fill': 0xA5}

_BASE = {'arg_type': 'scalars_to_scattered_globals', 'dialog': _DIALOG, 'scratch_size': _SCRATCH,
         'scratch_fill': 0, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
         'pre_fill_byte': None, 'fold_ret': None}


def _fix(**kw):
    d = dict(_BASE)
    d.update(kw)
    return d


# ── 0x00498d60 PopulateModeCombo(HWND hCombo), __cdecl ─────────────────────────────────────────────────────────────
def _pm(hwnd_id, sub, **kw):
    return {'args': [{'dialog': hwnd_id}], 'seed': _tables(sub, **kw)}


FIXES['populate_mode_combo_s2'] = _fix(
    signature={'ret': 'void', 'args': ['pointer']}, observe=[_MAP],
    path1_tests=[_pm(0x3e9, 0), _pm(0x3e9, 1), _pm(0x3e9, 0, n0=0), _pm(0x3e9, 0, n0=-1), _pm(0x3e9, 0, n0=3),
                 _pm(0x3e9, 1, n1=2), _pm(0x3e8, 0, n0=5), _pm(0x3e8, 1)],
    path2_tests=[_pm(0x3e9, 0)])

# ── 0x00499740 SetControlTextFromResource(HWND, UINT id), __cdecl ──────────────────────────────────────────────────
# Ids from MASHED.exe's RT_STRING table (2000..2305): 2000 'OK', 2013 'Cancelar', 2020 'Select Video Mode',
# 2021 (non-ASCII), 2110, 2121 (non-ASCII), 2240, 2251. An id with no string would leave the stack buffer
# uninitialised, so none is used.
def _st(hwnd_id, sid):
    return {'args': [{'dialog': hwnd_id}, sid]}


FIXES['set_control_text_from_resource_i3'] = _fix(
    signature={'ret': 'void', 'args': ['pointer', 'uint32']}, observe=[],
    path1_tests=[_st(1, 2000), _st(0, 2020), _st(0x417, 2251), _st(0x41a, 2121), _st(2, 2013), _st(0x3e8, 2110),
                 _st(0x419, 2021), _st(0x41b, 2240)],
    path2_tests=[_st(1, 2000)])

# ── 0x00498f60 VideoDialogInit(EAX = hDlg) ─────────────────────────────────────────────────────────────────────────
def _vi(sub, nsub, cur_mode, chk, **kw):
    return {'args': [{'dialog': 0}], 'seed': _tables(sub, nsub=nsub, cur_mode=cur_mode, chk=chk, **kw)}


FIXES['video_dialog_init_i3'] = _fix(
    signature={'ret': 'void', 'args': ['pointer']}, eax_arg=0, observe=[_MAP],
    path1_tests=[_vi(0, 3, 3, (0, 1, 0, 5)), _vi(1, 2, 0, (1, 0, 7, 0)), _vi(0, 0, 1, (0, 0, 0, 0)),
                 _vi(1, 3, 2, (2, 2, 2, 2)), _vi(0, 1, 7, (0, 0, 1, 1), n0=8)],
    path2_tests=[_vi(0, 3, 3, (0, 1, 0, 5))])

# ── 0x00499170 SubsystemSelectionChanged(EAX = hDlg) ───────────────────────────────────────────────────────────────
# The 0x3e8 combo is created with three items (data 0, 1, 1) and selection 2.
_STUBS = [{'addr': 0x004c2e70}, {'addr': 0x004c2f00, 'bare': True, 'nargs': 0, 'ret': 3, 'abi': 'mscdecl'}]
_RECORDER = {'args': 1, 'abi': 'mscdecl', 'off': _REC, 'max_calls': 4}
_SS_OBS = [{'addr': 0x0077340c, 'len': 4}, {'addr': 0x00773200, 'len': 4, 'fill': 0xA5}, _MAP,
           {'addr': {'scratch': _REC}, 'len': 0x14}]


def _ss(hwnd_id, sub, **kw):
    return {'args': [{'dialog': hwnd_id} if hwnd_id is not None else 0], 'seed': _tables(sub, **kw)}


FIXES['subsystem_selection_changed_i3'] = _fix(
    signature={'ret': 'void', 'args': ['pointer']}, eax_arg=0, observe=_SS_OBS, stub_at=_STUBS,
    recorder=_RECORDER,
    path1_tests=[_ss(0, 2), _ss(0, 0), _ss(0, 5), _ss(0, -1), _ss(0, 0, n1=1), _ss(None, 0), _ss(None, 1)],
    path2_tests=[_ss(0, 0)])

# ── 0x004991f0 VideoSettingsDlgProc(HWND, UINT, WPARAM, LPARAM), __stdcall (ret 0x10) ──────────────────────────────
# b04's 13 hDlg = NULL vectors, plus the same messages on the real tree (0x417 unchecked, 0x418/0x419 checked,
# 0x41b unchecked), the 0x3e8 arm with and without a notification code (reaches 0x00499170, stubs as above),
# WM_INITDIALOG (reaches 0x00498f60), IDOK/IDCANCEL with hi 0 and hi 1, and 0x3e9 with hi 0 and hi 1.
# The re-port returns with ret 0x10, so the port arm is called __stdcall too (b04 used mscdecl for the old export).
def _dlg(h, msg, wp, sub=0):
    return {'args': [{'dialog': 0} if h else 0, msg, wp, 0], 'seed': _tables(sub, chk=(0xA5A5A5A5,) * 4)}


_DP_OBS = [{'addr': 0x007733a4, 'len': 0x10, 'fill': 0xA5}, {'addr': 0x00773200, 'len': 4, 'fill': 0xA5},
           {'addr': 0x0077340c, 'len': 4}, _MAP, {'addr': {'scratch': _REC}, 'len': 0x14}]
_B04 = [(0x111, 0x417), (0x111, 0x418), (0x111, 0x419), (0x111, 0x41b), (0x111, 0x103e9), (0x111, 1), (0x111, 2),
        (0x111, 0x3eb), (0x111, 0x3ea), (0x111, 0x500), (0x112, 0x417), (0x111, 0x10417), (0x111, 0x5041b)]
_REAL = [(0x111, 0x417), (0x111, 0x418), (0x111, 0x419), (0x111, 0x41b), (0x111, 0x10417), (0x111, 0x30419),
         (0x111, 0x5041b), (0x111, 0x3e8), (0x111, 0x103e8), (0x111, 0x3e9), (0x111, 0x103e9), (0x111, 1),
         (0x111, 0x10001), (0x111, 2), (0x111, 0x10002), (0x110, 0), (0x111, 0x3ed), (0x111, 0x41a)]
FIXES['video_settings_dlg_proc_s2'] = _fix(
    signature={'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32', 'uint32']},
    orig_calling_convention='stdcall', reimpl_calling_convention='stdcall', fold_ret=True,
    observe=_DP_OBS, stub_at=_STUBS, recorder=_RECORDER,
    path1_tests=[_dlg(False, m, w) for m, w in _B04] + [_dlg(True, m, w) for m, w in _REAL],
    path2_tests=[_dlg(True, 0x111, 0x417)])
