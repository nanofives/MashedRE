# RESULT AUDIO-STACK-STEP1-RECHECK (run 47, top-down row, L-158 recheck on the merged integration head)

## Verdict: GREEN

- Branch `orch/AUDIO-STACK-STEP1`. Merge commit `38a8a0dd2` = `994fe6644` (STEP1) + orch/integration `64dfde3cb`.
  `64dfde3cb` is the head after the run-47 drain: none of DDKOBJ-DIFFERENT-REASONS, L156-ECX-RUNTIME-TARGETS or
  MATCH-RWT-PORT-FOLLOWUPS is still queued, and `mashed_lock.py status` showed `free`.
- Both builds are OK on the tip. `rva-lint` reports 313 known and NEW 0 on each.
- Both audio init arms stop at **0x005ADC10**, called from `_PortTop_Bound_005add60`:
  - VS2022: `+0xd`.
  - Object build: `+0xc`.
  - These are the same offsets as STEP1 (2e89f3602) and AUDIOPOOL-5AE650.
- All six IMG-5 image-ON arms reach `Calling AppDestroy` and log `WinMain 0x00492370 returned 0`, on both builds.
- Every self-test logs PASS. The logs contain 0 `self-test FAIL` lines.
- `ddkobj.py check`:
  - Tip: IDENTICAL 1545/1590, DIFFERENT 30.
  - Base (`64dfde3cb`, separate checkout): IDENTICAL 1330/1357, DIFFERENT 12.
  - 18 units are DIFFERENT on the tip and not on the base. 17 are admitted and 1 is provisional. None falls in neither
    table (section 3).
- G-CTRL, B and the 12x4 captures were not run: this is a top-down row (WORKER_RULES "Row kinds"). The merge train runs
  the default gates.

## 1. Merge

- **Where it ran.** `orch/AUDIO-STACK-STEP1` was already checked out, clean at `994fe6644`, in
  `.worktrees/AUDIO-STACK-STEP1`. The merge ran there, per the brief.
- **Worktree cleanup.** The empty worktree `.worktrees/AUDIO-STACK-STEP1-RECHECK` (`994fe6644`, no work) was removed
  with `py -3.12 scripts/diag.py wt-remove`.
- **The merge.** `git -c merge.autoStash=false merge --no-ff 64dfde3cb`.

### Conflicts (2), hand-merged

| file | kind | resolution |
|---|---|---|
| `re/match/ADMITTED.tsv` | STEP1's 12 s30 rows against an empty integration side, at the tail | union (STEP1's 12 rows kept). No duplicate rva. 0x00554200 (admitted on both sides) appears once |
| `re/match/PROVISIONAL.tsv` | STEP1's 7 rows (0x005a5e70, 0x005ae650, 0x0045cd30, 0x004647f0, 0x005a6d60, 0x005aab70, 0x005acda0) against integration's 9 rows (DDKOBJ-UNLISTED-10), at the tail | union, 16 rows. No duplicate rva |

- **The 92 dropped PROVISIONAL rows.** Against our side, PROVISIONAL.tsv loses 92 rows.
  - All 92 are in the merge-base `d1e5ac586` and absent from `64dfde3cb`.
  - Integration deleted them on purpose: `ce2f65b40` and `ea81a4bd6` (MATCH-RWT-PORT-FOLLOWUPS: byte-identical, relocheck OK,
    C4 by match).
  - The merge keeps those deletions. Against `64dfde3cb`, the merged table only adds rows (+10).
- **Auto-merged files.** `hooks.csv`, `re/analysis/CHANGELOG.md` and `verify/porttop_20261108/BINDINGS.tsv` merged
  without conflict.
- **Hand checks after the merge:**
  - No duplicate non-comment rva in `hooks.csv`, `BINDINGS.tsv`, `ADMITTED.tsv` or `PROVISIONAL.tsv`.
  - No duplicate line in `mashedmod/exe_sources.rsp` or `re/tools/dual_copy_allowlist.txt`.
  - No conflict markers.
- **Why `PortTopCodePtrImage.inc` was not regenerated.** Between `d1e5ac586` and `64dfde3cb`, integration changed only
  `PortStateImage.cpp`, `PortTopCodePtr.cpp` and `BINDINGS.tsv` among the build inputs: no `CODEPTR_IMAGE.tsv`, no rsp
  and no generated `.inc`.

## 2. Builds and boots (tip `38a8a0dd2`)

