"""negctl re-earn, batch b10 (log/negctl_reearn/batches/b10.tsv). Decomp/disasm cited per entry.

Most entries move to scalars_to_scattered_globals with per-test seeds, freeze_main and fold_ret. Entries whose body
reaches boot-only side effects (DirectInput8Create, the launch handshake) use stub_dispatch_observe recorders planted
on the direct-CALL targets, so nothing real is created and the recorded call arguments are the observable.
"""
import struct

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


_RET = 0x0042b8b6           # bare `ret` (cdecl, caller pops), used as fake RwGlobals render slots (b03.py)


# ── 0x0046d780 LaunchRevRelease(int p) ─────────────────────────────────────────────────────────────────────────────
# if (0xf < p) return 0; v = *(0x00882194 + p*0xd04); v > 1000: +0x198 = 2, FUN_00422b50(p, 1000 - v);
# 0 < v <= 1000: +0x198 = 1, +0x194 = v + 1000, FUN_00422b50(p, v + 1000); return 1.
# FUN_00422b50(p, d): if (*(*(0x005f2770) + p*4 + 0x34) != 1) runs the RtRandom path (FUN_00472650); then
# *(0x008995bc + p*0x138) += d. The old cache_setter_observe entry never compared the return (ret BLIND) and did
# not observe 0x008995bc. Point 0x005f2770 at a scratch record whose +0x34.. slots are 1 (deterministic path).
def _lrr(p, v, flag=7, acc=0x1000):
    s = [{'addr': 0x005f2770, 'val': {'scratch': 0}}]
    s += [{'addr': {'scratch': 0x34 + 4 * k}, 'val': 1} for k in range(16)]
    if 0 <= p <= 15:
        s += [{'addr': 0x00882194 + p * 0xd04, 'val': v & 0xFFFFFFFF},
              {'addr': 0x00882198 + p * 0xd04, 'val': flag},
              {'addr': 0x008995bc + p * 0x138, 'val': acc}]
    return {'args': [p], 'seed': s}


_LRR_OBS = []
for _p in (0, 1, 3):
    _LRR_OBS += [{'addr': 0x00882194 + _p * 0xd04, 'len': 8}, {'addr': 0x008995bc + _p * 0x138, 'len': 4}]

FIXES['launch_rev_release_46d780'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'int', 'args': ['int32']}, 'fold_ret': True,
    'observe': _LRR_OBS, 'pre_fill_byte': 0xA5, 'scratch_size': 0x80, 'freeze_main': True,
    'path1_tests': [_lrr(0, 3000), _lrr(0, 1001), _lrr(0, 1000), _lrr(1, 1, 0, 0x55), _lrr(0, 0, 9),
                    _lrr(0, -5, 7), _lrr(16, 0), _lrr(3, 500, 4, 0x77777777)],
    'path2_tests': [_lrr(0, 3000)],
}


# ── 0x0046d7f0 BoundedTableSignClamp(int p, int d) ─────────────────────────────────────────────────────────────────
# if (0xf < p) return 0; if (160.0 (_DAT_005cea3c = 0x43200000) < (float)byte[0x007f103c + idx*0x13],
# idx = *(int*)(0x007f1a14 + p*0x10)) +0x194 += 2*d; then +0x194 -= d, clamp to [0, 3000]; return 1.
# The old entry never compared the return (ret BLIND).
def _btc(p, d, idx, byte, v):
    s = []
    if 0 <= p <= 15:
        s = [{'addr': 0x007f1a14 + p * 0x10, 'val': idx}, {'addr': 0x007f103c + idx * 0x13, 'val': byte, 'size': 1},
             {'addr': 0x00882194 + p * 0xd04, 'val': v & 0xFFFFFFFF}]
    return {'args': [p, d], 'seed': s}


