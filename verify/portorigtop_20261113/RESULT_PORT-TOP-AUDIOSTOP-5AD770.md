# RESULT PORT-TOP-AUDIOSTOP-5AD770: 0x005ad770 matched (100% byte-identical) and bound on the image path

## Verdict: GREEN (0x005ad770 shaped to a byte match and bound; the init arms get past 0x005AD770 and stop at its first call, the named stop 0x005A61B0)

- **Self-test: PASS in both arms, both builds.** portstate-image PASS (0 of 8 checks failed, witness PortStateSeed
  272/272), OT_CODEPTR PASS (0 of 367 checks failed; 188 slots, 11 bound to an exe body, 177 to a stop thunk),
  code-trap PASS (0 of 6 checks failed, 459/459 pages).
- **Arms** (2026-11-15, test-slot slot 2/8 after 948 s of memory wait, mashed_lock taken by this run as pid 3520 and
  released; ran local, paths outside the repo):

  | arm | build | exit | where | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005A61B0** after 26.4 s | named stop, return 0x0023C28E = `_PortTop_Bound_005ad770+0xe` (`mashedmod/build/mashed_re.map`, symbol at 0x0023c280) | c76955eb22f3f006 |
  | ddk_init_audio | object | **0x005A61B0** after 24.5 s | named stop, return 0x00049D6F = `_PortTop_Bound_005ad770+0xf` (`mashedmod/build/ddkobj/mashed_re_ddk.map`, symbol at 0x00049d60) | 704a9ea778526b3e |

  - No stop at 0x005AD770 any more in either build. The stop line reads
    `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005A61B0 (DAT_006904e8 = 0, DAT_006904ec = 0)`.
  - The return lands right after the first call in 0x005ad770's body in both builds: the source's first statement
    `uVar1 = FUN_005a61b0(param_1)`; in the original that call is at 0x005ad77a and ends at 0x005ad77f (0x005ad770 + 0xf, 5-byte call after
    10 bytes of pushes / load). The object-build offset +0xf equals the original (the DDK body is byte-identical:
    `push ecx; push ebx; push ebp; push esi; mov esi,[esp+0x14]; push edi; push esi; call`).
    [UNCERTAIN: the VS2022 offset +0xe comes from its map only; the VS2022 body is not the DDK
    body and its byte layout was not disassembled.]
  - Last line before the stop in both logs: `0x004627f0 sound engine HW init: provisional exe body`, the same as the
    previous row.
- **New row filed** (`roadmap.ps1 add`, section D4): `PORT-TOP-AUDIOSTOP-5A61B0` (the new stop; `re/match/s25/005a61b0.c`
  is 71.4%, `re/match/s27/005b35a0.c` 60.0%).
- Gates: not run (top-down row). No menu walk: no menu code changed.

## 1. equiv and shaping (evidence in `audiostop_5ad770/`)
- `re/tools/match/equiv.py` (orch/integration copy, no diff on this branch): `0x005ad770 NOT-PROVEN unsupported: rep movsd
  with a variable count` (`equiv.txt`). The same verdict on the old 69.9% source. The tool cannot prove a body with the
  memcpy intrinsic, so the route taken is a byte match, not an admission.
- **Shaping, one attempt, first form 100%** (`match_before.txt` = old 69.9% diff, `match_bytes.txt` = new):
  - The two `memcpy` calls moved out of the `param_5` arms into one shared tail (`if (param_2) memcpy(+0xc ...)`,
    `if (param_3) memcpy(+0x10 ...)`), which gives the original's single copy block at 0x005ad83b..0x005ad87d and its
    jumps from both arms (0x005ad7e8 `jmp 0x5ad83b`, 0x005ad7f5 `jmp 0x5ad85e`).
  - `+0xc = 0` stored before `+0x10 = 0` (original order at 0x005ad7ba / 0x005ad7bd).
  - `+0x14 = param_1` stored before the call to 0x005b35a0 (original `mov [ebx+0x14],esi` at 0x005ad889, after `call 0x5ae0b0` at 0x005ad87e and before the call to 0x5b35a0).
    **Behaviour note:** the old source stored +0x14 after that call. The new source follows the original's order. The
    old source was never bound (0x005ad770 was a stop), so no earlier run executed the old order.
  - Result: `match.py --bytes`: 100.0% (regs 100.0%, ops 100.0%, 112/112 ins), 307 B whole symbol, 0 differing unmasked
    bytes, BYTE-IDENTICAL. `relocheck.py`: OK, checked=12 unchecked=0 (`relocheck.txt`). Not inline asm.
- **Promotion** (HB-TRACKER-AT-MERGE): `re/promotions/PORT-TOP-AUDIOSTOP-5AD770.tsv`, route `match`. Dry run
  `c4_reclass.py batch --tsv` (`c4_reclass_dry.txt`): `[1/1] 0x005ad770 match PASS byte-identical 307 B, relocheck OK (12/0)`,
  1 pass, 0 refused. hooks.csv row today: C2. No `--apply` on the branch.
- `PROVISIONAL.tsv` / `ADMITTED.tsv`: 0x005ad770 was in neither, so no drop row (`prov_drop.py` dry run lists no 0x005ad770).

