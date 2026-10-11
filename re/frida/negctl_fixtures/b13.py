"""negctl re-earn, batch b13 (counted-stride for-all, VFS read, RtFS handler leaves, font init, RWS audio tree / list /
chunk / pool helpers).

Decomp from re/tools/decomp_pc.py (pool slot, read-only), disasm from re/tools/disasm_va.py on
original\\MASHED.exe.unpatched.

Original leaves used as callees (verified by disasm):
  P1   0x004099e0  `mov eax,[esp+4]; mov [0x008a95a0],eax; ret`              records arg1, returns arg1
  P2   0x004d1630  `mov eax,[esp+8]; mov [0x007d46c8],eax; mov eax,1; ret`    records arg2, returns 1
  R3   0x00562a10  `mov eax,[esp+0xc]; ret`                                   returns arg3
  G    0x00402f40  `mov eax,[0x00636ad8]; ret`                                returns the seeded dword
  RD   0x004cbd30  RwStreamRead(stream, buf, n); type-3 (memory) stream: [0]=3, [3]=pos, [4]=size, [5]=data
  M8D0 0x005aa8d0  (p1, p2): if *p1==*p2 && (p1[7]&4) { p2[1]=p1[7]; return 0 } return 1
  PCTX 0x005ac930  (p1, node, ctx): walks node+0x20 list, memcmp16 (0x005adf30) of entry[-0x40] vs ctx[0];
                   on match ctx[1] = entry-0x40, return 1; else 0
RwFree 0x004522d0 is `mov ecx,[0x007d3ff8]; jmp [ecx+0x10c]`: a fake RwGlobals (DAT_007d3ff8 -> scratch) whose +0x10c
slot is P1 records the freed pointer. Every recorder target is pre-filled 0xA5 and restored after each arm.
"""

FIXES = {}
_SCAL = 'scalars_to_scattered_globals'

P1, P2, R3, G, RD, M8D0, PCTX = 0x004099e0, 0x004d1630, 0x00562a10, 0x00402f40, 0x004cbd30, 0x005aa8d0, 0x005ac930
_P1_OBS = {'addr': 0x008a95a0, 'len': 4, 'fill': 0xA5}
_P2_OBS = {'addr': 0x007d46c8, 'len': 4, 'fill': 0xA5}
_CLR = {'target_global': None, 'crash_equal_ok': None, 'degenerate_ok': None}


def S(off):
    return {'scratch': off}


def _d(off, v):
    """dword seed at scratch+off (v may be S(..))"""
    return {'addr': S(off), 'val': v}


def _g(addr, v):
    return {'addr': addr, 'val': v}


def _bytes(off, bs):
    return [{'addr': S(off + i), 'val': b, 'size': 1} for i, b in enumerate(bs)]


def _u32(x):
    return x & 0xFFFFFFFF


# ── 0x004f0d80 CountedStride0cForAll(obj, cb, user) ─────────────────────────────────────────────────────────────────
# count = u16 [obj+4]; cursor = obj + [obj+0xc] + 0x10; while count--: if cb(cursor, obj, user) == 0 break;
# cursor += 0xc; return obj. Old stub_dispatch_observe vectors: ret not observed (BLIND), field_first BLIND.
# Now: obj is scratch, cb is an original leaf, return folded.
#  A (cb=P1): last cursor recorded at 0x008a95a0 -> count (u16, upper half 0x0001 set), base +0x10, stride.
#  B (cb=M8D0): matching element k writes obj[1] = e_k[7] and stops; a second match at k=3 carries another value, so
#    early-exit is visible.
#  C (cb=RD, count 1): RwStreamRead(cursor, obj, user) copies `user` bytes from the element's stream into obj, so
#    cursor, obj and user (the forwarded 3rd arg) are all visible.
def _fa_hdr(count, off, key=0):
    return [_d(0, key), _d(4, (count & 0xFFFF) | 0x00010000), _d(0xc, _u32(off))]