FIXES['bounded_table_signclamp_46d7f0'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'int', 'args': ['int32', 'int32']}, 'fold_ret': True,
    'observe': [{'addr': 0x00882194, 'len': 4}, {'addr': 0x00882194 + 0xd04, 'len': 4},
                {'addr': 0x00882194 + 5 * 0xd04, 'len': 4}],
    'pre_fill_byte': 0xA5, 'freeze_main': True,
    'path1_tests': [_btc(0, 100, 0, 200, 500), _btc(0, 100, 0, 50, 500), _btc(1, 50, 2, 200, 1000),
                    _btc(0, 5000, 0, 200, 0), _btc(0, 5000, 0, 50, 100), _btc(16, 1, 0, 0, 777),
                    _btc(0, 100, 0, 160, 500), _btc(0, 100, 0, 161, 500), _btc(5, -100, 3, 200, 400),
                    _btc(0, -4000, 1, 10, 7)],
    'path2_tests': [_btc(0, 100, 0, 200, 500)],
}


# ── 0x0046dc00 EntityFieldSet(int i, u32 v): (&DAT_008815a8)[i*0x341] = v; return (void) ────────────────────────────
# The entry declared ret uint32, so the ret mutant flipped a leftover EAX nobody reads (ret BLIND). Ret is void.
FIXES['entity_field_set'] = {'signature': {'ret': 'void', 'args': ['int32', 'uint32']}}


# ── 0x00472820 Vec3IsValidNonTiny(float *v) ────────────────────────────────────────────────────────────────────────
# _finite (FUN_004a3907) on v[0], v[1], v[2]; then 1 iff x*x+y*y+z*z > 1e-6 (_DAT_005ce54c = 0x358637bd). The old
# ptr_arg_int_get vectors filled the buffer with one repeated dword. Pass a scratch vec3, fold the return and
# observe the whole scratch (field mutants on the arg window land there).
def _v3(x, y, z):
    return {'args': [{'scratch': 0}], 'seed': [{'addr': {'scratch': 0}, 'val': x}, {'addr': {'scratch': 4}, 'val': y},
                                              {'addr': {'scratch': 8}, 'val': z}]}


_INF, _NAN, _NINF = 0x7f800000, 0x7fc00000, 0xff800000
FIXES['vec3_is_valid_non_tiny'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'int', 'args': ['pointer']}, 'fold_ret': True,
    'observe': [{'addr': {'scratch': 0}, 'len': 0x400}], 'scratch_size': 0x400, 'scratch_fill': 0x3C,
    'struct_size': None,
    'path1_tests': [_v3(_f(1.0), 0, 0), _v3(0, 0, 0), _v3(_NAN, 0, 0), _v3(0, _INF, 0), _v3(0, 0, _NINF),
                    _v3(_f(1e-4), 0, 0), _v3(_f(1e-3), _f(1e-3), 0), _v3(_f(-3.0), _f(4.0), _f(5.0)),
                    _v3(1, 1, 1), _v3(_f(0.0006), _f(0.0006), _f(0.0006)), _v3(_f(1e30), 0, _f(-1e30))],
    'path2_tests': [_v3(_f(1.0), 0, 0)],
}


