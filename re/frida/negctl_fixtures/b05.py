"""negctl re-earn, batch b05 (RW stream / VFS / low-RVA render / frontend leaves).

Decomp cited per entry (re/tools/decomp_pc.py, 2026-10-11). Rows with no fixture here: 0x004026d0 boot_queue_flush
(only effects are RwCameraClear/BeginUpdate/EndUpdate/ShowRaster on the live camera; nothing lands in memory the
harness can read). See log/negctl_reearn/dec/b05.tsv.
"""
import struct

FIXES = {}


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def _str(off, s):
    b = s.encode('ascii') + b'\0'
    b += b'\0' * (-len(b) % 4)
    return [{'addr': {'scratch': off + i}, 'val': int.from_bytes(b[i:i + 4], 'little')} for i in range(0, len(b), 4)]


def _pat(off, n, k):
    return [{'addr': {'scratch': off + 4 * i}, 'val': ((k * 0x01010101) ^ (i * 0x11223344)) & 0xFFFFFFFF}
            for i in range(n)]


# ── 0x004cbe80 RwStreamWrite(stream, src, len) ───────────────────────────────────────────────────────────────────────
# switch (*stream): 1/2 file -> FUN_00550980; 3 memory: (stream[5] != 0, stream[4] - stream[3] >= len) copy len bytes to
# stream[5] + stream[3], stream[3] += len, return stream; 4 -> stream[5](stream[7], src, len), return stream or 0;
# default -> RwError(0xe), return 0. Old entry called it with zero args. Fixture: a type-3 memory stream in the
# scratch (header @0, src @0x40 patterned, dest @0x200 cap 0x200, never grown so RwRealloc is not reached), a type-4
# stream whose callback is FUN_00426090 (returns &DAT_0066ce58, nonzero, ignores args), and a bad type.
_S, _SRC, _DST = 0, 0x40, 0x200


def _mem(pos, n, k, typ=3, cap=0x200):
    seed = [{'addr': {'scratch': _S}, 'val': typ}, {'addr': {'scratch': _S + 0xc}, 'val': pos},
            {'addr': {'scratch': _S + 0x10}, 'val': cap}, {'addr': {'scratch': _S + 0x14}, 'val': {'scratch': _DST}}]
    return seed + _pat(_SRC, 0x60, k)


def _cb(n):
    return [{'addr': {'scratch': _S}, 'val': 4}, {'addr': {'scratch': _S + 0x14}, 'val': 0x00426090},
            {'addr': {'scratch': _S + 0x1c}, 'val': 0x1234}] + _pat(_SRC, 4, 9)


def _w(seed, n):
    return {'args': [{'scratch': _S}, {'scratch': _SRC}, n], 'seed': seed}


_RWS_TESTS = [_w(_mem(0, 0, 1), 0x10), _w(_mem(0x20, 0, 2), 5), _w(_mem(0, 0, 3), 0x101), _w(_mem(0x1f0, 0, 4), 0x10),
              _w(_mem(7, 0, 5), 0), _w(_cb(4), 4), _w(_mem(0, 0, 6, typ=9), 8)]
_RWS = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'uint32']},
    'fold_ret': True, 'scratch_size': 0x400, 'scratch_fill': 0xA5, 'freeze_main': True, 'target_global': None,
    'crash_equal_ok': None, 'observe': [{'addr': {'scratch': 0}, 'len': 0x400}],
    'path1_tests': _RWS_TESTS, 'path2_tests': _RWS_TESTS[:1],
}
FIXES['rw_stream_write_s2'] = dict(_RWS)

# ── 0x004cc6e0 RwStreamWriteChunked(stream, src, len) ────────────────────────────────────────────────────────────────
# while (len) { n = min(len, 0x100); copy n to a stack buffer; if (!FUN_004cbe80(stream, buf, n)) return 0;
# len -= n; src += n; } return stream. Old entry: zero args. Same memory-stream fixture; 0x150 exercises two chunks,
# the bad type the failure return.
FIXES['rw_stream_write_chunked'] = dict(_RWS, path1_tests=[
    _w(_mem(0, 0, 1), 0x150), _w(_mem(0x10, 0, 2), 0x100), _w(_mem(0, 0, 3), 7), _w(_mem(3, 0, 4), 0),
    _w(_mem(0, 0, 5, typ=9), 0x20), _w(_cb(4), 0x120)])

