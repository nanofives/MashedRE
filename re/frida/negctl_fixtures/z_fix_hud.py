"""negctl re-earn, cluster hud (reearn/hud, 2026-10-12). Loads last, so these keys override b01/b02/b03/b07.

Rows re-ported from the bytes in this branch: 0x00403160, 0x0041db80, 0x0041ded0, 0x004295a0, 0x00429aa0, 0x0043c000.
Every vector cites the instruction it exercises. Ports: HUD/HudBatch.cpp, Frontend/BatchAA_s1.cpp, Util/UtilMid.cpp.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


_SC = {'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'target_global': None,
       'crash_equal_ok': None, 'field_offset': None, 'is_getter': None}


def _fix(**kw):
    d = dict(_SC)
    d.update(kw)
    return d


FIXES = {}

# ── 0x00403160 Sub00403160_SubMode0BViewport ─────────────────────────────────────────────────────────────────────────
# Bytes: 0x004031b6 mov eax,[0x007d3ff8]; push 0; push 6; call [eax+0x20] (one dereference), then (8,0), FUN_00402fb0,
# FUN_00428760(flag, 50,30,240,120, 0) iff [0x00771964] (0x004031d7), (6,1), (8,1), FUN_004c19f0(cam).
# b01's vector faulted on both arms at 0x0 inside FUN_00427e00 (text draw, reached from FUN_00402fb0 at
# 0x0040301b/0x0040303b): its font globals are not live at the menu. Harness:
#   * DAT_007d3ff8 -> scratch+0x100, scratch+0x120 = recorder (2 args, cdecl: add esp,0x10 at 0x004031cf), so the
#     render-state calls are recorded. FUN_004671a0 / FUN_004c19f0 / FUN_004c1a00 / FUN_004c1c80 do not read
#     DAT_007d3ff8 (disasm 0x004671a0, 0x004c19f0, 0x004c1a00, 0x004c1c80).
#   * stub_at 0x00427e00 and 0x00428760 with the same recorder (both cdecl, caller cleans: 0x00403040 add esp,0x38,
#     0x004031fc add esp,0x18), so their first two args land in the same log, in call order.
#   * scratch camera (as b01): +0x1c = 0x004099e0 (end-update), +0x18 = 0x004099e0 (begin, EAX != 0) or
#     0x0040dca5 (xor eax,eax; ret: begin fails, 0x004031b4 je 0x00403221).
_VP_REC = 0x180


def _vp(v, flag=0, begin=0x004099e0):
    s = [{'addr': 0x00636afc, 'val': _f(v)}, {'addr': 0x00771964, 'val': flag},
         {'addr': 0x006905b0, 'val': {'scratch': 0}},
         {'addr': {'scratch': 0x18}, 'val': begin},
         {'addr': {'scratch': 0x1c}, 'val': 0x004099e0},
         {'addr': {'scratch': 0x68}, 'val': _f(1.0)},
         {'addr': {'scratch': 0x6c}, 'val': _f(1.0)},
         {'addr': 0x007d3ff8, 'val': {'scratch': 0x100}},
         {'addr': {'scratch': 0x120}, 'val': {'recorder': True}}]
    return {'args': [], 'seed': s}


FIXES['sub_00403160_submode0b_viewport'] = _fix(
    signature={'ret': 'int', 'args': []}, fold_ret=True, scratch_size=0x200,
    recorder={'args': 2, 'abi': 'mscdecl', 'off': _VP_REC, 'max_calls': 8},
    stub_at=[0x00427e00, 0x00428760],
    observe=[{'addr': 0x00636afc}, {'addr': 0x008a95a0, 'fill': 0xA5}, {'addr': {'scratch': 0x68}, 'len': 0x10},
             {'addr': {'scratch': _VP_REC}, 'len': 4 + 8 * 2 * 4}],
    path1_tests=[_vp(0.0), _vp(1.0), _vp(2.5), _vp(0.5, flag=0x12345678), _vp(1.0, begin=0x0040dca5)],
    path2_tests=[_vp(0.0)])

# ── 0x0041db80 -> body 0x0041dab0 ────────────────────────────────────────────────────────────────────────────────────
# As b01 (thresholds 0x005cd238 0.34, 0x005cd234 0.82, 0x005cd230 1.3, 0x005ccdf4 1.86, 0x005cc35c 4.0) plus the exact
# thresholds 0.82 / 1.3 / 1.86, a negative counter and a NaN (unordered: block 1 skipped at 0x0041dac1 jp, block 2
# entered at 0x0041dae1 jnp, every inner jp taken, tail exits at 0x0041db5c on C0).
_DISP_FN = {0: 0x00552d70, 1: 0x00552d70, 2: 0x004769f0, 3: 0x004769f0, 4: 0x004099e0, 5: 0x004099e0}


def _hd(cbits):
    s = [{'addr': 0x0063d588, 'val': cbits}, {'addr': 0x00912b04, 'val': 100}]
    for k, fn in _DISP_FN.items():
        o = 0x40 * k
        s += [{'addr': 0x0063d55c + 4 * k, 'val': {'scratch': o}},
              {'addr': {'scratch': o}, 'val': 0xC0DE0000 + k},
              {'addr': {'scratch': o + 0x48}, 'val': fn}]
    return {'args': [], 'seed': s}


_HD = [_f(c) for c in (0.0, 0.5, 1.0, 1.5, 2.5, 5.0, 0.34, 4.0)] + \
      [0x3f51eb85, 0x3fa66666, 0x3fee147b, _f(-1.0), 0x7fc00000]
FIXES['sub_0041db80_hud_threshold_dispatch'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x200,
    observe=[{'addr': 0x00912b04}, {'addr': 0x00912bd8}, {'addr': 0x00912bec},
             {'addr': 0x008a95a0}, {'addr': 0x00692554}], pre_fill_byte=0xA5,
    path1_tests=[_hd(c) for c in _HD], path2_tests=[_hd(_f(1.5))])

# ── 0x00429aa0 GameStateSlotsFill ────────────────────────────────────────────────────────────────────────────────────
# As b02, plus: fold the return (EAX at 0x00429af0 / 0x00429b26 = third FUN_00430790 idx), FUN_004a2c48 on a negative
# fraction and on 3.0e9 (above 2^31: the low dword of the 64-bit conversion), and a signalling-NaN third float on both
# paths (0x00429ae3 / 0x00429b19 fld+fstp quiet it; a dword copy would not).
def _gsf(pred, idx, a, b, c):
    s = [{'addr': 0x0067ed6c, 'val': pred}, {'addr': 0x0067f17c, 'val': idx}]
    if pred:
        s += [{'addr': 0x00614718 + idx * 0xc, 'val': a}, {'addr': 0x0061471c + idx * 0xc, 'val': b},
              {'addr': 0x00614720 + idx * 0xc, 'val': c}]
    else:
        s += [{'addr': 0x007f0db4 + idx * 4, 'val': a}, {'addr': 0x007f0de8 + idx * 4, 'val': b},
              {'addr': 0x007f0e1c + idx * 4, 'val': c}]
    return {'args': [], 'seed': s}


_GSF = [_gsf(0, 0, 0x1234, 0x5678, _f(1.5)), _gsf(0, 3, 7, 9, _f(2.25)), _gsf(1, 0, _f(3.0), _f(45.0), _f(0.5)),
        _gsf(1, 2, _f(12.0), _f(7.0), _f(0.25)), _gsf(1, 1, _f(2.5), _f(59.0), _f(0.75)),
        _gsf(1, 1, _f(-2.7), _f(3.0e9), 0x7f800001), _gsf(0, 2, 0xdeadbeef, 0x01020304, 0x7f800001)]
FIXES['game_state_slots_fill'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x0067d990, 'len': 4}, {'addr': 0x0067d998, 'len': 4}, {'addr': 0x0067d9a0, 'len': 4}],
    'pre_fill_byte': 0xA5, 'freeze_main': True,
    'path1_tests': _GSF, 'path2_tests': _GSF[:1],
}

# ── 0x0043c000 TimerSlotTickDispatcher ───────────────────────────────────────────────────────────────────────────────
# As b03, plus: fold the return (EAX = last slot's ftol byte source or its state dword, 0x0043c55c), state 1 on the 14
# slots without an action (the four action slots 0x0067e7a8/7c8/7d8/7e0 read live input and stay 0), x = 0.5
# ((1-0.5)*255 = 127.5, truncated at 0x004a2c48), x = -0.0 (equal to [0x005d757c] 0.0: reset path), x = NaN
# (unordered: 0x0043c01a jp skips the reset).
_ACTION_IDX = {0, 4, 6, 7}


def _tsd(xbits, states):
    s = [{'addr': 0x0067e840, 'val': xbits}]
    for i, st in enumerate(states):
        s.append({'addr': 0x0067e7a8 + i * 8, 'val': st})
    return {'args': [], 'seed': s}


_ONE = [0 if i in _ACTION_IDX else 1 for i in range(19)]
_MIX = [0 if i in _ACTION_IDX else (1, 2, 3)[i % 3] for i in range(19)]
FIXES['timer_slot_tick_dispatcher'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x0067e7a8, 'len': 0x98}, {'addr': 0x0067e840, 'len': 4}], 'pre_fill_byte': 0xA5,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_tsd(_f(0.0), [0] * 19), _tsd(_f(0.5), [0] * 19), _tsd(_f(0.5), [2] * 19),
                    _tsd(_f(0.25), [5] * 19), _tsd(_f(0.0), [2, 0, 5] * 6 + [2]),
                    _tsd(_f(0.3), _ONE), _tsd(_f(0.5), _MIX), _tsd(0x80000000, _MIX), _tsd(0x7fc00000, _MIX),
                    _tsd(_f(0.0), _ONE)],
    'path2_tests': [_tsd(_f(0.5), [0] * 19)],
}

# ── 0x004295a0 HudDualLabelRender ────────────────────────────────────────────────────────────────────────────────────
# Bytes: FUN_0040dc80 = fld [0x0063b910]; ret (no args). FUN_00427e00 gets six args twice:
#   0x004295ed (0xff, y + [0x005cc31c] 3.0f, 63.0f, 0xff000000, 1.0f, 2)
#   0x00429614 (0xff, y, 60.0f, 0xff0000ff, 1.0f, 2)
# FUN_00427e00 draws text and faults at the menu (font globals 0x0067d838/0x0067d83c not live), so stub it with a
# 6-arg cdecl recorder (0x004295f6 add esp,0x18 / 0x00429619 add esp,0x20: caller cleans) and seed DAT_0063b910.
# EAX at ret (0x0042961c) = the second FUN_00427e00 return = the recorder's 0.
_DL_REC = 0x40


def _dl(ybits):
    return {'args': [], 'seed': [{'addr': 0x0063b910, 'val': ybits}]}


FIXES['hud_dual_label_render'] = _fix(
    signature={'ret': 'uint32', 'args': []}, fold_ret=True, scratch_size=0x100,
    recorder={'args': 6, 'abi': 'mscdecl', 'off': _DL_REC, 'max_calls': 4},
    stub_at=[0x00427e00],
    observe=[{'addr': {'scratch': _DL_REC}, 'len': 4 + 4 * 6 * 4}],
    path1_tests=[_dl(_f(v)) for v in (0.0, 100.0, -3.0, 0.1, 3.0e38, 1.0e-45)] + [_dl(0x7fc00000)],
    path2_tests=[_dl(_f(100.0))])
