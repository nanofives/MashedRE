"""negctl re-earn, race lane (the 51 scenario='race' rows of re/analysis/negctl/race_uncovered_20261011.txt).

Fixtures only for the entries whose first race-lane negctl run (log/negctl-race/triage) failed the keep criterion.
Every fixture runs in a live race (run_negctl --race / run_diff scenario='race'), freezes the game thread while it
seeds, and restores every seeded and observed byte after each arm, so the race state the next job sees is unchanged.
Signatures declare uint32 for int / pointer args (same cdecl ABI): Frida's int32 marshalling rejects the >>>0 form
the handler produces for negative indices.
"""

FIXES = {}
_S = 'scalars_to_scattered_globals'


def _s2(sig_ret, nargs, tests, observe, **kw):
    d = {'arg_type': _S, 'signature': {'ret': sig_ret, 'args': ['uint32'] * nargs},
         'observe': observe, 'freeze_main': True, 'fold_ret': sig_ret != 'void',
         'target_global': None, 'seed_globals': None, 'call_args': None, 'crash_equal_ok': None,
         'seed_table': None, 'state_gate': None,
         'path1_tests': tests, 'path2_tests': tests[:2]}
    d.update(kw)
    return d


# ── 0x00423480 AiFilenameBuild() ─────────────────────────────────────────────────────────────────────────────────────
# Builds "AI%d.AI" (FUN_004a2b60 at 0x00423480, index = FUN_00426c00()) into a local, copies "" to DAT_00644110, then
# appends the local at the terminator found from 0x00644110; returns &DAT_00644110. Old void_write_observe did not
# fold the return (ret mutant BLIND). Observe the 0x20-byte buffer pre-filled 0xA5 and fold the return.
FIXES['ai_filename_build'] = _s2('uint32', 0, [{'args': []}] * 4,
                                 [{'addr': 0x00644110, 'len': 0x20, 'fill': 0xA5}])

# ── 0x004464c0 CameraEntryDispatchAll(param_1) ───────────────────────────────────────────────────────────────────────
# void (decomp 0x004464c0): for i < DAT_00898994, entry = 0x008964c0 + i*0xd8, dispatch on entry[1] (0 -> FUN_00445aa0,
# 1 -> FUN_00441d40, 2 -> FUN_00442440). Old int_scalar observed EAX only, which is not an output of a void body; the
# field mutants flipped 0x008964fc and 0x00896960 (inside the entry array) unseen. Observe the whole entry array plus
# the count (0x008964c0..0x00898997), game thread frozen. Arg kept from the old entry (0x00897fe0, the struct the
# callees read as param_2 and FUN_004430a0 writes; inside the window). First fixture run: original-vs-original
# differed on test 0 only, then every arm agreed, so the first call changes some state outside the window once
# ([UNCERTAIN] which: the three callees' direct stores all land in the window). prep_call_str runs the ORIGINAL
# dispatcher once before each arm (same restored start state on both arms), so the target runs on the settled state.
FIXES['camera_entry_dispatch_all'] = _s2('void', 1, [{'args': [0x00897fe0], 'prep_args': [0x00897fe0]}] * 4,
                                         [{'addr': 0x008964c0, 'len': 0x00898998 - 0x008964c0}],
                                         prep_call_str='0x004464c0', prep_arg_types=['uint32'])


# ── 0x0046c730 / 0x0046c750 per-vehicle dword getters ───────────────────────────────────────────────────────────────
# return (0xf < p) ? 0 : *(u32*)(base + p*0xd04), signed compare (base 0x00882198 / 0x00882194). Both tables read 0
# in the race, so every arm returned 0 (DEGENERATE, null BLIND). Seed distinct values at the slot each test reads,
# plus an out-of-range slot (16: the seed proves the bound) and p = -1 (signed: the body reads base - 0xd04).
def _veh_get(base):
    t = []
    for p, v in ((0, 0x11000001), (1, 0x22000002), (3, 0x33000003), (15, 0x44000004), (16, 0x55000005),
                 (0xFFFFFFFF, 0x66000006), (100, None)):
        a = (base + ((p - 0x100000000) if p >= 0x80000000 else p) * 0xd04) & 0xFFFFFFFF
        t.append({'args': [p], 'seed': [] if v is None else [{'addr': a, 'val': v}]})
    return t


