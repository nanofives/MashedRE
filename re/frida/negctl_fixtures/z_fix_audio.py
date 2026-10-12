"""negctl re-earn 2026-10-12, cluster AUDIO: fixtures for the re-ported rows (loads last, overrides b13/b14).

0x005ae650 / 0x005ae800 / 0x005ae920: the b14 vectors measured the one defect the static read named. These extend
them over every branch of the bytes re-port (mashedmod/src/mashed_re/Audio/AudioRws.cpp), keeping the b14 vectors.
0x005baf90 / 0x005bc400: b14 left them without a fixture (no fake stdcall method that writes its out-param). The
fake COM methods below are existing MASHED.exe leaf bodies with the exact stdcall shape, verified by disassembly
of MASHED.exe.unpatched:
  0x005c08a0  mov eax,[esp+4]; mov edx,[esp+8]; mov ecx,[eax+0x10]; xor eax,eax; mov [edx],ecx; ret 8
              -> 2-arg method: *out = this[+0x10], returns 0
  0x005c07cf  mov eax,[esp+8]; mov dword [eax],0; mov eax,0x80040209; ret 8      -> *out = 0, returns <0
  0x0049fa30  xor eax,eax; ret 8                                                  -> out untouched
  0x005beda0  mov eax,[esp+4]; mov edx,[esp+0xc]; mov ecx,[eax+0x128]; xor eax,eax; mov [edx],ecx; ret 0xc
              -> QueryInterface-shaped: *ppv = this[+0x128], returns 0 (S_OK)
  0x005bf6fb  mov eax,[esp+0xc]; mov dword [eax],0; mov eax,0x80004002; ret 0xc  -> *ppv = 0, E_NOINTERFACE
  0x0049cf50  mov eax,0x80004001; ret 0xc                                         -> E_NOTIMPL, ppv untouched
  0x0049c7f0  mov eax,1; ret 0xc                                                  -> S_FALSE (>= 0), ppv untouched
  0x005044b0  mov eax,[esp+4]; inc dword [eax+4]; mov eax,[eax+4]; ret 4          -> Release-shaped, this[+4]++
Recorders shared with b14: P 0x004099e0 (RwFree slot, records its arg at 0x008a95a0), G 0x00402f40 (RwMalloc slot,
returns the seeded dword at 0x00636ad8).
"""

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'
P, G = 0x004099e0, 0x00402f40
_BASE = {'scratch_fill': 0xEE, 'freeze_main': True, 'target_global': None, 'crash_equal_ok': None,
         'pre_fill_byte': None, 'nscalar': None, 'seed_byte': None, 'fold_ret': True}


def _sc(off):
    return {'scratch': off}


def _w(addr, val, size=4):
    return {'addr': addr, 'val': val, 'size': size}


def _bytes(off, bs):
    return [_w(_sc(off + i), b & 0xff, 1) for i, b in enumerate(bs)]


# ── pool layout (scratch 0x800) ────────────────────────────────────────────────────────────────────────────────────
# 0x000 fake RwGlobals (+0x108 = G, +0x10c = P)   0x220 header H   0x260 block B1   0x300 header H2
# 0x380 global-list node   0x400 block B3   0x600 block B2 (what G hands out when 0x00636ad8 = scratch+0x600)
H, B1, H2, NODE, B3, B2 = 0x220, 0x260, 0x300, 0x380, 0x400, 0x600
_POOL = dict(_BASE, **{'arg_type': _SCAL, 'scratch_size': 0x800})
_OBS_POOL = [{'addr': _sc(0x200), 'len': 0x600}, {'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5},
             {'addr': 0x00636ad8, 'len': 4}, {'addr': 0x007dda80, 'len': 8}, {'addr': 0x007ddab0, 'len': 4}]


def _rwg(alloc=B2):
    return [_w(0x007d3ff8, _sc(0)), _w(_sc(0x10c), P), _w(_sc(0x108), G),
            _w(0x00636ad8, _sc(alloc) if alloc is not None else 0)]


def _hdr(off, elem, cap, nbytes, align, blocks, flags):
    """Header at scratch+off with the circular block list [blocks] (scratch offsets, in list order)."""
    sent = _sc(off + 0x10)
    head = _sc(blocks[0]) if blocks else sent
    tail = _sc(blocks[-1]) if blocks else sent
    s = [_w(_sc(off), elem), _w(_sc(off + 4), cap), _w(_sc(off + 8), nbytes), _w(_sc(off + 0xc), align),
         _w(_sc(off + 0x10), head), _w(_sc(off + 0x14), tail), _w(_sc(off + 0x18), flags)]
    for i, b in enumerate(blocks):
        nxt = _sc(blocks[i + 1]) if i + 1 < len(blocks) else sent
        prv = _sc(blocks[i - 1]) if i > 0 else sent
        s += [_w(_sc(b), nxt), _w(_sc(b + 4), prv)]
    return s


# ── 0x005ae800 AudioPoolBlockAlloc(h, flags) ───────────────────────────────────────────────────────────────────────
def _ba(bitmaps, cap, nbytes=1, elem=0x10, align=4, alloc=B2, blocks=(B1, B3)):
    blks = list(blocks[:len(bitmaps)])
    s = _rwg(alloc) + _hdr(H, elem, cap, nbytes, align, blks, 0)
    for b, bm in zip(blks, bitmaps):
        s += _bytes(b + 8, bm)
    return {'args': [_sc(H), 0x30800], 'seed': s}


FIXES['audio_pool_block_alloc'] = dict(_POOL, **{
    'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32']}, 'observe': _OBS_POOL,
    'path1_tests': [
        _ba([[0x80]], 3), _ba([[0xC0]], 3), _ba([[0xE0]], 3), _ba([[0xFF]], 8), _ba([[0xE0]], 8),   # b14 set
        _ba([[0xFF, 0xF0]], 12, 2),            # countdown 12-8=4 runs out inside byte 1 -> new block
        _ba([[0xFF, 0xF0]], 16, 2),            # slot 12
        _ba([[0xFF, 0x00]], 4, 2),             # 0xff at countdown 4 wraps it -> byte 1 scanned, slot 8
        _ba([[0xFF, 0xFF, 0x00]], 20, 3),      # slot 16
        _ba([[0x7F, 0x00]], 10, 2),            # MSB-first: slot 0
        _ba([[0xFF], [0x80]], 8),              # first block full -> slot 1 of the second block
        _ba([[0xE0], [0x00]], 3),              # cap 3 exhausted in block 1 -> slot 0 of block 2
        _ba([[0xC0]], 8, 1, 0x14, 0x10),       # elem 0x14, align 0x10 -> slot 2
        _ba([], 8),                            # empty list -> new block
        _ba([], 8, alloc=None),                # empty list, RwMalloc returns 0 -> 0
    ],
    'path2_tests': [_ba([[0x80]], 3)],
})


