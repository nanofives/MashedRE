"""negctl re-earn fix, cluster bootio (2026-10-12). Loads last; overrides earlier entries by hook key.

fps_discretise (0x00493480): b10's entry runs the real callee FUN_00493390, which ends in `mov dword [0x7f1000], 0x32`
(0x004933d5) on every call, so the four snap bands (0x0049348a..0x004934cf) only ever see ms = 50 and the 0xc5 lower
bound of the 200 band cannot be told from the old port's 0xc4. Here the callee is replaced by the scalar handler's
recorder (stub_at, 0 args, cdecl: `call 0x493390` at 0x00493480 pushes nothing) and DAT_007f1000 is seeded directly,
so ms is a test input. The recorder's call count sits in the scratch and is observed, so a port that skips the call is
RED. Vectors cover every band edge (in and out), negative ms and acc (signed jl/jg at 0x0049348d.. and 0x004934e1 /
0x004934fd / 0x00493505), the 0x31/0x32 and 0x2f/0x30 and 2/3 remainder edges, and a wrapping sum.

fsopen_safe (0x004a4541): the old int_pair entry passed (0, 0) and both arms faulted equally (crash_equal_ok). The body
is `push 0x40; push mode; push filename; call 0x004a44e5; add esp,0xc; ret`. A stub_dispatch_observe recorder replaces
the game __fsopen at 0x004a44e5 (cdecl, 3 args), so nothing is opened; the recorded (filename, mode, shflag) triple and
the passed-through return are the observable. The reference is the PATCHED MASHED.exe, whose 0x004a4541 is the fix_fopen
JMP into the cave 0x0050f6c4: same call, then `cmp eax,0x10000; jae; xor eax,eax`. Scripted returns are therefore 0 or
>= 0x10000, where cave and unpatched body agree. A return in 1..0xffff would compare the boot patch, not the port.
"""

FIXES = {}


# ── 0x00493480 FpsDiscretise ────────────────────────────────────────────────────────────────────────────────────────
def _fd(ms, acc):
    s = [{'addr': 0x007f1000, 'val': ms & 0xFFFFFFFF}, {'addr': 0x007719d4, 'val': acc & 0xFFFFFFFF}]
    return {'args': [], 'seed': s}


_FD_TESTS = [
    _fd(0x2e, 0), _fd(0x2f, 0), _fd(0x35, 0), _fd(0x36, 0),
    _fd(0x60, 0), _fd(0x61, 0), _fd(0x67, 0), _fd(0x68, 0),
    _fd(0x92, 0), _fd(0x93, 0), _fd(0x99, 0), _fd(0x9a, 0),
    _fd(0xc4, 0), _fd(0xc5, 0), _fd(0xcb, 0), _fd(0xcc, 0),
    _fd(0x32, 0x2f), _fd(0x32, 0x30), _fd(0x32, 0x31), _fd(0, 0x31), _fd(0, 0x32), _fd(0, 2), _fd(0, 3),
    _fd(0x32, -60), _fd(-1, 0), _fd(-0x32, 0x10), _fd(0, -0x7FFFFFFF), _fd(0x7FFFFFF0, 0x20),
    _fd(1000, 17), _fd(0x12345, 0x2a),
]

FIXES['fps_discretise'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'freeze_main': True, 'target_global': None, 'needs_booted_race': None,
    'scratch_size': 0x40,
    'stub_at': [{'addr': 0x00493390, 'nargs': 0, 'abi': 'mscdecl', 'ret': 0}], 'stub_nargs': None, 'stub_ret': None,
    'recorder': {'args': 0, 'abi': 'mscdecl', 'off': 0x0, 'max_calls': 4},
    'observe': [{'addr': 0x007f1000, 'len': 8, 'fill': 0xA5}, {'addr': 0x007719d4, 'len': 4},
                {'addr': {'scratch': 0x0}, 'len': 4}],
    'pre_fill_byte': None, 'observe_addrs': None, 'seed_sets': None,
    'path1_tests': _FD_TESTS, 'path2_tests': _FD_TESTS[:2],
}


# ── 0x004a4541 FsopenSafe(filename, mode) ───────────────────────────────────────────────────────────────────────────
# File names are ones the game does not use and the modes are read-only, so a broken port that reaches a real CRT
# fopen (the witness run of the old port did) opens nothing and creates nothing. A first draft used 'contcfg0.bin' /
# 'wb' and the witness run created a 0-byte original\contcfg0.bin (CreationTime = that run, removed afterwards).
def _fo(ret, name=b'zz_bootio_absent_a.bin', mode=b'rb'):
    seed = [{'buf': 0, 'off': k, 'type': 'u8', 'value': c} for k, c in enumerate(name)]
    seed += [{'buf': 1, 'off': k, 'type': 'u8', 'value': c} for k, c in enumerate(mode)]
    return {'scalars': [], 'seed': seed, 'stub_rets': {'fsopen': ret}}


_FO_TESTS = [_fo(0), _fo(0x00ABC010), _fo(0x00010000), _fo(0x7FFFFFFF), _fo(-1), _fo(0x00ABC010, b'zz_bootio_absent_b.bin', b'r'),
             _fo(0, b'zz_bootio_absent_c.bin', b'rt')]

FIXES['fsopen_safe'] = {
    'arg_type': 'stub_dispatch_observe', 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer']},
    'num_bufs': 2, 'buf_size': 0x20, 'arg_layout': [{'buf': 0}, {'buf': 1}],
    'stub_at': [{'addr': '0x004a44e5', 'nargs': 3, 'abi': 'mscdecl', 'name': 'fsopen'}], 'stub_nargs': 3,
    'stub_abi': 'mscdecl', 'stub_ret': 0, 'observe_calls': True, 'observe_ret': True, 'crash_equal_ok': None,
    'freeze_main': True,
    'path1_tests': _FO_TESTS, 'path2_tests': _FO_TESTS[:2],
}
