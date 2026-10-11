"""negctl re-earn, group 2: the 16 at-risk rows that read RED or ERROR in the warm lane with no independent evidence.

Five of the 16 have no port to re-earn (no export in the dev .asi: 0x0041a3e0, 0x0041c0c0, 0x0041c300, 0x00426ba0 are
only called through their original address in HUD/HudDispatch.cpp; 0x004abc53's install is MASS-DISABLED). They get no
fixture here and are demoted. The other 11 are below.
"""

FIXES = {}

# ── RenderState_SetOpcode34..38 (0x004d7200 / 7250 / 72a0 / 72f0 / 7340) ─────────────────────────────────────────────
# Body (port RenderStateSettersA.cpp, byte-verified there): if (p == CACHE) return 1; VALUE = raw p (34) or
# TABLE[p] (35-37: 0x005d8d28, 38: 0x005d8d4c); if (DIRTY == 0) { DIRTY = 1; QUEUE[COUNT] = opcode; COUNT++; }
# CACHE = p; return 1. QUEUE = 0x007d5168, COUNT = 0x007d6c14. The old fixture never reset CACHE, so the second arm
# saw the first arm's p and took the early return (real AND baseline RED). Seed CACHE / DIRTY / COUNT per arm,
# pre-fill VALUE and the queue slot, fold the return, freeze the render thread (it owns COUNT and the queue).
_RS = {
    'RenderState_SetOpcode34': (0x007d6af0, 0x007d5998, 0x007d599c),
    'RenderState_SetOpcode35': (0x007d6af4, 0x007d59a0, 0x007d59a4),
    'RenderState_SetOpcode36': (0x007d6af8, 0x007d59a8, 0x007d59ac),
    'RenderState_SetOpcode37': (0x007d6afc, 0x007d59b0, 0x007d59b4),
    'RenderState_SetOpcode38': (0x007d6b00, 0x007d59b8, 0x007d59bc),
}
_COUNT, _K = 0x007d6c14, 3
_SLOT = 0x007d5168 + _K * 4
def _rs(cache_a, dirty_a, p, cache, dirty):
    return {'args': [p], 'seed': [{'addr': cache_a, 'val': cache}, {'addr': dirty_a, 'val': dirty},
                                  {'addr': _COUNT, 'val': _K}]}
for _k, (_c, _v, _d) in _RS.items():
    FIXES[_k] = {
        'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['uint32']},
        'fold_ret': True, 'pre_fill_byte': 0xA5, 'freeze_main': True, 'target_global': None,
        'observe': [{'addr': _c, 'len': 4}, {'addr': _v, 'len': 4}, {'addr': _d, 'len': 4},
                    {'addr': _COUNT, 'len': 4}, {'addr': _SLOT, 'len': 4}],
        'path1_tests': [_rs(_c, _d, 1, 0, 0), _rs(_c, _d, 2, 0, 1), _rs(_c, _d, 3, 3, 0), _rs(_c, _d, 0, 5, 0),
                        _rs(_c, _d, 2, 1, 0)],
        'path2_tests': [_rs(_c, _d, 1, 0, 0)],
    }

# ── 0x00401ee0 EntityTableSelectUpdate(p) ──────────────────────────────────────────────────────────────────────────────
# FUN_00401570(p): DAT_00636ac0 = the 0x68-byte entry in 0x00636578..0x00636ac0 whose +0x38 == p (else the first).
# FUN_00401da0(): when *DAT_00636ac0 != 0, DAT_00636ac4 += _DAT_005cc320 and the entry's frame is rebuilt from it.
# DAT_00636ac4 accumulates across calls, so the second arm started from the first arm's value (real AND baseline RED).
# Seed DAT_00636ac4 per arm; pre-fill DAT_00636ac0 so the selection write is visible; freeze the main thread.
def _ets(p, ang):
    return {'args': [p], 'seed': [{'addr': 0x00636ac4, 'val': ang}]}
FIXES['entity_table_select_update'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['uint32']},
    'observe': [{'addr': 0x00636ac0, 'len': 4, 'fill': 0xA5}, {'addr': 0x00636ac4, 'len': 4}],
    'freeze_main': True, 'target_global': None,
    'path1_tests': [_ets(0, 0), _ets(1, 0x3f800000), _ets(2, 0), _ets(3, 0x40490fdb), _ets(0xDEAD, 0)],
    'path2_tests': [_ets(0, 0)],
}

# ── 0x004d5570 D3d9State_SetSampler(stage, type, value) ───────────────────────────────────────────────────────────────
# i = type + stage*0xe; if (DAT_007d4f60[i] != value) { DAT_007d4f60[i] = value;
#   (*(*DAT_007d4110 + 0x114))(DAT_007d4110, stage, type, value); }
# The real device call timed out from the harness thread. Give the arm a fake device (B0 -> vtable B1, slot 0x114 =
# 0x004a4217, a bare `ret 0x10` epilogue in .text) with the main thread frozen; the cache dword is seeded with a
# sentinel (observe_globals), so a port that skips the store reads RED.
_RET10 = 0x004a4217
def _smp(stage, typ, val, same=False):
    idx = typ + stage * 0xe
    t = {'blocks': [0x10, 0x120], 'args': [stage, typ, val],
         'fields': [['B0', 0, 'B1'], ['B1', 0x114, _RET10]],
         'globals': [[0x007d4110, 'B0']]}
    if same:
        t['globals'].append([0x007d4f60 + idx * 4, val])
    return t, 0x007d4f60 + idx * 4
_T = [_smp(0, 0, 0x11223344), _smp(1, 2, 0x55667788), _smp(2, 5, 0xCAFEBABE), _smp(0, 0, 0x99, same=True),
      _smp(3, 13, 7)]
FIXES['d3d9_set_sampler'] = {
    'arg_type': 'standin_object_call', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']},
    'freeze_main': True, 'object_size': 0x10, 'object_alloc': 'frida',
    'observe_globals': [{'addr': 0x007d4110, 'len': 4, 'seed': False}]
                       + [{'addr': a, 'len': 4} for _, a in _T],
    'path1_tests': [t for t, _ in _T], 'path2_tests': [_T[0][0]],
}

# ── draw rows: ChromeBaseDraw 0x00472c60, BorderQuadFourAlpha 0x004736c0, TextSpriteUVExplicit 0x00473870 ────────────
# draw_quad_observe fingerprints VBUF 0x00898a20 after the call; the running game rewrote it between call and read
# (real AND baseline RED). Same vectors, main thread frozen for the run.
for _k in ('chrome_base_draw', 'border_quad_four_alpha', 'text_sprite_uv_explicit'):
    FIXES[_k] = {'freeze_main': True}

# ── 0x0042bde0 HudRectEmitter5(int row, byte alpha) ───────────────────────────────────────────────────────────────────
# Five ChromeBaseDraw calls (port Frontend/BatchAA_s1.cpp). The entry declared zero args ('none'), so row/alpha were
# leftover stack. draw_quad_observe with the real signature: VBUF holds the last quad (x = _DAT_005cd784 + 534.0,
# y from row, colour from alpha).
FIXES['hud_rect_emitter5'] = {
    'arg_type': 'draw_quad_observe', 'signature': {'ret': 'void', 'args': ['int32', 'uint32']},
    'freeze_main': True,
    'path1_tests': [[100, 255], [13, 128], [0, 0], [240, 64], [-20, 200]],
    'path2_tests': [[100, 255]],
}
