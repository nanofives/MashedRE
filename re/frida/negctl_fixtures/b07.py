"""negctl re-earn, batch b07 (log/negctl_reearn/batches/b07.tsv). Decomp/disasm cited per entry.

All fixtures use scalars_to_scattered_globals with per-test seeds and freeze_main. Dispatchers that call through
object slots `call [obj+0x48]` (cdecl, obj pushed) get harness fake objects whose +0x48 slot is 0x0041e130
(disasm: mov eax,[esp+4]; mov [0x0063d7e0],eax; ret), so the last object dispatched lands in DAT_0063d7e0,
which is observed pre-filled 0xA5. Fake k sits at scratch + k*0x50.
"""

FIXES = {}

_SETTER = 0x0041e130        # writes its stack arg to DAT_0063d7e0
_SINK = 0x0063d7e0
_FAKE = 0x50


def _fake_seeds(n):
    return [{'addr': {'scratch': k * _FAKE + 0x48}, 'val': _SETTER} for k in range(n)]


# ── 0x00417730 VehicleRaceAngleGet(int i): mov eax,[esp+4]; fld dword [eax*4+0x0089a880]; ret ───────────────────────
# Old vectors read the array at the menu (all 0.0). Seed 8 distinct float bit patterns, fold the float return.
_ANG = (0x3f800000, 0xc2c80000, 0x40490fdb, 0x00000001, 0x7f7fffff, 0xbf000000, 0x42f60000, 0x3dcccccd)


def _ang(i):
    return {'args': [i], 'seed': [{'addr': 0x0089a880 + 4 * j, 'val': v} for j, v in enumerate(_ANG)]}


FIXES['vehicle_race_angle_get'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'float', 'args': ['int32']},
    'fold_ret': True, 'observe': [], 'freeze_main': True, 'lut_root_delta': None,
    'path1_tests': [_ang(i) for i in (0, 1, 2, 3, 4, 5, 6, 7)], 'path2_tests': [_ang(0), _ang(2)],
}

# ── 0x0041e130 TimerTrackSetter(u32 v): DAT_0063d7e0 = v ─────────────────────────────────────────────────────────────
FIXES['timer_track_setter'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['uint32']},
    'observe': [{'addr': _SINK, 'len': 4, 'fill': 0xA5}], 'freeze_main': True,
    'path1_tests': [{'args': [v]} for v in (0, 1, 0x42424242, 0xdeadbeef, 0x80000001, 0x3f800000, 0xffffffff, 123)],
    'path2_tests': [{'args': [0x42424242]}],
}

# ── 0x0041b630 HudSlotLoopB630: for 4 records at 0x0063c8d0 stride 0x74: if (+0x6c) FUN_0041b340 (EAX = record) ─────
# FUN_0041b340 dispatches [rec+0x40] and [rec+0] always, then by flags at rec+0x68: 0x800 -> [5],[4],(0x40)[6];
# else 0x1000 -> [2],[1],(0x40)[3]; bits 1,2,4,8,0x10,0x20 -> [7..0xc]; 0x2000 [0xd]; 0x4000 [0xe]; 0x8000 [0xf].
# At the menu every enable was 0, so the port's loop never called anything. Point all 17 slots at fakes.
def _b630(recs):
    s = _fake_seeds(17)
    for r in range(4):
        base = 0x0063c8d0 + r * 0x74
        on = r in recs
        s.append({'addr': base + 0x6c, 'val': 1 if on else 0})
        if on:
            s.append({'addr': base + 0x68, 'val': recs[r]})
            s += [{'addr': base + 4 * k, 'val': {'scratch': k * _FAKE}} for k in range(17)]
    return {'args': [], 'seed': s}


FIXES['hud_slot_loop_b630'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': _SINK, 'len': 4, 'fill': 0xA5}], 'scratch_size': 0x600, 'scratch_fill': 0,
    'freeze_main': True,
    'path1_tests': [_b630({}), _b630({0: 0}), _b630({1: 0x800}), _b630({2: 0x840}), _b630({3: 0x1000}),
                    _b630({0: 0x1040}), _b630({1: 0x1}), _b630({2: 0x20}), _b630({3: 0x2000}),
                    _b630({0: 0x4000}), _b630({1: 0x8000}), _b630({2: 0xffff}), _b630({0: 0x8000, 3: 0x4}),
                    _b630({3: 0x4, 1: 0x8000})],
    'path2_tests': [_b630({1: 0x800})],
}