# ── 0x00472f40 TextGradientV0V1Override / 0x00473540 GradientQuadHorizAlpha (x, y, w, h, argb) ────────────────────
# Both rebuild the Im2D vertex buffer 0x00898a20..0x00898a98 from the screen dims (FUN_0042b8b0/0042b8c0 read
# DAT_0067ea54/56), read *(RwGlobals+0x18) into each vertex, then call RwGlobals +0x20 (render state) x4 and +0x30
# (Im2D render). The live calls wrote engine state outside the observed buffer (field_first flipped 0x007d709c, BLIND).
# Fake RwGlobals (b03.py menu_chrome_shell_a pattern): +0x20/+0x30 = bare ret, +0x18 = fixed float.
def _dq(x, y, w, h, c, sw=640, sh=480):
    return {'args': [x, y, w, h, c], 'seed': [
        {'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x18}, 'val': _f(0.5)},
        {'addr': {'scratch': 0x20}, 'val': _RET}, {'addr': {'scratch': 0x30}, 'val': _RET},
        {'addr': 0x0067ea54, 'val': (sw & 0xFFFF) | ((sh & 0xFFFF) << 16)}]}


_DQ_TESTS = [_dq(0.0, 0.0, 640.0, 64.0, 0xA0000000), _dq(0.0, 0.0, 640.0, 64.0, 0xFF000000),
             _dq(100.0, 50.0, 200.0, 100.0, 0xC0FF80FF), _dq(10.0, 10.0, 50.0, 50.0, 0xFF123433),
             _dq(10.0, 10.0, 50.0, 50.0, 0xA1002200, 800, 600), _dq(-5.5, 3.25, 1.0, 1.0, 0x80FF00FF, 320, 240),
             _dq(0.0, 0.0, 1.0, 1.0, 0x0, 1024, 768)]
for _k in ('text_gradient_v0v1_override', 'gradient_quad_horiz_alpha'):
    FIXES[_k] = {
        'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['float', 'float', 'float', 'float', 'uint32']},
        'observe': [{'addr': 0x00898a20, 'len': 0x78}], 'pre_fill_byte': 0xA5, 'scratch_size': 0x40,
        'freeze_main': True, 'crash_equal_ok': None,
        'path1_tests': _DQ_TESTS, 'path2_tests': _DQ_TESTS[:1],
    }


# ── 0x004769a0 ParticleEmitter_SetPosition(u32 *p): p = p ? p : &DAT_006925a8; 0x00692528..30 = p[0..2] ─────────────
# ── 0x00476a40 ParticleEmitter_SetRGBA(u32 *p):     p = p ? p : &DAT_00613294; 0x00692598..a4 = p[0..3] ─────────────
# Old vectors were all NULL with target already equal to the default (DEGENERATE). Seed the default, pass a scratch
# tuple too, pre-fill the target.
def _pe(use, dflt_addr, dflt, vals):
    s = [{'addr': dflt_addr + 4 * k, 'val': v} for k, v in enumerate(dflt)]
    s += [{'addr': {'scratch': 4 * k}, 'val': v} for k, v in enumerate(vals)]
    return {'args': [{'scratch': 0} if use else 0], 'seed': s}


FIXES['particle_emitter_set_position'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': 0x00692528, 'len': 12, 'fill': 0xA5}], 'scratch_size': 0x400, 'freeze_main': True,
    'target_global': None,
    'path1_tests': [_pe(0, 0x006925a8, (_f(1.0), _f(2.0), _f(3.0)), ()),
                    _pe(1, 0x006925a8, (_f(1.0), _f(2.0), _f(3.0)), (_f(-7.5), _f(0.25), 0x7f7fffff)),
                    _pe(1, 0x006925a8, (0, 0, 0), (0x11111111, 0x22222222, 0x33333333)),
                    _pe(0, 0x006925a8, (0xdeadbeef, 0, 0x80000000), ())],
    'path2_tests': [_pe(1, 0x006925a8, (0, 0, 0), (0x11111111, 0x22222222, 0x33333333))],
}
FIXES['particle_emitter_set_rgba'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': 0x00692598, 'len': 16, 'fill': 0xA5}], 'scratch_size': 0x400, 'freeze_main': True,
    'target_global': None,
    'path1_tests': [_pe(0, 0x00613294, (0x10, 0x20, 0x30, 0x40), ()),
                    _pe(1, 0x00613294, (0x10, 0x20, 0x30, 0x40), (0xff, 0x80, 0x01, 0xfe)),
                    _pe(1, 0x00613294, (0, 0, 0, 0), (0x11111111, 0x22222222, 0x33333333, 0x44444444)),
                    _pe(0, 0x00613294, (0xdeadbeef, 0, 0x80000000, 7), ())],
    'path2_tests': [_pe(1, 0x00613294, (0, 0, 0, 0), (0x11111111, 0x22222222, 0x33333333, 0x44444444))],
}


# ── 0x00486610 ParticleBurst4(m, s) = FUN_004864f0(m, s, 4) ────────────────────────────────────────────────────────
# FUN_004864f0 copies 16 dwords of *m; if |m.pos(+0x30..+0x38) - last(0x006fe780..88)|^2 >= 0.05 (0x005cc9a0):
# last = pos, FUN_00486370(copy, s) with EAX=1 (0x00486564), then 4x: two RtRandom draws (FUN_00472650), RwMatrix
# translate (FUN_004c51a0 combine 2), FUN_00486370 with EAX=0 (0x004865f7). FUN_00486370 writes pool entry
# DAT_00703038 (stride 0x7c from 0x006fe790, counter wraps after 0x95) and draws RtRandom once.
# RtRandom (0x00534870) state lives at RwGlobals + DAT_007dc578: [+0] table start, [+4] cursor a, [+8] cursor b,
# [+0xc] table end. The old entry observed only 0x006fe780 (field_last flipped the pool counter, BLIND). Fake
# RwGlobals in scratch with a seeded RtRandom table makes every pool write deterministic; observe the touched pool
# entries, the counter, the last position and the whole scratch.
_PB_RNG = 0x100            # table at scratch+0x100 .. +0x180
_PB_MAT = 0x200            # input matrix at scratch+0x200


def _pb(cnt, pos, last, s=1.5):
    seed = [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': 0x007dc578, 'val': 0x40},
            {'addr': {'scratch': 0x40}, 'val': {'scratch': _PB_RNG}},
            {'addr': {'scratch': 0x44}, 'val': {'scratch': _PB_RNG + 0x5c}},
            {'addr': {'scratch': 0x48}, 'val': {'scratch': _PB_RNG}},
            {'addr': {'scratch': 0x4c}, 'val': {'scratch': _PB_RNG + 0x80}},
            {'addr': 0x00703038, 'val': cnt},
            {'addr': 0x006fe780, 'val': _f(last[0])}, {'addr': 0x006fe784, 'val': _f(last[1])},
            {'addr': 0x006fe788, 'val': _f(last[2])}]
    seed += [{'addr': {'scratch': _PB_RNG + 4 * k}, 'val': (0x9E3779B9 * (k + 1)) & 0xFFFFFFFF} for k in range(32)]
    mat = [1.0, 0.0, 0.0, 0.0, 0.0, 0.8, 0.6, 0.0, 0.0, -0.6, 0.8, 0.0, pos[0], pos[1], pos[2], 0.0]
    seed += [{'addr': {'scratch': _PB_MAT + 4 * k}, 'val': _f(v)} for k, v in enumerate(mat)]
    seed += [{'addr': {'scratch': _PB_MAT + 0x0c}, 'val': 0x00020003}]
    return {'args': [{'scratch': _PB_MAT}, _f(s)], 'seed': seed}


_PB_OBS = [{'addr': 0x006fe780, 'len': 12}, {'addr': 0x00703038, 'len': 4},
           {'addr': 0x006fe790, 'len': 4 * 0x7c}, {'addr': 0x006fe790 + 0x10 * 0x7c, 'len': 5 * 0x7c},
           {'addr': 0x006fe790 + 0x95 * 0x7c, 'len': 0x7c}, {'addr': {'scratch': 0}, 'len': 0x600}]
FIXES['particle_burst_4'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']},
    'observe': _PB_OBS, 'pre_fill_byte': 0xA5, 'scratch_size': 0x600, 'freeze_main': True,
    'call_args': None, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_pb(0x10, (10.0, 2.0, -3.0), (0.0, 0.0, 0.0)), _pb(0x10, (0.1, 0.1, 0.1), (0.0, 0.0, 0.0)),
                    _pb(0x10, (5.0, 5.0, 5.0), (5.0, 5.0, 5.1)), _pb(0x95, (-1.0, 0.0, 2.0), (3.0, 3.0, 3.0), 0.25),
                    _pb(0x96, (100.0, 0.0, 0.0), (0.0, 0.0, 0.0), 2.0), _pb(0x11, (0.3, 0.0, 0.0), (0.0, 0.0, 0.0))],
    'path2_tests': [_pb(0x10, (10.0, 2.0, -3.0), (0.0, 0.0, 0.0))],
}


