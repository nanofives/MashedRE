"""negctl re-earn, batch b02 (log/negctl_reearn/batches/b02.tsv). Decomp/disasm cited per entry.

All warm-lane fixtures use scalars_to_scattered_globals with per-test seeds, freeze_main, and either a folded
return or pre-filled observe windows the body overwrites.
"""

FIXES = {}

# ---------------------------------------------------------------------------------------------------------------
# Team-aggregating score getters 0x00423dd0 .. 0x004241c0. Shape (decomp):
#   [gate rows only] if (FUN_0042f6a0() == 4) return 0;          FUN_0042f6a0 = return DAT_0067e9fc
#   if (FUN_0042f500() == 0) return base[idx*0x4e];               FUN_0042f500 = return DAT_0067ea64
#   t = DAT_007f1a18[idx*4]; r = base[idx*0x4e];
#   for j in 0..3: if (DAT_007f1a18[j*4] == t && idx != j [&& DAT_007f1a14[j*4] != -1]) r += base[j*0x4e];
# The old int_scalar vectors read the live menu state (DAT_0067ea64 = 0, all fields 0), so every call returned 0.
# Seed team mode, team ids, the -1 flags and four distinct per-player fields, and fold the return.
_PSTRIDE = 0x138   # 0x4e dwords
_VALS = (0x11, 0x220, 0x3300, 0x44000)


def _score_test(base, idx, team_mode, teams, submode=0, flags=(0, 0, 0, 0)):
    s = [{'addr': 0x0067ea64, 'val': team_mode}, {'addr': 0x0067e9fc, 'val': submode}]
    for j in range(4):
        s.append({'addr': 0x007f1a18 + 0x10 * j, 'val': teams[j]})
        s.append({'addr': 0x007f1a14 + 0x10 * j, 'val': flags[j] & 0xFFFFFFFF})
        s.append({'addr': base + _PSTRIDE * j, 'val': _VALS[j]})
    return {'args': [idx], 'seed': s}


def _score_fix(base, gated, flagged):
    t = []
    for idx in range(4):
        t.append(_score_test(base, idx, 0, (0, 0, 1, 1)))
        t.append(_score_test(base, idx, 1, (0, 0, 1, 1)))
        t.append(_score_test(base, idx, 1, (5, 5, 5, 5)))
    t.append(_score_test(base, 1, 1, (0, 1, 0, 1)))
    t.append(_score_test(base, 2, 1, (7, 3, 7, 7)))
    if gated:
        t.append(_score_test(base, 0, 1, (5, 5, 5, 5), submode=4))
        t.append(_score_test(base, 3, 0, (0, 0, 1, 1), submode=4))
        t.append(_score_test(base, 0, 1, (5, 5, 5, 5), submode=3))
    if flagged:
        t.append(_score_test(base, 0, 1, (5, 5, 5, 5), flags=(0, -1, 0, -1)))
        t.append(_score_test(base, 3, 1, (5, 5, 5, 5), flags=(-1, 0, -1, 0)))
        t.append(_score_test(base, 2, 1, (5, 5, 5, 5), flags=(-1, -1, -1, -1)))
    return {
        'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['int32']},
        'fold_ret': True, 'observe': [], 'freeze_main': True,
        'path1_tests': t, 'path2_tests': t[:3],
    }


FIXES['player_score_team_acc_gated_c'] = _score_fix(0x00899a48, gated=True, flagged=False)   # 0x00423dd0
FIXES['player_score_team_acc_cum_b'] = _score_fix(0x00899f7c, gated=False, flagged=False)    # 0x00423e60
FIXES['player_block2_field00_get'] = _score_fix(0x00899f20, gated=False, flagged=False)      # 0x00423ee0
FIXES['player_block2_field04_get'] = _score_fix(0x00899f24, gated=True, flagged=False)       # 0x00423f60
FIXES['player_block2_field50_get'] = _score_fix(0x00899f70, gated=False, flagged=False)      # 0x00423ff0
FIXES['player_block2_field08_get'] = _score_fix(0x00899f28, gated=True, flagged=False)       # 0x00424070
FIXES['team_block_zero_get'] = _score_fix(0x008999a0, gated=False, flagged=True)             # 0x00424100
FIXES['team_block_one_get'] = _score_fix(0x00899e80, gated=False, flagged=True)              # 0x004241c0

# ---------------------------------------------------------------------------------------------------------------
# Timer dispatchers. Disasm:
#   0x00426c10: call 0x41ea40; test; je ret; push 0x646e58; call 0x41e920; add esp,4; jmp 0x480100
#   0x00426c30: call 0x41ea30; test; je ret; mov [esp+4],0x646e58; jmp 0x41e910
#   0x00426c70: call 0x41ea50; test; je ret; mov [esp+4],0x646e58; jmp 0x41e930
# Gates return *(DAT_0063d7e4 + 0x30/0x2c/0x34); actions jump through the SAME slot (mov ecx,[0x63d7e4]; jmp [ecx+off]).
# At the menu the slot was 0, so the body never dispatched. Point DAT_0063d7e4 at a scratch record whose slot holds
# 0x004769f0 (disasm: mov eax,[esp+4]; DAT_00692554 = eax ? *eax : *0x00613290; ret), so the dispatched call
# copies *(0x00646e58) into 0x00692554 only when the 0x00646e58 arg reached it.
# The c30/c70 originals write [esp+4] of their caller, so both get one dummy stack arg (cdecl, ignored by the port).
_SET_COLOUR = 0x004769f0


