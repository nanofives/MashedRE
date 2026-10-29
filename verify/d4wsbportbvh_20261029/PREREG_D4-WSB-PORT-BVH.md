# PREREG D4-WSB-PORT-BVH

Committed alone, before any arm. Branch `orch/D4-WSB-PORT-BVH` off `orch/integration` @ `4ee1823e1`.
Input: the 25 `D4-WSB-PORT-BVH` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (all C2, census EXEC).

## Scope: 24 rows ported, 1 row not ported

| rows | what this step does |
|---|---|
| `0055f450 0055f480 0055f520 0055f670 0055f800 0055fe10 0055fe30 0055fe40 00562520 00562580 005625e0 00562600 00562fd0 00563810 00563940 00563de0 00563df0 00564100 00564130 005645b0 00565570 00565590 005655a0 00565c20` (24) | Lane-2 ports, one TU each `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`. Generator output commit `fe65b14b4`, hand fixes commit `59087341a` (list below). |
| `00563840` (1) | **not ported.** Exe-only `Collision/QhullBridge.cpp` already carries its body (`CollHull_Alloc`, malloc allocator by default). A Lane-2 copy is a new CROSS-TARGET dual copy and the rva-lint fails the build. `re/tools/dual_copy_allowlist.txt` says adding a line is a decision, so none was added. Stays C2. |

### How the TUs were made

1. `decomp_pc.py --file rvas.txt --callees --port --json` (pool0, run from the main checkout, 2026-10-29).
2. `verify/d4wsbportbvh_20261029/prep_decomp.py`: two exact-regex text rewrites before the generator.
   R1: 6 stores `*(code **)(E) = FUN_x;` in `0x0055f800` (5) and `0x00562600` (1) become `*(unsigned int *)(E) = 0x<x>u;`.
   They are stores of the original's fixed function address, not calls, and decomp2port's `INDIRECT` regex refuses
   the bare word `code`. R2: `CONCAT22(CONCAT11(p,p),CONCAT11(p,p))` in `0x00564100` becomes `p * 0x01010101u`.
3. `re/tools/decomp2port.py --emit-dir Lane2 --apply`: 25/25 emitted (8 `PORT:ret`, 17 `PORT:void(tracked)`).
4. Hand fixes from disassembly (`disasm_lin.py`, `scan_eax_use.py` on `MASHED.exe.unpatched`), each cited in the TU:
   - EAX returns. Ghidra types 5 rows void, but a caller reads EAX after the call (`scan_eax_use.txt`, READ32):
     `0x0055f450` (return of the call at `0x0055f475`), `0x00562520` (slot result, `0x00562547`), `0x00563840`
     (dropped), `0x00563df0` (`[param_1+8]` loaded at `0x00563dfa`), `0x00564130` (0 at ret `0x00564159`, pad + result
     at `0x00564179`). The ports return that value. The `0x00564130` thunks inside `0x0055f800` and `0x00562600` return it too.
   - `0x0055f450` arg 4: the generator wrote `(float)(param_3)`, a numeric int->float conversion. The original pushes the
     raw dword (`push ecx` at `0x0055f46d`), so the port passes `f32bits(param_3)`.
   - `0x00563940`: Ghidra pseudo-op `SQRT` -> `sqrtf` (`fsqrt` at `0x005639a0`). `0x0055f800`: pointer return of
     `0x00568d20` cast to the dword it is stored as.
   - Shadow counter phase: the 18 census `race_load` rows use `ShadowAB::kPhaseAny`, the 6 `race_run` rows keep
     `kPhaseRace` (D4-WSB-PORT-SOLV amendment 1: `kPhaseRace` skips phase 2, `Core/ShadowAB.h:159`).
   - Not changed: `0x00564130` stores its pad byte into the arg-3 stack slot (`mov byte ptr [esp+0x14], cl` at
     `0x00564164`). The port assigns its own parameter copy. Recorded here, judged by the shadow read-out.
   - `scan_eax_use.txt` has one site the walk did not finish: `0x005655a0` from `0x005744e9` ("walk limit", 40
     x87 instructions with no EAX read). Recorded, not acted on.

## Default-OFF knob

- All 24 register as `L2_FUN_<rva>`. `HookSystem::InstallAll` skips `L2_` names unless `MASHED_HOOK_ONLY` lists them
  or `MASHED_HOOK_LANE2=1` (`Core/HookSystem.cpp:244-253`). They are in `asi_sources.rsp` only. A default `.asi` run
  and `mashed_re.exe` do not carry them. No exe source changes. Nothing flips a default.
- Float codegen: SSE2 (default). None of the 24 is added to `mashedmod/x87_tus.txt` (that list is measured, not reasoned).

Build `MASHED_NO_DEPLOY=1` at `59087341a`: `mashed_re.exe` `13d3a74b0f84b594...9e56`, `mashed_re_dev.asi`
`427c85453556ef03...99c5e`. 0 compiler warnings in the 24 TUs. The `.asi` is copied to the private root
`D:/MashedWT/D4-WSB-PORT-BVH_root/original/` only, never to `original/`.

## Boot check (done before this PREREG)

2026-10-29 ~16:15 local: endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before and after,
`scenario_launch.py --track 0 --mode 10 --cars 4 --hold 15` on the private root, no hooks, under `mashed_lock.py`:
race running (phase 3), game PID 6664, 0 MASHED Event-1000 events. Transcript `D:/MashedWT/D4-WSB-PORT-BVH_data/bootcheck.txt`.

## Arm S (shadow A/B, one hook installed alone per boot)