# ── 0x004cc770 RwStreamWriteBytes(stream, src, len) ──────────────────────────────────────────────────────────────────
# FUN_004cbe80(stream, src, len); return stream. Old entry: int_pair (two args) on stream 0x00400000 (the PE header,
# bad type), so the write never happened and the return is the arg. Memory stream: the write is visible in the scratch.
FIXES['rw_stream_write_bytes'] = dict(_RWS, path1_tests=[
    _w(_mem(0, 0, 1), 0x10), _w(_mem(0x30, 0, 2), 0x23), _w(_mem(0, 0, 3), 0x180), _w(_mem(0, 0, 5, typ=9), 4)])

# ── 0x004b3bb0 FileWriteWrapper_i3(name, buf, size) ─────────────────────────────────────────────────────────────────
# h = FUN_004cc230(2, 2, name); r = 0; if (h) { r = FUN_004cbe80(h, buf, size); FUN_004cc160(r, 0); } return r.
# Old entry: zero args, so name was stack garbage and the open failed. Write a scratch file under ..\log (game CWD is
# original\) and a path in a missing directory (open fails, returns 0); fold the return (stream pointer from the
# RW freelist, same address on both arms when the game thread is frozen).
def _fw(name, size):
    return {'args': [{'scratch': 0}, {'scratch': 0x80}, size], 'seed': _str(0, name) + _pat(0x80, 0x20, 7)}


FIXES['file_write_wrapper_i3'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'uint32']},
    'fold_ret': True, 'scratch_size': 0x200, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'observe': [{'addr': {'scratch': 0x80}, 'len': 0x80}],
    'path1_tests': [_fw('..\\log\\negctl_b05_fw.bin', 0x40), _fw('..\\log\\negctl_b05_fw.bin', 3),
                    _fw('..\\negctl_b05_no_such_dir\\x.bin', 0x10), _fw('..\\log\\negctl_b05_fw.bin', 0)],
    'path2_tests': [_fw('..\\log\\negctl_b05_fw.bin', 0x40)],
}

# ── 0x004c5820 RwTexDictionaryGetCurrent() = *(DAT_007d3ff8 + DAT_007d4054 + 0x1c) ──────────────────────────────────
# The live slot read the same value on every arm. Point DAT_007d3ff8 at the scratch for the call (game thread frozen)
# and vary both the plugin offset DAT_007d4054 and the slot value.
def _td(off, val):
    return {'args': [], 'seed': [{'addr': 0x007d3ff8, 'val': {'scratch': 0}}, {'addr': 0x007d4054, 'val': off},
                                 {'addr': {'scratch': off + 0x1c}, 'val': val}]}


FIXES['rw_tex_dictionary_get_current'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': []},
    'fold_ret': True, 'observe': [], 'scratch_size': 0x200, 'freeze_main': True, 'target_global': None,
    'path1_tests': [_td(0, 0x11223344), _td(0x40, 0xDEADBEEF), _td(0x100, 0), _td(0x8, 0x7fffffff), _td(0x1c0, 1)],
    'path2_tests': [_td(0x40, 0xDEADBEEF)],
}

# ── 0x00550b00 VfsFileExists(name) ──────────────────────────────────────────────────────────────────────────────────
# With DAT_007dc75c != 0: scan name for ':'; device-prefixed names select a device from the DAT_007dc754 list, else
# DAT_007dc76c; return device->vtable[0x13](device, name). Old vectors were two names that do not exist (both sides 0).
# Pass existing and missing files, and a device-prefixed name.
def _ex(name):
    return {'args': [{'scratch': 0}], 'seed': _str(0, name)}


FIXES['vfs_file_exists'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['pointer']},
    'fold_ret': True, 'observe': [], 'scratch_size': 0x100, 'freeze_main': True, 'target_global': None,
    'path1_tests': [_ex('videocfg.bin'), _ex('negctl_b05_missing.bin'), _ex('MASHED.exe'), _ex('TOASTART\\PC\\remap.lua'),
                    _ex('zz:videocfg.bin'), _ex('gamesave.bin')],
    'path2_tests': [_ex('videocfg.bin')],
}

# ── 0x00550bc0 VfsStreamGetType(p) = *(p + 8) ────────────────────────────────────────────────────────────────────────
# Old vectors passed NULL on every arm (identical crash). Pass a scratch record with distinct values at +8.
def _gt(v):
    return {'args': [{'scratch': 0}], 'seed': [{'addr': {'scratch': 8}, 'val': v}, {'addr': {'scratch': 4}, 'val': 0x55}]}