def _fa_a(count, off):
    return {'args': [S(0), P1, 0x5151], 'seed': _fa_hdr(count, off)}


def _fa_b(f1, f3, count=5):
    K = 0x55
    base = 0x10
    seeds = _fa_hdr(count, 0, K)
    for k in range(6):
        seeds.append(_d(base + 0xc * k, 0))
    seeds += [_d(base + 0xc * 1, K), _d(base + 0xc * 1 + 0x1c, f1),
              _d(base + 0xc * 3, K), _d(base + 0xc * 3 + 0x1c, f3)]
    return {'args': [S(0), M8D0, 0x77], 'seed': seeds}


def _fa_c(user, pos, size=0x20, off=0):
    e0 = 0x10 + off
    seeds = _fa_hdr(1, off) + [_d(e0, 3), _d(e0 + 0xc, pos), _d(e0 + 0x10, size), _d(e0 + 0x14, S(0x80))]
    seeds += [_d(0x80 + 4 * i, 0x11223344 + 0x01010101 * i) for i in range(8)]
    return {'args': [S(0), RD, user], 'seed': seeds}


FIXES['d3d9_decl_element_forall'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0xc0}, _P1_OBS], 'scratch_size': 0xc0, 'scratch_fill': 0xEE,
    'freeze_main': True, 'arg_layout': None, 'num_bufs': None, 'buf_size': None, 'stub_nargs': None,
    'stub_abi': None, 'observe_ret': None, 'observe_calls': None,
    'path1_tests': [_fa_a(3, 0), _fa_a(1, 0x20), _fa_a(5, -8), _fa_a(0, 0), _fa_a(2, 0x14),
                    _fa_b(0xAAAA0004, 0xBEEF0004), _fa_b(0xAAAA0003, 0xBEEF0004), _fa_b(0xAAAA0004, 0xBEEF0004, 2),
                    _fa_b(0xAAAA0004, 0xBEEF0004, 0),
                    _fa_c(6, 2), _fa_c(4, 0x10), _fa_c(0, 0), _fa_c(8, 0x1c), _fa_c(3, 1, 0x20, 0x8)],
    'path2_tests': [_fa_a(3, 0)],
})


# ── 0x00550980 VfsStreamRead(buf, esize, count, ctx) ─────────────────────────────────────────────────────────────────
# return (unsigned)[[ctx+0x38]+0x34](ctx, buf, esize*count) / esize  (DIV ESI at 0x005509a5, unsigned).
# Old int_pair vectors passed ctx = 0: crash-equal on both sides. ctx is now a scratch type-3 RW memory stream whose
# +0x38 points at a scratch table; slot +0x34 is RD (reads total bytes into buf), R3 (returns the total) or P1
# (records ctx). R3 vectors with a total >= 0x80000000 or a negative esize probe the unsigned divide.
def _vfs(fn, esize, count, pos=0, size=0x20):
    seeds = [_d(0, 3), _d(0xc, pos), _d(0x10, size), _d(0x14, S(0x100)), _d(0x38, S(0x40)), _d(0x40 + 0x34, fn)]
    seeds += [_d(0x100 + 4 * i, 0xA0B0C0D0 + 0x01020304 * i) for i in range(8)]
    return {'args': [S(0x200), _u32(esize), _u32(count), S(0)], 'seed': seeds}


FIXES['vfs_stream_read'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32', 'pointer']},
    'fold_ret': True, 'observe': [{'addr': S(0), 'len': 0x240}, _P1_OBS], 'scratch_size': 0x240,
    'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': [_vfs(RD, 4, 2), _vfs(RD, 3, 3, 4), _vfs(RD, 4, 5, 0x10), _vfs(RD, 5, 1), _vfs(RD, 8, 3, 0x14),
                    _vfs(R3, 7, 3), _vfs(R3, 1, 0x1234), _vfs(P1, 1, 1), _vfs(P1, 4, 1),
                    _vfs(R3, 4, 0x20000001), _vfs(R3, -1, -5), _vfs(R3, 2, 0x40000000)],
    'path2_tests': [_vfs(RD, 4, 2)],
})