# ── 0x005ae920 AudioPoolFree(h, item) ──────────────────────────────────────────────────────────────────────────────
def _pf(item_off, bitmaps, flags, cap=3, nbytes=1, blocks=(B1,)):
    blks = list(blocks)
    s = _rwg() + _hdr(H, 0x10, cap, nbytes, 4, blks, flags)
    for b, bm in zip(blks, bitmaps):
        s += _bytes(b + 8, bm)
    return {'args': [_sc(H), _sc(item_off)], 'seed': s}


def _it(blk, k, nbytes=1, extra=0):
    return blk + 8 + nbytes + 0x10 * k + extra     # range base = block + nbytes + 8 (0x005ae93d)


FIXES['audio_pool_free'] = dict(_POOL, **{
    'signature': {'ret': 'void', 'args': ['pointer', 'pointer']}, 'fold_ret': False, 'observe': _OBS_POOL,
    'path1_tests': [
        _pf(_it(B1, 1), [[0xE0]], 0), _pf(_it(B1, 0), [[0xE0]], 0), _pf(_it(B1, 3), [[0xF0]], 0),   # b14 set
        _pf(_it(B1, 0), [[0x80]], 2), _pf(_it(B1, 1), [[0xE0]], 2),
        _pf(_it(B1, 3, extra=1), [[0xF0]], 0),         # one past the inclusive end -> no-op
        _pf(_it(B1, 0, extra=-1), [[0xF0]], 0),        # one before base -> no-op
        _pf(_it(B1, 1, extra=5), [[0xE0]], 0),         # unaligned item -> idx 1
        _pf(_it(B3, 0), [[0xF0], [0x80]], 2, blocks=(B1, B3)),   # list non-empty, bitmap empties -> RwFree(B3)
        _pf(_it(B3, 0), [[0xF0], [0xC0]], 2, blocks=(B1, B3)),   # bytes still non-zero -> relink B3 at head
        _pf(_it(B3, 1), [[0xF0], [0x40]], 0, blocks=(B1, B3)),   # no compaction flag -> relink
        _pf(_it(B1, 0), [[0x80], [0xC0]], 3, blocks=(B1, B3)),   # flags 3, B1 empties, B3 left -> RwFree(B1)
        _pf(_it(B1, 9, 2), [[0xFF, 0xC0]], 2, cap=12, nbytes=2), # byte 1 bit 1
    ],
    'path2_tests': [_pf(_it(B1, 1), [[0xE0]], 0)],
})


# ── 0x005ae650 AudioPoolConstruct(elem, count, align, prealloc, hdr, flags) ────────────────────────────────────────
def _pc(elem, count, align, prealloc=0, hdr=H, flags=0, alloc=B2, hpool=None):
    s = _rwg(alloc) + [_w(0x007dda80, _sc(NODE)), _w(0x007dda84, _sc(NODE + 0x10)), _w(_sc(NODE + 4), 0x0BADF00D)]
    if hpool is None:
        s.append(_w(0x007ddab0, 0))
    else:
        s.append(_w(0x007ddab0, _sc(H2)))
        s += _hdr(H2, 0x24, 8, 1, 4, [B3], 0) + _bytes(B3 + 8, [hpool])
    return {'args': [elem, count, align, prealloc, _sc(hdr) if hdr is not None else 0, flags], 'seed': s}


