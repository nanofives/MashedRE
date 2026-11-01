# PREREG D4-WSB-PORT-BROAD-2

Committed alone, before any arm. Branch `orch/D4-WSB-PORT-BROAD-2` off `orch/integration` @ `0e3d71a75`.
Input: the 18 `D4-WSB-PORT-BROAD-2` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (all C2, census EXEC).
Method: the D4-WSB-PORT-BROAD-1 recipe (`verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md`), unchanged.

## Scope: 18 rows ported, 0 not ported

| rows | what this step does |
|---|---|
| `0055b060 0055b080 0055b4a0 0055b650 0055b860 0055bab0 0055bac0 0055c380 0055c490 0055c4a0 0055c540 0055cd40 0055dc70 0055dca0 0055ddd0 0055ded0` (16) | Lane-2 ports from `decomp2port.py`, plus the hand fixes below. |
| `0055b940 0055bbf0` (2) | Hand-written Lane-2 ports (generator refused: `UNKNOWN_TYPE:ret float10`, `INDIRECT_CALL`). |

One TU each, `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`, commit `e498a8356`. None of the 18 had a body in
`mashedmod/src/mashed_re/` before this step (the mentions in `Collision/RwpBuildExterns.cpp`, `Collision/RwpVtableKV2.cpp`,
`Physics/PromoLoop_standins_r7.cpp`, `Lane2/L2_00562600.cpp`, `Lane2/L2_00562fd0.cpp` and `Lane2/L2_0055b030.cpp` are
raw-RVA thunks to the original), so no TU is a dual copy. rva-lint: 0 NEW.

### How the TUs were made

1. `decomp_pc.py --file rvas.txt --callees --callers --xrefs --port --json` (pool0, 2026-11-01). Dry run of
   `decomp2port.py`: 16 accepted (8 `PORT:ret`, 8 `PORT:void(tracked)`), 2 refused (above).
2. `decomp2port.py --emit-dir Lane2 --apply`: 16/16 emitted, all wrappers `ShadowAB::RunTracked`.
3. Hand fixes from the disassembly (`disasm_lin.py`, `scan_eax_use.py` on `MASHED.exe.unpatched`, copies from
   `verify/d4wsbportbroad1_20261029`), each cited in its TU:
   - `.rdata` float constants read as float, not `const unsigned int`: `0x005cc320` (1.0f) in `0x0055b080`, `0x0055b4a0`,
     `0x0055cd40`; `0x005cc558` (0.001f) in `0x0055b080`; `0x005cc33c` (-1.0f) in `0x0055cd40`. `0x005cd0c8` is read as a
     **double** (`fld qword ptr [0x5cd0c8]` at `0x0055b19e`, value 1.0).
   - `0x0055b080`: `param_1[7]` stores the float bits (`fstp dword ptr [ebx+0x1c]` at `0x0055b42e`); the generator wrote a
     numeric float->int conversion. `SQRT` -> `sqrtf` (x3).
   - `0x0055b4a0`: `SQRT` -> `sqrtf` (x3). The locals are three stack blocks passed by address: the 7-float source of the
     second `0x005655a0` call and its two 9-dword outputs (`0x005655a0` writes `[ecx]..[ecx+0x20]`,
     `0x005656ab..0x0056576d`). Ghidra showed them as 25 scalars; now three arrays.
   - `0x0055cd40`: the third sign test is `fcomp / test ah,5 / jp` (`0x0055cd96..0x0055cda2`), which sends NaN to the 1.0f
     side. Ghidra's `0 <= x` would not; rewritten `!(x < 0)`.
   - EAX returns (`scan_eax_use.txt`): `0x0055dc70` is typed void but its only caller stores EAX (`mov esi,eax` at
     `0x0047f544`, READ32); EAX at its `ret` is the tail call's result, so the port returns it. `0x0055b650` is typed void;
     EAX at `ret` is `param_1` (`0x0055b653`, never rewritten) and 11 of its 15 call sites are in tail position (RET), so
     the port returns `param_1`. `0x0055dca0` stores the EAX of `0x00559b50` and `0x0055f450` (both typed void): their
     thunks return it (`0x00559b50` per D4-WSB-PORT-BROAD-1; `0x0055f450` returns its tail call to `0x0055f800`).
   - The other 6 void rows (`0x0055b860`, `0x0055bab0`, `0x0055bac0`, `0x0055c380`, `0x0055c490`, `0x0055c4a0`) have every
     caller KILL EAX before a read (the two `read16` hits, `0x00480cf5` and `0x00471a88`, follow `mov al,..` and
     `fnstsw ax`). They stay void.
   - Shadow counter phase: the 16 census `race_load` / `race_load,race_run` rows use `ShadowAB::kPhaseAny`;
     `0x0055b4a0` and `0x0055b650` (census `race_run` only) keep `kPhaseRace`.
