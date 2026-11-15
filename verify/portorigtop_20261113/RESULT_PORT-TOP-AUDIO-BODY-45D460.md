# RESULT PORT-TOP-AUDIO-BODY-45D460: 0x0045d460 bound as a provisional body on the ORIG_TOP audio-init lane

## Verdict: GREEN on the binding legs. The body is not reached yet, so the oracle leg did not run (new row RO-AUDIOINIT-45D460-REACH)

- **Not VOID.** At base 17202f6c3, `Match/PortTop_004669b0.cpp` bound `FUN_0045d460` to a stand-in (`PortTop_AInit_0045d460`,
  log text `rva 0x0045d460: stand-in, not run`). hooks.csv `0045d460` is C2, `new`. So `PORT-TOP-AUDIOSTOP-5AD770` stays
  open for run 50.
- **Self-test: PASS in both arms, both builds.**
  - portstate-image PASS (0 of 8 checks failed).
  - OT_CODEPTR PASS (0 of 367 checks failed, 188 slots: 11 bound to an exe body, 177 to a stop thunk).
  - code-trap PASS (0 of 6 checks failed, 459/459 pages).
- **Arms** (2026-11-15, test-slot after about 10 min of memory wait, then mashed_lock taken by this run as pid 11184 and released):

  | arm | build | exit | where | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AD770** after 6.7 s | named stop, return 0x0023B050 = `_PortTop_Bound_005adc10+0xe0` (`mashedmod/build/mashed_re.map`) | 7144ac6ce5b23c90 |
  | ddk_init_audio | object | **0x005AD770** after 6.6 s | named stop, return 0x00238F70 = `_PortTop_Bound_005adc10+0xe0` (`mashedmod/build/ddkobj/mashed_re_ddk.map`) | d3ef1b56c7cd25cb |

  - This is the same stop as PORT-TOP-AUDIOSTOP-5ADC10, in both builds.
  - The last `[OT_AUDIOINIT]` line before the stop is `0x004627f0 sound engine HW init: provisional exe body`. There is no
    `0x0045d460 PERMDICT.RWS loader` line, so the new body did not run.
  - Why: `re/match/topm01/004669b0.c` calls `FUN_004627f0()` (line 63) before `FUN_0045d460()` (line 66), and the original
    calls 0x0045d460 at `0x004669f6` after that. The stop at 0x005AD770 is inside 0x004627f0's closure
    (0x004627f0 -> ... -> 0x005add60 -> 0x005adc10 -> 0x005ad770, PORT-TOP-AUDIOSTOP-5ADC10 section 2).
- **Oracle not re-run.** The RO arm cannot reach 0x0045d460 before 0x005ad770 has a body, so the first divergence
  `0x00604604` (RESULT_RO-ORACLE-GATE @ 32576d460, ref 4, run 0) cannot move yet. Filed as row
  `RO-AUDIOINIT-45D460-REACH` (`roadmap.ps1 add`, section D4): after PORT-TOP-AUDIOSTOP-5AD770, re-run the init arms,
  bind or stop the next callees inside 0x0045d460, then set `MASHED_OT_AUDIOINIT` on the RO arm and re-run the oracle.
- Gates: not run (top-down row). No menu walk: no menu code changed.

## 1. equiv and residual (evidence in `audio_body_45d460/`)
- Source: `re/match/s10/0045d460.c`, flags `/O2 /GS /Ob1`, 94.2% (regs 97.7%, ops 98.8%), 36 differing unmasked bytes of
  822 (`match_show.txt`, `match.py --bytes --show`). Not byte-identical, so it is not called matched and there is no
  promotion TSV.
- `re/tools/match/equiv.py` @ ca7473532 (orch/integration copy, no diff on this branch), `equiv.txt`:
  `0x0045d460 NOT-PROVEN indirect-call-esp-reset`. The original restores ESP from a register in a function that calls
  through `*(DAT_007d3ff8 + 0x114)`. This is the tool stopping at the indirect call, not a located difference.
- Residual (`match_show.txt` lines 109-134), the voice-count sum at 0x0045d5c4..0x0045d5e8:
  - original: `mov ecx,[0x68fc88]; xor edx,edx; cmp ecx,ebx; mov [0x68fc84],edx; jbe`, a loop over
    `[0x604574 + 0x94*i]` that adds when above 0, then `mov [0x68fc84],edx` after the loop.
  - source: stores ebx (0) to DAT_0068fc84 before the loop and the running sum (edx) inside the loop.
  - Both write 0 first and the same sum last. The loop has no call, so no other code reads DAT_0068fc84 between those
    stores. **[UNCERTAIN]: from reading the listing, not proved by a tool.** Its test is row `RO-AUDIOINIT-45D460-REACH`:
    once the body runs, the log line `0x0045d460 returned: ... DAT_0068fc84=...` must give 139 (0x8b), the value the
    original holds in RESULT_RO-ORACLE-GATE section (a).
- Not admitted. Listed **provisional** in `re/match/PROVISIONAL.tsv` (L-126), knob path only, never C4. Merge bar: not
  worse than 94.2% plus the behaviour checks above.
- No shaping attempt in this row. The s10 note in the source header lists the forms tried before (permuter 300 s included).

