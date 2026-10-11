"""negctl re-earn, batch b01 (log/negctl_reearn/batches/b01.tsv).

Each fixture moves the row onto scalars_to_scattered_globals, seeds the globals the body reads, pre-fills the globals it
writes, and folds the return, so a port that does nothing (null) or returns a wrong EAX (ret) reads RED. freeze_main
on every entry: every seeded global here is a live game global. Decomp cited per entry (decomp_pc.py, 2026-10-11).
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def _str_seeds(addr, s):
    """4-byte seeds spelling NUL-terminated `s` at `addr` (addr may be a {'scratch': off} dict)."""
    b = s.encode('ascii') + b'\0'
    b += b'\0' * (-len(b) % 4)
    out = []
    for i in range(0, len(b), 4):
        a = {'scratch': addr['scratch'] + i} if isinstance(addr, dict) else addr + i
        out.append({'addr': a, 'val': struct.unpack('<I', b[i:i + 4])[0]})
    return out


_SC = {'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'target_global': None,
       'crash_equal_ok': None, 'field_offset': None, 'is_getter': None}


def _fix(**kw):
    d = dict(_SC)
    d.update(kw)
    return d


FIXES = {}

# 0x00407a20 GetAiLapCounter: return (&DAT_008a9648)[param_1 * 0xc3] (stride 0x30c). The table is zero at the menu.
_AI = [0x11110001, 0x22220002, 0x33330003, 0x44440004, 0x55550005, 0x66660006, 0x77770007, 0x88880008]
_AI_SEED = [{'addr': 0x008a9648 + i * 0x30c, 'val': v} for i, v in enumerate(_AI)]
FIXES['get_ai_lap_counter'] = _fix(
    signature={'ret': 'int', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [i], 'seed': _AI_SEED} for i in range(8)],
    path2_tests=[{'args': [3], 'seed': _AI_SEED}])

# 0x0040b6c0 FrontendArrayGet: return (&DAT_008a94f0)[param_1].
# 0x0040b6d0 UtilArrayGet8a94e0: return (&DAT_008a94e0)[param_1].  Seed 0x008a94dc..0x008a9518 with distinct values.
_ARR_SEED = [{'addr': 0x008a94dc + 4 * k, 'val': 0xA0B00000 + k * 0x1013} for k in range(16)]
FIXES['frontend_array_get'] = _fix(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [i], 'seed': _ARR_SEED} for i in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1)],
    path2_tests=[{'args': [2], 'seed': _ARR_SEED}])
FIXES['util_array_get_8a94e0'] = _fix(
    signature={'ret': 'int', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [i], 'seed': _ARR_SEED} for i in (0, 1, 2, 3, 4, 5, 6, 7, 8, -1)],
    path2_tests=[{'args': [3], 'seed': _ARR_SEED}])

# 0x0040dd60 GuardConcludedAndP1Won: return (DAT_0063b90c != 1) - 1 & DAT_007f0fcc.
def _g(flag, won):
    return {'args': [], 'seed': [{'addr': 0x0063b90c, 'val': flag}, {'addr': 0x007f0fcc, 'val': won}]}
FIXES['guard_concluded_and_p1_won'] = _fix(
    signature={'ret': 'uint32', 'args': []}, fold_ret=True, observe=[],
    path1_tests=[_g(1, 1), _g(1, 0x12345678), _g(0, 0x12345678), _g(2, 1), _g(1, 0), _g(0xFFFFFFFF, 0xFFFFFFFF)],
    path2_tests=[_g(1, 0x12345678)])

# 0x00423cc0 PlayerScoreTeamAccGatedB: FUN_0042f6a0() (= DAT_0067e9fc) == 4 -> 0; FUN_0042f500() (= DAT_0067ea64) == 0
# -> (&DAT_00899a44)[i*0x4e]; else own value plus the value of every other slot k whose team (&DAT_007f1a18)[k*4]
# equals slot i's team.  0x00423d50 PlayerScoreTeamAccBaseB: same without the mode gate, base DAT_00899a90.
def _ps(base, mode, teamflag, teams, i, gated):
    s = [{'addr': 0x0067ea64, 'val': teamflag}]
    if gated:
        s.append({'addr': 0x0067e9fc, 'val': mode})
    for k, (v, t) in enumerate(zip((101, 2002, 30003, 400004), teams)):
        s.append({'addr': base + k * 0x138, 'val': v})
        s.append({'addr': 0x007f1a18 + k * 0x10, 'val': t})
    return {'args': [i], 'seed': s}
for _key, _base, _gated in (('player_score_team_acc_gated_b', 0x00899a44, True),
                            ('player_score_team_acc_base_b', 0x00899a90, False)):
    _t = []
    for _i in range(4):
        _t.append(_ps(_base, 0, 0, (0, 1, 2, 3), _i, _gated))
        _t.append(_ps(_base, 0, 1, (0, 0, 1, 1), _i, _gated))
        _t.append(_ps(_base, 0, 1, (7, 7, 7, 7), _i, _gated))
    _t.append(_ps(_base, 4, 1, (7, 7, 7, 7), 2, _gated))
    _t.append(_ps(_base, 4, 0, (0, 1, 2, 3), 1, _gated))
    FIXES[_key] = _fix(signature={'ret': 'int', 'args': ['int32']}, fold_ret=True, observe=[],
                       path1_tests=_t, path2_tests=[_t[4]])

# 0x0040bb70 / 0x0040bb90 SpriteLookupTableA/B: return FUN_004c5c00(DAT_0063b900 / DAT_0063b904, key).
# FUN_004c5c00 walks the list at dict+8 (next at node+8), compares the name at node+0x10 case-insensitively
# ('a'..'z' folded), returns node or 0. The live dictionaries held no entry named by the old .rdata keys, so every
# call returned 0. Build a 3-node dict in scratch and point the global at it.
def _dict_seeds(gaddr):
    s = [{'addr': gaddr, 'val': {'scratch': 0x00}},
         {'addr': {'scratch': 0x08}, 'val': {'scratch': 0x48}},      # dict.head -> T1.link
         {'addr': {'scratch': 0x48}, 'val': {'scratch': 0x88}},      # T1 -> T2
         {'addr': {'scratch': 0x88}, 'val': {'scratch': 0xc8}},      # T2 -> T3
         {'addr': {'scratch': 0xc8}, 'val': {'scratch': 0x08}}]      # T3 -> head
    s += _str_seeds({'scratch': 0x50}, 'Wheel')
    s += _str_seeds({'scratch': 0x90}, 'BADGE_01')
    s += _str_seeds({'scratch': 0xd0}, 'wheel2')
    for off, key in ((0x100, 'wheel'), (0x110, 'badge_01'), (0x120, 'WHEEL2'), (0x130, 'whee'), (0x140, 'zzz'),
                     (0x150, 'Wheel22')):
        s += _str_seeds({'scratch': off}, key)
    return s
for _key, _g_addr in (('sprite_lookup_table_a', 0x0063b900), ('sprite_lookup_table_b', 0x0063b904)):
    _s = _dict_seeds(_g_addr)
    FIXES[_key] = _fix(
        signature={'ret': 'uint32', 'args': ['pointer']}, fold_ret=True, observe=[], scratch_size=0x200,
        path1_tests=[{'args': [{'scratch': o}], 'seed': _s} for o in (0x100, 0x110, 0x120, 0x130, 0x140, 0x150)],
        path2_tests=[{'args': [{'scratch': 0x110}], 'seed': _s}])

# 0x00409900 LoadingState2Enter: wprintf; DAT_008a9584 = 2; DAT_008a958c = 1; DAT_008a95b0 = 0.
# The old void_write_observe entry observed 0x008a9584 only; observe all three, pre-filled.
FIXES['loading_state2_enter'] = _fix(
    signature={'ret': 'void', 'args': []},
    observe=[{'addr': 0x008a9584, 'fill': 0xA5}, {'addr': 0x008a958c, 'fill': 0xA5}, {'addr': 0x008a95b0, 'fill': 0xA5},
             # printf 0x004a2cbd output: stdout FILE at 0x00616110 and the bytes it buffers at 0x007739f8 (the
             # first-run field_first mutant flipped 0x007739f8). Not filled (no pre_fill_byte); saved/restored per arm.
             {'addr': 0x00616110, 'len': 0x20}, {'addr': 0x007739f8, 'len': 0x20}],
    path1_tests=[{'args': []}] * 3, path2_tests=[{'args': []}])

# 0x004099a0 AutosaveTrigger: if (DAT_008a95ac != 0) { wprintf; DAT_008a9584 = 4; DAT_008a9594 = 1; DAT_008a95b0 = 0; }
# Void (the old 'uint32' ret compared a leftover EAX). DAT_008a95ac is 0 at the menu, so the body never ran.
def _as(g):
    return {'args': [], 'seed': [{'addr': 0x008a95ac, 'val': g}]}
FIXES['autosave_trigger'] = _fix(
    signature={'ret': 'void', 'args': []},
    observe=[{'addr': 0x008a9584}, {'addr': 0x008a9594}, {'addr': 0x008a95b0}], pre_fill_byte=0xA5,
    path1_tests=[_as(1), _as(0), _as(0x8a0000), _as(0)], path2_tests=[_as(1)])

# 0x0040de00 thunk -> 0x004117b0: if (DAT_005f29c8 == 0) { if (DAT_0063bb10 != 0 && FUN_00430820() (= DAT_0067ed6c)
# == 0) { save replay ... }; DAT_005f29c8 = 1; }. The latch was already 1 at the menu. Seed it 0 and keep the
# replay-save branch closed (DAT_0063bb10 = 0, or DAT_0067ed6c != 0), so the only write is the latch.
def _rs(latch, rec, gate):
    return {'args': [], 'seed': [{'addr': 0x005f29c8, 'val': latch},
                                 {'addr': 0x0063bb10, 'val': {'scratch': 0} if rec else 0},
                                 {'addr': 0x0067ed6c, 'val': gate}]}
FIXES['thunk_replay_save'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x40,
    observe=[{'addr': 0x005f29c8}, {'addr': 0x008a9584}],
    path1_tests=[_rs(0, 0, 0), _rs(0, 1, 1), _rs(1, 0, 0), _rs(2, 0, 0), _rs(0, 1, 0x55)],
    path2_tests=[_rs(0, 0, 0)])

# 0x00404e50 SaveLoad: FUN_004b3b70("gamesave.bin", &DAT_00803358, 0x24fa0); FUN_004099e0(0) (DAT_008a95a0 = 0);
# return 0. Pre-fill the 0x24fa0-byte buffer and DAT_008a95a0: the read overwrites the buffer with the file bytes.
FIXES['save_load'] = _fix(
    signature={'ret': 'int32', 'args': []}, fold_ret=True,
    observe=[{'addr': 0x008a95a0}, {'addr': 0x00803358, 'len': 0x24fa0}], pre_fill_byte=0xA5,
    path1_tests=[{'args': []}] * 2, path2_tests=[{'args': []}])

# 0x00404f50 SaveWrite: FUN_004b3bb0("gamesave.bin", &DAT_00803358, 0x24fa0); FUN_004099e0(0); return 0.
# The buffer is NOT pre-filled (it is written to disk). Observable: DAT_008a95a0 (pre-filled) and the return.
FIXES['save_write'] = _fix(
    signature={'ret': 'int32', 'args': []}, fold_ret=True,
    observe=[{'addr': 0x008a95a0}], pre_fill_byte=0xA5,
    path1_tests=[{'args': []}] * 2, path2_tests=[{'args': []}])

# 0x00403160 Sub00403160_SubMode0BViewport: cam = FUN_004671a0(0); ... if (FUN_004c1a00(cam) != 0) {
# (*(DAT_007d3ff8 + 0x20))(6,0); (8,0); FUN_00402fb0() (DAT_00636afc += DAT_005cc56c = 0.1f); ... }.
# DAT_00636afc is the observable; DAT_00771964 = 0 keeps FUN_00428760 out.
# Run 1 (DAT_006905b0 left live) faulted on BOTH arms ("access violation accessing 0x60"): FUN_004671a0(0) returns
# DAT_006905b0 (when FUN_0042b930() != 3), which is no usable camera at the menu. Point it at a scratch camera whose
# +0x18 (FUN_004c1a00 target) and +0x1c (FUN_004c19f0 target) are 0x004099e0 (DAT_008a95a0 = arg; EAX = arg != 0),
# so the begin test passes, +0x04 = 0 (FUN_004c1c80 skips FUN_004c0e50), +0x68/+0x6c = 1.0f.
def _vp(v):
    return {'args': [], 'seed': [{'addr': 0x00636afc, 'val': _f(v)}, {'addr': 0x00771964, 'val': 0},
                                 {'addr': 0x006905b0, 'val': {'scratch': 0}},
                                 {'addr': {'scratch': 0x18}, 'val': 0x004099e0},
                                 {'addr': {'scratch': 0x1c}, 'val': 0x004099e0},
                                 {'addr': {'scratch': 0x68}, 'val': _f(1.0)},
                                 {'addr': {'scratch': 0x6c}, 'val': _f(1.0)}]}
FIXES['sub_00403160_submode0b_viewport'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x100,
    observe=[{'addr': 0x00636afc}, {'addr': 0x008a95a0, 'fill': 0xA5}, {'addr': {'scratch': 0x68}, 'len': 0x10}],
    path1_tests=[_vp(v) for v in (0.0, 1.0, 2.5)], path2_tests=[_vp(0.0)])

# 0x0041db80 -> body 0x0041dab0 (EAX = 0x0063d55c): counter c = float DAT_0063d588; thresholds .rdata
# 0x005cd238 = 0.34, 0x005cd234 = 0.82, 0x005cd230 = 1.3, 0x005ccdf4 = 1.86, 0x005cc35c = 4.0.
#   c < 4.0: call obj[0], obj[5] (0x0063d55c, 0x0063d570)
#   c >= 0.34: c < 0.82: obj[1]; 0.82..1.3: obj[1], obj[2]; 1.3..1.86: obj[1..3]; then 1.86 <= c < 4.0: obj[4]
# Each call is (*(obj+0x48))(obj), cdecl (caller cleans: add esp at 0x0041dad0 etc.). Point every obj at a
# scratch record whose +0x48 is an original leaf with a visible write:
#   0x00552d70 (if DAT_00912b04 > 0: DAT_00912b04--, DAT_00912bd8 = DAT_00912bec = 0) counts calls,
#   0x004099e0 (DAT_008a95a0 = arg) records the last object pointer, 0x004769f0 (DAT_00692554 = *arg) the last id.
_DISP_FN = {0: 0x00552d70, 1: 0x00552d70, 2: 0x004769f0, 3: 0x004769f0, 4: 0x004099e0, 5: 0x004099e0}
def _hd(c):
    s = [{'addr': 0x0063d588, 'val': _f(c)}, {'addr': 0x00912b04, 'val': 100}]
    for k, fn in _DISP_FN.items():
        o = 0x40 * k
        s += [{'addr': 0x0063d55c + 4 * k, 'val': {'scratch': o}},
              {'addr': {'scratch': o}, 'val': 0xC0DE0000 + k},
              {'addr': {'scratch': o + 0x48}, 'val': fn}]
    return {'args': [], 'seed': s}
FIXES['sub_0041db80_hud_threshold_dispatch'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x200,
    observe=[{'addr': 0x00912b04}, {'addr': 0x00912bd8}, {'addr': 0x00912bec},
             {'addr': 0x008a95a0}, {'addr': 0x00692554}], pre_fill_byte=0xA5,
    path1_tests=[_hd(c) for c in (0.0, 0.5, 1.0, 1.5, 2.5, 5.0, 0.34, 4.0)],
    path2_tests=[_hd(1.5)])