# ── 0x00551180 RtFSHandlerCancel(slot) ───────────────────────────────────────────────────────────────────────────────
# `mov ecx,[esp+4]; mov eax,1; mov [ecx+0x20],eax; ret`. ptr_arg_int_get never fingerprinted the buffer, so the field
# mutants on the +0x20 store were BLIND. Scratch slot (0xEE fill) observed, return folded.
FIXES['rtfs_handler_cancel'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x60}], 'scratch_size': 0x60, 'scratch_fill': 0xEE,
    'struct_size': None,
    'path1_tests': [{'args': [S(0)]}, {'args': [S(4)]}, {'args': [S(0x10)]}, {'args': [S(0x3c)]}],
    'path2_tests': [{'args': [S(0)]}],
})


# ── 0x005514e0 RtFSHandler_IsEOF(int* slot) ──────────────────────────────────────────────────────────────────────────
# return 0 iff slot[2] != -1 && slot[3] <= slot[1] && (slot[3] != slot[1] || slot[2] < slot[0])  (signed compares).
# Scratch slot seeded with (sizeLow, sizeHigh, posLow, posHigh); vectors cover each branch and the signed edges.
def _eof(sl, sh, pl, ph):
    return {'args': [S(0)], 'seed': [_d(0, _u32(sl)), _d(4, _u32(sh)), _d(8, _u32(pl)), _d(0xc, _u32(ph))]}


FIXES['rtfshandler_is_eof'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x20}], 'scratch_size': 0x20, 'scratch_fill': 0xEE, 'struct_size': None,
    'path1_tests': [_eof(100, 0, 50, 0), _eof(100, 0, 100, 0), _eof(100, 0, -1, 0), _eof(100, 1, 5, 0),
                    _eof(100, 0, 5, 1), _eof(0x80000000, 0, 0x7fffffff, 0), _eof(5, 0, 0x80000000, 0),
                    _eof(0, 0, 0, -1), _eof(0, -1, 0, -2), _eof(7, 3, 6, 3)],
    'path2_tests': [_eof(100, 0, 50, 0)],
})


# ── 0x00552c10 FontSys_InitRenderState() ─────────────────────────────────────────────────────────────────────────────
# Stores 0x00912b04, 0x00912a84 (= FUN_004c57a0()), 0x00912be4/a50/bec/be0/bd8/bf0, 0x00912a44..a7c, the u16 table
# 0x00912500[0..255] = i, 0x00912a88[0..30] = 0, 0x00912c0c/bf8/bfc/c00/c04, 0x00913204/08; FUN_00552750 fills the
# matrix [0x00912a84]. Old: none handler, only the constant return 1 was compared, field mutants BLIND.
# FUN_004c57a0 allocates through [RwGlobals+0x118]([RwGlobals+[0x007d4028]], 0x3000d): DAT_007d3ff8 -> scratch fake
# whose +0x118 is G, and 0x00636ad8 = scratch+0x200 makes the "allocation" a scratch matrix (same address both arms).
def _font(mat):
    return {'args': [], 'seed': [_g(0x007d3ff8, S(0)), _d(0x118, G), _g(0x007d4028, 0x40), _g(0x00636ad8, S(mat))]}


FIXES['font_sys_init_render_state'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'observe': [{'addr': 0x00912500, 'len': 0x200, 'fill': 0xA5}, {'addr': 0x00912a44, 'len': 0x1cc, 'fill': 0xA5},
                {'addr': 0x00913204, 'len': 8, 'fill': 0xA5}, {'addr': S(0x200), 'len': 0x100}],
    'scratch_size': 0x300, 'scratch_fill': 0xA5, 'freeze_main': True,
    'path1_tests': [_font(0x200), _font(0x240), _font(0x280)],
    'path2_tests': [_font(0x200)],
})


