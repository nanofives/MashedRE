# PREREG D4-WSB-55B080-ULP: 0x00546b10 port precision fix, re-run of the 0x0055b080 arm with the port bound

Committed alone, before every arm below. Branch `orch/D4-WSB-55B080-ULP` off `orch/integration` @ `6d9d01a7f`
(carries D4-WSB-PORT-BROAD-2, merged at `3a3186c4a`). Code under test: `e56e0cb3d`.

## Finding being fixed

D4-WSB-PORT-BROAD-2 arm S pass 1 (`verify/d4wsb_portbroad2_20261101/shadow_summary_pass1.tsv`, raw
`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/0055b080_pass1_superseded/`): `0x0055b080` with the `0x00546b10` port bound,
DIVERGENT 4/24 (samples 16-19), return equal, 1-2 dwords per sample at `+0x770/+0x774` and `+0x780/+0x784` of one heap
page, each 1 ulp (`bbfc4cb0` vs `bbfc4caf`), 16-byte stride. Binding the original `0x00546b10` gave CLEAN 24/24, so the
difference comes from the `0x00546b10` port.

## Static cause (no run needed for this part)

- `Collision/RwpSolverMath2.cpp` is not in `mashedmod/x87_tus.txt`, so it compiles `/arch:SSE2`. Its `float10`
  (`long double` = 64-bit double on MSVC) temporaries were computed in SSE2 double.
- The original (capstone over `original/MASHED.exe.unpatched`, 2026-11-01) keeps the chain on the x87 stack:
  trace `fld [esi+0x28]; fadd [esi]; fadd [esi+0x14]` (`0x00546b23..0x00546b2d`), `fcom [0x5d757c]`, `fadd [0x5cc320]`,
  `fstp [esp]` to FastSqrt (`0x00546b46`), `fld 0.5; fmul st(1); fstp [edi+0xc]`, `fdivr [0x5cc32c]` (`0x00546b5e`),
  then three `fld; fsub; fmul st(1); fstp` rows. The race thread runs CW `0x007f` (PC_24, U-9220 measurement), so every
  one of those ops rounds to 24 bits. In the SSE2 port `0.5/s`, each `m[i]-m[j]` and the trace stayed 53-bit until the
  final float32 store. The X / Z siblings `0x00546bf0` / `0x00546cb0` have the same shape (`fdivr` at `0x00546c22` /
  `0x00546ce3`). `0x00546c50` already had this fix (U-9220, `RwpSolverMath2QuatY.cpp`, user ruling 2026-10-11).
- Fix (`e56e0cb3d`): the three bodies move unchanged to `Collision/RwpSolverMath2Quat.cpp`, listed in `x87_tus.txt`.
  `L2_0055b080.cpp` binds the port again (the amendment-1 raw thunk is removed). The new objects carry 41 / 24 / 24 x87
  ops and 0 SSE ops for the three bodies on both targets, and the `0x00546b10` codegen follows the original's op tree.

## Classification