# ── 0x00493480 FpsDiscretise(): FUN_00493390(); snap DAT_007f1000 to 50/100/150/200; accumulate into DAT_007719d4;
# DAT_007f1000 = n*50, DAT_007f1004 = (float)(n*50) * k; return 1 (decomp says undefined4, the entry said void).
# FUN_00493390 reads FUN_004950b0() = QPC*3000000 / QPF (0x00771e70/74) and writes 0x00771984, 0x007f0ff0..0x007f1030.
# The old entry observed 3 globals; the field mutants flipped the time-derived 0x00771984 / 0x007f1031 (BLIND).
# Seeding QPF = 0x8000000000000000 makes the signed 64-bit __alldiv quotient 0 for every QPC, so every write is a
# function of the seeds; observe all of them, fold the return.
def _fd(a, b, acc, cnt=100, d=0, e=0x1234, f=0x5678):
    s = [{'addr': 0x00771e70, 'val': 0}, {'addr': 0x00771e74, 'val': 0x80000000},
         {'addr': 0x00771984, 'val': a & 0xFFFFFFFF}, {'addr': 0x007f1030, 'val': b & 0xFFFFFFFF},
         {'addr': 0x007719d4, 'val': acc & 0xFFFFFFFF}, {'addr': 0x007f0ff0, 'val': cnt},
         {'addr': 0x007f102c, 'val': d & 0xFFFFFFFF}, {'addr': 0x007f1024, 'val': e}, {'addr': 0x007f1028, 'val': f}]
    return {'args': [], 'seed': s}