4. Hand-written:
   - `0x0055bbf0`: the slot call `call dword ptr [eax+0xc]` is cdecl (3 pushes, `add esp,0xc` at `0x0055bc0d`) and returns
     ST0, stored as float (`fstp dword ptr [esp+0x38]` at `0x0055bc10`). `[esi+0x5c]` is read as a pointer dword (Ghidra's
     `(int)param_1[0x17]` would convert a float). The two local blocks are one 7-float array. Every float sum follows the x87
     order (`0x0055bc29..0x0055bd5e`), not Ghidra's text order (e.g. Ghidra's `p6*q0 + p3*q3 + x1` is
     `(p3*q3 + x1) + p6*q0` in the code). `0x00546b10` is bound to its port, as the generator binds it.
   - `0x0055b940`: returns a float in ST0. The 10 locals are one array. Three compares are rewritten to the NaN side the
     `fnstsw / test / jp` code takes: the `param_3 == 0` exit (`0x0055ba14..0x0055ba19`), the max-of-three selection
     (`0x0055ba1b..0x0055ba54`) and the final `r < FLT_MIN` test (`0x0055ba67..0x0055ba73`).
   - Not reproduced, recorded: both functions write a value into one of their own arg slots (`0x0055bbf0` `fstp [esp+0x38]`
     = its `param_1` slot; `0x0055b940` `fstp [esp+0x34]` = its `param_2` slot). No caller reads that slot after the call
     (the 7 `0x0055b940` sites pop ST0 and read `[esp+0x30]` or higher; the `0x0055bbf0` sites read `[esp+0x40]` and
     `[esp+0x28]`). The ports keep the value in a local.
   - Not changed, recorded: `0x0055b080` copies four floats through `fld / fstp` (`0x0055b147..0x0055b155`); the port copies
     the dwords. The two differ only on a signalling NaN.

## Default-OFF knob

- All 18 register as `L2_FUN_<rva>`. `HookSystem::InstallAll` skips `L2_` names unless `MASHED_HOOK_ONLY` lists them
  or `MASHED_HOOK_LANE2=1`. They are in `asi_sources.rsp` only, not in `exe_sources.rsp`. A default `.asi` run and
  `mashed_re.exe` do not carry them. No exe source changes. Nothing flips a default.
- Float codegen: SSE2 (default). None of the 18 is added to `mashedmod/x87_tus.txt`.

Build `MASHED_NO_DEPLOY=1` at `e498a8356`: `mashed_re.exe` `b53c0ed2fb51f216...ba13`, `mashed_re_dev.asi`
`23d17c78db54b165...fcdb` (0 compile errors, no warning in the 18 TUs). The `.asi` is copied to the private root
`D:/MashedWT/D4-WSB-PORT-BROAD-2_root/original/` only, never to `original/`.

## Arm S (shadow A/B, one hook installed alone per boot)

`verify/d4wsb_portbroad2_20261101/drive_pb2.py shadow <rva...>` (copy of `verify/d4wsbportbroad1_20261029/drive_pb1.py`,
runner name, data path and label changed) around `run_shadow9.py` (copy of `run_shadow8.py`, data path and label
changed), wrapped in the test slot, each row in its own solo window under `mashed_lock.py`. Order: the 18 rows in
address order.

Env on every boot: `MASHED_ROOT=D:/MashedWT/D4-WSB-PORT-BROAD-2_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
MASHED_SHADOW_STW=1`, `TRACK=0`, mode 10, 4 cars, `HOLD=60`, `SAMPLES_DONE=24`. This is the census scenario. 
`scenario_launch.py --hooks 0x<rva>` sets `MASHED_HOOK_ONLY`, which installs the `L2_` hook.

