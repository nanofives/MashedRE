# PREREG RO-FIRST-DIVERGENCE (run 44 resume, 2026-11-13)

Committed alone, before any arm of this resume. Branch `orch/RO-FIRST-DIVERGENCE` after merging `orch/integration`
`92fa3f30c` (merge commit `e4c5322d2`). Diagnosis row: nothing here changes default behaviour. Every knob used is
default OFF and read only on the `MASHED_ORIG_TOP` branch: `MASHED_REPLAY*` (RO-EXE-REPLAY-KNOB / RO-EXE-TICK-TRACE),
`MASHED_REPLAY_TRACE_WATCH` (this row, harness only), `MASHED_PORTSTATE_IMAGE` (IMG-2-LOADER), `MASHED_PORTSTATE_FRONTEND`
(PORT-STATE-01, stack `PSF`).

Exe arms use the object build `mashedmod/build/ddkobj/mashed_re_ddk.exe` (L-125), rebuilt on this merged tree.
Root `D:\MashedWT\RO-FIRST-DIVERGENCE_root`, with `original\mashed_image.bin` copied from
`D:\MashedWT\IMG-2-LOADER_root\original\` (SHA-256 `81CC77758FFE93AC0B681EB1FED3D63912D25E7D059503AF6D4F871DEA9AFCD8`).
test-slot outside, `mashed_lock.py run` inside, driver `verify/replayoracle_20261113/firstdiv_arms.sh`.

## The `rec` field

`rec` in a watch hit = the number of `ticks.bin` records written before the store (exe: `PortTop_ReplayTrace.cpp`
`WHit.rec`; original: `detorig.js`, the tick state word `st+4`). The writes between record r - 1 and record r (tick
c = r + 1) are the hits with `rec == r`. `detorig_firstdiv.py` uses `rec` for the frame when hits carry it, else
`tick == c - 1`.

## Arms

| arm | side | config | watch (DR / detorig `--watch`) | purpose |
|---|---|---|---|---|
| A1 `RO_DEF-fdref2` | exe ddk | default RO arm | `0x007f0ff0` | reference for the self-test |
| A2 `RO_RONEG-fdneg2` | exe ddk | stack `RONEG` (+1 on `0x007f0ff0` at tick 400) | `0x007f0ff0` | self-test with `rec` |
| A3 `o4` | original | matched replay (q0 `0x40000000`, warp tick 300 clock 300, 600 records), `--ckpt-ticks 17,18` | `0x00898c00` | `rec` on the original side |
| A4 `RO_ROIMG-fdimg` | exe ddk | new stack `ROIMG` = `MASHED_PORTSTATE_IMAGE=1`, `A4_RO_CKPT=0,1,17,18,299,301,600` | `0x005ea004,0x00898c00,0x007f1004,0x0067eca4` | record 0 with the image ON; who calls the 0x004325c0 bridge |
| A5 `RO_ROIMGPSF-fdimgp` (optional, only if A4 shows the bridge runs) | exe ddk | `ROIMG` + `PSF` | same as A4 | does `0x00898c00` match the original at tick 18 when the records live at `0x00898ac0` |

A4 also needs an image-OFF twin for the census: the existing `RO_DEF-fd2` (object build of run 43) is not the same
exe, so A1 (same exe, image OFF, `A4_RO_CKPT` default incl. tick 1) is the OFF side; A1 gets `A4_RO_CKPT=0,1,17,18,299,301,600`.

Reports: `detorig_firstdiv.py` on (A1, A2) for the self-test, (o2, A1) and (o2, A4) for the record-0 census, tick 18
(`--tick 18`) on (o2 or A3, A4), `detorig_ticks.py` NEW-block verdicts on the same pairs.

## Predictions and what each outcome means

1. **Self-test (A1 vs A2).** First divergent record 399 (tick 400), block `0x007f0c00`, dword `0x007f0ff0`, ref
   `0x00001356` vs run `0x00001357` (or the same +1 relation if the value moved with the rebuild). The A2 watch has
   a hit at the planted pc (`_ReplayTrace_Tick+...`, resolved by `--exe-map`) with `tick` 400 and `rec` 399, and the
   report names it as the last writer in the frame `rec == 399`.
   - PASS: `rec` frame is exact on the exe side; the run-43 gap ("planted write not inside the frame") is closed.
   - FAIL (hit has `rec` 400 or the report names another pc): the `rec` sampling point is wrong; fix the tool before
     using writer attributions. No further claims on writers from this run.
2. **Original `rec` (A3).** The first hit on `0x00898c00` that changes the value to `0x409fffff` is at pc `0x00432763`
   (FUN_004325c0) with `rec` 17, and the report on (A3, A4) `--tick 18` names it in the frame `rec == 17`.
   - Match: `rec` confirmed on the original side. Mismatch by one: the original's tick state is sampled at another
     point than the exe's; record it and fix the tool's original-side rule.
3. **Image ON (A4) at record 0.** Prediction: record 0 still diverges. The FILEINIT-MISSING class (run-43 count 47,577
   `.data` dwords, first `0x005ea004`) drops to 0 or near 0 (left: dwords the original changed before tick 1, which
   then show as BOOT-WRITE or OTHER). The BOOT-WRITE class (224 `.data` + 31,698 bss + 2 `_rwdseg` = 31,924 dwords, run
   equal to the file initializer and the original not) stays about the same, because the image holds the file
   initializers, which is what the exe already holds there. `0x005ea004` reads `0x004a57e4` on both sides at tick 0.
   - Prediction holds: the image fixes the pad class only; the next first divergence at record 0 is the boot-write
     class (writers that run in the original's boot before tick 1 and not in the exe's ORIG_TOP boot). A fix belongs
     to the ORIG_TOP boot rows (named in the RESULT, nothing filed here).
   - Record 0 no longer diverges: report the new first divergent record, block, dwords and writers.
   - No `ticks.bin` / trace self-test FAIL / crash before 600 frames: report BLOCKED on the image-ON boot (IMG-4
     owns the teardown AV, which comes after the frame cap and does not count here when `ticks.bin` and `meta.json`
     are complete).
4. **0x00898c00 / 0x004325c0 in the exe (A4).** The ORIG_TOP log line `0x0043dfd0 stand-in calls N` gives the hand
   stand-in count (run-43 log: 1 per frame to frame 300, then 300). The watch on `0x007f1004` / `0x0067eca4`
   counts the `Anim_Tick` bridge (`Frontend/MenuNavSM.cpp`, which sets both words before `MenuAnimTickTwin` and
   restores them after: 2 hits per call per word at pcs in `Anim_Tick`).
   - Bridge pcs present: the exe runs 0x004325c0 during the menu. With `MASHED_PORTSTATE_FRONTEND` OFF the bridge
     restores `0x00898ac0..` after each call, so `0x00898c00` holds its saved value at every record. A5 then says
     whether the knob makes tick 18 match. A fix would be the knob path's frontend-state row (PORTSTATE-FRONTEND on
     ORIG_TOP), named not filed.
   - No bridge pc: the hand stand-in does not reach 0x004325c0 in the replay menu. The fix would be the 0x0043dfd0
     body (PROVISIONAL row, PORT-TOP-08 lane), named not filed. A5 is not run.

No G-CTRL, no B, no 12 captures (top-down diagnosis row). Raw arm data stays in `D:\MashedWT\RO-FIRST-DIVERGENCE_data`
with SHA-256s in the RESULT.
