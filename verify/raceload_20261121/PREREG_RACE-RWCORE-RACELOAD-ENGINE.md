# PREREG RACE-RWCORE-RACELOAD-ENGINE, step 1 (run 54, 2026-11-21)

Committed alone, before any build or arm. Branch `orch/RACE-RWCORE-RACELOAD-ENGINE` from `orch/integration` `77dedfece`.
Row kind: race path. Step 1 binds nothing, changes no source, flips no default.

## Question

The race load's clump read under 0x004b3bf0 (`FUN_004e7420` = `MashedRw_ClumpStreamRead_004e7420`) goes to the RW core
body `RwCore_004e7420` only under `RwCore_FrameRoute()` (`MASHED_RWC_CORE=1` + `MASHED_RWC_FRAME=1`,
`LibRw/RwApiBind.cpp:981`). RACE-RWLAYER-00543D40 ran the race-load arm without that route. Where does the race load stop
when the engine is the RW core?

## Arm (one)

`RO_ROLOADRWC-pr1`, stack `verify/savelink_20261023/stacks/ROLOADRWC.txt` = `@ROLOAD` + `MASHED_RWC_CORE=1` +
`MASHED_RWC_FRAME=1` + `MASHED_RWC_FRAME_N=100000`. `MASHED_RWC_FRAME_N` (default 1, `RwCore/RwCore_Knob.cpp:437`) ends the
boot after the Nth RwRasterShowRaster on the core. Default 1 would end the arm at its first present, before any race
load, so the arm carries the value the run-52 frameT arms used (`verify/rwcbombdestroy_20261116/run_arms.sh`). Object
build (`mashed_re_ddk.exe`, L-125), private root `D:/MashedWT/RACE-RWCORE-RACELOAD-ENGINE_root`, test-slot outside,
mashed_lock inside, `A4_RO_CKPT=0,1,17,18,299,301,600` (RACE-RWLAYER's set).

## Outcomes (exactly one is reported)

- A. The race is reached on the core: `meta.json` `race.reached` true and `race.n` > 3. Report `race.n` and the first
  divergence against ref1 (`detorig_firstdiv.py`, RACE-RWLAYER's command).
- B. A missing RW core body is called: `[RWC_CORE] STOP: rva 0x........` and exit `0x5257xxxx`. Report the rva.
- C. An access violation: the first-AV line of `RwCore_Knob.cpp`, or the exit `0xC0000005` with the faulting address.
  Report the address resolved against `mashed_re_ddk.map` and the RVA of the body it falls in.
- D. Anything else (another stop line, a hang to the 300 s limit, a non-race exit): report the last log lines.

For B, C and D the first stop is classed game / RW / library: RW = an RVA in the RW core body list
(`verify/rwccorerun_20261111/RwCore_Bodies.tsv`) or a RenderWare subsystem row in `hooks.csv`; library = CRT, Lua, zlib,
libpng or qhull; game = the rest.

## Expectation (not a pass criterion)

No prediction of the outcome class. The route has never run on the replay arm: on the menu path (knob_boot) it runs to
the title and past it with `MASHED_RWC_FRAME_N=100000` (RESULT_RWC-FRAME-BOMB-DESTROY.md section 2). Whether the replay
arm's race load reaches 0x004b3bf0 on the core at all is open.

## Checks

VS2022 build and object build (`set MASHED_DDKOBJ=1`), `ddkobj.py check` against `log/orch/run54_headcheck/` (no source
change, so the expected delta is none), the self-test lines the arm prints (`[portstate-image]`, `[OT_CODEPTR]`,
`[REPLAY]` init, `[RWC-DEVICE]`, the binding self-test).
