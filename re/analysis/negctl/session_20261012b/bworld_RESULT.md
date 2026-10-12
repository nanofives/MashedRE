# bworld: B rows that needed game state the menu harness lacked (2026-10-12)

Branch `work/bworld`. Rows: 0x00426dc0 FrontendRaycastForward, 0x00493540 thunk_LaunchLangGate.

## Verdicts

| RVA | key | verdict | evidence in one line |
|---|---|---|---|
| 0x00426dc0 | `frontend_raycast_forward` | **C2 -> C3 ready** | race lane, 42 live-anchored rays, GREEN 42/42 + self-baseline 42/42, negctl null/ret/field RED, witness RED 2/42 that the old b08 race vectors read GREEN 5/5 |
| 0x00493540 | `launch_lang_gate` | **C2 -> C3 ready** | per-test DAT_007719e8 sweep over every jump-table slot, GREEN 14/14 + self-baseline 14/14, negctl null/ret RED, witness RED 1/14 that the old b10 fixture read GREEN 3/3 |

**Unpatched-reference arm for 0x00493540: not needed, not built.** The skip_controller_dialog patch lives INSIDE
FUN_00495150, and both arms run that body from the same image (the port is the same thunk). Bytes, original vs patched:

| VA | MASHED.exe.unpatched | MASHED.exe |
|---|---|---|
| 0x004951aa | `a1 c0 47 61 00` mov eax,[0x6147c0] | `33 c0` xor eax,eax |
| 0x004951ac..0x004951ae | (inside the mov) | `90 90 90` nop x3 |

The surrounding bytes are identical (0x004951a9 `push eax`, 0x004951af `push eax`, 0x004951b0 `call 0x498510`). The
patch changes exactly one thing: the first argument of the 0x00498510 call is 0, not *(0x006147c0). The port
(`Boot/LaunchHandshake.cpp`, `return s_FUN_00495150();`) contains none of those bytes. An unpatched arm would run the
same original instruction on both sides, so it cannot see a port defect. The sweep shows the patched value directly:
with 0x006147c0 seeded 0x5eed0001+i, every vector logs `sel(0x0, ...)`. If FUN_00495150 is ever re-ported as a body
(not a thunk), the 0x004951aa push becomes port code and this question reopens. That is a user decision then, not now.

## New capability and how to use it

| piece | where | what |
|---|---|---|
| `{f32_at: addr, add: f}` seed value | `diff_template.js` resolveVal (scalars_to_scattered_globals), mirrored in `verify_hook_install_template.js` | f32 bits of fround(*(float*)addr + add), read when the seed is applied. Anchors a vector to live state (car world position) so it lands on the loaded track whatever track the race lane reached |
| `observe_raw: True` | `diff_template.js` snapFp, forwarded in `run_diff.py` (whitelist) | fingerprint becomes `<fnv> <window hex>;..;r=<ret>`, so the CSV says miss / hit / which value, not only a hash |
| `race_sentinel: <addr>` | `run_diff.wait_race_sentinel`, called by `run_diff.py`, `run_negctl.py --race` (per slot, for the hooks it runs) and `run_verify_hook.py` | after `drive_to_race`, vectors run only once that dword is non-zero (30 s bound). Otherwise exit 6, never an AV on both arms |
| scenario `race` in path2 | `run_verify_hook.py` | path2 now pins the save, drives to a race and waits on `race_sentinel`, same as path1. Syntax-checked (`py_compile`, `node --check`), **not run** |

Recipe for a hook that reads a race-only object: set `scenario: 'race'`, `race_sentinel: <the global the body
dereferences>`, `freeze_main: True`, and seed inputs from live records with `{f32_at}`. Car i world position:
`0x008815a0 + i*0xd04 + 0x958/0x95c/0x960` (x/y/z, memory `msd-world-position-is-the-0x928-matrix-row`).

The 0x00493540 sweep needed no new handler: `scalars_to_scattered_globals` already had per-test `seed`, named
`call_log` recorders with `at`, and `freeze_main`.

