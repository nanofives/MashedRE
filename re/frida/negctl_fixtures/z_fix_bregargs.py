"""bregargs (2026-10-12, triage B): register-arg delivery in the warm lane (0x00427840) and an import-thunk callee
(0x00495530). Loads last; replaces both entries whole. Byte citations: `py -3.12 re/tools/disasm_fn.py` on
original\\MASHED.exe.unpatched.

font_text_utf16_widen_copy (0x00427840..0x00427875): src in EAX (0x00427842 mov edi,eax), dst in ECX
  (0x0042784a mov esi,ecx), len = (*(0x007d3ff8))[+0xf4](src) (0x00427844..0x00427852, cdecl, 1 arg), and when
  len > 0 (signed, 0x00427857 jle) len bytes are zero-extended into dst words (0x00427860 movzx cx,[edi]), then one
  0 word is stored (0x0042786f). EAX on return: 0 after the loop, len when len <= 0. New key `reg_args`
  {eax: 0, ecx: 1} delivers both args through standins.js regThunk on BOTH arms. Scratch: src at +0x000, dst at
  +0x100 (0x200 bytes), a fake RwGlobals at +0x300; the whole scratch is the observe window, filled 0xA5 before each
  arm, so a missing terminator, a write past it, or a sign-extended byte all change it.
  Vectors 0..4 keep the LIVE RwGlobals (its +0xf4 is the length of the NUL-terminated string). Vectors 5..10 point
  *(0x007d3ff8) at scratch+0x300, whose +0xf4 is the `len` recorder: it logs the src it was handed (as s+off and the
  string there) and returns the test's length, so a missing `push edi` (the 2026-05-27 defect,
  re/analysis/menu_crash_scoping/REPORT.md) and the len <= 0 / len > strlen paths are seen. Hence freeze_main.

create_dinput_object (0x00495530..0x00495575): 0x004987b0("Creating DInput object\\n") (0x00495530), GetModuleHandleA(0)
  through [0x5cc038] (0x00495550), DirectInput8Create(hinst, 0x800, 0x5d0a8c, 0x771e78, 0) through the thunk 0x0049b300
  (jmp [0x5cc014]), jge 0x00495570 -> return 1, else 0x004987b0("Failed\\n") and return 0. The old stub_at fixture could
  not install on the thunk (Frida follows jmp [IAT] to dinput8). Here the IAT DWORDS are swapped by the import
  stand-in (standins.js), gated to the A/B thread: the real DirectInput8Create never runs on either arm, and the game's
  own threads forward to it. Recorded per arm: GetModuleHandleA's arg, all five DirectInput8Create args (hinst is the
  stand-in's sentinel 0x51510000, so the port must pass GetModuleHandleA's result through), and the TEXT each
  0x004987b0 call hands OutputDebugStringA (its buffer is on the stack, 0x004987d9). Per-test `import_ret` sets the
  HRESULT, so both sides of the sign test run. 0x00771e78 is observed (seed off: it holds the live interface).
"""

FIXES = {}


def _bytes_seeds(off, bs):
    bs = list(bs)
    while len(bs) % 4:
        bs.append(0)
    return [{'addr': {'scratch': off + i}, 'val': bs[i] | (bs[i + 1] << 8) | (bs[i + 2] << 16) | (bs[i + 3] << 24)}
            for i in range(0, len(bs), 4)]


_SRC, _DST, _RWG = 0x000, 0x100, 0x300
_ARGS = [{'scratch': _SRC}, {'scratch': _DST}]


def _live(s):
    """Live RwGlobals: len = rwstrlen(src). s: bytes WITHOUT the terminator (appended here)."""
    return {'args': _ARGS, 'seed': _bytes_seeds(_SRC, list(s) + [0])}


def _fake(s, n, nul=True):
    """Fake RwGlobals: len = n from the `len` recorder. nul=False leaves the 0xA5 fill after s."""
    seed = [{'addr': 0x007d3ff8, 'val': {'scratch': _RWG}},
            {'addr': {'scratch': _RWG + 0xf4}, 'val': {'call_log': 'len'}}]
    return {'args': _ARGS, 'seed': seed + _bytes_seeds(_SRC, list(s) + ([0] if nul else [])),
            'stub_rets': {'len': n}}


_LONG = [((i * 7) % 255) + 1 for i in range(255)]           # 255 non-zero bytes, 0x01..0xff
_WC_TESTS = [
    _live(b'Hello'),                                         # 0: ret 0, dst "Hello\0"
    _live(b''),                                              # 1: len 0 -> ret 0, dst[0] = 0 only
    _live([0x80, 0xA9, 0xFF, 0xE9, 0x7F, 0x01]),             # 2: high bytes zero-extended (0x0080 .. 0x0001)
    _live(b'Z'),                                             # 3
    _live(_LONG),                                            # 4: 255 words + terminator at +0x2fe
    _fake(b'ABCDEFGH', 3),                                   # 5: copies 3, not strlen 8 -> "ABC\0"
    _fake(b'ABCDEFGH', 0),                                   # 6: ret 0, terminator only
    _fake(b'ABCDEFGH', 0xffffffff),                          # 7: len -1 -> ret 0xffffffff, terminator only
    _fake(b'ABCDEFGH', 0x80000000),                          # 8: signed jle -> ret 0x80000000, terminator only
    _fake(b'ABC', 8),                                        # 9: copies the NUL and 4 fill bytes (no stop at NUL)
    _fake([0xFE], 1, nul=False),                             # 10: 0x00FE then 0
]

FIXES['font_text_utf16_widen_copy'] = {
    '__replace__': True, 'rva': 0x00427840, 'export': 'FontText_UTF16WidenCopy',
    'arg_type': 'scalars_to_scattered_globals',
    'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer']},
    'reg_args': {'eax': 0, 'ecx': 1},
    'freeze_main': True, 'lut_root_delta': 0, 'fold_ret': True,
    'scratch_size': 0x400, 'scratch_fill': 0xA5,
    'observe': [{'addr': {'scratch': 0}, 'len': 0x400}],
    'call_log': [{'name': 'len', 'nargs': 1, 'deref': {0: 'a'}}],
    'path1_tests': _WC_TESTS, 'path2_tests': [_WC_TESTS[0], _WC_TESTS[2]],
}


def _di(hr):
    return {'args': [], 'import_ret': {'DirectInput8Create': hr}}


_DI_TESTS = [_di(v) for v in (0x00000000, 0x00000001, 0x7fffffff,              # jge taken -> 1
                              0x80000000, 0x80004005, 0xffffffff, 0x8007000e)]  # negative -> "Failed\n", 0

FIXES['create_dinput_object'] = {
    '__replace__': True, 'rva': 0x00495530, 'export': 'CreateDInputObject',
    'arg_type': 'standin_object_call', 'signature': {'ret': 'uint32', 'args': []}, 'object_alloc': 'frida',
    'freeze_main': True, 'heap_standin': None, 'lut_root_delta': 0,
    'observe_globals': [{'addr': '0x00771e78', 'len': 4, 'seed': False}],
    'import_standin': {'KERNEL32.dll!GetModuleHandleA': {'args': 1, 'ret': 0x51510000},
                       'DINPUT8.dll!DirectInput8Create': {'args': 5, 'ret': 0},
                       'KERNEL32.dll!OutputDebugStringA': {'args': 1, 'ret': 0, 'deref': {0: 'a'}}},
    'path1_tests': _DI_TESTS, 'path2_tests': [_DI_TESTS[0], _DI_TESTS[4]],
}