| build | result | rva-lint | exe sha256 (16) |
|---|---|---|---|
| VS2022 | Build OK | 313 known, NEW 0 | `aea338ab20cccdb2` |
| object (`MASHED_DDKOBJ=1`) | Build OK | 313 known, NEW 0 | `ebe6ba51fe9695e7` (`mashed_re_ddk.exe`) |

Both builds ran with `MASHED_NO_DEPLOY=1`, so `original\mashed_re_dev.asi` was not deployed.

### How the boots ran (`D:/MashedWT/AUDIO-STACK-STEP1-RECHECK_data/boots.sh`, the 2e89f3602 recipe)

- `test-slot` outside, with no hub override. The run waited 10 min for memory, then took slot 3/8 under the cap. It ran
  locally.
- `mashed_lock.py run --label AUDIO-STACK-STEP1-RECHECK` inside.
- Private root `D:/MashedWT/AUDIO-STACK-STEP1-RECHECK_root`, a robocopy of STEP1's root. `original/mashed_image.bin`
  sha256 is `81cc7775...9afcd8`, the same file as STEP1, IMG-5 and AUDIOPOOL.
- After the run, `original/mashed_image.bin` is absent from the real install, so the real original is untouched.

| arm | exe | rc | end |
|---|---|---|---|
| def_off | VS2022 | 0 (48.7 s) | knobs off, `B17-SUMMARY reached-main-loop` |
| def_on_img1_cp_trap1 | VS2022 | 0 (27.7 s) | AppDestroy, `WinMain 0x00492370 returned 0 after 3000 frames` |
| ddk_on_img1_cp_trap1 | object | 0 (34.7 s) | same |
| def_on_img1_cp_trap2 | VS2022 | 0 (29.4 s) | same |
| ddk_on_img1_cp_trap2 | object | 0 (29.0 s) | same |
| def_aud_img1_cp_trap1 | VS2022 | 0 (8.0 s) | AppDestroy, `WinMain ... returned 0 after 600 frames` |
| ddk_aud_img1_cp_trap1 | object | 0 (10.5 s) | same |
| vs_init_audio | VS2022 | 0x005ADC10 (6.1 s) | `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005ADC10 (DAT_006904e8 = 0, DAT_006904ec = 0) ... Return address 0023A70D` |
| ddk_init_audio | object | 0x005ADC10 (11.0 s) | same line, `Return address 0004B09C` |

**Symbolizing the stop return addresses.**
- The logged return address is a VA. Both exes have preferred load address 0x00010000 in their `.map`.
- The nearest preceding map symbol for each:
  - `0x0023A70D` = `_PortTop_Bound_005add60+0xd` (`default/mashed_re.map`).
  - `0x0004B09C` = `_PortTop_Bound_005add60+0xc` (`ddkobj/mashed_re_ddk.map`).
- STEP1's VS2022 address `0x0023A71D` gives the same symbol and offset, `+0xd`. The function moved by 0x10 bytes in the
  merged image.

### Knobs each arm sets, against the new defaults

- `aba00694a` (IMG-5-ORIGTOP-DEFAULT, L-159 (a)) is in the tip. Since then `MASHED_PORTSTATE_IMAGE` and
  `MASHED_OT_CODEPTR` default ON under `MASHED_ORIG_TOP`, and `=0` opts out (`Compat/PortStateImage.cpp`,
  `Compat/PortTopCodePtr.cpp`).
- The arms still set both to `=1`, which is the same value as the default. So these arms measure the default image-ON path.

| arm | knobs set | needed explicitly on this tip |
|---|---|---|
| vs/ddk_init_audio | `WIN_POS=primary-bl DET_FRAMES=600 ORIG_TOP=1 OT_TIMER=1 PORTSTATE_IMAGE=1 CODE_TRAP=1 OT_CODEPTR=1 OT_AUDIOINIT=1 OT_AUDIOTICK=1` | `ORIG_TOP`, `OT_TIMER`, `OT_AUDIOINIT`, `OT_AUDIOTICK`, `CODE_TRAP` (+ harness `WIN_POS`, `DET_FRAMES`). `PORTSTATE_IMAGE=1` and `OT_CODEPTR=1` are redundant with the default |
| *_on_img1_cp_trap1/2 | `DET_FRAMES=3000 ORIG_TOP=1 PORTSTATE_IMAGE=1 OT_CODEPTR=1 CODE_TRAP=1/2` | `ORIG_TOP`, `CODE_TRAP`. Image and codeptr are redundant |
| *_aud_img1_cp_trap1 | `WIN_POS DET_FRAMES=600 ORIG_TOP=1 OT_TIMER=1 OT_AUDIOTICK=1 PORTSTATE_IMAGE=1 OT_CODEPTR=1 CODE_TRAP=1` | `ORIG_TOP`, `OT_TIMER`, `OT_AUDIOTICK`, `CODE_TRAP`. Image and codeptr are redundant |
| def_off | `DET_FRAMES=3000` | knobs-off control |

