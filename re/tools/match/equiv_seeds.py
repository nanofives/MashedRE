"""Seeded inputs for equiv_dyn.py (EQUIV-ADMIT-1, L-126 cross-check coverage).

Some proven rows walk circular lists whose end is the list head, or loop on a callee's result. Uniform random memory
never closes the list and a stub's hash return is never zero, so every run of those rows hits the instruction or call
cap and the cross-check stays inconclusive. A seed builds a well-formed input for one RVA: the structures its body
walks (read from the body itself, cited per seed), the stack arguments, and optionally a callee effect (bytes the stub
writes through a pointer argument, derived from the stub record's hash, so both bodies see the same bytes when they
make the same call).

A seed only chooses inputs. The comparison is unchanged: same stub records, same non-stack writes, same registers.

    seed(rng) -> dict(args=[u32,...], regs={name: u32}, mem=[(addr, bytes)], effect=fn or None)
    effect(target, args, h) -> [(addr, bytes)]      target = resolved callee address, h = the record hash (12 bytes)

In seeded mode a stub also returns EAX = 0 when the record hash's first byte is a multiple of 4 (one call in four), so
loops on a callee result end and the zero-result branches run.
"""
from __future__ import annotations

import struct

SEED_BASE = 0x58000000          # seeded structures (mapped by the harness on write)
EXT_FN = 0x30F00000             # a function pointer outside every mapped range: a call to it is stubbed


def u32(v):
    return struct.pack("<I", v & 0xFFFFFFFF)


def _rand_ptr(rng):
    return 0 if rng.random() < 0.3 else SEED_BASE + 0x80000 + rng.randrange(0, 0x1000) * 4


# 0x005aaac0: p = DAT_007dccf0; while p != &DAT_007dccf0: if cb(p - 1, param_2) == 0 return; p = *p
# (re/match/s25/005aaac0.c). The list head is the global at 0x007dccf0; a node's link is at node + 4.
def seed_005aaac0(rng):
    head = 0x007DCCF0
    n = rng.randrange(0, 6)
    nodes = [SEED_BASE + 0x40 * i for i in range(n)]
    mem = []
    links = [b + 4 for b in nodes]
    mem.append((head, u32(links[0] if n else head)))
    for i, b in enumerate(nodes):
        mem.append((b, u32(rng.getrandbits(32))))
        mem.append((b + 4, u32(links[i + 1] if i + 1 < n else head)))
    return dict(args=[EXT_FN, rng.getrandbits(32)], mem=mem)


# 0x005aab00(param_1, param_2): obj[0] = vtable, vtable + 0x2c = fn or 0; obj[3] = circular list head (obj + 0xc);
# a link points at element + 0x14, the element's [0] is its vtable (re/match/s25/005aab00.c).
def seed_005aab00(rng):
    obj = SEED_BASE
    mem = []

    def vtbl(at):
        mem.append((at + 0x2C, u32(EXT_FN + 0x10 * rng.randrange(0, 4) if rng.random() < 0.7 else 0)))
        return at

    mem.append((obj, u32(vtbl(SEED_BASE + 0x1000))))
    n = rng.randrange(0, 5)
    head = obj + 0xC
    elems = [SEED_BASE + 0x2000 + 0x100 * i for i in range(n)]
    links = [e + 0x14 for e in elems]
    mem.append((head, u32(links[0] if n else head)))
    for i, e in enumerate(elems):
        mem.append((e, u32(vtbl(SEED_BASE + 0x3000 + 0x40 * i))))
        mem.append((e + 0x14, u32(links[i + 1] if i + 1 < n else head)))
    return dict(args=[obj, rng.choice([0, 0, 1, rng.getrandbits(32)])], mem=mem)


