"""negctl re-earn, batch b14 (RWS audio sub-struct free, bitmap pool, DSound wrappers, 0x005c6b60 delta).

Decomp from re/tools/decomp_pc.py (pool slot, read-only), disasm from re/tools/disasm_va.py on MASHED.exe.unpatched.
Rows demoted on a static port defect (0x005baf90, 0x005bc400) have no fixture: see log/negctl_reearn/dec/b14.tsv.
0x005ae650 / 0x005ae800 / 0x005ae920 get a fixture only to measure the defect the static read found.

Shared layout (scratch, filled 0xEE):
  0x000 fake RwGlobals (seeded into DAT_007d3ff8 only where a free/alloc runs): +0x10c (RwFree, reached by the
        `jmp [ecx+0x10c]` at 0x004522d6) = P, +0x108 (RwMalloc, `jmp [edx+0x108]` at 0x005aea06) = G
  0x200 sub-struct S      0x220 pool header H      0x260 block B1      0x2c0 block B2
  0x300 pool header H2    0x380 fake list node (0x005ae650 global list)
Recorders: P 0x004099e0 `mov eax,[esp+4]; mov [0x008a95a0],eax; ret` (observed, fill 0xA5);
           G 0x00402f40 `mov eax,[0x00636ad8]; ret` (returns the seeded dword).
"""
import struct

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'
P, G = 0x004099e0, 0x00402f40
S, H, B1, B2, H2, NODE = 0x200, 0x220, 0x260, 0x2c0, 0x300, 0x380
POOL_A = 0x007dda28          # FUN_005ae080 passes &DAT_007dda28 (0x005ae080 decomp line `FUN_005ae920(&DAT_007dda28, ...)`)
POOL_A_SENT = POOL_A + 0x10  # header +0x10 is the circular block-list sentinel


def _sc(off):
    return {'scratch': off}


def _w(addr, val, size=4):
    return {'addr': addr, 'val': val, 'size': size}


def _rwg():
    return [_w(0x007d3ff8, _sc(0)), _w(_sc(0x10c), P), _w(_sc(0x108), G)]


def _hdr(base, elem, cap, nbytes, align, head, tail, flags):
    a = (lambda o: _sc(base + o)) if isinstance(base, int) and base < 0x1000 else (lambda o: base + o)
    return [_w(a(0), elem), _w(a(4), cap), _w(a(8), nbytes), _w(a(0xc), align), _w(a(0x10), head),
            _w(a(0x14), tail), _w(a(0x18), flags)]


def _blk(off, nxt, prv, bitmap_dword):
    return [_w(_sc(off), nxt), _w(_sc(off + 4), prv), _w(_sc(off + 8), bitmap_dword)]


_SCR = {'scratch_size': 0x400, 'scratch_fill': 0xEE, 'freeze_main': True, 'target_global': None,
        'crash_equal_ok': None, 'pre_fill_byte': None}
_OBS_SCR = [{'addr': _sc(0x200), 'len': 0x200}]
_OBS_P = [{'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5}]


# ── 0x005ae080 AudioSubStructAFree(int* s) ─────────────────────────────────────────────────────────────────────────
# if (s[0] != 0 && (s[2] & 1)) { FUN_005ae920(&DAT_007dda28, s[0]); s[2] &= ~1; } return s.
# Old: registered with zero args (`none`): the body ran on whatever stack word sat at [esp+4]. Now s is a scratch
# struct and the pool at DAT_007dda28 is replaced (seeded, restored per arm) by a 16-slot pool whose single block B1
# lives in scratch with every bit set: the callee clears the bit of the item it was handed, so the forwarded
# argument and the pool address are both visible. Pool flags 0 (no compaction, no RwFree).
def _poolA(bitmap=0xFFFF):
    return _hdr(POOL_A, 0x10, 16, 2, 4, _sc(B1), _sc(B1), 0) + _blk(B1, POOL_A_SENT, POOL_A_SENT, bitmap)


def _itemA(k):
    return _sc(B1 + 8 + 2 + 0x10 * k)   # FUN_005ae920 range base = block + bitmap_bytes + 8 (0x005ae920 decomp)


def _af(item, flags, bitmap=0xFFFF):
    return {'args': [_sc(S)], 'seed': _poolA(bitmap) + [_w(_sc(S), item), _w(_sc(S + 4), 0x0BADF00D),
                                                         _w(_sc(S + 8), flags)]}


_OBS_A = _OBS_SCR + [{'addr': POOL_A, 'len': 0x1c}]
FIXES['audio_sub_struct_a_free'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': _OBS_A,
    'path1_tests': [_af(_itemA(0), 1), _af(_itemA(5), 3), _af(_itemA(15), 0xFFFFFFFF), _af(_itemA(9), 2),
                    _af(0, 1), _af(0x12345678, 0x101), _af(_itemA(3), 0x81, 0x00F0)],
    'path2_tests': [_af(_itemA(5), 3)],
})


# ── 0x005ae050 AudioSubStructBFree(int s) ──────────────────────────────────────────────────────────────────────────
# if (*(s+4) != 0 && (*(byte*)(s+8) & 2)) { FUN_004522d0(*(s+4)); *(s+8) &= ~2; } return s.
# FUN_004522d0 = `mov ecx,[0x007d3ff8]; jmp [ecx+0x10c]`: the fake RwGlobals sends the free to P, which records the
# pointer it was handed (0x008a95a0). Old: zero-arg registration, nothing observed but EAX.
def _bf(ptr, flags):
    return {'args': [_sc(S)], 'seed': _rwg() + [_w(_sc(S), 0x0BADF00D), _w(_sc(S + 4), ptr), _w(_sc(S + 8), flags)]}


FIXES['audio_sub_struct_b_free'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': _OBS_SCR + _OBS_P,
    'path1_tests': [_bf(0x11223344, 2), _bf(0x55667788, 0xFFFFFFFF), _bf(0, 2), _bf(0x1234, 1), _bf(_sc(B1), 6),
                    _bf(0x7f00ff00, 0x102)],
    'path2_tests': [_bf(0x11223344, 2)],
})


