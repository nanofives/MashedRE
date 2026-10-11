"""negctl re-earn, device-call rows: 0x004d5570 D3d9State_SetSampler (g2) and 0x004d6b90 D3d9State_SetStage0Filter (b12).

Both call the device's SetSamplerState through (*DAT_007d4110)->vtbl[0x114](dev, stage, type, value) (stdcall, 4 args).
The first fixtures put a bare `ret 0x10` (0x004a4217) in that slot, so the arguments were never compared: a port
passing the wrong stage/type/value still read GREEN. Here the slot is the harness `recorder` (scalars_to_scattered_globals),
which appends every call's 4 args to scratch+0x400; that window is observed. Loads after b12.py / g2 (sorted), so it
overrides both entries. Main thread frozen: the render thread owns DAT_007d4110.
"""

def S(off):
    return {'scratch': off}

_REC = {'args': 4, 'abi': 'stdcall', 'off': 0x400, 'max_calls': 8}
_REC_OBS = {'addr': S(0x400), 'len': 4 + 4 * 4 * 8}
_DEV = [{'addr': 0x007d4110, 'val': S(0)}, {'addr': S(0), 'val': S(0x10)}, {'addr': S(0x10 + 0x114), 'val': {'recorder': True}}]

# ── 0x004d6b90: same vectors as b12 (cache globals seeded), slot -> recorder ──────────────────────────────────────────
def _sf(arg, b3c, b44, f7c=0xAAAAAAAA):
    return {'args': [arg], 'seed': _DEV + [
        {'addr': 0x007d6b3c, 'val': b3c}, {'addr': 0x007d6b44, 'val': b44}, {'addr': 0x007d4f7c, 'val': f7c},
        {'addr': 0x007d4f78, 'val': 0xBBBBBBBB}, {'addr': 0x007d4f74, 'val': 0xCCCCCCCC},
        {'addr': 0x007d4f88, 'val': 0xDDDDDDDD}]}
_T1 = [_sf(0, 0xFFFFFFFF, 0), _sf(3, 0xFFFFFFFF, 0), _sf(2, 2, 5), _sf(2, 2, 0), _sf(1, 0xFFFFFFFF, 2),
       _sf(0, 0xFFFFFFFF, 0, f7c=0)]
FIXES = {}
FIXES['d3d9_set_stage0_filter'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'fold_ret': True,
    'scratch_size': 0x600, 'freeze_main': True, 'recorder': _REC,
    'observe': [{'addr': a, 'len': 4} for a in (0x007d6b3c, 0x007d4f78, 0x007d4f74, 0x007d4f7c, 0x007d6b44, 0x007d4f88)]
               + [_REC_OBS],
    'path1_tests': _T1, 'path2_tests': _T1[:1],
}

# ── 0x004d5570: i = type + stage*0xe; if (DAT_007d4f60[i] != value) { store; device call } ─────────────────────────────
def _smp(stage, typ, val, cache):
    a = 0x007d4f60 + (typ + stage * 0xe) * 4
    return {'args': [stage, typ, val], 'seed': _DEV + [{'addr': a, 'val': cache}]}, a
_T2 = [_smp(0, 0, 0x11223344, 0x5EED0000), _smp(1, 2, 0x55667788, 0x5EED0001), _smp(2, 5, 0xCAFEBABE, 0x5EED0002),
       _smp(0, 0, 0x99, 0x99), _smp(3, 13, 7, 0x5EED0004)]
FIXES['d3d9_set_sampler'] = {
    '__replace__': True,
    'rva': 0x004d5570, 'export': 'D3d9State_SetSampler', 'lut_root_delta': 0,
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']},
    'scratch_size': 0x600, 'freeze_main': True, 'recorder': _REC,
    'observe': [{'addr': a, 'len': 4} for _, a in _T2] + [_REC_OBS],
    'path1_tests': [t for t, _ in _T2], 'path2_tests': [_T2[0][0]],
}
