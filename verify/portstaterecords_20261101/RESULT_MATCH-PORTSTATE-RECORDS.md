# RESULT MATCH-PORTSTATE-RECORDS (2026-11-01)

Verdict: **GREEN.** With `MASHED_PORTSTATE_RECORDS` (default OFF, exe only) the exe's vehicle records live at the
original's 0x008815a0 (16 x 0xd04, so 0x00881e80 and 0x008815a8 are live too). The car visual records live at
0x0063d9e0 (4 x 0x2ac). Neither is a copy: the port state now sits at the original addresses.

The first run (g1) moved every knob-ON gate hash. The cause was one more piece of original state the knob did not yet
carry: the handling global 0x0061313c. Addendum A1 (committed alone) adds it. On run g2 every prediction holds, and
no gate hash moves with the knob OFF or ON, so no decided-flip decision is filed.

The T13 re-check makes one row's matched read live with no residue: 0x0046dbe0, 0 mismatches in 52,800 car-frames.
Its swap still needs the knob to be ON by default. The other record-block rows keep a second blocker, listed below.
0x008995bc is not migrated: the exe has no state for it.

PREREG: `verify/portstaterecords_20261101/PREREG_MATCH-PORTSTATE-RECORDS.md`, committed alone at `3fab09ccc`.
Addendum A1 committed alone at `a83849ab9`, after g1 and before g2. Basis: L-78 (c) (Mariano 2026-11-01), port-state
migration 2/3. Base branch: `orch/MATCH-PORTSTATE-COURSE` @ 622335487 (1/3, not yet on `orch/integration`).

## Change

- `282046ead`
  - `Vehicle/RecordBlockState.{h,cpp}` (new, exe only) holds the knob (`PortStateRecords_On`, read once) and the
    `[portstate-records]` witness. The witness runs with the knob ON only, writes nothing, and logs one line per 600
    race frames.
  - `Vehicle/VehiclePhysicsRun.cpp`: `g_records` becomes `unsigned char* const`. With the knob ON it is 0x008815a0;
    with it OFF it is the exe's own store `g_recordsStore[16*0xd04]`. The init memset now uses `16 * kRec`, since
    `sizeof` of a pointer would be wrong.
  - `Race/CarRegistrant.{h,cpp}`: `s_rec` changes the same way (knob ON = 0x0063d9e0 + v*0x2ac). It adds
    `CarsReg_RecordBase()` for the witness.
  - `D3d9Render/TrackRenderer.cpp`: the witness is called right after `VehiclePhysics_RecDumpFrame`.
  - `exe_sources.rsp`: the new TU. `run_arms.py`: `STACK['PSR'] = ['MASHED_PORTSTATE_RECORDS']`.
- `44b55ed24` (A1): `Vehicle/VehicleInit.cpp`. The standalone `A3PublishHandling` also stores param, spring, k1 and
  k2 at 0x00613108 / 0x00613114 / 0x00613130 / 0x0061313c when the knob is ON. These are the original A3's stores,
  the same as the asi branch of the same function.
- PREREG tooling:
  - `absref_scan.py`: a capstone scan of every displacement or immediate that falls in the three ranges.
  - `gates_mask.py`: the P3 / P4 witness on gates.csv.

Both builds: `MASHED_NO_DEPLOY=1`, `=== Build OK ===` for both targets, `[rva-lint] 3 finding(s) ... allowlisted=3
NEW=0`, `[asm-lint] OK`.

## Predictions

