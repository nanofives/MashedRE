"""negctl follow-up, triage A2 stub_at group (2026-10-12). Loads last; replaces the four entries whole.

All four use scalars_to_scattered_globals + freeze_main + the new `call_log` key (diff_template.js): every direct
callee is a NAMED recorder with its own arity, Interceptor.replace'd only while the game's first thread is frozen,
and the per-arm call sequence (with arguments) is folded into the fingerprint and written to the CSV. Byte citations
are from `py -3.12 re/tools/disasm_fn.py` on original\\MASHED.exe.unpatched.

subsystem_init (0x00492270): push 0; call 0x493710; test eax; ret 0 | call 0x4921d0; call 0x428590; mov eax,1.
  Recorders replace all three callees, so nothing re-inits the live engine. Observable: return + call log.

boot_queue_flush (0x004026d0): per loop, the caller's `add esp,4` after `call 0x4671a0` leaves the earlier pushes in
  place, so 0x004c1bb0 gets (cam, &local_4, 3) (0x004026f0..0x00402702) and 0x004c1be0 gets (cam, 0, 1)
  (0x0040272b..0x0040273a). local_4 = 0xff000000 (byte stores 0x004026d8..0x004026e7) is read through the pointer
  (deref), because its stack address differs per arm. 0x004671a0 reads one stack arg ([esp+4] at 0x004671aa).

per_mode_render_machine (0x00404320): *(0x007d3ff8) -> scratch (fake RwGlobals): +0x00 camera = scratch+0x100,
  +0x20 = recorder 'rs' (render-state calls `call [ecx+0x20]`, 2 cdecl args, 0x00404385). The camera +0x68/+0x6c
  view window is seeded. DAT_007f0fd0 = mode. 0x0040e350 is NOT replaced: it is `mov eax,[0x63ba8c]; ret`
  (0x0040e350), so DAT_0063ba8c is seeded instead. The mode-10 table 0x00636b88 (6 x 0x14, float at +0x10) is
  seeded against *(float*)0x005d757c = 0.0 (.rdata). 0x00403fa0 also logs EAX (0x004045f1 mov eax,esi).
  A zero camera is NOT a vector: the original faults at 0x00404342 (mov ecx,[esi+0x68]) on both arms.

course_loadcurrent_40d440 (0x0040d440): call 0x40d270(*0x005f2770, *0x0063ba7c) then 0x0063ba78 = *0x0063ba7c
  (re-read at 0x0040d452). All three globals are seeded with distinct sentinels and observed.
"""

FIXES = {}

_COMMON = {
    'arg_type': 'scalars_to_scattered_globals', 'freeze_main': True, 'lut_root_delta': 0,
    'fold_ret': False, 'scratch_size': 0, 'observe': [],
}


# ── 0x00492270 SubsystemInit ────────────────────────────────────────────────────────────────────────────────────────
def _si(r):
    return {'args': [], 'seed': [], 'stub_rets': {'rwinit': r}}


_SI_TESTS = [_si(0), _si(1), _si(2), _si(-1), _si(0x80000000), _si(0x100), _si(0), _si(1)]

FIXES['subsystem_init'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x00492270, 'export': 'SubsystemInit',
    'signature': {'ret': 'uint32', 'args': []}, 'fold_ret': True,
    'call_log': [{'name': 'rwinit', 'at': '0x00493710', 'nargs': 1, 'ret': 1},
                 {'name': 'display', 'at': '0x004921d0', 'nargs': 0},
                 {'name': 'viewport', 'at': '0x00428590', 'nargs': 0}],
    'path1_tests': _SI_TESTS, 'path2_tests': [_si(0), _si(1)],
})


# ── 0x004026d0 BootQueueFlush(int n) ───────────────────────────────────────────────────────────────────────────────
def _bq(n, begin):
    return {'args': [n], 'seed': [],
            'stub_rets': {'cam': [0x0CA00001 + k for k in range(16)], 'begin': begin}}


_BQ_TESTS = [_bq(0, [1]), _bq(1, [1]), _bq(1, [0]), _bq(2, [1, 0]), _bq(3, [0, 1, 0]), _bq(3, [1, 1, 1]),
             _bq(-1, [1]), _bq(-0x7FFFFFFF, [1]), _bq(4, [0, 0, 1, 1])]

FIXES['boot_queue_flush'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x004026d0, 'export': 'BootQueueFlush',
    'signature': {'ret': 'void', 'args': ['int32']},
    'call_log': [{'name': 'cam', 'at': '0x004671a0', 'nargs': 1},
                 {'name': 'clear', 'at': '0x004c1bb0', 'nargs': 3, 'deref': {1: 1}},
                 {'name': 'begin', 'at': '0x004c1a00', 'nargs': 1, 'ret': 1},
                 {'name': 'end', 'at': '0x004c19f0', 'nargs': 1},
                 {'name': 'show', 'at': '0x004c1be0', 'nargs': 3}],
    'path1_tests': _BQ_TESTS, 'path2_tests': [_bq(1, [1]), _bq(2, [1, 0])],
})