Faithful correction (the port now follows the thread's precision control as the original does), the U-9220 precedent.
No knob. On the standalone the thread runs PC_53 (`RwpSolverMath2.cpp` u9776 note), where x87 and SSE2 compute the
same double chain, so the default gates are predicted unchanged. If gate G differs from `merge_train.json`, this
classification is wrong: the change gets a default-OFF knob and a PREREG amendment before any further arm.

## Arms

Scenario for S/C = the census one used by D4-WSB-PORT-BROAD-2: `scenario_launch.py --track 0 --mode 10 --cars 4
--hold 60 --hooks 0x<rva>`, `MASHED_SHADOW_AB=1`, hook installed alone, private root
`D:/MashedWT/D4-WSB-55B080-ULP_root`, 24 samples (`SAMPLES_DONE=24`). Runner `run_shadow10.py` (copy of
`verify/d4wsb_portbroad2_20261101/run_shadow9.py`, data path, label and summary changed), driver `drive_ulp.py` (copy of
`drive_pb2.py`, runner, log path and label changed; solo window per row, audio endpoint check before and after,
crash events read after). Verdicts from `re/tools/shadow_ab_report.py`.

| arm | build | hook | prediction |
|---|---|---|---|
| S1 | `e56e0cb3d` | `0x0055b080` (port `0x00546b10` bound) | CLEAN 24/24, ndiff 0, A/B-IS-REAL |
| S2 | `e56e0cb3d` | `0x00546b10` | CLEAN, ndiff 0 |
| S3 | `e56e0cb3d` | `0x00546bf0` | CLEAN, or no samples (branch not reached in the window) |
| S4 | `e56e0cb3d` | `0x00546cb0` | CLEAN, or no samples |
| C  | control: `e56e0cb3d` with the `RwpSolverMath2Quat` line removed from `x87_tus.txt` (SSE2 codegen, same source), built locally, never committed | `0x00546b10` | DIVERGENT on >= 1 sample, 1-ulp in the out quaternion |
| G  | `e56e0cb3d` exe | default gates, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo lock, `A4_ROOT` = the private root | all six equal `merge_train.json` (G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3) |

Order: S1, S2, S3, S4 on the fix `.asi`, then C (control `.asi` copied into the root, fix `.asi` restored after), then G.

## Read-out

- **GREEN**: S1 CLEAN 24/24 and S2 CLEAN, G equal to refs. C DIVERGENT shows the arm resolves the 1-ulp difference.
  If C is CLEAN, S2 is uninformative and the verdict rests on S1 against pass 1 (same scenario, same body except the
  `0x00546b10` binding codegen); the RESULT says so.
- **RED**: S1 or S2 DIVERGENT with a differing dword in a quaternion `0x00546b10` writes. The x87 hypothesis is
  then refuted or incomplete; no further fix inside this step without an amendment.
- S3 / S4 with no samples are reported as not exercised, not as a failure.
- Any other DIVERGENT dword (outside the quaternion) is reported with its offset and read out, no re-fix in this step.

## Stop / re-run rules

- U-9677 piz collision (`+0x1509b4`, `+0xe66db`): re-run that row (max 2 re-runs). A launch failure (NO_LOG with no game
  output) is re-run once and recorded.
- Audio endpoint missing before launch, dropped during the run, or a fault at `0x005baf48`: stop, BLOCKED, no retry.
- Machine lock: `mashed_lock.py status` before each window; no solo window while a merge train is PRIORITY-PENDING.

## Tracker transaction (re-classify, after the arms)

No level change predicted. `hooks.csv` `file` column of `0x00546b10` / `0x00546bf0` / `0x00546cb0` moves to
`Collision/RwpSolverMath2Quat.cpp` with a dated note citing this RESULT. `0x0055b080` gets a dated note that the port
`0x00546b10` is bound again (S1). No C4 (no race diff with the hook installed, `re/CONFIDENCE.md` write-set leg).

## Files

Committed: this PREREG, the RESULT, `shadow_summary.tsv`, the two runner copies. Raw (not committed):
`D:/MashedWT/D4-WSB-55B080-ULP_data/` (per-row `shadow_ab.log`, transcripts, build logs, gate outputs), listed with
SHA-256 in the RESULT.

## Amendment 1 (2026-11-01, after S1 / S2 / S3, before S1a / S2a / Ca / G)

Two harness defects make S1 and S2 vacuous for the quaternion. Neither is read as evidence.

1. **S1 env.** `run_shadow10.py` / `drive_ulp.py` did not set `MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, which the
   PORT-BROAD-2 PREREG sets on every boot (its "Env on every boot" line). Without `MASHED_SHADOW_PRIVATE` the page tracker
   in `Core/ShadowTrack.h` (`priv_on`, line ~456) protects no heap page, so the quaternion table (heap) is not compared.
   S1's log shows `pages=o:0,n:0` on all 24 samples where pass 1 had 9 and pass 2 had 7. S1 (CLEAN 24/24) is void.
2. **S2 wrapper.** The generated `0x00546b10` wrapper used `ShadowAB::Run`, which compares only the return
   (`Core/ShadowAB.h:38`). The return is 1 on every non-null pair (`mov ebx,1` at `0x00546b28`, `mov eax,ebx` at
   `0x00546b59` / `0x00546bba` / `0x00546be0`), so S2 (CLEAN 48/48) never compared the output. The same holds for the row's
   existing C3 evidence (48/48 via `Run`, `log/shadow_ab/shadow_ab_20260910_001251_16x_00407600.log`). Fix in this commit:
   the wrapper calls `RunRegion` over the 0x10-byte quaternion at `param_1` (null args take the plain path, as before).
   Inert unless `MASHED_SHADOW_AB` is armed, so no default change.

S3 (`0x00546bf0`): the full 60 s race ran, no sample, so the X branch is not exercised in this scenario (reported as such).
S4 runs as planned on the first `.asi` (its wrapper is `RunRegion`, not affected by either defect).

Re-run arms, env on every boot `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, same scenario and
runner (env added in the runner's invocation, nothing else changed), new `.asi` built from this commit:

| arm | build | hook | prediction |
|---|---|---|---|
| S1a | this commit, x87 | `0x0055b080` | CLEAN 24/24 with heap pages tracked (`pages` > 0, as pass 2) |
| S2a | this commit, x87 | `0x00546b10` (RunRegion) | CLEAN, `fields=0/4` on every sample |
| Ca1 | this commit, `RwpSolverMath2Quat` removed from `x87_tus.txt` (SSE2), never committed | `0x00546b10` | DIVERGENT on >= 1 sample, `fields` 1-4 of 4 |
| Ca2 | same control | `0x0055b080` | DIVERGENT on >= 1 sample (reproduces pass 1) |
| G | this commit's exe | default gates | equal to `merge_train.json` |

Read-out as in the main PREREG, with S1a / S2a / Ca1 / Ca2 in place of S1 / S2 / C. If a control arm is CLEAN, the
paired fix arm is uninformative and the RESULT says so. Row keys carry `SHADOW_TAG` suffixes `a1` (fix) and `ctl`
(control) so the void S1 / S2 rows stay in `shadow_summary.tsv`.