# ── 0x0041ccc0 HudSlotLoopCcc0: for 4 records at 0x0063ce20 stride 0x114: if (+0x110) FUN_0041c9a0 (EAX = record) ──
# FUN_0041c9a0: flag +0x64 -> [+0xe4]; +0x08 -> *(**(*([+0x88]+0x18)+0x20)+4) = 0xff323232, [+0x88]; same for +0x14 /
# [+0x94]; loop i=0..31 (iVar2 = i-2, skipped for iVar2 in {0,3,5,6,7,8,9,10,0x17}): flags[i] && ptr[+0x80+4i] ->
# dispatch; then flags +0x1c..+0x30 -> [+0x9c..+0xb0]. Fakes k=0..31 for ptr slot +0x80+4k. Fake 2 (+0x88) chains
# +0x18 -> S+0xc00, [S+0xc20] = S+0xc40, [S+0xc40] = S+0xc80 (write lands at S+0xc84); fake 5 (+0x94) -> S+0xd00..d84.
def _ccc0(recs):
    s = _fake_seeds(32)
    s += [{'addr': {'scratch': 2 * _FAKE + 0x18}, 'val': {'scratch': 0xc00}},
          {'addr': {'scratch': 0xc20}, 'val': {'scratch': 0xc40}}, {'addr': {'scratch': 0xc40}, 'val': {'scratch': 0xc80}},
          {'addr': {'scratch': 5 * _FAKE + 0x18}, 'val': {'scratch': 0xd00}},
          {'addr': {'scratch': 0xd20}, 'val': {'scratch': 0xd40}}, {'addr': {'scratch': 0xd40}, 'val': {'scratch': 0xd80}}]
    for r in range(4):
        base = 0x0063ce20 + r * 0x114
        on = r in recs
        s.append({'addr': base + 0x110, 'val': 1 if on else 0})
        if on:
            fl = recs[r]
            s += [{'addr': base + 4 * i, 'val': 1 if i in fl else 0} for i in range(32)]
            s += [{'addr': base + 0x80 + 4 * k, 'val': {'scratch': k * _FAKE}} for k in range(32)]
    return {'args': [], 'seed': s}


_ALL = set(range(32))
FIXES['hud_slot_loop_ccc0'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': _SINK, 'len': 4, 'fill': 0xA5}, {'addr': {'scratch': 0xc80}, 'len': 8},
                {'addr': {'scratch': 0xd80}, 'len': 8}],
    'scratch_size': 0xe00, 'scratch_fill': 0xA5, 'freeze_main': True,
    'path1_tests': [_ccc0({}), _ccc0({0: set()}), _ccc0({1: _ALL}), _ccc0({2: {2}}), _ccc0({3: {5}}),
                    _ccc0({0: {0}}), _ccc0({1: {31}}), _ccc0({2: {25}}), _ccc0({3: {3}}), _ccc0({0: {11}}),
                    _ccc0({1: {24}}), _ccc0({2: {0, 3, 4, 13, 30}}), _ccc0({0: {1}, 3: {2, 6}}),
                    _ccc0({0: {9, 10}})],
    'path2_tests': [_ccc0({1: _ALL})],
}

# ── 0x0041ded0 HudModeGuardDispatch(p): if (DAT_0063d5e8) { push esi; push p; mov esi,0x0063d5a0; call 0x0041de80 } ──
# FUN_0041de80 (ESI = record): dispatches [+0x34],[+8],[+0],[+4],[+0x1c + 4*DAT_007f1a1c]; if (p) tail-jumps
# [+8 + 4*[+0x50]]. Old vectors ran with DAT_0063d5e8 = 0 (gate early-out). Seed the gate, the record's slots
# (fake k for slot +4k, k=0..19), DAT_007f1a1c and +0x50.
def _ded0(p, gate=1, sel=0, idx=0):
    # Slots +0x00..+0x34 only (k = 0..13): the gate DAT_0063d5e8 is the record's own +0x48 dword.
    s = _fake_seeds(14)
    s += [{'addr': 0x0063d5a0 + 4 * k, 'val': {'scratch': k * _FAKE}} for k in range(14)]
    s += [{'addr': 0x0063d5e8, 'val': gate}, {'addr': 0x007f1a1c, 'val': sel}, {'addr': 0x0063d5a0 + 0x50, 'val': idx}]
    return {'args': [p], 'seed': s}