## 2. Binding
- `verify/portorigtop_20261113/tools/audiotick_bind_gen.py`:
  - `PROV_BIND |= {'0045d460'}`.
  - `ALIAS["004a2cbd"] = ("PortTop_Crt_004a2cbd", "Match/PortTop_00493710.cpp")`. 0x0045d460 calls printf 0x004a2cbd
    (hooks.csv: CRT, waived L-67). Without the alias the generator made it a named stop, and the body would stop at its
    first `numpermwaves` print. The exe body already exists (it writes the line to `mashed_re.log`).
- Roots: the table row's roots plus `0045d460` (`D:/MashedWT/PORT-TOP-AUDIO-BODY-45D460_data/roots.txt`).
- Plan: 252 bind (222 match, 15 admitted, 8 provisional, 7 replay), 64 reuse, 41 stops. Previous row: 245 / 59 / 39.
  - Binds (new): 0x0045d460 (provisional), and C4-match callees 0x005a6280, 0x005a6710, 0x005a7aa0, 0x005a7af0,
    0x005b8570, 0x005adf60. New TUs `Match/PortTop_<rva>.cpp` for these seven.
  - Reuse (new): 0x004cc160 / 0x004cc230 / 0x004cc5e0 (`RwCore_<rva>`), 0x004a2cbd (`PortTop_Crt_004a2cbd`),
    0x005aded0 (`AudioListNodeCount`). 0x005a6dc0 was already bound.
  - New named stops (`PortTop_AudioStops.cpp`): **0x005a7b60** (in the 0x809 chunk loop; PROVISIONAL.tsv lists it at 60.9%
    with a branch-condition difference, not added to PROV_BIND) and **0x005a71f0** (47.9%, no listing).
  - The new wrappers came out with the table row's header text. Fixed by hand to name the 0x004669f6 call.
- `Match/PortTop_004669b0.cpp`: `PortTop_AInit_0045d460` now logs, calls `PortTop_Bound_0045d460` and logs DAT_0068fc88,
  DAT_0068fc84, DAT_0088e674, DAT_0069047c and DAT_00604604 after it returns.
- The generator rewrote other wrappers with LF line endings. `git diff` shows no content change in them, not committed.
- `newfiles.txt` +7, `sync_rsp_allowlist.py` (ARMS_STEP=this row): rsp +7, allowlist +1 (CROSS-TARGET 0x005a7af0,
  `Util/PromoLoop_sessionB.cpp`, text edited to name this closure). `bindings_update.py`: BINDINGS.tsv updated 39, added 9.

## 3. Builds and checks
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint 314 known, NEW 0.
- Object build (`MASHED_DDKOBJ=1`): Build OK. rva-lint NEW 0.
- `ddkobj.py check` on this branch: IDENTICAL 1555/1598, DIFFERENT 28, not-in-map 15, units linked 1541/1751,
  refusals 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4).
  - Against the previous row's branch check (PORT-TOP-AUDIOSTOP-5ADC10, merged into this base): IDENTICAL 1549/1592,
    DIFFERENT 28. **The DIFFERENT set is the same 28 rvas** (sets compared), so no unit is newly DIFFERENT.
    No separate fresh check of 17202f6c3 was run: with an equal set there is nothing to classify.
  - IDENTICAL +6: the six new C4 TUs (BYTECHECK.tsv rows IDENTICAL for 0x005a6280, 0x005a6710, 0x005a7aa0, 0x005a7af0,
    0x005adf60, 0x005b8570).
  - 0x0045d460 is provisional, so it keeps its VS2022 body in the object build.

## 4. Arms command
`ARMS_STEP=PORT-TOP-AUDIO-BODY-45D460 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-BODY-45D460_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-BODY-45D460 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
- Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOSTOP-5ADC10_root`, its old `mashed_re.log` removed.
- The runs stayed local: their paths are outside the repo.

## 5. Next
- `PORT-TOP-AUDIOSTOP-5AD770` (open, run 50): needed before 0x0045d460 can run.
- `RO-AUDIOINIT-45D460-REACH` (new): re-run the init arms after that row, then 0x005a7b60 / 0x005a71f0, the
  DAT_0068fc84 = 139 check, and the oracle with `MASHED_OT_AUDIOINIT` on the RO arm.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-BODY-45D460_data/`, sha256)
- `vs_init_audio.log` 7144ac6ce5b23c9094fca88a74a6f3c18cd8446eef0cb7182d54f01128cb5f07
- `ddk_init_audio.log` d3ef1b56c7cd25cb420a1997e2563693a6e39fa26dcfe1d498c05d1e0f2cd2a1
- `arms1.out` 4a900594872595fc2b98ed7ad53e630296b27b1429269ad70e6ec348fc8d3bbd
- `build_vs.log` 4053f47ba406f6d17bc5514157948ec9c86879c1f4065feb33528b50b31ff760
- `build_ddk.log` 8368b9c043c58e14e82f6ee750b598d04171caf8eece6184abcf9ee96e4d6d9e
- `ddkcheck.log` adfaaf3be197e1d3f5b95ef6bceb4801008a88358a1f29cebd1d390be67a0d05
- `BYTECHECK.tsv` c69064e2e4fe49e2548abc817d1db49dfc2d3bcc223b90ab5984ba13842b7794
- `plan_dry.txt` fe2a0fe0b51fa810852d74c418b1dad933aaa61b915dcd792f1b7577e7b79d5f
- `plan_write.txt` 2583d319c8e5e237895d286a845d2a83c90a7d950a715e7e6c2cd1d8a467c1c2
- `roots.txt` 73566306c2eb5600447befbb32c4667ff8465d6d18e268986f50ac1fe77142bb
