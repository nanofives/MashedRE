# PREREG D4-WSB-PORT-BROAD-1

Committed alone, before any arm. Branch `orch/D4-WSB-PORT-BROAD-1` off `orch/integration` @ `f6033d936`.
Input: the 17 `D4-WSB-PORT-BROAD-1` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (all C2, census EXEC).

## Scope: 17 rows ported, 0 not ported

| rows | what this step does |
|---|---|
| `00558c50 00558ca0 00558cb0 00559040 00559560 005595d0 00559b50 00559bc0 00559be0 00559c40 00559ee0 0055ad20 0055ad30 0055ade0 0055ae70 0055af40 0055b030` (17) | Lane-2 ports, one TU each `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`. Generator output commit `38a4c4d09`, hand fixes commit `5b9eb00b2` (list below). |

None of the 17 had a body anywhere in `mashedmod/src/mashed_re/` before this step (`FUN_00559c40` / `FUN_0055ae70` in
`Collision/RwpBuildExterns.cpp` are raw-RVA thunks), so no TU is a dual copy. rva-lint: 0 NEW.

### How the TUs were made

1. `decomp_pc.py --file rvas.txt --callees --callers --port --json` (pool0, 2026-10-29). Dry run: 11 accepted,
   6 refused (5 `INDIRECT_CALL`, 1 `PSEUDO_OP:CONCAT22`).
2. `verify/d4wsbportbroad1_20261029/prep_broad1.py`: exact-regex text rewrites before the generator, counts checked per row.
   - V1: 6 object vtable calls `(**(code **)(*P + OFF))(args)` in `0x00559c40` (3), `0x00559ee0`, `0x0055ad30`,
     `0x0055ade0` become `L2_vcall(*(unsigned int *)(*P + OFF), args)`. Every site is cdecl in the disassembly
     (`call [reg+off]` then `add esp, 4*n`: `0x00559d0c`/`0x00559d0f`, `0x00559dd4`/`0x00559de0`, `0x00559e73`/`0x00559e76`,
     `0x00559f3c`/`0x00559f45`, `0x0055ad74`/`0x0055ad77`, `0x0055ae0d`/`0x0055ae1a`).
   - V2: `0x00558cb0` keeps the vtable `+0x20` dword in a local (`mov ebp,[eax+0x20]` at `0x00558cc1`) and calls the
     RwGlobals `+0xe8` slot. The slot call gets **two** stack args (result, `param_2`), not Ghidra's one: `call ebp`,
     `add esp, 4` (`0x00558cf6`) pops only the record, then `push eax` / `call [ebx]` / `add esp, 8` (`0x00558cf9..fc`).
   - C1: `0x0055ae70` `CONCAT22(extraout_var,uVar2)` becomes `(uint)uVar2`. The high half is stale EDX from the call to
     `0x00565570` (`push edx` at `0x0055aebf`); the callee `0x00565200` masks it (`and ecx, 0xffff` at `0x00565208`).
     The other two 16-bit pushes reach `0x00565260` (masked, `and ebx, 0xffff` at `0x00565277`) and `0x00565570`
     (masked, `and ecx, 0xffff` at `0x0056557d`).
3. `re/tools/decomp2port.py --emit-dir Lane2 --apply`: 17/17 emitted (13 `PORT:ret`, 4 `PORT:void(tracked)`), all
   wrappers `ShadowAB::RunTracked`.
4. Hand fixes from disassembly (`disasm_lin.py`, `scan_eax_use.py` on `MASHED.exe.unpatched`), each cited in the TU:
   - `0x00559040`: `.rdata` `0x005cc320` (1.0f), `0x005ccac8` (`3eaaaaab`), `0x005e4e28` (`39800000`) read as float, not
     as `const unsigned int` (D4-WSB-PORT-BVH amendment 1 class). `SQRT` -> one `sqrtf` reused for the store and the
     divide (`fsqrt` at `0x005590f2`, `fld st(0)` / `fstp [edi]` / `fdivr` at `0x005590fd..0x00559101`). The compare is
     written `!(x > C)`: `fcomp` / `test ah,5` / `jp` at `0x005590e9..f0` sends NaN to the zero path, Ghidra's `<=` would not.
   - `0x005595d0`: `.rdata` `0x005cc328` (`3c23d70a`) read as float. Arg 5 of `0x005645b0` is the raw float bits of
     `param_4 * 0.01f` (`fstp dword ptr [esp]` at `0x005597c8`) and `puVar7[3]` stores the raw `param_4` dword
     (`mov ecx,[esp+0x20]` / `mov [edi+0xc],ecx` at `0x005595d8`/`0x005595f8`): the generator wrote numeric float->uint
     conversions for both. The `0x00563df0` thunk returns EAX (`mov [edi+0x54],eax` at `0x005597bb`). Pointer -> dword cast.
   - `0x00559b50`: Ghidra types it void; its only caller stores EAX (`mov [esi],eax` at `0x0055dcc0`, `scan_eax_use.txt`
     READ32) and nothing after the `0x005595d0` call writes EAX, so the port returns that call's result. Its three locals
     are one 12-byte block at `[esp+4..0xc]` passed by address (`lea eax,[esp+0x20]` at `0x00559b86`), whose `+8` dword
     `0x005595d0` reads and advances: now one array. Arg 4 to `0x005595d0` is the raw dword (`push ecx` at `0x00559b84`).
     The `0x00564130` thunk returns EAX (`0x00559b77`).
   - `0x00558c50`: pointer return of `0x005c4ad0` cast to the int it is stored as.
   - Shadow counter phase: the 14 census `menu` / `race_load` rows use `ShadowAB::kPhaseAny`; `0x00559040`, `0x0055ad30`,
     `0x0055ae70` (census `race_run` only) keep `kPhaseRace` (D4-WSB-PORT-SOLV amendment 1).
   - Not changed, recorded for the read-out: `0x00558cb0`'s slot callee sees `param_2` in the arg slot the first
     callee (`call ebp`) was given, which a cdecl callee may write; the port passes a fresh `param_2`. (`0x00559c40`'s
     `param_3` reassignment is register-only in the original, `mov ebx,[esp+0x30]` at `0x00559c44`, no slot write.)
   - `scan_eax_use.txt`: the other 3 void rows (`0x00558ca0`, `0x00559be0`, `0x0055ad20`) have every caller KILL EAX
     before any read. They stay void.
   - Callees bound to existing ports by the generator: `FUN_00564c80` / `FUN_00565260` (`Collision/RwpSolverCore4.cpp`),
     `FUN_00565200` (`Collision/RwpSolverBroadphase3.cpp`), `UtilDeref55dec0` (`Util/UtilLeaves_ab6.cpp`). Every other
     callee is a raw-RVA thunk to the original.