FIXES['vfs_stream_get_type'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['pointer']},
    'fold_ret': True, 'observe': [], 'scratch_size': 0x40, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_gt(1), _gt(2), _gt(0xDEADBEEF), _gt(0), _gt(0x7fffffff)],
    'path2_tests': [_gt(1)],
}

# ── 0x00403d30 / 0x00403ed0: 2 / 4 calls to FUN_00427ad0 (text draw) with constant args ─────────────────────────────
# FUN_00427ad0 -> FUN_00556e90(DAT_0067d83c, &colour x4) fills the colour record (+0..+0x64) and FUN_005555b0 draws
# through the dynamic vertex ring, whose next-write offsets live at 0x007d70cc.. (bucket_004d7ac0/0x004dc8a0.md).
# Old entries observed nothing (void). Point DAT_0067d83c at the scratch (it is fully written before it is read) and
# observe it plus the ring state 0x007d70c8..0x007d70eb; game thread frozen so the ring only moves for our draws.
_RING = {'addr': 0x007d70c8, 'len': 0x24}
_DRAW = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': []},
    'observe': [_RING, {'addr': {'scratch': 0}, 'len': 0x68}], 'scratch_size': 0x80, 'freeze_main': True,
    'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [{'args': [], 'seed': [{'addr': 0x0067d83c, 'val': {'scratch': 0}}]}] * 3,
    'path2_tests': [{'args': [], 'seed': [{'addr': 0x0067d83c, 'val': {'scratch': 0}}]}],
}
FIXES['render_00403d30'] = dict(_DRAW)
FIXES['render_00403ed0'] = dict(_DRAW)

# ── 0x00404320 PerModeRenderMachine(void) ────────────────────────────────────────────────────────────────────────────
# Switch on DAT_007f0fd0 (5/8/9/10); each arm: RwCameraEndUpdate, set view window, BeginUpdate, render states, then on
# DAT_0063ba8c in {3,4,5} FUN_00403ed0 / FUN_00403db0 / FUN_00403d30 / FUN_00403c60, restore. Old entry passed an
# arg and ran at menu mode (no-op). Seed the mode and sub-mode; mode 10 sub 6 runs the 0x00636b88 loop with every
# gate float 0 (no FUN_00403fa0 call). Observe the ring and the redirected colour record as for 0x00403d30.
def _pm(mode, sub):
    s = [{'addr': 0x007f0fd0, 'val': mode}, {'addr': 0x0063ba8c, 'val': sub},
         {'addr': 0x0067d83c, 'val': {'scratch': 0}}]
    s += [{'addr': 0x00636b88 + 0x14 * i + 0x10, 'val': 0} for i in range(6)]
    return {'args': [], 'seed': s}


FIXES['per_mode_render_machine'] = dict(_DRAW, path1_tests=[_pm(5, 3), _pm(8, 3), _pm(9, 3), _pm(10, 3), _pm(9, 0),
                                                            _pm(10, 6), _pm(0, 3), _pm(5, 5)],
                                        path2_tests=[_pm(5, 3)])

# ── 0x00407b00 RangeTableFindGroup(p) over the table FUN_00426090() = &DAT_0066ce58 ─────────────────────────────────
# -1/-10 separators (group = index after it, -2 after a separator ends), (lo, hi) ranges, (v, 0x100, w) value pairs.
# The live table put every test value 0..20 in group 0. Seed a crafted table (results 0/3/6/12 by a Python model of
# the decomp) with the game thread frozen.
_RT = [100, 105, -1, 110, 120, -10, 7, 0x100, 9, 50, 60, -1, 200, 0x100, 300, -1, -2]


def _rt(p):
    return {'args': [p & 0xFFFFFFFF], 'seed': [{'addr': 0x0066ce58 + 4 * i, 'val': v & 0xFFFFFFFF} for i, v in enumerate(_RT)]}


FIXES['range_table_find_group'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'int', 'args': ['int32']},
    'fold_ret': True, 'observe': [], 'freeze_main': True, 'target_global': None,
    'path1_tests': [_rt(p) for p in (0, 100, 103, 106, 110, 115, 7, 9, 50, 55, 61, 200, 300, -5)],
    'path2_tests': [_rt(110)],
}

