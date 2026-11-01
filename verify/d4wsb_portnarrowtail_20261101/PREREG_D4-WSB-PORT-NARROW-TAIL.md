# PREREG D4-WSB-PORT-NARROW-TAIL

Committed alone, before any arm. Branch `orch/D4-WSB-PORT-NARROW-TAIL` off `orch/integration` @ `0e3d71a75`.
Input: the 13 `D4-WSB-PORT-NARROW-TAIL` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (NARROW 4 + TAIL 9,
all C2, census EXEC). Method: the D4-WSB-PORT-BROAD-1 recipe, plus the D4-WSB-PORT-SOLV pattern for rows that already
have a body (`.asi`-only `RunTracked` wrapper under `#ifndef MASHED_STANDALONE`).

## Scope: 12 rows armed, 1 held

| rows | what this step does |
|---|---|
| `00574ac0 0057c1b0 0057c300 0057c500` (4) | Lane-2 ports from `decomp2port.py`; `0057c300` and `0057c500` hand-fixed (below). |
| `00574920 005749b0` (2) | Lane-2, generator TU with the body hand-written (below). |
| `00574a20 0057c220 0057c550` (3) | Lane-2, hand-written (generator refused: `UNKNOWN_TYPE` string global; `CALLEE_REG_ARG` x2). |
| `0057c270 0057c2b0 0057c440` (3) | Body already in `Collision/RwpVtableKV2.cpp` (KV Batch A). This step adds an `.asi`-only `ShadowAB::RunTracked` wrapper; the exe keeps the unwrapped body. |
| `0057c420` (1) | **Held, no arm.** Body already in `Collision/RwpVtableKV2.cpp`, typed void. Three callers read its EAX (see "Held row"). A void shadow wrapper compares memory only and would pass a body that drops the return value, so the row is not armed. Stays C2. |

Lane-2 TUs: `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`, commit `997947a7b`, registered `L2_FUN_<rva>` (opt-in). None of
the 9 had a body in `mashedmod/src/mashed_re/` before (the other mentions are raw-RVA thunks), so no TU is a dual copy.
rva-lint: 0 NEW.

### How the TUs were made

1. `decomp_pc.py --file rvas.txt --callees --callers --xrefs --port --json` (pool0, 2026-11-01). `decomp2port.py` dry
   run: 6 `PORT:ret`, 2 `REFUSE:CALLEE_REG_ARG`, 1 `REFUSE:UNKNOWN_TYPE`, 4 `SKIP:already-ported`.
2. `decomp2port.py --emit-dir Lane2 --apply`: 6 emitted.
3. Hand fixes and hand-written bodies, from the disassembly (`disasm_lin.py`, `scan_eax_use.py` on `MASHED.exe.unpatched`),
   each cited in its TU:
   - `0x00574920`: Ghidra's `cpuid_basic_info(0)` is a pseudo-op. Written with `__cpuid(r, 0)`, stores EBX, EDX, ECX to
     `p[0..2]` (`0x00574958..0x0057495e`). The SEH frame (scope table `0x005e5948`: filter `0x0057497e` returns 1, handler
     `0x00574984` returns 0) is reproduced with `__try / __except(1) { return 0; }`.
   - `0x005749b0`: the decompilation keeps only `return 1`. The guarded `xorps xmm0, xmm0` probe (`0x005749dd`) and its SEH
     frame (scope table `0x005e5958`, same filter/handler shape) are reproduced with inline asm inside `__try`.
   - `0x00574a20`: string globals passed by address (`0x00618448` "GenuineIntel", `0x00618438` "AuthenticAMD") to the
     original `_strncmp` `0x004a3a90`; `cpuid` leaf 1 EDX bit `0x2000000` (`0x00574a75..0x00574a8e`). Its one caller KILLs
     EAX (`0x00567c45`), so it stays void.
   - `0x0057c220`: `0x0055dff0` is called with 2 pushed args (`0x0057c252..0x0057c25c`), Ghidra's prototype shows 1.
   - `0x0057c550`: `0x0055b930` reads 1 stack arg (Ghidra shows 0); both callees share one pushed block and each reads only
     `[esp+4]`, so each port call pushes one fresh arg. RwGlobals `+0x10c` slot, one cdecl arg.
   - `0x0057c300`: the table address stored as a dword (`mov dword ptr [esi], 0x62403c` at `0x0057c31a`).
   - `0x0057c500`: `0x00564130` returns EAX (stored at `0x0057c533`); its thunk now returns it.
   - Shadow counter phase: census `race_load` / `menu` / `race_load,race_run` rows `kPhaseAny`; census `race_run`-only
     rows `0x0057c2b0` and `0x0057c440` `kPhaseRace`.