FIXES['fps_discretise'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x00771984, 'len': 4}, {'addr': 0x007719d4, 'len': 4}, {'addr': 0x007f0ff0, 'len': 0x44}],
    'pre_fill_byte': 0xA5, 'freeze_main': True,
    'observe_addrs': None, 'seed_sets': None,
    'path1_tests': [_fd(0, 0, 0), _fd(0, -300000, 10), _fd(50010, 0, 48), _fd(5000000, 0, 100),
                    _fd(-100, 0x10, 0x31, 7, 0xFFFFFFF0), _fd(49990, 0, 2), _fd(0, 0, 47), _fd(60000, 1, -60)],
    'path2_tests': [_fd(0, 0, 0)],
}


# ── 0x00493540 thunk -> FUN_00495150 (launch handshake) ────────────────────────────────────────────────────────────
# FUN_00495150 (disasm 0x00495150..0x004951c5): call 0x00495120; call 0x004955b0; call 0x00499710 -> push ->
# call 0x004960a0; call 0x00499710 -> push -> call 0x004963b0; switch DAT_007719e8 (1..5 -> 2,3,4,5,1, else 0);
# push that; push DAT_006147c0 (boot patch skip_controller_dialog at 0x004951aa replaces it with `xor eax,eax`, so
# both sides push 0); call 0x00498510; return DAT_007719e4 == 0. The old `none` entry ran the whole boot init
# (DirectInput create, log open, mode dialog) and compared a constant 1. Recorders replace the five init callees
# (all cdecl: add esp,8 at 0x00495175 / 0x004951bd); the HWND getter 0x00499710 stays real (mov eax,[0x7e9584]).
_LLG_STUBS = [{'addr': '0x00495120', 'nargs': 0, 'name': 'qpf', 'ret': 1},
              {'addr': '0x004955b0', 'nargs': 0, 'name': 'di', 'ret': 1},
              {'addr': '0x004960a0', 'nargs': 1, 'name': 'a0', 'ret': 1},
              {'addr': '0x004963b0', 'nargs': 1, 'name': 'b0', 'ret': 1},
              {'addr': '0x00498510', 'nargs': 2, 'name': 'sel', 'ret': 0}]
