"""negctl re-earn, batch b11 (joypad strcpy, pipeline fragment builders, window pump/show, COM release, CRT
pre-init / exit, RW slot-7 tail call).

Decomp from re/tools/decomp_pc.py (pool slot, read-only) and re/tools/disasm_va.py on original\\MASHED.exe.unpatched.
Rows not listed here (0x0049fa40 const-zero, 0x004a31b1 / 0x004a332b ExitProcess paths, 0x004a78b0 empty loop) have no
fixture: see log/negctl_reearn/dec/b11.tsv.

Call recorders (original no-arg, cdecl, `xor eax, eax` then zero stores, `ret`):
  R1 0x00405400 -> 0x00639d70/74/78     R2 0x004298c0 -> 0x0067d98c/94/9c, 0x008991bc
  R3 0x0045c860 -> 0x0088f0a0..ac      R4 0x004842b0 -> 0x006cf068/6c, 0x006cfc88/8c
  G  0x00402f40  `mov eax, [0x00636ad8]; ret` (getter, returns the seeded dword)
  T  0x004a2710  thiscall, `this[0..0x14] = 0,0,0,0xa,0,0; ret 4`
  P  0x004099e0  `mov eax, [esp+4]; mov [0x008a95a0], eax; ret`
Every recorder target window is pre-filled 0xA5 and restored after each arm.
"""

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'

R1, R2, R3, R4, G, T, P = 0x00405400, 0x004298c0, 0x0045c860, 0x004842b0, 0x00402f40, 0x004a2710, 0x004099e0
_REC_OBS = [{'addr': 0x00639d70, 'len': 12, 'fill': 0xA5}, {'addr': 0x0067d98c, 'len': 0x14, 'fill': 0xA5},
            {'addr': 0x008991bc, 'len': 4, 'fill': 0xA5}, {'addr': 0x0088f0a0, 'len': 16, 'fill': 0xA5},
            {'addr': 0x006cf068, 'len': 8, 'fill': 0xA5}, {'addr': 0x006cfc88, 'len': 8, 'fill': 0xA5}]


def _tbl(base, vals):
    return [{'addr': base + 4 * i, 'val': v} for i, v in enumerate(vals)]


# ── 0x00495830 JoypadStrcpy(int idx, char* dst) ─────────────────────────────────────────────────────────────────────
# if (idx < DAT_00772fac && dst != 0) { strcpy(dst, 0x00771eb4 + idx*0x448); return 1; } return 0. Signed compare
# (idx -1 copies from 0x00771a6c). Old int_pair vectors passed dst = 0 or count was 0: every call returned 0.
def _str(addr, s):
    return [{'addr': addr + i, 'val': b, 'size': 1} for i, b in enumerate(s.encode() + b'\0')]


def _js(idx, dst, count, src_s, src_idx=None):
    si = idx if src_idx is None else src_idx
    return {'args': [idx & 0xFFFFFFFF, dst],
            'seed': [{'addr': 0x00772fac, 'val': count & 0xFFFFFFFF}] + _str(0x00771eb4 + si * 0x448, src_s)}


FIXES['joypad_strcpy'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'int', 'args': ['int32', 'pointer']}, 'fold_ret': True,
    'observe': [{'addr': {'scratch': 0}, 'len': 16}], 'scratch_size': 0x20, 'scratch_fill': 0xEE,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_js(0, {'scratch': 0}, 4, 'AB'), _js(2, {'scratch': 0}, 4, 'XYZW123'),
                    _js(3, {'scratch': 4}, 4, 'pad'), _js(4, {'scratch': 0}, 4, 'no', 0),
                    _js(1, 0, 4, 'nul'), _js(-1, {'scratch': 0}, 4, 'Q'), _js(0, {'scratch': 0}, 0, 'zz'),
                    _js(1, {'scratch': 2}, 2, '')],
    'path2_tests': [_js(0, {'scratch': 0}, 4, 'AB')],
}