**Self-tests.** Every knob arm logs PASS. The count of `self-test FAIL` lines is 0 in every log.

| self-test | PASS line | arms |
|---|---|---|
| `[portstate-image]` | PASS (0 of 8 checks failed) | all 8 image arms |
| `[OT_CODEPTR]` | applied 188 slots (11 bound, 177 stop thunk), PASS (0 of 367 checks failed) | all 8 image arms |
| `[code-trap]` | PASS (0 of 6 checks failed) | all 8 image arms |
| `[OT_TIMER]` | PASS (0 of 8413 checks failed) | the 2 AUDIOTICK arms |

[UNCERTAIN] The init arms print no `[OT_TIMER] self-test` line. They stop at 0x005ADC10 before that line is written.
STEP1 and AUDIOPOOL had the same property.

Each image-ON log has 1864 lines (the aud arms have 1870), the same line count as STEP1's run. I did not diff the logs
line by line against STEP1's.

## 3. `py -3.12 re/tools/ddkobj/ddkobj.py check`: tip against base

| tree | IDENTICAL | DIFFERENT | not-in-map | units linked | refusals |
|---|---|---|---|---|---|
| base `64dfde3cb` (separate detached checkout `.worktrees/AUDIO-STACK-STEP1-RECHECK-base`, both builds OK) | 1330/1357 | 12 | 15 | 1304/1502 | 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4) |
| tip `38a8a0dd2` | 1545/1590 | 30 | 15 | 1533/1741 | 9 (same) |

The base's 12 DIFFERENT units are all DIFFERENT on the tip as well.

**DIFFERENT on the tip and not on the base: 18 units.** None of them is in the base BYTECHECK, because these units are
not linked there. The table rows are line numbers in the tip's tables, and the diff bytes are the tip `BYTECHECK.tsv`
`diff_bytes`.

| rva | diff bytes | class | table row (tip) | verdict |
|---|---|---|---|---|
| 0x0045c480 | 77 | admitted | ADMITTED.tsv:12 (`re/match/s10/0045c480.c`) | EQUIV |
| 0x0045cd30 | 319 | **provisional** | PROVISIONAL.tsv:918 (`re/match/s10/0045cd30.c`) | NOT-PROVEN |
| 0x005a6b30 | 58 | admitted | ADMITTED.tsv:45 | EQUIV |
| 0x005a6e50 | 20 | admitted | ADMITTED.tsv:46 | EQUIV |
| 0x005a7810 | 8 | admitted | ADMITTED.tsv:47 | EQUIV |
| 0x005a78b0 | 14 | admitted | ADMITTED.tsv:48 | EQUIV |
| 0x005a8890 | 40 | admitted | ADMITTED.tsv:25 (`re/match/s25/005a8890.c`) | EQUIV |
| 0x005a9970 | 21 | admitted | ADMITTED.tsv:49 | EQUIV |
| 0x005aa480 | 7 | admitted | ADMITTED.tsv:50 | EQUIV |
| 0x005aab00 | 63 | admitted | ADMITTED.tsv:28 (`re/match/s25/005aab00.c`) | EQUIV |
| 0x005ad030 | 68 | admitted | ADMITTED.tsv:51 | EQUIV |
| 0x005ad320 | 219 | admitted | ADMITTED.tsv:31 (`re/match/s26/005ad320.c`) | EQUIV |
| 0x005adb70 | 109 | admitted | ADMITTED.tsv:52 | EQUIV |
| 0x005ae210 | 43 | admitted | ADMITTED.tsv:53 | EQUIV |
| 0x005ae5b0 | 15 | admitted | ADMITTED.tsv:54 | EQUIV |
| 0x005ae610 | 61 | admitted | ADMITTED.tsv:55 | EQUIV |
| 0x005ae920 | 3 | admitted | ADMITTED.tsv:33 (`re/match/s26/005ae920.c`) | EQUIV |
| 0x005aea60 | 17 | admitted | ADMITTED.tsv:56 | EQUIV |

The s30 rows without a path above use `re/match/s30/<rva>.c`.