# ── 0x005ae030 AudioSubStructCleanup(s) = FUN_005ae080(s); FUN_005ae050(s); return s ──────────────────────────────
def _cl(item, ptr, flags, bitmap=0xFFFF):
    return {'args': [_sc(S)], 'seed': _rwg() + _poolA(bitmap) + [_w(_sc(S), item), _w(_sc(S + 4), ptr),
                                                                 _w(_sc(S + 8), flags)]}


FIXES['audio_sub_struct_cleanup'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': _OBS_A + _OBS_P,
    'path1_tests': [_cl(_itemA(2), 0x11223344, 3), _cl(_itemA(7), 0x55667788, 1), _cl(_itemA(11), 0x66778899, 2),
                    _cl(0, 0x22334455, 0xFFFFFFFF), _cl(_itemA(4), 0, 0xFFFFFFFF), _cl(_itemA(1), 0x3344, 0x100)],
    'path2_tests': [_cl(_itemA(2), 0x11223344, 3)],
})


# ── 0x005ae780 AudioPoolDestroy(int h) ─────────────────────────────────────────────────────────────────────────────
# Drain list at h+0x10: unlink each block, FUN_004522d0(block). Then if !(byte(h+0x18) & 1): DAT_007ddab0 != h &&
# DAT_007ddab0 != 0 ? FUN_005ae920(DAT_007ddab0, h) : FUN_004522d0(h). Old: int_scalar 0 (crash-equal).
# P records the LAST pointer freed; unlinks are visible in scratch; the PoolFree path clears H's bit in H2's block B3.
# B3 sits so that H is its slot 0: base = B3 + 1 (bitmap) + 8 = H.
B3 = H - 9


def _two_blocks():
    return _blk(B1, _sc(B2), _sc(H + 0x10), 0x11) + _blk(B2, _sc(H + 0x10), _sc(B1), 0x22)


def _pd(blocks, flags, ddab0):
    s = _rwg()
    if blocks == 2:
        s += _hdr(H, 0x10, 8, 1, 4, _sc(B1), _sc(B2), flags) + _two_blocks()
    elif blocks == 1:
        s += _hdr(H, 0x10, 8, 1, 4, _sc(B1), _sc(B1), flags) + _blk(B1, _sc(H + 0x10), _sc(H + 0x10), 0x33)
    else:
        s += _hdr(H, 0x10, 8, 1, 4, _sc(H + 0x10), _sc(H + 0x10), flags)
    if ddab0 == 'H2':
        s += _hdr(H2, 0x24, 4, 1, 4, _sc(B3), _sc(B3), 0)
        s += [_w(_sc(B3), _sc(H2 + 0x10)), _w(_sc(B3 + 4), _sc(H2 + 0x10)), _w(_sc(B3 + 8), 0xC0, 1)]
        s += [_w(0x007ddab0, _sc(H2))]
    elif ddab0 == 'H':
        s += [_w(0x007ddab0, _sc(H))]
    else:
        s += [_w(0x007ddab0, 0)]
    return {'args': [_sc(H)], 'seed': s}


FIXES['audio_pool_destroy'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': _OBS_SCR + _OBS_P + [{'addr': 0x007ddab0, 'len': 4}],
    'path1_tests': [_pd(2, 1, 0), _pd(2, 3, 'H2'), _pd(0, 2, 0), _pd(1, 0, 'H2'), _pd(2, 0, 'H'), _pd(0, 0, 'H2'),
                    _pd(1, 0xFFFFFFFE, 0)],
    'path2_tests': [_pd(1, 0, 'H2')],
})


# ── 0x005ae920 AudioPoolFree(h, item) — defect measurement ─────────────────────────────────────────────────────────
# Original range test (0x005ae920 decomp): skip while item < base || base + cap*elem < item  -> end is INCLUSIVE;
# after unlinking, compaction (flags & 2) is considered only when the list is non-empty (`puVar3 != puVar1`).
# Port: exclusive end; compaction checked even when the block was the only one (frees it).
def _pf(k, bitmap, flags):
    s = _rwg() + _hdr(H, 0x10, 3, 1, 4, _sc(B1), _sc(B1), flags) + _blk(B1, _sc(H + 0x10), _sc(H + 0x10), bitmap)
    return {'args': [_sc(H), _sc(B1 + 9 + 0x10 * k)], 'seed': s}