Gate runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSR B_PSR_K0 B_PSR_K1
B_PSR_K26`. Settings:

- `A4_ROOT=D:/MashedWT/MATCH-PORTSTATE-RECORDS_root` (a robocopy of `original/`).
- The exe is copied out before the run (`gates.sh` / `gates2.sh`).
- The run is wrapped in `test-slot` and `mashed_lock.py run --label MATCH-PORTSTATE-RECORDS`.

All 16 arm runs exited 0. H arms ran 123 to 139 s, B arms 12 to 17 s.

| id | prediction | g1 (`282046ead`) | g2 (`44b55ed24`) | holds |
|---|---|---|---|---|
| P1 | knob OFF: G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 | H_DEF 8284be46 / ba407a16 / 31fe043e, B_DEF 1d5bf4f9 x3 | the same | yes (both) |
| P2 | knob ON: record base 0x008815a0, visual base 0x0063d9e0 | `base=008815a0 vis0=0063d9e0` | the same | yes (both) |
| P3 | knob ON: gates.csv absolute columns = port record columns on every row; knob OFF they read 0 | OFF 0 nonzero rows of 54,084; ON abs != rec on 0 of 54,084 | the same (`gates_mask_g2.txt`) | yes |
| P3 | witness: 0x0046dbe0 read = `VehRecord(v)+8` | `dbe0_bad=0` | `dbe0_bad=0` of 52,800 | yes |
| P3 | witness: 0x0041f030 read = `VehRecord(v)+0x958` | 39,699 / 52,800 differ | 26,421 / 52,800 differ | **no as worded**: the witness was mis-timed, see below |
| P4 | knob ON: step 8284be46, ptrace 31fe043e, B 1d5bf4f9 x3, masked gates equal | step cd061e1a, ptrace 118260d1, B df54a9a9 x3 | (P4', after A1) step 8284be46, ptrace 31fe043e, B 1d5bf4f9 x3, masked gates 0 differing cells | g1 **no**, g2 yes |

H_PSR.gates.csv on g2 is `67931834` instead of ba407a16. That is the by-construction change P4 names: the five
absolute columns (`veh_type_abs`, `state0_abs`, `rec_t_abs`, `rec_x_abs`, `rec_z_abs`) now read the live record
instead of the pad's 0. With those columns replaced by the port columns, H_PSR equals H_DEF in all 54,084 rows.

**g1 failure and A1.** The first divergent row in both H_PSR and B_PSR_K0 is frame 2, car 1: `rec_9e4` goes from
370.513153 to 370.685852, and position follows.

- `AiPreTickRubberBand` (`Ai/AiStandalone.cpp:1688-1689`, FUN_0046dd80 / FUN_0046dd90) writes `F32(0x0061313c)` into
  record `+0x154` of cars 0..3 on every frame.
- In the original, A3 0x0046b540 writes that global (0x0046b562 `mov [0x61313c],0x3fc00000`, 0x0046b5cd
  `fstp [0x61313c]`) and copies it to `+0x154` (0x0046b5ee).
- The exe's standalone A3 did not publish it. The pad's 0.0 therefore overwrote k2 = 1.5 once the record was live.

The PREREG claimed that +0x154 has no port reader. That was wrong: the record field is read on the physics path. The
read site was not located by name [UNCERTAIN: which port function reads +0x154; the evidence is the move on the first
frame after the write]. A1 publishes the original's four handling globals with the knob ON. The exe's
`HandlingTypeIndex()` gives one value for all cars, so the round trip writes back the same k2 and g2 reproduces the
refs. The mode-9 / mode-4 scaling arms (`AiStandalone.cpp:1716-1747`) did not change the gate scenario's hashes.

**0x0041f030 witness.** This is a measurement-timing defect, not a storage defect.

- `CarsReg_FrameTick` (`Race/CarRegistrant.cpp`) is the only writer of the visual record's `+0x258..+0x263`, with
  either knob value. It copies `VehRecord(v)+0x958` there before the frame's physics.
- The witness compares after physics. The differing samples are therefore cars that moved between the copy and the end
  of the frame.
- An equality check taken at the copy was not run [UNCERTAIN until a witness samples at `CarsReg_FrameTick`].

## Re-check of the T13 record-block rows (knob ON, g2)

Witness, last line of H_PSR (`witness_H_PSR_g2.txt`):
`frames=13200 samples=52800 base=008815a0 vis0=0063d9e0 dbe0_bad=0 f030_live=52800 f030_bad=26421 d510_bad=29813
d510_bad0=29813 score=0,0,0,0 held=00000000,...`

| row | rva | record read, knob ON | remaining blocker | outcome |
|---|---|---|---|---|
| T13 | 0x0046dbe0 | `DAT_008815a8[v*0x341]` = the port's `VehRecord(v)+8` on every sample (0 / 52,800) | none for the read. Exe caller: the port body `VehicleRacePositionGet` (`Vehicle/VehicleState.cpp`) is called by `RubberBandGrip` (`Vehicle/ForceIntegratorLeaves.cpp:53-54`). With the knob OFF the matched body would read the pad's 0, so the swap is sound only once the knob is ON by default | left; **swap-ready behind a flip** |
| T13 | 0x0046d510 | `+0x928` matrix and `+0x9d4` are live | (1) the port's `+0x928` `at` row differs from its `+0x9d4` in 29,813 / 52,800 samples: the stated deviation in `VehiclePhysicsRun.cpp:135-139` (the port keeps the body basis in `g_bodyBasis`, not at +0x928). The matched body would overwrite the port's `+0x9d4` with a different vector. (2) `DAT_00614708` (0,0,1) is file-backed `.data` that the exe does not seed. (3) callee 0x004c3df0 has no exe binding. No exe caller by symbol | left; next blocker = the body-basis layout at +0x928 |
| T13 | 0x00414490 | through 0x0046d510 (above) | the held word 0x0088fc88 reads 0 on all four cars (`held=00000000` x4). It is not part of this migration: the exe holds it as a type code in `PowerupSystem pw_`, while the original holds an entry pointer at +0xa8 | left |
| T13 | 0x0041f030 | `+0x258..+0x267` of the visual record is at 0x0063d9e0 (centre + radius) | no exe call site: the exe calls the stub `TG_Stub41f030`, its consumer 0x0048a630 is a stub, and the matched TU is asi-only | left |
| T13 | 0x0041f1e0 | reads `objs[p2]` (RpAtomic*, +0..+0x1ff), which the exe never builds (0) | no exe call site (U-9738, the RWP-body branch); callee 0x004c0ed0 has no exe body | left |
| T13 | 0x00422b50 | 0x008995bc + v*0x138 reads 0 (`score=0,0,0,0`) | no exe producer or caller: both calls in 0x0046d780 are `#ifndef MASHED_STANDALONE`, and the exe body calls the unbound RVAs 0x0040e470 / 0x00472650 / 0x004a2c48. Porting the producer is a behaviour change (an RNG draw on the non-player arm), so it needs its own PREREG | left; not in this step |

