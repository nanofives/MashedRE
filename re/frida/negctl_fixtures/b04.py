"""negctl re-earn, batch b04 (boot / settings-config / settings-dialog rows).

Fixtures only for rows whose body can be driven at the menu and whose effect lands in memory the harness can read.
Rows demoted without a fixture (no fixture here): 0x00492270 (re-runs RwEngineInit/Open/Start), 0x004963e0 and
0x00496400 (original body neutralised by the disable_log boot patch), 0x00498d20 / 0x00498d60 / 0x00498f60 /
0x00499170 / 0x00499740 (need a real dialog HWND, and/or port ABI defects), 0x004a4541 (port returns a FILE* from
the DLL's CRT, install refused by the fix_fopen boot-patch guard). See log/negctl_reearn/dec/b04.tsv.
"""

FIXES = {}

# ── 0x00493f80 IntroVideoDimGetter(u32 *p1, u32 *p2) ─────────────────────────────────────────────────────────────────
# if (p1) { p1[0] = DAT_007719ec; p1[1] = DAT_007719f0; } if (p2) { p2[0] = DAT_007719f4; p2[1] = DAT_007719f8; }
# Old vectors were all (NULL, NULL): every arm took both early-outs. Pass harness buffers (pre-filled 0xA5) and seed
# the four source globals with distinct values.
def _dim(p1, p2, a, b, c, d):
    return {'args': [{'scratch': 0} if p1 else 0, {'scratch': 0x10} if p2 else 0],
            'seed': [{'addr': 0x007719ec, 'val': a}, {'addr': 0x007719f0, 'val': b},
                     {'addr': 0x007719f4, 'val': c}, {'addr': 0x007719f8, 'val': d}]}
FIXES['intro_video_dim_getter'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['pointer', 'pointer']},
    'observe': [{'addr': {'scratch': 0}, 'len': 0x20}], 'scratch_size': 0x20, 'scratch_fill': 0xA5,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_dim(1, 1, 640, 480, 0x11223344, 0x55667788), _dim(1, 0, 1, 2, 3, 4), _dim(0, 1, 5, 6, 7, 8),
                    _dim(1, 1, 0x3f800000, 0x40000000, 0xFFFFFFFF, 0), _dim(0, 0, 9, 9, 9, 9)],
    'path2_tests': [_dim(1, 1, 640, 480, 0x11223344, 0x55667788)],
}

# ── 0x00498950 ConfigLoad / 0x004989b0 ConfigSave ─────────────────────────────────────────────────────────────────────
# Load: name = FUN_00498910() (copies "videocfg.bin" to 0x007731e8); f = FUN_004a4541(name, "rb");
#       if (f) { _fread(0x00773208, 1, 0x200, f); _fclose(f); return 1; } return 0.
# Save: same with "wb" and _fwrite(0x00773208, 1, 0x200, f).
# The warm-lane run was nontrivial (ret 1) but field_first/field_last flipped 0x007739f8, a byte no window observed.
# Observe 0x007739f8 (not filled), the filename buffer and (load) the 0x200 settings buffer (both pre-filled 0xA5 so the body's
# writes are visible); restored after each arm, so the live settings and videocfg.bin keep their bytes (Save writes
# the restored, real buffer). Fold the return.
_CFG_OBS_LOAD = [{'addr': 0x007731e8, 'len': 0x10, 'fill': 0xA5}, {'addr': 0x00773208, 'len': 0x200, 'fill': 0xA5},
                 {'addr': 0x007739f8, 'len': 4}]
FIXES['config_load'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': []},
    'fold_ret': True, 'pre_fill_byte': None, 'observe': _CFG_OBS_LOAD, 'freeze_main': True,
    'target_global': None,
    'path1_tests': [{'args': []}] * 4, 'path2_tests': [{'args': []}],
}
FIXES['config_save'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': []},
    'fold_ret': True, 'pre_fill_byte': None, 'freeze_main': True, 'target_global': None,
    'observe': [{'addr': 0x007731e8, 'len': 0x10, 'fill': 0xA5}, {'addr': 0x007739f8, 'len': 4}],
    'path1_tests': [{'args': []}] * 4, 'path2_tests': [{'args': []}],
}