# 0x005ae920(pool, addr): pool[0] = element size, pool[1] = count, pool[2] = bitmap bytes, pool + 0x10 = circular
# doubly linked head (next, prev), pool[6] bit 1 = flag; a node is (next, prev, bitmap[nbytes], data[count*size])
# (re/match/s26/005ae920.c). addr lands inside one node's data, between nodes, or outside them all.
def seed_005ae920(rng):
    pool = SEED_BASE
    es = rng.choice([4, 8, 12, 16, 32])
    cnt = rng.randrange(1, 24)
    nb = rng.choice([(cnt + 7) // 8, (cnt + 7) // 8, rng.randrange(1, 5)])
    n = rng.randrange(0, 5)
    head = pool + 0x10
    nodes = [SEED_BASE + 0x1000 * (i + 1) for i in range(n)]
    mem = [(pool, u32(es) + u32(cnt) + u32(nb) + u32(rng.getrandbits(32))),
           (pool + 0x18, u32(rng.getrandbits(32)))]
    ring = [head] + nodes
    for i, a in enumerate(ring):
        nxt = ring[(i + 1) % len(ring)]
        prv = ring[i - 1]
        mem.append((a, u32(nxt) + u32(prv)))
    for a in nodes:
        r = rng.random()
        if r < 0.3:
            bm = bytes(nb)
        elif r < 0.6:
            bm = bytes([0x80 >> rng.randrange(0, 8)]) + bytes(nb - 1)
        else:
            bm = bytes(rng.getrandbits(8) for _ in range(nb))
        mem.append((a + 8, bm))
    r = rng.random()
    if n and r < 0.75:
        a = rng.choice(nodes)
        ds = a + 8 + nb
        addr = ds + rng.randrange(0, cnt * es + 1)
    else:
        addr = rng.getrandbits(32)
    return dict(args=[pool, addr], mem=mem)


# 0x005ab410(p1, id, out_b, out_c): FUN_005ab380(p1, &h, 0) fills a 20-byte header h on the caller's stack; the loop
# runs while h.a != id, calling FUN_004cc050(p1, h.b) (re/match/s25/005ab410.c). The effect writes h for each
# 0x005ab380 call: h.a = id one time in three, h.c inside [0x35000, 0x37002] one time in two.
def seed_005ab410(rng):
    ident = rng.getrandbits(32)

    def effect(target, args, h):
        if target != 0x005AB380 or len(args) < 2:
            return []
        a = ident if h[1] % 3 == 0 else struct.unpack_from("<I", h, 4)[0]
        c = 0x35000 + (struct.unpack_from("<H", h, 8)[0] % 0x2003) if h[2] % 2 == 0 else struct.unpack_from("<I", h, 8)[0]
        return [(args[1], u32(a) + h[4:8] + u32(c) + h[0:8])]

    return dict(args=[rng.getrandbits(32), ident, _rand_ptr(rng), _rand_ptr(rng)], effect=effect)


# 0x00485b30(pool, data): Pool = {u16 count, u16 max, head, tail, nodes, used*}; PoolNode = {data, prev, next, u16 idx}
# (re/match/s14/00485b30.c). A doubly linked list of 0..5 nodes, head and tail consistent; data is the head's, the
# tail's, a middle node's, or none of them.
def seed_00485b30(rng):
    pool = SEED_BASE
    n = rng.randrange(0, 6)
    nodes = [SEED_BASE + 0x100 + 0x20 * i for i in range(n)]
    datas = [rng.getrandbits(32) for _ in range(n)]
    used = SEED_BASE + 0x800
    mem = [(pool, struct.pack("<HH", rng.randrange(0, 50), rng.randrange(0, 50)) + u32(nodes[0] if n else 0) +
            u32(nodes[-1] if n else 0) + u32(nodes[0] if n else 0) + u32(used))]
    for i, a in enumerate(nodes):
        mem.append((a, u32(datas[i]) + u32(nodes[i - 1] if i else 0) + u32(nodes[i + 1] if i + 1 < n else 0) +
                    struct.pack("<HH", rng.randrange(0, 16), 0)))
    mem.append((used, bytes(rng.getrandbits(8) for _ in range(64))))
    r = rng.random()
    data = datas[rng.randrange(0, n)] if n and r < 0.8 else rng.getrandbits(32)
    return dict(args=[pool, data], mem=mem)


# 0x004b61c0(geom, color): geom +4 triangles (u16 x3 + s16), +8 vertices (3 floats), u16 counts at +0x82 / +0x84;
# color is an RGBA pointer or NULL; RenderStateSet calls through *(DAT_007d3ff8 + 0x20)
# (re/match/s18/004b61c0.c). Small counts, so the copy loops finish under the instruction cap.
def seed_004b61c0(rng):
    geom = SEED_BASE
    nv = rng.randrange(0, 14)
    nt = rng.randrange(0, 7)
    tris = SEED_BASE + 0x1000
    verts = SEED_BASE + 0x2000
    col = SEED_BASE + 0x3000
    rwg = SEED_BASE + 0x4000
    mem = [(geom, u32(rng.getrandbits(32)) + u32(tris) + u32(verts)),
           (geom + 0x82, struct.pack("<HH", nv, nt)),
           (tris, b"".join(struct.pack("<HHHh", rng.randrange(0, 16), rng.randrange(0, 16), rng.randrange(0, 16),
                                         rng.randrange(-4, 4)) for _ in range(nt))),
           (verts, b"".join(struct.pack("<fff", rng.uniform(-50, 50), rng.uniform(-50, 50), rng.uniform(-50, 50))
                            for _ in range(nv))),
           (col, bytes([rng.getrandbits(8), rng.getrandbits(8), rng.getrandbits(8),
                        0xFF if rng.random() < 0.5 else rng.getrandbits(8)])),
           (0x007D3FF8, u32(rwg)),
           (rwg + 0x20, u32(EXT_FN + 0x40))]
    return dict(args=[geom, col if rng.random() < 0.7 else 0], mem=mem)


SEEDS = {
    0x005AAAC0: seed_005aaac0,
    0x005AAB00: seed_005aab00,
    0x005AE920: seed_005ae920,
    0x005AB410: seed_005ab410,
    0x00485B30: seed_00485b30,
    0x004B61C0: seed_004b61c0,
}