`verify/d4wsbportbvh_20261029/drive_pb.py shadow <rva...>` (copy of `verify/d4wsbportsolv_20261029/drive_ps.py`,
runner name, data path and label changed) around `run_shadow7.py` (copy of `run_shadow6.py`, data path and label
changed), wrapped in the test slot, each row in its own solo window under `mashed_lock.py`. Order: the 24 rows in
address order.

Env on every boot: `MASHED_ROOT=D:/MashedWT/D4-WSB-PORT-BVH_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
MASHED_SHADOW_STW=1`, `TRACK=0`, mode 10, 4 cars, `HOLD=60`, `SAMPLES_DONE=24`. This is the census scenario (the one
that marked these rows EXEC). `scenario_launch.py --hooks 0x<rva>` sets `MASHED_HOOK_ONLY`, which installs the `L2_` hook.

Audio rule: before each launch `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list an endpoint. If not, or if
an Event-1000 fault at `0x005baf48` appears: stop, BLOCKED "audio endpoint", no retry. A U-9677 crash
(`MASHED.exe+0x1509b4` / `+0xe66db`) is re-run (max 2), not a finding.

## Read-out per row (fixed now, from `re/tools/shadow_ab_report.py`)

- **C2 -> C3**: verdict `CLEAN` (n > 0 samples, ndiff 0 on every sample, `PATCHBYTE ... A/B-IS-REAL`), plus the
  re-classify C3 gate (callers / callees at C2+). Promoted through re-classify with the log path, sample count and proof
  line. Heap pages compared are recorded for a later C4 leg but do not gate C3.
- **Stays C2, dated note**: `DIVERGENT` (offsets reported), `DIVERGENT_FLOAT10`, `UNPROVEN`, `SKIPPED`, `NO_SAMPLES`,
  `NO_LOG`, or a non-U-9677 crash with the hook armed (crash reported with its offset). A DIVERGENT float row is not
  retried under x87 in this step without an amendment committed first.
- **No C4 in this step.** C4 needs the race diff and the write-set leg (`re/CONFIDENCE.md`, L-38/L-40/L-54).
- `0x00563840`: no arm, stays C2.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
machine lock, must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`. A mismatch is a RED for the step.

## Amendment 1 (2026-10-29, after arm S rows 1-5, before any re-run)

Arm S row 5 on build `.asi 427c8545`: `0x0055f800` DIVERGENT n=1, one dword at `param_1+0xc` (page `0x0f3eb000+0xa9c`),
original `3dcccccd`, port `4e773333`. Cause, read from source: decomp2port types the `.rdata` constant `_DAT_005cc56c`
as `undefined4` -> `(*(const unsigned int*)0x005cc56cu)`, so `param_4 * _DAT_005cc56c` multiplied by 1036831949 (the
bits of 0.1f) instead of 0.1f. `4e773333` is 1.0f times that integer. Scan of all `Lane2/L2_*.cpp` for an `unsigned int`
`.rdata` define in float arithmetic: 4 TUs, all in this step: `0x0055f800` (`0x005cc56c` = 0.1f), `0x00563940`
(`0x005cc320` = 1.0f, `0x005cc32c` = 0.5f), `0x005655a0` (`0x005cc320`), `0x00565c20` (`0x005cc32c`). Values read from
`MASHED.exe.unpatched`.

Change (commit `1f09a4b77`): those 5 defines read `const float`. Nothing else. New build `MASHED_NO_DEPLOY=1`:
`mashed_re.exe` `558ca866...6f6d`, `mashed_re_dev.asi` `9ae3c140...0de8`.

Arm S continues to its end on `.asi 427c8545` (rows 6-24). Then the new `.asi` goes into the private root, and arm S is
re-run as written above for `0055f800 00563940 005655a0 00565c20`. Their first-pass verdicts are kept in the RESULT as
superseded and are not used. The other 20 rows keep their first-pass result (their TUs are unchanged by `1f09a4b77`).
Read-out unchanged. Default gates run on the final build.

## Amendment 2 (2026-10-29, after the amendment-1 re-run, before any further run)

Amendment-1 re-run on `.asi 9ae3c140`: `0x0055f800` CLEAN n=1, `0x005655a0` CLEAN n=24, `0x00563940` DIVERGENT 4/4
(1-ULP values, e.g. `438505a4` vs `438505a5`), `0x00565c20` DIVERGENT 24/24 with large values (sample 0, the only one
with an untouched pre-state: radius original `3f0069c0`, port `58d07606`).

Cause of `0x00565c20`, read from disassembly and source: the original builds the displacement as one 3-float array at
`[esp+4..0xc]` (fstp at `0x00565c30`, `0x00565c3a`, `0x00565c49`) and passes its address (`lea eax,[esp+4]` at
`0x00565c44`) to `0x004c3ac0`. Ghidra shows it as three scalars `local_c/local_8/local_4`, and the port passed
`&local_c`, so `Vec3Magnitude` read two locals MSVC had not placed after it. A scan of every `Lane2/L2_*.cpp` for `(&local_`
finds no other site.

Change (commit `4c0f9cec1`): `0x00565c20` keeps the displacement as `float lv[3]`. Nothing else. New build
`MASHED_NO_DEPLOY=1`: `mashed_re.exe` `f026b40b...bbb4`, `mashed_re_dev.asi` `18f76050...42d3`.

Re-run: arm S as written above for `00565c20` only, on the new `.asi`. `0x00563940` is not re-run: its divergence is the
float-rounding class, which this PREREG leaves at C2 without an x87 amendment. No x87 trial in this step. The other 23
rows keep their result (their TUs are unchanged by `4c0f9cec1`). Read-out unchanged. Default gates run on this build.