## 2. Binding
- `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`, no code change. Roots: PORT-TOP-AUDIO-BODY-45D460's
  (`D:/MashedWT/PORT-TOP-AUDIO-BODY-45D460_data/roots.txt`, sha256 73566306...). A first run with the RWAUDIO-TABLE
  roots dropped 0x0045d460's closure (stops 0x005a71f0 / 0x005a7b60 that `PortTop_0045d460.cpp` uses), so it was
  discarded and re-run with the 45D460 roots.
  - Plan: 256 bind (226 match, 15 admitted, 8 provisional, 7 replay), 64 reuse, 39 stops.
  - 0x005ad770 binds (pending match promotion). Its callees: 0x005ad6f0 binds (C4 match, new TU), 0x005ad8b0 /
    0x005ad8d0 bind (already), 0x005ae0b0 reuses `AudioSubStructZeroInit`, 0x005a61b0 and 0x005ae800 stay stops,
    **0x005b35a0 is a new named stop**.
  - Two more bind as a side effect of hooks.csv on the current base (C4 match rows that were stops at the last run):
    0x0042b900 and 0x0045caf0 (new TUs `PortTop_0042b900.cpp`, `PortTop_0045caf0.cpp`; their callers
    `PortTop_0045cb20.cpp` / `PortTop_00466b50.cpp` now call the bound body). `PortTop_004cd060.cpp` now includes
    `re/match/s20/004cd060.c` in place of `s30/` (the hooks.csv c4=match note on the base names s20). All four are
    IDENTICAL in the object build (section 3).
  - `PortTop_AudioStops.cpp`: stops 0x0042b900, 0x0045caf0, 0x005ad770 removed, 0x005b35a0 added.
- The generator rewrote the other wrappers with LF line endings only; those were restored and not committed.
- `newfiles.txt` +4, `sync_rsp_allowlist.py` (ARMS_STEP=this row): rsp +4, allowlist +2 (ORIGTOP-ONLY CROSS-TARGET for
  0x0042b900 and 0x0045caf0). `bindings_update.py`: BINDINGS.tsv updated 41, added 2.

## 3. Builds and checks
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. Object build (`MASHED_DDKOBJ=1`): Build OK. rva-lint 316 known, NEW 0.
- `ddkobj.py check`: IDENTICAL 1557/1593, DIFFERENT 21, not-in-map 15, units linked 1543/1755, refusals 11
  (externalized-static-register-convention 7, wrapper-redefines-globals 4).
  - 0x005ad770 `_PortTop_Bound_005ad770` IDENTICAL (`re/match/s26/005ad770.c`); 0x005ad6f0, 0x0042b900, 0x0045caf0 IDENTICAL.
  - DIFFERENT set: the 21 rvas are a subset of the 28 in PORT-TOP-AUDIOSTOP-5ADC10's and PORT-TOP-AUDIO-BODY-45D460's
    checks. No unit is newly DIFFERENT. The 7 that left the DIFFERENT list (0x00401690, 0x0043c5b0, 0x0045cd30, 0x00498510,
    0x00498810, 0x004c0c20, 0x004c0f00) are not rows of the substituted set on this head (their sources now appear only
    as other IDENTICAL symbols or refusals). This branch does not touch those sources, so the change comes from the
    orch/integration commits since those checks. [UNCERTAIN: no fresh check of the base 9a44eee36 was run; the
    comparison is against the two previous rows' checks.]

## 4. Arms command
`ARMS_STEP=PORT-TOP-AUDIOSTOP-5AD770 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOSTOP-5AD770_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIOSTOP-5AD770 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
- Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOSTOP-5ADC10_root`, its old `mashed_re.log` removed.

## 5. Next
- `PORT-TOP-AUDIOSTOP-5A61B0`: a body for 0x005a61b0 (71.4%), then 0x005b35a0 (60.0%) and 0x005ae800 behind it.
- `TOOL-EQUIV-PUSHECX-SLOT` (open from the previous row) still gates an admission of 0x005adc10.
- equiv.py has no support for `rep movsd` with a variable count: every memcpy-intrinsic body in this closure needs a
  byte match to run off the provisional list.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIOSTOP-5AD770_data/`, sha256)
- `vs_init_audio.log` c76955eb22f3f006e899027328c944fb461d39a2e923bce11a0f94ebae1e0aa8
- `ddk_init_audio.log` 704a9ea778526b3ee8e7dbb26926dfd9d34bf6f67de7a49c8905ce0cfe7d40a0
- `arms1.out` 5d4da00b507ce5e15c2f3394add22d62108c5e3f173e4914ece4babcd2fbf2c7
- `build_vs.log` a75fd4a2cfc74a755bd529cfef4f36662bc8e3d255313fd76d6509e92af85675
- `build_ddk.log` 008593fe06082b46382058555eacc6e97bc6775fa2f12add081f01c9cd3be05d
- `ddkcheck.log` ca9d12cc4817c935b412dae1010313718052d9e6a26e95d6533693694e874106
- `plan_write.txt` 4d13b8e3404f255950d8e0816dea92d62ca5db80297ec6bd22c1f0dba13ba096
- `roots.txt` 73566306c2eb5600447befbb32c4667ff8465d6d18e268986f50ac1fe77142bb
- `005ad770_before.c` 852213716f476e4368211b28a563cfa1473b3d719f750e5834ecc268b5833c57
