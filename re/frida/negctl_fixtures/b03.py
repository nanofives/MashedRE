"""negctl re-earn, batch b03 (frontend menu leaves, championship, timer dispatcher, misc).

Decomp from re/tools/decomp_pc.py (pool slot, read-only) and re/tools/disasm_va.py. Every entry freezes the game's
main thread because the globals it seeds (input arrays, menu screen index, game state, unlock arrays) are live.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

_SCAL = 'scalars_to_scattered_globals'

# ── shared: input button arrays ───────────────────────────────────────────────────────────────────────────────────────
# Held bytes at 0x007f1040 + col + k*0x4c, previous-frame bytes at 0x007f1500 + col + k*0x4c, k = 0..7 (pointer walk
# ends at 0x007f1761.. in every function below). DAT_007f1a0c == 0x1000 selects the all-8-pads path; otherwise the
# four slot ids at 0x007f1a14 + i*0x10 are walked and FUN_0040e470(i) (*(PTR_PTR_005f2770 + i*4 + 0x34)) is consulted.
def _btn(col, held, prev):
    s = []
    for k in range(8):
        s.append({'addr': 0x007f1040 + col + k * 0x4c, 'val': 1 if (held >> k) & 1 else 0, 'size': 1})
        s.append({'addr': 0x007f1500 + col + k * 0x4c, 'val': 1 if (prev >> k) & 1 else 0, 'size': 1})
    return s


def _slots(ids):
    return [{'addr': 0x007f1a14 + i * 0x10, 'val': v & 0xFFFFFFFF} for i, v in enumerate(ids)]


# ── 0x0042ae10 MenuReadinessCheckA ────────────────────────────────────────────────────────────────────────────────────
# if (DAT_0067eab0 != 2 && DAT_0067e7c8 == 0 && DAT_00898ab0 != 0) return 0; then col +2: return 1 if any pad has
# held != 0 && prev == 0. The old vectors were ten zero-arg calls at the menu returning 0 on both sides.
def _rdy(eab0, e7c8, ab0, mode, held, prev, slots=(-1, -1, -1, -1)):
    s = [{'addr': 0x0067eab0, 'val': eab0}, {'addr': 0x0067e7c8, 'val': e7c8}, {'addr': 0x00898ab0, 'val': ab0},
         {'addr': 0x007f1a0c, 'val': mode}] + _btn(2, held, prev) + _slots(slots)
    return {'args': [], 'seed': s}


FIXES['menu_readiness_check_a'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_rdy(2, 0, 1, 0x1000, 0x01, 0x00), _rdy(2, 0, 1, 0x1000, 0x80, 0x80),
                    _rdy(2, 0, 1, 0x1000, 0x81, 0x01), _rdy(0, 0, 1, 0x1000, 0xFF, 0x00),
                    _rdy(0, 1, 1, 0x1000, 0x04, 0x00), _rdy(2, 0, 1, 0x1000, 0x00, 0x00),
                    _rdy(2, 0, 1, 0, 0x01, 0x00, (0, -1, -1, -1)), _rdy(2, 0, 1, 0, 0x01, 0x00)],
    'path2_tests': [_rdy(2, 0, 1, 0x1000, 0x01, 0x00)],
}


# ── 0x0042aff0 / 0x0042b180 MenuButtonDetectA / B ─────────────────────────────────────────────────────────────────────
# col +6 (A) / +7 (B); timer float _DAT_0067f1b0 (A) / _DAT_0067f1b4 (B). Held anywhere: if screen type
# *(0x0067ed3c + DAT_0067e9f8*0x40) is 6 or 7, timer += DAT_007f1004 and when timer > _DAT_005ccac0 (0x3ecccccd 0.4)
# timer = 0.2, return 1. Not held: timer = 0.0. Then edge detect (held && !prev) -> 1.
def _det(col, tmr, held, prev, stype, t0, dt, mode=0x1000, slots=(-1, -1, -1, -1)):
    s = [{'addr': 0x007f1a0c, 'val': mode}, {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed3c, 'val': stype},
         {'addr': tmr, 'val': _f(t0)}, {'addr': 0x007f1004, 'val': _f(dt)}] + _btn(col, held, prev) + _slots(slots)
    return {'args': [], 'seed': s}


for _k, _col, _tmr in (('menu_button_detect_a', 6, 0x0067f1b0), ('menu_button_detect_b', 7, 0x0067f1b4)):
    FIXES[_k] = {
        'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
        'observe': [{'addr': _tmr, 'len': 4}], 'freeze_main': True, 'target_global': None,
        'path1_tests': [_det(_col, _tmr, 0x02, 0x02, 6, 0.3, 0.2), _det(_col, _tmr, 0x02, 0x02, 6, 0.1, 0.1),
                        _det(_col, _tmr, 0x02, 0x00, 3, 0.1, 0.1), _det(_col, _tmr, 0x00, 0x00, 7, 0.3, 0.2),
                        _det(_col, _tmr, 0x40, 0x00, 7, 0.0, 0.05), _det(_col, _tmr, 0x80, 0x80, 8, 0.5, 0.5),
                        _det(_col, _tmr, 0x01, 0x00, 6, 0.1, 0.1, 0, (0, -1, -1, -1))],
        'path2_tests': [_det(_col, _tmr, 0x02, 0x02, 6, 0.3, 0.2)],
    }


# ── 0x0042b310 MenuButtonDetectC ──────────────────────────────────────────────────────────────────────────────────────
# col +4; int timer _DAT_0067f1b8 cleared when nothing held. Held: screen type 0x13 -> sub *(0x0067ed40 + idx*0x40)
# 0/1 or 2 -> return 1, other sub -> edge detect; type 0x1e -> return 1; any other type -> edge detect.
def _detc(held, prev, stype, sub, t0=0x1234):
    s = [{'addr': 0x007f1a0c, 'val': 0x1000}, {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed3c, 'val': stype},
         {'addr': 0x0067ed40, 'val': sub}, {'addr': 0x0067f1b8, 'val': t0}] + _btn(4, held, prev)
    return {'args': [], 'seed': s}


FIXES['menu_button_detect_c'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x0067f1b8, 'len': 4}], 'freeze_main': True, 'target_global': None,
    'path1_tests': [_detc(0x00, 0x00, 3, 0), _detc(0x01, 0x01, 0x13, 0), _detc(0x01, 0x01, 0x13, 2),
                    _detc(0x01, 0x01, 0x13, 5), _detc(0x01, 0x00, 0x13, 5), _detc(0x10, 0x10, 0x1e, 0),
                    _detc(0x80, 0x00, 3, 0), _detc(0x08, 0x08, 3, 0)],
    'path2_tests': [_detc(0x01, 0x00, 3, 0)],
}


# ── 0x0042b770 MenuButtonDetectE ──────────────────────────────────────────────────────────────────────────────────────
# col +9: return 1 if any pad has held != 0 && prev == 0.
def _dete(held, prev, mode=0x1000, slots=(-1, -1, -1, -1)):
    return {'args': [], 'seed': [{'addr': 0x007f1a0c, 'val': mode}] + _btn(9, held, prev) + _slots(slots)}


FIXES['menu_button_detect_e'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_dete(0x01, 0x00), _dete(0x01, 0x01), _dete(0x80, 0x00), _dete(0x00, 0x00), _dete(0x7F, 0x7F),
                    _dete(0x10, 0x00), _dete(0x01, 0x00, 0, (0, -1, -1, -1))],
    'path2_tests': [_dete(0x01, 0x00)],
}


# ── 0x0042aae0 MenuIm2DQuad ───────────────────────────────────────────────────────────────────────────────────────────
# Disasm 0x0042aae0: `push ecx` only reserves a local; the routine reads no argument. It builds the 4-vertex buffer
# 0x0067ec30..0x0067eca8 from the screen dims (shorts at 0x0067ea54 / 0x0067ea56) and colour alpha DAT_0067eca8<<24,
# then draws only when DAT_0067eca8 != 0. The old int_scalar vectors only rendered the live overlay, so nothing
# observed changed. Observe the pre-filled vertex buffer; alpha 0 keeps the draw call out of the harness thread.
def _quad(w, h, a):
    return {'args': [0], 'seed': [{'addr': 0x0067ea54, 'val': (w & 0xFFFF) | ((h & 0xFFFF) << 16)},
                                  {'addr': 0x0067eca8, 'val': a}]}


FIXES['menu_im2d_quad'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['uint32']},
    'observe': [{'addr': 0x0067ec30, 'len': 0x78}, {'addr': 0x0067eca8, 'len': 4}], 'pre_fill_byte': 0xA5,
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_quad(640, 480, 0), _quad(800, 600, 0), _quad(1, 2, 0), _quad(0x7fff, 0x8000, 0)],
    'path2_tests': [_quad(640, 480, 0)],
}


# ── 0x0042e3a0 MenuChromeShellA ───────────────────────────────────────────────────────────────────────────────────────
# If the screen pointer *(0x0067ed38 + DAT_0067e9f8*0x40) != 0x005f6980, DAT_0067ebc0 = 0 (the == branch runs the
# 3580-byte FUN_0042d5a0 and is not driven). Then 4 quads (8 when DAT_0067f17c > 9) through FUN_00472f40 /
# FUN_004730b0 / FUN_00472c60, each rebuilding the vertex buffer 0x00898a20..0x00898a98 from the screen dims.
# Observe DAT_0067ebc0 and the buffer (last quad written).
# Iteration 2: the live RwIm2D draw (RwGlobals 0x007d3ff8 slot +0x30) raised a system error on BOTH sides from the
# harness thread (crash-equal, degenerate). Point DAT_007d3ff8 at a harness fake whose +0x20 (render-state set) and
# +0x30 (Im2D render) are the bare `ret` at 0x0042b8b6 (cdecl, caller pops) and whose +0x18 is a fixed float; the
# vertex buffer the quads build is unchanged by the stub.
_RET = 0x0042b8b6


def _chr(sel, w, h, ebc0=0x1234):
    return {'args': [], 'seed': [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x18}, 'val': _f(0.5)},
                                 {'addr': {'scratch': 0x20}, 'val': _RET}, {'addr': {'scratch': 0x30}, 'val': _RET},
                                 {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed38, 'val': 0},
                                 {'addr': 0x0067f17c, 'val': sel}, {'addr': 0x0067ebc0, 'val': ebc0},
                                 {'addr': 0x0067ea54, 'val': (w & 0xFFFF) | ((h & 0xFFFF) << 16)}]}


FIXES['menu_chrome_shell_a'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x0067ebc0, 'len': 4}, {'addr': 0x00898a20, 'len': 0x78}], 'pre_fill_byte': 0xA5,
    'scratch_size': 0x40, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_chr(0, 640, 480), _chr(12, 640, 480), _chr(9, 800, 600), _chr(10, 320, 240)],
    'path2_tests': [_chr(0, 640, 480)],
}


# ── unlock-table seeding (0x007f0a40 .. 0x007f0c90, 12-dword records of stride 0x30) ───────────────────────────────
_UNL_BASE, _UNL_END = 0x007f0a40, 0x007f0c90


def _unl(fn):
    return [{'addr': a, 'val': fn(a) & 0xFFFFFFFF} for a in range(_UNL_BASE, _UNL_END, 4)]


# ── 0x0042ebe0 FrontendPlayerSlotCheck(param) ─────────────────────────────────────────────────────────────────────────
# One pass over the record at 0x007f0a48 (stride 0x78 dwords, loop ends at 0x007f0c28); param 10/11/12 select dword
# columns; returns (*(0x007f0a44 + param*0x30) != 0) && !found_zero. Old vectors ran on the live table only.
def _psc(p, fn):
    return {'args': [p], 'seed': _unl(fn)}


_PATS = [lambda a: 0, lambda a: 1, lambda a: 2 if (a >> 2) % 3 else 0, lambda a: (a >> 2) & 1,
         lambda a: 0 if a == 0x007f0b04 else 1, lambda a: 0 if a == 0x007f0c08 else 1]
FIXES['frontend_player_slot_check'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['int32']}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_psc(p, fn) for p in (10, 11, 12) for fn in _PATS],
    'path2_tests': [_psc(10, _PATS[1])],
}


# ── 0x0042fe50 RaceEndAltFlagIfEndMode ────────────────────────────────────────────────────────────────────────────────
# return -(FUN_0042f6a0() != 0xb) & DAT_0067ea78; FUN_0042f6a0 returns DAT_0067e9fc.
def _rea(mode, v):
    return {'args': [], 'seed': [{'addr': 0x0067e9fc, 'val': mode}, {'addr': 0x0067ea78, 'val': v}]}


FIXES['race_end_alt_flag_if_end_mode'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_rea(3, 1), _rea(0xb, 1), _rea(3, 0xDEADBEEF), _rea(0xb, 0xDEADBEEF), _rea(0, 0), _rea(0xa, 7)],
    'path2_tests': [_rea(3, 1)],
}


# ── 0x00430290 Championship::Complete ─────────────────────────────────────────────────────────────────────────────────
# Gate: DAT_0067e9fc in {3,4,5} and FUN_0040dd60() != 0 ((DAT_0063b90c != 1) - 1 & DAT_007f0fcc). Mode 3/4/5 rewrite
# the unlock records at 0x007f0a40.. for DAT_0067f17c, post a message via FUN_0042a920 (DAT_00898ab0 = 0x40,
# DAT_00899140 = id) and finish with FUN_004099a0 (autosave flags 0x008a9584/94/b0 only when DAT_008a95ac != 0).
def _chp(mode, idx, fn, gate=1, auto=0):
    s = [{'addr': 0x0067e9fc, 'val': mode}, {'addr': 0x0063b90c, 'val': gate}, {'addr': 0x007f0fcc, 'val': 1},
         {'addr': 0x0067f17c, 'val': idx}, {'addr': 0x008a95ac, 'val': auto}, {'addr': 0x007f0f2c, 'val': 0}]
    return {'args': [], 'seed': s + _unl(fn)}


def _silver(a):
    return 2 if (a - 0x007f0a48) % 0x30 == 0 and a < 0x007f0c28 else 0


def _all2_but(hole):
    return lambda a: 0 if a in hole else 2


FIXES['championship_complete'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': _UNL_BASE, 'len': _UNL_END - _UNL_BASE}, {'addr': 0x007f0f2c, 'len': 4},
                {'addr': 0x00898ab0, 'len': 4}, {'addr': 0x00899140, 'len': 4},
                {'addr': 0x008a9584, 'len': 4}, {'addr': 0x008a9594, 'len': 4}, {'addr': 0x008a95b0, 'len': 4}],
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_chp(3, 0, lambda a: 0), _chp(3, 2, lambda a: 2 if (a >> 2) % 5 == 0 else 0),
                    _chp(4, 1, lambda a: 0), _chp(4, 3, _silver), _chp(5, 3, lambda a: 0),
                    _chp(5, 2, _all2_but({0x007f0a54 + 2 * 0x30})), _chp(2, 0, lambda a: 0),
                    _chp(3, 0, lambda a: 0, gate=0), _chp(3, 1, lambda a: 1, auto=1)],
    'path2_tests': [_chp(3, 0, lambda a: 0)],
}


# ── 0x0043c000 TimerSlotTickDispatcher ────────────────────────────────────────────────────────────────────────────────
# Disasm: fld [0x0067e840]; fcomp [0x005d757c] (0.0). Equal -> every state dword in 0x0067e7a8.. != 1 is set to 0.
# Then per slot: state 1 -> mov byte [counter], al of _ftol((1.0 - x840) * 255.0) + action call; state 2 -> byte of
# _ftol(x840 * 255.0); else byte 0 (slot 6 writes the byte at 0x0067e7d8, its own state, at 0x0043c1c6). States 1 are
# not driven (the actions read live input). Observe the whole slot block, pre-filled.
def _tsd(x840, states):
    s = [{'addr': 0x0067e840, 'val': _f(x840)}]
    for i, st in enumerate(states):
        s.append({'addr': 0x0067e7a8 + i * 8, 'val': st})
    return {'args': [], 'seed': s}


FIXES['timer_slot_tick_dispatcher'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x0067e7a8, 'len': 0x98}, {'addr': 0x0067e840, 'len': 4}], 'pre_fill_byte': 0xA5,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_tsd(0.0, [0] * 19), _tsd(0.5, [0] * 19), _tsd(0.5, [2] * 19), _tsd(0.25, [5] * 19),
                    _tsd(0.0, [2, 0, 5] * 6 + [2])],
    'path2_tests': [_tsd(0.5, [0] * 19)],
}


# ── 0x00442c80 ModeGatedPlayerCheck(param) ────────────────────────────────────────────────────────────────────────────
# return FUN_0040e350() == 6 (DAT_0063ba8c) && DAT_007f0fd0 not in {4,8,9} && *(float*)(0x008989b0 + p*4) >
# _DAT_005cc9b8 (0x40e00000 7.0). At the menu DAT_0063ba8c != 6, so every old call returned 0.
def _mg(p, gs, sub, v):
    return {'args': [p], 'seed': [{'addr': 0x0063ba8c, 'val': gs}, {'addr': 0x007f0fd0, 'val': sub},
                                  {'addr': 0x008989b0 + p * 4, 'val': _f(v)}]}


FIXES['mode_gated_player_check'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['int32']}, 'fold_ret': True, 'observe': [],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_mg(0, 6, 0, 8.0), _mg(1, 6, 2, 7.5), _mg(2, 6, 4, 8.0), _mg(3, 6, 8, 8.0), _mg(0, 6, 9, 8.0),
                    _mg(1, 6, 0, 7.0), _mg(2, 6, 0, 6.0), _mg(3, 3, 0, 8.0), _mg(0, 6, 1, -8.0)],
    'path2_tests': [_mg(0, 6, 0, 8.0)],
}


# ── 0x00475a60 PendingOpQueueFlush ────────────────────────────────────────────────────────────────────────────────────
# for i < DAT_0069160c: FUN_004b6520(ptrs[i], sizes[i]*0x50) (-> FUN_004b64e0(p, 0, n), zero-fill);
# results[i] = 0, with ptrs = *0x00691604, sizes = *0x00691610, results = *0x00691614. Count is 0 at the menu.
# Point all three at a harness scratch: ptrs @+0x00, sizes @+0x20, results @+0x40, targets @+0x100 / +0x200.
def _pq(count, sizes):
    s = [{'addr': 0x0069160c, 'val': count}, {'addr': 0x00691604, 'val': {'scratch': 0x00}},
         {'addr': 0x00691610, 'val': {'scratch': 0x20}}, {'addr': 0x00691614, 'val': {'scratch': 0x40}},
         {'addr': {'scratch': 0x00}, 'val': {'scratch': 0x100}}, {'addr': {'scratch': 0x04}, 'val': {'scratch': 0x200}}]
    for i, n in enumerate(sizes):
        s.append({'addr': {'scratch': 0x20 + i * 4}, 'val': n})
    return {'args': [], 'seed': s}


FIXES['pending_op_queue_flush'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': {'scratch': 0x40}, 'len': 8}, {'addr': {'scratch': 0x100}, 'len': 0x100},
                {'addr': {'scratch': 0x200}, 'len': 0x100}],
    'scratch_size': 0x300, 'scratch_fill': 0xA5, 'freeze_main': True, 'target_global': None,
    'path1_tests': [_pq(1, [1]), _pq(2, [1, 2]), _pq(2, [3, 0]), _pq(0, [1, 1]), _pq(1, [0])],
    'path2_tests': [_pq(1, [1])],
}
