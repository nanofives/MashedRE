"""negctl re-earn, batch b08 (world render end, raycast forwarder, frontend text/measure, lap getters, readiness and
button detectors, 1P slot init, chrome shell B).

Decomp from re/tools/decomp_pc.py (pool slot, read-only) and re/tools/disasm_va.py. Every entry freezes the game's
main thread: the globals seeded below (input arrays, menu screen, lap arrays, RW plugin offset, font/colour context,
RwGlobals) are live.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}
_SCAL = 'scalars_to_scattered_globals'
_RET = 0x0042b8b6          # bare `ret` (cdecl stand-in, caller pops), as in b03.menu_chrome_shell_a


# ── shared: input button arrays (same layout as b03) ────────────────────────────────────────────────────────────────
def _btn(col, held, prev):
    s = []
    for k in range(8):
        s.append({'addr': 0x007f1040 + col + k * 0x4c, 'val': 1 if (held >> k) & 1 else 0, 'size': 1})
        s.append({'addr': 0x007f1500 + col + k * 0x4c, 'val': 1 if (prev >> k) & 1 else 0, 'size': 1})
    return s


def _slots(ids):
    return [{'addr': 0x007f1a14 + i * 0x10, 'val': v & 0xFFFFFFFF} for i, v in enumerate(ids)]


# ── 0x004266b0 WorldRenderDispatch_End(p) ───────────────────────────────────────────────────────────────────────────
# if (DAT_0066d704 != 0) FUN_004e4350(DAT_0066d700 ? DAT_00656ee8 : DAT_0065742c, p); FUN_004e4350(h, p2):
# p2 += DAT_007d716c; if (*(p2+0xc) != 0) { *(p2+0xc) = 0; *(p2+8) = 0; }. Old vectors: guard 0 at the menu.
# Seed the guard, DAT_007d716c = 0, pass a scratch record whose +8/+0xc the body clears.
def _wre(guard, sel, f8, fc, off=0):
    return {'args': [{'scratch': off}],
            'seed': [{'addr': 0x0066d704, 'val': guard}, {'addr': 0x0066d700, 'val': sel},
                     {'addr': 0x007d716c, 'val': 0},
                     {'addr': {'scratch': off + 8}, 'val': f8}, {'addr': {'scratch': off + 0xc}, 'val': fc}]}


FIXES['world_render_dispatch_end'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': {'scratch': 0}, 'len': 0x40}], 'scratch_size': 0x40, 'scratch_fill': 0xA5,
    'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
    'path1_tests': [_wre(1, 0, 0x11111111, 0x22222222), _wre(1, 1, 0x33333333, 0x44444444, 0x10),
                    _wre(1, 0, 0x55555555, 0, 0x20), _wre(0, 0, 0x66666666, 0x77777777),
                    _wre(1, 1, 0, 1, 0x30)],
    'path2_tests': [_wre(1, 0, 0x11111111, 0x22222222)],
}


# ── 0x00426dc0 FrontendRaycastForward(ray, out, frac) ───────────────────────────────────────────────────────────────
# Disasm 0x00426dc0: pushes the 3 stack args + 0x00646e58, calls FUN_00479100, returns its EAX. FUN_00479100 queries
# the collision object *(0x00646e58 + 0x10090) = DAT_00656ee8; on a hit it writes *frac and (when the hit material
# has +0x84 != 0) *out, returning 1. Old vectors were zero-arg calls.
# Iteration 1 (menu): both sides AV at 0x1c inside FUN_00538c80 (the collision object is NULL at the menu).
# Iteration 2: run in the race scenario.
def _ray(sx, sy, sz, ex, ey, ez):
    seed = [{'addr': {'scratch': i * 4}, 'val': _f(v)} for i, v in enumerate((sx, sy, sz, ex, ey, ez))]
    return {'args': [{'scratch': 0}, {'scratch': 0x40}, {'scratch': 0x44}], 'seed': seed}


FIXES['frontend_raycast_forward'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'pointer']}, 'fold_ret': True,
    'observe': [{'addr': {'scratch': 0x40}, 'len': 8}], 'scratch_size': 0x80, 'scratch_fill': 0xA5,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None, 'lut_root_delta': None, 'scenario': 'race',
    'path1_tests': [_ray(0, 1000, 0, 0, -1000, 0), _ray(10, 500, 10, 10, -500, 10), _ray(-50, 200, 30, -50, -200, 30),
                    _ray(0, 5, 0, 0, 4, 0), _ray(100, 1000, -100, 100, -1000, -100)],
    'path2_tests': [_ray(0, 1000, 0, 0, -1000, 0)],
}


# ── 0x00426e00 TrackLoaderFloatGet: fld dword [0x00644368]; ret ─────────────────────────────────────────────────────
_TLF = (0x3f800000, 0xc2c80000, 0x40490fdb, 0x00000001, 0x7f7fffff, 0x00000000, 0x42f60000, 0xbdcccccd)
FIXES['track_loader_float_get'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'float', 'args': []}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
    'path1_tests': [{'args': [], 'seed': [{'addr': 0x00644368, 'val': v}]} for v in _TLF],
    'path2_tests': [{'args': [], 'seed': [{'addr': 0x00644368, 'val': _TLF[0]}]}],
}


# ── fake font (for 0x00427ad0) ──────────────────────────────────────────────────────────────────────────────────────
# FUN_005555b0(font, str, size, rect, mode, colour) / FUN_005554d0(font, str, size) read: font+0x10 bit 2 (16-bit
# chars), +0xc spacing float, +0x24 short glyph index per char < 0x80, +0x124/+0x128/+0x12c high-char table,
# +0x134 -> {glyph_base, nonzero} (FUN_005c4d30), glyph advance float at glyph_base + idx*0x20, +0x138 render fn
# (cdecl, `add esp,0x14` at 0x00555810). The render fn is the bare ret, so nothing is drawn from the harness thread.
_FONT, _GDESC, _GLY, _COL = 0x000, 0x140, 0x200, 0x600


def _font_seeds():
    s = [{'addr': {'scratch': _FONT + 0x10}, 'val': 2}, {'addr': {'scratch': _FONT + 0xc}, 'val': _f(1.5)},
         {'addr': {'scratch': _FONT + 0x124}, 'val': 0x100}, {'addr': {'scratch': _FONT + 0x128}, 'val': 0},
         {'addr': {'scratch': _FONT + 0x12c}, 'val': 0}, {'addr': {'scratch': _FONT + 0x134}, 'val': {'scratch': _GDESC}},
         {'addr': {'scratch': _FONT + 0x138}, 'val': _RET},
         {'addr': {'scratch': _GDESC}, 'val': {'scratch': _GLY}}, {'addr': {'scratch': _GDESC + 4}, 'val': 1}]
    for c in range(0, 0x80, 2):
        lo, hi = c % 8, (c + 1) % 8
        s.append({'addr': {'scratch': _FONT + 0x24 + c * 2}, 'val': lo | (hi << 16)})
    for g in range(8):
        s.append({'addr': {'scratch': _GLY + g * 0x20}, 'val': _f(3.0 + g * 1.25)})
    return s


# ── 0x00427ad0 MenuMenusBB(id, x, y, w, h, argb, size) ──────────────────────────────────────────────────────────────
# FUN_00427780/FUN_004277a0 transcode string id into a stack buffer; FUN_00552d10 (Im2D matrix push, DAT_00912b04);
# FUN_00556e90(DAT_0067d83c, &argb x4) writes the colour context floats [0..0x19]; rect from x/y/w/h and
# _DAT_0067d830/834; FUN_005555b0(DAT_0067d838, buf, size*0.0708, rect, 1, DAT_0067d83c); FUN_00552d70 pops and
# zeroes DAT_00912bd8 / DAT_00912bec. Old vectors were zero-arg calls. Point the font and colour context at
# harness records; observe the colour context and the two pop-cleared globals (pre-filled).
def _bb(sid, x, y, w, h, argb, size, vw=640.0, vh=480.0):
    s = _font_seeds() + [{'addr': 0x0067d838, 'val': {'scratch': _FONT}}, {'addr': 0x0067d83c, 'val': {'scratch': _COL}},
                         {'addr': 0x0067d830, 'val': _f(vw)}, {'addr': 0x0067d834, 'val': _f(vh)}]
    return {'args': [sid, _f(x), _f(y), _f(w), _f(h), argb, _f(size)], 'seed': s}


FIXES['menu_menus_bb'] = {
    'arg_type': _SCAL,
    'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32']},
    'observe': [{'addr': {'scratch': _COL}, 'len': 0x68}, {'addr': 0x00912bd8, 'len': 4, 'fill': 0xA5},
                {'addr': 0x00912bec, 'len': 4, 'fill': 0xA5}, {'addr': 0x00912b04, 'len': 4}],
    'scratch_size': 0x700, 'scratch_fill': 0xA5, 'freeze_main': True, 'target_global': None,
    'crash_equal_ok': None, 'lut_root_delta': None,
    'path1_tests': [_bb(1, 10.0, 20.0, 200.0, 30.0, 0xFF8040C0, 24.0), _bb(5, 0.0, 0.0, 640.0, 480.0, 0x11223344, 12.0),
                    _bb(20, 100.0, 50.0, 20.0, 10.0, 0x00000000, 40.0), _bb(40, 320.0, 240.0, 64.0, 64.0, 0xFFFFFFFF, 8.0),
                    _bb(3, 5.0, 5.0, 50.0, 50.0, 0x80808080, 16.0, 800.0, 600.0)],
    'path2_tests': [_bb(1, 10.0, 20.0, 200.0, 30.0, 0xFF8040C0, 24.0)],
}


# ── 0x004282a0 MenuMenusBA(id, size) -> float ───────────────────────────────────────────────────────────────────────
# return FUN_005554d0(DAT_0067d838, transcode(id), size * 0.0708 [0x005cd5fc]) / _DAT_0067d830 * 640.0 [0x005cd618].
# Old vectors were zero-arg calls. Use the fake font (glyph advances 3.0 + 1.25*(c%8) + 1.5 spacing) and seed the
# viewport width, fold the float return.
def _ba(sid, size, vw=640.0):
    s = _font_seeds() + [{'addr': 0x0067d838, 'val': {'scratch': _FONT}}, {'addr': 0x0067d830, 'val': _f(vw)}]
    return {'args': [sid, _f(size)], 'seed': s}


FIXES['menu_menus_ba'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'float', 'args': ['uint32', 'uint32']}, 'fold_ret': True, 'observe': [],
    'scratch_size': 0x700, 'scratch_fill': 0x00, 'freeze_main': True, 'target_global': None,
    'crash_equal_ok': None, 'lut_root_delta': None,
    'path1_tests': [_ba(1, 24.0), _ba(5, 12.0), _ba(20, 40.0), _ba(40, 8.0, 800.0), _ba(100, 1.0), _ba(3, 16.0, 320.0)],
    'path2_tests': [_ba(1, 24.0)],
}


# ── 0x00429a30 MenuMenusBE ──────────────────────────────────────────────────────────────────────────────────────────
# i = FUN_00430790() (= DAT_0067f17c); DAT_007f0db4[i] = DAT_0067d98c; DAT_007f0de8[i] = DAT_0067d994;
# DAT_007f0e1c[i] = DAT_0067d99c. Old vectors stored live zeros. Seed index + sources, observe all three arrays.
def _be(i, laps, secs, frac):
    return {'args': [], 'seed': [{'addr': 0x0067f17c, 'val': i}, {'addr': 0x0067d98c, 'val': laps},
                                 {'addr': 0x0067d994, 'val': secs}, {'addr': 0x0067d99c, 'val': _f(frac)}]}


FIXES['menu_menus_be'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x007f0db4, 'len': 0x9c, 'fill': 0xA5}], 'freeze_main': True, 'target_global': None,
    'lut_root_delta': None,
    'path1_tests': [_be(0, 3, 45, 0.5), _be(1, 0, 59, 0.25), _be(5, 9, 0, 0.0), _be(12, 1, 2, 0.75),
                    _be(7, 0x12345678, 0x0BADF00D, -1.5)],
    'path2_tests': [_be(0, 3, 45, 0.5)],
}


# ── 0x00429a70 / 0x00429a80 / 0x00429a90 lap getters by slot ────────────────────────────────────────────────────────
# a70: fld dword [0x0067d99c + i*4]; a80: return [0x0067d98c + i*4]; a90: return [0x0067d994 + i*4].
# Old vectors read the live lap block (zeros). Seed 12 distinct dwords over 0x0067d98c..0x0067d9bb.
_LAPV = (0x00000003, 0x0000002d, 0x3f000000, 0x00000007, 0xc2c80000, 0x0000003b, 0x40490fdb, 0x80000001,
         0x7f7fffff, 0x00000001, 0x12345678, 0xbdcccccd)


def _lap(i):
    return {'args': [i], 'seed': [{'addr': 0x0067d98c + 4 * j, 'val': v} for j, v in enumerate(_LAPV)]}


for _k, _r in (('lap_frac_get_by_slot', 'float'), ('lap_laps_get_by_slot', 'uint32'),
               ('lap_secs_get_by_slot', 'uint32')):
    FIXES[_k] = {
        'arg_type': _SCAL, 'signature': {'ret': _r, 'args': ['int32']}, 'fold_ret': True, 'observe': [],
        'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
        'path1_tests': [_lap(i) for i in range(6)], 'path2_tests': [_lap(0), _lap(1)],
    }


# ── 0x0042aeb0 MenuReadinessCheckB ──────────────────────────────────────────────────────────────────────────────────
# if (DAT_0067eab0 != 2 && DAT_00898ab0 != 0) return 0; col +3 (0x007f1043 / 0x007f1503): 1 if any pad held && !prev.
def _rdyb(eab0, ab0, mode, held, prev, slots=(-1, -1, -1, -1)):
    s = [{'addr': 0x0067eab0, 'val': eab0}, {'addr': 0x00898ab0, 'val': ab0},
         {'addr': 0x007f1a0c, 'val': mode}] + _btn(3, held, prev) + _slots(slots)
    return {'args': [], 'seed': s}


FIXES['menu_readiness_check_b'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
    'path1_tests': [_rdyb(2, 1, 0x1000, 0x01, 0x00), _rdyb(2, 1, 0x1000, 0x80, 0x80),
                    _rdyb(2, 1, 0x1000, 0x81, 0x01), _rdyb(0, 1, 0x1000, 0xFF, 0x00),
                    _rdyb(0, 0, 0x1000, 0x04, 0x00), _rdyb(2, 0, 0x1000, 0x00, 0x00),
                    _rdyb(2, 1, 0, 0x01, 0x00, (0, -1, -1, -1)), _rdyb(2, 1, 0, 0x01, 0x00)],
    'path2_tests': [_rdyb(2, 1, 0x1000, 0x01, 0x00)],
}


# ── 0x0042af50 MenuReadinessCheckC (two registry keys, one RVA) ─────────────────────────────────────────────────────
# if (DAT_0067eab0 != 2 && DAT_0067e7c8 == 0 && DAT_00898ab0 != 0) return 0; col +1 (0x007f1041 / 0x007f1501).
def _rdyc(eab0, e7c8, ab0, mode, held, prev, slots=(-1, -1, -1, -1)):
    s = [{'addr': 0x0067eab0, 'val': eab0}, {'addr': 0x0067e7c8, 'val': e7c8}, {'addr': 0x00898ab0, 'val': ab0},
         {'addr': 0x007f1a0c, 'val': mode}] + _btn(1, held, prev) + _slots(slots)
    return {'args': [], 'seed': s}


for _k in ('menu_readiness_check', 'menu_readiness_check_c'):
    FIXES[_k] = {
        'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True, 'observe': [],
        'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
        'path1_tests': [_rdyc(2, 0, 1, 0x1000, 0x01, 0x00), _rdyc(2, 0, 1, 0x1000, 0x80, 0x80),
                        _rdyc(2, 0, 1, 0x1000, 0x81, 0x01), _rdyc(0, 0, 1, 0x1000, 0xFF, 0x00),
                        _rdyc(0, 1, 1, 0x1000, 0x04, 0x00), _rdyc(2, 0, 1, 0x1000, 0x00, 0x00),
                        _rdyc(2, 0, 1, 0, 0x01, 0x00, (0, -1, -1, -1)), _rdyc(2, 0, 1, 0, 0x01, 0x00)],
        'path2_tests': [_rdyc(2, 0, 1, 0x1000, 0x01, 0x00)],
    }


# ── 0x0042b540 MenuButtonDetectD ────────────────────────────────────────────────────────────────────────────────────
# col +5; int timer _DAT_0067f1bc cleared when no pad 0..6 is held (and pad 7 not held, 0x1000 path). Held: screen
# type *(0x0067ed3c + DAT_0067e9f8*0x40) 0x13 -> sub *(0x0067ed40 + ..) 0/1/2 -> 1, other sub -> edge detect;
# type 0x1e -> 1; other -> edge detect (held && !prev).
def _detd(held, prev, stype, sub, t0=0x1234, mode=0x1000, slots=(-1, -1, -1, -1)):
    s = [{'addr': 0x007f1a0c, 'val': mode}, {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed3c, 'val': stype},
         {'addr': 0x0067ed40, 'val': sub}, {'addr': 0x0067f1bc, 'val': t0}] + _btn(5, held, prev) + _slots(slots)
    return {'args': [], 'seed': s}


FIXES['menu_button_detect_d'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x0067f1bc, 'len': 4}], 'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
    'path1_tests': [_detd(0x00, 0x00, 3, 0), _detd(0x01, 0x01, 0x13, 0), _detd(0x01, 0x01, 0x13, 2),
                    _detd(0x01, 0x01, 0x13, 5), _detd(0x01, 0x00, 0x13, 5), _detd(0x10, 0x10, 0x1e, 0),
                    _detd(0x80, 0x00, 3, 0), _detd(0x80, 0x80, 3, 0), _detd(0x08, 0x08, 3, 0),
                    _detd(0x01, 0x00, 3, 0, mode=0, slots=(0, -1, -1, -1)), _detd(0x00, 0x00, 3, 0, mode=0)],
    'path2_tests': [_detd(0x01, 0x00, 3, 0)],
}


# ── 0x0042b960 CarSlotInit1P ────────────────────────────────────────────────────────────────────────────────────────
# DAT_007f1a14 = index of the first pad k (0..7) with 0x007f1042+k*0x4c != 0 && 0x007f1502+k*0x4c == 0 (8 if none);
# DAT_007f1a24/34/44 = -1; DAT_007f1a0c = 1; FUN_0040e480(i, v) for (0,1),(1,0),(2,0),(3,0) writes
# *(*(0x005f2770) + i*4 + 0x34) = v (disasm 0x0040e488). Old vectors: zero-arg call with live input (no writes seen
# because the observed set was empty). Point 0x005f2770 at a pre-filled scratch, observe the slot block and it.
def _csi(held, prev):
    return {'args': [], 'seed': [{'addr': 0x005f2770, 'val': {'scratch': 0}}] + _btn(2, held, prev)}


FIXES['car_slot_init_1p'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x007f1a0c, 'len': 0x48, 'fill': 0xA5}, {'addr': {'scratch': 0x30}, 'len': 0x18}],
    'scratch_size': 0x60, 'scratch_fill': 0xA5, 'freeze_main': True, 'target_global': None, 'lut_root_delta': None,
    'path1_tests': [_csi(0x01, 0x00), _csi(0x04, 0x00), _csi(0x80, 0x00), _csi(0x00, 0x00), _csi(0x06, 0x02),
                    _csi(0xFF, 0xFF)],
    'path2_tests': [_csi(0x04, 0x00)],
}


# ── 0x0042e5b0 MenuChromeShellB ─────────────────────────────────────────────────────────────────────────────────────
# Disasm 0x0042e65a: phase = _ftol(DAT_007f1010 * 60.0 [0x005cc728]) & 0x1ff (FUN_004a2c48 is _ftol of ST0).
# phase 0x1e0 -> DAT_0067e848 += 2; 0xe0 -> DAT_0067f0b8 += 2; DAT_0067f0b8 forced odd; DAT_008990e0 -= 0x10 clamped
# [0, 0x200]. Draws go through RwGlobals (0x007d3ff8) +0x20 / +0x30: fake RwGlobals with bare-ret slots (b03).
# Observe the counters, the slide and the Im2D vertex buffer (last quad written by FUN_00473ee0).
# Iteration 2: both sides AV'd at 0x8. FUN_0042e590 (disasm 0x0042e590) looks a texture name up through
# FUN_0040bb70 -> FUN_004c5c00(DAT_0063b900, name), which reads *(DAT_0063b900 + 8): point DAT_0063b900 at an empty
# sentinel list in scratch (+0x48: next = itself) so the lookup returns 0. FUN_00473ee0's FUN_004c3b30 reads the
# sqrt table *(*(DAT_007d3ff8 + DAT_007d3ffc) + i*4): seed DAT_007d3ffc = 0x134 (the offset the agent reports) and
# fake+0x134 -> a zeroed 0x4000-byte table at scratch+0x1000.
def _chb(t, e848, f0b8, slide):
    return {'args': [], 'seed': [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x18}, 'val': _f(0.5)},
                                 {'addr': {'scratch': 0x20}, 'val': _RET}, {'addr': {'scratch': 0x30}, 'val': _RET},
                                 {'addr': 0x0063b900, 'val': {'scratch': 0x40}},
                                 {'addr': {'scratch': 0x48}, 'val': {'scratch': 0x48}},
                                 {'addr': 0x007d3ffc, 'val': 0x134}, {'addr': {'scratch': 0x134}, 'val': {'scratch': 0x1000}},
                                 {'addr': 0x0086ecc8, 'val': 0x80}, {'addr': 0x0086eccc, 'val': 0x40},
                                 {'addr': 0x007f1010, 'val': _f(t)}, {'addr': 0x0067e848, 'val': e848},
                                 {'addr': 0x0067f0b8, 'val': f0b8}, {'addr': 0x008990e0, 'val': slide}]}


FIXES['menu_chrome_shell_b'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x0067e848, 'len': 4}, {'addr': 0x0067f0b8, 'len': 4}, {'addr': 0x008990e0, 'len': 4},
                {'addr': 0x00898a20, 'len': 0x78, 'fill': 0xA5}],
    'scratch_size': 0x5000, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None, 'lut_root_delta': None,
    'path1_tests': [_chb(8.0, 4, 6, 0x100), _chb(224.5 / 60.0, 4, 6, 0x08), _chb(1.0, 0, 3, 0x300),
                    _chb(5.0, 10, 10, 0x40), _chb(8.3, 1, 2, 0x200), _chb(0.0, 0, 0, 0)],
    'path2_tests': [_chb(8.0, 4, 6, 0x100)],
}