Fixture: `re/frida/negctl_fixtures/z_fix_bworld.py` (replaces both entries whole, loads after b08.py / b10.py).
`re/frida/ARG_TYPES.md` regenerated (line numbers only, no new arg_type).

## 0x00426dc0 FrontendRaycastForward

**Bytes** 0x00426dc0..0x00426ddc: `push [esp+0xc]; push [esp+8]; push [esp+4]; push 0x646e58; call 0x479100;
add esp,0x10; ret`. EAX is FUN_00479100's return. Sole caller FUN_0041f590 tests it: `0x0041f628 call 0x426dc0;
0x0041f62d add esp,0xc; 0x0041f630 test eax,eax; je 0x41f63e`.

**Port fix.** The export was `void` and called a `void` function pointer. EAX reached the caller only because MSVC
emitted `call eax; add esp,0x10; ret`. It now returns `std::uint32_t` through a `uint32` pointer. Predicted and
confirmed: identical compiled body (`push [esp+0xc]; mov eax,0x479100; push x2; push 0x646e58; call eax;
add esp,0x10; ret`), EAX carried by contract instead of by codegen.

**FUN_00479100 outcomes** (listing 0x00479100..0x00479326), which define what the vectors must reach:
- miss: `xor eax,eax` (0x00479150), nothing written
- hit: `*frac = [esp+0x98]` when frac != 0 (0x0047912b..0x0047913d)
  - mat+0x84 == 0: return 0 (0x00479146)
  - out == 0: return 1 (0x00479162 -> 0x00479319)
  - mat+0x30 == 0: `*out = 0x3f800000`, return 1 (0x0047916e -> 0x00479311)
  - else: blend through `call 0x559040` (0x0047920a), clamped against [0x5cc320] (0x004792d6), return 1

**Why the old lanes were blind.** At the menu DAT_00656ee8 (= 0x00646e58 + 0x10090, read at 0x00479104) is NULL and
both arms AV at 0x1c. The b08 race fixture, run today on the worktree build, reaches a race and reads GREEN 5/5, but
its 5 vectors are fixed world coordinates. 3 of them are misses (fingerprint 0x204b0efd = FNV of 8 bytes 0xA5 +
ret 0, computed) and none passes a NULL out or frac.

**New vectors** (42): vertical rays (y+5 -> y-5) under cars 0..3 at 9 lateral offsets each, two misses (500 above,
400 below car 0/1), an up-pointing ray, and NULL out / NULL frac / both NULL. Outcomes read from the observe_raw CSV:

| outcome (exclusive, counted from the committed CSV) | vectors |
|---|---|
| hit, out = value other than 0.0 / 1.0, frac written, r=1 | 21 |
| hit, out = 0.0, frac written, r=1 | 11 |
| hit, out = 0x3f800000 exactly, frac written, r=1 | 4 (idx 7, 16, 29, 35; clamp and mat+0x30 == 0 give the same bits) |
| miss, r=0, nothing written | 3 (idx 36, 37, and 41 the up-pointing ray) |
| out NULL, frac written, r=1 | 1 (idx 38) |
| frac NULL, out written, r=1 | 1 (idx 39) |
| both NULL, r=1 | 1 (idx 40) |

Exact per-vector values are in `bworld_evidence/frontend_raycast_forward.csv` (observe_raw column).
Not reached on this track: the mat+0x84 == 0 path (hit with r=0). It is a branch of the original FUN_00479100, which
the port calls unchanged, so it does not gate the forwarder. The race track was not recorded. The rays do not depend
on it (anchored to car positions).

## 0x00493540 thunk_LaunchLangGate

**Bytes** (from MASHED.exe.unpatched): thunk `jmp 0x495150`. FUN_00495150: calls 0x00495120, 0x004955b0,
0x00499710 -> 0x004960a0, 0x00499710 -> 0x004963b0; `0x00495170 mov eax,[0x7719e8]; cmp eax,5; ja 0x4951a7;
jmp [eax*4+0x4951c8]`. Table 0x004951c8 = {0x4951a7, 0x495184, 0x49518b, 0x495192, 0x495199, 0x4951a0}, so case 0
and anything above 5 unsigned give 0, and 1->2, 2->3, 3->4, 4->5, 5->1. Then `push eax; [patch site]; push eax;
call 0x498510`; return `[0x7719e4] == 0` (`sete al`, 0x004951c2).

**Fixture**: recorders `qpf`/`di`/`a0`/`b0`/`sel` at the five init callees (cdecl, args from the pushes above),
0x00499710 stays real. Per test: DAT_007719e8 ∈ {0,1,2,3,4,5,6,0xffffffff,0x80000000,0x7fffffff,0x100,5,1,0},
DAT_007719e4 ∈ {0,1,0xffffffff,0x100,0x80000000}, DAT_006147c0 = 0x5eed0001+i. Observed `sel` second arguments:
0,2,3,4,5,1,0,0,0,0,0,1,2,0. All six table targets are reached.

**Why the old fixture was blind.** b10 ran with live DAT_007719e8, which logged `sel:0x0,0x0` on all 3 vectors, so
it never reached a jump-table case other than slot 0, and r=1 every time.

## Evidence (`re/analysis/negctl/session_20261012b/bworld_evidence/`)

Final build: both TUs recompiled "(source newer)" after each witness restore. `.asi` 20:53:38 after
BatchAA_s1.cpp 20:53:20, export disassembly checked guard-free. `.asi loaded` from the worktree build in every run.

| leg | frontend_raycast_forward | launch_lang_gate | files |
|---|---|---|---|
| run_diff | GREEN 0/42 (race lane, sentinel 0x00656ee8 non-zero) | GREEN 0/14 | `<key>.csv`, `drawstream_launch_lang_gate.csv` |
| --self-baseline | GREEN 0/42 | GREEN 0/14 | `<key>_selfbaseline.csv` |
| negctl real / baseline | GREEN / GREEN | GREEN / GREEN | `negctl_20261012_205026.tsv` / `negctl_20261012_203831.tsv` |
| negctl null | RED 39/42 | RED 14/14 | same |
| negctl ret | RED 42/42 | RED 14/14 | same |
| negctl field_first / field_last | RED 38/42 / RED 38/42 | NA / NA (flips=0, the thunk writes no memory) | same |
| negctl field_none | CTRL-GREEN | CTRL-GREEN | same |
| witness, new fixture | RED 2/42 (idx 38, 40) | RED 1/14 (idx 4) | `<key>_witness_red.csv` |
| witness, OLD fixture | GREEN 0/5 (b08 race vectors) | GREEN 0/3 (b10) | `<key>_witness_oldfixture.csv` |
| re-run after restore | GREEN (rows above) | GREEN (rows above) | |

negctl commands:
`py -3.12 re/frida/run_negctl.py --handlers scalars_to_scattered_globals --hooks frontend_raycast_forward --any-conf
--race --classes null,ret,field_first,field_last,field_none --slots 1` and the same without `--race` for
`launch_lang_gate`.

**Witnesses.**
- 0x00426dc0: `if (!param_2) return 0;` before the call (a defensive NULL guard the bytes do not have). New fixture:
  idx 38 orig `a5a5a5a553d9ff3e;r=1` vs reimpl `a5a5a5a5a5a5a5a5;r=0`, idx 40 orig r=1 vs r=0. The b08 race
  vectors never pass a NULL out and read GREEN 5/5 on the same build.
- 0x00493540: inline port of the body with case 4 -> 4 (bytes 0x00495192 `mov eax,5`). New fixture: idx 4 first
  divergence at call 4, `sel(0x0,0x5)` vs `sel(0x0,0x4)`. b10 read GREEN 3/3 on the same build.

**Callers / callees** (`re/tools/decomp_pc.py --callers --callees --no-decomp`, pool slot Mashed_pool0, run from the
main checkout because the worktree has no pool):
- 0x00493540: callers FUN_00492370; callees none (thunk), target FUN_00495150 (callees 0x00495120, 0x004955b0,
  0x004960a0, 0x004963b0, 0x00498510, 0x00499710; no other callers).
- 0x00426dc0: callers FUN_0041f590; callees FUN_00479100 (-> FUN_004b4cd0, FUN_00559040), FUN_004b4cd0 ->
  FUN_004b4c80 -> FUN_00538c80.

## Incidents (no effect on evidence)

- Four `attach failed ... terminated during injection` on spawn (pids 3056, 7360, 21424, 28532). Each process had
  already exited, and each run was repeated. One stale copy (taken after the 3056 failure) was overwritten by the
  re-run before commit. Nothing was killed. The only MASHED alive at the end belonged to worktree `bnoreturn`.
- `drive_to_race` printed `[nav] timeout: d3 depth=2 phase=3` three times on every race run and still reached the
  race (phase 0) on 7 of 7 navigations today (b08 check, probe, negctl, self-baseline, two witness runs, final). The 5 runs that carried race_sentinel read it non-zero (the two b08-fixture runs predate or
  lack the key). The b08 `process died` failures of
  2026-10-11 were not reproduced. The race lane re-run rule (U-9677) applies if one appears.

## Commits

| commit | content | keep/drop |
|---|---|---|
| `265f1184` | `{f32_at}` seed, `observe_raw`, `race_sentinel` (run_diff / run_negctl / run_verify_hook), path2 race drive + f32_at mirror, BatchAA_s1.cpp return-type fix, z_fix_bworld.py, ARG_TYPES.md regen, evidence | keep |
| (this) | this note | keep |

## Path2 (orchestrator)

- `launch_lang_gate`: arg_type `scalars_to_scattered_globals`, `freeze_main`, `call_log` with `at` entries (the
  a2-stubat path2 mirror). Install: `RH_ScopedInstall(thunk_LaunchLangGate, 0x00493540)`, Boot/LaunchHandshake.cpp.
- `frontend_raycast_forward`: same arg_type, `scenario: 'race'`, `race_sentinel: 0x00656ee8`, `{f32_at}` seeds.
  Needs this branch's `run_verify_hook.py` (race drive) and `verify_hook_install_template.js` (`f32_at`), both not
  run. Install: `RH_ScopedInstall(FrontendRaycastForward, 0x00426dc0)`, Frontend/BatchAA_s1.cpp.

## Proposed re-classify transaction (not applied)

hooks.csv:
- `00426dc0` FrontendRaycastForward: C2 -> **C3**; frida_diff
  `re/analysis/negctl/session_20261012b/bworld_evidence/frontend_raycast_forward.csv`; append: "bworld 2026-10-12:
  C2->C3. Race lane with race_sentinel DAT_00656ee8 and 42 rays anchored to live car positions ({f32_at} seeds):
  hits, misses, NULL out/frac. GREEN 42/42, self-baseline GREEN, negctl race null RED 39/42, ret RED 42/42,
  field_first/last RED 38/42. Witness (NULL-out guard) RED 2/42, b08 race vectors GREEN 5/5 on it. Port now returns
  FUN_00479100's EAX by contract (caller tests it at 0x0041f630). Ref
  re/analysis/negctl/session_20261012b/bworld_RESULT.md".
- `00493540` thunk_LaunchLangGate: C2 -> **C3**; frida_diff
  `re/analysis/negctl/session_20261012b/bworld_evidence/launch_lang_gate.csv`; append: "bworld 2026-10-12: C2->C3.
  Per-test seed of DAT_007719e8 (every slot of table 0x004951c8 plus out-of-range), DAT_007719e4 and DAT_006147c0,
  named recorders on the five init callees. GREEN 14/14, self-baseline GREEN, negctl null/ret RED 14/14. Witness
  (case 4->4) RED 1/14, b10 GREEN 3/3 on it. skip_controller_dialog (0x004951aa) is inside FUN_00495150, which both
  arms run unchanged, so no unpatched arm is needed while the port stays a thunk. Ref
  re/analysis/negctl/session_20261012b/bworld_RESULT.md".

STUBS / UNCERTAINTIES / DEFERRED: no row added or closed. CHANGELOG: one entry for both promotions and the
race_sentinel / f32_at / observe_raw capability.

## Decisions needed

None. No forced input, no differently-patched MASHED.exe and no new tool was used.