- **Neither class: 0 units.**
- **Not listed here, per the brief:** 0x00493900, 0x00493710, 0x00498810, 0x00401690 and 0x004929d0 are DIFFERENT on
  both trees, so they are not new. 0x0045a3a0 is not DIFFERENT on either tree. None of the six was relabeled or admitted.
- L-140, L-161 and L-162 were not acted on.

## 4. Next

- Lead: enqueue `orch/AUDIO-STACK-STEP1` at the tip sha in the REPORT. The train's default gates must be unchanged.
- The next audio stop is still 0x005ADC10, reached from 0x005add60. It is already filed as `PORT-TOP-AUDIOSTOP-5ADC10`.
- The base checkout `.worktrees/AUDIO-STACK-STEP1-RECHECK-base` (detached at 64dfde3cb) can be removed with
  `py -3.12 scripts/diag.py wt-remove`. It holds no work.

## Raw data (`D:/MashedWT/AUDIO-STACK-STEP1-RECHECK_data/`, sha256)
```
50943a402efffc8c9e3162df58cdb1f04f17a6dc87c5d514e942929970d311ff  boots.sh
d3d7c2f6a9da2c604e9861d496e8f2c88ace2362dc8fd01be9c7104738603f75  builds.sh
a77ea17e3cee7613327c50418d9d82c4d71460498e13bc2c2ee08d68e4cdb78c  symbolize.py
86a3af630ec0022d2262fd9032cf4445b11d1f289d55ee7a245f315bf17dd889  boots.out
1c92c8993c2daa5604e998e971c40e82e269ebe974dc118e3834cfaf36143ce0  builds_tip.out
12629f0768e86e5cc7d415222619759ab7ed03bd6ddf128153be505c11d921a6  builds_base.out
294475dc0d3a252a3e6d6113c53d9e38452fbb981005b6c05404ac00fdba3b35  build_vs.log
4c8f0d2b4cd47d35906804f69c757d4981f5ce6a404011bee1df157ff9dcdbc4  build_ddk.log
f48e5119b7348f9c7b56705ce955e77d4cf8ca1da34830b5228482e2ab82277e  base_build_vs.log
83a3166f67e03acf8dfd1cd94c5864516d311a0279f6d2dc4ed690fc12e43dba  base_build_ddk.log
367628a2904e7e8caaca999c66854b64a6782c927ffaeb8efbbbb2bc60cb541a  ddkcheck.log
4936c7f0aa81fb1dbe1be4b7f4a4b5f68454df337a4b6878d2077b4595c38d2a  base_ddkcheck.log
50e8a9dfcd6fc0496919896d628a345414a8f55c0eca143f3ca85e86ea221f43  BYTECHECK.tsv
3530f6dbba19f7e823c5b8dacbb44fa7b22f7ee4332727e24030282f9d5b354e  base_BYTECHECK.tsv
445ed66095e9fd915716bc6f0f63fa0dca7a70226d49348ee06cca987764b8ca  boots/def_off.log
710b4cdb113801b49c3a136380007b4fe66543e9b90cfdac146c8fd6cbabf38f  boots/def_on_img1_cp_trap1.log
db82e993975a04ad65b66ff334af87a25b115d50ac3c5a62e8bbc4974adb1ed1  boots/ddk_on_img1_cp_trap1.log
e90c0fa492e85b2e89501e97c2577f351c583a37588a0f557298433b834593a4  boots/def_on_img1_cp_trap2.log
d90bc0edcc41eecd3abaab492f46cb2105663abf49c6ee438c68d51fa3e699c4  boots/ddk_on_img1_cp_trap2.log
18925423619863273a15c5e8fa6471fd71043c9664afa40d4274a7c5416af44d  boots/def_aud_img1_cp_trap1.log
eb6f482c32b5217982e37a5f2302e8a04afcaa502d0261535890954d30e92c93  boots/ddk_aud_img1_cp_trap1.log
4801bfafc2cb9c0a8a0766ef78576963c37df6a05a3b67e8338664584186e855  vs_init_audio.log
9551e17496b21d3be8ac9ece8fe4849079e79a2597ad392be831d1aac72e2e34  ddk_init_audio.log
aea338ab20cccdb2fa0c403e6211eb95aba3a559a26bd6aed0d99a38880bd970  default/mashed_re.exe
ebe6ba51fe9695e7e5b349feb4f5e162a206582468bf288d1a9733a88d38a74f  ddkobj/mashed_re.exe
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  ../AUDIO-STACK-STEP1-RECHECK_root/original/mashed_image.bin
```
`symbolize.py` takes an RVA. For the stop lines it was given the logged VA minus 0x00010000.