FIXES['hud_mode_guard_dispatch'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['int32']},
    'observe': [{'addr': _SINK, 'len': 4, 'fill': 0xA5}], 'scratch_size': 0x700, 'scratch_fill': 0,
    'freeze_main': True, 'crash_equal_ok': None,
    'path1_tests': [_ded0(0, gate=0), _ded0(0), _ded0(0, sel=3), _ded0(1, idx=0), _ded0(1, idx=5),
                    _ded0(1, sel=2, idx=9)],
    'path2_tests': [_ded0(0)],
}

# ── 0x00422fd0 FrontendRaceResultsDispatch(int p): if (p < 0x10) { ... } return 0 ─────────────────────────────────
# 0x0046c5c0(p): if (p <= 0xf) { [0x008815a4 + p*0xd04] = 0; [0x008820b0 + p*0xd04] = DAT_007f1030 }
# 0x0046c790(p,1): if (p <= 0xf) [0x008815b0 + p*0xd04] = 1
# 0x004215c0(p, 50.0f, s): edx = 0x0063e4b8 + p*0x24; 0x00420de0: [edx+s*4] += 50.0f, clamped by compare with
#   [0x005cd120] -> 0x42480000
# 0x0045ba00(p,2): [0x0068d1f0 + p*4] = 2
# DAT_007f0fd0 seeded 7 so the 0x0040e470 / thunk 0x004189f0 branch is not taken. Old vectors were p = 16..20 only.
_PIDX = (0, 5, 15)


def _rr(p, clock, f0, f1):
    s = [{'addr': 0x007f1030, 'val': clock}, {'addr': 0x007f0fd0, 'val': 7}]
    for q in _PIDX + (-1,):
        s.append({'addr': 0x0063e4b8 + q * 0x24, 'val': f0})
        s.append({'addr': 0x0063e4b8 + q * 0x24 + 4, 'val': f1})
    return {'args': [p], 'seed': s}


_RR_OBS = []
for _q in _PIDX:
    _RR_OBS += [{'addr': 0x008815a4 + _q * 0xd04, 'len': 0x10, 'fill': 0xA5},
                {'addr': 0x008820b0 + _q * 0xd04, 'len': 4, 'fill': 0xA5},
                {'addr': 0x0063e4b8 + _q * 0x24, 'len': 8},
                {'addr': 0x0068d1f0 + _q * 4, 'len': 4, 'fill': 0xA5}]
_RR_OBS += [{'addr': 0x0063e4b8 - 0x24, 'len': 8}, {'addr': 0x0068d1f0 - 4, 'len': 4, 'fill': 0xA5}]
FIXES['frontend_race_results_dispatch'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['int32']},
    'fold_ret': True, 'observe': _RR_OBS, 'freeze_main': True,
    'path1_tests': [_rr(0, 0x1234, 0x00000000, 0x41200000), _rr(5, 0x55aa, 0x447a0000, 0xc2c80000),
                    _rr(15, 0x7fffffff, 0x3f800000, 0x7fc00000), _rr(16, 0x99, 0, 0), _rr(0x7fffffff, 0x99, 0, 0),
                    _rr(-1, 0x4321, 0x41200000, 0x41200000)],
    'path2_tests': [_rr(5, 0x55aa, 0x447a0000, 0xc2c80000)],
}

# ── 0x00423b00 FrontendInputDispatch: if (DAT_007f1a50 == 1) { 0x00423040; 0x00423270; 0x00423320; jmp 0x00423670 } ─
# Old vectors ran the four handlers on live, idle menu state (no pad bytes set, nothing written that was observed).
# Seed the editor state 0x007f1a54..0x007f1a98, the repeat counters 0x006440e8..0x0064410c and the pad bytes
# (held 0x007f1042..47 / 0x007f1070..76, previous 0x007f1503 / 0x007f1530..35). 0x007f1042 and 0x007f1076 stay 0
# (they reach 0x00417450 / 0x00417530) and DAT_007f1a54 never 4 (reaches 0x0046d4a0 / 0x00408b00).
_PADS = [0x007f1042 + i for i in range(6)] + [0x007f1070 + i for i in range(7)] + [0x007f1503] + \
        [0x007f1530 + i for i in range(6)]