FIXES['entity_damage_state_get'] = _s2('uint32', 1, _veh_get(0x00882198), [])
FIXES['entity_velocity_counter_get'] = _s2('uint32', 1, _veh_get(0x00882194), [])

# ── 0x004840d0 JointPtr6ce81cGet(u32 p) ──────────────────────────────────────────────────────────────────────────────
# return (3 < p) ? 0 : *(u32*)(0x006ce81c + p*4), unsigned compare. Table all zero in the race (DEGENERATE).
FIXES['joint_ptr_6ce81c_get'] = _s2('uint32', 1, [
    {'args': [p], 'seed': [{'addr': 0x006ce81c + (p & 7) * 4, 'val': 0xA1000000 + p}] if p < 8 else []}
    for p in (0, 1, 2, 3, 4, 0xFFFFFFFF)], [])

# ── 0x00496930 Table773030Get / TimerTable773030Get (one RVA, two port exports) ─────────────────────────────────────
# return *(u32*)(0x00773030 + p*4), no bound. Table all zero in the race (DEGENERATE). Seed the slot each test reads.
_t773 = [{'args': [p], 'seed': [{'addr': 0x00773030 + p * 4, 'val': 0xB0000000 + p * 0x1111}]} for p in (0, 1, 2, 5, 7, 12)]
FIXES['table_773030_get'] = _s2('uint32', 1, _t773, [])
FIXES['timer_table_773030_get'] = _s2('uint32', 1, _t773, [])

# ── 0x0041ea80 TrackDescField40Get() ─────────────────────────────────────────────────────────────────────────────────
# return *(u32*)(DAT_0063d7e4 + 0x40). The live record's +0x40 reads 0 in the race (DEGENERATE). Point DAT_0063d7e4
# at a harness record (game thread frozen, pointer restored after each arm) and seed +0x40.
FIXES['track_desc_field40_get'] = _s2('uint32', 0, [
    {'args': [], 'seed': [{'addr': 0x0063d7e4, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x40}, 'val': v}]}
    for v in (0x00000001, 0x12345678, 0x3f800000, 0xFFFFFFFF)], [], scratch_size=0x80, scratch_fill=0x5C)

# ── 0x0055c810 PhysicsBodySetLinearVelocity(body, vec) ───────────────────────────────────────────────────────────────
# FUN_0055c380(body, &DAT_005e4fe0) (writes body[0..0x17]), then body+0x40/44/48 = vec[0..2]; returns body.
# Old ptr_seed_observe did not fold the return (ret mutant BLIND). Body = harness record pre-filled 0xA5, observe
# 0x80 bytes, fold the return (the same scratch address on both arms).
FIXES['physics_body_set_linear_velocity'] = _s2('uint32', 2, [
    {'args': [{'scratch': 0}, {'scratch': 0xC0}],
     'seed': [{'addr': {'scratch': 0xC0}, 'val': a}, {'addr': {'scratch': 0xC4}, 'val': b},
              {'addr': {'scratch': 0xC8}, 'val': c}]}
    for a, b, c in ((0x3f800000, 0x40000000, 0x40400000), (0xdeadbe01, 0xdeadbe02, 0xdeadbe03),
                    (0, 0xFFFFFFFF, 0x80000000), (0x11111111, 0x22222222, 0x33333333))],
    [{'addr': {'scratch': 0}, 'len': 0x80}], scratch_size=0x100, scratch_fill=0xA5)