FIXES['audio_pool_construct'] = dict(_POOL, **{
    'signature': {'ret': 'pointer', 'args': ['int32', 'uint32', 'uint32', 'int32', 'pointer', 'uint32']},
    'observe': _OBS_POOL,
    'path1_tests': [
        _pc(0x10, 20, 4), _pc(0x1c, 7, 0), _pc(3, 64, 8),                    # b14 set
        _pc(0x10, 20, 4, hdr=None, flags=0x30806),                           # DAT_007ddab0 == 0 -> RwMalloc(0x24)
        _pc(0x10, 20, 4, hdr=None, flags=0x30806, hpool=0x80),               # header from FUN_005ae800(H2, 0x30800)
        _pc(0x10, 20, 4, hdr=None, alloc=None),                              # header alloc fails -> 0
        _pc(0x10, 20, 4, prealloc=1, flags=0x30806),                         # one block, head insert
        _pc(0x0c, 9, 4, prealloc=2, flags=0x30806),                          # G hands out B2 twice
        _pc(0x10, 20, 4, prealloc=1, alloc=None),                            # block alloc fails -> FUN_005ae780, 0
    ],
    'path2_tests': [_pc(0x10, 20, 4)],
})


# ── 0x005baf90 AudioDSoundRelease(p) ───────────────────────────────────────────────────────────────────────────────
# Scratch 0x300: 0x000 vtable V (+0x20 = slot 8), 0x100 object O (+0 = V, +0x10 = value), p at 0x200 (+0x7c = O).
def _dr(method, value, poff=0x200):
    s = [_w(_sc(0x20), method), _w(_sc(0x100), _sc(0)), _w(_sc(0x110), value & 0xFFFFFFFF),
         _w(_sc(poff + 0x7c), _sc(0x100))]
    return {'args': [_sc(poff)], 'seed': s}


_GET, _ZERO, _NOP8 = 0x005c08a0, 0x005c07cf, 0x0049fa30
FIXES['audio_dsound_release'] = dict(_BASE, **{
    'arg_type': _SCAL, 'signature': {'ret': 'int32', 'args': ['pointer']}, 'scratch_size': 0x300,
    'lut_root_delta': None, 'observe': [{'addr': _sc(0), 'len': 0x300}],
    'path1_tests': [_dr(_GET, v) for v in (0, 1, 2, 3, 4, 5, 6, 7, 8, 0xFF, 0x105, 0x1234502, 0xFFFFFF07)]
                   + [_dr(_ZERO, 5), _dr(_NOP8, 5, 0x205), _dr(_NOP8, 5, 0x201)],
    'path2_tests': [_dr(_GET, 5)],
})


# ── 0x005bc400 AudioDSoundQIChain(obj, out) ────────────────────────────────────────────────────────────────────────
# Scratch 0x600: 0x000 V1 / 0x040 V2 (slot 0 = QI, slot 2 = Release); 0x100 O1 (+0x128 -> what QI writes),
# 0x300 O2 (+0x128 = marker); 0x500 out[2] (fill 0xEE). Release = 0x005044b0 bumps this[+4].
_QOK, _QNOI, _QNI, _QFALSE, _REL = 0x005beda0, 0x005bf6fb, 0x0049cf50, 0x0049c7f0, 0x005044b0
O1, O2, OUT = 0x100, 0x300, 0x500


def _qi(qi1, qi2, o1_target=O2):
    s = [_w(_sc(0x00), qi1), _w(_sc(0x08), _REL), _w(_sc(0x40), qi2), _w(_sc(0x48), _REL),
         _w(_sc(O1), _sc(0x00)), _w(_sc(O1 + 4), 0x10), _w(_sc(O1 + 0x128), _sc(o1_target)),
         _w(_sc(O2), _sc(0x40)), _w(_sc(O2 + 4), 0x20), _w(_sc(O2 + 0x128), 0x13579BDF)]
    return {'args': [_sc(O1), _sc(OUT)], 'seed': s}


FIXES['audio_dsound_qi_chain'] = dict(_BASE, **{
    'arg_type': _SCAL, 'signature': {'ret': 'int32', 'args': ['pointer', 'pointer']}, 'scratch_size': 0x600,
    'lut_root_delta': None, 'observe': [{'addr': _sc(0), 'len': 0x600}],
    'path1_tests': [
        _qi(_QOK, _QOK),            # both succeed -> 1, out = {O2, marker}
        _qi(_QOK, _QNOI),           # second fails, writes 0 -> Release(O2), 0
        _qi(_QOK, _QNI),            # second fails, no write -> Release(O2), 0
        _qi(_QOK, _QFALSE),         # S_FALSE counts as success -> 1, out[1] stays 0
        _qi(_QNOI, _QOK),           # first fails -> 0
        _qi(_QNI, _QOK),            # first fails, no write -> 0
        _qi(_QOK, _QOK, O1),        # O1 answers with itself; second QI runs on O1 -> 1
        _qi(_QOK, _QNOI, O1),       # Release lands on O1
    ],
    'path2_tests': [_qi(_QOK, _QOK)],
})
