# RESULT fwd494fd0 (2026-10-16)

Branch `work/fwd494fd0` from d074095d, worktree `.worktrees/fwd494fd0`. Three parts: **A** fix and re-earn of `0x00495080`,
**B** the inline-asm lint, **C** the AI dual-copy burn-down (ROADMAP D4 item 2, section at the end).

## A. 0x00495080 Fwd494fd0_DoublePlusHalf: fixed, re-earned, C2 -> C3

**Fix** (`Render/PromoLoop_round256.cpp`): `mov esi, dword ptr ds:[771A0Ch]`. Built export read back
(`verify/dupinstall2_20261016/disasm_asi_export.py`): `3E 8B 35 0C 1A 77 00  mov esi, dword ptr ds:[0x771a0c]` (the unfixed
build had `BE 0C 1A 77 00  mov esi, 0x771a0c`). The source comments that called the load a "dead load" were wrong
(`FUN_00494fd0` reads ESI: `mov eax,[esi]` at `0x00494fee`, `call [eax+0x1c]` at `0x00494ff1`) and are corrected.

**Register-input check in path1** (the harness allowed it as an additive, defaulted config key): `stub_esi` in
`re/frida/diff_template.js` `stub_dispatch_observe` replaces each `stub_at` entry with a 6-byte thunk
`mov [slot], esi ; jmp <recorder>` and appends `esi=0x...` to the recorded call; `stub_esi_seed {addr, val}` writes a
sentinel (`0xD1D0C0DE`) to the global the register is loaded from for the run (restored after). Forwarded in
`run_diff.py` `build_config` (the whitelist), turned on for `input_axis_scale_bias_apply`, `ARG_TYPES.md` regenerated.

| step | result | file |
|---|---|---|
| path1 on the UNFIXED build, hook bypassed (negative control) | **RED 8/8**: `orig esi=0xd1d0c0de`, `reimpl esi=0x771a0c`, every stack argument equal | `path1_before_fix.txt` |
| path1 on the fixed build | **GREEN 8/8** (same vectors, ESI compared) | `path1_after_fix.txt`, `path1_after_fix.csv` |
| path2 `run_verify_hook` | **PASS**: E9 + rel32 to the export, 2/2 entries | `win1/path2_input_axis_scale_bias_apply.txt` |
| nav boot to a race, FULL hook set, no `MASHED_HOOK_SKIP` | `drive_to_race=True`, process alive (`poll()=None`), no detach | `win1/nav_probe_full_hookset.txt` |
| full hook set boot, 45 s | 1290 of 1331 hooks installed, `0x00495080 Fwd494fd0_DoublePlusHalf` among them, survived | `win1/install_all/`, `win1/install_observe_all.txt` |
| the three race rows, path2 without the skip | `table_773030_get` 2/2, `plugin_data_dword_a` 1/1, `plugin_data_dword_b` 1/1, all PASS | `win1/path2_*.txt` |

Row `0x00495080` C2 -> C3 via the hooks.csv + CHANGELOG transaction (`tracker.py`). Not C4: no canonical-scenario diff of the
body with the JMP live was run (the function is reached from `FUN_0043dfd0`; not attempted here).
`KNOWN_ISSUES.md` `AUTOHOOK-INLINE-ASM-IMMEDIATE` is closed (FIXED block, guard, workaround retired).
The exe is unaffected: `Render/PromoLoop_round256.cpp` is an asi-only TU, `build.bat` gained only the lint call, so G-CTRL
was not re-run.

## B. scripts/lint_inline_asm_abs.py

Fails the build on a bare numeric absolute memory operand (`[0x771A0C]`, `[771A0Ch]`) in inline asm without a segment override.
- Scope: inside an `__asm` block or on a single-line `__asm`, and in `.inc` files, but only on lines that start with an x86
  mnemonic (after an optional label). That condition matters: `Vehicle/CarMaterialTables_dirt.inc` is a C++ data table
  (`body[0x1e] = 0`), the first draft flagged 13 lines of it.