# ── 0x005a9e40 AudioOutputNodeCbDispatch(node, p2) ──────────────────────────────────────────────────────────────────
# if [node+0x3c] { [node+0x3c](0, p2); return node } return 0. Old int_pair [0,0]: node 0, crash-equal, ret BLIND.
# Scratch node; +0x3c = P2 (records the forwarded p2), P1 (records the literal 0 first arg) or 0.
def _ocb(cb, p2):
    return {'args': [S(0), _u32(p2)], 'seed': [_d(0x3c, cb)]}


FIXES['audio_output_node_cb_dispatch'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x40}, _P1_OBS, _P2_OBS], 'scratch_size': 0x40, 'scratch_fill': 0xEE,
    'freeze_main': True,
    'path1_tests': [_ocb(P2, 0x12345678), _ocb(P2, 0xDEADBEEF), _ocb(P2, 0), _ocb(P1, 0x4444), _ocb(0, 0x5555),
                    _ocb(R3, 0x6666)],
    'path2_tests': [_ocb(P2, 0x12345678)],
})


# ── shared scratch tree for 0x005aa0c0 / 0x005ac900 ───────────────────────────────────────────────────────────────────
# node N: +0x00 key dword (M8D0), +0x04 visit-log slot (M8D0 writes parent[7] here), +0x10 child-list head (sentinel
# N+0x10), +0x18 sibling link (child = link-0x18), +0x1c M8D0 flag dword, +0x20 PCTX entry list (sentinel N+0x20),
# entry link at N+0x80 (entry-0x40 = N+0x40 holds the key pointer N+0xa0), 16-byte key at N+0xa0.
# Tree: R(0x000) -> [A(0x100), B(0x200)], A -> [C(0x300)], B -> [D(0x400)].
_N = {'R': 0x000, 'A': 0x100, 'B': 0x200, 'C': 0x300, 'D': 0x400}
_KIDS = {'R': ['A', 'B'], 'A': ['C'], 'B': ['D'], 'C': [], 'D': []}
_CTX, _QK, _OUT = 0x800, 0x840, 0x880


def _key(n):
    i = list(_N).index(n)
    return [(0x10 * (i + 1) + j) & 0xFF for j in range(16)]


def _tree(entries=('A', 'B', 'C', 'D'), m8_key=None, m8_flag=None):
    m8_key = m8_key or {}
    m8_flag = m8_flag or {}
    s = []
    for n, base in _N.items():
        kids = _KIDS[n]
        s.append(_d(base + 0x00, m8_key.get(n, 0x77)))
        s.append(_d(base + 0x04, 0))
        s.append(_d(base + 0x1c, m8_flag.get(n, 0x4 | (list(_N).index(n) << 8))))
        # child list
        if kids:
            s.append(_d(base + 0x10, S(_N[kids[0]] + 0x18)))
            for a, b in zip(kids, kids[1:] + [None]):
                s.append(_d(_N[a] + 0x18, S(_N[b] + 0x18) if b else S(base + 0x10)))
        else:
            s.append(_d(base + 0x10, S(base + 0x10)))
        # entry list
        if n in entries:
            s += [_d(base + 0x20, S(base + 0x80)), _d(base + 0x80, S(base + 0x20)), _d(base + 0x40, S(base + 0xa0))]
            s += _bytes(base + 0xa0, _key(n))
        else:
            s.append(_d(base + 0x20, S(base + 0x20)))
    # root's sibling link is never followed; give it a defined value
    s.append(_d(_N['R'] + 0x18, 0))
    return s