# ── 0x00496d60 / 0x00496db0 PipelineFragment{4eb3c0,4eb9d0}Create ───────────────────────────────────────────────────
# Node getters 0x004eb3c0 / 0x004eb9d0 return the static nodes 0x00618708 / 0x00618768 (`mov eax, imm32; ret`). The
# callees write node+0x3c (0x00618744 / 0x006187a4): negctl field_first/field_last flipped that byte and read GREEN
# because no window covered the node. Observe the node (seed: False -> observe + restore only).
for _k, _node in (('pipeline_fragment_4eb3c0_create', 0x00618708), ('pipeline_fragment_4eb9d0_create', 0x00618768)):
    FIXES[_k] = {
        'observe_globals': [{'addr': '0x007d3ff8', 'deref': True, 'len': 300, 'seed': False},
                            {'addr': '0x007d3ff8', 'deref': True, 'len': 4, 'off': 1324, 'seed': False},
                            {'addr': hex(_node), 'len': 0x60, 'seed': False}],
    }


# ── 0x00499690 WindowMsgPump ────────────────────────────────────────────────────────────────────────────────────────
# Disasm 0x00499690..0x004996e8: PeekMessageA(0x007e95a0, 0, 0, 0, 1); if != 0 { if [0x007e95a4] == 0x12 -> return 1
# (0x004996b0 mov eax, 1); TranslateMessage; DispatchMessageA }; if [0x0077391c] == 0 WaitMessage (0x004996d3 is the
# no_focus_pause boot patch: patched to always skip); return [0x00773918] != 0. Old vectors: the A/B thread has no
# queue, so PeekMessageA returned 0 and both sides returned the 0 quit flag. Import stand-ins return the scripted
# PeekMessageA result and record every call; the MSG id and both flags are seeded. Active flag kept 1 (no boot-patch
# path). PeekMessageA results are indexed by call number: one call per arm, two arms per test.
_MP = [(1, 0x0f, 1, 0), (1, 0x0f, 1, 1), (1, 0x12, 1, 0), (1, 0x12, 1, 1), (1, 0x100, 1, 0)]


# The generic import_standin wrapper (diff_template.js:1688 `globalThis.send = ...`) throws "'send' is read-only"
# under scalars_to_scattered_globals, so both window rows use standin_object_call: t.globals stores, observe_globals
# (seed: False) saves/observes/restores them, the import log is part of each arm's observation.
_MPG = [{'addr': '0x007e95a0', 'len': 8, 'seed': False}, {'addr': '0x00773918', 'len': 8, 'seed': False}]


def _mp(msg, active, quit_):
    return {'args': [], 'globals': [[0x007e95a4, msg], [0x0077391c, active], [0x00773918, quit_]]}


FIXES['window_msg_pump'] = {
    'arg_type': 'standin_object_call', 'signature': {'ret': 'int', 'args': []}, 'object_alloc': 'frida',
    'observe_globals': _MPG, 'freeze_main': True, 'heap_standin': None,
    'import_standin': {'USER32.dll!PeekMessageA': {'args': 5, 'ret': 1},   # ret arrays index calls WITHIN an arm (standins.js:309)
                       'USER32.dll!TranslateMessage': {'args': 1, 'ret': 1},
                       'USER32.dll!DispatchMessageA': {'args': 1, 'ret': 0},
                       'USER32.dll!WaitMessage': {'args': 0, 'ret': 1}},
    'path1_tests': [_mp(t[1], t[2], t[3]) for t in _MP],
    'path2_tests': [_mp(0, 1, 0)],
}


# ── 0x004996f0 WindowShow(int nCmdShow) ─────────────────────────────────────────────────────────────────────────────
# ShowWindow(DAT_007e9584, n); UpdateWindow(DAT_007e9584). No memory effect: the old vectors compared void == void.
# Fake HWND stored, both imports stood in and recorded with their arguments.
def _ws(n, hwnd=0x00012345):
    return {'args': [n], 'globals': [[0x007e9584, hwnd]]}


FIXES['window_show'] = {
    'arg_type': 'standin_object_call', 'signature': {'ret': 'void', 'args': ['int32']}, 'object_alloc': 'frida',
    'observe_globals': [{'addr': '0x007e9584', 'len': 4, 'seed': False}], 'freeze_main': True, 'heap_standin': None,
    'import_standin': {'USER32.dll!ShowWindow': {'args': 2, 'ret': 1},
                       'USER32.dll!UpdateWindow': {'args': 1, 'ret': 1}},
    'path1_tests': [_ws(1), _ws(5), _ws(0), _ws(9), _ws(3, 0x00055aa0)],
    'path2_tests': [_ws(5)],
}


