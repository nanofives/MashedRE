# RESULT PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13, run 44, section D4 T1)

Verdict: **RED**. Top-down row (knob path only). Branch `orch/PORT-TOP-AUDIO-HWINIT-4627F0`, on top of
`orch/PORT-TOP-AUDIOTICK-STOPS` @ aa79679df.

**Self-test results:**
- Control arms (AUDIOTICK only, gate closed): **`[OT_TIMER] self-test PASS (0 of 8413 checks failed)`** on both builds,
  600 frames, audio branch 600.
- Both-knob arms (AUDIOINIT + AUDIOTICK): no self-test summary on either build. The run faults inside 0x004627f0, before
  0x004669b0 returns, so the `[OT_AUDIOINIT]` self-test (it runs after 0x004669b0) never prints.

What landed:
1. 0x004627f0 runs as an exe body under `MASHED_OT_AUDIOINIT=1`: an L-126 provisional body (`re/match/PROVISIONAL.tsv`,
   knob path only, never C4). It replaces hand piece 3 (`HandTop_InitAudio`) at that call.
2. Its closure is bound: 33 matched bodies (all IDENTICAL in the object build), 1 more provisional body (0x005aa0c0), and a
   new CRT binding for `sprintf` (0x004a2b60).

The both-knob boot now gets into 0x004627f0 and faults in its first callee, 0x005a5f00 (RWS engine open), with an
access violation.

The cause: 0x005a5f60 calls through a table of 16 {init, shutdown} pairs at 0x00632ba8. In the original these are
code addresses. In the exe the table is 0, and none of the 34 functions involved has an exe body. So **the PORT-TOP-AUDIO-RWA
arms do not pass**: they stop earlier than before, at 0x005a5f00, instead of 0x005aab00.

## 1. 0x004627f0 (L-126: equiv.py first)

| rva | source | match.py (DDK, /O2 /GS /Ob1) | equiv.py | outcome |
|---|---|---|---|---|
| 0x004627f0 | re/match/s10/004627f0.c | 96.8% (93/94 ins) | NOT-PROVEN `call-memory: region ours +0xe8 / orig 0x004628d8 call 0x5baf40: memory differs` | PROVISIONAL.tsv |

- equiv.py was run twice, with the branch copy and with the `orch/integration` copy (e47e10795, which differs from the branch
  copy). Both return the same NOT-PROVEN verdict.
- The residual is the one the source header records:
  - the original keeps `or [eax+0x70],0x10` and `or [eax+0x70],0x40` as two stores, around the +0x6c / +0x68 float stores;
  - the DDK folds them into one `or 0x50`;
  - equiv.py does not prove that the reordered +0x70 stores are the same memory as the original at the 0x005baf40 call.
- Compiler shaping was not tried again. The source header lists 219 permuter compiles with no gain.

0x005aa0c0 (`AudioTreeWalkPredicateSearch`, called unconditionally by the matched 0x005aa060):
- It already had a `provisional` row in PROVISIONAL.tsv.
- That row records a PROV-FIX-9 source fix and `IDENTICAL-FOR-2724` after the fix.
- It binds through the generator's `PROV_BIND`, the same narrow list that holds 0x005a6ea0 / 0x005a6f30.

## 2. Closure (`tools/audiotick_bind_gen.py`, roots 00466b50,0045d3f0,0045d430,004627f0)

Generator changes:
- `PROV_BIND` gains 004627f0 and 005aa0c0.
- New wrappers (rvas not in the previous `plan.json`) carry a header naming the 0x004627f0 closure and
  `MASHED_OT_AUDIOINIT`. Older wrappers keep their text.
- `sync_rsp_allowlist.py` takes `ARMS_STEP` for the allowlist comment.

Plan:
- 167 bind (155 match, 6 replay, 4 provisional, 2 admitted), 42 reuse, 27 stops.
- Before this row: 132 bind, 35 reuse, 23 stops.

New exe bodies (35 TUs):
- Matched (33): 0x004624c0, 0x00550400, 0x00550430, 0x005504d0, 0x00550520, 0x00550740, 0x00550750, 0x00550c30,
  0x00550c50, 0x00550d80, 0x00550da0, 0x00551050, 0x00551090, 0x00551180, 0x00551190, 0x00551290, 0x005512b0,
  0x005512d0, 0x00551330, 0x00551410, 0x005514e0, 0x00551550, 0x005a5f00, 0x005a5f60, 0x005a6110, 0x005a6190,
  0x005a8e70, 0x005aa060, 0x005aa1e0, 0x005ad080, 0x005b9e30, 0x005bbb20, 0x005bbb70.
- Provisional (2): 0x004627f0, 0x005aa0c0.

Reused existing exe bodies:
- `Orig_004963e0`
- `MashedCrt_sprintf_004a2b60` (new, below)
- `Orig_004d7ff0`
- `RwCore_00551510`
- the C4 hand leaves `AudioOutputNodeCbDispatch` (0x005a9e40), `AudioRendererField3cSet` (0x005baf40) and
  `AudioBufFieldSet` (0x005baf60)