# ── 0x00404320 PerModeRenderMachine() ──────────────────────────────────────────────────────────────────────────────
_F = {'0.5': 0x3F000000, '0.75': 0x3F400000, '1': 0x3F800000, '-1': 0xBF800000, '0': 0x00000000,
      '-0': 0x80000000, 'nan': 0x7FC00000, 'tiny': 0x00000001, '2': 0x40000000}
_TABLE_A = ['1', '0', '-1', 'nan', 'tiny', '2']
_TABLE_B = ['-0', '0.5', '0', '0', '0.75', '-1']


def _pm(mode, sub, begin=1, vw=('0.5', '0.75'), table=_TABLE_A):
    s = [{'addr': 0x007d3ff8, 'val': {'scratch': 0}},
         {'addr': {'scratch': 0x00}, 'val': {'scratch': 0x100}},
         {'addr': {'scratch': 0x20}, 'val': {'call_log': 'rs'}},
         {'addr': {'scratch': 0x168}, 'val': _F[vw[0]]},
         {'addr': {'scratch': 0x16c}, 'val': _F[vw[1]]},
         {'addr': 0x007f0fd0, 'val': mode},
         {'addr': 0x0063ba8c, 'val': sub}]
    s += [{'addr': 0x00636b88 + 0x14 * k + 0x10, 'val': _F[v]} for k, v in enumerate(table)]
    return {'args': [], 'seed': s, 'stub_rets': {'begin': begin}}


_PM_TESTS = []
for _m in (9, 8, 5, 10):
    for _s in (5, 4, 3, 6, 0, 7):
        _PM_TESTS.append(_pm(_m, _s))
    _PM_TESTS.append(_pm(_m, 6, begin=0))
_PM_TESTS += [_pm(10, 6, table=_TABLE_B, vw=('1', '-1')), _pm(9, 6, vw=('2', 'tiny')), _pm(5, 6, vw=('-0', 'nan')),
              _pm(0, 6), _pm(1, 3), _pm(11, 6), _pm(0xFFFFFFFF, 5)]

FIXES['per_mode_render_machine'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x00404320, 'export': 'PerModeRenderMachine',
    'signature': {'ret': 'void', 'args': []}, 'scratch_size': 0x200,
    'observe': [{'addr': {'scratch': 0x100}, 'len': 0x80}],
    'call_log': [{'name': 'end', 'at': '0x004c19f0', 'nargs': 1},
                 {'name': 'vw', 'at': '0x004c1c80', 'nargs': 2, 'deref': {1: 2}},
                 {'name': 'begin', 'at': '0x004c1a00', 'nargs': 1, 'ret': 1},
                 {'name': 'r9', 'at': '0x00403d30', 'nargs': 0},
                 {'name': 'r8', 'at': '0x00403db0', 'nargs': 0},
                 {'name': 'r5', 'at': '0x00403ed0', 'nargs': 0},
                 {'name': 'r10', 'at': '0x00403c60', 'nargs': 0},
                 {'name': 'entry', 'at': '0x00403fa0', 'nargs': 1, 'regs': ['eax']},
                 {'name': 'sub6', 'at': '0x004041c0', 'nargs': 0},
                 {'name': 'rs', 'nargs': 2}],
    'path1_tests': _PM_TESTS, 'path2_tests': [_pm(9, 3), _pm(10, 6)],
})


# ── 0x0040d440 Course::LoadCurrent() ──────────────────────────────────────────────────────────────────────────────
def _cl(tbl, sel, loaded):
    return {'args': [], 'seed': [{'addr': 0x005f2770, 'val': tbl}, {'addr': 0x0063ba7c, 'val': sel},
                                 {'addr': 0x0063ba78, 'val': loaded}]}


_CL_TESTS = [_cl(0x00ABC000, 3, 0x5A5A5A5A), _cl(0x00ABC100, 0, 7), _cl(0, 1, 1), _cl(0x12345678, 0xFFFFFFFF, 0),
             _cl(0x00ABC200, 0x7FFFFFFF, 0x80000000), _cl(0x00ABC300, 12, 0xDEADBEEF)]

FIXES['course_loadcurrent_40d440'] = dict(_COMMON, **{
    '__replace__': True, 'rva': 0x0040d440, 'export': 'Course_LoadCurrent_40d440',
    'signature': {'ret': 'void', 'args': []},
    'observe': [{'addr': 0x0063ba78, 'len': 8}, {'addr': 0x005f2770, 'len': 4}],
    'call_log': [{'name': 'finish', 'at': '0x0040d270', 'nargs': 2}],
    'path1_tests': _CL_TESTS, 'path2_tests': _CL_TESTS[:2],
})
