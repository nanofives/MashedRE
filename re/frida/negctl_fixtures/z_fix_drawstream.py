"""drawstream (2026-10-12, triage B item 9): draw-stream fixtures for 0x00427ad0 MenuMenusBB and 0x00430b90
ProgressBarSetA. Loads last; replaces both entries whole.

Both rows were demoted because their only output is draws: run_diff could not see past the render slot (BB) or past
the quad / sprite / text callees (PBSA). Here every DRAW callee, found by Ghidra function boundary
(`re/tools/decomp_pc.py 0x00427ad0 0x00430b90 --callees`), is a named `call_log` recorder installed at its address
while the game's first thread is frozen, so each arm's ordered call sequence, with float args by value and the
pointed-to rect / string / colour data, is the fingerprint. Callees that compute rather than draw stay REAL and
their globals are seeded instead. Byte citations: `py -3.12 re/tools/disasm_fn.py` on original\\MASHED.exe.unpatched.

menu_menus_bb (0x00427ad0): callees 0x00427780 + 0x004277a0 (string id -> UTF-16 stack buffer, register ABI, REAL),
  0x00552d10 `push`, 0x00556e90 `color` (ctx, &argb x4, 0x00427b05..0x00427b2c), 0x005555b0 `text`
  (font, buf, size, &rect[4], 1, colour ctx; 0x00427b46..0x00427bb7), 0x00552d70 `pop`. The rect is four caller-local
  floats (deref f4) and the buffer is read as a UTF-16 string (deref w), so the stack addresses never enter the log.
  DAT_0067d838 / DAT_0067d83c are sentinels (no recorder dereferences them). Seeded vectors point string-table entry
  2 (0x0066d828 + 8, base-relative per 0x00427784..0x0042778b) at a scratch string holding the control codes
  8..0xe that 0x004277a0 remaps (0x004277ba..0x0042780e); the rest use live table ids.

progress_bar_set_a (0x00430b90): fake RwGlobals at scratch+0 (+0x20 = recorder `rs`, `call [reg+0x20]`, 2 cdecl
  args). Recorders: 0x00472c60 `quad` (5), 0x0040bb50 `lookup` (1, the "Arrow" string at 0x005cd828), 0x004739f0
  `spr12` (12), 0x00473870 `spr7` (7), 0x00427e00 `icon` (6), 0x004282a0 `tsize` (2, returns an x87 float that the
  real ftol 0x004a2c48 converts into the right-arrow x offset, 0x00431222..0x00430ea1). REAL: 0x0042ac00 (token count
  of the dword stream in EDX), 0x0042ac50 (row base from EAX/ECX), 0x0042b8b0 / 0x0042b8c0 (ax = [0x67ea54] /
  [0x67ea56]; the caller relies on EDX surviving 0x0042b8b0 at 0x00430e99), 0x004a2c48. Seeded: DAT_0067e7f8 mode,
  DAT_0067e7fc alpha, DAT_0067e9f8 index, the stream pointers at 0x0067ed38 / 0x0067ed78 + index*64, the screen
  shorts, the fill floats 0x007f0f04 / 0x007f0f00 / 0x007f0f08 and the case-3 selector 0x007f0f10.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

_COMMON = {
    'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'lut_root_delta': 0,
    'fold_ret': False, 'observe': [], 'scratch_fill': 0xA5,
}


# ── 0x00427ad0 MenuMenusBB(id, x, y, w, h, argb, size) ──────────────────────────────────────────────────────────────
_BB_STR = 0x100
# length 12: 'A' 8 9 10 11 12 13 14 'z' 0x0f 0x81 0x7f 'Q'  -> 8..14 remapped, 0x0f / 0x81 / 0x7f passed through
_BB_CHARS = [0x41, 8, 9, 10, 11, 12, 13, 14, 0x7a, 0x0f, 0x81, 0x7f, 0x51]


def _wstr_seeds(off, chars):
    u16 = [len(chars)] + list(chars) + [0]
    if len(u16) % 2:
        u16.append(0)
    return [{'addr': {'scratch': off + 2 * i}, 'val': u16[i] | (u16[i + 1] << 16)} for i in range(0, len(u16), 2)]


def _bb(sid, x, y, w, h, argb, size, vw=640.0, vh=480.0, seeded=False):
    s = [{'addr': 0x0067d838, 'val': 0x0F017838}, {'addr': 0x0067d83c, 'val': 0x0C01783C},
         {'addr': 0x0067d830, 'val': _f(vw)}, {'addr': 0x0067d834, 'val': _f(vh)}]
    if seeded:
        s += _wstr_seeds(_BB_STR, _BB_CHARS)
        s.append({'addr': 0x0066d828 + 4 * sid, 'val': {'scratch': _BB_STR, 'minus': 0x0066d828}})
    return {'args': [sid, _f(x), _f(y), _f(w), _f(h), argb, _f(size)], 'seed': s}


_BB_TESTS = [
    _bb(2, 123.456, 77.7, 333.3, 41.9, 0xFF8040C0, 17.3, seeded=True),
    _bb(2, 0.1, 0.2, 0.3, 0.7, 0x11223344, 24.0, 800.0, 600.0, seeded=True),
    _bb(2, -15.25, 479.9, 1e-3, 3.3e4, 0x00000000, -2.5, 1680.0, 1050.0, seeded=True),
    _bb(1, 10.0, 20.0, 200.0, 30.0, 0xFF8040C0, 24.0),
    _bb(5, 0.0, 0.0, 640.0, 480.0, 0x80808080, 12.0, 1024.0, 768.0),
    _bb(20, 319.99, 239.01, 64.123, 64.987, 0xFFFFFFFF, 8.0, 1366.0, 768.0),
    _bb(40, 1.0 / 3.0, 2.0 / 3.0, 7.0 / 3.0, 11.0 / 7.0, 0x7F00FF00, 1.0 / 7.0, 1280.0, 720.0),
]

FIXES['menu_menus_bb'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x00427ad0, 'export': 'MenuMenusBB',
    'signature': {'ret': 'void', 'args': ['uint32'] * 7},
    'scratch_size': 0x200,
    'call_log': [{'name': 'push', 'at': '0x00552d10', 'nargs': 0},
                 {'name': 'color', 'at': '0x00556e90', 'nargs': 5, 'deref': {1: 1, 2: 1, 3: 1, 4: 1}},
                 {'name': 'text', 'at': '0x005555b0', 'nargs': 6, 'deref': {1: 'w', 3: 'f4'}, 'fargs': [2]},
                 {'name': 'pop', 'at': '0x00552d70', 'nargs': 0}],
    'path1_tests': _BB_TESTS, 'path2_tests': [_BB_TESTS[0], _BB_TESTS[3]],
})


# ── 0x00430b90 ProgressBarSetA() ────────────────────────────────────────────────────────────────────────────────────
_PB_STREAM_A, _PB_STREAM_B = 0x100, 0x400        # each stream has room for 0x7e tokens
_TOK, _END = 0xff060000, 0xff070000


def _stream(off, n):
    assert (2 * n + 1) * 4 <= 0x300, n
    words = []
    for k in range(n):
        words += [0x1000 + k, _TOK]          # filler dwords between tokens are skipped (0x0042ac20..0x0042ac34)
    words.append(_END)
    return [{'addr': {'scratch': off + 4 * i}, 'val': v} for i, v in enumerate(words)]


def _pb(mode, alpha, idx=0, count=4, w=640, h=480, fills=(0.5, 0.25, 0.005), sel=0, tsize=37.9):
    f04, f00, f08 = fills
    s = [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': {'scratch': 0x20}, 'val': {'call_log': 'rs'}},
         {'addr': 0x0067e7f8, 'val': mode & 0xffffffff}, {'addr': 0x0067e7fc, 'val': alpha & 0xffffffff},
         {'addr': 0x0067e9f8, 'val': idx},
         {'addr': 0x0067ed38 + 64 * idx, 'val': {'scratch': _PB_STREAM_A}},
         {'addr': 0x0067ed78 + 64 * idx, 'val': {'scratch': _PB_STREAM_B}},
         {'addr': 0x0067ea54, 'val': (w & 0xffff) | ((h & 0xffff) << 16)},
         {'addr': 0x007f0f04, 'val': f04 if isinstance(f04, int) else _f(f04)},
         {'addr': 0x007f0f00, 'val': f00 if isinstance(f00, int) else _f(f00)},
         {'addr': 0x007f0f08, 'val': f08 if isinstance(f08, int) else _f(f08)},
         {'addr': 0x007f0f10, 'val': sel & 0xffffffff}]
    # stream A (mode 1) and B (other modes) carry different counts, so a wrong table pick changes the row count.
    s += _stream(_PB_STREAM_A, count if mode == 1 else count + 1)
    s += _stream(_PB_STREAM_B, count if mode != 1 else count + 2)
    return {'args': [], 'seed': s,
            'stub_rets': {'lookup': [0x5A000001 + k for k in range(100)], 'tsize': tsize}}


_PB_TESTS = [
    _pb(0, 0x40),                                                      # early-out: states only
    _pb(0, 0x80),                                                      # 4 rows, stream B
    _pb(1, 0x20, count=6, w=800, h=600, fills=(0.01, 1.234567, 0.0100001), sel=1, tsize=12.5),
    _pb(2, 0x1A0, idx=3, count=1, w=1680, h=1050, fills=(7.77, 0.0, 0.0)),
    _pb(0, 0xff, count=0),                                             # no rows
    _pb(1, -127, count=4, w=0x7fff, h=-2, fills=(0x7fc00000, 0.0100000007, 1e30), sel=7, tsize=-3.75),
    _pb(0, 0x60, count=3, w=320, h=240, fills=(0.333, 0.667, 0.999), sel=2),
    _pb(5, 0x7f, idx=1, count=5, w=1024, h=768, fills=(0.123, 0.456, 0.789), sel=2, tsize=3e9),
    # 40 rows from base 0xf0 - 19*0x1e - 15 = -345 (0x0042ac50, even count): |(y - 8) * 0x7fff| passes 2^24 on the
    # outer rows, so the 0x00430ca9..0x00430ccb chain rounds its intermediate at PC_24 and not at PC_53.
    _pb(1, 0x90, count=40, w=640, h=0x7fff, fills=(0.75, 0.5, 0.25), sel=0, tsize=99.5),
]

FIXES['progress_bar_set_a'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x00430b90, 'export': 'ProgressBarSetA',
    'signature': {'ret': 'void', 'args': []},
    'scratch_size': 0x700,
    'call_log': [{'name': 'rs', 'nargs': 2},
                 {'name': 'quad', 'at': '0x00472c60', 'nargs': 5, 'fargs': [0, 1, 2, 3]},
                 {'name': 'lookup', 'at': '0x0040bb50', 'nargs': 1, 'deref': {0: 'a'}},
                 {'name': 'spr12', 'at': '0x004739f0', 'nargs': 12, 'fargs': [1, 2, 3, 4, 6, 9]},
                 {'name': 'spr7', 'at': '0x00473870', 'nargs': 7, 'fargs': [1, 2, 3, 4]},
                 {'name': 'icon', 'at': '0x00427e00', 'nargs': 6, 'fargs': [1, 2, 4]},
                 {'name': 'tsize', 'at': '0x004282a0', 'nargs': 2, 'fargs': [1], 'ret_type': 'float'}],
    'path1_tests': _PB_TESTS, 'path2_tests': [_PB_TESTS[0], _PB_TESTS[1]],
})
