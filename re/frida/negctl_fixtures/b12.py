"""negctl re-earn, batch b12 (intro-splash slots, RW engine/device wrappers, D3D9 stage-0 cache, RW plugin lists,
pixel leaves).

Decomp from re/tools/decomp_pc.py (pool slot, read-only), 2026-10-11. Call recorders are original one-line leaves
(bytes read from MASHED.exe.unpatched):
  0x004099e0  DAT_008a95a0 = a1; EAX = a1
  0x00491010  DAT_006146b8 = a1; DAT_006146bc = a2; EAX = a1
  0x004292c0  *a1 = a2
  0x004b4070  *a2 = a1; return 0
  0x0048fed0  *(a2 + 8) = a1          0x0048fee0  *(a2 + 0xc) = a1
  0x005c8060  *(a1 + 0x34) = a3; EAX = a1   (8b 44 24 04 8b 4c 24 0c 89 48 34 c3, not a Ghidra function)
  0x00562a10  return a3                      (8b 44 24 0c c3, DATA xrefs 0x005e52bc / 0x005e54d4)
  0x005742b0  *a4 = a1; return 0             (8b 44 24 10 8b 4c 24 04 89 08 33 c0 c3, DATA xref 0x005e5928)
  0x004dfa70  return *a1                     0x004dfa40  byte reorder of *a1
  0x004a4217  ret 0x10 (bare stdcall-4 epilogue, c2 10 00)
Rows left unchanged here (their stored fixture already meets the keep criterion; re-run only): 0x004c2d90
rw_engine_register_plugin, 0x004d8550 rw_pipe_module_dtor.
"""
FIXES = {}

_SCAL = 'scalars_to_scattered_globals'
_RWG = 0x007d3ff8
_DROP = {'target_global': None, 'crash_equal_ok': None, 'obs_globals': None, 'struct_size': None,
         'arg_layout': None, 'num_bufs': None, 'buf_size': None, 'observe_bufs': None, 'observe_calls': None,
         'observe_ret': None, 'stub_at': None, 'stub_abi': None, 'stub_nargs': None, 'stub_ret': None}


def _e(sig_ret, sig_args, tests, **kw):
    d = dict(_DROP)
    d.update({'arg_type': _SCAL, 'signature': {'ret': sig_ret, 'args': sig_args}, 'fold_ret': sig_ret != 'void',
              'path1_tests': tests, 'path2_tests': tests[:1]})
    d.update(kw)
    return d


def S(off):
    return {'scratch': off}


# ── 0x004c1a00 IntroSplashVtableSlot6(obj) ───────────────────────────────────────────────────────────────────────────
# mov eax,[esp+4]; mov [esp+4],eax; jmp [eax+0x18]: a tail jump, the target inherits the frame (arg = obj). The old
# vectors passed small integers as obj, so both sides faulted (crash_equal_ok). obj = scratch, +0x18 = an original
# leaf: 0x004dfa70 returns *obj, 0x004dfa40 returns *obj byte-reordered, 0x004099e0 stores obj to DAT_008a95a0.
def _s6(tgt, val):
    return {'args': [S(0)], 'seed': [{'addr': S(0x18), 'val': tgt}, {'addr': S(0), 'val': val}]}


