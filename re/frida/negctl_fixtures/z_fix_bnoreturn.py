"""negctl follow-up, triage B no-return rows (bnoreturn 2026-10-12). Loads last; replaces both entries whole.

Both rows end in a Win32 call that never returns, so a stand-in that returns sends the caller into the int3 at
0x004a31e0 and both arms error alike (the old int_scalar entries never even reached the body: 3-arg signature, 1-arg
test, "bad argument count" on both arms, negctl calls=0). The new channel is the import stand-in's `noreturn` mode
(re/frida/js/standins.js): the IAT slots of MASHED.exe and of the .asi are swapped for the A/B thread only, the call
and its arguments are recorded, then the stand-in faults on its own PAGE_NOACCESS page and Frida's NativeFunction
unwinds to the frame it saved at entry. The real ExitProcess / TerminateProcess never run in the harness process.
The per-arm import log (row S<k> of the CSV) carries the whole path: which import, in which order, with which args.
Byte citations: `py -3.12 re/tools/disasm_va.py <va> <len> original/MASHED.exe.unpatched`.

crt_exit_process_j5 (0x004a31b1, ___crtExitProcess):
  0x004a31b1 push 0x5d11e0 "mscoree.dll"; call [0x5cc038] GetModuleHandleA; test eax; je 0x4a31d6
  0x004a31c0 push 0x5d11d0 "CorExitProcess"; push eax; call [0x5cc0b4] GetProcAddress; test eax; je 0x4a31d6
  0x004a31d0 push [esp+4]; call eax               <- NO stack adjust after: the callee pops its arg (stdcall)
  0x004a31d6 push [esp+4]; call [0x5cc1b8] ExitProcess; 0x004a31e0 int3
  The stand-ins: GetModuleHandleA returns 0 or a fake handle 0x7e570000, GetProcAddress returns 0 or one of two FAKE
  function pointers: CorExitProcess (stdcall, records, RETURNS, so the code after 0x004a31d4 runs) or
  CorExitProcessNR (records, never returns). String args are logged by content (a"..."), not by address.

crt_exit_no_return_j5 (0x004a332b): push 0; push 0; push [esp+0xc]; call 0x4a3258 (CrtExitCore(code, 0, 0)).
  CrtExitCore 0x004a3258: __lock(8) 0x004a325f (EnterCriticalSection [0x5cc068] on *(0x616408 + 8*8));
  if [0x7739dc] == 1: TerminateProcess(GetCurrentProcess(), code) 0x004a3270..0x004a327a; [0x7739d8] = 1, byte
  [0x7739d4] = 0 (0x004a3287 / 0x004a328d); skip_atexit == 0: walk *(0x8ab6cc) down to *(0x8ab6d0) calling each
  non-null (0x004a3294..0x004a32c5), then slots 0x005ea05c..0x005ea060 (0x004a32c7..0x004a32e4); slots
  0x005ea068..0x005ea06c (0x004a32e6..0x004a3303); keep == 0: [0x7739dc] = 1 (0x004a3319), ___crtExitProcess(code)
  (0x004a3323). Callees by Ghidra boundary: ___endstdio 0x004a5f07 and FUN_004af32d sit in those slot tables (real
  game teardown: stream flush/close). They NEVER run here: every vector seeds all four slots and the atexit list
  with the b11 recorders (R1..R4 zero their own windows) and the seeds are restored after each arm.
  EnterCriticalSection / LeaveCriticalSection are stand-ins too (recorded, not performed): the no-return path never
  reaches _unlock 0x004a77eb, so a real lock 8 would stay owned by the A/B thread and deadlock the game's next
  atexit registration. 0x007739dc is observed (b11's _EC_OBS stopped at 0x007739db).
"""

FIXES = {}

R1, R2, R3, R4 = 0x00405400, 0x004298c0, 0x0045c860, 0x004842b0      # b11 recorders (see b11.py header)
_REC_OBS = [{'addr': 0x00639d70, 'len': 12, 'fill': 0xA5}, {'addr': 0x0067d98c, 'len': 0x14, 'fill': 0xA5},
            {'addr': 0x008991bc, 'len': 4, 'fill': 0xA5}, {'addr': 0x0088f0a0, 'len': 16, 'fill': 0xA5},
            {'addr': 0x006cf068, 'len': 8, 'fill': 0xA5}, {'addr': 0x006cfc88, 'len': 8, 'fill': 0xA5}]
FAKE_MSCOREE = 0x7e570000