# ── 0x00415020 AiLastPlaceFrustration(i) ─────────────────────────────────────────────────────────────────────────────
# f = FUN_00442cc0(i) (i < 4: float at 0x008989b0 + i*4, else DAT_005d757c = 0.0). f != 0.0 -> rec[i].e4 = 0, ret 0.
# Else last j in 0..3 with FUN_0040e470(j) == 1 (*(*(0x005f2770) + 0x34 + j*4)); none -> rec[i].e4 = 0, ret 0.
# Else g = FUN_00442cc0(j); rec[i].e8 = g; if 2.5 (0x005cd088) < g: rec[i].e4 += DAT_007f1008, ret 1 if > 72000;
# if g >= 1.0 (0x005cc320) ret 0 (e4 kept); else rec[i].e4 = 0. rec = 0x0089a4e4 + i*0x74.
# Old void_write_observe did not fold the return (ret BLIND) and drove only the first branch. Every branch below,
# with *(0x005f2770) pointed at a harness record holding the four active flags.
def _frust(i, fi, act, fj, e4, dt):
    s = [{'addr': 0x005f2770, 'val': {'scratch': 0}},
         {'addr': 0x008989b0 + i * 4, 'val': fi} if i < 4 else None,
         {'addr': 0x0089a4e4 + i * 0x74, 'val': e4}, {'addr': 0x0089a4e8 + i * 0x74, 'val': 0xA5A5A5A5},
         {'addr': 0x007f1008, 'val': dt}]
    if act is not None:
        s.append({'addr': {'scratch': 0x34 + act * 4}, 'val': 1})
        s.append({'addr': 0x008989b0 + act * 4, 'val': fj})
    return {'args': [i], 'seed': [x for x in s if x]}


FIXES['sa2_ai_last_place_frustration'] = _s2('uint32', 1, [
    _frust(0, 0x3fc00000, None, 0, 0x1234, 16),             # f != 0 -> e4 = 0
    _frust(1, 0, None, 0, 0x1234, 16),                      # no active car -> e4 = 0
    _frust(1, 0, 2, 0x40400000, 71990, 100),                # g = 3.0 > 2.5, e4 -> 72090 > 72000 -> ret 1
    _frust(1, 0, 2, 0x40400000, 10, 100),                   # g = 3.0, e4 -> 110, ret 0
    _frust(3, 0, 0, 0x3fc00000, 0x1234, 100),               # g = 1.5 -> ret 0, e4 kept
    _frust(3, 0, 0, 0x3f000000, 0x1234, 100),               # g = 0.5 -> e4 = 0
    _frust(4, 0, 1, 0x40400000, 71999, 7),                  # i = 4: f = DAT_005d757c (0.0), e4 -> 72006, ret 1
], [{'addr': 0x0089a4e4, 'len': 0x74 * 5}], scratch_size=0x80, scratch_fill=0)


# ── 0x004922e0 CarEventTrigger(car, a, b, c) ─────────────────────────────────────────────────────────────────────────
# FUN_0040e470(car) == 1 and FUN_0040e350() (DAT_0063ba8c) == 6 and dword [rec+4] != 0 (0x0049230e), slot =
# dword @0x007f1a14 + car*0x10 (shl esi,4 @0x004922fd) -> rec = 0x007f1058 + slot*0x4c: +0x10 = a, +0 = 1,
# +0xc = b, +0x14 = c. (Ghidra's DAT_007f105c[slot*0x13] / DAT_007f1a14[car*4] are DWORD-indexed.) In the race
# DAT_0063ba8c is 3, so the old vectors never wrote (null BLIND). Drive every guard; written fields pre-seeded
# 0xA5A5A5A5 so a write is visible.
def _cev(car, active, mode, slot, flag, a, b, c):
    r = 0x007f1058 + slot * 0x4c
    s = [{'addr': 0x005f2770, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x34 + car * 4}, 'val': active},
         {'addr': 0x0063ba8c, 'val': mode}, {'addr': 0x007f1a14 + car * 16, 'val': slot},
         {'addr': r + 4, 'val': flag}]
    s += [{'addr': r + o, 'val': 0xA5A5A5A5} for o in (0x0, 0xc, 0x10, 0x14)]
    return {'args': [car, a, b, c], 'seed': s}


