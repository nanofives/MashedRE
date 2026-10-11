"""negctl re-earn, batch b06 (log/negctl_reearn/batches/b06.tsv).

Decomp from re/tools/decomp_pc.py (pool slot, read-only, 2026-10-11) and re/tools/disasm_va.py. Every
scalars_to_scattered_globals entry freezes the game's main thread: the globals it seeds are live game globals.

Shared leaves used as observable stand-ins (all original code, cdecl, caller cleans):
  0x004099e0  mov eax,[esp+4]; mov [0x008a95a0],eax; ret   -> records its first argument in DAT_008a95a0
  0x0042b8b6  ret
Fake RwGlobals: DAT_007d3ff8 pointed at a harness scratch block whose used slots hold one of the leaves above.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

_SC = {'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'target_global': None,
       'needs_booted_race': None, 'stub_at': None, 'stub_nargs': None, 'stub_ret': None}


def _fix(**kw):
    d = dict(_SC)
    d.update(kw)
    return d


_REC = 0x004099e0   # records arg0 in DAT_008a95a0
_RET = 0x0042b8b6   # bare ret
_E9FC = 0x0067e9fc  # FUN_0042f6a0 returns it
_EA64 = 0x0067ea64  # FUN_0042f500 returns it
_D704 = 0x0066d704  # FUN_00426ba0 returns it
_BA8C = 0x0063ba8c
_FD0 = 0x007f0fd0


# ── 0x0040b6b0 ModeScoreGetBySlot: return (&DAT_008a9530)[param_1] ───────────────────────────────────────────────────
# Old vectors read the live array, all zero at the menu. Seed distinct values in the 4-dword array.
def _arr(vals_by_addr):
    return [{'addr': a, 'val': v} for a, v in vals_by_addr]


_TBL = [(0x008a9530 + 4 * k, 0x10203000 + 0x11 * k + 1) for k in range(16)]   # 0x008a9530..0x008a956c
FIXES['mode_score_get_by_slot'] = _fix(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [i], 'seed': _arr(_TBL)} for i in (0, 1, 2, 3, 2, 0)],
    path2_tests=[{'args': [1], 'seed': _arr(_TBL)}])

# ── 0x0040b7b0 PlayerHotkeyTableGet: switch(param_2) 0 -> 0x008a9530[i], 1 -> 0x008a9560, 2 -> 0x008a9540,
# 3 -> 0x008a9550, default 0. Same seeded 16-dword block.
FIXES['player_hotkey_table_get'] = _fix(
    signature={'ret': 'uint32', 'args': ['int32', 'uint32']}, fold_ret=True, observe=[],
    path1_tests=[{'args': [i, s], 'seed': _arr(_TBL)} for s in (0, 1, 2, 3, 4) for i in (0, 3)]
               + [{'args': [2, 1], 'seed': _arr(_TBL)}, {'args': [1, 2], 'seed': _arr(_TBL)}],
    path2_tests=[{'args': [1, 1], 'seed': _arr(_TBL)}])

# ── 0x0040e470 CarSlotStateGet: return *(PTR_PTR_005f2770 + param_1*4 + 0x34) ─────────────────────────────────────
# Point 0x005f2770 at a scratch record with distinct dwords at +0x30..+0x50.
def _css(i):
    s = [{'addr': 0x005f2770, 'val': {'scratch': 0}}]
    s += [{'addr': {'scratch': 0x30 + 4 * k}, 'val': 0xA0B0C000 + 7 * k + 3} for k in range(9)]
    return {'args': [i], 'seed': s}


FIXES['car_slot_state_get'] = _fix(
    signature={'ret': 'uint32', 'args': ['int32']}, fold_ret=True, observe=[], scratch_size=0x80,
    path1_tests=[_css(i) for i in (0, 1, 2, 3, -1, 4, 7)], path2_tests=[_css(2)])

# ── 0x0040d020 Track_LoadByIndex: FUN_00426e10(**(param_1 + param_2*4)) ──────────────────────────────────────────
# FUN_00426e10 is the 723-byte track loader (piz open, Lua, AI data); the recorder replaces it. buf0 = table of
# pointers (slot param_2 -> buf1+off), buf1 holds the value the double deref must forward.
def _tl(idx, val, off=0):
    return {'scalars': [idx], 'seed': [{'buf': 0, 'off': idx * 4, 'ptr_to': 1},
                                       {'buf': 1, 'off': 0, 'type': 'u32', 'value': val}]}


FIXES['track_loadbyindex_40d020'] = {
    'arg_type': 'stub_dispatch_observe', 'needs_booted_race': None,
    'signature': {'ret': 'void', 'args': ['pointer', 'int32']},
    'num_bufs': 2, 'buf_size': 0x40, 'arg_layout': [{'buf': 0}, {'i32': True}],
    'stub_at': ['0x00426e10'], 'stub_nargs': 1, 'stub_ret': 0, 'stub_abi': 'mscdecl',
    'observe_calls': True, 'observe_ret': False,
    'path1_tests': [_tl(0, 3), _tl(1, 7), _tl(2, 0x12), _tl(5, 0xDEAD), _tl(3, 0)],
    'path2_tests': [_tl(1, 7)],
}

# ── 0x0040d440 Course_LoadCurrent: FUN_0040d270(PTR_PTR_005f2770, DAT_0063ba7c); DAT_0063ba78 = DAT_0063ba7c ─────
# FUN_0040d270 (441 bytes, course finish/load) is replaced by the recorder; it logs both live arguments.
FIXES['course_loadcurrent_40d440'] = {
    'arg_type': 'stub_dispatch_observe', 'needs_booted_race': None, 'observe': None,
    'signature': {'ret': 'void', 'args': []},
    'num_bufs': 1, 'buf_size': 0x10, 'arg_layout': [],
    'stub_at': ['0x0040d270'], 'stub_nargs': 2, 'stub_ret': 0, 'stub_abi': 'mscdecl',
    'observe_calls': True, 'observe_ret': False,
    'path1_tests': [{'scalars': []}, {'scalars': []}, {'scalars': []}],
    'path2_tests': [{'scalars': []}],
}


# ── 0x0040df60 ConditionalRenderSubPass ──────────────────────────────────────────────────────────────────────────
# 4 < DAT_0063ba8c < 7 && 2 < FUN_0042f6a0() (DAT_0067e9fc) < 6 && DAT_007f0fd0 in {4,7,8,9,10} -> FUN_00401f10().
# FUN_00401f10: (*(RwGlobals+0x20))(0x14,1),(8,0),(6,0); clump renders FUN_004e6680 over *DAT_00636ac0,
# DAT_00636ac0[0x10..] (count FUN_0040e340 = DAT_008a94d0) and [0x14..] (count FUN_00412130: fd0 4->1, 7->0, 8->1,
# 9->1, 10->6, table 0x0041216c); then (6,1),(8,1),(0x14,2). Fake RwGlobals +0x20 = recorder, so DAT_008a95a0 ends
# 0x14 iff FUN_00401f10 ran. Every clump slot points at a scratch clump whose +8 list head points at itself
# (FUN_004e6680 0x004e668d: empty list, no atomic dispatched).
def _crs(ba8c, e9fc, fd0, n=1):
    s = [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x20}, 'val': _REC},
         {'addr': {'scratch': 0x208}, 'val': {'scratch': 0x208}}, {'addr': {'scratch': 0x20c}, 'val': {'scratch': 0x208}},
         {'addr': 0x00636ac0, 'val': {'scratch': 0x300}}, {'addr': {'scratch': 0x300}, 'val': {'scratch': 0x200}},
         {'addr': 0x008a94d0, 'val': n},
         {'addr': _BA8C, 'val': ba8c}, {'addr': _E9FC, 'val': e9fc}, {'addr': _FD0, 'val': fd0}]
    s += [{'addr': {'scratch': 0x340 + 4 * k}, 'val': {'scratch': 0x200}} for k in range(10)]
    return {'args': [], 'seed': s}


FIXES['conditional_render_sub_pass'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x400,
    observe=[{'addr': 0x008a95a0, 'fill': 0xA5}],
    path1_tests=[_crs(5, 3, 4), _crs(6, 5, 7, 0), _crs(5, 4, 8), _crs(6, 3, 9), _crs(5, 5, 10),
                 _crs(4, 3, 4), _crs(7, 3, 4), _crs(5, 2, 4), _crs(5, 6, 4), _crs(5, 3, 5), _crs(5, 3, 2)],
    path2_tests=[_crs(5, 3, 4)])


# ── 0x0040dfc0 HudIngameDispatch ─────────────────────────────────────────────────────────────────────────────────
# FUN_00426ba0 (DAT_0066d704) == 0 -> return; DAT_0063ba8c < 5 or > 7 -> return; 5..6 with FUN_0042f6a0() == 0xb
# -> FUN_00403160() and the trailing `if (FUN_0042f6a0() == 0xb) return` skips FUN_0041db80. Every other branch is a
# HUD draw. FUN_00403160 (0x00403160) on a scratch camera (DAT_006905b0; FUN_004671a0(0) returns it when
# FUN_0042b930() = DAT_0067ecb0 != 3): FUN_004c19f0(cam) = jmp [cam+0x1c] = 0x004099e0 (DAT_008a95a0 = cam);
# FUN_004c1c80(cam, {0.8f,0.8f}) writes cam+0x68/+0x6c and +0x70/+0x74 = 1.0f/x (fld [0x005cc320]);
# FUN_004c1a00(cam) = jmp [cam+0x18] = 0x00430820 (returns DAT_0067ed6c, seeded 0), so the branch holding the
# render-state calls and FUN_00402fb0's two FUN_00427e00 text draws is skipped (run_diff attaches during boot and
# those faulted at 0x0 on both arms, iterations 1-2); then FUN_004c1c80(cam, saved +0x68/+0x6c) and FUN_004c1a00.
# Observable: DAT_008a95a0 and cam+0x68..+0x77 (+0x70/+0x74 = 1/seed).
def _hud(d704, ba8c, e9fc, v=2.0):
    s = [{'addr': _D704, 'val': d704}, {'addr': _BA8C, 'val': ba8c}, {'addr': _E9FC, 'val': e9fc},
         {'addr': 0x0067ecb0, 'val': 0}, {'addr': 0x0067ed6c, 'val': 0}, {'addr': 0x006905b0, 'val': {'scratch': 0}},
         {'addr': {'scratch': 0x18}, 'val': 0x00430820}, {'addr': {'scratch': 0x1c}, 'val': _REC},
         {'addr': {'scratch': 0x68}, 'val': _f(v)}, {'addr': {'scratch': 0x6c}, 'val': _f(v * 0.5)}]
    return {'args': [], 'seed': s}


FIXES['hud_ingame_dispatch'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x100,
    observe=[{'addr': 0x008a95a0, 'fill': 0xA5}, {'addr': {'scratch': 0x68}, 'len': 0x10}],
    path1_tests=[_hud(1, 5, 0xb, 2.0), _hud(1, 6, 0xb, 4.0), _hud(1, 5, 0xb, 0.25), _hud(0, 5, 0xb, 2.0),
                 _hud(1, 4, 0xb, 2.0), _hud(1, 8, 0xb, 2.0), _hud(1, -1, 0xb, 2.0)],
    path2_tests=[_hud(1, 5, 0xb, 2.0)])


# ── 0x00411170 TimeTrialRecordPlayback(param_1) ──────────────────────────────────────────────────────────────────
# FUN_00411600(DAT_007f0ff4) (returns at once unless DAT_0067e9fc == 2); FUN_00411ae0(param_1, DAT_007f0ff4):
# DAT_0063bb24 == 0 -> DAT_0063bb1c = tick, clamped to *(DAT_0063bb10 + 0x174) when DAT_0063bb10 != 0;
# FUN_00410d10() returns 0 when FUN_00443080() (DAT_00897ffc) == 1 -> return. Observe DAT_0063bb1c.
# The DAT_0063ba8c = 7 / FUN_00430290 tail is not driven (FUN_00410510 + state-7 entry are whole-game transitions).
def _ttp(p1, tick, cap=None):
    s = [{'addr': _E9FC, 'val': 0}, {'addr': 0x0063bb24, 'val': 0}, {'addr': 0x007f0ff4, 'val': tick},
         {'addr': 0x00897ffc, 'val': 1}]
    if cap is None:
        s.append({'addr': 0x0063bb10, 'val': 0})
    else:
        s += [{'addr': 0x0063bb10, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x174}, 'val': cap}]
    return {'args': [p1], 'seed': s}


FIXES['time_trial_record_playback'] = _fix(
    signature={'ret': 'void', 'args': ['int32']}, scratch_size=0x200,
    observe=[{'addr': 0x0063bb1c, 'fill': 0xA5}],
    path1_tests=[_ttp(0, 100), _ttp(1, 12345), _ttp(2, 100, 50), _ttp(0, 40, 50), _ttp(3, 0x7fffffff, 0x10000)],
    path2_tests=[_ttp(0, 100)])


# ── 0x004114e0 ReplayCleanup ─────────────────────────────────────────────────────────────────────────────────────
# DAT_0063bb04 / DAT_0063bb08 != 0 -> = FUN_00483a40(p) (frees *(p+0x14) then p via RwGlobals+0x10c, returns 0);
# DAT_0063bb14 = DAT_0063bb10 = DAT_0063bb2c = 0. Fake RwGlobals +0x10c = recorder (last freed pointer).
def _rc(a, b):
    s = [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x10c}, 'val': _REC},
         {'addr': {'scratch': 0x214}, 'val': 0x11112222}, {'addr': {'scratch': 0x314}, 'val': 0x33334444},
         {'addr': 0x0063bb04, 'val': {'scratch': 0x200} if a else 0},
         {'addr': 0x0063bb08, 'val': {'scratch': 0x300} if b else 0}]
    return {'args': [], 'seed': s}


FIXES['replay_cleanup'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x400, pre_fill_byte=0xA5,
    observe=[{'addr': 0x0063bb04}, {'addr': 0x0063bb08}, {'addr': 0x0063bb10, 'len': 8}, {'addr': 0x0063bb2c},
             {'addr': 0x008a95a0}],
    path1_tests=[_rc(1, 1), _rc(1, 0), _rc(0, 1), _rc(0, 0)], path2_tests=[_rc(1, 1)])


# ── 0x00411580 / 0x004115c0 ReplayGetBestTime / ReplayGetCurrentTime(idx, p2, p3, p4) ────────────────────────────
# Disasm 0x00411580: obj = DAT_0063bb10 (0x004115c0: DAT_0063bb14); if obj && *(obj+0x17c+idx*4): push p4,p3,p2,
# idx, obj; call 0x00411530; add esp,0x14; return 1. 0x00411530 -> 0x00411350(*(obj+0x17c+idx*4), p2, p3, p4)
# (fild, *p3 = seconds, *p2 = frac float, *p4 = minutes). Outs are scratch dwords pre-filled 0xA5.
def _gt(glob, idx, val):
    s = [{'addr': glob, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x17c + 4 * idx}, 'val': val}]
    return {'args': [idx, {'scratch': 0x400}, {'scratch': 0x410}, {'scratch': 0x420}], 'seed': s}


def _gt0(glob):
    return {'args': [0, {'scratch': 0x400}, {'scratch': 0x410}, {'scratch': 0x420}], 'seed': [{'addr': glob, 'val': 0}]}


for _k, _g in (('replay_get_best_time', 0x0063bb10), ('replay_get_current_time', 0x0063bb14)):
    FIXES[_k] = _fix(
        signature={'ret': 'uint32', 'args': ['int32', 'pointer', 'pointer', 'pointer']}, fold_ret=True,
        scratch_size=0x480, scratch_fill=0xA5,
        observe=[{'addr': {'scratch': 0x400}, 'len': 4}, {'addr': {'scratch': 0x410}, 'len': 4},
                 {'addr': {'scratch': 0x420}, 'len': 4}],
        path1_tests=[_gt(_g, 0, 3000), _gt(_g, 1, 123456), _gt(_g, 0, 0), _gt(_g, 1, 59), _gt0(_g)],
        path2_tests=[_gt(_g, 0, 3000)])


# ── 0x00411600 ReplayRecordFrame(param_1) ────────────────────────────────────────────────────────────────────────
# DAT_0067e9fc != 2 -> return. FUN_0046d4a0(&local_4, 0): local_4 = &DAT_00881ec8 + DAT_00881f48*0x10 (dwords).
# FUN_004829d0(DAT_0063bb14, local_4, param_1, 10): with rec+0x194 == 0 it copies *(local_4+0x30..0x38) into the
# current frame (rec+0x1c) +0x10..0x18, FUN_00546b10(frame, local_4), frame+0x20 = param_1, links to frame+0x1c,
# rec+0x174 = rec+0x178 = param_1, rec+0x168++, rec+0x170 = 2, rec+0x194 = 1. DAT_0063bb20 = 0. Checkpoint table
# FUN_00407a00(0,k) = DAT_008a9914[k]: nonzero and rec+0x17c/+0x180 == 0 -> slot = param_1.
# DAT_0063bb10 = 0 keeps the "recording error" logger (0x004987b0) out.
def _rf(p1, e9fc=2, ck0=0, ck1=0):
    s = [{'addr': _E9FC, 'val': e9fc}, {'addr': 0x0063bb10, 'val': 0}, {'addr': 0x0063bb14, 'val': {'scratch': 0}},
         {'addr': 0x00881f48, 'val': 0},
         {'addr': 0x00881ec8 + 0x30, 'val': _f(1.5)}, {'addr': 0x00881ec8 + 0x34, 'val': _f(-2.25)},
         {'addr': 0x00881ec8 + 0x38, 'val': _f(7.0)},
         {'addr': 0x008a9914, 'val': ck0}, {'addr': 0x008a9918, 'val': ck1},
         {'addr': {'scratch': 0x1c}, 'val': {'scratch': 0x200}}, {'addr': {'scratch': 0x21c}, 'val': {'scratch': 0x300}}]
    return {'args': [p1], 'seed': s}


FIXES['replay_record_frame'] = _fix(
    signature={'ret': 'void', 'args': ['int32']}, scratch_size=0x400,
    observe=[{'addr': {'scratch': 0}, 'len': 0x400}, {'addr': 0x0063bb20, 'fill': 0xA5}],
    path1_tests=[_rf(100), _rf(250, ck0=1), _rf(77, ck1=1), _rf(5, e9fc=3)],
    path2_tests=[_rf(100)])


# ── 0x00411750 ReplayStartLap ────────────────────────────────────────────────────────────────────────────────────
# DAT_0067e9fc != 2 -> return. DAT_0063bb10 -> FUN_00483a30 (+0x1c = +0x18); DAT_0063bb14 -> FUN_00482860 (ring
# reset over the +0x14 buffer, count +0x16c) then +0x17c = +0x180 = 0; DAT_0063bb1c = 0.
def _sl(e9fc=2, a=1, b=1):
    s = [{'addr': _E9FC, 'val': e9fc},
         {'addr': 0x0063bb10, 'val': {'scratch': 0} if a else 0},
         {'addr': 0x0063bb14, 'val': {'scratch': 0x200} if b else 0},
         {'addr': {'scratch': 0x18}, 'val': 0xCAFE0001},
         {'addr': {'scratch': 0x214}, 'val': {'scratch': 0x400}}, {'addr': {'scratch': 0x36c}, 'val': 1},
         {'addr': {'scratch': 0x37c}, 'val': 0x77}, {'addr': {'scratch': 0x380}, 'val': 0x88}]
    return {'args': [], 'seed': s}


FIXES['replay_start_lap'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x500, scratch_fill=0xA5,
    observe=[{'addr': {'scratch': 0}, 'len': 0x500}, {'addr': 0x0063bb1c, 'fill': 0xA5}],
    path1_tests=[_sl(), _sl(a=0), _sl(b=0), _sl(a=0, b=0), _sl(e9fc=3)],
    path2_tests=[_sl()])


# ── 0x004117b0 ReplaySave ────────────────────────────────────────────────────────────────────────────────────────
# DAT_005f29c8 != 0 -> return. DAT_0063bb10 != 0 && FUN_00430820() (DAT_0067ed6c) == 0: sprintf via the REAL
# RwGlobals +0xc4 (cdecl: 0x00411843 add esp,0x30 pops both calls), logger 0x004987b0, FUN_00483ca0(DAT_0063bb10,
# &DAT_008a94a8) copies 0x67 dwords of the record to the first free entry of *DAT_008a94a8 (scratch, empty), then
# record[0x5b]*9 dwords (0 here); second sprintf; FUN_004099a0 (DAT_008a95ac = 0: no-op). Then DAT_005f29c8 = 1.
def _rs(latch, bb10, ed6c):
    s = [{'addr': 0x005f29c8, 'val': latch}, {'addr': 0x0063bb10, 'val': {'scratch': 0} if bb10 else 0},
         {'addr': 0x0067ed6c, 'val': ed6c}, {'addr': 0x008a94a8, 'val': {'scratch': 0x200}},
         {'addr': 0x008a95ac, 'val': 0}, {'addr': 0x00644158, 'val': 7},
         {'addr': {'scratch': 0x00}, 'val': 0x11111111}, {'addr': {'scratch': 0x08}, 'val': 0x22222222},
         {'addr': {'scratch': 0x100}, 'val': 0x33333333}, {'addr': {'scratch': 0x198}, 'val': 0x44444444}]
    return {'args': [], 'seed': s}


FIXES['replay_save'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x400,
    observe=[{'addr': 0x005f29c8, 'fill': 0xA5}, {'addr': {'scratch': 0x200}, 'len': 0x1a0}],
    # _rs(0, 1, 0) (the inner body) killed the process in run 1: the compiled port calls the +0xc4 slot as __thiscall
    # without stack cleanup (asi RVA 0xf0e5/0xf10d) -> ESP off by 0x20 at the /GS check. Row demoted; vector left out.
    path1_tests=[_rs(0, 0, 0), _rs(1, 1, 0), _rs(0, 1, 1)],
    path2_tests=[_rs(0, 0, 0)])
