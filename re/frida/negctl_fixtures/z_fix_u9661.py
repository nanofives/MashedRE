"""U-9661 fixture (2026-10-12): 0x005ade90 list drain, keys audio_list_drain (installed) + audio_list_drain2 (dead copy).

Installed body: AudioListDrain in Audio/AudioRws.cpp (live RH_ScopedInstall). AudioListDrain2 in Audio/AudioDSound.cpp
is still compiled and exported but its RH_ScopedInstall is commented out. Both keys get the same vectors.

Old fixtures: `audio_list_drain` drained empty sentinels only (all tests 0), so the loop body at 0x005ade9c never ran.
`audio_list_drain2` built nodes with the game's insert 0x005addd0, which takes nodes from the pool at 0x009146c0;
that pool is not set up at inject time and the insert AVs at 0x0 on both arms before the target runs.

Bytes (MASHED.exe.unpatched):
  0x005ade95 mov eax,[esi+4]; cmp eax,esi; je done         -- current = sentinel[+4]
  0x005ade9c ecx=[eax]; edx=[eax+4]; [ecx+4]=edx            -- node[+0] (prev)'s +4 = node[+4]
  0x005adeaa ecx=[eax+4]; edx=[eax]; [ecx]=edx              -- node[+4] (next)'s +0 = node[+0]
  0x005adeb1 call 0x005ae920(0x009146c0, node)              -- pool free, header at 0x009146c0 itself
  0x005adeb6 eax=[esi+4]; cmp eax,esi; jne 0x005ade9c
Pool free 0x005ae920 (header h: +0 elem, +4 cap, +8 nbytes, +0x10/+0x14 block-list sentinel, +0x18 flags):
  walks blocks from h[+0x10]; item in [blk+nbytes+8, +cap*elem] (0x005ae93d..0x005ae950, inclusive end);
  clears bit 0x80>>(idx&7) of byte blk[8+idx>>3] (0x005ae983); unlinks blk (0x005ae98f); if list non-empty and
  flags&2 and every bitmap byte is 0 -> 0x004522d0(blk) (RwFree) (0x005ae9c4); else relinks blk at the head.

Fixture: the pool header is SEEDED at 0x009146c0 (BSS; seeds are saved and restored around each arm, game frozen);
blocks, list sentinel and fake RwGlobals live in scratch. Nodes are pool items, so each pool free does real work.
RwFree slot = 0x004099e0, which records its arg at 0x008a95a0 (same recorder as z_fix_audio.py).
"""

FIXES = {}
POOL = 0x009146c0
PSENT = POOL + 0x10
P, G = 0x004099e0, 0x00402f40
SENT = 0x180                          # list sentinel (scratch)
B1, B3 = 0x200, 0x400                 # pool blocks (scratch)


def _sc(off):
    return {'scratch': off}


def _w(addr, val, size=4):
    return {'addr': addr, 'val': val, 'size': size}


def _bytes(off, bs):
    return [_w(_sc(off + i), b & 0xff, 1) for i, b in enumerate(bs)]


def _item(blk, k, nbytes=1, elem=0x10, extra=0):
    return blk + 8 + nbytes + elem * k + extra


def _dr(items, bitmaps, flags=0, cap=8, nbytes=1, elem=0x10, blocks=(B1,)):
    """items: scratch offsets of the list nodes, in list order from sentinel[+4]."""
    blks = list(blocks[:len(bitmaps)])
    s = [_w(0x007d3ff8, _sc(0)), _w(_sc(0x10c), P), _w(_sc(0x108), G), _w(0x00636ad8, 0)]
    # pool header at 0x009146c0
    head = _sc(blks[0]) if blks else PSENT
    tail = _sc(blks[-1]) if blks else PSENT
    s += [_w(POOL, elem), _w(POOL + 4, cap), _w(POOL + 8, nbytes), _w(POOL + 0xc, 4),
          _w(POOL + 0x10, head), _w(POOL + 0x14, tail), _w(POOL + 0x18, flags)]
    for i, b in enumerate(blks):
        nxt = _sc(blks[i + 1]) if i + 1 < len(blks) else PSENT
        prv = _sc(blks[i - 1]) if i > 0 else PSENT
        s += [_w(_sc(b), nxt), _w(_sc(b + 4), prv)]
    for b, bm in zip(blks, bitmaps):
        s += _bytes(b + 8, bm)
    # circular list: node[+0] = prev, node[+4] = next; sentinel[+4] = first
    ring = [SENT] + list(items)
    for i, n in enumerate(ring):
        s += [_w(_sc(n), _sc(ring[i - 1])), _w(_sc(n + 4), _sc(ring[(i + 1) % len(ring)]))]
    return {'args': [_sc(SENT)], 'seed': s}


_VECS = [
    _dr([], [[0xE0]]),                                                    # empty list -> no pool call
    _dr([_item(B1, 0)], [[0x80]]),                                        # 1 node, bit 0
    _dr([_item(B1, 0), _item(B1, 1), _item(B1, 2)], [[0xE0]]),            # 3 nodes in slot order
    _dr([_item(B1, 2), _item(B1, 0), _item(B1, 1)], [[0xE0]]),            # same nodes, other order
    _dr([_item(B1, k) for k in (4, 1, 6, 0, 3)], [[0xDA]]),               # 5 nodes, other bits stay set
    _dr([_item(B1, 0), _item(B3, 1)], [[0x80], [0x40]], flags=2,
        blocks=(B1, B3)),                                                 # flags 2: B1 empties -> RwFree(B1)
    _dr([_item(B1, 0), _item(B3, 0)], [[0xC0], [0x80]], flags=2,
        blocks=(B1, B3)),                                                 # B1 keeps a bit, B3 empties -> RwFree(B3)
    _dr([_item(B1, 0)], [[0x80]], flags=2),                               # lone block empties, list empty -> relink
    _dr([_item(B1, 9, 2), _item(B1, 2, 2)], [[0xFF, 0xC0]], cap=12,
        nbytes=2),                                                        # nbytes 2, byte 1 bit 1
    _dr([_item(B1, 1), 0x600], [[0xC0]]),                                 # second node outside every block -> unlink only
    _dr([_item(B1, 8, extra=1), _item(B1, 1, extra=5)], [[0xC0]]),        # one past inclusive end; unaligned -> idx 1
]

_OBS = [{'addr': _sc(0x100), 'len': 0x600}, {'addr': POOL, 'len': 0x1c},
        {'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5}]
_FIX = {'arg_type': 'scalars_to_scattered_globals', 'scratch_size': 0x800, 'scratch_fill': 0xEE,
        'freeze_main': True, 'fold_ret': False, 'signature': {'ret': 'void', 'args': ['pointer']},
        'observe': _OBS, 'insert_rva': None, 'lut_root_delta': None, 'target_global': None,
        'crash_equal_ok': None, 'pre_fill_byte': None, 'nscalar': None, 'seed_byte': None,
        'path1_tests': _VECS, 'path2_tests': [_VECS[2]]}

FIXES['audio_list_drain'] = dict(_FIX)
FIXES['audio_list_drain2'] = dict(_FIX)