def _ctx(qkey_ptr, qbytes=None):
    s = [_d(_CTX, qkey_ptr), _d(_CTX + 4, 0x12121212), _d(_OUT, 0x34343434)]
    if qbytes is not None:
        s += _bytes(_QK, qbytes)
    return s


# ── 0x005aa0c0 AudioTreeWalkPredicateSearch(root, out, pred, user, selftest) ─────────────────────────────────────────
# Old `none` vectors passed nothing (DEGENERATE). Now driven over the scratch tree with three original predicates.
def _tw(root, out, pred, user, p5, tree, extra=()):
    return {'args': [root, out, pred, user, p5], 'seed': tree + list(extra)}


_TW_TESTS = [
    # PCTX, query by pointer equality with C's key pointer -> returns C, *out = A
    _tw(S(0), S(_OUT), PCTX, S(_CTX), 1, _tree(), _ctx(S(_N['C'] + 0xa0))),
    # PCTX, query by byte equality with D's key -> returns D, *out = B
    _tw(S(0), S(_OUT), PCTX, S(_CTX), 1, _tree(), _ctx(S(_QK), _key('D'))),
    # PCTX, no match -> full walk, return 0, *out = 0
    _tw(S(0), S(_OUT), PCTX, S(_CTX), 1, _tree(), _ctx(S(_QK), [0x99] * 16)),
    # PCTX, root has an entry matching, selftest 1 -> returns R, *out = R
    _tw(S(0), S(_OUT), PCTX, S(_CTX), 1, _tree(entries=('R', 'A', 'B', 'C', 'D')), _ctx(S(_QK), _key('R'))),
    # same, selftest 0 -> root not tested; no child carries R's key -> 0
    _tw(S(0), S(_OUT), PCTX, S(_CTX), 0, _tree(entries=('R', 'A', 'B', 'C', 'D')), _ctx(S(_QK), _key('R'))),
    # PCTX, out = NULL
    _tw(S(0), 0, PCTX, S(_CTX), 1, _tree(), _ctx(S(_N['B'] + 0xa0))),
    # M8D0 visit log, D's key differs -> D is the "match" (pred returns 1), *out = B; visited nodes carry [1]
    _tw(S(0), S(_OUT), M8D0, 0x1111, 1, _tree(m8_key={'D': 0x66}), ()),
    # M8D0 all keys equal and flags set -> full walk, every visited node logged, 0
    _tw(S(0), S(_OUT), M8D0, 0x1111, 1, _tree(), ()),
    # M8D0, A's flag lacks bit 2 -> when A is the parent of C, C is the match
    _tw(S(0), S(_OUT), M8D0, 0x1111, 1, _tree(m8_flag={'A': 0x300}), ()),
    # P1 predicate never returns 1 -> full walk; last parent recorded
    _tw(S(0), S(_OUT), P1, 0x2222, 1, _tree(), ()),
    # root NULL -> DAT_009146fc (seeded to the scratch root)
    _tw(0, S(_OUT), PCTX, S(_CTX), 1, _tree(), _ctx(S(_N['C'] + 0xa0)) + [_g(0x009146fc, S(0))]),
    # start at subtree B
    _tw(S(_N['B']), S(_OUT), PCTX, S(_CTX), 1, _tree(), _ctx(S(_QK), _key('D'))),
]

FIXES['audio_tree_walk_predicate_search'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer', 'pointer', 'uint32', 'int32']},
    'fold_ret': True, 'observe': [{'addr': S(0), 'len': 0x900}, _P1_OBS], 'scratch_size': 0x900,
    'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': _TW_TESTS, 'path2_tests': _TW_TESTS[:1],
})


# ── 0x005ac900 AudioContextLookup(key) ───────────────────────────────────────────────────────────────────────────────
# {key, 0} on the stack; FUN_005aa0c0(0, 0, PCTX, &ctx, 1); return ctx[1]. Root NULL -> DAT_009146fc, seeded to the
# scratch tree. PCTX writes ctx[1] (the 4 bytes after the key slot) = matching entry-0x40.
def _cl(keyptr, qbytes=None, entries=('A', 'B', 'C', 'D')):
    s = _tree(entries=entries) + [_g(0x009146fc, S(0))]
    if qbytes is not None:
        s += _bytes(_QK, qbytes)
    return {'args': [keyptr], 'seed': s}