_ST = {0x007f1a50: 1, 0x007f1a54: 0, 0x007f1a58: 0x42c80000, 0x007f1a5c: 0x43480000, 0x007f1a64: 0,
       0x007f1a68: 0, 0x007f1a6c: 1, 0x007f1a70: 0x77, 0x007f1a74: 1, 0x007f1a90: 10, 0x007f1a98: 3}
_CNT = 0x00801aa0 + 0x204 + 0x200      # spline count dword for a68 = 0, a6c = 1


def _fid(st=None, pads=(), ctr=None, cnt=3):
    s = []
    d = dict(_ST); d.update(st or {})
    s += [{'addr': a, 'val': v} for a, v in d.items()]
    s += [{'addr': a, 'val': 1 if a in pads else 0, 'size': 1} for a in _PADS]
    c = ctr if ctr is not None else 0x11
    s += [{'addr': 0x006440e8 + 4 * k, 'val': (c + k) if c else 0} for k in range(10)]
    s.append({'addr': _CNT, 'val': cnt})
    return {'args': [], 'seed': s}


FIXES['frontend_input_dispatch'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x007f1a50, 'len': 0x4c}, {'addr': 0x006440e8, 'len': 0x28},
                {'addr': 0x00801aa0 + 0x204, 'len': 0x204}],
    'freeze_main': True, 'crash_equal_ok': None,
    'path1_tests': [
        _fid({0x007f1a50: 0}),                                          # gate closed
        _fid(),                                                         # idle: counters cleared, a70 = 0
        _fid(pads=(0x007f1044, 0x007f1045, 0x007f1046, 0x007f1047), ctr=0),   # first press: step floats, counters 10
        _fid(pads=(0x007f1044, 0x007f1046)),                            # held: counters count down
        _fid({0x007f1a54: 2, 0x007f1a64: 4}, pads=(0x007f1071,)),       # tab wrap 5 -> 2
        _fid({0x007f1a54: 5, 0x007f1a64: 2}, pads=(0x007f1070,)),       # tab wrap 1 -> 3
        _fid({0x007f1a64: 0, 0x007f1a90: 10}, pads=(0x007f1072, 0x007f1075)),  # cursor
        _fid({0x007f1a64: 1, 0x007f1a90: 0x7d}, pads=(0x007f1073, 0x007f1074)),
        _fid({0x007f1a54: 0, 0x007f1a64: 8}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 0, 0x007f1a64: 1}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 0, 0x007f1a64: 5}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 0, 0x007f1a64: 7}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 1, 0x007f1a64: 2}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 3, 0x007f1a64: 2}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 6, 0x007f1a64: 2}, pads=(0x007f1043,), cnt=3),   # spline point write, count 3 -> 4
        _fid({0x007f1a54: 6, 0x007f1a64: 3, 0x007f1a98: 1}, pads=(0x007f1043,), cnt=5),  # spline insert
        _fid({0x007f1a54: 6, 0x007f1a64: 5}, pads=(0x007f1043,)),
        _fid({0x007f1a54: 6, 0x007f1a64: 0, 0x007f1a98: 7}, pads=(0x007f1072, 0x007f1075), cnt=5),  # a98 wrap, a6c+1
        _fid({0x007f1a54: 6, 0x007f1a64: 0, 0x007f1a6c: 0}, pads=(0x007f1074,)),  # a6c -1 -> 2
        _fid({0x007f1a54: 0, 0x007f1a64: 2}, pads=(0x007f1043, 0x007f1503)),      # 1503 held: no edge
    ],
    'path2_tests': [_fid()],
}

# ── 0x00423b40/60/80/a0 PlayerScoreAccA..D(int i): if (DAT_0067e9fc == 4) return 0; return base[i*0x4e] ─────────
_PSTRIDE = 0x138
_VALS = (0x11, 0x220, 0x3300, 0x44000)


def _acc(base, i, sub):
    return {'args': [i], 'seed': [{'addr': 0x0067e9fc, 'val': sub}] +
            [{'addr': base + _PSTRIDE * j, 'val': _VALS[j]} for j in range(4)]}


def _acc_fix(base):
    t = [_acc(base, i, 0) for i in range(4)] + [_acc(base, i, 2) for i in range(4)] + \
        [_acc(base, 0, 4), _acc(base, 3, 4), _acc(base, 1, 3), _acc(base, 2, 5)]
    return {'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['int32']},
            'fold_ret': True, 'observe': [], 'freeze_main': True, 'path1_tests': t, 'path2_tests': t[:2]}