# ── 0x00408a50 PerCarRaceProgressGet(i) = *(float *)(0x008a96e8 + i * 0x30c) (ST0) ───────────────────────────────────
# The menu values were 0.0 for every car. Seed distinct floats.
_PCP = [1.5, -2.25, 1234.5, 0.001]


def _pc(i):
    return {'args': [i], 'seed': [{'addr': 0x008a96e8 + 0x30c * j, 'val': _f(v)} for j, v in enumerate(_PCP)]}


FIXES['per_car_race_progress_get'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'float', 'args': ['int32']},
    'fold_ret': True, 'observe': [], 'freeze_main': True, 'target_global': None,
    'path1_tests': [_pc(0), _pc(1), _pc(2), _pc(3)], 'path2_tests': [_pc(1)],
}

# ── 0x00408a70 FrontendC2RoundI(i, float f) ──────────────────────────────────────────────────────────────────────────
# if (i > 3) return 0; printf("put precisepos %d,%f\n"); n = FUN_004a2c48() (ST0 = f); per-car record at
# 0x008a9640 + i*0x30c: +0, +4, +0xb0, +0xb4 = n; +0xa8 = f; return 1. Old vectors were all i > 3 (early out).
# Observe the four records' written spans (pre-filled 0xA5), fold the return.
_OBS_RI = []
for _j in range(4):
    _b = 0x008a9640 + 0x30c * _j
    _OBS_RI += [{'addr': _b, 'len': 8, 'fill': 0xA5}, {'addr': _b + 0xa8, 'len': 4, 'fill': 0xA5},
                {'addr': _b + 0xb0, 'len': 8, 'fill': 0xA5}]
# 0x007739f8 (not filled): CRT state the printf changes (field_first flipped it on run 1).
_OBS_RI.append({'addr': 0x007739f8, 'len': 4})
FIXES['frontend_c2_round_i'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'uint32', 'args': ['int32', 'float']},
    'fold_ret': True, 'observe': _OBS_RI, 'pre_fill_byte': None, 'freeze_main': True, 'target_global': None,
    'path1_tests': [{'args': [0, 1.5]}, {'args': [1, 2.5]}, {'args': [2, -3.75]}, {'args': [3, 100.7]},
                    {'args': [0, 0.0]}, {'args': [3, -0.5]}, {'args': [4, 9.0]}, {'args': [1, 1e6]}],
    'path2_tests': [{'args': [1, 2.5]}],
}

# ── 0x0040b460 MenuMenusBD(int *out4) ────────────────────────────────────────────────────────────────────────────────
# key[i] = DAT_008a94f0[i] (or -1 when *(int *)(0x007f1a14 + 0x10*i) < 0); out[i] = i; bubble sort out by key desc;
# DAT_007f0fd0 4/7: out[i] = FUN_00417740(i); 9: out[0..1] = DAT_007f0fcc ? (0,1) : (1,0). Old vectors passed NULL.
# Pass a scratch out buffer (pre-filled 0xA5), seed keys, the sign words, mode and DAT_007f0fcc.
def _bd(keys, neg, mode, fcc):
    s = [{'addr': 0x008a94f0 + 4 * i, 'val': k & 0xFFFFFFFF} for i, k in enumerate(keys)]
    s += [{'addr': 0x007f1a14 + 0x10 * i, 'val': (-1 if i in neg else i) & 0xFFFFFFFF} for i in range(4)]
    s += [{'addr': 0x007f0fd0, 'val': mode}, {'addr': 0x007f0fcc, 'val': fcc}]
    return {'args': [{'scratch': 0}], 'seed': s}


FIXES['menu_menus_bd'] = {
    'arg_type': 'scalars_to_scattered_globals', 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': {'scratch': 0}, 'len': 0x10}], 'scratch_size': 0x20, 'scratch_fill': 0xA5,
    'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_bd([3, 9, 1, 5], (), 0, 0), _bd([10, 20, 30, 40], (1,), 0, 0), _bd([7, 7, 7, 7], (), 0, 0),
                    _bd([-5, 2, 8, 0], (0, 3), 0, 0), _bd([1, 2, 3, 4], (), 9, 0), _bd([1, 2, 3, 4], (), 9, 1),
                    _bd([4, 3, 2, 1], (2,), 1, 0)],
    'path2_tests': [_bd([3, 9, 1, 5], (), 0, 0)],
}