4. `Collision/RwpVtableKV2.cpp`: under `#ifndef MASHED_STANDALONE` the three bodies become `_impl` and an `extern "C"`
   wrapper keeps the `FUN_` name and the `RH_ScopedInstall`. In the `.asi`, `FUN_0057c2b0`'s body calls
   `FUN_0057c440_impl` (the port body), as the exe body calls `FUN_0057c440`. The exe source is unchanged.

### Held row: `0x0057c420`

`0x0057c420` ends with the tail of a call to `0x0055bd80` (`call 0x55bd80` / `add esp,0x10` / `ret` at
`0x0057c435..0x0057c43d`), so its EAX is `0x0055bd80`'s EAX, which is the EAX of the shape slot call
`call dword ptr [edx+0x10]` (`0x0055bdca`, `add esp,0x10`, `ret` at `0x0055bdd4`). `scan_eax_use.py` finds all three direct
callers READ32 that EAX: `0x0047d213`, `0x0047d30e`, `0x00480263`, each `push eax` as the third argument of `0x0055ab30`,
which tests it (`mov eax,[esp+0x18]` / `test eax,eax` / `je` at `0x0055ab69..0x0055ab6f`). The existing ports
`FUN_0057c420` (`Collision/RwpVtableKV2.cpp`) and `FUN_0055bd80` (`Collision/RwpSolverGlue5.cpp`) are both void, and both
install by default in the `.asi` (non-`L2_` names). The fix is to return the slot call's EAX through both. That changes
default `.asi` behaviour, so it is not made here; it is reported in the RESULT for the lead.

## Default-OFF knob

- The 9 Lane-2 TUs register as `L2_FUN_<rva>` (`asi_sources.rsp` only): installed only through `MASHED_HOOK_ONLY` or
  `MASHED_HOOK_LANE2=1`. A default `.asi` run and `mashed_re.exe` do not carry them.
- The 3 KV2 wrappers keep the existing `FUN_` names, which install by default in the `.asi` (as before this step). With
  `MASHED_SHADOW_AB` unset the wrapper calls the same body it wraps. `mashed_re.exe` compiles the unwrapped bodies; no exe
  source changed. Nothing flips a default.
- Float codegen: SSE2 (default). Nothing added to `mashedmod/x87_tus.txt`.

Build `MASHED_NO_DEPLOY=1` at `997947a7b`: `mashed_re.exe` `26f6e7d6f06cc9bd...1a1a`, `mashed_re_dev.asi`
`343d85914ede226a...a390` (0 compile errors, no warning in the changed TUs). The `.asi` is copied to the private root
`D:/MashedWT/D4-WSB-PORT-NARROW-TAIL_root/original/` only, never to `original/`.

## Arm S (shadow A/B, one hook installed alone per boot)

`verify/d4wsb_portnarrowtail_20261101/drive_nt.py shadow <rva...>` (copy of `verify/d4wsb_portbroad2_20261101/drive_pb2.py`,
runner name, data path and label changed) around `run_shadow10.py` (copy of `run_shadow9.py`, data path and label changed),
wrapped in the test slot, each row in its own solo window under `mashed_lock.py`. Order: the 12 armed rows in address order.