# ── 0x0049f180 / 0x0049f2b0 ComRefRelease14 / ComRefRelease10 ───────────────────────────────────────────────────────
# Disasm 0x0049f180: n = InterlockedDecrement(p+0x14); if (n == 0 && p) { push 1; ecx = p; call [[p]+0x1c] }; return n.
# Old vectors started at 2..0x7fffffff: the count never reached 0 and the release call never ran. Added: count 1 with a
# vtable whose slot 7 is T (0x004a2710, thiscall ret 4, zeroes this[0..0x14] and writes this+0xc = 0xa), and count 0.
for _k, _off in (('com_ref_release14', 0x14), ('com_ref_release10', 0x10)):
    _vt = {'ptr_off': 0, 'size': 0x20, 'fields': [{'off': 0x1c, 'type': 'u32', 'value': T}]}
    _old = [{'seeds': [{'off': _off, 'type': 'u32', 'value': v}]} for v in (2, 3, 5, 16, 0x7fffffff)]
    _new = [{'seeds': [{'off': _off, 'type': 'u32', 'value': 1}, {'off': 4, 'type': 'u32', 'value': 0x11111111},
                       {'off': 8, 'type': 'u32', 'value': 0x22222222}], 'nested': [_vt]},
            {'seeds': [{'off': _off, 'type': 'u32', 'value': 1}], 'nested': [_vt]},
            {'seeds': [{'off': _off, 'type': 'u32', 'value': 0}], 'nested': [_vt]},
            {'seeds': [{'off': _off, 'type': 'u32', 'value': 2}], 'nested': [_vt]}]
    FIXES[_k] = {
        # off 0 (the per-side nested vtable pointer) is not observed: it differs by address on the no-call vectors.
        'observe': [{'off': 4, 'src': 'struct', 'type': 'u32'},
                    {'off': 8, 'src': 'struct', 'type': 'u32'}, {'off': 0xc, 'src': 'struct', 'type': 'u32'},
                    {'off': 0x10, 'src': 'struct', 'type': 'u32'}, {'off': 0x14, 'src': 'struct', 'type': 'u32'}],
        'path1_tests': _old + _new,
        'path2_tests': [_new[0]],
    }


# ── 0x004a31f3 CrtPreInit ───────────────────────────────────────────────────────────────────────────────────────────
# if (PTR 0x00616044) call it; r = 0; for slot in 0x005ea03c..0x005ea058 (7): { if (r) return r; if (slot) r = slot(); }
# if (r == 0) { _atexit(0x004a78f4); for slot in 0x005ea000..0x005ea038 (14): if (slot) slot(); r = 0; } return r.
# Old: zero-arg call returning 0 on both sides. All 21 table slots and the pre-init pointer are seeded (the live game's
# C++ initializers never run); the atexit list end 0x008ab6cc / begin 0x008ab6d0 are observed and restored per arm.
def _pi(pre, t1, t2, gval=0):
    t1 = list(t1) + [0] * (7 - len(t1))
    t2 = list(t2) + [0] * (14 - len(t2))
    return {'args': [], 'seed': [{'addr': 0x00616044, 'val': pre}, {'addr': 0x00636ad8, 'val': gval}]
            + _tbl(0x005ea03c, t1) + _tbl(0x005ea000, t2)}


FIXES['crt_pre_init'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'int', 'args': []}, 'fold_ret': True,
    'observe': _REC_OBS + [{'addr': 0x008ab6cc, 'len': 8}], 'freeze_main': True, 'target_global': None,
    'path1_tests': [_pi(R1, [G], [R3], 0x77), _pi(0, [0, R2, G], [R4], 5), _pi(R1, [R2], [R3, 0, 0, 0, 0, 0, 0, R4]),
                    _pi(0, [], [0, R4]), _pi(R4, [0, 0, 0, 0, 0, 0, G], [R1], 0x80000001)],
    'path2_tests': [_pi(R1, [G], [R3], 0x77)],
}


