# RESULT D4-DC-INC-0046d4a0-KNOB

**Verdict: DECISION.** The knob is in, default OFF, and the default gates are unchanged. With the knob ON the gates are
also unchanged, but that is vacuous: no live recipe on the current build reaches the call (0 calls in 3 scenarios), and
the only harness that does reach it (the M3 callback replay) runs before the exe allocates the record array, so the
one-body form returns 0 there and the original's code then reads address `0x30`. Per the PREREG, no flip is filed as
GREEN. The flip is filed as a user decision with the evidence and the expected hashes.

PREREG `PREREG_D4-DC-INC-0046d4a0-KNOB.md` (`910c5e6f2`, amendments `b285e6653`, `4f028d98e`). Knob commit `910c5e6f2`.
Exe `mashed_re.exe` sha256 `5e0df7b42210812be2afa6dd6374cf6c9e29257b8dc39947353170308d25fdea`, `mashed_re_dev.asi`
`20b0cd0647027fe76c47ff8f48252fa490efd1ef684ea166a14821b9fe9f9b60` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not
touched). Build log `D:/MashedWT/D4-DC-INC-0046d4a0-KNOB_data/build1.log`. Every arm ran under
`mashed_lock.py run --label D4-DC-INC-0046d4a0-KNOB` (never solo) inside `test-slot`, private root
`D:/MashedWT/D4-DC-INC-0046d4a0-KNOB_root`. No original `MASHED.exe` was booted.

## 1. What changed

- `re/tools/naked_specs/pickups_m3.json`: the `0x0046d4a0` call maps to `PK_46d4a0_sel`. The regenerated
  `Gameplay/PickupCollect_naked.inc` (`re/tools/gen_naked_asm.py`) differs from the committed one by that one call line
  (`0x00458581`).
- `Gameplay/PickupTick.cpp`: `PK_46d4a0_sel`. `MASHED_PK_ONEBODY_46D4A0` unset: `PK_46d4a0` (verbatim). `=1`:
  `Vehicle/VehicleRecordPtr.cpp:PtrCompute881ec8`. `MASHED_PK_ONEBODY_46D4A0_WIT=<path>`: per call, both forms' return,
  pointer and `+0x30/+0x34/+0x38` dwords.
- `verify/savelink_20261023/run_arms.py`: stacks `PK46`, `PK46W`, `DEFW`, `PK46R4W`, `DEFR4W`.
- `rva-lint` 25 findings, NEW=0. `archive_lint.py --ref orch/D4-DC-INC-0046d4a0-KNOB` (`910c5e6f2`) PASS (include-closure,
  rva-lint, asm-lint 971 files). The allowlist key `DUP-IN-TARGET:0046d4a0` stays until the flip.

## 2. Gates

| Gate | Result | Detail |
|---|---|---|
| **G-DEF** | **PASS** | `H_DEF` `8284be46 / ba407a16 / 31fe043e`, `B_DEF_K0/K1/K26` `1d5bf4f9` x3 = `merge_train.json` refs |
| **G-ON** | PASS, **vacuous** | `H_PK46` `8284be46 / ba407a16 / 31fe043e`, `B_PK46_K*` `1d5bf4f9` x3. 0 calls (G-EXEC) |
| G-WIT-INERT | PASS | `H_PK46W` and `H_DEFW` = `8284be46 / ba407a16 / 31fe043e` |
| **G-EXEC** | **FAIL** | no witness file from `H_PK46W` or `H_DEFW`: 0 calls in 14,400 frames (the log shows `R4 pickups: 17 real powerup spawns`) |
| G-EXEC-R4 (amendment 1) | **FAIL** | `H_DEFR4W`, `H_PK46R4W` (round rule 4, no game mode 10): 0 calls each |
| G-ON-R4 | PASS, vacuous | `H_PK46R4W` = `H_DEFR4W` = `346d3c84 / 6bb19800 / 27cb8188` |
| G-RP-OFF (amendment 2) | **PASS** | `RP_DEF`: `rows=3888 exact=3888 entry_bad=0 calls_bad=0`, `arms: bump=60 collect=60 no-op=3522 vanish=246` |
| G-EXEC-RP | 60 calls (knob OFF) | `RP_DEF.pk46.tsv`: 60 rows, idx 0/1/2/3/7 x 12 each |
| **G-READS-RP** | **finding** | every one of the 60 rows: `naked_ret=1`, `naked_ptr = 0x00881ec8 + idx*0xd04` (t = 0), xyz `0/0/0`; `one_ret=0`, `one_ptr=0` |
| G-RP-ON | **not run** | `RP_PK46` was queued 30+ min behind merge-train windows and then cancelled by me (my own PIDs only). Its outcome is fixed by `RP_DEF`'s witness: `one_ret=0` on all 60 bump rows, so the knob-ON replay reads address `0x30` (see below) |