def _disp_test(off, on, arg_val=0x5EED1234):
    return {'args': [0], 'seed': [
        {'addr': 0x0063d7e4, 'val': {'scratch': 0}},
        {'addr': {'scratch': off}, 'val': _SET_COLOUR if on else 0},
        {'addr': 0x00646e58, 'val': arg_val},
        {'addr': 0x00613290, 'val': 0x0BADC0DE}]}


def _disp_fix(off, extra_obs=()):
    t = [_disp_test(off, 1), _disp_test(off, 0), _disp_test(off, 1, 0x80FF00FF), _disp_test(off, 1, 0)]
    return {
        'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['uint32']},
        'observe': [{'addr': 0x00692554, 'len': 4, 'fill': 0xA5}] + list(extra_obs),
        'scratch_size': 0x40, 'freeze_main': True, 'crash_equal_ok': None,
        'path1_tests': t, 'path2_tests': t[:1],
    }


FIXES['timer_dispatch_30'] = _disp_fix(0x2c)   # 0x00426c30
FIXES['timer_dispatch_70'] = _disp_fix(0x34)   # 0x00426c70
# 0x00426c10 also tail-jumps to FUN_00480100. That loop (200 slots) writes *(0x006c74f8 + 0x18 + i*0x28) = -1,
# DAT_006c3030[i] = 0, DAT_006c3350[i] = 0, and runs the slot tear-down only when DAT_006c6eb8[i] != 0. Zero
# DAT_006c6eb8 (fill 0) so the tear-down never runs, pre-fill the three written arrays, observe all of them.
FIXES['timer_dispatch_10'] = _disp_fix(0x30, extra_obs=(
    {'addr': 0x006c6eb8, 'len': 0x320, 'fill': 0x00},
    {'addr': 0x006c3030, 'len': 0x320, 'fill': 0xA5},
    {'addr': 0x006c3350, 'len': 0x320, 'fill': 0xA5},
    {'addr': 0x006c74f8, 'len': 0x1f40, 'fill': 0xA5},
))
FIXES['timer_dispatch_10']['signature'] = {'ret': 'void', 'args': []}

# ---------------------------------------------------------------------------------------------------------------
# 0x00428610 ViewportScaledRectDraw: NOT re-earned (no fixture kept). The field_first mutant flipped 0x007d709c,
# outside draw_quad_observe's VBUF. Observing that dword through scalars_to_scattered_globals made the
# original-vs-original baseline RED 12/12 (iterations 1-2: window 0x007d7090+0x20, then 0x007d709c+4); VBUF alone is
# GREEN/GREEN but leaves the flip unseen. Row demoted.

# ---------------------------------------------------------------------------------------------------------------
# 0x00429aa0 GameStateSlotsFill. Disasm: if (DAT_0067ed6c != 0) { i = DAT_0067f17c;
#   fld [i*0xc + 0x614718]; call 0x4a2c48 (ftol of ST0) -> DAT_0067d990; fld [i*0xc + 0x61471c]; ftol -> DAT_0067d998;
#   fld [i*0xc + 0x614720]; fstp DAT_0067d9a0 } else { DAT_0067d990 = 0x7f0db4[i]; DAT_0067d998 = 0x7f0de8[i];
#   fld/fstp 0x7f0e1c[i] -> DAT_0067d9a0 }.
# The old vectors left the predicate/index at their menu values and the targets already equal to the sources.
import struct as _st


def _f(x):
    return _st.unpack('<I', _st.pack('<f', x))[0]


def _gsf(pred, idx, a, b, c):
    s = [{'addr': 0x0067ed6c, 'val': pred}, {'addr': 0x0067f17c, 'val': idx}]
    if pred:
        s += [{'addr': 0x00614718 + idx * 0xc, 'val': _f(a)}, {'addr': 0x0061471c + idx * 0xc, 'val': _f(b)},
              {'addr': 0x00614720 + idx * 0xc, 'val': _f(c)}]
    else:
        s += [{'addr': 0x007f0db4 + idx * 4, 'val': int(a)}, {'addr': 0x007f0de8 + idx * 4, 'val': int(b)},
              {'addr': 0x007f0e1c + idx * 4, 'val': _f(c)}]
    return {'args': [], 'seed': s}


_GSF = [_gsf(0, 0, 0x1234, 0x5678, 1.5), _gsf(0, 3, 7, 9, 2.25), _gsf(1, 0, 3.0, 45.0, 0.5),
        _gsf(1, 2, 12.0, 7.0, 0.25), _gsf(1, 1, 2.5, 59.0, 0.75)]
FIXES['game_state_slots_fill'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x0067d990, 'len': 4}, {'addr': 0x0067d998, 'len': 4}, {'addr': 0x0067d9a0, 'len': 4}],
    'pre_fill_byte': 0xA5, 'freeze_main': True,
    'path1_tests': _GSF, 'path2_tests': _GSF[:1],
}