FIXES['player_score_acc_a'] = _acc_fix(0x00899a94)
FIXES['player_score_acc_b'] = _acc_fix(0x00899a98)
FIXES['player_score_acc_c'] = _acc_fix(0x00899f74)
FIXES['player_score_acc_d'] = _acc_fix(0x00899f78)


# ── 0x00423bc0 / 0x00423c40 team accumulators (no submode gate). FUN_0042f500 = DAT_0067ea64; team ids at
# 0x007f1a18 + 0x10*j; r = base[i]; for j != i with same team: r += base[j]. ──────────────────────────────────────
def _team(base, i, mode, teams):
    s = [{'addr': 0x0067ea64, 'val': mode}]
    for j in range(4):
        s.append({'addr': 0x007f1a18 + 0x10 * j, 'val': teams[j]})
        s.append({'addr': base + _PSTRIDE * j, 'val': _VALS[j]})
    return {'args': [i], 'seed': s}


def _team_fix(base):
    t = []
    for i in range(4):
        t += [_team(base, i, 0, (0, 0, 1, 1)), _team(base, i, 1, (0, 0, 1, 1)), _team(base, i, 1, (5, 5, 5, 5))]
    t += [_team(base, 1, 1, (0, 1, 0, 1)), _team(base, 2, 1, (7, 3, 7, 7))]
    return {'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['int32']},
            'fold_ret': True, 'observe': [], 'freeze_main': True, 'path1_tests': t, 'path2_tests': t[:3]}


FIXES['player_score_team_acc_c'] = _team_fix(0x00899a9c)
FIXES['player_score_team_acc_base'] = _team_fix(0x00899a40)

# ── 0x00425ef0 ActiveSlotCount(): counts k=0..7 with [0x008992a4 + k*0x4c] != 0 && [0x00899260 + k*0x4c] != 0 ─────
# Old vectors: menu state, every slot 0 -> 0 on both sides. Seed all 16 dwords per vector.
def _asc(pairs):
    s = []
    for k in range(8):
        a, b = pairs[k]
        s += [{'addr': 0x008992a4 + k * 0x4c, 'val': a}, {'addr': 0x00899260 + k * 0x4c, 'val': b}]
    return {'args': [], 'seed': s}


FIXES['active_slot_count'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': []},
    'fold_ret': True, 'observe': [], 'freeze_main': True,
    'path1_tests': [_asc([(0, 0)] * 8), _asc([(1, 1)] * 8), _asc([(1, 0), (0, 1), (1, 1), (0, 0)] * 2),
                    _asc([(7, 9)] + [(0, 5)] * 7), _asc([(0, 0)] * 7 + [(0x80000000, 1)]),
                    _asc([(1, 1), (1, 1), (1, 1), (0, 1), (1, 1), (1, 0), (1, 1), (1, 1)])],
    'path2_tests': [_asc([(1, 1)] * 8)],
}

# ── 0x00426670 WorldRenderDispatch_Begin(p): if (DAT_0066d704) 0x004e4320(DAT_0066d700 ? [0x00656ee8] : [0x0065742c], p)
# 0x004e4320(h, p): q = DAT_007d716c + p; if ([p+4]) 0x004c0e50([p+4]); [q+0xc] = h.
# Old vectors passed small ints as p with the gate closed. Pass a scratch record (+4 = 0, so 0x004c0e50 is not
# reached), seed DAT_007d716c = 0x40 so the store lands at scratch+0x4c, and seed both handles.
def _wrb(gate, sel, off=0x40):
    return {'args': [{'scratch': 0}], 'seed': [
        {'addr': 0x0066d704, 'val': gate}, {'addr': 0x0066d700, 'val': sel},
        {'addr': 0x00656ee8, 'val': 0xB0B0B0B1}, {'addr': 0x0065742c, 'val': 0xA0A0A0A1},
        {'addr': 0x007d716c, 'val': off}, {'addr': {'scratch': 4}, 'val': 0}]}


FIXES['world_render_dispatch_begin'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': {'scratch': 0}, 'len': 0x80}], 'scratch_size': 0x80, 'scratch_fill': 0xA5,
    'freeze_main': True,
    'path1_tests': [_wrb(0, 0), _wrb(1, 0), _wrb(1, 1), _wrb(1, 0, 0x20), _wrb(2, 0x80000000, 0x60)],
    'path2_tests': [_wrb(1, 1)],
}
