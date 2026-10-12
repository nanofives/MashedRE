"""negctl re-earn, fix/hud-dispatch (2026-10-12). Loads after b0x/z_fix_hud, so these keys override the old entries.

Rows ported from the bytes in this branch (HUD/HudDispatch.cpp): 0x0041a3e0, 0x0041c0c0, 0x0041c300, 0x00426ba0.

Why the old vectors measured nothing: all four were `arg_type: none` (or `read_global`) entries for exports that did
not exist (negctl 2026-10-11: "export not found in .asi"). Even with an export, a `none` call at the quiescent menu
only exercises the early-outs: DAT_0063c628 / DAT_0063cdbc are 0 there, and 0x0041c0c0's records live in HUD state
the menu never fills, so the observable was a constant.

Harness (scalars_to_scattered_globals, freeze_main):
  * scratch+0x000: the `recorder` log (1 arg, cdecl: 0x0041c2ee add esp,0x10 / 0x0041bc64 add esp,4, caller cleans)
    = [count, obj0, obj1, ...]. Each callee hands the recorder its object pointer, so the log is the dispatch ORDER
    and the object set.
  * scratch+0x200 + k*0x50: fake objects; only [obj+0x48] is read (0x0041c2d7 call [eax+0x48], 0x0041bc61 the same),
    seeded to the recorder.
  * 0x0041c2d0 and 0x0041bc50 run for real (their original bytes, C2, not installed); the fixture drives them past
    every guard.
"""

_SC = {'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'target_global': None,
       'crash_equal_ok': None, 'field_offset': None, 'is_getter': None, 'fold_ret': True,
       'signature': {'ret': 'uint32', 'args': []}}

_LOG = 0x000
_MAXC = 64
_OBJ = 0x200
_OSTR = 0x50


def _fix(**kw):
    d = dict(_SC)
    d.update(kw)
    return d


def _obj(k):
    return {'scratch': _OBJ + k * _OSTR}


def _objseed(ks):
    return [{'addr': {'scratch': _OBJ + k * _OSTR + 0x48}, 'val': {'recorder': True}} for k in sorted(set(ks))]


_REC = {'args': 1, 'abi': 'mscdecl', 'off': _LOG, 'max_calls': _MAXC}
_OBS_LOG = [{'addr': {'scratch': _LOG}, 'len': 4 + 4 * _MAXC}]
_SCR = _OBJ + _OSTR * 64

FIXES = {}

# ── 0x0041a3e0 HudDispatchMode10 / 0x0041c300 HudDispatchMode5 ───────────────────────────────────────────────────────
# Bytes: mov eax,[flag]; test eax,eax; je ret; mov eax,block; jmp 0x0041c2d0.
#   mode10: flag 0x0063c628 (0x0041a3e0), block 0x0063c610 (0x0041a3e9)
#   mode5 : flag 0x0063cdbc (0x0041c300), block 0x0063cda4 (0x0041c309)
# 0x0041c2d0 calls [m+0x48](m) for m = block[3], block[1], block[0], block[2] (0x0041c2d3 / 0x0041c2da / 0x0041c2e1 /
# 0x0041c2e7). BOTH blocks and BOTH flags are seeded in every vector, with disjoint objects, so a port that reads the
# other row's flag or hands 0x0041c2d0 the other row's block records a different log instead of faulting.
_M10 = (0x0063c628, 0x0063c610)
_M5 = (0x0063cdbc, 0x0063cda4)


def _mode(me, other, flag, other_flag, objs, other_objs=(10, 11, 12, 13)):
    s = [{'addr': me[0], 'val': flag}, {'addr': other[0], 'val': other_flag}]
    for i in range(4):
        s.append({'addr': me[1] + 4 * i, 'val': _obj(objs[i])})
        s.append({'addr': other[1] + 4 * i, 'val': _obj(other_objs[i])})
    s += _objseed(list(objs) + list(other_objs))
    return {'args': [], 'seed': s}


