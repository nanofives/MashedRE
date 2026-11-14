# RESULT PORT-TOP-AUDIOPOOL-5AE650: 0x005ae650 (AudioPoolConstruct) as a provisional body on the image path

## Verdict: GREEN (0x005ae650 bound as a provisional body; the init arms get past 0x005AE650 and stop at the next named stop, 0x005ADC10)

- **Self-test: PASS in both arms, both builds.** portstate-image PASS (0 of 8 checks failed), OT_CODEPTR PASS (0 of 359
  checks failed; 188 slots, 99 image slots written, 0 refused), code-trap PASS (0 of 6 checks failed, 459/459 pages).
- **Arms** (run 2026-11-14 14:44 after the LIVING-PC reboot, test-slot slot 6/8 after the 10 min memory wait, mashed_lock
  taken by this run, pid 4128, the stale pre-reboot SOAK-P0 holder reclaimed by the tool itself):

  | arm | build | exit | where | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005ADC10** after 39.9 s | named stop, return 0x0023671D = `_PortTop_Bound_005add60+0xd` (`mashedmod/build/mashed_re.map`) | adc1083bbb69febc |
  | ddk_init_audio | object | **0x005ADC10** after 27.6 s | named stop, return 0x000434BC = `_PortTop_Bound_005add60+0xc` (`mashedmod/build/ddkobj/mashed_re_ddk.map`) | b3e12bb281f040be |

  - Both builds stop at the same place: no stop at 0x005AE650 any more. The stop line reads
    `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005ADC10`. The caller field of the line is 0; the map
    resolves the return address to 0x005add60 in both builds.
  - The expected next stop was 0x005ae800. It was not reached first: 0x005ADC10 comes first. Per `plan.json`,
    0x005ada90 calls 0x005add60, 0x005ae650, 0x005ae7e0 and 0x005b3580, and 0x005add60 calls 0x005adc10. Which of
    them runs first inside 0x005ada90 is [UNCERTAIN] (the plan lists callees, not call order).
  - The last lines before the stop in both logs: `0x004627f0 sound engine HW init: provisional exe body`.
- **New rows filed** (`roadmap.ps1 add`, section D4): `PORT-TOP-AUDIOSTOP-5ADC10` (the new stop; `re/match/s26/005adc10.c`
  is 78.9%) and `TOOL-EQUIV-EBPDATA-REPSTOS` (the two equiv gaps of section 1).
- **equiv verdict line** (`re/tools/match/equiv.py` @ fea5b5483, the same file as orch/integration, no diff):
  `0x005ae650	NOT-PROVEN	call-args: region ours +0x20 / orig 0x005ae672 call 0x5ae800 arg 3: ebx_in | esi_in`
  So it is not admitted. Listed as **provisional** in `re/match/PROVISIONAL.tsv` (L-126), knob path only, never C4.
- Branch `orch/PORT-TOP-AUDIOPOOL-5AE650`, created from `orch/PORT-TOP-RWAUDIO-TABLE-632BA8` @ c3eb90cee (the
  stack is AUDIOTICK-STOPS aa79679df, then HWINIT-4627F0 7e5466a15, then RWAUDIO-TABLE c3eb90cee).
- Gates: not run (top-down row). No menu walk: no menu code changed.

## 1. Why equiv stops (evidence in `audiopool_5ae650/`)
- `equiv.txt`: the verdict above.
- The callee summary of 0x005ae800 is `N=None`, with `n_why` = 0x005ae80b. That address is `mov edi,[ebp+0x10]`.
  0x005ae800 uses ebp as a data register (`mov ebp,[esp+0x10]` at 0x005ae803 loads its first argument). The summary
  only bounds stack-argument reads when ebp is a frame pointer (equiv.py `_summary_body`, the `bn == "esp" or ebp is None`
  branch), so here it treats every stack slot as possibly read.
  - The original pushes ebx, ebp, esi and edi at 0x005ae650..0x005ae653, before the first branch.
  - The DDK pushes ebx and edi first, and ebp and esi after the allocation block.
  - So the saved register in the slot above the two pushed arguments differs: ESI on the original side, EBX on ours.
- Read from the disassembly of 0x005ae800..0x005ae90c: 0x005ae800 reads two stack arguments, `[esp+0x10]` after 3
  pushes and `[esp+0x1c]` after 5. It stores into the argument-1 slot at `[esp+0x18]`. The differing slot is not read.
  **[UNCERTAIN]: this comes from reading the disassembly. No tool proved it.**