0x00881e80 needs no separate step: it is +0x8e0 of record 0, inside the relocated block.

Also measured (not a T13 row):

- 50 exe sites address the 0x008815a0 range absolutely (`absref_course_exe.txt`).
- With the knob ON, the ones on the gate path either change no hash (the `AL_46d570` rebase through `g_al_recdelta`
  becomes delta 0, and `ElimTraceFrame` is not armed) or change only diagnostic columns (`U9186GateDump`).
- The rubber band's `+0x154` round trip is now the original's (A1).
- The original addresses the same range from 229 sites (`absref_original.txt`).

## Scope limits

- Measured on Training only (the gate scenario).
- The default build's code layout changed: `rec()` loads a pointer, and there is a new TU. The layout-sensitive tracks
  in memory are Rouabout / Storm / Highway, not Training, and the train's `-Full` pass covers them.
- This branch touches `Vehicle/` (in `full_set_paths`), `Race/` and `D3d9Render/`.
- It sits on top of the unmerged COURSE branch, so merging it carries COURSE too.

## Re-classify

None. No row changes level, and no new exe caller runs in the default build.

## Decision

None filed. No gate hash moved with the knob ON (g2), so there is no decided-flip decision. For the lead: flipping
`MASHED_PORTSTATE_RECORDS` to default ON would make 0x0046dbe0 a sound swap (the read is live, and an exe caller
exists). Its value today is that single row, with gates unchanged on Training.

## Raw data (D:/MashedWT/MATCH-PORTSTATE-RECORDS_data, not committed)

```
99df07e03c31aa97f95300107fc791b0c6e43f4a14f35081f23535c7d879c0e7  exe_g1/mashed_re.exe (282046ead)
dd9892876baea65b930369d3e9982547dfc8ba9560b19e687a4caa26c318a272  exe_g2/mashed_re.exe (44b55ed24)
7fd281f8721f9f16040a37c1f3d27e747e14dc4993e5797d39fb75fd7a433fa4  build1.log
7ae110f9fd5a3180cdf42c3c96388dacebb300a716e997c66a1663dc102590c2  build2.log
d9dcf21eb98550c5578c578356a6c2ec4808630bb876794246db965c11b3f8f5  gates_g1.log
5070f3bd4e56c7ee292d215e45df3dc783767dab0c4ae97ccd7586e646d7e4d3  gates_g2.log
cd061e1a2822f47880b5e7aec830b87c650ed4ae031043f71fa300ae1deaff67  arms_g1/H_PSR.step.csv
df54a9a94a0b316431cc2886419a8c8c1f9d1223e5d2beff35dc655b3b88c252  arms_g1/B_PSR_K{0,1,26}.step.csv
d4d8f8697caca9d309e7de878e7b00765d398ae42da027891fd17bf3684e04d4  arms_g1/H_PSR.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g2/H_DEF.step.csv (= H_PSR.step.csv)
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g2/H_DEF.gates.csv
6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c  arms_g2/H_PSR.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g2/H_DEF.ptrace.log (= H_PSR.ptrace.log)
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g2/B_{DEF,PSR}_K{0,1,26}.step.csv (all six)
4828e77276b7a857f221e4085ecb5051a4e2353eb3bb65c51feedf02288a8409  arms_g2/H_PSR.log
```

Committed extracts:

- `gates_mask_g2.txt`
- `witness_H_PSR_g1.txt`, `witness_H_PSR_g2.txt`
- `absref_course_exe.txt` (scan of the COURSE exe `a7f4913f`, the base of this branch)
- `absref_original.txt` (scan of `MASHED.exe.unpatched`)
- `raw_sha256.txt`