### The G-READS-RP finding

`PickupCb_MaybeReplay` runs at track load (`D3d9Render/TrackRenderer.cpp:3254`), before `VehiclePhysics_Init` sets
`g_vehicleArrayBase` (`Vehicle/VehiclePhysicsRun.cpp:320`). `PtrCompute881ec8` then returns 0 without writing
(`Vehicle/VehicleRecordPtr.cpp`, `if (!rec) return 0`). `FUN_00458570` zeroes its out slot first (`0x00458579`), ignores the
return, and reads `[slot + 0x30]` (`0x00458586..0x0045858d`), so the one-body form reads address `0x30`. The original's
`FUN_0046d4a0` never returns 0 for `idx < 0x10` (`0x0046d4a4..0x0046d4c8`). In a live race the callback runs from the
registry tick with cars registered, after `VehiclePhysics_Init`, so this window is the replay's, not the race's. That is
an argument from code order, not a measurement: no live call was observed on either knob state.

What the knob-OFF exe reads today, from the 60 replay rows: `naked_ptr` lands in blank image pad and the car position it
reads is `0/0/0` on every row. The one-body form would read the record's ring row. Either way `FUN_00458570`'s result is
dead (PREREG section 1), and the replay confirms it on the OFF side: 60 / 60 bump rows exact on entry bytes and call list.

## 3. Raw data (not committed), `D:/MashedWT/D4-DC-INC-0046d4a0-KNOB_data/arms/`

| File | sha256 |
|---|---|
| `H_DEF.step.csv` / `.gates.csv` / `.ptrace.log` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` / `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` / `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `H_PK46.*`, `H_PK46W.step.csv`, `H_DEFW.step.csv` | identical to `H_DEF.*` |
| `B_DEF_K0/K1/K26.step.csv`, `B_PK46_K0/K1/K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` (all six) |
| `H_DEFR4W.step.csv`, `H_PK46R4W.step.csv` | `346d3c84b90079d71b36c1f31c92ad6f5ccca7780d323b6ebf03204e6d46f0dc` |
| `RP_DEF.replay.txt` | `e2c76356a17371d48f367e2e0dffad3a5b419d929047fa397df2d2e714c4fe01` |
| `RP_DEF.pk46.tsv` | `5f2293bc257186c3272f52be96d26650c8db030c3add5028aedb4a12949fa76a` |
| `RUNLOG.txt` (env per arm) | `d00f48c0a9656db420dd8142f0bb4829387d0f617439645e69299afbca23bfb2` |

Committed extracts here: `RP_DEF.replay.txt`, `RP_DEF.pk46.tsv`, `RUNLOG.txt`, `RUNLOG_replay.txt` (small), `score_pk46.py` (scorer), `run_replay.py` (replay runner).

## 4. Decision filed

`roadmap.ps1 decision D4-DC-INC-0046d4a0-KNOB`:

> Flip MASHED_PK_ONEBODY_46D4A0 (one body for 0x0046d4a0 in pickup collect, closes D4-DC-INC-0046d4a0)? Knob ON leaves the default gates unchanged (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3) but no live recipe calls it (0 calls on gate recipe and on round rule 4). The only harness that reaches it, the M3 callback replay, runs before the record array exists, so the one-body form returns 0 there and the bump arm reads address 0x30 (60/60 rows). A flip as-is would break G-M3-REPLAY-SYNTH. Options: (a) new step: move PickupCb_MaybeReplay after VehiclePhysics_Init (or give the replay an allocated array), rerun RP_DEF/RP_PK46 (predict 3888/3888 both), then flip; (b) flip now on the static argument (FUN_00458570's result is dead, 0x004589e2) and accept the replay AV; (c) keep both bodies, allowlist key stays as DELIBERATE with this evidence. Expected default hashes after a flip: unchanged. Recommend (a). Evidence verify/d4dc_0046d4a0_knob_20261101/RESULT_D4-DC-INC-0046d4a0-KNOB.md

## 5. Expected hashes after a flip

Default G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 (unchanged: the call's result is dead, and the knob-ON
arms already match). The flip itself is a code-layout change (the `PK_46d4a0` body and the selector go), so the merge
train's gate run on the flip commit is the confirmation.