- `--selftest` (12 positive/negative samples) runs on every invocation, so a broken regex fails loudly instead of passing everything.
- Wired into `mashedmod/build.bat` right after the rva lint (`MASHED_SKIP_ASM_LINT=1` skips).
- On the UNFIXED tree it reports exactly one line, `PromoLoop_round256.cpp:172`, and nothing else; on the fixed tree: OK over
  621 source files. The fix build ran through it (`build_fix.log`).
- Left uncommitted by Part A and committed with Part C: `log/diff_input_axis_scale_bias_apply.csv` (the tracked path1 evidence
  file the 0x00495080 row cites; byte-identical to `path1_after_fix.csv`, with the `esi=` column).

## C. ROADMAP D4 item (2): the AI copies

**Headline.** Allowlist **64 -> 56** keys (13 CROSS-TARGET, 43 DUP-IN-TARGET, 0 DUP-INSTALL). The 12 keys that touched
the AI area became 4: **8 are gone, 3 were narrowed to the real pair** (same RVA, the files that actually hold the bodies), and
`0x0040e350`, which sits in `Vehicle/`, was skipped. **Two real pairs were consolidated into one shared TU each and re-earned at C3 on the shared body**
(`0x0040e480` -> `Ai/CarSlotState.cpp`, `0x00417180` -> `Ai/AiBankSwitch.cpp`). The exe G-CTRL fingerprint is unchanged
across every step. Edit transactions, in order: `allowlist_edit.py`, `tracker2.py` (hooks.csv + CHANGELOG, six rows), steps in
`make_steps2.py` / `make_steps3.py`. The allowlist count understates the item (see "What the lint cannot see").

### What each of the 12 keys was