FIXES['sa2_car_event_trigger'] = _s2('void', 4, [
    _cev(0, 1, 6, 1, 1, 0x11111111, 0x22222222, 0x33333333),
    _cev(2, 1, 6, 3, 0x80, 3, 10, 128),
    _cev(3, 1, 6, 0, 0xFFFFFFFF, 0xDEADBEEF, 0, 0x7FFFFFFF),
    _cev(1, 0, 6, 2, 1, 4, 5, 6),                            # car inactive -> no write
    _cev(0, 1, 3, 1, 1, 4, 5, 6),                            # mode 3 -> no write
    _cev(0, 1, 6, 1, 0, 4, 5, 6),                            # slot flag 0 -> no write
], [{'addr': 0x007f1058, 'len': 0x4c * 4}], scratch_size=0x80, scratch_fill=0)


# ── 0x00487d00 Particle3rdEmitterSubmit(src, byte col) ───────────────────────────────────────────────────────────────
# if (DAT_00703058 < DAT_00703070) { FUN_00476a10(src) (64 bytes -> 0x006924e8); FUN_004769f0(&(col << 24))
# (-> DAT_00692554); FUN_00476d00(0x00703068) (pool: +8 cap, +0xc count, +0x10..+0x28 array pointers, entry
# count-th written from 0x006924xx/0x006925xx, count++); DAT_00703058++ }. The field mutants flipped 0x006924f4 and
# 0x00703074, which the old single-global observe never saw. Point the six pool arrays at harness memory, seed the
# source matrix, observe every write (game thread frozen; pool pointers restored after each arm).
_PSRC = [{'addr': {'scratch': k * 4}, 'val': 0x3f000000 + k * 0x10101} for k in range(16)]
_PARR = [(0x10, 0x100), (0x14, 0x200), (0x18, 0x300), (0x1c, 0x400), (0x20, 0x600), (0x24, 0x700), (0x28, 0x800)]


def _pem(col, n58, cap, cnt):
    s = list(_PSRC) + [{'addr': 0x00703058, 'val': n58}, {'addr': 0x00703070, 'val': cap},
                       {'addr': 0x00703074, 'val': cnt}]
    s += [{'addr': 0x00703068 + o, 'val': {'scratch': so}} for o, so in _PARR]
    return {'args': [{'scratch': 0}, col], 'seed': s}


FIXES['sa2_particle_3rd_emitter_submit'] = _s2('void', 2, [
    _pem(0xFF, 0, 100, 0), _pem(0x7F, 5, 100, 2), _pem(0x01, 1, 100, 3), _pem(0x80, 99, 100, 1),
    _pem(0x42, 100, 100, 0),                                 # counter at cap -> no-op
    _pem(0x42, 0, 4, 4),                                     # pool full: arrays untouched, counter still ++
], [{'addr': {'scratch': 0x100}, 'len': 0x800}, {'addr': 0x006924e8, 'len': 0x40}, {'addr': 0x00692554, 'len': 4},
    {'addr': 0x00703058, 'len': 0x38}], scratch_size=0x1000, scratch_fill=0)


# ── 0x004331a0 RaceFinalizeOnce(p) ───────────────────────────────────────────────────────────────────────────────────
# if (DAT_0067eca4 == 0) { DAT_0067eca4 = 1; FUN_0042d3a0() (zeroes 0x0067ed78..0x0067f0bb); 0x0067ebb0..bc =
# 0x43520000; DAT_0067ecac = p; ~12 globals and 0x0067ebd0[0..0x13] = 0; FUN_004248b0() (copies into
# 0x008999a0..0x00899dbf); FUN_00424920() (32 accumulators 0x00899e80..0x0089a327) }. field_last flipped 0x0089a228
# (an FUN_00424920 accumulator) unseen. Observe all three regions (0x0067e840 and 0x0067ed78 windows pre-filled 0xA5
# so the zeroing is visible), restore after each arm: the old harness left DAT_0067eca4 = 1 in the live race.
def _fin(p, eca4):
    return {'args': [p], 'seed': [{'addr': 0x0067eca4, 'val': eca4}]}


