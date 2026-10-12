r"""negctl fix U-9663 (2026-10-12): 0x004971b0 ControllerConfigLoad_j5 (key controller_config_load_j5). Loads last.

The registry entry (eax_implicit_int, slots 0..9) always read 0 on both arms: 0x00497190 formats the RELATIVE name
`contcfg%d.bin` (format at 0x005d000c, buffer 0x007730e4, 0x00497191..0x004971a3), no such file exists in the game CWD
(original\), so `call 0x4a4541` (0x004971cd) returns NULL and both arms take the "Unable to load" exit at 0x0049720f.
The read path (`call 0x4a49cf` _fread at 0x004971ed, `call 0x4a4368` _fclose at 0x004971f3), which the bootio re-port
switched to the game CRT, never ran.

This fixture never touches original\. The formatter 0x00497190 is replaced on BOTH arms (stub_at, 0 stack args: the
target reaches it with `mov esi,eax; call 0x497190` at 0x004971b3/0x004971b5, slot in EAX) by a callback that returns
scratch + EAX*0x80 (`ret_scratch`, diff_template.js, added for this fix). Each test seeds four ABSOLUTE paths into the
scratch, one per slot, all under %TEMP%\\mashed_u9663\\ (written by this module at import). So a port that passes the
wrong EAX to the formatter opens a different file. Everything after the formatter is real game code on both arms:
0x004a4541 (on the patched reference the fix_fopen JMP into the cave 0x0050f6c4, which the port also calls by VA),
the game _fread into DAT_007e95c0 + slot*0x200 (`shl esi,9; add esi,0x7e95c0` at 0x004971e1/0x004971e6, count 0x200
at 0x004971dc, size 1 at 0x004971e4), and the game _fclose. Observed: the whole 4-slot table 0x007e95c0..0x007e9dc0
(pre-filled 0xA5 per arm, restored after) folded with the return (1 at 0x00497208, 0 at 0x0049721f).

The target has two branches only (FILE* NULL / non-NULL, `test ebx,ebx; je` at 0x004971d7) and does not check the
_fread count, so a short or empty file still returns 1 with a partial copy; the record checks (+0x000 == 6, device
name strcmp) live in the caller FUN_00498510 (0x00498694 onward, re/analysis/structs/contcfg_record.md), not here.
Vectors: valid 0x200-byte records per slot (layout of contcfg_record.md: +0x000 = 6, +0x004 name, +0x108 13 binding
dwords, +0x13c type, +0x140 joypad index, tail), a 0x2c0-byte file (only 0x200 read), a 0x51-byte file, an empty file,
an absent file, and cross-slot mixes. Slots stay 0..3: the table is 0x800 bytes (FUN_00498510 zero-fills 0x800 at
0x007e95c0), a larger slot would write past it.
"""
import os
import struct

FIXES = {}

_DIR = os.path.join(os.environ.get('TEMP') or os.environ.get('TMP') or '.', 'mashed_u9663')
_STRIDE = 0x80          # scratch bytes per slot path
_SCRATCH = 0x200


def _record(k):
    """0x200-byte contcfg record for slot k (contcfg_record.md layout), distinct per slot."""
    kb = [0x1f, 0x2d, 0x1e, 0x20, 0x1c, 0x01, 0x01, 0x1e, 0x2e, 0xcb, 0xcd, 0xc8, 0xd0]   # FUN_00498510 keyboard defaults
    jp = [0, 1, 2, 3, 0, 1, 4, 2, 5, 0, 0, 0, 0]                                          # joypad defaults, 9..12 zero
    dtype = [1, 1, 2, 0][k]
    b = bytearray(0x200)
    struct.pack_into('<I', b, 0x000, 6)
    name = (b'Keyboard' if dtype == 2 else b'u9663 pad %d' % k)
    b[0x004:0x004 + len(name)] = name
    binds = kb if dtype == 2 else [(v + k) & 0xff for v in jp]
    struct.pack_into('<13I', b, 0x108, *binds)
    struct.pack_into('<II', b, 0x13c, dtype, k)
    for i in range(0x144, 0x200):
        b[i] = (k * 0x35 + i * 7) & 0xff
    return bytes(b)


_FILES = {
    'valid0': _record(0), 'valid1': _record(1), 'valid2': _record(2), 'valid3': _record(3),
    'long': _record(2) + bytes((0x5a ^ i) & 0xff for i in range(0xc0)),      # 0x2c0 bytes, 0x200 read
    'short': _record(1)[:0x51],
    'empty': b'',
}


def _path(name):
    return os.path.join(_DIR, 'cc_%s.bin' % name)


def _materialise():
    os.makedirs(_DIR, exist_ok=True)
    for n, data in _FILES.items():
        p = _path(n)
        try:
            with open(p, 'rb') as f:
                if f.read() == data:
                    continue
        except OSError:
            pass
        with open(p, 'wb') as f:
            f.write(data)
    absent = _path('absent')
    if os.path.exists(absent):
        os.remove(absent)


_materialise()


def _seed_paths(names):
    """Seed four NUL-terminated absolute paths at scratch + k*0x80 as dwords."""
    seed = []
    for k, n in enumerate(names):
        s = _path(n).encode('mbcs') + b'\0'
        assert len(s) <= _STRIDE, s
        s += b'\0' * (-len(s) % 4)
        for o in range(0, len(s), 4):
            seed.append({'addr': {'scratch': k * _STRIDE + o}, 'val': struct.unpack_from('<I', s, o)[0]})
    return seed


def _t(slot, names):
    return {'args': [slot], 'seed': _seed_paths(names)}


_V = ['valid0', 'valid1', 'valid2', 'valid3']
_TESTS = [
    _t(0, _V), _t(1, _V), _t(2, _V), _t(3, _V),                              # present and valid, every slot
    _t(2, ['long'] * 4),                                                      # longer than 0x200
    _t(1, ['short'] * 4),                                                     # 0x51 bytes: partial copy, ret 1
    _t(3, ['empty'] * 4),                                                     # 0 bytes: no copy, ret 1
    _t(0, ['absent'] * 4), _t(3, ['absent'] * 4),                             # NULL FILE*: ret 0, table untouched
    _t(1, ['absent', 'valid3', 'absent', 'absent']),                          # only the slot's own path is opened
    _t(2, ['valid0', 'valid1', 'absent', 'valid3']),                          # own path absent, neighbours valid
    _t(3, ['valid0', 'short', 'valid2', 'short']),
    _t(0, ['empty', 'valid1', 'valid2', 'valid3']),
    _t(2, ['valid3', 'valid3', 'valid0', 'valid3']),                          # slot 2 table row gets slot 0's record
]

FIXES['controller_config_load_j5'] = {
    '__replace__': True,
    'rva': 0x004971b0, 'export': 'ControllerConfigLoad_j5',
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['uint32']},
    'eax_arg': 0, 'fold_ret': True, 'freeze_main': True,
    'scratch_size': _SCRATCH,
    'stub_at': [{'addr': 0x00497190, 'nargs': 0, 'abi': 'mscdecl',
                 'ret_scratch': {'off': 0, 'eax_stride': _STRIDE, 'eax_max': 3}}],
    'observe': [{'addr': 0x007e95c0, 'len': 0x800, 'fill': 0xA5}],
    'lut_root_delta': 0,
    'path1_tests': _TESTS, 'path2_tests': _TESTS[:3],
}