FIXES['audio_pool_free'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer', 'pointer']},
    'observe': _OBS_SCR + _OBS_P,
    'path1_tests': [_pf(1, 0xE0, 0), _pf(0, 0xE0, 0), _pf(3, 0xF0, 0), _pf(0, 0x80, 2), _pf(1, 0xE0, 2)],
    'path2_tests': [_pf(1, 0xE0, 0)],
})


# ── 0x005ae800 AudioPoolBlockAlloc(h, flags) — defect measurement ──────────────────────────────────────────────────
# Original scan counts down from h[1] (capacity) per bit and stops at 0, so with cap 3 and bitmap 0xE0 no slot is free
# and a new block is allocated (FUN_005aea00 -> RwGlobals+0x108 = G -> seeded 0x00636ad8 = scratch B2) and HEAD-
# inserted (`*piVar6 = old head; piVar6[1] = sentinel`). Port: scans all 8 bits (returns slot 3) and tail-inserts.
def _ba(bitmap, cap=3):
    s = _rwg() + [_w(0x00636ad8, _sc(B2))] + _hdr(H, 0x10, cap, 1, 4, _sc(B1), _sc(B1), 0)
    s += _blk(B1, _sc(H + 0x10), _sc(H + 0x10), bitmap)
    return {'args': [_sc(H), 0x30800], 'seed': s}


FIXES['audio_pool_block_alloc'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32']}, 'fold_ret': True,
    'observe': _OBS_SCR + [{'addr': 0x00636ad8, 'len': 4}],
    'path1_tests': [_ba(0x80), _ba(0xC0), _ba(0xE0), _ba(0xFF, 8), _ba(0xE0, 8)],
    'path2_tests': [_ba(0x80)],
})


# ── 0x005ae650 AudioPoolConstruct(elem, count, align, prealloc, hdr, flags) — defect measurement ───────────────────
# Original tail (0x005ae650 decomp): hdr[8] = &DAT_007dda80; hdr[7] = DAT_007dda80; *(DAT_007dda80 + 4) = hdr+7;
# DAT_007dda80 = hdr+7 (DAT_007dda80 is a pointer variable, head insert). Port treats 0x007dda80 as a node and
# tail-inserts through 0x007dda84. Both globals point into scratch for the arm. prealloc 0, hdr supplied: no alloc.
def _pc(elem, count, align):
    s = [_w(0x007dda80, _sc(NODE)), _w(0x007dda84, _sc(NODE + 0x10))]
    return {'args': [elem, count, align, 0, _sc(H), 0], 'seed': s}


FIXES['audio_pool_construct'] = dict(_SCR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'pointer', 'args': ['int32', 'uint32', 'uint32', 'int32', 'pointer',
                                                                'uint32']}, 'fold_ret': True,
    'observe': _OBS_SCR + [{'addr': 0x007dda80, 'len': 8}],
    'path1_tests': [_pc(0x10, 20, 4), _pc(0x1c, 7, 0), _pc(3, 64, 8)],
    'path2_tests': [_pc(0x10, 20, 4)],
})


# ── 0x005c6b60 Delta5c6b60(int* p, x, y) ───────────────────────────────────────────────────────────────────────────
# p[3] = ((x<<8) - p[1]) / 64 (round toward zero), p[4] likewise from y and p[2]; p[0] = (p[3]|p[4]) ? 0x40 : 0;
# EAX at both RETs = the value stored to p[0] (0x005c6ba0 / 0x005c6ba8). Old deref_struct_set never folded the
# return, so the ret mutant read BLIND. Now the struct is a scratch record and the return is folded.
def _d(x, y, p1, p2):
    return {'args': [_sc(0), x & 0xFFFFFFFF, y & 0xFFFFFFFF],
            'seed': [_w(_sc(0), 0x77777777), _w(_sc(4), p1 & 0xFFFFFFFF), _w(_sc(8), p2 & 0xFFFFFFFF),
                     _w(_sc(0xc), 0x55555555), _w(_sc(0x10), 0x66666666)]}


FIXES['delta_5c6b60'] = {
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32']}, 'fold_ret': True,
    'observe': [{'addr': _sc(0), 'len': 0x18}], 'scratch_size': 0x20, 'scratch_fill': 0xEE,
    'nscalar': None, 'seed_byte': None, 'target_global': None, 'crash_equal_ok': None,
    'path1_tests': [_d(0x10, 0x20, 0x1000, 0x1000), _d(0x10, 0x10, 0x1000, 0x1000), _d(0, 0, 0x1f, -0x1f),
                    _d(1, 0, 0x100, 0x3f), _d(-3, 5, 0x40, -0x80), _d(0x7fff, -0x7fff, -1, 1),
                    _d(2, 2, 0x200 - 63, 0x200 + 63), _d(0x111111, 0x222222, 0x1111, 0x2222)],
    'path2_tests': [_d(0x10, 0x20, 0x1000, 0x1000)],
}