# ── 0x004a3258 CrtExitCore(code, skip_atexit, keep) / 0x004a334d CrtExitNormal_j5 = core(0, 0, 1) ───────────────────
# __lock(8); if (DAT_007739dc == 1) TerminateProcess; DAT_007739d8 = 1; (byte) DAT_007739d4 = keep; if (!skip_atexit) {
# if (DAT_008ab6d0) walk DAT_008ab6cc down to DAT_008ab6d0 calling each non-null; slots 0x005ea05c..0x005ea064 }
# slots 0x005ea068..0x005ea070; keep ? FUN_004a77eb(8) : (DAT_007739dc = 1, ___crtExitProcess(code)).
# Old: DEGENERATE int_scalar (every arm keep = 0 -> ExitProcess; crash-equal). Every vector here has keep != 0. The
# atexit list is pointed at a scratch table of recorders (list end observed), the four exit slots are seeded.
def _ec(args, atexit, t1, t2, guard=0):
    s = [{'addr': 0x007739dc, 'val': guard}]
    if atexit is None:
        s += [{'addr': 0x008ab6d0, 'val': 0}, {'addr': 0x008ab6cc, 'val': {'scratch': 0x20}}]
    else:
        s += [{'addr': 0x008ab6d0, 'val': {'scratch': 0x10}},
              {'addr': 0x008ab6cc, 'val': {'scratch': 0x10 + 4 * len(atexit)}}]
        s += [{'addr': {'scratch': 0x10 + 4 * i}, 'val': v} for i, v in enumerate(atexit)]
    s += _tbl(0x005ea05c, t1) + _tbl(0x005ea068, t2)
    return {'args': args, 'seed': s}


_EC_OBS = _REC_OBS + [{'addr': 0x007739d4, 'len': 8, 'fill': 0xA5}, {'addr': 0x008ab6cc, 'len': 8}]
FIXES['crt_exit_core'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['uint32', 'int32', 'int32']},
    'observe': _EC_OBS, 'scratch_size': 0x40, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_ec([7, 0, 1], [R1, 0, R2], [R3, 0], [R4, 0]), _ec([7, 1, 1], [R1, 0, R2], [R3, 0], [R4, 0]),
                    _ec([0, 0, 0x100], [R2], [0, R1], [0, R3]), _ec([3, 0, 1], None, [R3, R4], [0, 0]),
                    _ec([5, 1, 2], None, [R1, 0], [R2, R4]), _ec([9, 0, 0xff], [0, 0, R4, R1], [0, 0], [R2, 0])],
    'path2_tests': [_ec([7, 0, 1], [R1, 0, R2], [R3, 0], [R4, 0])],
}
FIXES['crt_exit_normal_j5'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': []},
    'observe': _EC_OBS, 'scratch_size': 0x40, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_ec([], [R1, 0, R2], [R3, 0], [R4, 0]), _ec([], [R2], [0, R1], [0, R3]),
                    _ec([], None, [R3, R4], [0, 0]), _ec([], [0, 0, R4, R1], [0, 0], [R2, 0])],
    'path2_tests': [_ec([], [R1, 0, R2], [R3, 0], [R4, 0])],
}


# ── 0x004c19f0 RwVtableSlot07Call(p) ────────────────────────────────────────────────────────────────────────────────
# Disasm: mov eax, [esp+4]; mov [esp+4], eax; jmp [eax+0x1c] -- a tail jump, so the slot receives p as its first stack
# argument. Old vectors: crash-equal / no observation. Slot 7 of a scratch record = P (stores its [esp+4] to
# 0x008a95a0, pre-filled 0xA5).
def _vs(off):
    return {'args': [{'scratch': off}], 'seed': [{'addr': {'scratch': off + 0x1c}, 'val': P}]}


FIXES['rw_vtable_slot07_call'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5}], 'scratch_size': 0x60, 'freeze_main': True,
    'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_vs(0), _vs(0x10), _vs(0x20), _vs(0x40)],
    'path2_tests': [_vs(0)],
}
