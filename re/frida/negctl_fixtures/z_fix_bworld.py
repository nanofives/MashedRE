"""B-WORLD (2026-10-12): B rows that need game state the menu harness lacked. Loads after b10.py; replaces whole.

launch_lang_gate (0x00493540): 4-byte `jmp 0x00495150`. The port (Boot/LaunchHandshake.cpp) is the same thunk: it
  calls the ORIGINAL 0x00495150, so both arms run one body from one image. FUN_00495150, listing from
  original\\MASHED.exe.unpatched:
    0x00495150 call 0x495120 | 0x00495155 call 0x4955b0 | 0x0049515a call 0x499710; push eax; call 0x4960a0
    0x00495165 call 0x499710; push eax; call 0x4963b0 | 0x00495170 mov eax,[0x7719e8]; add esp,8
    0x00495178 cmp eax,5; ja 0x4951a7; jmp [eax*4+0x4951c8]
      table 0x004951c8 = {0x4951a7, 0x495184, 0x49518b, 0x495192, 0x495199, 0x4951a0}
      -> case 0 / >5 unsigned: xor eax,eax (0); 1->2, 2->3, 3->4, 4->5, 5->1
    0x004951a9 push eax | 0x004951aa mov eax,[0x6147c0] | 0x004951af push eax | 0x004951b0 call 0x498510
    0x004951b5 mov ecx,[0x7719e4]; xor eax,eax; add esp,8; test ecx,ecx; sete al; ret
  Boot patch skip_controller_dialog rewrites 0x004951aa..0x004951ae to `xor eax,eax; nop; nop; nop` (diffed against
  original\\MASHED.exe): the ONLY effect is that the first argument of the 0x00498510 call is 0, not *(0x006147c0).
  The old fixture (b10.py, stub_dispatch_observe) covered one switch case: whatever DAT_007719e8 held live.
  This fixture seeds DAT_007719e8 (every table slot, the unsigned-above default, and the extremes), DAT_007719e4 (the
  sete input) and DAT_006147c0 (a sentinel that the patched bytes must NOT forward), per test, with the game's first
  thread frozen. The five init callees are named call_log recorders (all cdecl, args read from the pushes above);
  0x00499710 (mov eax,[0x7e9584]; ret) stays real, so a0/b0 log the live HWND, equal on both arms.
"""

FIXES = {}

_LLG_LOG = [{'name': 'qpf', 'at': '0x00495120', 'nargs': 0, 'ret': 1},
            {'name': 'di', 'at': '0x004955b0', 'nargs': 0, 'ret': 1},
            {'name': 'a0', 'at': '0x004960a0', 'nargs': 1, 'ret': 1},
            {'name': 'b0', 'at': '0x004963b0', 'nargs': 1, 'ret': 1},
            {'name': 'sel', 'at': '0x00498510', 'nargs': 2, 'ret': 0}]


def _llg(case, e4, i):
    return {'args': [], 'seed': [{'addr': 0x007719e8, 'val': case & 0xFFFFFFFF},
                                 {'addr': 0x007719e4, 'val': e4 & 0xFFFFFFFF},
                                 {'addr': 0x006147c0, 'val': 0x5EED0001 + i}]}


_LLG_CASES = [(0, 0), (1, 1), (2, 0), (3, 0xFFFFFFFF), (4, 0), (5, 0x100), (6, 0), (0xFFFFFFFF, 1),
              (0x80000000, 0), (0x7FFFFFFF, 0x80000000), (0x100, 0), (5, 0), (1, 0), (0, 1)]
_LLG_TESTS = [_llg(c, e, i) for i, (c, e) in enumerate(_LLG_CASES)]

FIXES['launch_lang_gate'] = {
    '__replace__': True, 'rva': 0x00493540, 'export': 'thunk_LaunchLangGate',
    'signature': {'ret': 'uint32', 'args': []},
    'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'lut_root_delta': 0,
    'fold_ret': True, 'scratch_size': 0,
    'observe': [{'addr': 0x007719e4, 'len': 8}, {'addr': 0x006147c0, 'len': 4}],
    'call_log': _LLG_LOG,
    'path1_tests': _LLG_TESTS, 'path2_tests': [_llg(5, 0, 0), _llg(2, 1, 1)],
}