FIXES['audio_context_lookup'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x900}], 'scratch_size': 0x900, 'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': [_cl(S(_N['C'] + 0xa0)), _cl(S(_QK), _key('D')), _cl(S(_QK), _key('A')), _cl(S(_QK), [0x99] * 16),
                    _cl(S(_QK), _key('R'), ('R', 'A', 'B', 'C', 'D')), _cl(S(_N['B'] + 0xa0))],
    'path2_tests': [_cl(S(_N['C'] + 0xa0))],
})


# ── 0x005aa8a0 AudioListSearch(key) ──────────────────────────────────────────────────────────────────────────────────
# {key, 0}; FUN_005aaac0(0x005aa8d0, &ctx) walks the list at 0x007dccf0 (sentinel 0x007dccf0) calling
# 0x005aa8d0(link-4, &ctx): node key [link-4], field [link+0x18]; first node with key match and field&4 sets
# ctx[1] = field and stops. Old: live list, return 0 every time (ret BLIND). Seed 0x007dccf0 -> scratch list.
def _ls(key, nodes):
    s = []
    links = [0x10 + 0x40 * i + 4 for i in range(len(nodes))]
    s.append(_g(0x007dccf0, S(links[0]) if nodes else 0x007dccf0))
    for i, (k, f) in enumerate(nodes):
        L = links[i]
        s += [_d(L - 4, k), _d(L, S(links[i + 1]) if i + 1 < len(nodes) else 0x007dccf0), _d(L + 0x18, f)]
    return {'args': [_u32(key)], 'seed': s}


_LSN = [(0x111, 0x105), (0x222, 0x203), (0x222, 0x4444), (0x333, 0x8)]
FIXES['audio_list_search'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x100}], 'scratch_size': 0x100, 'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': [_ls(0x111, _LSN), _ls(0x222, _LSN), _ls(0x333, _LSN), _ls(0x444, _LSN), _ls(0, _LSN),
                    _ls(0x111, [])],
    'path2_tests': [_ls(0x111, _LSN)],
})


# ── RW memory stream holding RWS chunk headers (0x005ab380 / 0x005ab410) ─────────────────────────────────────────────
# stream at scratch 0 ([0]=3, [3]=pos, [4]=size, [5]=data scratch+0x100). Chunks (type, size, version, payload):
#   +0x00 (0x1,   4, 0x00000360, 4 bytes)     old format -> 0x36000
#   +0x10 (0x80d, 8, 0x1C02FFFF, 8 bytes)     new format -> 0x37002 (in range, edge)
#   +0x24 (0x80e, 0, 0x0000034f)              old format -> 0x34f00 (out of range)
#   +0x30 (0x80f, 0, 0x1C03FFFF)              new format -> 0x37003 (out of range, edge)
#   +0x3c (0x810, 0, 0x00000350)              old format -> 0x35000 (in range, edge)
_CH = [0x1, 4, 0x360, 0xA1A2A3A4,
       0x80d, 8, 0x1C02FFFF, 0xB1B2B3B4, 0xB5B6B7B8,
       0x80e, 0, 0x34f,
       0x80f, 0, 0x1C03FFFF,
       0x810, 0, 0x350]


def _stream(pos, size=0x48):
    s = [_d(0, 3), _d(0xc, pos), _d(0x10, size), _d(0x14, S(0x100))]
    s += [_d(0x100 + 4 * i, v) for i, v in enumerate(_CH)]
    return s


def _hr(pos, size=0x48):
    return {'args': [S(0), S(0x80)], 'seed': _stream(pos, size)}