FIXES['launch_lang_gate'] = {
    'arg_type': 'stub_dispatch_observe', 'signature': {'ret': 'uint32', 'args': []},
    'num_bufs': 1, 'buf_size': 0x10, 'arg_layout': [], 'stub_at': _LLG_STUBS, 'stub_nargs': 0,
    'stub_abi': 'mscdecl', 'stub_ret': 0, 'observe_calls': True, 'observe_ret': True, 'crash_equal_ok': None,
    'path1_tests': [{'scalars': []}, {'scalars': []}, {'scalars': []}],
    'path2_tests': [{'scalars': []}],
}


# ── 0x00495120 TimerQpfStore(): QueryPerformanceFrequency -> 0x00771e70/74; return 1 ──────────────────────────────────
# Window pre-filled so a port that skips the store reads the sentinel, not the already-equal live value.
FIXES['timer_qpf_store'] = {
    'observe': [{'addr': 0x00771e70, 'len': 8, 'fill': 0xA5}], 'freeze_main': True,
}


# ── 0x00495530 CreateDInputObject() / 0x004955b0 DInputInitPredicate() ─────────────────────────────────────────────
# 0x00495530: push 0x5cfeb4; call 0x004987b0 (log); DirectInput8Create(GetModuleHandleA(0), 0x800, 0x5d0a8c,
# &DAT_00771e78, 0) through the thunk 0x0049b300 (jmp [0x5cc014], stdcall, 5 args); hr < 0: log 0x5cfeac, return 0;
# else return 1. 0x004955b0: call 0x00495530; neg/sbb/neg -> 0/1. The old `none`/`void` entries created a live
# IDirectInput8 at the menu and compared a constant 1. Recorders replace the logger and the DirectInput8Create thunk
# (the port routes both through the same game VAs, Input/DirectInput.cpp); the per-test HRESULT drives both branches.
_DI_STUBS = [{'addr': '0x004987b0', 'nargs': 1, 'name': 'log', 'ret': 0},
             {'addr': '0x0049b300', 'nargs': 5, 'abi': 'stdcall', 'name': 'di8'}]


def _di(hr):
    return {'scalars': [], 'stub_rets': {'di8': hr}}


_DI_TESTS = [_di(0), _di(-2147467259), _di(1), _di(-1), _di(0x7fffffff), _di(-2147483648)]
for _k in ('create_dinput_object', 'dinput_init_predicate'):    # create_dinput_object_bool removed 2026-10-16 (dupinstall)
    FIXES[_k] = {
        'arg_type': 'stub_dispatch_observe', 'signature': {'ret': 'uint32', 'args': []},
        'num_bufs': 1, 'buf_size': 0x10, 'arg_layout': [], 'stub_at': _DI_STUBS, 'stub_nargs': 0,
        'stub_abi': 'mscdecl', 'stub_ret': 0, 'observe_calls': True, 'observe_ret': True, 'crash_equal_ok': None,
        'path1_tests': _DI_TESTS, 'path2_tests': _DI_TESTS[:2],
    }


# Iteration 2 (0x004955b0 keys): stub_at on the jmp [IAT] thunk 0x0049b300 refused to install (Frida plants the
# replacement on the import target, the thunk bytes stay unchanged, the harness install guard aborts). For the
# 0x004955b0 predicate the recorder sits on its sole callee 0x00495530 instead (call at 0x004955b0); the per-test
# return drives neg/sbb/neg (0x004955b5..0x004955b9) through 0, 1 and non-0/1 values.
def _dp(v):
    return {'scalars': [], 'stub_rets': {'cdi': v}}


_DP_TESTS = [_dp(0), _dp(1), _dp(2), _dp(-1), _dp(-2147483648), _dp(0x100), _dp(0)]
for _k in ('dinput_init_predicate',):
    FIXES[_k] = dict(FIXES[_k], stub_at=[{'addr': '0x00495530', 'nargs': 0, 'name': 'cdi'}],
                     path1_tests=_DP_TESTS, path2_tests=_DP_TESTS[:2])
