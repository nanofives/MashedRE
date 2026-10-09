# RESULT: D-11073 leg 1a (exe substrate, INERT in the default build)

Date 2026-10-09. Brief: `re/BRIEF_D11073.md` §2 "Leg 1a". Pre-registration: `PREREG_LEG1A.md`, committed
`e2679752` before any code. One post-commit amendment, before any run: the two new arms were renamed
`L1A_A` / `L1A_SUB` (were `H4_A` re-run / `H4_SUB`) so the committed H4_A files stay as the reference.
No default flip. No `FUN_0040dbd0` / `FUN_004103a0`, no `aib_game_sub_mode` change. C-levels unchanged (C3).

## 0. Verdict first

> **Leg 1a is done and the default build is byte-identical.** All pre-registered gates PASS, with two
> harness findings reported in §3 (neither is caused by this leg).
>
> - Default build (`L1A_A`) and `MASHED_SUBSTATE=1` (`L1A_SUB`) both give step `c741a4c5`, gates
>   `642af0ac`, ptrace `05984e30`.
> - Under the knob, `DAT_0063d588` equals the float32 running sum of the step on **1288/1288** live
>   samples, bit-exact (no torn reads), step bits `0x3c888888` on 1287/1287. With the knob off, all
>   three globals are 0 on 1543/1543 samples while the clock advances.
> - Seven callees now have one body each in `Race/SubStateCallees.cpp` (both `.rsp` files). path1
>   GREEN on every one, path2 E9+rel32 to the new export on every one.
> - **Owed original columns:** `DAT_0067ea64` = 0 on every sample, so `FUN_0040dbd0` (case 10) takes the
>   `FUN_0041b520` arm. `0x00882194 + v*0xd04` = `0, 250, 250, 250` on the last phase-5 sample and
>   `0, 1100, 1100, 1100` on the first phase-6 sample; `0x00882198` = 0 -> `0, 1, 1, 1`.

## 1. What changed

