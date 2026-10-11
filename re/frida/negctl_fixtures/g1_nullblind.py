"""negctl re-earn, group 1: the 12 rows a null port passed (re/analysis/negctl/NEGCTL_20261011.md, group D).

Every fixture here drives the body past its early-out and makes its effect visible: observed globals are pre-filled
with a sentinel the body overwrites, and returns are folded into the fingerprint. Decomp cited per entry.
"""
import struct

def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]

FIXES = {}

# 0x00405400 / 0x004298c0 / 0x0045c860 / 0x004842b0: zero-clears. The warm-lane globals were already 0, so a port
# that did nothing matched. Pre-fill 0xFF (the early-window lane's original evidence used the same fill).
for _k in ('clear_639d70_x3', 'clear_67d99c_x4', 'clear_88f0a0_x4', 'clear_4842b0'):
    FIXES[_k] = {'pre_fill_byte': 0xFF}

# 0x00429870 LapTimeALessThanB: (laps_a*0x3c + secs_a + frac_a) < (laps_b*0x3c + secs_b + frac_b), globals
# 0x0067d98c/994/99c (A) and 0x0067d990/998/9a0 (B). The old read_global vectors seeded only laps_a, so every
# call returned 0 and a null port (EAX = 0) passed.
def _lap(la, sa, fa, lb, sb, fb):
    return {'args': [], 'seed': [
        {'addr': 0x0067d98c, 'val': la}, {'addr': 0x0067d994, 'val': sa}, {'addr': 0x0067d99c, 'val': _f(fa)},
        {'addr': 0x0067d990, 'val': lb}, {'addr': 0x0067d998, 'val': sb}, {'addr': 0x0067d9a0, 'val': _f(fb)}]}
FIXES['lap_time_a_less_than_b'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': []},
    'fold_ret': True, 'observe': [],
    'target_global': None,
    'path1_tests': [_lap(1, 10, 0.5, 1, 11, 0.0), _lap(2, 0, 0.0, 1, 59, 0.9), _lap(0, 30, 0.25, 0, 30, 0.75),
                    _lap(0, 30, 0.75, 0, 30, 0.25), _lap(3, 3, 0.0, 3, 3, 0.0), _lap(0, 0, 0.0, 9, 0, 0.0)],
    'path2_tests': [_lap(1, 10, 0.5, 1, 11, 0.0)],
}

# 0x004b4140 RenderElemArrayCopyAll: FUN_004b4130 -> FUN_004b40c0(p[0x18], 0); FUN_004b40c0 reads count =
# *(s+0x24) and returns it in EAX (port note, Render/ElemArrayCopyAll.cpp). The entry declared ret void and
# observe_ret False, so the forwarded count was never compared.
FIXES['render_elem_array_copy_all'] = {
    'signature': {'ret': 'int', 'args': ['pointer']}, 'observe_ret': True,
}

# 0x0041e9b0 TrackNodeFieldCmp10: bool FUN_0041e9b0(int param_1) { return *(int*)(DAT_0063d7e4 + 0x10) == param_1; }
# The entry declared zero args, so param_1 was leftover stack and every call returned 0. Point DAT_0063d7e4 at a
# harness record and pass param_1.
def _cmp(arg, field):
    return {'args': [arg], 'seed': [{'addr': 0x0063d7e4, 'val': {'scratch': 0}},
                                   {'addr': {'scratch': 0x10}, 'val': field}]}
FIXES['track_node_field_cmp10'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['int32']},
    'fold_ret': True, 'observe': [], 'scratch_size': 0x40,
    'field_offset': None, 'is_getter': None,
    'path1_tests': [_cmp(5, 5), _cmp(5, 6), _cmp(0, 0), _cmp(0xDEADBEEF, 0xDEADBEEF), _cmp(1, 0), _cmp(-1, -1 & 0xFFFFFFFF)],
    'path2_tests': [_cmp(5, 5)],
}