FIXES['sa2_race_finalize_once'] = _s2('void', 1, [
    _fin(0, 0), _fin(1, 0), _fin(0xDEADBEEF, 0), _fin(0x7FFFFFFF, 0), _fin(5, 1),   # last: already finalised -> no-op
], [{'addr': 0x0067e840, 'len': 0x0067ecb0 - 0x0067e840, 'fill': 0xA5},
    {'addr': 0x0067ed78, 'len': 0x0067f0bc - 0x0067ed78, 'fill': 0xA5},
    {'addr': 0x008999a0, 'len': 0x0089a328 - 0x008999a0}])

# ── 0x0047d150 SlotObjectField8(i) ───────────────────────────────────────────────────────────────────────────────────
# (-1 < i < 200) ? *(*(FUN_0057c210(DAT_006c71d8[i]) + 0x10) + 8) : 0. Old int_scalar run read real AND baseline RED
# 1/10: i = 199 access-violates in BOTH arms (original vs original too, run_diff --self-baseline idx 8), so it is
# dropped. Remaining vectors as before, game thread frozen.
FIXES['slot_object_field8'] = _s2('uint32', 1, [{'args': [i]} for i in (0, 1, 2, 3, 4, 5, 6, 7, 200, 0xFFFFFFFF)],
                                  [], state_gate=[{'any_nonzero': 0x006c71d8, 'words': 32}])

# ── 0x0046d740 VehicleVec3At6E4Set(vec, i) ───────────────────────────────────────────────────────────────────────────
# (0xf < i) ? 0 : (0x008816e4 + i*0xd04)[0..2] = vec[0..2], return 1. Real/baseline/field were fine; only the
# return was unobserved (ret BLIND). cache_setter_observe's fold_ret folds it.
FIXES['vehicle_vec3_at_6e4_set'] = {'fold_ret': True}


# ── 0x0046d510 VehicleVelocityWorldGet(out, i) ───────────────────────────────────────────────────────────────────────
# (0xf < i) ? 0 : FUN_004c3df0(0x00881f74 + i*0xd04, &DAT_00614708, 1, 0x00881ec8 + i*0xd04), out[0..2] = that
# result, return 1. Old cache_setter_observe read baseline RED 1/4 (the running game rewrote a seeded input between
# arms) and did not fold the return (ret BLIND). Same seeds, game thread frozen, out = harness buffer pre-filled
# 0xA5, the in-record result windows of slots 0..2 observed, return folded.
_MAT = [(0x00614708 + k * 4, v) for k, v in enumerate(
    (0x3f800000, 0, 0, 0, 0, 0x3f800000, 0, 0, 0, 0, 0x3f800000, 0, 0, 0, 0, 0x3f800000))]


def _vvw(i, v):
    s = [{'addr': a, 'val': x} for a, x in _MAT]
    if v:
        s += [{'addr': 0x00881ec8 + i * 0xd04 + k * 4, 'val': x} for k, x in enumerate(v)]
    return {'args': [{'scratch': 0}, i], 'seed': s}


FIXES['vehicle_velocity_world_get'] = _s2('uint32', 2, [
    _vvw(0, (0x3f800000, 0x40000000, 0x40400000)), _vvw(1, (0x40800000, 0x40a00000, 0x40c00000)),
    _vvw(2, (0xbfc00000, 0x3e800000, 0x41000000)), _vvw(16, None),
], [{'addr': {'scratch': 0}, 'len': 0x10}] + [{'addr': 0x00881f74 + i * 0xd04, 'len': 12} for i in range(3)],
    scratch_size=0x20, scratch_fill=0xA5)

# ── 0x00423b20 CarSnapshotDwordGet(i) ────────────────────────────────────────────────────────────────────────────────
# return (&DAT_008995ec)[i * 0x4e] (dword array, stride 0x138 bytes, no bound). Passed the triage negctl run on live
# race values, but the run_diff evidence boot read 0 for every slot (INCONCLUSIVE-DEGENERATE): the live value depends
# on race timing. Seed the slot each test reads.
FIXES['car_snapshot_dword_get'] = _s2('uint32', 1, [
    {'args': [i], 'seed': [{'addr': 0x008995ec + i * 0x138, 'val': v}]}
    for i, v in ((0, 0x00000001), (1, 0x0000ABCD), (2, 0x7FFFFFFF), (3, 0x80000000), (5, 0xC0DE0005))], [])