FIXES['audio_rws_chunk_header_read'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x160}], 'scratch_size': 0x160, 'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': [_hr(0), _hr(0x10), _hr(0x24), _hr(0x30), _hr(0x3c), _hr(0x40), _hr(4)],
    'path2_tests': [_hr(0)],
})


def _ts(typ, p3=S(0x90), p4=S(0x94), pos=0, size=0x48):
    return {'args': [S(0), _u32(typ), p3, p4], 'seed': _stream(pos, size)}


FIXES['audio_rws_chunk_type_seek'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer', 'int32', 'pointer', 'pointer']},
    'fold_ret': True, 'observe': [{'addr': S(0), 'len': 0x160}], 'scratch_size': 0x160, 'scratch_fill': 0xEE,
    'freeze_main': True,
    'path1_tests': [_ts(0x1), _ts(0x80d), _ts(0x80e), _ts(0x80f), _ts(0x810), _ts(0x999), _ts(0x80d, 0, S(0x94)),
                    _ts(0x80d, S(0x90), 0), _ts(0x810, pos=0x10), _ts(0x80d, size=0x20)],
    'path2_tests': [_ts(0x80d)],
})


# ── 0x005abf80 AudioWaveVtableSlot1cDispatch(node) ───────────────────────────────────────────────────────────────────
# `mov eax,[esp+4]; mov ecx,[eax+0xc]; mov eax,[ecx+0x1c]; test eax,eax; jne -> jmp eax; ret`: tail jump, the slot
# sees node as its [esp+4] and its EAX is returned; slot 0 returns 0. Old int_scalar node 0: crash-equal.
def _s1c(slot, gval=0):
    return {'args': [S(0)], 'seed': [_d(0xc, S(0x40)), _d(0x40 + 0x1c, slot), _g(0x00636ad8, gval)]}


FIXES['audio_wave_vtable_slot_1c_dispatch'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'fold_ret': True,
    'observe': [{'addr': S(0), 'len': 0x80}, _P1_OBS], 'scratch_size': 0x80, 'scratch_fill': 0xEE,
    'freeze_main': True,
    'path1_tests': [_s1c(P1), _s1c(0), _s1c(G, 0x13572468), _s1c(G, 0)],
    'path2_tests': [_s1c(P1)],
})


# ── 0x005abcb0 AudioWaveNodeFree(node) ───────────────────────────────────────────────────────────────────────────────
# flags [node+0x54]: bit0 -> FUN_005ae920(pool, node) with pool = [[node+0xc]+0x3c] or 0x007dd634 when 0;
# else bit2 -> nothing; else jmp RwFree (0x004522d0) with node as [esp+4]. Old int_scalar node 0: crash-equal.
# Scratch pool P (0x300): [0] elemsize 0x60, [1] count 16, [2] bitmap bytes 4, [3] 4, [4]/[5] = block B, [6] flags 0.
# Block B (0x400): [0]/[1] = P+0x10, bitmap 0x408..0x40b = 0xFF, data from 0x40c. A node at data + i*0x60 is freed by
# clearing bit (0x80 >> i%8) of byte i/8. Fake RwGlobals (scratch 0) +0x10c = P1 records the RwFree argument.
_PL, _BL = 0x300, 0x400


def _pool_seeds(pool_addr_seed, sentinel):
    s = [_d(_BL, sentinel), _d(_BL + 4, sentinel), _d(_BL + 8, 0xFFFFFFFF)]
    hdr = [0x60, 16, 4, 4, S(_BL), S(_BL), 0]
    s += [pool_addr_seed(4 * i, v) for i, v in enumerate(hdr)]
    return s


