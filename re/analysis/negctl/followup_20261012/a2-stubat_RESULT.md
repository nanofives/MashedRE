# negctl follow-up, triage A2 `stub_at` group: result (2026-10-12)

Branch `work/a2-stubat` (base 4d6f7038). Four rows from `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md`
§A2. Bytes read with `re/tools/disasm_fn.py` on `original\MASHED.exe.unpatched`. Built with `MASHED_NO_DEPLOY=1`;
the shared `original\mashed_re_dev.asi` was never touched. No path2, no tracker edits, no re-classify.

**Which .asi the diffs ran.** `run_diff.py` resolves `ROOT` from its own location, so in this worktree
`ASI_PATH = <worktree>\mashedmod\build\mashed_re_dev.asi`, passed to the agent as `CONFIG.asi_path` and
`Module.load`ed from that absolute path (`diff_template.js:1603`). `MASHED.exe` is `<worktree>\original\` (junction to
the real install). Every run spawned its own PID and killed only that PID.

| row | hook key | port defect found in the bytes | verdict |
|---|---|---|---|
| 0x00492270 | `subsystem_init` | yes: callees reached through this TU's own bodies, not the original addresses | FIXED, all legs pass |
| 0x004026d0 | `boot_queue_flush` | yes: two callees given 1 arg where the bytes give 3 | FIXED, all legs pass |
| 0x00404320 | `per_mode_render_machine` | yes: mode-5 sub-state-6 branch, EAX arg to 0x00403fa0 | FIXED, all legs pass |
| 0x0040d440 | `course_loadcurrent_40d440` | none (port matches the bytes) | all legs pass; witness is an injected defect |

**Ready for path2: `subsystem_init`, `boot_queue_flush`, `per_mode_render_machine`, `course_loadcurrent_40d440`.**

Evidence directory: `re/analysis/negctl/followup_20261012/evidence/a2-stubat/`
(`diff_<key>.csv` GREEN on the fixed build, `diff_<key>_selfbaseline.csv`, `diff_<key>_witness_red.csv`,
`negctl_a2_stubat.tsv`). Fixture: `re/frida/negctl_fixtures/z_fix_a2_stubat.py` (replaces all four entries whole).

---

## Harness change: `call_log` for `scalars_to_scattered_globals`

The triage's named features were not enough as they stood. `stub_dispatch_observe` has named recorders but no
`freeze_main` and no globals observe. `scalars_to_scattered_globals` has `freeze_main`, `stub_at` and globals observe,
but ONE shared recorder with one arity that does not say which callee fired, and it installs `stub_at` before the
freeze, so the game's first thread could run a recorder (the RwCamera* leaves run every frame on that thread).

New key `call_log: [{name, nargs, abi?, ret?, at?, deref?, regs?}]` (`diff_template.js`, scalars handler):
- one NativeCallback per name with its own arity / ABI; each call appends `name(args|reg=..)` to a per-arm log;
- the log is folded into the FNV fingerprint AND returned with it, so every CSV row carries the full call sequence;
- `at` entries are `Interceptor.replace`d only AFTER `__freeze.thread()` and reverted BEFORE `__freeze.flush()`
  (refuses to run without `freeze_main`; same 5-byte install guard as `stub_at`);
- args inside the scratch are written `s+0xOFF`; `deref {i: n}` logs n dwords read through arg i instead of the raw
  address when it is outside the scratch (a pointer to a caller local differs per arm); `regs ['eax']` logs the
  register at entry; per-test `stub_rets {name: v | [v_call0, ...]}`; a seed value `{call_log: name}` resolves to
  the recorder address (fake RwGlobals +0x20 slot).
- Forwarded by `run_diff.py build_config` and `run_verify_hook.py` (both whitelists). Lean mirror in
  `verify_hook_install_template.js` (returns only, same freeze/install/revert bracket); `node --check` passes on it,
  it has NOT been run (path2 is the orchestrator's). `re/frida/ARG_TYPES.md` was not regenerated on this branch (the
  regeneration rewrites every row's line number and would conflict with sibling branches): regenerate after merge.

---

## 0x00492270 SubsystemInit (`subsystem_init`), Boot/SubsystemInit.cpp

**Why C2.** `hooks.csv:60` negctl re-earn 2026-10-11: unreachable at the menu, the body runs the RW engine init
chain; needs stubs for 0x00493710 / 0x004921d0 / 0x00428590.

**Bytes.** `0x00492270 push 0; 0x00492272 call 0x493710; 0x00492277 add esp,4; 0x0049227a test eax,eax;
0x0049227c jne; 0x0049227e ret; 0x0049227f call 0x4921d0; 0x00492284 call 0x428590; 0x00492289 mov eax,1; ret`.

**Defect.** The port called `DisplayInit()` / `ViewportInit()`, the C++ bodies in the same TU, instead of the
addresses 0x004921d0 / 0x00428590. `ViewportInit`'s `RH_ScopedInstall` is MASS-DISABLED (`SubsystemInit.cpp:371`
before this change), so with `subsystem_init` installed the game ran an uninstalled, unverified ViewportInit body in
place of the original 0x00428590, and no stub at either address could observe the calls. Fixed: both are called
through their original addresses (an installed DisplayInit hook is still reached through its JMP).

**Fixture.** Recorders `rwinit` (0x00493710, 1 arg, per-test return), `display` (0x004921d0, 0 args), `viewport`
(0x00428590, 0 args). Observable: return (`fold_ret`) + call log. Returns 0, 1, 2, -1, 0x80000000, 0x100.

| leg | result |
|---|---|
| run_diff | GREEN 8/8 (`rwinit(0x0)` alone for return 0, `rwinit(0x0);display();viewport()` otherwise) |
| --self-baseline | GREEN 8/8 |
| negctl | real GREEN, baseline GREEN, null RED 8/8, ret RED 8/8, field_first NA, field_last NA, field_none CTRL-GREEN |
| witness (old body) | RED 6/8: reimpl log `rwinit(0x0)` only, every vector with a non-zero gate (the 2 GREEN are return 0) |
| re-run after restore | GREEN 8/8 (build showed `SubsystemInit.cpp (source newer)`) |

Field NA: the body writes no observed memory; its whole output is the call sequence and the return, which null and
ret cover. Callers (Ghidra boundary, `decomp_pc.py --callers`): FUN_00492370 (C2). Callees: 0x00493710, 0x004921d0,
0x00428590, all recorders in the fixture.

The witness run executed the old port's real DisplayInit / ViewportInit bodies on the A/B thread against the live
engine. The process survived to the end of the run; it was then killed by PID as usual.

## 0x004026d0 FUN_004026d0 (`boot_queue_flush`), Boot/BootLowRvaCluster.cpp

**Why C2.** `hooks.csv:169` negctl re-earn: every effect is a RwCamera* call; needs `stub_at` + `freeze_main` +
pointer normalisation for the `&local_4` stack arg.

**Bytes and defect.** The caller pops only part of what it pushed for `call 0x4671a0`, so the next callee inherits the
rest:
- `0x004026f0 push 3; 0x004026f2 lea eax,[esp+8]; 0x004026f6 push eax; 0x004026f7 push 0; 0x004026f9 call 0x4671a0;
  0x004026fe add esp,4; 0x00402701 push eax; 0x00402702 call 0x4c1bb0` -> `FUN_004c1bb0(cam, &local_4, 3)`
- `0x0040272b push 1; push 0; push 0; 0x00402731 call 0x4671a0; 0x00402736 add esp,4; 0x00402739 push eax;
  0x0040273a call 0x4c1be0; 0x0040273f add esp,0xc` -> `FUN_004c1be0(cam, 0, 1)`
- 0x004c1bb0 reads `[esp+4]`, `[esp+8]`, `[esp+0xc]` (0x004c1bbe, 0x004c1bb9, 0x004c1bb0); 0x004c1be0 the same
  (0x004c1be9, 0x004c1be4, 0x004c1be0). 0x004671a0 reads one stack arg (`[esp+4]` at 0x004671aa).
The decomp shows `FUN_004c1bb0(uVar1)` / `FUN_004c1be0(uVar1)`, and the port declared both 1-arg, so their 2nd and 3rd
args were the port's stack garbage. Fixed: 3-arg typedefs, called `(cam, &local_4, 3)` and `(cam, nullptr, 1)`.

**Fixture.** Recorders `cam` (0x004671a0, per-call returns 0x0ca00001..), `clear` (0x004c1bb0, 3 args, arg 1
dereferenced), `begin` (0x004c1a00, per-call returns drive the `iVar2 != 0` branch), `end` (0x004c19f0), `show`
(0x004c1be0, 3 args). n = 0, 1, 2, 3, 4, -1, -0x7fffffff.

| leg | result |
|---|---|
| run_diff | GREEN 9/9 (e.g. n=1: `cam(0x0);clear(0xca00001,@ff000000,0x3);cam(0x0);begin(0xca00002);cam(0x0);end(0xca00003);cam(0x0);show(0xca00004,0x0,0x1)`) |
| --self-baseline | GREEN 9/9 |
| negctl | real GREEN, baseline GREEN, null RED 6/9, ret SKIP (void), field_first NA, field_last NA, field_none CTRL-GREEN |
| witness (old 1-arg calls) | RED 6/9: reimpl `clear(0xca00001,@AV,0x465c608)` and `show(..,0x0,0x0)` |
| re-run after restore | GREEN 9/9 (`BootLowRvaCluster.cpp (source newer)`) |

The 3 GREEN vectors in null / witness are n = 0, -1, -0x7fffffff, where the original makes no call either
(`0x004026ec jle 0x402745`). Callers: FUN_00402750 (C2, AppInitialiseOnBootup). Callees: 0x004671a0, 0x004c19f0,
0x004c1a00, 0x004c1bb0, 0x004c1be0, all recorders.

## 0x00404320 PerModeRenderMachine (`per_mode_render_machine`), Render/PerModeRender.cpp

**Why C2.** `hooks.csv:563` negctl re-earn: every arm runs RwCamera* on the live main camera from the harness
thread; original-vs-original RED 7/8; needs stubbed RwCamera* callees with the game frozen.

**Defects found in the bytes (the port is rewritten to follow them):**
1. **Mode 5 never queries sub-state 6.** `0x0040452d jne 0x4043be` goes straight to the tail after the 5/4/3 chain
   fails. Modes 9 and 8 go to `0x00404482 call 0x40e350; cmp eax,6; ... 0x00404490 call 0x4041c0`. The port ran the
   ==6 query and `FUN_004041c0` in mode 5 as well.
2. **0x00403fa0 takes the entry pointer in EAX.** `0x004045f0 push edi; 0x004045f1 mov eax,esi; 0x004045f3 call
   0x403fa0`; the callee reads and WRITES the entry's float through EAX (`0x00403fa3 fld [eax+0x10]`,
   `0x00403fc1 fstp [eax+0x10]`). The decomp shows `FUN_00403fa0(iVar4)` only. The port called it as plain cdecl, so
   EAX held a leftover (0x6 / 0x0 in the witness), i.e. in the game it would read and write `[0x16]` / `[0x10]`.
   Fixed with a naked thunk `Call_00403fa0(entry, idx)` that loads EAX and pushes idx.
3. Not observable, aligned anyway: EndUpdate is called BEFORE the +0x68/+0x6c view-window save (0x0040433d before
   0x00404342); the 5/4/3 chain calls 0x0040e350 once per test with short-circuit (0x00404397 / 0x004043a1 /
   0x004043ab); the 0.0 gate at 0x005d757c (.rdata) is read per iteration. The old port also returned the camera
   pointer as its BeginUpdate flag; a zero camera is not a vector because the original faults at 0x00404342
   (`mov ecx,[esi+0x68]`) on both arms.

**Fixture.** `*(0x007d3ff8)` -> scratch (fake RwGlobals): +0x00 camera = scratch+0x100 (view window at +0x68/+0x6c
seeded), +0x20 = recorder `rs` (`call [ecx+0x20]`, 2 cdecl args, 0x00404385). Seeds DAT_007f0fd0 (mode) and
DAT_0063ba8c (0x0040e350 is `mov eax,[0x63ba8c]; ret`, so it is left real and seeded), and the six +0x10 floats of
the mode-10 table at 0x00636b88 (1, 0, -1, NaN, denormal, 2 / -0, 0.5, 0, 0, 0.75, -1). Recorders: `end` 0x004c19f0,
`vw` 0x004c1c80 (float pair dereferenced), `begin` 0x004c1a00, `r9` 0x00403d30, `r8` 0x00403db0, `r5` 0x00403ed0,
`r10` 0x00403c60, `entry` 0x00403fa0 (+EAX), `sub6` 0x004041c0. 35 vectors: modes 9/8/5/10 x sub-states
5/4/3/6/0/7 + BeginUpdate fail per mode, two table/view-window variants, modes 0/1/11/0xffffffff.

| leg | result |
|---|---|
| run_diff | GREEN 35/35 (mode 10 sub 6: `entry(0x0|eax=0x636b88);entry(0x4|eax=0x636bd8);entry(0x5|eax=0x636bec)`) |
| --self-baseline | GREEN 35/35 |
| negctl | real GREEN, baseline GREEN, null RED 31/35, ret SKIP (void), field_first NA, field_last NA, field_none CTRL-GREEN |
| witness (old file) | RED 4/35: idx 17 and 30 (mode 5 sub 6: reimpl adds `sub6()`), idx 24 and 28 (mode 10 sub 6: reimpl `eax=0x6` / `eax=0x0`) |
| re-run after restore | GREEN 35/35 (`PerModeRender.cpp (source newer)`) |

The 4 GREEN vectors under null are modes 0/1/11/0xffffffff, where the original makes no call either
(`0x00404540 jne 0x404642`). Callers: FUN_00492e90 (C2). Callees: all ten Ghidra callees are recorders except
0x0040e350 (pure getter, seeded through 0x0063ba8c).

## 0x0040d440 Course::LoadCurrent (`course_loadcurrent_40d440`), Track/CourseLoadDispatch.cpp

**Why C2.** `hooks.csv:1366` negctl re-earn: the recorder proved the call but the store DAT_0063ba78 was not
observed (field mutants NA).

**Bytes.** `0x0040d440 mov eax,[0x63ba7c]; 0x0040d445 mov ecx,[0x5f2770]; push eax; push ecx; 0x0040d44d call
0x40d270; 0x0040d452 mov edx,[0x63ba7c]; 0x0040d458 add esp,8; 0x0040d45b mov [0x63ba78],edx; ret`. The port
matches; no change to the source.

**Fixture.** Recorder `finish` at 0x0040d270 (2 args). Seeds 0x005f2770, 0x0063ba7c and 0x0063ba78 with distinct
values; observes 0x0063ba78..0x0063ba7f and 0x005f2770. Six vectors.

**Witness: a deliberately injected single-instruction defect, stated as such** (not a defect of the port): the call's
second argument read from 0x0063ba78 instead of 0x0063ba7c (the `mov eax,[0x63ba7c]` at 0x0040d440 pointed at the
neighbouring global). negctl's classes mutate the port's outputs and return; none changes a callee argument, so this
class is not already covered.

| leg | result |
|---|---|
| run_diff | GREEN 6/6 (`finish(0xabc000,0x3)` etc., plus the global windows in the fingerprint) |
| --self-baseline | GREEN 6/6 |
| negctl | real GREEN, baseline GREEN, null RED 6/6, field_first RED 5/6 (flip 0x0063ba78), field_last RED 5/6 (flip 0x0063ba7b), field_none CTRL-GREEN, ret SKIP (void) |
| witness (injected) | RED 5/6: reimpl `finish(0xabc000,0x5a5a5a5a)` vs `finish(0xabc000,0x3)`; the GREEN vector seeds 0x0063ba78 == 0x0063ba7c == 1 |
| re-run after restore | GREEN 6/6 (`CourseLoadDispatch.cpp (source newer)`) |

The old field-mutant NA is closed: the store is now observed and both field mutants read RED. Callers: FUN_004929d0
(C2). Callee: 0x0040d270, recorder.

---

## Proposed tracker changes (orchestrator applies; NOT applied here)

All four: C2 stays C2 on this branch. After the orchestrator's merge, rebuild with deploy and path2 PASS:

| RVA | hooks.csv row | proposed | frida_diff pointer |
|---|---|---|---|
| 0x00492270 | `00492270,SubsystemInit` | C2 -> C3 (port FIXED) | `re/analysis/negctl/followup_20261012/evidence/a2-stubat/diff_subsystem_init.csv` |
| 0x004026d0 | `004026d0,FUN_004026d0` | C2 -> C3 (port FIXED) | `.../a2-stubat/diff_boot_queue_flush.csv` |
| 0x00404320 | `00404320,PerModeRenderMachine` | C2 -> C3 (port FIXED) | `.../a2-stubat/diff_per_mode_render_machine.csv` |
| 0x0040d440 | `0040d440,FUN_0040d440` | C2 -> C3 (port unchanged) | `.../a2-stubat/diff_course_loadcurrent_40d440.csv` |

Note to append to each row: "negctl follow-up a2-stubat 2026-10-12: scalars_to_scattered_globals + freeze_main +
call_log fixture (re/frida/negctl_fixtures/z_fix_a2_stubat.py); run_diff GREEN, self-baseline GREEN, negctl
re/analysis/negctl/followup_20261012/evidence/a2-stubat/negctl_a2_stubat.tsv; witness RED
(diff_<key>_witness_red.csv); result note re/analysis/negctl/followup_20261012/a2-stubat_RESULT.md", plus for the
three fixed rows the one-line defect from the table at the top.

Stub rows: `STUBS.md:207` S-0172 says "FUN_004c1bb0; called with result of FUN_004671a0(0,&local,3)" and
`STUBS.md:210` S-0175 "FUN_004c1be0; called with result of FUN_004671a0(0,0,1)". Both read as one-arg calls. Proposed
text: S-0172 "FUN_004c1bb0(cam, &local_4, 3): args 2/3 are the pushes left by `add esp,4` at 0x004026fe";
S-0175 "FUN_004c1be0(cam, 0, 1): args 2/3 left by `add esp,4` at 0x00402736".

No new U-rows. C4 for any of the four is not claimed and was not attempted: 0x00492270 runs once at boot, 0x004026d0
only from FUN_00402750 at boot, 0x00404320 needs a mode 5/8/9/10 screen, 0x0040d440 a course load. Each needs its
own canonical-scenario run with the hook installed; no forced input was used.