Audio rule: before each launch `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list a VB-Audio endpoint. If not,
or if an Event-1000 fault at `0x005baf48` appears: stop, BLOCKED "audio endpoint", no retry. A U-9677 crash
(`MASHED.exe+0x1509b4` / `+0xe66db`) is re-run (max 2), not a finding.

## Read-out per row (fixed now, from `re/tools/shadow_ab_report.py`)

- **C2 -> C3**: verdict `CLEAN` (n > 0 samples, ndiff 0 on every sample, `PATCHBYTE ... A/B-IS-REAL`), plus the
  re-classify C3 gate (callers / callees at C2+). Promoted through re-classify with the log path, sample count and proof
  line. Heap pages compared are recorded for a later C4 leg but do not gate C3.
- **Stays C2, dated note**: `DIVERGENT` (offsets reported), `DIVERGENT_FLOAT10`, `UNPROVEN`, `SKIPPED`, `NO_SAMPLES`,
  `NO_LOG`, or a non-U-9677 crash with the hook armed (crash reported with its offset). A DIVERGENT row is not
  re-fixed and re-run in this step without an amendment committed first.
- **No C4 in this step.** C4 needs the race diff and the write-set leg (`re/CONFIDENCE.md`, L-38/L-40/L-54).

### C3 gate pre-check (hooks.csv at `0e3d71a75`, `decomp_pc.py --callers --xrefs`, pool0)

- Caller half met by a C2+ caller: 17 rows. Callee half: C2+ callee, or none (leaf), or the slot call of `0x0055bbf0`
  (runtime-data dispatch, CONFIDENCE.md indirect-dispatch clause; its other callee `0x00546b10` is C3).
- Caller half **not met** (shadow arm still runs, row stays C2 with a dated note even if CLEAN): `0x0055cd40`. Its only
  reference is `DATA 0x005e4ff4` (a table slot), and `re/analysis/pointer_callers_2026-10-11.tsv` lists it as
  `data_orphan` (no instruction references any slot at or before it), which the installer-caller clause excludes.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
machine lock, must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (G-CTRL `8284be46/ba407a16/31fe043e`,
B `1d5bf4f9/1d5bf4f9/1d5bf4f9` at `0e3d71a75`). A mismatch is a RED for the step.

## Amendment 1 (2026-11-01, during arm S row 13, before any re-run)

Arm S pass 1 on `.asi 23d17c78`:

- `0x0055b060`: NO_LOG. The transcript shows `scenario_launch pid=24152` then `error: could not attach` (the launcher
  never attached, so no shadow sample was taken). This is a launch failure, not a hook outcome. The row is re-run once,
  as written above. A second launch failure leaves it at C2 (NO_LOG).
- `0x0055b080`: DIVERGENT 4/24 (samples 16-19), return equal, 9/9 heap pages, 1 page differs: 1-2 dwords at
  `+0x770/+0x774` and `+0x780/+0x784` of one heap page, each a 1-ulp difference (e.g. `bbfc4cb0` vs `bbfc4caf`). The
  dwords are adjacent pairs at a 16-byte stride, the layout of the quaternion table that `0x00546b10` writes from this
  body (`param_4 != 0` branch, `FUN_00546b10(quat[idx], matrix[idx])`). The generator bound that call to the port in
  `Collision/RwpSolverMath2.cpp`, whose X / Z branch callees build SSE2; U-9220 already measured the Y branch RED in SSE2.
  Every other float expression of the TU was checked against the listing before pass 1.

Change (commit below, `L2_0055b080.cpp` only): the `0x00546b10` call goes through a raw-RVA thunk to the original
instead of the port. Nothing else. New build `MASHED_NO_DEPLOY=1`: `mashed_re.exe` `285749aa80bbc25f...4d61` (exe sources
unchanged, the link is not bit-reproducible), `mashed_re_dev.asi` `a7275505f8bb01de...76d5` (0 errors, rva-lint 0 NEW).

Arm S continues to its end on `.asi 23d17c78` (rows 13-18). Then the new `.asi` goes into the private root and arm S is
re-run as written above for `0055b080` and `0055b060` only. Their pass-1 verdicts stay in the RESULT as superseded
(`shadow_summary_pass1.tsv`). The other 16 rows keep their pass-1 result (their TUs are unchanged by this commit).
Read-out unchanged: CLEAN promotes, anything else leaves the row at C2 with no further fix in this step. If
`0055b080` is CLEAN on the re-run, the 4 pass-1 samples are reported as a `0x00546b10` port finding, not fixed here.
Default gates are re-run on the final build.

## Amendment 2 (2026-11-01, recovery after the 14:23 local reboot, before any further arm)

State at the reboot (`D:/MashedWT/D4-WSB-PORT-BROAD-2_data/drive_pb2.log`, `pass2.log`):

- Pass 1 on `.asi 23d17c78` finished rows 1-16 (`shadow_summary_pass1.tsv`, identical to `shadow_summary.tsv` at that
  point). Row 17 `0x0055ddd0` never launched: the audio endpoints dropped while it queued (`post endpoints: <none>` at
  17:14:42Z, BLOCKED audio endpoint, no shadow log, no data folder). Row 18 `0x0055ded0` never ran.
- The pass-2 script (`run_pass2.sh`) had already put `.asi a7275505` (Amendment 1 build) into the private root and queued
  `0055ddd0 0055ded0 0055b080 0055b060`. Its first launch was BLOCKED audio endpoint (none present, 17:23:20Z), then the
  machine rebooted. No pass-2 arm produced a shadow log. The final gate run (`gates_final`) never started.

Change (harness order only): rows 17-18 run on `.asi a7275505` instead of `23d17c78`, in the same invocation as the
`0055b080` / `0055b060` re-runs. The two builds differ only in `L2_0055b080.cpp` (`git diff e498a8356 4e1812a6e --
mashedmod/`), and arm S installs one hook alone per boot (`MASHED_HOOK_ONLY`), so the code installed for `0055ddd0` and
`0055ded0` is the same source as in pass 1. Every other rule (audio check per launch, U-9677 re-run, read-out, gates on
the final `mashed_re.exe` `285749aa`) is unchanged. VB-CABLE endpoints present at 17:52Z after the reboot.