- Diagnostic, not a verdict (`equiv_n2_diag.py` and `.txt`): I forced N=2 for 0x005ae800 in the summary cache. The
  proof then stops at the next point, `unsupported: rep stosd with a variable count` (the memset of the cell body).
  So even with that summary fixed, equiv cannot admit this body today. That takes two tool changes:
  - a summary that bounds stack reads through a non-frame ebp;
  - a model for a variable-count `rep stos`.
  Neither is in this row's scope.
- Shaping was not attempted again. The 74.1% source already records the same prologue-push placement residual as
  005ae250, 005af690 and 005b0df0 (compiler version). The row brief allows provisional when shaping does not reach 100%.

## 2. Binding
- `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`: added `PROV_BIND |= {'005ae650'}`.
- The generator was re-run with the same roots as the table row (`D:/MashedWT/PORT-TOP-RWAUDIO-TABLE-632BA8_data/roots.txt`).
  - Plan: 242 bind (214 match, 15 admitted, 7 replay, 6 provisional), 59 reuse, 37 stops. At the table row it was
    240 / 59 / 38.
  - 0x005ae650 binds (provisional). 0x005aea00 now binds (C4 match, `PortTop_005aea00.cpp` new). 0x005ae780 is bound.
  - **0x005ae800 (AudioPoolBlockAlloc, C3) stays a named stop.** It has `re/match/s26/005ae800.c`, but that source is
    neither admitted nor provisional. This is the next stop I expect for the init arms. Not confirmed: the arms have
    not run.
- Every wrapper that called `PortTop_AStop_005ae650` now calls `PortTop_Bound_005ae650`. The 0x005ae650 stop was
  removed from `PortTop_AudioStops.cpp`.
- The generator also rewrote other wrappers with LF line endings. Those diffs are CRLF-only and were not committed.
- The two new TUs were added to `newfiles.txt`, then `sync_rsp_allowlist.py` (ARMS_STEP=this row): rsp +2, allowlist +1
  (CROSS-TARGET 005aea00). `bindings_update.py`: BINDINGS.tsv updated 38, added 1.

## 3. Builds and checks (done)
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint 301 known, NEW 0. The provisional source compiled as C++ with no
  edit. Log `D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_data/build_vs.log`.
- Object build (`MASHED_DDKOBJ=1`): Build OK. rva-lint NEW 0. Log `..._data/build_ddk.log`.
- `ddkobj.py check`: IDENTICAL 1412/1461, DIFFERENT 34, not-in-map 15, refusals 4 (wrapper-redefines-globals).
  - The DIFFERENT set is the same 34 rvas as the table row.
  - IDENTICAL went up by one (1411 to 1412).
  - 0x005ae650 is provisional, so it keeps its VS2022 body.
  - Log `..._data/ddkcheck.log`.
- `mashed_image.bin`: the build skipped it because the worktree has no `original\` junction. I extracted it with
  `re/tools/image/extract_image.py --exe <main>/original/MASHED.exe.unpatched`: sha256 81CC7775...9AFCD8, equal to the
  table row's. It is copied into the private root.

## 4. Arms command (run 2026-11-14 after the reboot; result in the verdict section)
1. `mashed_lock.py status` showed only the stale pre-reboot SOAK-P0 holder (pid 19856, no such process); `mashed_lock.py run`
   reclaimed it itself.
2. Command, from the worktree:
   `ARMS_STEP=PORT-TOP-AUDIOPOOL-5AE650 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIOPOOL-5AE650 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
   - The private root `D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_root` is ready, image included.
   - Logs go to `D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_data/`.
3. Return addresses symbolized against the build maps (preferred load address 0x00010000), nearest preceding symbol.

## 5. Next
- `PORT-TOP-AUDIOSTOP-5ADC10`: body for 0x005adc10, then the init arms again on this branch.
- `TOOL-EQUIV-EBPDATA-REPSTOS`: with both gaps closed, re-run equiv on 0x005ae650 (it may then be admitted).
- The audio stack (AUDIOTICK-STOPS, HWINIT-4627F0, RWAUDIO-TABLE, this row) still merges together only after the arms pass.

## Raw data
- `D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_data/`: `plan_write.txt`, `build_vs.log`, `build_ddk.log`, `ddkcheck.log`,
  `arms1.out` (queue lines only, no game ran), `arms2.out`, `vs_init_audio.log`, `ddk_init_audio.log`.