# ── 0x00426dc0 FrontendRaycastForward(ray, out, frac) ─────────────────────────────────────────────────────────────
# Forwarder, listing 0x00426dc0..0x00426ddc: push [esp+0xc]/[esp+8]/[esp+4]; push 0x646e58; call 0x479100;
# add esp,0x10; ret -- EAX is 0x00479100's return, and the sole caller tests it (0x0041f62d add esp,0xc;
# 0x0041f630 test eax,eax). FUN_00479100(world, ray, out, frac): eax = [world+0x10090] (0x00479104, = DAT_00656ee8),
# call 0x4b4cd0(eax, ray, &hit) (0x0047911f; 0x004b4ce9 copies 6 dwords of the ray). Outcomes:
#   miss               eax=0 (0x00479150), nothing written
#   hit                *frac = hit dword [esp+0x98] when frac != 0 (0x0047912b..0x0047913d)
#     mat+0x84 == 0    return 0 (0x00479146 cmp word [ecx+0x84],0; jne; 0x00479150)
#     out == 0         return 1 (0x00479162 test ebp,ebp; 0x00479319)
#     mat+0x30 == 0    *out = 0x3f800000, return 1 (0x0047916e; 0x00479311)
#     else             *out = barycentric blend of 3 vertex bytes * [0x5ceb90] (call 0x559040 at 0x0047920a),
#                      clamped to [0x5cc320] (0x004792d6 fcomp; jp 0x4792f9), return 1
# The collision object exists only with a track loaded: race lane + race_sentinel DAT_00656ee8. Rays are anchored to
# live car world positions, record 0x008815a0 + i*0xd04, +0x958/+0x95c/+0x960 (x/y/z), read under freeze_main.
_CAR = [0x008815a0 + i * 0xd04 + 0x958 for i in range(4)]


def _cray(car, dx, dz, y0, y1, out=0x40, frac=0x44):
    b = _CAR[car]
    seed = [{'addr': {'scratch': 0x00}, 'val': {'f32_at': b + 0, 'add': dx}},
            {'addr': {'scratch': 0x04}, 'val': {'f32_at': b + 4, 'add': y0}},
            {'addr': {'scratch': 0x08}, 'val': {'f32_at': b + 8, 'add': dz}},
            {'addr': {'scratch': 0x0c}, 'val': {'f32_at': b + 0, 'add': dx}},
            {'addr': {'scratch': 0x10}, 'val': {'f32_at': b + 4, 'add': y1}},
            {'addr': {'scratch': 0x14}, 'val': {'f32_at': b + 8, 'add': dz}}]
    args = [{'scratch': 0}, {'scratch': out} if out is not None else 0, {'scratch': frac} if frac is not None else 0]
    return {'args': args, 'seed': seed}


_RAY_TESTS = []
for _c in range(4):
    for _dx, _dz in ((0, 0), (2, 0), (-2, 0), (0, 3), (0, -3), (6, 6), (-6, -6), (12, 0), (0, 12)):
        _RAY_TESTS.append(_cray(_c, _dx, _dz, 5.0, -5.0))
_RAY_TESTS += [_cray(0, 0, 0, 500.0, 400.0), _cray(1, 0, 0, -400.0, -500.0),        # miss above / below
               _cray(0, 0, 0, 5.0, -5.0, out=None), _cray(0, 0, 0, 5.0, -5.0, frac=None),
               _cray(0, 0, 0, 5.0, -5.0, out=None, frac=None), _cray(0, 0, 0, -5.0, 5.0)]

FIXES['frontend_raycast_forward'] = {
    '__replace__': True, 'rva': 0x00426dc0, 'export': 'FrontendRaycastForward',
    'arg_type': 'scalars_to_scattered_globals',
    'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'pointer']}, 'fold_ret': True,
    'observe': [{'addr': {'scratch': 0x40}, 'len': 8}], 'observe_raw': True,
    'scratch_size': 0x80, 'scratch_fill': 0xA5, 'freeze_main': True, 'lut_root_delta': 0,
    'scenario': 'race', 'race_sentinel': 0x00656ee8,
    'path1_tests': _RAY_TESTS, 'path2_tests': [_cray(0, 0, 0, 5.0, -5.0), _cray(0, 0, 0, 500.0, 400.0)],
}