# 0x004769f0 ParticleEmitter_SetColour: DAT_00692554 = param_1 ? *param_1 : DAT_00613290. All ten old vectors were
# NULL, and DAT_00613290 equals the harness sentinel, so the write was invisible. Seed the default colour, pass a
# real pointer too, pre-fill the target.
def _col(use_ptr, dflt, val):
    return {'args': [{'scratch': 0} if use_ptr else 0],
            'seed': [{'addr': 0x00613290, 'val': dflt}, {'addr': {'scratch': 0}, 'val': val}]}
FIXES['particle_emitter_set_colour'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': 0x00692554, 'len': 4}], 'pre_fill_byte': 0xA5, 'scratch_size': 0x10,
    'target_global': None,
    'path1_tests': [_col(0, 0x11223344, 0), _col(1, 0x11223344, 0x55667788), _col(1, 0, 0x80FF00FF),
                    _col(0, 0xFFFFFFFF, 0x12345678)],
    'path2_tests': [_col(1, 0x11223344, 0x55667788)],
}

# 0x00411d60 ReplayCheckTimer: if (FUN_0042f6a0() == 2 && DAT_0063bb14 != 0 && *(int*)(DAT_0063bb14+0x194) == 0)
# DAT_007f0ff4 = 0. FUN_0042f6a0 returns DAT_0067e9fc. At the menu the sub-mode is not 2, so the body never wrote.
def _rct(mode, rec, flag):
    s = [{'addr': 0x0067e9fc, 'val': mode},
         {'addr': 0x0063bb14, 'val': {'scratch': 0} if rec else 0},
         {'addr': {'scratch': 0x194}, 'val': flag}]
    return {'args': [], 'seed': s}
FIXES['replay_check_timer'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x007f0ff4, 'len': 4}], 'pre_fill_byte': 0xA5, 'scratch_size': 0x200,
    'target_global': None,
    'path1_tests': [_rct(2, 1, 0), _rct(2, 1, 1), _rct(2, 0, 0), _rct(1, 1, 0), _rct(2, 1, 0)],
    'path2_tests': [_rct(2, 1, 0)],
}

# 0x00422120 TimerInitLoop: for 4 elements at 0x0063fb90 (stride 0x208) call FUN_00421c50 (EAX = element), which
# runs FUN_00421c10 on two lanes (element, element+0x100). FUN_00421c10: for 10 slots at lane+0x10 (stride 0x18)
# call FUN_00421980 when slot != 0 and *(slot+0x24) != 0; then lane+0xf4 = 0, lane+0xf0 = 0. The old fixture read
# only 0x0063fb90 (+0, never written). Observe the 16 cleared dwords (pre-filled), and zero the 80 slot pointers so
# the FUN_00421980 tear-down never runs on the live game.
_LANES = [0x0063fb90 + e * 0x208 + l * 0x100 for e in range(4) for l in range(2)]
_TIL_OBS = [{'addr': b + 0xf0, 'len': 8} for b in _LANES]
_TIL_SEED = [{'addr': b + 0x10 + i * 0x18, 'val': 0} for b in _LANES for i in range(10)]
FIXES['timer_init_loop'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': _TIL_OBS, 'pre_fill_byte': 0xA5, 'target_global': None,
    'path1_tests': [{'args': [], 'seed': _TIL_SEED}] * 3,
    'path2_tests': [{'args': [], 'seed': _TIL_SEED}],
}

# 0x00552d70 ViewportStackPop: if (DAT_00912b04 <= 0) return; DAT_00912b04 -= 1; DAT_00912bd8 = 0; DAT_00912bec = 0.
# Depth is 0 at the menu, so the body always took the early return.
def _vsp(depth):
    return {'args': [], 'seed': [{'addr': 0x00912b04, 'val': depth & 0xFFFFFFFF}]}
FIXES['viewport_stack_pop'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x00912b04, 'len': 4}, {'addr': 0x00912bd8, 'len': 4}, {'addr': 0x00912bec, 'len': 4}],
    'pre_fill_byte': 0xA5, 'target_global': None,
    'path1_tests': [_vsp(3), _vsp(1), _vsp(0), _vsp(-1), _vsp(0x7fffffff)],
    'path2_tests': [_vsp(3)],
}