| RVA | before | what it really was | action | now |
|---|---|---|---|---|
| 0x00417180 | DUP-IN-TARGET AiControllerAB + AiPreTick | `AbAiBank` is an A/B driver, not a body. The real pair was invisible to the scanner: `AiStandalone.cpp` `BankSwitch` (audit verdict UNREVIEWED, row C3 with `exe_file != file`) | compared line by line, same state machine; shared TU `Ai/AiBankSwitch.cpp` | **one body, C3 re-earned** |
| 0x0040e480 | DUP-IN-TARGET AiStandalone + RaceResults | two real copies of a 3-line setter (.asi installed one in `Frontend/RaceResults.cpp`, the exe's private one with a null-base guard) | shared TU `Ai/CarSlotState.cpp`, guard kept in the one body | **one body, C3 re-earned** |
| 0x0046d510 | DUP-IN-TARGET AiTargeting + VehicleVelocityWorldGet | `AiVehicleVelocity3`: dead (installer removed 2026-09-01, no callers, same call as the survivor) | body and its helper deleted | gone |
| 0x00418860 | DUP-IN-TARGET AiController + AbAiTick + ScenarioLeaves_sa2 | `AbAiTick` = driver. `AiPerFrameTick` in `HUD/ScenarioLeaves_sa2.cpp`: never installed, no callers, compiled into BOTH targets and calling original VAs (garbage in the exe) | body, 7 callee pointers and 2 consts deleted; driver re-anchored | gone |
| 0x00418560 | DUP-IN-TARGET AiController + AbAiVehStep | `AbAiVehStep` = driver | re-anchored | gone |
| 0x004a2c48 | 2 DUP-IN-TARGET + 1 CROSS-TARGET | `FtolFrom` is a thunk INTO the original (`push 0x004a2c48 / ret`); `SetupScreenRenderers.cpp` was a function-pointer typedef comment; the real body `Math/FPURound.cpp` was never anchored | re-anchored both, anchored the verbatim body | 3 keys -> 1 true key |
| 0x004177b0 | DUP-IN-TARGET (AbAi + HookSystem + AiPreTick) + CROSS-TARGET | a prologue-bytes comment above a `void*` decl and prose in `HookSystem.cpp` | re-anchored | 2 keys -> 1 true CROSS-TARGET |
| 0x00417640 | DUP-IN-TARGET AiController + AbAiPost + AiStandalone | `AbAiPost` = driver; the real pair is AiController vs AiStandalone | re-anchored | 1 true key kept |
| 0x0040e350 | DUP-IN-TARGET PromoLoop_round1 + Vehicle/PhysicsChainHooks | one body is in `Vehicle/` | **skipped** (d2posmodel's lane) | unchanged |

Re-anchoring was comment edits only (no scanner exemption, no allowlist silencing): the scanner binds a body to a comment whose
first token after `//` is the RVA, so `// 0x00417640 AbAiPost ---` became `// AbAiPost (A/B driver installed AT 0x00417640; harness, not a
port body)`. Files: `AiControllerAB.cpp`, `Core/HookSystem.cpp`, `Frontend/SetupScreenRenderers.cpp`, `Ai/VehicleFloatFieldAsInt.cpp`,
`Math/FPURound.cpp` (the one ADDED anchor, which the hook-authoring convention already required).

### Consolidation 1: 0x0040e480 CarSlotStateSet -> Ai/CarSlotState.cpp

One body in `exe_sources.rsp` and `asi_sources.rsp`, installed in the .asi. `Frontend/RaceResults.cpp`'s copy and
`AiStandalone.cpp`'s inline copy are gone (AiStandalone keeps a one-line forwarder so its call sites are untouched). The standalone's
`base == 0 -> return` guard is in the one body: the image-pad zero-fills the original's `.data` where `*(u32*)0x005f2770` is
`0x005f2728`, so the guard is unreachable inside `MASHED.exe`. In the exe the guard is also moot by default, because
`TrackRenderer.cpp:446` seeds that pointer unless `MASHED_NO_SLOTSTATE_SEED`, so the G-CTRL run below executes the write path of the
shared body.

### Consolidation 2: 0x00417180 AiBankSwitch -> Ai/AiBankSwitch.cpp

The row was C3 with `exe_file = AiStandalone.cpp` and an audit verdict of UNREVIEWED, so the C3 covered the .asi copy only. Compared
the two line by line: the same state machine (addresses `0x0089a500` request, `0x0089a504` timer, `0x0089a4cc` line type,
`0x0089a4d0` spline index, `0x008022ac` / `0x00801ca0` / `0x00802ec4` + idx * `0x204` bank counts, `0x007f1008`, `0x0089a368`,
`0x007f0ff8` / 3000, `0x0063bd90`), same branch order, same read-once / re-read pattern, same 9000 cap. The body kept is
`AiPreTick.cpp`'s, the one the evidence cites. Two things cannot be the same in both targets and are the two thunks of the file:
- the reset, `FUN_00414030(-1)`: inline loop writing 1000 into **four** slots `0x008032d4 + i*0x14`. Read from the unpatched
  original, `0x00414039..0x0041404e`: `mov eax,0x8032d4 / mov [eax],0x3e8 / add eax,0x14 / cmp eax,0x803324 / jl`, and
  `0x803324 - 0x8032d4 = 0x50 = 4 * 0x14`. (hooks.csv's "resets all 5 slots" on that row is wrong, noted there.) It is what the exe's
  `SplineBankTimerReset` did too, and it needs no original code, so it is the same in both targets;
- the random roll, `FUN_00472650(0, 1.0f)`: the original function in the .asi, `Ai_RandRange` (the verbatim `FUN_00534870` /
  `FUN_00534990` ring port) in the exe. Same law, not the same draw index, so the exe keeps no per-draw evidence.
`_DAT_005cc32c` (0.5f) is a literal, as in the exe's old copy. AiStandalone's `BankSwitch`, `SplineBankTimerReset` and `RandUnit` are
deleted and its `VehicleStep` calls `::AiBankSwitch`.

### Evidence (evidence build `build_partc2.log`, final tree `build_final.log`: both targets OK, rva-lint NEW=0, asm-lint OK)

All device runs went through `verify/dupinstall2_20261016/window.py` (no foreign MASHED.exe when each window opened, shared
`original\mashed_re_dev.asi` backed up and restored in a `finally`, sha256 `270e036f...` before and after, both windows
`matches original: True`). Windows 2 and 3 deployed the worktree build `8cfbae10...`.

| step | result | file |
|---|---|---|
| **0x0040e480** path1 `car_slot_state_set`, hook bypassed, `ptr_ptr_entity_set` | **GREEN 10/10** | `win2/path1_car_slot_state_set.txt`, `log/diff_car_slot_state_set.csv` |
| path2 `car_slot_state_set`, first run | install OK (E9 + rel32 to the new export, site verified) but the call-through died `bad argument count`: the registry's `path2_tests` were bare `[0, 1]`, which the verify template does not apply (it applies `{'scalars': [...]}`). A harness vector-shape bug of the kind dupinstall2 found for 8 other keys, not a defect in the body. Fixed in `re/frida/hooks_registry.py` | `win2/path2_car_slot_state_set.txt` |
| path2 re-run, fixed shape | **PASS**: E9 + rel32, `reimpl interceptor fired 2x as expected`, 2/2 calls returned | `win3/path2_car_slot_state_set.txt` |
| **0x00417180** `AbAiBank` in a live race, track 0, 4 cars, 120 s (the row's recipe: `--track 0 --cars 4 --rule 0 --hooks AbAiBank`), MINE = the shared body, ORIG committed | **>= 9217 calls, mism=0, rawmism=0**, coverage req=93 timer=1968 rand=2 | `win2/ab/AbAiBank.track0.log` |
| same, track 3 | **>= 10497 calls, mism=0, rawmism=0**, coverage req=2605 timer=4728 rand=181 | `win2/ab/AbAiBank.track3.log` |
| nav boot to a race, FULL hook set (no skip) | `drive_to_race=True`, process alive, no detach | `win2/nav_probe_full_hookset.txt` |
| nav boot to a race, ONLY `CarSlotStateSet` live (the original's AI tick calls it) | `drive_to_race=True`, alive | `win2/nav_probe_only_CarSlotStateSet.txt` |
| nav boot to a race, ONLY `AiBankSwitch` live | `drive_to_race=True`, alive | `win2/nav_probe_only_AiBankSwitch.txt` |
| full hook set, 45 s | 1331 hooks seen, 1290 installed (the same numbers as Part A, nothing gained or lost), `CarSlotStateSet` (index 645) and `AiBankSwitch` (646) among them, survived | `win2/install_observe_all.txt`, `win2/install_all/hook_install_manifest.txt` |
| **exe G-CTRL**, `run_gctrl_private.py`, baseline = the exe built from `81b2add0` (this lane's Part A/B commit, same exe sources as d074095d) | `716cb2fd` / `2a2afda1` / `ba90ed88` | `gctrl_base_a.log` |
| after CarSlotState + the dead-body deletions | `716cb2fd` / `2a2afda1` / `ba90ed88` | `gctrl_after_a.log` |
| after BOTH consolidations (final exe) | `716cb2fd` / `2a2afda1` / `ba90ed88`, line counts 18664 / 56153 / 28076 identical | `gctrl_after_b.log` |

What this does and does not show:
- C3, not C4: the A/B driver commits the ORIGINAL's result, so the port never steers the canonical scenario (the driver's own
  header says so). The ONLY-hook nav boots prove install, survival and a race reached with the hook live, not a counted
  canonical-scenario diff, so no C4 is claimed.
- The exe's `0x00417180` runs the same body, but its random-roll thunk is the ring port `Ai_RandRange`, which this evidence does not
  cover (same law, not the same draw index). Its reset thunk is the inline loop the A/B did cover.
- By call order the G-CTRL scenario (Training, rule 4) runs both shared bodies in the exe: `VehicleStep` calls `::AiBankSwitch(v)` before
  every `ControlStep` and `step.csv` records `ControlStep` (18664 lines), and `Ai_Standalone_Tick` calls `CarSlotStateSet` for alive cars
  when the rule selector is 4, with the slot-state pointer seeded by default (`TrackRenderer.cpp:446`). The calls were not counted
  separately. The unchanged hashes are a regression guard on the exe, not evidence against the original.
- The final source tree differs from the evidence build only in comments (`AiControllerAB.cpp` header, `ROADMAP.md`) and the
  registry path2 shape; it was rebuilt before commit (see `build_final.log`).

### The three that stay, and why none is a mechanical merge

- **0x00417640 PostStepPowerupBrake** (AiController.cpp vs AiStandalone.cpp). The exe copy pins `rate = 0.0f` ([U-C-RATE1]) so its
  brake branch cannot fire and it omits `FUN_0046d570`; the .asi copy calls the real `FUN_0046d6d0` and can apply full brake. Opponent AI
  on track index 0x21 never full-brakes in the exe (hooks.csv demotion, 2026-09-29). Merge needs the Host to supply both floats
  (`veh_f32(v, 0x9e4)` already exists for `FUN_0046d6d0`; nothing for `FUN_0046d570`, whose port is exe-only in `Ai/AiLeafTargets.cpp`)
  and changes exe behaviour on track 0x21, which the G-CTRL scenario cannot be assumed to cover. It is the next candidate for the
  AiBankSwitch recipe with three thunks (track index, rate, range).
- **0x004177b0 AiPreTick** (CROSS-TARGET AiPreTick.cpp vs RuleEngine.cpp). `RuleEngine.cpp:UpdateFinishOrder` is a fragment (its own
  comment says so). The exe's whole-function copy, `AiStandalone.cpp:AiPreTickRubberBand`, is invisible to the scanner and is now a
  structural port of all five phases, so the 2026-09-29 demotion note listing the speed scaling and the 0x0089a368 machine as missing is
  stale on that point. What it still reads from places no exe TU writes (`0x008a9648` / `0x008a96ec`, the leaves `FUN_00407a20` /
  `FUN_00408ad0`) is an INPUT problem (race-metric producers, the u9685 area), not a body problem, and the .asi body reaches the
  original through eleven call-through helpers (`RandFloat`, `TargetEnable`, `SpeedBaseGet`, `VehSpeedSet`, `PerVehA20`, `RaceAngleRaw`,
  `RaceAngleGet`, `PowerupAlive`, `VehType`, `SubGameMode`, `Ftol`) that the exe replaces by host calls or direct reads. Not compared line
  by line in this lane.
- **0x004a2c48 `__ftol`** (FPURound.cpp vs RaceCamera.cpp). Verbatim naked x87 body (C3, installed in the .asi, compiled into the exe with no
  caller) vs `static_cast<int>` in `RaceCamera.cpp:Ftol_4a2c48`. RaceCamera.cpp's own comment records them agreeing on the camera's range
  (measured 2026-08-26); out-of-int-range and NaN inputs were not compared [UNCERTAIN]. Outside the AI lane (a camera file) and needs a camera
  parity run, so left.

### What the lint cannot see

`scripts/lint_rva_bodies.py` only counts bodies with an RVA comment directly above the definition. `AiStandalone.cpp` has none above
its copies of VehicleStep, the tick, ControlStep x3, steering, PostStep and PreTick, so `NEW=0` says nothing about them. The measure
that is not blind: `hooks.csv` rows at C3/C4 whose `exe_file != file`. For the AI area that was **3 rows** at the start of this lane
(0x0040e350, 0x00417180, 0x0040e480). **0x00417180 and 0x0040e480 are now closed by consolidation; the third is the Vehicle/-side
`0x0040e350`.** Every other AI row with `exe_file != file` is already C2 from the 2026-09-29 demotions, so no further demotion was
needed. The remaining AI pairs (VehicleStep 0x00418560, the tick 0x00418860, ControlStep x3, steering 0x00415e20) are the D3 work the
roadmap names: each exe copy still has its documented substitutions, and the .asi ports call original callees that the exe lacks, so
they need the same host-thunk treatment with the in-process A/B drivers as the judge.

### Skipped (and why)

- Anything in `Vehicle/` or `Collision/` (d2posmodel): `0x0040e350` (`Vehicle/PhysicsChainHooks.cpp` holds one body).
- `0x00418560` / `0x00418860` real pairs, ControlStep family: need D3 callee ports, see above.
- Not touched: u9683 / u9685 files (`TrackRenderer.cpp`, `Race/SpectatorDistances.cpp`).