| file | change |
|---|---|
| `Race/SubStateCallees.cpp` / `.h` (new; E+A) | one body each: `0x0041d910`, `0x0041da90`, `0x0041ae20`, `0x0041b520` (now C, calls `Copy41ae20` directly; the asi copy called the raw `0x0041ae20`, outside the exe's mapped text), `0x0046baa0`, `0x0046c750`, `0x0046c730`. Plus the knob `SubState_Enabled()` and two PARTIALS: `SubState_StoreStep` (`FUN_0040fc00` `0x0040fc09..0x0040fc57`) and `HudSlideBillboardTimerTail` (`FUN_0041d930` `0x0041da5f..0x0041da76`) |
| `HUD/ScenarioLeaves_sa2.cpp` | `HudSlideBillboardTick` (whole `0x0041d930`) now ends with `HudSlideBillboardTimerTail()`. One copy of the tail |
| `Ai/AiStandalone.cpp` `Ai_AdvanceClock` | `SubState_StoreStep(units)` (no-op unless `MASHED_SUBSTATE`) |
| `D3d9Render/TrackRenderer.cpp` | `if (SubState_Enabled()) HudSlideBillboardTimerTail();` after `Ai_Standalone_Tick` and its two dumps, inside the `faithful_nav` block (step store precedes the tail in a frame, as `0x0040fc57` precedes the call at `0x0040fd11`) |
| `Util/PromoLoop_round10/20/39.cpp`, `PromoLoop_sessionB.cpp` | old copies removed, pointer comments left |
| `Util/PromoLoop_round38.cpp`, `round80.cpp` | deleted (emptied); off `asi_sources.rsp`. round38 held the dead `Table88219xGet` copies (refused at install since the 2026-09-09 audit) |
| `re/frida/hooks_registry.py` | `table_882194_get` / `table_882198_get` repointed to the installed exports. NEW key `vehicle_physics_state_reset_obs` (§3.2) |
| `re/tools/orig_rampwatch.py --cam` | columns `team_ea64`, `dt_100c`, `c2194_v`, `c2198_v`, records `r<k>_i64/f50/f54/f58/i70` |
| `run_h4.ps1` | arms `L1A_A`, `L1A_SUB`; `MASHED_SUBSTATE` cleared between arms |

`FUN_0041ae20` (the one unsurveyed callee) was already ported and C3: `Copy41ae20` matches the listing
store for store (`PREREG_LEG1A.md` §1).

## 2. Gates

| id | result | evidence |
|---|---|---|
| G-BUILD | **PASS** | `build.bat` exit 0; exe 00:09:56, asi 00:10:04; all edited TUs' objs fresh |
| G-LINT | **PASS** | `lint_rva_bodies.py`: `NEW=0` (118 known, was 122: the two removed dual installs). Exactly one `RH_ScopedInstall` per RVA for all eight |
| G-CTRL | **PASS** | `L1A_A`: step `c741a4c5` (19419 lines), gates `642af0ac` (53993), ptrace `05984e30` (26996). `L1A_RUN.txt` |
| G-INERT | **PASS** | `L1A_SUB`: same three sha8. `[substate]` line in `L1A_SUB.log` (1), absent from `L1A_A.log` (0): the knob took |
| G-STEP | **PASS** | `L1A_STEP_SCORE.txt`: (a) 1287/1287; (b) exact 1288, torn 0, other 0; (c) median 0.016666412 (\|d\| 2.9e-7). Last sample n=1306, t=21.7665. `L1A_step_SUB.csv` |
| G-STEP-CTL | **PASS** | `L1A_step_OFF.csv`: 0 non-zero rows of 1543, 1263 distinct clock values |
| G-PATH1 | **PASS** | `L1A_PATH1.txt` and `log/diff_*.csv`, per RVA below |
| G-PATH2 | **PASS on the E9 criterion for all 8**, call-through PASS for 5 | `L1A_PATH2.txt`, per RVA below |
| G-ORIG | **PASS** (G-NOPRESS repeat) + values reported | `L1A_ORIG_SCORE.txt`, `o_leg1a.csv` |

Per RVA (path1 / path2):

| RVA | key | path1 | path2 |
|---|---|---|---|
| `0041d910` | `clear_63d584_pair` | run_diff GREEN 10/10 | PASS 3/3 |
| `0041da90` | `delta_time_out_get` | run_diff GREEN 10/10 (race; live value `0x405bbbaf` constant across vectors, as at its C3) | E9 OK; call-through not run ("expected a pointer", harness arg shape) |
| `0041ae20` | `copy_41ae20` | early_window GREEN 6/6, varied | E9 OK; call-through not run ("expected a pointer") |
| `0041b520` | `update_41b520` | early_window GREEN 2/2, varied | PASS 2/2 |
| `0046baa0` | `vehicle_physics_state_reset_obs` / `vehicle_physics_state_reset` | run_diff GREEN 7/7, a distinct fingerprint per slot, slot 16 = slot 0xffffffff (no store) | PASS 4/4 (int key; returns 1,1,0,0) |
| `0046c750` | `table_882194_get` | early_window table-seed GREEN 10/10 (`0xc0de0000|i`, 0 for 16/17) | PASS |
| `0046c730` | `table_882198_get` | early_window table-seed GREEN 10/10 | PASS |
| `0041d930` | `hud_slide_billboard_tick` | run_diff GREEN 10/10 (race; S -> S+1 on 10 sentinels) | E9 OK; call-through AVs at `0x10` at the menu, and the ORIGINAL does the same there (`L1A_D930_ORIGCTL.txt`: hooks off, `DAT_0063d580` = 0, 3/3 "access violation accessing 0x10") |

What the gates cannot see: no moved body is called by the exe in 1a, so path1/path2 prove the
transcription and the install, not exe-side behaviour. `hud_slide_billboard_tick` observes `d584` only;
the `d588` half of the tail is covered by G-STEP (bit-exact in the exe).

## 3. Findings outside the pre-registration

### 3.1 The full auto-hook boot crashes (pre-existing, not this leg)

Every `run_verify_hook.py` call first failed at attach (`VirtualAllocEx` 0x5). Cause: with the full
canonical hook set installed, `MASHED.exe` AVs during boot (WER dump `MASHED.exe.27024.dmp`: read of
`0x1c` at `eip 0x004c7785`, ret chain `0x004ccefa 0x004ccb4e 0x004d801d 0x004c7853 0x0042d535 0x0042d593
0x004923ad 0x004a4d38`), and WER holds the process while the dump is written. Controls, own PIDs:

| arm | result |
|---|---|
| full auto-hook | exit `0xC0000005` by 5-8 s |
| `MASHED_RE_NO_AUTO_HOOK=1` | alive at 12 s |
| full set **minus the 8 leg-1a RVAs** (`MASHED_HOOK_SKIP`) | exit `0xC0000005` by ~4 s |
| **only** the 8 leg-1a RVAs (`MASHED_HOOK_ONLY`) | alive at 12 s |
| `run_verify_hook bounded_table_signclamp_46d7f0` (untouched hook, last OK 2026-10-01) | same attach failure |

So path2 was run per hook with `MASHED_HOOK_ONLY=<rva>`, which still checks the E9 at the RVA lands on
the new export. [UNCERTAIN] which installed hook causes the boot AV and since when: not bisected here.
It blocks every full-set path2 and any `--hooks all` canonical run. Recommend a bisect with
`MASHED_HOOK_LO/HI` as its own task.

### 3.2 `vehicle_physics_state_reset`'s old GREEN saw only the return value

The existing key is `int_scalar` (returns 1 or 0), so its C3 never observed the stores. The new
`_obs` key does (5 slot windows of `0xd04`, pre-filled `0xAA`, plus the return) and is GREEN. Also:
`early_window_leaf_diff.py`'s `scalars_to_scattered_globals` handler calls with NO args and reads one
dword per window (ignores `len`, `fill`, `fold_ret`), so a GREEN from it on this key was degenerate
(all seven cases identical); it is recorded as VOID in `L1A_PATH1.txt` and the `run_diff.py` run is
the evidence. The `entity_*` race keys came back INCONCLUSIVE-DEGENERATE on run_diff (live table all
zero); the table-seeded keys are the evidence for those two RVAs.

### 3.3 Original capture (`o_leg1a.csv`, 2,080 samples, `--statediff-out`)

- Sub-states `0 (341) -> 2 (5) -> 3 (659) -> 5 (107) -> 6 (208) -> 7 (760)`; phase 4 not caught by the
  60 Hz poll (one frame). G-NOPRESS PASS: 0 non-zero input bytes in phase 3, span 692 frames.
  Phase-3 length this run: 692 (leg 0 runs: 651 / 652 / 730), again different.
- Phase 5 again ends at `n_d584` = 112, `t_d588` = **1.8666651** (the predicted float32 value, exact).
  `dt_100c` = 0.016666666 on all 974 samples of sub-states 3/5/6.
- `DAT_0067ea64` = **0** on every sample in every sub-state, `DAT_0067e9fc` = 10, `DAT_007f0fd0` = 0.
  `FUN_0040dbd0` case 10: `FUN_0042f500()` = 0 -> **`FUN_0041b520`**, then `DAT_0063ba8c = 5`.
- Records `0x0063c8d0 + k*0x74`: before phase 4, `+0x64` = k (0,1,2,3) and `+0x50..+0x58` = 0. On the
  first phase-5 sample: `+0x54/+0x58` = row k of `0x005f3304` exactly (±0.35, 1.0; table is file-backed,
  rows `(±0.48, ±0.35, 1.0)`), `+0x70` = 0, `+0x50` = ±0.945, then -0.015 per frame (0.93, 0.915, ...)
  to ±0.465. `2 * 0.48` = 0.96, so `+0x50` has already moved one frame by the first sample.
  [UNCERTAIN] the per-frame writer of `+0x50` (not read; `FUN_0041b540`, called at `0x0040fdd4`, is a
  candidate only).
- `0x00882194 + v*0xd04`: phase 5 rises to `0, 200, 200, 200` then `0, 250, 250, 250` on the last
  phase-5 sample (n=112), `0, 1100, 1100, 1100` on the first phase-6 sample, then -200 per sample
  (900, 700). `0x00882198`: `0,0,0,0` in phase 5, `0,1,1,1` from the first phase-6 sample.
  [UNCERTAIN] whether `FUN_004103a0`'s exit arm reads these before or after the 1100 write in its own
  frame; a poll cannot order them.
- Harness slip: `--statediff-out` was given a directory, so the side `.msd` failed to write
  (`[Errno 13]`, `o_leg1a.log`). The flag still took (presses skipped; G-NOPRESS PASS). Leg 0 passed a
  `.msd` path; do that next time.

## 4. For leg 1b (predictions owed before writing `FUN_0040dbd0` / `FUN_004103a0`)

- In the standalone, records `0x0063c8d0..` are BSS: `+0x64` reads 0 for all four unless something
  seeds it, so a faithful `FUN_0041b520` would copy row 0 into all four records where the original
  has k = 0,1,2,3. Read the standalone's `+0x64` first; [UNCERTAIN] the original's writer of `+0x64`.
- `0x00882194/98 + v*0xd04` read 0 in the standalone (RESULT_LEG0 §3). The exit arm would copy 0 where
  the original copies 250/1100 and 0/1. Find their writer before porting the exit arm.
- Census readers of `0x0063c8d0..0x0063caa0` and `0x007f0a04/08` in the exe before 1b calls anything.
- The 12000 seed and the alive routing (RESULT_LEG0 §4 items 2, 3) are unchanged by 1a.

## 5. Raw

`L1A_RUN.txt`, `L1A_A.*`, `L1A_SUB.*`, `L1A_step_SUB.csv/.log`, `L1A_step_OFF.csv`, `L1A_STEP_SCORE.txt`,
`L1A_PATH1.txt`, `L1A_PATH2.txt`, `L1A_D930_ORIGCTL.txt`, `o_leg1a.csv/.log`, `L1A_ORIG_SCORE.txt`.
Scorers: `score_leg1a_step.py`, `score_leg1a_orig.py`. Poller: `sa_substate_poll.py`.
Every MASHED / mashed_re PID spawned this session was its own and was killed or self-exited.
