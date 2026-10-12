"""U-9671 follow-up (2026-10-12): fixture for the re-port of 0x0041c2d0 FUN_0041c2d0 (HUD/HudDispatch.cpp).

Bytes (original/MASHED.exe.unpatched, capstone and Ghidra pool decomp agree):
  0x0041c2d0 push esi / 0x0041c2d1 mov esi,eax                        ; block in EAX
  0x0041c2d3 mov eax,[esi+0xc] / push eax / 0x0041c2d7 call [eax+0x48]  ; m = block[3]; m->[+0x48](m)
  0x0041c2da mov eax,[esi+4]   / push eax / 0x0041c2de call [eax+0x48]  ; block[1]
  0x0041c2e1 mov eax,[esi]     / push eax / 0x0041c2e4 call [eax+0x48]  ; block[0]
  0x0041c2e7 mov esi,[esi+8]   / push esi / 0x0041c2eb call [esi+0x48]  ; block[2]
  0x0041c2ee add esp,0x10 / 0x0041c2f1 pop esi / 0x0041c2f2 ret          ; EAX = last call's EAX

Why the old entry measured nothing: `arg_type: none`, crash_equal_ok, EAX never set. Both arms faulted at the first
dereference, so the old port (vtable deref, no arg pops, EAX reused after the callee clobbers it) read "GREEN".

Harness (scalars_to_scattered_globals, eax_arg 0, freeze_main):
  * arg 0 = the block pointer, delivered in EAX on both arms (`mov eax,[esp+4]; jmp target`).
  * scratch+0x000: the `recorder` log [count, m, m, ...] (1 arg, mscdecl: 0x0041c2ee add esp,0x10, caller cleans).
    The log is the dispatch ORDER and the member set. The recorder returns 0, so the folded EAX is 0 on the original.
  * scratch+0x100 / +0x140 / +0x180: candidate blocks (4 dwords each). Every vector seeds a DECOY block too, with
    disjoint members that also record, so a port that ignores EAX records a different log instead of faulting.
  * scratch+0x200 + k*0x50: fake members; only [m+0x48] = recorder is seeded. [m+0] stays 0 (scratch_fill 0), so a
    port that reads a vtable at [m] and calls [vtbl+0x48] faults on both boots instead of matching.
  * Two vectors pass the live mode blocks 0x0063c610 (0x0041a3e9) and 0x0063cda4 (0x0041c309), seeded and restored.
"""

_LOG = 0x000
_MAXC = 16
_OBJ = 0x200
_OSTR = 0x50
_NOBJ = 24
_B0, _B1, _B2 = 0x100, 0x140, 0x180
_M10_BLOCK = 0x0063c610
_M5_BLOCK = 0x0063cda4


def _obj(k):
    return {'scratch': _OBJ + k * _OSTR}


def _blk(addr):
    return {'scratch': addr} if addr < 0x1000 else addr


def _vec(block, objs, decoy=None, decoy_objs=(20, 21, 22, 23)):
    s = []
    for i in range(4):
        a = block + 4 * i if block >= 0x1000 else {'scratch': block + 4 * i}
        s.append({'addr': a, 'val': _obj(objs[i])})
    used = list(objs)
    if decoy is not None:
        for i in range(4):
            a = decoy + 4 * i if decoy >= 0x1000 else {'scratch': decoy + 4 * i}
            s.append({'addr': a, 'val': _obj(decoy_objs[i])})
        used += list(decoy_objs)
    s += [{'addr': {'scratch': _OBJ + k * _OSTR + 0x48}, 'val': {'recorder': True}} for k in sorted(set(used))]
    return {'args': [_blk(block)], 'seed': s}


_TESTS = [
    _vec(_B0, (0, 1, 2, 3), _B1),             # order 3,1,0,2
    _vec(_B0, (4, 5, 6, 7), _B1),
    _vec(_B0, (3, 2, 1, 0), _B2),
    _vec(_B1, (1, 3, 0, 2), _B0),             # block moved: EAX must be read
    _vec(_B2, (2, 2, 9, 9), _B0),             # aliased members
    _vec(_B1, (5, 5, 5, 5), _B2),
    _vec(_B2, (6, 0, 6, 0), _B1),
    _vec(_M10_BLOCK, (8, 9, 10, 11), _M5_BLOCK, (12, 13, 14, 15)),   # live mode-10 block
    _vec(_M5_BLOCK, (12, 13, 14, 15), _M10_BLOCK, (8, 9, 10, 11)),   # live mode-5 block
    _vec(_B0, (16, 17, 18, 19), _M10_BLOCK, (8, 9, 10, 11)),
]

FIXES = {
    'hud_dispatch_draw4': {
        '__replace__': True,
        'rva': 0x0041c2d0,
        'export': 'FUN_0041c2d0',
        'arg_type': 'scalars_to_scattered_globals',
        'signature': {'ret': 'uint32', 'args': ['uint32']},
        'eax_arg': 0,
        'freeze_main': True,
        'fold_ret': True,
        'lut_root_delta': 0,
        'scratch_size': _OBJ + _OSTR * _NOBJ,
        'recorder': {'args': 1, 'abi': 'mscdecl', 'off': _LOG, 'max_calls': _MAXC},
        'observe': [{'addr': {'scratch': _LOG}, 'len': 4 + 4 * _MAXC}],
        'path1_tests': _TESTS,
        'path2_tests': _TESTS[0:1],
    },
}