_IMPORTS = {
    'KERNEL32.dll!GetModuleHandleA': {'args': 1, 'strings': [0], 'ret': 0},
    'KERNEL32.dll!GetProcAddress': {'args': 2, 'strings': [1], 'ret': 0},
    'KERNEL32.dll!ExitProcess': {'args': 1, 'noreturn': True},
    'KERNEL32.dll!TerminateProcess': {'args': 2, 'noreturn': True},
    'KERNEL32.dll!EnterCriticalSection': {'args': 1, 'ret': 0},
    'KERNEL32.dll!LeaveCriticalSection': {'args': 1, 'ret': 0},
}
_FAKES = {'CorExitProcess': {'args': 1, 'abi': 'stdcall', 'ret': 0},
          'CorExitProcessNR': {'args': 1, 'noreturn': True}}
# mscoree modes: absent (GetModuleHandleA -> 0), nofn (-> handle, GetProcAddress -> 0), corret / cornr (-> a fake).
_MODES = {
    'absent': {'GetModuleHandleA': 0},
    'nofn': {'GetModuleHandleA': FAKE_MSCOREE, 'GetProcAddress': 0},
    'corret': {'GetModuleHandleA': FAKE_MSCOREE, 'GetProcAddress': {'fake': 'CorExitProcess'}},
    'cornr': {'GetModuleHandleA': FAKE_MSCOREE, 'GetProcAddress': {'fake': 'CorExitProcessNR'}},
}
_COMMON = {
    '__replace__': True, 'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'lut_root_delta': 0,
    'signature': {'ret': 'void', 'args': ['uint32']}, 'fold_ret': False,
    'import_standin': _IMPORTS, 'standin_fakes': _FAKES,
}


# ── 0x004a31b1 CrtExitProcess_j5(code) ──────────────────────────────────────────────────────────────────────────────
def _ep(code, mode):
    return {'args': [code & 0xFFFFFFFF], 'seed': [], 'import_rets': _MODES[mode]}


_EP_TESTS = [_ep(0, 'absent'), _ep(1, 'nofn'), _ep(0xff, 'corret'), _ep(7, 'cornr'), _ep(0x80000000, 'absent'),
             _ep(-1, 'corret'), _ep(0x12345678, 'nofn'), _ep(0x7fffffff, 'cornr'), _ep(3, 'corret')]

FIXES['crt_exit_process_j5'] = dict(_COMMON, **{
    'rva': 0x004a31b1, 'export': 'CrtExitProcess_j5', 'scratch_size': 0, 'observe': [],
    'path1_tests': _EP_TESTS, 'path2_tests': [_ep(0xff, 'corret'), _ep(7, 'cornr')],
})


# ── 0x004a332b CrtExitNoReturn_j5(code) = CrtExitCore(code, 0, 0) ────────────────────────────────────────────────────
def _tbl(base, vals):
    return [{'addr': base + 4 * i, 'val': v} for i, v in enumerate(vals)]


def _nr(code, atexit, t1, t2, guard=0, mode='absent'):
    s = [{'addr': 0x007739dc, 'val': guard}]
    if atexit is None:
        s += [{'addr': 0x008ab6d0, 'val': 0}, {'addr': 0x008ab6cc, 'val': {'scratch': 0x20}}]
    else:
        s += [{'addr': 0x008ab6d0, 'val': {'scratch': 0x10}},
              {'addr': 0x008ab6cc, 'val': {'scratch': 0x10 + 4 * len(atexit)}}]
        s += [{'addr': {'scratch': 0x10 + 4 * i}, 'val': v} for i, v in enumerate(atexit)]
    s += _tbl(0x005ea05c, t1) + _tbl(0x005ea068, t2)
    return {'args': [code & 0xFFFFFFFF], 'seed': s, 'import_rets': _MODES[mode]}


_NR_OBS = _REC_OBS + [{'addr': 0x007739d4, 'len': 8, 'fill': 0xA5}, {'addr': 0x007739dc, 'len': 4},
                      {'addr': 0x008ab6cc, 'len': 8}]
_NR_TESTS = [_nr(7, [R1, 0, R2], [R3, 0], [R4, 0]), _nr(0, [R2], [0, R1], [0, R3]),
             _nr(3, None, [R3, R4], [0, 0]), _nr(0xff, [0, 0, R4, R1], [0, 0], [R2, 0], mode='corret'),
             _nr(5, [R1], [R2, 0], [0, R4], guard=1), _nr(0x80000000, None, [0, 0], [0, 0], mode='cornr'),
             _nr(-1, [R4, R3, R2, R1], [R1, R2], [R3, R4], mode='nofn'), _nr(9, [R3], [R4, 0], [R1, R2], guard=1,
                                                                             mode='corret')]

FIXES['crt_exit_no_return_j5'] = dict(_COMMON, **{
    'rva': 0x004a332b, 'export': 'CrtExitNoReturn_j5', 'scratch_size': 0x40, 'observe': _NR_OBS,
    'path1_tests': _NR_TESTS, 'path2_tests': [_nr(7, [R1, 0, R2], [R3, 0], [R4, 0])],
})
