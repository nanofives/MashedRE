"""negctl re-earn, batch b09 (frontend sprite gates / menu renderers, mode getters, race-end check, vehicle setters).

Decomp from re/tools/decomp_pc.py (pool slot, read-only), disasm from re/tools/disasm_va.py. Every entry freezes the
game's main thread: the globals seeded/observed (DAT_0067e9fc, the menu screen table, input arrays, unlock table,
per-vehicle records) are live.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}
_SCAL = 'scalars_to_scattered_globals'
_CLR = {'target_global': None, 'crash_equal_ok': None, 'field_offset': None, 'is_getter': None}


def _e(**kw):
    d = dict(_CLR)
    d.update({'arg_type': _SCAL, 'freeze_main': True})
    d.update(kw)
    return d


# ── 0x0042ee00 SpriteSlotGate(slot) ───────────────────────────────────────────────────────────────────────────────────
# Disasm 0x0042ee00..0x0042ee3b: slot 0/1/2 rewrite [esp+4] to "lock"/"dot"/"check" (0x005cd7b8/b4/ac) and tail-jmp
# FUN_0040bb50 (BADGES dictionary lookup, returns the sprite pointer); other slots return 0. The old vectors were
# only 3/4/5/99 (every call returned 0).
FIXES['sprite_slot_gate'] = _e(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [a], 'seed': []} for a in (0, 1, 2, 3, -1, 0, 1, 2)],
    path2_tests=[{'args': [1], 'seed': []}])


# ── 0x0042ee40 FrontendModeDispatch(p) ────────────────────────────────────────────────────────────────────────────────
# Switch on DAT_0067e9fc (0x0042ee40): 2 "TimeTrial"; 3/4/5 with DAT_0067f17c > 9 -> p 0/1/2 medals, then p >= 1000
# -> p - 1000 cups; 6..9 p == 0 "MultiPlayer"; 10 "QuickRace"; each a tail-jmp to FUN_0040bb90 (lookup). Old vectors
# ran at the menu mode only.
def _fmd(mode, f17c, p):
    return {'args': [p], 'seed': [{'addr': 0x0067e9fc, 'val': mode}, {'addr': 0x0067f17c, 'val': f17c}]}


FIXES['frontend_mode_dispatch'] = _e(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[_fmd(2, 0, 7), _fmd(3, 12, 0), _fmd(4, 12, 1), _fmd(5, 12, 2), _fmd(3, 5, 0), _fmd(4, 5, 1001),
                 _fmd(5, 5, 1002), _fmd(3, 12, 1001), _fmd(6, 0, 0), _fmd(9, 0, 1), _fmd(10, 0, 5), _fmd(0, 0, 0),
                 _fmd(11, 0, 0), _fmd(5, 5, 3)],
    path2_tests=[_fmd(10, 0, 5)])


# ── 0x0042f6a0 GetRaceSubMode() = DAT_0067e9fc ────────────────────────────────────────────────────────────────────────
# Key hud_sub_mode_get named export HudSubModeGet, which the .asi does not export (the port is GetRaceSubMode,
# Util/GameStateGetters.cpp). Seed the global.
def _gm(v):
    return {'args': [], 'seed': [{'addr': 0x0067e9fc, 'val': v}]}


FIXES['hud_sub_mode_get'] = _e(
    export='GetRaceSubMode', signature={'ret': 'uint32', 'args': []}, fold_ret=True, observe=[],
    path1_tests=[_gm(v) for v in (0, 2, 3, 0xb, 0xDEADBEEF, 0xFFFFFFFF, 7)],
    path2_tests=[_gm(3)])


# ── 0x0042f6b0 MenuModeSync() ─────────────────────────────────────────────────────────────────────────────────────────
# switch DAT_0067f184 -> DAT_0067e9fc (0->2, 1->3, 2->4, 3->6, 5->5, 8->7, 9->8, 10->9, 11->10, else no write).
# Old vectors ran at the menu value only. Observe e9fc pre-filled 0xA5.
def _mms(v):
    return {'args': [], 'seed': [{'addr': 0x0067f184, 'val': v & 0xFFFFFFFF}]}


FIXES['menu_mode_sync'] = _e(
    signature={'ret': 'void', 'args': []}, observe=[{'addr': 0x0067e9fc, 'len': 4, 'fill': 0xA5}],
    path1_tests=[_mms(v) for v in (0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, -1)],
    path2_tests=[_mms(1)])


# ── 0x0042f8d0 MenuMenusBC(x, y, w, h; alpha in AL) ──────────────────────────────────────────────────────────────────
# Five FUN_00472c60 quads; each rebuilds the Im2D vertex buffer 0x00898a20 (stride 0x1c: x, y, z = *(RwGlobals+0x18),
# rhw, colour, u, v) and draws through RwGlobals +0x20/+0x30. The entry declared zero args. Real args now; RwGlobals
# (0x007d3ff8) points at a harness fake whose +0x20/+0x30 are the bare `ret` at 0x0042b8b6 (b03 menu_chrome_shell_a).
# AL (alpha) cannot be set by this handler, so the colour dwords (+0x10 of each vertex) are NOT observed: only
# x, y, z, rhw of the last quad.
_RET = 0x0042b8b6


def _fake_rw():
    return [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x18}, 'val': _f(0.5)},
            {'addr': {'scratch': 0x20}, 'val': _RET}, {'addr': {'scratch': 0x30}, 'val': _RET}]


def _bc(x, y, w, h):
    return {'args': [x, y, w, h], 'seed': _fake_rw()}


FIXES['menu_menus_bc'] = _e(
    signature={'ret': 'void', 'args': ['float', 'float', 'float', 'float']},
    observe=[{'addr': 0x00898a20 + k * 0x1c, 'len': 0x10, 'fill': 0xA5} for k in range(4)],
    scratch_size=0x40,
    path1_tests=[_bc(100.0, 50.0, 200.0, 30.0), _bc(0.0, 0.0, 640.0, 480.0), _bc(12.5, -7.25, 3.0, 4.0),
                 _bc(300.0, 400.0, 1.0, 1.0)],
    path2_tests=[_bc(100.0, 50.0, 200.0, 30.0)])


# ── 0x0042f0c0 / 0x0042fb70 / 0x0042fe90 MenuSpriteDispatchA/B/C ──────────────────────────────────────────────────────
# Three-row renderers: "Arrow" sprites (FUN_0040bb50 -> FUN_004739f0 / FUN_00473870, both write VBUF 0x00898a20) and
# text (FUN_00427e00 -> colour record *DAT_0067d83c + the dynamic vertex ring 0x007d70c8..). Real RwGlobals (warm lane,
# main thread frozen), as b05 render_00403d30 / g2 text_sprite_uv_explicit. Observe VBUF (pre-filled), the ring, and
# the colour record redirected to the scratch.
_RING = {'addr': 0x007d70c8, 'len': 0x24}
_VB = {'addr': 0x00898a20, 'len': 0x78, 'fill': 0xA5}
_DESC = [1, 0xFF060000, 2, 0xFF060000, 3, 0xFF070000]   # FUN_0042ac00 list: -0xfa0000 separators, -0xf90000 end


def _menu_common(extra):
    s = [{'addr': 0x0067d83c, 'val': {'scratch': 0}}, {'addr': 0x0067e9f8, 'val': 0},
         {'addr': 0x0067ed38, 'val': {'scratch': 0x100}}, {'addr': 0x0067ed78, 'val': {'scratch': 0x100}}]
    s += [{'addr': {'scratch': 0x100 + i * 4}, 'val': v} for i, v in enumerate(_DESC)]
    return {'args': [], 'seed': s + extra}


def _ma(e7b0, e7b4, sel, e844, ed80, ea8c, c4, c8):
    return _menu_common([{'addr': 0x0067e7b0, 'val': e7b0}, {'addr': 0x0067e7b4, 'val': e7b4},
                         {'addr': 0x0067ed40, 'val': sel}, {'addr': 0x0067e844, 'val': e844},
                         {'addr': 0x0067ed80 - 2 * 0x40, 'val': ed80}, {'addr': 0x0067ed80, 'val': ed80},
                         {'addr': 0x0067ea8c, 'val': ea8c}, {'addr': 0x005f76c4, 'val': c4},
                         {'addr': 0x005f76c8, 'val': c8}])


def _mb(e7e8, e7ec, sel, ea84, ea78, ea80):
    return _menu_common([{'addr': 0x0067e7e8, 'val': e7e8}, {'addr': 0x0067e7ec, 'val': e7ec},
                         {'addr': 0x0067ed40, 'val': sel}, {'addr': 0x0067ed80, 'val': sel},
                         {'addr': 0x0067ea84, 'val': ea84}, {'addr': 0x0067ea78, 'val': ea78},
                         {'addr': 0x0067ea80, 'val': ea80}])


def _mc(e810, e814, heads, dims, types, feat, scr=0):
    # 0x0042fe90 re-read from the bytes 2026-10-12: dim flag = dword 0x0067ed44 + scr*0x40 + i*4 (shl 4 @0x0042ff1b,
    # [edx*4] @0x0042ff20); descriptor = dword 0x0067ed38 (mode 1) / 0x0067ed78 (else) + scr*0x40; player field =
    # DWORD 0x007f105c + type*0x4c (imul 0x4c @0x00430010, mov edi,dword @0x00430013); 4 entries 0x00898b1c..0x00898bec.
    s = [{'addr': 0x0067e810, 'val': e810}, {'addr': 0x0067e814, 'val': e814}]
    s += [{'addr': 0x00898b1c + i * 0x34, 'val': h} for i, h in enumerate(heads)]
    s += [{'addr': 0x0067ed44 + scr * 0x40 + i * 4, 'val': d} for i, d in enumerate(dims)]
    s += [{'addr': 0x00898aa0 + i * 4, 'val': t} for i, t in enumerate(types)]
    s += [{'addr': 0x007f105c + t * 0x4c, 'val': feat[i]} for i, t in enumerate(types)]
    if scr:
        s += [{'addr': 0x0067e9f8, 'val': scr}, {'addr': 0x0067ed38 + scr * 0x40, 'val': {'scratch': 0x100}},
              {'addr': 0x0067ed78 + scr * 0x40, 'val': {'scratch': 0x100}}]
    return _menu_common(s)


_MENU = dict(signature={'ret': 'void', 'args': []}, observe=[_VB, _RING, {'addr': {'scratch': 0}, 'len': 0x68}],
             scratch_size=0x200)
FIXES['menu_sprite_dispatch_a'] = _e(
    path1_tests=[_ma(0, 0x80, 1, 0, 0, 2, 1, 0), _ma(0, 0x10, 0, 0, 0, 0, 0, 0), _ma(1, 0x40, 0, 0, 3, 1, 0, 1),
                 _ma(0, 0xff, 2, 1, 0, 3, 5, 1), _ma(1, 0xc0, 0, 1, 0, 0, 1, 1)],
    path2_tests=[_ma(0, 0x80, 1, 0, 0, 2, 1, 0)], **_MENU)
FIXES['menu_sprite_dispatch_b'] = _e(
    path1_tests=[_mb(0, 0x80, 1, 0, 0, 1), _mb(0, 0x10, 0, 0, 0, 0), _mb(1, 0x20, 2, 3, 1, 2),
                 _mb(0, 0xff, 0, 2, 0, 0)],
    path2_tests=[_mb(0, 0x80, 1, 0, 0, 1)], **_MENU)
FIXES['menu_sprite_dispatch_c'] = _e(
    path1_tests=[_mc(0, 0x80, [0, 0, 0, 0], [1, 0, 1, 0], [0, 1, 2, 3], [1, 0, 1, 0]),
                 _mc(2, 0x80, [0, 0, 0, 0], [1, 1, 1, 1], [0, 1, 2, 3], [1, 1, 1, 1]),
                 _mc(0, 0x10, [0, 0, 0, 0], [1, 1, 1, 1], [0, 1, 2, 3], [1, 1, 1, 1]),
                 _mc(1, 0x40, [0, 0xc3, 0, 0xc3], [1, 1, 0, 0], [4, 5, 6, 7], [0, 1, 1, 0]),
                 # last drawn field has only a high byte set (an int8 read sees 0); alpha 0x1a0 -> colour byte 0xa0
                 _mc(0, 0x1a0, [0, 0, 0, 0], [0, 1, 0, 0], [3, 2, 1, 0], [0, 0, 0, 0x100]),
                 # mode 3 skips the alpha guard; head 0x1c3 is not the 0xc3 sentinel as a dword
                 _mc(3, -5, [0x1c3, 0, 0, 0], [1, 0, 1, 1], [7, 0, 5, 2], [0x01000000, 0, 7, 0]),
                 # screen 1: descriptor 0x0067edb8 / dim flags 0x0067ed84.. (stride 0x40, not 0x10)
                 _mc(0, 0xc0, [0, 0, 0, 0], [0, 1, 0, 1], [1, 3, 5, 7], [1, 0, 0x10000, 0], scr=1),
                 _mc(1, 0x70, [0, 0, 0, 0], [1, 0, 0, 1], [6, 4, 2, 0], [0, 0x200, 1, 0x80000000], scr=1)],
    path2_tests=[_mc(0, 0x80, [0, 0, 0, 0], [1, 0, 1, 0], [0, 1, 2, 3], [1, 0, 1, 0])],
    boot_settle_s=8.0, **_MENU)   # run_diff/run_verify_hook: menu sprite dictionaries load after LUT-ready


# ── 0x00430830 SplitScreenTrackAssignment(p) ──────────────────────────────────────────────────────────────────────────
# Disasm 0x00430848: lea eax,[eax+eax*2]; shl eax,4 -> p*0x30; switch DAT_0067e9fc picks column 0x007f0a44 (2/3/10),
# 0x48 (4), 0x54 (5), 0x4c (6), 0x60 (7), 0x64 (8), 0x68 (9). Seed the whole 0x007f0a40..0x007f0c90 table with
# distinct values so every column/row reads differently. Old vectors ran at the menu mode (returns 0).
_UNL_BASE, _UNL_END = 0x007f0a40, 0x007f0c90


def _ss(mode, p):
    s = [{'addr': 0x0067e9fc, 'val': mode}]
    s += [{'addr': a, 'val': 0x11000000 | a} for a in range(_UNL_BASE, _UNL_END, 4)]
    return {'args': [p], 'seed': s}


FIXES['split_screen_track_assignment'] = _e(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[_ss(m, p) for m, p in ((2, 0), (2, 1), (3, 2), (4, 1), (5, 3), (6, 2), (7, 1), (8, 3), (9, 1),
                                         (10, 2), (0, 1), (11, 1))],
    path2_tests=[_ss(4, 1)])


# ── 0x00432080 RaceEndCheckFinish(p) ──────────────────────────────────────────────────────────────────────────────────
# DAT_0067f19c = 0; if DAT_0067eca4 == 0: DAT_007f1a0c 1 -> pad id DAT_007f1a14, col +1 edge (0x007f1041/0x007f1501
# + id*0x4c); 2 -> 4 slots 0x007f1a14 + i*0x10 gated by FUN_0040e470(i) = *(*(0x005f2770) + 0x34 + i*4) == 1;
# 0x1000 -> 8 fixed pads; other -> return 0. On a hit: FUN_0042d3a0 (zeroes 0x0067ed78..0x0067f0bc), DAT_0067ecac = p,
# e844 = e9f8 = e914 = 0, f19c = 2, return 1. Old vectors returned 0 at the menu. 0x005f2770 is redirected to a
# harness scratch so the slot gate is controlled.
def _btn(col, held, prev):
    s = []
    for k in range(8):
        s.append({'addr': 0x007f1040 + col + k * 0x4c, 'val': 1 if (held >> k) & 1 else 0, 'size': 1})
        s.append({'addr': 0x007f1500 + col + k * 0x4c, 'val': 1 if (prev >> k) & 1 else 0, 'size': 1})
    return s


def _re(p, eca4, mode, held, prev, slots=(-1, -1, -1, -1), gate=(0, 0, 0, 0), extra=()):
    s = [{'addr': 0x0067eca4, 'val': eca4}, {'addr': 0x007f1a0c, 'val': mode},
         {'addr': 0x005f2770, 'val': {'scratch': 0}}] + _btn(1, held, prev)
    s += [{'addr': 0x007f1a14 + i * 0x10, 'val': v & 0xFFFFFFFF} for i, v in enumerate(slots)]
    s += [{'addr': {'scratch': 0x34 + i * 4}, 'val': g} for i, g in enumerate(gate)]
    s += [{'addr': a, 'val': v & 0xFFFFFFFF} for a, v in extra]
    return {'args': [p], 'seed': s}


_RE_OBS = [{'addr': a, 'len': 4, 'fill': 0xA5} for a in (0x0067f19c, 0x0067ecac, 0x0067e844, 0x0067e9f8, 0x0067e914)]
_RE_OBS += [{'addr': 0x0067eca4, 'len': 4}, {'addr': 0x0067ed78, 'len': 0x344, 'fill': 0xA5}]
FIXES['race_end_check_finish'] = _e(
    signature={'ret': 'int', 'args': ['int32']}, fold_ret=True, observe=_RE_OBS, scratch_size=0x80,
    path1_tests=[_re(7, 0, 0x1000, 0x01, 0x00), _re(7, 0, 0x1000, 0x80, 0x80), _re(9, 0, 0x1000, 0x81, 0x01),
                 _re(7, 1, 0x1000, 0x01, 0x00), _re(5, 0, 1, 0x04, 0x00, (2, -1, -1, -1)),
                 _re(5, 0, 1, 0x04, 0x04, (2, -1, -1, -1)), _re(5, 0, 1, 0x04, 0x00, (-1, -1, -1, -1)),
                 _re(3, 0, 2, 0x02, 0x00, (-1, -1, 1, -1), (0, 0, 1, 0)),
                 _re(3, 0, 2, 0x02, 0x00, (-1, -1, 1, -1), (1, 1, 0, 1)),
                 _re(4, 0, 2, 0x01, 0x00, (-1, -1, -1, -1), (1, 1, 1, 1), ((0x007f1a18, 0),)),
                 _re(6, 0, 3, 0xff, 0x00)],
    path2_tests=[_re(7, 0, 0x1000, 0x01, 0x00)])


# ── 0x00467300 VehicleCollisionWinTrigger(p) ──────────────────────────────────────────────────────────────────────────
# If *(0x00881f90 + p*0xd04) == 0 && *(0x008815a4 + p*0xd04) != 0: FUN_00413c70(i, 3, 0) for i 0..3 (writes
# 0x0063bc68 + (i*5+3)*12 = 0, and 0x0063bc60 + same = 0x3c23d70a when it equals 0.0), DAT_007f0fd4 = p,
# DAT_007f0fdc = 6000. Old vectors ran on live records (gate closed).
def _vw(p, f90, a4, zero_mask=0):
    s = [{'addr': 0x00881f90 + p * 0xd04, 'val': f90}, {'addr': 0x008815a4 + p * 0xd04, 'val': a4}]
    for i in range(4):
        s.append({'addr': 0x0063bc60 + (i * 5 + 3) * 12, 'val': 0 if (zero_mask >> i) & 1 else _f(2.0)})
    return {'args': [p], 'seed': s}


FIXES['vehicle_collision_win_trigger'] = _e(
    signature={'ret': 'void', 'args': ['int32']},
    observe=[{'addr': 0x007f0fd4, 'len': 4, 'fill': 0xA5}, {'addr': 0x007f0fdc, 'len': 4, 'fill': 0xA5},
             {'addr': 0x0063bc60 + 3 * 12, 'len': 0x100, 'fill': 0xA5}],
    path1_tests=[_vw(0, 0, 1), _vw(1, 0, 5, 0xf), _vw(2, 1, 1), _vw(3, 0, 0), _vw(3, 0, 0xDEADBEEF, 0x5),
                 _vw(15, 0, 2, 0x2)],
    path2_tests=[_vw(0, 0, 1)])


# ── 0x0046be10 Write46be10(src*, idx, off, val) ───────────────────────────────────────────────────────────────────────
# Disasm 0x0046be10..0x0046be46: idx >= 0x10 (unsigned) -> return 0; else k = (idx*0x341 + off)*4,
# *(0x0088219c + k) = *src, *(0x0088221c + k) = val, return 1 (0x0046be41 mov eax,1). The deref_struct_set entry
# never compared the return.
def _w(idx, off, src, val):
    return {'args': [{'scratch': 0}, idx & 0xFFFFFFFF, off, val], 'seed': [{'addr': {'scratch': 0}, 'val': src}]}


FIXES['write_46be10'] = _e(
    signature={'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32', 'uint32']}, fold_ret=True,
    observe=[{'addr': 0x0088219c + i * 0xd04, 'len': 0xa0, 'fill': 0xA5} for i in (0, 1, 15)],
    scratch_size=0x10, nscalar=None, abs_observe=None, seed_byte=None,
    path1_tests=[_w(0, 0, 0x11111111, 0x22222222), _w(1, 3, 0xDEADBEEF, 7), _w(15, 7, 5, 0xCAFEBABE),
                 _w(16, 0, 1, 2), _w(-1, 0, 1, 2), _w(0, 5, 0, 0)],
    path2_tests=[_w(1, 3, 0xDEADBEEF, 7)])


# ── 0x0046cbe0 VehicleCarStateSet(idx, a, b) ──────────────────────────────────────────────────────────────────────────
# Disasm 0x0046cbe0..0x0046cc0b: idx >= 0x10 (unsigned jb) -> return 0; else dword *(0x00881f90 + idx*0xd04) = a,
# dword *(0x00881f94 + idx*0xd04) = b, return 1. The entry declared zero args.
def _cs(idx, a, b):
    return {'args': [idx & 0xFFFFFFFF, a, b], 'seed': []}


FIXES['vehicle_car_state_set'] = _e(
    signature={'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']}, fold_ret=True,
    observe=[{'addr': 0x00881f90 + i * 0xd04, 'len': 8, 'fill': 0xA5} for i in (0, 1, 15)],
    path1_tests=[_cs(0, 2, 0x1234), _cs(1, 0xff, 7), _cs(15, 0x12345678, 0xCAFEBABE), _cs(16, 1, 2), _cs(0, 0, 0)],
    path2_tests=[_cs(1, 0xff, 7)])
# idx = -1 is NOT included: the port compares signed (Vehicle/SmallLeaves_o5.cpp:53) and would write 0xd04 bytes
# below 0x00881f90 (unobserved live memory); the original returns 0.
