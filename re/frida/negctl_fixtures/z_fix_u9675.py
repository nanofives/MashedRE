"""U-9675 (2026-10-12): fixture for the re-port of 0x0041bc50 FUN_0041bc50 (HUD/HudDispatch.cpp since 97e1560a). Key hud_rec29_dispatch.

Bytes (original/MASHED.exe.unpatched, capstone and Ghidra pool decomp agree):
  0x0041bc50 push esi / 0x0041bc51 mov esi,eax                                  ; record in EAX
  29 pairs, first: 0x0041bc53 mov eax,[esi+0x1c] / test eax,eax / je 0x0041bc67
                   0x0041bc5a mov eax,[esi+0xbc] / push eax / 0x0041bc61 call [eax+0x48] / 0x0041bc64 add esp,4
  last:            0x0041be95 mov eax,[esi+0x38] / test / je 0x0041bea9
                   0x0041be9c mov esi,[esi+0xd8] / push esi / 0x0041bea3 call [esi+0x48] / add esp,4
  0x0041bea9 pop esi / 0x0041beaa ret                                            ; EAX = last pair's EAX
Flag offsets X in byte order are _FLAGS below; the object is always [rec+X+0xa0]. 0x00..0x0c and 0x54..0x64 are never
read.

Harness (scalars_to_scattered_globals, eax_arg 1, freeze_main, fold_ret):
  * args = [decoy record, record]. The trampoline is `mov eax,[esp+8]; jmp target` on both arms, so EAX = the record and
    the stack slot [esp+4] = the DECOY record. A port that reads its record from the stack instead of EAX dispatches
    the decoy's objects (all 29 flags on, disjoint objects) and records a different log instead of faulting.
  * scratch+0x000: the `recorder` log [count, o, o, ...] (1 arg, mscdecl: 0x0041bc64 add esp,4, caller cleans). It is
    the dispatch ORDER and the object set. The recorder returns 0, so the folded EAX is 0 on the original in every
    vector (skip path: the zero flag; taken path: the recorder's 0).
  * scratch+0x200 / +0x400: scratch records. Two vectors use the live 0x0041c0c0 records 0x0063cab8 / 0x0063cc24
    (0x0041c0c1, stride 0x16c at 0x0041c0cd), seeded and restored.
  * scratch+0x600 + k*0x50: fake objects; only [o+0x48] = recorder is seeded. [o+0] stays 0 (scratch fill 0), so a
    port that reads a vtable at [o] and calls [vtbl+0x48] faults instead of matching.
  * Every vector also sets the 9 unread flag dwords (0x00..0x0c, 0x54..0x64) non-zero with recording objects at
    X+0xa0, so a port that reads one of them records an extra entry.
"""

_LOG = 0x000
_MAXC = 64
_R0, _R1 = 0x200, 0x400
_OBJ = 0x600
_OSTR = 0x50
_NOBJ = 80
_LIVE0, _LIVE1 = 0x0063cab8, 0x0063cab8 + 0x16c

_FLAGS = [0x1c, 0x18, 0x10, 0x14, 0x20, 0x80, 0x84, 0x88, 0x8c, 0x90, 0x94, 0x68, 0x6c, 0x70, 0x74, 0x78, 0x7c,
          0x3c, 0x40, 0x44, 0x48, 0x4c, 0x50, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38]
_GAPS = [0x00, 0x04, 0x08, 0x0c, 0x54, 0x58, 0x5c, 0x60, 0x64]
_ALL = (1 << 29) - 1


def _obj(k):
    return {'scratch': _OBJ + k * _OSTR}


def _a(base, off):
    return base + off if base >= 0x1000 else {'scratch': base + off}


def _rec(base, r, mask, fval=1, alias=None):
    """Seed one record: flag i on per mask bit i, object id r*40+i (or `alias` for every pair); gaps always on."""
    s, used = [], []
    for i, x in enumerate(_FLAGS):
        on = (mask >> i) & 1
        s.append({'addr': _a(base, x), 'val': fval if on else 0})
        k = alias if alias is not None else r * 40 + i
        s.append({'addr': _a(base, x + 0xa0), 'val': _obj(k)})
        if on:
            used.append(k)
    for j, x in enumerate(_GAPS):
        k = r * 40 + 29 + j
        s.append({'addr': _a(base, x), 'val': 1})
        s.append({'addr': _a(base, x + 0xa0), 'val': _obj(k)})
        used.append(k)
    return s, used


def _vec(rec, mask, decoy, fval=1, alias=None):
    s, used = _rec(rec, 0, mask, fval, alias)
    s2, used2 = _rec(decoy, 1, _ALL)
    s += s2
    used += used2
    s += [{'addr': {'scratch': _OBJ + k * _OSTR + 0x48}, 'val': {'recorder': True}} for k in sorted(set(used))]

    def arg(b):
        return {'scratch': b} if b < 0x1000 else b
    return {'args': [arg(decoy), arg(rec)], 'seed': s}


_TESTS = [
    _vec(_R0, _ALL, _R1),                          # all 29 pairs, byte order
    _vec(_R0, 0, _R1),                             # no pair: EAX = the 0x38 flag (0), log empty
    _vec(_R1, 0x15555555, _R0),                    # alternate pairs, record moved
    _vec(_R0, 0x0aaaaaaa, _R1),
    _vec(_R1, 1 << 28, _R0),                       # last pair only (0x38 -> 0xd8, the ESI-loaded pair)
    _vec(_R0, (1 << 10) | 1, _R1),                 # 0x94 (the pair the old port lacked) + first pair 0x1c
    _vec(_R1, 0x0f0f0f0f & _ALL, _R0, fval=0x80000000),   # sign bit only: test eax,eax is a dword test
    _vec(_R0, _ALL, _R1, fval=0x100),              # low byte 0: a byte test would skip every pair
    _vec(_R1, _ALL, _R0, alias=7),                 # every pair the same object
    _vec(_LIVE0, 0x1f0f00f1, _LIVE1),              # live 0x0041c0c0 records
    _vec(_LIVE1, 0x00f0ff0e, _LIVE0),
]

FIXES = {
    'hud_rec29_dispatch': {
        '__replace__': True,
        'rva': 0x0041bc50,
        'export': 'FUN_0041bc50',
        'arg_type': 'scalars_to_scattered_globals',
        'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'eax_arg': 1,
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