New named stops (4):
- 0x00550f20: stored as a handler method at +0x34 by 0x00551190, not called directly.
- 0x00551460: called by 0x00550c50 / 0x005512d0. C2, source not 100%.
- 0x005a5fd0 and 0x005a6010: the per-entry callbacks 0x005a5f00 passes to 0x005a5f60. No hooks.csv row, no source.

**sprintf binding.** `Match/PortTop_CrtStdio.cpp` gains `MashedCrt_sprintf_004a2b60`.
- hooks.csv records 0x004a2b60 as libid identical `_sprintf` (libcmt.lib sprintf.obj), a waived CRT row (L-67).
- The binding uses the same UCRT legacy options as the existing `MashedCrt_vsnprintf_004a3b84`, with count (size_t)-1.
- Its caller on this path: 0x004627f0's two "SOUND CARD" lines.

**0x004669b0 wrapper** (`Match/PortTop_004669b0.cpp`):
- `PortTop_AInit_004627f0` now calls `PortTop_Bound_004627f0` and logs six globals after it returns. The fault means that
  line has not printed yet.
- The 0x0045d460 stand-in (permdict.rws) stays and is not run. With the knob on, no permdict bank is loaded.
- Knob off: unchanged (hand piece 3).

Source edit (C++ compile only):

| source | edit | match.py --bytes | relocheck |
|---|---|---|---|
| s10/004624c0.c | `(undefined4 *)DAT_0068f4cc` at the 0x00550430 call | 61 B BYTE-IDENTICAL | OK checked=5 |

The src sha256 recorded in the 004624c0 row's hooks.csv C4 note is now stale. The verdict is unchanged. hooks.csv is not
edited on a branch.

