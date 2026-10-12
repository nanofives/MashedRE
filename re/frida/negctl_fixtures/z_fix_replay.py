"""negctl re-earn fix, cluster replay (2026-10-12). Loads last; overrides b06 entries by hook key.

replay_save: b06 left out the inner-body vector (_rs(0, 1, 0)) because the old port died on it (the RwGlobals +0xc4
slot called __thiscall, ESP off by 0x20 at the /GS check). The re-port calls it __cdecl like 0x004117b0 (0x00411843
add esp,0x30), so the inner body goes back in. Seeds and observes are b06's; the record contents vary so
FUN_00483ca0's copy into the *DAT_008a94a8 scratch table differs between vectors.

replay_record_frame: b06 seeded DAT_0063bb10 = 0 to keep the 0x004987b0 logger branch out. One extra vector sets
DAT_0063bb10 == DAT_0063bb14 (0x00411616 cmp/jne) so the re-ported logger call runs; one sets both checkpoints.

Both bodies reach the logger 0x004987b0, which ends in OutputDebugStringA (IAT 0x005cc1b0, read-only .rdata). Under
Frida its debug-print exception (0x40010006) fails BOTH arms with 'system error' (first run of this fixture: replay_save
idx 0/4 and replay_record_frame idx 4, original-vs-original included). `stub_at` (scalars_to_scattered_globals,
diff_template.js, added 2026-10-12) replaces 0x004987b0 with the handler's `recorder`, which stores the call count and
each call's first argument (the format-string address, 0x005ccf00 / 0x005ccedc) in the scratch, so the logger call is
itself observed.
"""
import struct


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

_SC = {'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'target_global': None,
       'needs_booted_race': None, 'stub_at': None, 'stub_nargs': None, 'stub_ret': None}


def _fix(**kw):
    d = dict(_SC)
    d.update(kw)
    return d


# ── 0x004117b0 ReplaySave (seeds as b06 _rs, plus a record-content variant) ─────────────────────────────────────────
def _rs(latch, bb10, ed6c, tag=0):
    s = [{'addr': 0x005f29c8, 'val': latch}, {'addr': 0x0063bb10, 'val': {'scratch': 0} if bb10 else 0},
         {'addr': 0x0067ed6c, 'val': ed6c}, {'addr': 0x008a94a8, 'val': {'scratch': 0x200}},
         {'addr': 0x008a95ac, 'val': 0}, {'addr': 0x00644158, 'val': 7},
         {'addr': {'scratch': 0x00}, 'val': 0x11111111 ^ tag}, {'addr': {'scratch': 0x08}, 'val': 0x22222222 ^ tag},
         {'addr': {'scratch': 0x100}, 'val': 0x33333333 ^ tag}, {'addr': {'scratch': 0x198}, 'val': 0x44444444 ^ tag}]
    return {'args': [], 'seed': s}


_LOG_STUB = [{'addr': 0x004987b0, 'nargs': 1, 'abi': 'mscdecl', 'ret': 0}]

FIXES['replay_save'] = _fix(
    signature={'ret': 'void', 'args': []}, scratch_size=0x400,
    stub_at=_LOG_STUB, recorder={'args': 1, 'abi': 'mscdecl', 'off': 0x3c0, 'max_calls': 4},
    observe=[{'addr': 0x005f29c8, 'fill': 0xA5}, {'addr': {'scratch': 0x200}, 'len': 0x1a0},
             {'addr': {'scratch': 0x3c0}, 'len': 0x14}],
    path1_tests=[_rs(0, 1, 0), _rs(0, 0, 0), _rs(1, 1, 0), _rs(0, 1, 1), _rs(0, 1, 0, 0x0F0F0F0F)],
    path2_tests=[_rs(0, 1, 0)])


# ── 0x00411600 ReplayRecordFrame (seeds as b06 _rf; bb10 selectable) ───────────────────────────────────────────────
_E9FC = 0x0067e9fc


def _rf(p1, e9fc=2, ck0=0, ck1=0, same=False):
    s = [{'addr': _E9FC, 'val': e9fc}, {'addr': 0x0063bb10, 'val': {'scratch': 0} if same else 0},
         {'addr': 0x0063bb14, 'val': {'scratch': 0}},
         {'addr': 0x00881f48, 'val': 0},
         {'addr': 0x00881ec8 + 0x30, 'val': _f(1.5)}, {'addr': 0x00881ec8 + 0x34, 'val': _f(-2.25)},
         {'addr': 0x00881ec8 + 0x38, 'val': _f(7.0)},
         {'addr': 0x008a9914, 'val': ck0}, {'addr': 0x008a9918, 'val': ck1},
         {'addr': {'scratch': 0x1c}, 'val': {'scratch': 0x200}}, {'addr': {'scratch': 0x21c}, 'val': {'scratch': 0x300}}]
    return {'args': [p1], 'seed': s}


FIXES['replay_record_frame'] = _fix(
    signature={'ret': 'void', 'args': ['int32']}, scratch_size=0x480,
    stub_at=_LOG_STUB, recorder={'args': 1, 'abi': 'mscdecl', 'off': 0x400, 'max_calls': 4},
    observe=[{'addr': {'scratch': 0}, 'len': 0x400}, {'addr': 0x0063bb20, 'fill': 0xA5},
             {'addr': {'scratch': 0x400}, 'len': 0x14}],
    path1_tests=[_rf(100), _rf(250, ck0=1), _rf(77, ck1=1), _rf(5, e9fc=3), _rf(4321, same=True),
                 _rf(3600, ck0=1, ck1=1)],
    path2_tests=[_rf(100)])