## Default-OFF knob

- All 17 register as `L2_FUN_<rva>`. `HookSystem::InstallAll` skips `L2_` names unless `MASHED_HOOK_ONLY` lists them
  or `MASHED_HOOK_LANE2=1` (`Core/HookSystem.cpp:244-253`). They are in `asi_sources.rsp` only, not in
  `exe_sources.rsp`. A default `.asi` run and `mashed_re.exe` do not carry them. No exe source changes. Nothing flips a default.
- Float codegen: SSE2 (default). None of the 17 is added to `mashedmod/x87_tus.txt`.

Build `MASHED_NO_DEPLOY=1` at `5b9eb00b2`: `mashed_re.exe` `d8c498ecca0581be...5b00`, `mashed_re_dev.asi`
`6965a5677515c242...d9a9`. The `.asi` is copied to the private root `D:/MashedWT/D4-WSB-PORT-BROAD-1_root/original/`
only, never to `original/`.

## Boot check (done before this PREREG)

2026-10-29 20:22-20:27Z: endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before and after,
`scenario_launch.py --track 0 --mode 10 --cars 4 --hold 15` on the private root, no hooks, under `mashed_lock.py`:
race running (phase 3), game PID 17040 (exited on its own), 0 MASHED Event-1000 events. Transcript
`D:/MashedWT/D4-WSB-PORT-BROAD-1_data/bootcheck.txt` (SHA-256 `6c116fe60b827ef0fae3a73d9bf2bb9c7ab5e88ed62f5b0b89214245c1f1483f`).

## Arm S (shadow A/B, one hook installed alone per boot)

`verify/d4wsbportbroad1_20261029/drive_pb1.py shadow <rva...>` (copy of `verify/d4wsbportbvh_20261029/drive_pb.py`,
runner name, data path and label changed) around `run_shadow8.py` (copy of `run_shadow7.py`, data path and label
changed), wrapped in the test slot, each row in its own solo window under `mashed_lock.py`. Order: the 17 rows in
address order.

Env on every boot: `MASHED_ROOT=D:/MashedWT/D4-WSB-PORT-BROAD-1_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
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
  `NO_LOG`, or a non-U-9677 crash with the hook armed (crash reported with its offset). A DIVERGENT row is not
  re-fixed and re-run in this step without an amendment committed first.
- **No C4 in this step.** C4 needs the race diff and the write-set leg (`re/CONFIDENCE.md`, L-38/L-40/L-54).

### C3 gate pre-check (hooks.csv at `f6033d936`, `decomp_pc.py --callers --xrefs`, pool0)

- Caller half met by a C2 caller: 14 rows. Callee half: C2+ callee, or none (leaf; the vtable / slot calls are
  runtime-data dispatch, CONFIDENCE.md indirect-dispatch clause).
- Caller half **not met** (shadow arm still runs, row stays C2 with a dated note even if CLEAN): `0x00558c50` (only
  xref `0x0055164c` in `FUN_00551610`, no hooks.csv row), `0x00558cb0` (only xref `0x00551982` in `FUN_005518f0`, no
  hooks.csv row), `0x0055b030` (only xref `0x005a59e5`, inside no Ghidra function). All three callers are anonymous,
  so the identified-caller clause does not apply either.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
machine lock, must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (G-CTRL `5943c5f1/306186d4/52052890`,
B `3a03670c/9fcdbd3a/53778664` at `f6033d936`). A mismatch is a RED for the step.