Env on every boot: `MASHED_ROOT=D:/MashedWT/D4-WSB-PORT-NARROW-TAIL_root`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1
MASHED_SHADOW_STW=1`, `TRACK=0`, mode 10, 4 cars, `HOLD=60`, `SAMPLES_DONE=24`. This is the census scenario.
`scenario_launch.py --hooks 0x<rva>` sets `MASHED_HOOK_ONLY`, which installs only that hook (for the 3 KV2 rows, only that
`FUN_` hook; the other default hooks are not installed on that boot).

Audio rule: before each launch `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list a VB-Audio endpoint. If not,
or if an Event-1000 fault at `0x005baf48` appears: stop, BLOCKED "audio endpoint", no retry. A U-9677 crash
(`MASHED.exe+0x1509b4` / `+0xe66db`) is re-run (max 2), not a finding.

## Read-out per row (fixed now, from `re/tools/shadow_ab_report.py`)

- **C2 -> C3**: verdict `CLEAN` (n > 0 samples, ndiff 0 on every sample, `PATCHBYTE ... A/B-IS-REAL`), plus the
  re-classify C3 gate (callers / callees at C2+). Heap pages compared are recorded for a later C4 leg.
- **Stays C2, dated note**: `DIVERGENT`, `DIVERGENT_FLOAT10`, `UNPROVEN`, `SKIPPED`, `NO_SAMPLES`, `NO_LOG`, or a
  non-U-9677 crash with the hook armed. A DIVERGENT row is not re-fixed and re-run in this step without an amendment
  committed first.
- **No C4 in this step.**

### C3 gate pre-check (hooks.csv at `0e3d71a75`, `decomp_pc.py --callers --xrefs`, pool0)

- Caller half met by a C2+ direct caller: 11 armed rows (`0x00567c40` C3 for `00574a20` / `00574ac0`; `00574a20` C2
  for `00574920` / `005749b0`; `0x0057c500` for `0057c1b0`; `0x004218b0` C3 / `0x004218d0` C3 and C2 sites for
  `0057c220` / `0057c300` / `0057c500` / `0057c550`; `0x00493640` C2 for `0057c270`; `0x0057c2b0` C2 for `0057c440`).
- `0x0057c2b0` has no direct caller. Its one reference is the store `mov [0x624058], 0x57c2b0` at `0x0057c293` inside
  `FUN_0057c270` (C2): `re/analysis/pointer_callers_2026-10-11.tsv` row kind `store_mem`, which CONFIDENCE.md's
  installer-caller clause lists as a code-kind reference Ghidra's caller set already counts. The row is armed; if
  re-classify does not accept that reference as the caller half, it stays C2 with a dated note.
- Callee half: C2+ callee or none for every armed row.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
machine lock, must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (G-CTRL `8284be46/ba407a16/31fe043e`,
B `1d5bf4f9/1d5bf4f9/1d5bf4f9` at `0e3d71a75`). A mismatch is a RED for the step.

## Amendment 1 (2026-11-01, recovery run, committed before the re-runs it governs)

LIVING-PC rebooted at 14:23 local (17:23Z) during run 28, while arm S row 1 (`0x00574920`) was in its window. That
attempt took no sample (summary row NO_LOG at 17:23:17Z; the post-run endpoint list was empty because the machine was
shutting down). Its partial output is kept as `D:/MashedWT/D4-WSB-PORT-NARROW-TAIL_data/00574920/shadow_transcript_reboot_cut.txt`
and is not evidence. Arm S restarted from row 1 for all 12 armed rows, same build (`.asi 343d8591`), same env.

The default gates finished before the reboot (14:12-14:14 local, exe `26f6e7d6`, every arm self-exited) and are not
re-run: they are complete runs on this step's exe.

Launch-failure rule (same as D4-WSB-PORT-BROAD-2 amendment 1): a row whose transcript shows `scenario_launch pid=N`
followed by `error: could not attach` took no sample, so its NO_LOG is a launch failure, not a hook outcome. It is
re-run once after the 12-row pass, same build and env. A second launch failure leaves the row at C2 (NO_LOG).

- Recovery pass row 1 `0x00574920`: transcript `scenario_launch pid=17208` then `error: could not attach`. Re-run once.
- Recovery pass row 11 `0x0057c500`: transcript `scenario_launch pid=1772` then `error: could not attach`. Re-run once
  under the same rule (listed here before that re-run).