def _nf(idx, flags, perctx=True, default_pool=False):
    node = _BL + 0xc + idx * 0x60
    s = [_g(0x007d3ff8, S(0)), _d(0x10c, P1), _d(node + 0x54, flags), _d(node + 0xc, S(0x200)),
         _d(0x200 + 0x3c, S(_PL) if perctx else 0)]
    if default_pool:
        s += _pool_seeds(lambda o, v: _g(0x007dd634 + o, v), 0x007dd634 + 0x10)
    else:
        s += _pool_seeds(lambda o, v: _d(_PL + o, v), S(_PL + 0x10))
    return {'args': [S(node)], 'seed': s}


FIXES['audio_wave_node_free'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': S(0x200), 'len': 0x800}, _P1_OBS], 'scratch_size': 0xa00, 'scratch_fill': 0xEE,
    'freeze_main': True,
    'path1_tests': [_nf(1, 1), _nf(9, 1), _nf(2, 5), _nf(3, 4), _nf(4, 0), _nf(5, 2), _nf(6, 1, False, True)],
    'path2_tests': [_nf(1, 1)],
})


# ── 0x005ac740 AudioSubStructBufCleanup(s) ───────────────────────────────────────────────────────────────────────────
# if (!(byte[s+0x18] & 2) && [s+0x10]) RwFree([s+0x10]); [s+0x10] = 0; [s+0x14] = 0. Old int_scalar s = 0.
def _bc(buf, flags):
    return {'args': [S(0x40)], 'seed': [_g(0x007d3ff8, S(0x100)), {'addr': S(0x100 + 0x10c), 'val': P1},
                                        _d(0x40 + 0x10, _u32(buf)), _d(0x40 + 0x14, 0x5A5A5A5A),
                                        _d(0x40 + 0x18, flags)]}


FIXES['audio_sub_struct_buf_cleanup'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer']},
    'observe': [{'addr': S(0x40), 'len': 0x40}, _P1_OBS], 'scratch_size': 0x220, 'scratch_fill': 0xEE,
    'freeze_main': True,
    'path1_tests': [_bc(0xCAFE0000, 0), _bc(0xCAFE0010, 2), _bc(0, 0), _bc(0xCAFE0020, 0xFD), _bc(0xCAFE0030, 0x102)],
    'path2_tests': [_bc(0xCAFE0000, 0)],
})


# ── 0x005addd0 AudioListInsertHead(head, payload) ────────────────────────────────────────────────────────────────────
# n = FUN_005ae800(0x009146c0, 0x30804) (bitmap pool); if n { n[2] = payload; n[1] = head; n[0] = *head;
# [*head+4] = n; *head = n }. Old `none` vectors: nothing observed. The pool header 0x009146c0 is seeded to one
# scratch block B (0x100): elemsize 0x10, count 8, bitmap 1 byte, align 4; a free bit exists in every vector so no
# new block is allocated. The node comes back at ((B+1+4+7)&~3) + bit*0x10, inside the observed scratch.
def _ih(payload, bitmap):
    s = [_g(0x009146c0 + 4 * i, v) for i, v in enumerate([0x10, 8, 1, 4, S(0x100), S(0x100), 0])]
    s += [_d(0x100, 0x009146d0), _d(0x104, 0x009146d0), {'addr': S(0x108), 'val': bitmap, 'size': 1},
          _d(0, S(0x40)), _d(0x40, 0x0BADF00D), _d(0x44, 0x22222222)]
    return {'args': [S(0), _u32(payload)], 'seed': s}


FIXES['audio_list_insert_head'] = dict(_CLR, **{
    'arg_type': _SCAL, 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']},
    'observe': [{'addr': S(0), 'len': 0x200}, {'addr': 0x009146c0, 'len': 0x1c}], 'scratch_size': 0x200,
    'scratch_fill': 0xEE, 'freeze_main': True,
    'path1_tests': [_ih(0x12345678, 0x80), _ih(0xDEADBEEF, 0xF0), _ih(0x1, 0x00), _ih(0x7, 0xFE)],
    'path2_tests': [_ih(0x12345678, 0x80)],
})
