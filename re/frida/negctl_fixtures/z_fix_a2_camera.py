"""negctl follow-up 2026-10-12, child a2-camera: 0x004464c0 CameraEntryDispatchAll.

Overrides the race.py fixture (files apply in sorted order). Same observe window (the whole entry array plus the count,
0x008964c0..0x00898997), same frozen game thread, same prep call (the ORIGINAL dispatcher once before each arm), and
two additions:
- the return is folded (sig uint32). EAX at RET is the count read at 0x004464c0 (count <= 0, JLE 0x0044651a) or
  re-read at 0x00446508; the old void fixture left the ret mutant SKIP.
- the count DAT_00898994 is seeded per test (restored after each arm), so the loop bound is exercised: -1 and 0 take the
  JLE at 0x004464ca, 1 runs entry 0 only (type 0 -> 0x00445aa0), 4 adds the three type-1 entries (0x00441d40), 6 adds
  the two type-2 entries (0x00442440), and the unseeded rows keep the live count (7 in the 2026-10-12 race:
  types 0,1,1,1,2,2,3).
The 2026-10-12 attribution probe (re/analysis/negctl/followup_20261012/a2-camera_evidence/) showed the old port's RED was
a 4-byte stack-frame shift: callees copy uninitialized stack dwords into the RwMatrix pad slots of the entry, so this
fixture's GREEN depends on both arms entering the callees at the same stack depth with the same prior stack contents.
The prep call provides that (it runs the original dispatcher at the same depth right before each arm).
"""

FIXES = {}

_ARG = 0x00897fe0
_T = []
for n in (None, None, -1, 0, 1, 4, 6, None):
    t = {'args': [_ARG], 'prep_args': [_ARG]}
    if n is not None:
        t['seed'] = [{'addr': 0x00898994, 'val': n & 0xFFFFFFFF}]
    _T.append(t)

FIXES['camera_entry_dispatch_all'] = {
    'arg_type': 'scalars_to_scattered_globals',
    'signature': {'ret': 'uint32', 'args': ['uint32']},
    'observe': [{'addr': 0x008964c0, 'len': 0x00898998 - 0x008964c0}],
    'freeze_main': True, 'fold_ret': True,
    'target_global': None, 'seed_globals': None, 'call_args': None, 'crash_equal_ok': None,
    'seed_table': None, 'state_gate': None,
    'prep_call_str': '0x004464c0', 'prep_arg_types': ['uint32'],
    'path1_tests': _T, 'path2_tests': _T[:2],
}