FIXES['intro_splash_vtable_slot6'] = _e('uint32', ['pointer'], [
    _s6(0x004dfa70, 0x11223344), _s6(0x004dfa70, 0), _s6(0x004dfa70, 0xDEADBEEF), _s6(0x004dfa40, 0xA1B2C3D4),
    _s6(0x004099e0, 0x55)],
    scratch_size=0x40, freeze_main=True, observe=[{'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5}])


# ── 0x004c1bb0 IntroSplashRenderState(a, b, c) ───────────────────────────────────────────────────────────────────────
# iVar1 = (*(RwGlobals + 0x9c))(a, b, c); return -(iVar1 != 0) & a. The old vectors were (0,0,0) x10 through the live
# slot. Fake RwGlobals = scratch (DAT_007d3ff8 seeded, main thread frozen), slot +0x9c = a recorder leaf:
# 0x00491010 records a and b; 0x005c8060 writes c at a+0x34; 0x00562a10 returns c (gates on c); 0x004b4070 writes a
# at *b and returns 0 (the zero arm).
def _rs(slot, a, b, c):
    return {'args': [a, b, c], 'seed': [{'addr': _RWG, 'val': S(0)}, {'addr': S(0x9c), 'val': slot}]}


FIXES['intro_splash_render_state'] = _e('uint32', ['uint32', 'pointer', 'uint32'], [
    _rs(0x00491010, 0x12345678, 0x9ABC, 7), _rs(0x00491010, 0, 0x1111, 0), _rs(0x005c8060, S(0x100), 0, 0xCAFEF00D),
    _rs(0x005c8060, S(0x140), 0x2222, 0x77), _rs(0x00562a10, 0x4444, 0, 0), _rs(0x00562a10, 0x4444, 0, 9),
    _rs(0x004b4070, 0x0BADF00D, S(0x180), 1)],
    scratch_size=0x200, scratch_fill=0xA5, freeze_main=True,
    observe=[{'addr': 0x006146b8, 'len': 8, 'fill': 0xA5}, {'addr': S(0x100), 'len': 0x100}])


# ── 0x004c2e10 RwEngineGetSubSystemInfo(out, idx) ────────────────────────────────────────────────────────────────────
# FUN_004c2c90(RwGlobals+0x10, 0xe, out, 0, idx) ? out : 0. The live device fn (0x004c7a70 case 0xe) fills *out. The
# old vectors passed out = 0x008a9550 (never observed) and idx 1 x10, so the field mutants flipped *out unseen.
# out = scratch (filled 0xA5), observed 0x100 bytes; the return (= out) is folded.
def _si(off, idx):
    return {'args': [S(off), idx], 'seed': []}


FIXES['rw_engine_get_sub_system_info'] = _e('uint32', ['pointer', 'int32'], [
    _si(0, 0), _si(0x10, 0), _si(0x40, 0), _si(0, 0)],
    scratch_size=0x200, scratch_fill=0xA5, observe=[{'addr': S(0), 'len': 0x200}])


# ── 0x004c2e40 RwEngineGetCurrentSubSystem() ─────────────────────────────────────────────────────────────────────────
# FUN_004c2c90(RwGlobals+0x10, 0xf, &local, 0, 0) ? local : -1. Live device fn 0x004c7a70 case 0xf: *out =
# DAT_007d410c; return 1. At the menu DAT_007d410c = 0 and a null port returns 0. Seed DAT_007d410c, main thread frozen.
def _cs(v):
    return {'args': [], 'seed': [{'addr': 0x007d410c, 'val': v}]}


FIXES['rw_engine_get_current_sub_system'] = _e('uint32', [], [_cs(1), _cs(0), _cs(2), _cs(0x7FFFFFFF), _cs(0xDEADBEEF)],
                                               freeze_main=True, observe=[])


# ── 0x004c2f30 RwEngineSetVideoMode(mode) ────────────────────────────────────────────────────────────────────────────
# FUN_004c2c90(RwGlobals+0x10, 7, 0, 0, mode) != 0. Live device fn 0x004c7a70 case 7: if DAT_007d4120 == 0 &&
# 0 <= mode < DAT_007d4128: DAT_007d4100 = mode; DAT_00911f84..f98 from table entry DAT_007d4130 + mode*0x14 (+f94 by
# format); DAT_006181b4..c0 from an IDirect3D9 CheckDeviceMultiSampleType loop; return 1. Else 0 (the menu: started).
# Seed DAT_007d4120, pre-fill and observe every written global (restored after each arm), main thread frozen.
def _vm(started, mode):
    return {'args': [mode], 'seed': [{'addr': 0x007d4120, 'val': started}]}


FIXES['rw_engine_set_video_mode'] = _e('int', ['int32'], [
    _vm(0, 0), _vm(0, 1), _vm(0, 3), _vm(1, 1), _vm(0, -1), _vm(0, 0x7FFFFFFF)],
    freeze_main=True,
    observe=[{'addr': 0x007d4100, 'len': 4, 'fill': 0xA5}, {'addr': 0x00911f84, 'len': 0x18, 'fill': 0xA5},
             {'addr': 0x006181b4, 'len': 0x10, 'fill': 0xA5}])


# ── 0x004cd140 RwRenderCommandBufferReset() ──────────────────────────────────────────────────────────────────────────
# p = RwGlobals + DAT_00911d00; if (*(p+0x3c) == 0) return 0; zero 15 dwords at p+0x38; return 1. The menu read the
# zero arm only. Fake RwGlobals = scratch (filled 0xA5), DAT_00911d00 seeded, main thread frozen, scratch observed.
def _cb(off, h=None):
    s = [{'addr': _RWG, 'val': S(0)}, {'addr': 0x00911d00, 'val': off}]
    if h is not None:
        s.append({'addr': S(off + 0x3c), 'val': h})
    return {'args': [], 'seed': s}


FIXES['rw_render_command_buffer_reset'] = _e('int', [], [_cb(0x10, 0x1234), _cb(0x10, 0), _cb(0x80), _cb(0x100, 1),
                                                         _cb(0x40, 0)],
                                             scratch_size=0x200, scratch_fill=0xA5, freeze_main=True,
                                             observe=[{'addr': S(0), 'len': 0x200}])


# ── 0x004d6b90 D3d9State_SetStage0Filter(f) ──────────────────────────────────────────────────────────────────────────
# if (1 < DAT_007d6b44) { DAT_007d6b44 = 1; DAT_007d4f88 = 1; dev->SetSamplerState(0,10,1); }
# if (f != DAT_007d6b3c) { v = *(0x005d8ca8 + f*8); a = *(0x005d8cac + f*8); DAT_007d6b3c = f; DAT_007d4f78 = v;
#   dev(0,6,v); DAT_007d4f74 = v; dev(0,5,v); if (DAT_007d4f7c != a) { DAT_007d4f7c = a; dev(0,7,a); } } return 1.
# cache_setter_observe never folded the constant return (ret BLIND). Same vectors through the scattered handler with
# fold_ret, on a fake device (DAT_007d4110 -> scratch, vtable +0x114 = 0x004a4217 `ret 0x10`), main thread frozen.
def _sf(arg, b3c, b44, f7c=0xAAAAAAAA):
    return {'args': [arg], 'seed': [
        {'addr': 0x007d4110, 'val': S(0)}, {'addr': S(0), 'val': S(0x10)}, {'addr': S(0x10 + 0x114), 'val': 0x004a4217},
        {'addr': 0x007d6b3c, 'val': b3c}, {'addr': 0x007d6b44, 'val': b44}, {'addr': 0x007d4f7c, 'val': f7c},
        {'addr': 0x007d4f78, 'val': 0xBBBBBBBB}, {'addr': 0x007d4f74, 'val': 0xCCCCCCCC},
        {'addr': 0x007d4f88, 'val': 0xDDDDDDDD}]}


FIXES['d3d9_set_stage0_filter'] = _e('uint32', ['int32'], [
    _sf(0, 0xFFFFFFFF, 0), _sf(3, 0xFFFFFFFF, 0), _sf(2, 2, 5), _sf(2, 2, 0), _sf(1, 0xFFFFFFFF, 2),
    _sf(0, 0xFFFFFFFF, 0, f7c=0)],
    scratch_size=0x200, freeze_main=True,
    observe=[{'addr': a, 'len': 4} for a in (0x007d6b3c, 0x007d4f78, 0x007d4f74, 0x007d4f7c, 0x007d6b44, 0x007d4f88)])


# ── 0x004d8060 RwPluginListDestructDispatch(obj, a) ──────────────────────────────────────────────────────────────────
# for (n = *(obj+0x14); n; n = n[0xd]) n[9](a, n[0], n[1]); return obj. stub_dispatch_observe left the return
# (= obj, a per-side buffer) unobserved. Scattered handler: obj = scratch (one address, both arms), nodes in the
# scratch, callees = recorder leaves; the return is folded.
_N1, _N2, _N3 = 0x100, 0x180, 0x200


def _node(at, fn_off, fn, n0, n1, nxt, nxt_off):
    return [{'addr': S(at + fn_off), 'val': fn}, {'addr': S(at), 'val': n0}, {'addr': S(at + 4), 'val': n1},
            {'addr': S(at + nxt_off), 'val': S(nxt) if nxt else 0}]


def _d3(a, nodes, head=0x14, obj=0):
    s = [{'addr': S(obj + head), 'val': S(nodes[0][0]) if nodes else 0}]
    for i, (at, fn, n0, n1) in enumerate(nodes):
        nxt = nodes[i + 1][0] if i + 1 < len(nodes) else 0
        s += _node(at, 0x24, fn, n0, n1, nxt, 0x34)
    return {'args': [S(obj), a], 'seed': s}


FIXES['rw_plugin_list_dispatch3'] = _e('uint32', ['pointer', 'uint32'], [
    _d3(0x1234, [(_N1, 0x00491010, 0x5555, 0x6666)]),
    _d3(0x2345, [(_N1, 0x0048fed0, S(0x300), 0x11), (_N2, 0x0048fee0, S(0x320), 0x22),
                 (_N3, 0x00491010, 0x7777, 0x8888)]),
    _d3(S(0x340), [(_N1, 0x005c8060, 0x9999, 0xABCD0001), (_N2, 0x00491010, 0xAAAA, 0xBBBB)]),
    _d3(0x77, [(_N1, 0x00491010, 1, 2), (_N2, 0x00491010, 3, 4)]),
    _d3(0x99, []),
    _d3(0x88, [], obj=0x40)],
    scratch_size=0x400, scratch_fill=0xA5, freeze_main=True,
    observe=[{'addr': 0x006146b8, 'len': 8, 'fill': 0xA5}, {'addr': S(0x300), 'len': 0x80}])


# ── 0x004d8090 RwPluginListCopyDispatch(obj, a, b) ───────────────────────────────────────────────────────────────────
# for (n = *(obj+0x10); n; n = n[0xc]) n[10](a, b, n[0], n[1]); return obj. Same scheme; node link at +0x30, callee
# at +0x28. Recorders: 0x00491010 (a, b), 0x004292c0 (*a = b), 0x005c8060 (*(a+0x34) = n[0]), 0x005742b0 (*n[1] = a).
def _d2(a, b, nodes, obj=0):
    s = [{'addr': S(obj + 0x10), 'val': S(nodes[0][0]) if nodes else 0}]
    for i, (at, fn, n0, n1) in enumerate(nodes):
        nxt = nodes[i + 1][0] if i + 1 < len(nodes) else 0
        s += _node(at, 0x28, fn, n0, n1, nxt, 0x30)
    return {'args': [S(obj), a, b], 'seed': s}


FIXES['rw_plugin_list_dispatch'] = _e('uint32', ['pointer', 'uint32', 'uint32'], [
    _d2(0x1234, 0x5678, [(_N1, 0x00491010, 0x10, 0x20)]),
    _d2(S(0x300), 0xFEED0001, [(_N1, 0x004292c0, 0, 0), (_N2, 0x00491010, 0, 0)]),
    _d2(S(0x340), 0x31, [(_N1, 0x005c8060, 0xC0DE0002, 0), (_N2, 0x005742b0, 0, S(0x320)),
                         (_N3, 0x00491010, 0, 0)]),
    _d2(0x41, 0x42, [(_N1, 0x00491010, 0, 0), (_N2, 0x00491010, 0, 0)]),
    _d2(0x51, 0x52, []),
    _d2(0x61, 0x62, [], obj=0x40)],
    scratch_size=0x400, scratch_fill=0xA5, freeze_main=True,
    observe=[{'addr': 0x006146b8, 'len': 8, 'fill': 0xA5}, {'addr': S(0x300), 'len': 0x80}])


# ── 0x004d8470 RwErrorModuleDtor(x) ──────────────────────────────────────────────────────────────────────────────────
# DAT_007d6c60 -= 1; return x. The _count entry observed the global but not the return (ret BLIND). Seed + observe
# the global and fold the return.
def _ed(x, c):
    return {'args': [x], 'seed': [{'addr': 0x007d6c60, 'val': c}]}


FIXES['rw_error_module_dtor_count'] = _e('uint32', ['uint32'], [
    _ed(0x11111111, 1), _ed(0x22222222, 2), _ed(0x33333333, 0), _ed(0x44444444, 0x7FFFFFFF),
    _ed(0x55555555, 0x80000000), _ed(0, 0xDEADBEEF)],
    freeze_main=True, observe=[{'addr': 0x007d6c60, 'len': 4}])


# ── 0x004dfa10 / 0x004dfa40 / 0x004dfa70 pixel leaves (fn(byte *p) -> uint32) ───────────────────────────────────────
# 0x004dfa10: ((p[2] | 0xffffff00) << 8 | p[1]) << 8 | p[0]; 0x004dfa40: CONCAT(p[3], p[0], p[1], p[2]);
# 0x004dfa70: *(uint32*)p. Pure reads, no stores. ptr_arg_int_get's diff was GREEN with null/ret RED; its field
# mutants hit one unobserved byte outside the function (no store exists to flip). Scattered handler with p = scratch
# (observed, so any byte the port changed there is in the fingerprint), main thread frozen.
_PIX = [0x11223344, 0xAABBCCDD, 0, 0xFFFFFFFF, 0xDEADBEEF, 0x01020304, 0xFF000000, 0x00FF00FF, 0x80, 0x7F7F7F7F]


def _px(v):
    return {'args': [S(0)], 'seed': [{'addr': S(0), 'val': v}]}


for _k in ('rgb_pack_encoder_4dfa10', 'bgra_reorder_4dfa40', 'pix_read_u32'):
    FIXES[_k] = _e('uint32', ['pointer'], [_px(v) for v in _PIX], scratch_size=0x10, freeze_main=True,
                   observe=[{'addr': S(0), 'len': 8}])