## 3. Builds and checks
- **VS2022** (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint OK, 283 known, NEW 0. 5 new ORIGTOP-ONLY allowlist lines from
  the lint's `--seed`: DUP-IN-TARGET 0x005aa0c0, and CROSS-TARGET 0x00551180, 0x005514e0, 0x005a6190, 0x005aa1e0.
  Log `D:/MashedWT/PORT-TOP-AUDIO-HWINIT-4627F0_build_vs.log` (534c7539d9c06862).
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. Log `D:/MashedWT/PORT-TOP-AUDIO-HWINIT-4627F0_build_ddk.log`
  (e20ed085d014a3f9).
- **`ddkobj.py check`:** IDENTICAL 1344/1378, DIFFERENT 19, not-in-map 15, units linked 1324/1604, 4 refusals
  (`wrapper-redefines-globals`, not this row's). Log `D:/MashedWT/PORT-TOP-AUDIO-HWINIT-4627F0_ddkcheck.log`
  (eace8e93f1db5ceb).
  - Against PORT-TOP-AUDIO-RWA: +33 IDENTICAL, the same 19 DIFFERENT. **All 33 new matched bodies are IDENTICAL.**
  - 0x004627f0 and 0x005aa0c0 are `skip:provisional` (VS2022 body, no byte check, L-126 (6a)).

## 4. Knob boots (PORT-TOP-AUDIO-RWA's arms, re-run on this branch)
Setup:
- Wrapping: `test-slot` outside (`PROYECTOS_TEST_HUB=off`), `mashed_lock.py run --label PORT-TOP-AUDIO-HWINIT-4627F0`
  inside.
- Root `D:/MashedWT/PORT-TOP-AUDIO-HWINIT-4627F0_root`, a private root.
- Script `audiotick_stops_arms.py` with `ARMS_STEP` and the new `ARMS_ROOT`.
- Common knobs: `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`.
- Output `arms_hwinit_run1.out`.

| arm | extra knobs | result | log sha256 (16) |
|---|---|---|---|
| vs_audio | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 | c1555c42fa096cd1 |
| ddk_audio | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 | 38cac5bc0acd20cb |
| vs_init_audio | AUDIOINIT + AUDIOTICK | last line `0x004627f0 sound engine HW init: provisional exe body`, then 0xC0000005 after 11.1 s | a3145bd493bb5855 |
| ddk_init_audio | AUDIOINIT + AUDIOTICK | same last line, then 0xC0000005 after 14.7 s | a0089add6ec65acd |

**RWA verdict on this branch: vs_init_audio / ddk_init_audio do NOT pass.**

### Fault site
Frida did not locate the fault:
- The scratch copy of `catch_standalone_phys_crash.py` caught no exception in 30 s.
- An Interceptor trace of the closure stopped at the same log line with no callee entered.

I then made one diagnostic VS2022 build (VS2022 only, not committed):
- `Match/PortTop_004627f0.cpp` routes each callee of 0x004627f0 through a logging shim.
- A vectored exception handler is installed around the 0x005a5f00 call.
- The shim copy is `D:/MashedWT/PORT-TOP-AUDIO-HWINIT-4627F0_data/diag_PortTop_004627f0.cpp` (81a8bd8fb94b85bc).
- The log is `diag_veh.log` (d0a4b483530cd5a9) and `diag_mashed_re.log` (e4eb8203a90bb6ae).
- The generated wrapper was restored and the VS2022 exe rebuilt from it afterwards.

Diagnostic log:
```
enter 005a5f00; dword at 0x00632ba8 0x00000000
dword at 0x00632bac 0x00000000
VirtualQuery(0x005a5e70) State 0x00001000 Protect 0x00000008 Type 0x01000000   (committed, image, read-write, holds 0)
VEH AV ExceptionAddress 0x00000000
VEH AV op (0 read 1 write 8 execute) 0x00000008
VEH AV target 0x00000000
VEH AV esp[0] (return address) 0x0022A85A  = _PortTop_Bound_005a5f60 + 0x1a (mashedmod/build/mashed_re.map)
```
`leave 005a5f00` never prints.

0x005a5f60 (re/match/s25/005a5f60.c) calls `(*puVar3)()` for each entry of its table argument. 0x005a5f00 passes table
0x00632ba8 with count 0x10. In the exe the first dword of that table is 0, so the first call executes address 0.

The table in `MASHED.exe.unpatched` (read from the file, 16 {first, second} pairs):
```
(005a5e70, 005a5ee0) (005ae5b0, 005ae610) (005ae1d0, 005ae210) (005adf90, 005adfc0)
(005add80, 005addb0) (005ada90, 005adb70) (005a9970, 005a9a70) (005aa480, 005aa520)
(005acff0, 005ad030) (005ac550, 005ac590) (005a7810, 005aba00) (005ab9d0, 005a78b0)
(005a6e50, 005a6e90) (005ab4a0, 005ab510) (005a6af0, 005a6b30) (005aafb0, 005aafe0)
```
This matches `verify/audiolink_20261023/POOL_CONSTRUCTOR.md` ("Who calls the inits").

- None of these 32 rvas, nor the callbacks 0x005a5fd0 / 0x005a6010, has a hooks.csv row, a re/match source, or a row in
  ADMITTED.tsv / PROVISIONAL.tsv.
- 0x00632ba8 is not one of the 22 entries `Compat/PortTopCodePtr.inc` binds (PORT-OT-SEED-CODEPTR,
  `MASHED_OT_CODEPTR`). The arms do not set that knob.

No G-CTRL, no B and no captures, because this is a top-down row. The menu walk was not run: no menu code changed.

## Next
1. **The RW-audio init/shutdown table 0x00632ba8**, as its own row:
   - 32 functions plus the 0x005a5f60 callbacks 0x005a5fd0 / 0x005a6010;
   - none has a hooks.csv row;
   - POOL_CONSTRUCTOR.md has read 0x005ae5b0 / 0x005ae610 only.
   They need discovery (C0 -> C1, Ghidra, including the missed-function check) and sources (match, or admit under L-126).
   Then the table needs exe addresses in place of the 32 original-code dwords. Two ways:
   - add 0x00632ba8 to the codeptr inventory (`verify/portorigtop_20261109/codeptr_inventory.py`, SEED2 entry list), which
     binds bodies or named stops, and run these arms with `MASHED_OT_CODEPTR=1`;
   - or have the 0x004669b0 wrapper write the table before it calls 0x004627f0.

   A stop-thunk table alone would turn the AV into a named stop at 0x005a5e70.
2. Then re-run `audiotick_stops_arms.py vs_init_audio ddk_init_audio` with `ARMS_ROOT` set. Callees of 0x004627f0 past
   0x005a5f00 have not run yet (0x005a6110, 0x005bbb20, 0x004624c0, 0x005b9e30, 0x005aa060, ...). The next fault or stop
   names the next body.
3. The provisional 0x004627f0 needs the PROV-INSITU replay (`replay_prov_insitu` = not run) before any admission or C4 talk.

## Files
- `re/match/PROVISIONAL.tsv`: +1 row (0x004627f0).
- `re/match/s10/004624c0.c`: port-compile cast.
- `mashedmod/src/mashed_re/Match/`:
  - `PortTop_*.cpp`: 35 new wrappers;
  - `PortTop_AudioStops.cpp`: +4 stops;
  - `PortTop_004669b0.cpp`: 0x004627f0 binding;
  - `PortTop_CrtStdio.cpp`: sprintf.
- `mashedmod/exe_sources.rsp`: +35 TUs.
- `re/tools/dual_copy_allowlist.txt`: +5.
- `verify/porttop_20261108/BINDINGS.tsv`: 21 updated, 42 added.
- `verify/portorigtop_20261113/tools/`:
  - `audiotick_bind_gen.py`, `sync_rsp_allowlist.py`, `plan.json`, `newfiles.txt`;
  - `audiotick_stops_arms.py`: `ARMS_ROOT`.
- `arms_hwinit_run1.out`.

No promotions file: nothing moves to C4 in this row.
