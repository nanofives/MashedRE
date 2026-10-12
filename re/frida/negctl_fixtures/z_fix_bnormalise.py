"""bnormalise (2026-10-12, triage B item 9): fixtures for the three B rows whose outputs did not reproduce between two
original calls: 0x004b3bb0 FileWriteWrapper_i3, 0x004921d0 DisplayInit, 0x00428610 ViewportScaledRectDraw. Loads
last; replaces the three entries whole.

In all three the non-reproducible value is produced by a CALLEE, not by the body under test: a fresh RW stream
(0x004cc230), a fresh TXD and texture (0x004b3eb0, 0x004c5c00), re-created live cameras (0x00467110, 0x0042d560,
0x0042f660), and the RW / D3D render state behind the quad draw (0x00450b10, which writes 0x007d709c through the
device). So the normalisation is done at the source: each such callee is a named `call_log` recorder returning a
seeded value (`stub_rets`), installed only while the game's first thread is frozen. Both arms then see identical
callee results, the call sequence with its arguments is the fingerprint, and nothing live is re-created. Callees
that only read or write plain globals stay REAL and their globals are seeded and observed.

New recorder keys (diff_template.js, this branch): deref 'l' (log 'stk' for a pointer to the caller's own locals,
whose address differs per arm because the port's frame differs) and outs / stub_outs (write a dword through an
out-parameter, so values a callee hands back through a pointer are seeded and distinct).
Byte citations: `py -3.12 re/tools/disasm_fn.py` on original\\MASHED.exe.unpatched.

file_write_wrapper_i3 (0x004b3bb0): 0x004b3bb4..0x004b3bb9 open(2, 2, name) at 0x004cc230; `je 0x4b3be6` on 0 (EAX 0
  returned); 0x004b3bcd push esi is a callee-save (popped at 0x004b3be5), so 0x004cbe80 gets 3 args (stream, buf,
  size); 0x004b3bd8..0x004b3bdb close(write_result, 0) at 0x004cc160; returns the write result (0x004b3be3).
  `add esp,0x14` at 0x004b3be0 = 3 + 2 args.

display_init (0x004921d0): 14 dwords pushed, cleaned by `add esp,0x38` at 0x00492247: vminfo(&local, (int)(short)
  vmode()) 0x004921d5..0x004921e3 (movsx at 0x004921da); w = 0x00498bc0(), h = 0x00498bd0(); 0x00467110(w, h);
  0x0042b890(w); 0x0042b8a0(h) (REAL: store the low word to 0x0067ea54 / 0x0067ea56, 0x0042b895 / 0x0042b8a5);
  0x0042d560(); 0x0042f660(); 0x004997b0(0x194, "RWTEXDICTIONARY" @0x005cf6a4, &L4, &L0) with L0 = L4 = 0
  (0x00492229 / 0x0049222d); 0x004b3eb0(L4, L0, 0, 0); 0x0077195c = result (0x0049224d); if nonzero
  0x00771960 = 0x004c5c00(result, "LoadIcon" @0x005cf698) (0x00492263).

viewport_scaled_rect_draw (0x00428610): 0x0042b8b0 / 0x0042b8c0 REAL (`mov ax,[0x67ea54]` / `[0x67ea56]`), seeded;
  0x00450b10 is the recorder `quad` (tex, x, y, w, h, argb, &uv[4]); the tex handle reaches only the render-state
  slot inside 0x00450b10 (0x00450c15..0x00450c23), never the 0x00898a20 vertex block the old entry observed.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

_COMMON = {
    'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'lut_root_delta': 0,
    'observe': [],
}


def _astr(off, s):
    b = s.encode('ascii') + b'\0'
    b += b'\0' * (-len(b) % 4)
    return [{'addr': {'scratch': off + i}, 'val': struct.unpack('<I', b[i:i + 4])[0]} for i in range(0, len(b), 4)]


# ── 0x004b3bb0 FileWriteWrapper_i3(name, buf, size) ─────────────────────────────────────────────────────────────────
def _fw(name, size, open_r, write_r):
    return {'args': [{'scratch': 0}, {'scratch': 0x100}, size], 'seed': _astr(0, name),
            'stub_rets': {'open': open_r, 'write': write_r}}


_FW_TESTS = [
    _fw('..\\log\\never_opened.bin', 0x40, 0, 0x11111111),             # open fails: no write, no close, ret 0
    _fw('a.bin', 0x40, 0x5A0C0001, 0x5A0C0001),                        # RwStreamWrite returns the stream
    _fw('b.bin', 3, 0x5A0C0002, 0),                                    # write fails: close(0, 0), ret 0
    _fw('c\\d.bin', 0, 0xFFFFFFFF, 0x12345678),                         # write result differs from the stream
    _fw('', 0x7FFFFFFF, 1, 0xFFFFFFFF),
    _fw('gamesave.bin', 0x80000000, 0x7FFFFFFF, 0x80000000),
    _fw('x', 0x24FA0, 0x0040_0000, 0x5A0C0003),
]

FIXES['file_write_wrapper_i3'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x004b3bb0, 'export': 'FileWriteWrapper_i3',
    'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'uint32']}, 'fold_ret': True,
    # 0x600: NegCtl's field classes scan a 0x400-byte window at each pointer arg (Harness/NegCtl.cpp (a)); with 0x200
    # the buf window (scratch+0x100) ran into Frida's heap, which the recorders' JS allocations write mid-call, and
    # field_first flipped such a byte (BLIND 0/7, flips=2, changed=0 on the final vector). Every window now lies in the
    # scratch, which only the harness fill writes.
    'scratch_size': 0x600,
    'call_log': [{'name': 'open', 'at': '0x004cc230', 'nargs': 3, 'deref': {2: 'a'}},
                 {'name': 'write', 'at': '0x004cbe80', 'nargs': 3},
                 {'name': 'close', 'at': '0x004cc160', 'nargs': 2}],
    'path1_tests': _FW_TESTS, 'path2_tests': [_FW_TESTS[1], _FW_TESTS[3]],
})


# ── 0x004921d0 DisplayInit() ────────────────────────────────────────────────────────────────────────────────────────
def _di(vmode, w, h, res_ptr, res_len, txd, find):
    return {'args': [], 'seed': [{'addr': 0x0077195c, 'val': 0x5A5A0001}, {'addr': 0x00771960, 'val': 0x5A5A0002},
                                 {'addr': 0x0067ea54, 'val': 0x5A035A04}],
            'stub_rets': {'vmode': vmode, 'rw': w, 'rh': h, 'txd': txd, 'find': find},
            'stub_outs': {'res': {2: res_ptr, 3: res_len}}}


_DI_TESTS = [
    _di(3, 640, 480, 0x00A01000, 0x1F40, 0x0BAD0001, 0x0BAD0002),
    _di(0x12348003, 1680, 1050, 0x00A02000, 0x77, 0x0BAD0011, 0),    # short mode sign-extends to 0xffff8003
    _di(0, 800, 600, 0, 0, 0, 0x0BAD0022),                            # TXD load fails: 0x0077195c = 0, 960 kept
    _di(0x7FFF, 0x12345678, 0x9ABCDEF0, 0xFFFFFFFF, 0x80000000, 0x0BAD0031, 0x0BAD0032),
    _di(-1, 0x8000, 0xFFFF, 0x00A03000, 1, 1, 0xFFFFFFFF),
    _di(0x10000, 1, 2, 0x00A04000, 0x00A04000, 0x80000000, 0x0BAD0052),
]

FIXES['display_init'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x004921d0, 'export': 'DisplayInit',
    'signature': {'ret': 'void', 'args': []}, 'fold_ret': False, 'scratch_size': 0,
    'observe': [{'addr': 0x0077195c, 'len': 8}, {'addr': 0x0067ea54, 'len': 4}],
    'call_log': [{'name': 'vmode', 'at': '0x004c2f00', 'nargs': 0},
                 {'name': 'vminfo', 'at': '0x004c2ed0', 'nargs': 2, 'deref': {0: 'l'}},
                 {'name': 'rw', 'at': '0x00498bc0', 'nargs': 0},
                 {'name': 'rh', 'at': '0x00498bd0', 'nargs': 0},
                 {'name': 'cam', 'at': '0x00467110', 'nargs': 2},
                 {'name': 'mcam', 'at': '0x0042d560', 'nargs': 0},
                 {'name': 'vcam', 'at': '0x0042f660', 'nargs': 0},
                 {'name': 'res', 'at': '0x004997b0', 'nargs': 4, 'deref': {1: 'a', 2: 'l', 3: 'l'}},
                 {'name': 'txd', 'at': '0x004b3eb0', 'nargs': 4},
                 {'name': 'find', 'at': '0x004c5c00', 'nargs': 2, 'deref': {1: 'a'}}],
    'path1_tests': _DI_TESTS, 'path2_tests': [_DI_TESTS[0], _DI_TESTS[2]],
})


# ── 0x00428610 ViewportScaledRectDraw(tex*, x, y, w, h, coordMode, blendMode) ───────────────────────────────────────
def _vs(tex, x, y, w, h, cm, bm, sw=640, sh=480):
    s = [{'addr': 0x0067ea54, 'val': (sw & 0xffff) | ((sh & 0xffff) << 16)}]
    a0 = 0
    if tex is not None:
        s.append({'addr': {'scratch': 0}, 'val': tex})
        a0 = {'scratch': 0}
    fx = [v if isinstance(v, int) else _f(v) for v in (x, y, w, h)]
    return {'args': [a0] + fx + [cm & 0xffffffff, bm & 0xffffffff], 'seed': s}


_VS_TESTS = [
    _vs(0x7E000123, 0.1, 0.2, 0.3, 0.4, 0, 0),
    _vs(None, 123.456, 77.7, 333.3, 41.9, 1, 1, 800, 600),
    _vs(0x7E000124, 1.0 / 3.0, 2.0 / 3.0, 7.0 / 3.0, 11.0 / 7.0, 2, 2, 1680, 1050),
    _vs(0xFFFFFFFF, 319.99, 239.01, 64.123, 64.987, 2, 3, 1366, 768),
    _vs(0x7E000125, 0.7, 0.9, 0.11, 0.13, 3, 0),                       # coordMode 3: x = y = 0
    _vs(None, 5.5, 6.5, 7.5, 8.5, -1, 4, 1024, 768),
    _vs(0, -15.25, 479.9, 1e-3, 3.3e4, 0x80000000, -1, 0x8000, -2),
    _vs(0x7E000126, 0x7FC00000, 0x80000000, 1e30, -1e-30, 2, 1, 1024, 768),
    _vs(0x7E000127, 0.123456, 0.654321, 0.333333, 0.777777, 7, 2, 1023, 767),
    _vs(0x7E000128, 0.1, 0.1, 0.3, 0.3, 2, 0, 1023, 767),
    _vs(0x00000001, 700.5, -12.125, 0.0625, 3.0, 1, 3, 320, 240),
    _vs(0x7E000129, 999.999, 0.000123, 12345.678, 0.5, 2, 2, 0x7FFF, 0x7FFF),
]

FIXES['viewport_scaled_rect_draw'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x00428610, 'export': 'ViewportScaledRectDraw',
    'signature': {'ret': 'void', 'args': ['uint32'] * 7}, 'fold_ret': False,
    'scratch_size': 0x400, 'scratch_fill': 0xA5,   # the tex window (NegCtl, 0x400 bytes at arg 0) stays in the scratch
    'call_log': [{'name': 'quad', 'at': '0x00450b10', 'nargs': 7, 'fargs': [1, 2, 3, 4], 'deref': {6: 'f4'}}],
    'path1_tests': _VS_TESTS, 'path2_tests': [_VS_TESTS[0], _VS_TESTS[2]],
})