# ── 0x00492770 MainLoopInit ───────────────────────────────────────────────────────────────────────────────────────────
# DAT_00828300 = 0; FUN_00495110() (DAT_007f1030 = clock, not observed); DAT_007f1000 = DAT_007f101c = DAT_007f1020 = 0;
# DAT_00771968 = 1; FUN_00433240(0x16) (no-op while DAT_0067eca4 != 0); FUN_0042b950() (DAT_007f1a0c = 0x1000);
# DAT_007e9dc0 = FUN_004c57a0(); return 1. Old entry: void_write_observe on 0x00828300 only, ret not folded (ret mutant
# BLIND), field_first hit 0x007e9dc0 (unobserved). Observe every written global pre-filled 0xA5 (DAT_0067eca4 stays
# nonzero so FUN_00433240 keeps its early-out) and fold the return.
# DAT_007e9dc0 is NOT observed: FUN_004c57a0 returns a fresh RW allocation per call (run 1 baseline RED 4/4 with it).
# Freeze the game thread (it reads DAT_00771968).
FIXES['main_loop_init'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': []},
    'fold_ret': True, 'pre_fill_byte': 0xA5, 'freeze_main': True, 'target_global': None,
    'observe': [{'addr': 0x00828300, 'len': 4}, {'addr': 0x007f1000, 'len': 4}, {'addr': 0x007f101c, 'len': 8},
                {'addr': 0x00771968, 'len': 4}, {'addr': 0x007f1a0c, 'len': 4},
                {'addr': 0x0067eca4, 'len': 4}],
    'path1_tests': [{'args': []}] * 4, 'path2_tests': [{'args': []}],
}

# ── 0x004b3b70 FileReadWrapper_i3(const char *name, void *buf, u32 size) ──────────────────────────────────────────────
# h = FUN_004cc230(2, 1, name); if (!h) return 0; n = FUN_004cbd30(h, buf, size); FUN_004cc160(h, 0); return n.
# The entry declared zero args, so name/buf/size were leftover stack and the open failed on every arm. Pass a real
# file name (videocfg.bin, 0x200 bytes in the game dir) and a harness buffer pre-filled 0xA5; fold the byte count.
def _str(off, s):
    b = s.encode('ascii') + b'\0'
    b += b'\0' * (-len(b) % 4)
    return [{'addr': {'scratch': off + i}, 'val': int.from_bytes(b[i:i + 4], 'little')} for i in range(0, len(b), 4)]
def _frw(name, size):
    return {'args': [{'scratch': 0}, {'scratch': 0x40}, size], 'seed': _str(0, name)}
FIXES['file_read_wrapper_i3'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['pointer', 'pointer', 'uint32']},
    'fold_ret': True, 'scratch_size': 0x400, 'scratch_fill': 0xA5, 'freeze_main': True, 'target_global': None,
    'crash_equal_ok': None,
    'observe': [{'addr': {'scratch': 0x40}, 'len': 0x300}],
    'path1_tests': [_frw('videocfg.bin', 0x200), _frw('videocfg.bin', 0x10), _frw('videocfg.bin', 0x300),
                    _frw('videocfg.bin', 0), _frw('negctl_b04_nofile.bin', 0x40)],
    'path2_tests': [_frw('videocfg.bin', 0x200)],
}

# ── 0x004991f0 VideoSettingsDlgProc(HWND, UINT msg, WPARAM, LPARAM), __stdcall (ret 0x10) ───────────────────────────
# WM_COMMAND (0x111) on LOWORD(wParam): 0x417/0x418/0x41b -> DAT_007733a8/a4/b0 = (BM_GETCHECK == 0);
# 0x419 -> DAT_007733ac = BM_GETCHECK; 1 (HIWORD 0) -> FUN_00498d20 (DAT_00773200) + EndDialog; 0x3e9 (HIWORD 1) ->
# FUN_00498d20; 0x3eb..0x3ed -> 1; others -> 0. The entry declared zero args, so msg was leftover stack and every
# arm returned 0. hDlg = NULL makes GetDlgItem/SendMessageA return 0, so each handled control writes a fixed value.
# The checkbox jump-table targets (0x004992d0 ..) do NOT test HIWORD(wParam): the last two vectors carry a nonzero
# notification code.
def _dlg(msg, wp):
    return {'args': [0, msg, wp, 0]}
FIXES['video_settings_dlg_proc_s2'] = {
    'arg_type': 'scalars_to_scattered_globals',
    'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32', 'uint32']},
    'orig_calling_convention': 'stdcall', 'reimpl_calling_convention': 'mscdecl',
    'fold_ret': True, 'pre_fill_byte': 0xA5, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'observe': [{'addr': 0x007733a4, 'len': 0x10}, {'addr': 0x00773200, 'len': 4}],
    'path1_tests': [_dlg(0x111, 0x417), _dlg(0x111, 0x418), _dlg(0x111, 0x419), _dlg(0x111, 0x41b),
                    _dlg(0x111, 0x103e9), _dlg(0x111, 1), _dlg(0x111, 2), _dlg(0x111, 0x3eb), _dlg(0x111, 0x3ea),
                    _dlg(0x111, 0x500), _dlg(0x112, 0x417), _dlg(0x111, 0x10417), _dlg(0x111, 0x5041b)],
    'path2_tests': [_dlg(0x111, 0x417)],
}