def _mode_tests(me, other):
    return [
        _mode(me, other, 0, 1, (0, 1, 2, 3)),                     # 0x...e7 je taken: no call, EAX = 0
        _mode(me, other, 1, 0, (0, 1, 2, 3)),                     # order 3,1,0,2
        _mode(me, other, 0xdeadbeef, 0, (4, 5, 6, 7)),
        _mode(me, other, 0x80000000, 1, (7, 6, 5, 4)),            # sign bit only: test eax,eax is a dword test
        _mode(me, other, 0x00000100, 0, (1, 3, 0, 2)),            # low byte 0: a byte test would skip
        _mode(me, other, 0xffffffff, 1, (2, 2, 9, 9)),            # aliased members
        _mode(me, other, 0, 0, (0, 1, 2, 3)),
        _mode(me, other, 1, 1, (8, 0, 9, 1), (14, 15, 16, 17)),
    ]


FIXES['hud_dispatch_mode10'] = _fix(
    scratch_size=_SCR, recorder=_REC, observe=_OBS_LOG,
    path1_tests=_mode_tests(_M10, _M5), path2_tests=_mode_tests(_M10, _M5)[1:2])

FIXES['hud_dispatch_mode5'] = _fix(
    scratch_size=_SCR, recorder=_REC, observe=_OBS_LOG,
    path1_tests=_mode_tests(_M5, _M10), path2_tests=_mode_tests(_M5, _M10)[1:2])

# ── 0x0041c0c0 HudDispatchSlot2 ──────────────────────────────────────────────────────────────────────────────────────
# Bytes: esi = 0x0063cab8 (0x0041c0c1); loop { eax = esi; call 0x0041bc50; esi += 0x16c } while esi < 0x0063cd90 (jl,
# 0x0041c0d9): records 0x0063cab8 and 0x0063cc24. 0x0041bc50 (mov esi,eax at 0x0041bc51) tests 29 flag dwords at rec+X
# and, when non-zero, calls [o+0x48](o) with o = [rec+X+0xa0]. Order of X (0x0041bc53 .. 0x0041be95):
_BC50_FLAGS = [0x1c, 0x18, 0x10, 0x14, 0x20, 0x80, 0x84, 0x88, 0x8c, 0x90, 0x94, 0x68, 0x6c, 0x70, 0x74, 0x78, 0x7c,
               0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38]
_RECS = (0x0063cab8, 0x0063cab8 + 0x16c)
_ALL = (1 << 29) - 1


def _slot2(m0, m1, fval=1):
    s, objs = [], []
    for r, (base, mask) in enumerate(zip(_RECS, (m0, m1))):
        for i, x in enumerate(_BC50_FLAGS):
            on = (mask >> i) & 1
            s.append({'addr': base + x, 'val': fval if on else 0})
            if on:
                k = r * 29 + i
                s.append({'addr': base + x + 0xa0, 'val': _obj(k)})
                objs.append(k)
    s += _objseed(objs)
    return {'args': [], 'seed': s}


FIXES['hud_dispatch_slot2'] = _fix(
    scratch_size=_SCR, recorder=_REC, observe=_OBS_LOG,
    path1_tests=[_slot2(0, 0), _slot2(_ALL, _ALL), _slot2(_ALL, 0), _slot2(0, _ALL),
                 _slot2(0x15555555, 0x0aaaaaaa), _slot2(1 << 28, 1), _slot2(1, 1 << 28),
                 _slot2(0x0f0f0f0f & _ALL, 0x10f0f0f0 & _ALL, fval=0x80000000),
                 _slot2(_ALL, _ALL, fval=0x100)],
    path2_tests=[_slot2(_ALL, _ALL)])

# ── 0x00426ba0 HudDrawEnabled ────────────────────────────────────────────────────────────────────────────────────────
# Bytes: 0x00426ba0 mov eax,[0x0066d704]; ret. Seed the global, fold EAX, observe the global (unchanged).
FIXES['hud_draw_enabled'] = _fix(
    observe=[{'addr': 0x0066d704}],
    path1_tests=[{'args': [], 'seed': [{'addr': 0x0066d704, 'val': v}]}
                 for v in (0, 1, 0xdeadbeef, 0xcafebabe, 0x12345678, 0xffffffff, 0x80000000, 0x55555555,
                           0xaaaaaaaa, 0x00000100)],
    path2_tests=[{'args': [], 'seed': [{'addr': 0x0066d704, 'val': 1}]}])
