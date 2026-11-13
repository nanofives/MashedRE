# RESULT PORT-TOP-AUDIOTICK-STOPS (2026-11-13, run 42, section D4)

Verdict: HANDOFF. Top-down row (knob path only).

Every callee in the 0x00466b50 / 0x0045d3f0 / 0x0045d430 closure that has a matched (`c4=match`) or L-126 admitted body
now runs that body in the exe. That includes 0x00550be0, 0x005a60b0, 0x005a60e0 and the 52 named stops of 0x00466b50.
`MASHED_OT_AUDIOINIT=1` + `MASHED_OT_AUDIOTICK=1` still cannot boot together. The run now stops one level deeper, at
**0x005a6ea0**, after about 2 s on frame 0. Before this row it stopped at 0x00550be0.

0x005a6ea0 has no matched or admitted body (C2, see "Why 0x005a6ea0 stays a stop"). Its sibling 0x005a6f30, called next by
0x005a60b0, has none either. Both are RenderWare-audio library code (0x005a....), the next work for this path.

**Self-test results:**
- Control arms (gate closed, AUDIOTICK only): **`[OT_TIMER] self-test PASS (0 of 8413 checks failed)`** on both builds,
  600 frames, audio branch 600.
- Knob-on arms (AUDIOINIT + AUDIOTICK): the self-test summary prints PASS (0 of 0), because the stop fires inside the first
  0x004926c0 call, before any check. These arms end with exit code 0x005A6EA0 (the named stop).

Nothing on the default path changed:
- Every new TU is exe-only under `MASHED_STANDALONE`, marked `// RVA-LINT: ORIGTOP-ONLY`.
- Every new TU is reached only under `MASHED_ORIG_TOP=1` + `MASHED_OT_AUDIOTICK=1`.

No C4 claim: the bodies were already C4 by route match, or are admitted (never C4, L-126 / L-128).

## Closure (generator `tools/audiotick_bind_gen.py`, plan `tools/plan.json`)
BFS from the three audio-tick bodies through each bound source's `FUN_` callees:

| binding | count | what |
|---|---|---|
| new exe body `PortTop_Bound_<rva>` | 117 matched + 3 admitted | `Match/PortTop_<rva>.cpp` includes the source (hooks.csv C4 `c4=match` with match.py 100% / BYTE-IDENTICAL and relocheck OK, or a row of `re/match/ADMITTED.tsv`). The three existing wrappers (0x00466b50 / 0x0045d3f0 / 0x0045d430) are regenerated the same way. |
| existing exe body | 34 | a `Match/PortTop_*` body, a match-include body whose source is a leaf, a C4 hand leaf, or a `MashedCrt_*` helper (`__ftol2` 0x004a2c48 -> `MashedCrt_ftol2_004a2c48`) |
| named stop | 30 | `Match/PortTop_AudioStops.cpp`. Each logs and exits with code = rva (`PortTop_AudioCalleeStop`, caller 0 = any). |

Admitted bodies used (L-126, `re/match/ADMITTED.tsv`, EQUIV):
- 0x0045c480
- 0x005a8890
- 0x005aab00

Remaining named stops (no matched or admitted body), all 30:
- 0x004075a0, 0x0042b900, 0x0042b930, 0x0045caf0, 0x0045cbe0, 0x0045cd30
- 0x0045d7a0, 0x0045dd60, 0x0045e990, 0x0045efe0, 0x00461e90, 0x00463640
- 0x004647f0, 0x00464ca0, 0x0046c7b0, 0x0046cbb0, 0x00493f70, 0x004a34b0
- 0x004c3ac0, 0x005459c0, 0x0057c210, 0x005a6d60, **0x005a6ea0**, **0x005a6f30**
- 0x005a75b0, 0x005a9f60, 0x005aa560, 0x005aab70, 0x005aac20, 0x005b0cf0

Callees of a stop were not explored. For example, 0x005a6ea0 calls 0x005ab040 / 0x005ab070, so the closure below the
stops is still open.

Special cases:
- **0x0045cbe0 is a forced stop.** `re/match/s10/0045cbe0.c` is `c4=match`, but it carries a second copy of 0x0045c4e0, and
  `re/match/s10/0045cb20.c` carries the other. Two exe copies of one rva, both in ORIGTOP-ONLY-marked TUs, are not a class the
  lint accepts (`scripts/lint_rva_bodies.py` `is_origtop`: exactly one marked TU). The other way out would be an untagged D4
  burn-down line, which this row does not add.
- **0x0045c640's exe body is the copy inside `re/match/s10/0045c6b0.c`.** It differs from `re/match/s10/0045c640.c` by casts
  only (`(int *)` / `(Ent28 *)` on four stores, `.f0` vs `.f[0]`). So there is one exe body per rva.
- File-local helper copies inside a matched source bind as `PortTop_Local_<owner>_<rva>`:
  - 0x0045c4e0, 0x0045c510 and 0x0045c550 inside 0045cb20.c.
  - 0x0045caf0 inside 0045cb20.c, which is also a named stop for its other callers.

## Source edits to `re/match` (C++ compile only)
Every edit below keeps the DDK object byte-identical. Re-checked after the edit with `match.py --bytes` and `relocheck.py`:

| source | edit | match.py --bytes | relocheck |
|---|---|---|---|
| s10/00462dd0.c | 3 casts (`(int *)&DAT_007f1a1c`, `(int)(&DAT_00690158 + ...)`, `(undefined4 *)&DAT_0068fc98`) | BYTE-IDENTICAL 240 B | OK checked=24 |
| s10/00462ec0.c | `(int *)*puVar2` | BYTE-IDENTICAL 132 B | OK checked=11 |
| s10/0045cc50.c | prototype of FUN_0045cbe0 takes the 3 args the call passes | BYTE-IDENTICAL 47 B | OK checked=3 |
| pilot/a1_004645c0.c | extern globals under `#ifndef MATCH_PORT` (the wrapper reads them at their addresses) | BYTE-IDENTICAL 171 B | OK checked=11 |
| pilot/a4_0045d250.c | same | BYTE-IDENTICAL 220 B | OK checked=17 |
| s02/00411530.c | `(float *)` / `(int *)` on three args | BYTE-IDENTICAL 65 B | OK checked=1 |
| s02/004115c0.c | `(undefined4 *)` on two args | BYTE-IDENTICAL 58 B | OK checked=2 |

The src sha256 recorded in each row's `hooks.csv` C4 note is now stale for these seven. The byte-match verdict is unchanged.
hooks.csv is not edited on a branch (HB-TRACKER-AT-MERGE).

0x00407600 binds with `MASHED_RE_PORT` (same as `Match/MatchS01.cpp`) and reads 0x00639d80 at its address. Two other
changes:
- Sources that call `FID_conflict__wprintf` bind it to `PortTop_Printf04G` (printf to log, `Match/PortTop_0040bbb0.cpp`).
- `[OT_TIMER]` self-test (`Match/PortTop_00492370.cpp`): the direct calls to 0x0045d3f0 / 0x0045d430 now run only while
  DAT_006904e8 == 0. With the gate open they would call 0x00550be0 / 0x005a60b0 / 0x005a60e0 once more than the original
  does. The `DAT_006904ec unchanged` check holds in both cases: 0x0045d3f0 sets 0 -> 1 and 0x0045d430 sets it back in the same
  call (re/match/s10/0045d3f0.c, 0045d430.c).

## Why 0x005a6ea0 stays a stop
- `re/match/s25/005a6ea0.c` scores 38.9% (28/36 ins, `/O2 /Ob1 /G5`).
- `equiv.py` verdict: `NOT-PROVEN call-args`.
- In the original, the float-to-int is inline: `fnstcw` / `or ah,0xc` / `fldcw` / `fistp` / `fldcw` at 0x005a6ea0+. The
  divide is `fild qword` / `fidiv dword`. The DDK build of the source emits `fild` / `fild` / `fdivp` and a call to the
  `_ftol` helper.
- Sibling 0x005a6f30: 65.4%, `equiv.py` `NOT-PROVEN esp-unknown` (aligned frame).

Neither is admitted. L-126 allows a NOT-PROVEN source on the knob path only as a `PROVISIONAL.tsv` body. This row's brief was
matched or admitted bodies, so no provisional row was added.

## Builds (commit below)
- **VS2022** (`MASHED_NO_DEPLOY=1`): Build OK.
  - rva-lint OK, 238 known, NEW 0.
  - 45 new `ORIGTOP-ONLY` allowlist lines (CROSS-TARGET / DUP-IN-TARGET pairs of an exe-only matched copy beside the asi
    or hand body), from the lint's own `--seed`.
  - Log `D:/MashedWT/PORT-TOP-AUDIOTICK-STOPS_build_vs.log` (sha256 1c27c6d558cb741c...).
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. Log `D:/MashedWT/PORT-TOP-AUDIOTICK-STOPS_build_ddk.log`
  (567f2b4474371e4a...).
- **`ddkobj.py check`:** IDENTICAL 1262/1295, DIFFERENT 14, not-in-map 19, units linked 1243/1510.
  - 4 refusals (`wrapper-redefines-globals`), none of them this row's.
  - Log `D:/MashedWT/PORT-TOP-AUDIOTICK-STOPS_ddkcheck.log` (9b32d1fb3ed39247...).
  - This row's bodies: **117 of 117 matched IDENTICAL**.
  - The 3 admitted bodies are DIFFERENT, as expected for non-100% sources: 0x0045c480, 0x005a8890, 0x005aab00.
  - The other 11 DIFFERENT entries are not this row's: 0x00401690 0x0042f530 0x004332a0 0x0045a3a0 0x00475a00 0x004929d0
    0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00.

## Knob boots
- Wrapping: `test-slot` outside (`PROYECTOS_TEST_HUB=off`), `mashed_lock.py run --label PORT-TOP-AUDIOTICK-STOPS` inside.
- Script `verify/portorigtop_20261113/audiotick_stops_arms.py`.
- Root `D:/MashedWT/PORT-TOP-AUDIOTICK-STOPS_root` (robocopy of `original\`).
- All arms: `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`.
- Outputs `arms_run1.out` (all four arms) and `arms_run2.out` (controls re-run) in this folder.

| arm | exe | extra knobs | result | log sha256 (16) |
|---|---|---|---|---|
| vs_audio | VS2022 | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 (run 2) | 307631615ebd97fd |
| ddk_audio | object build | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 (run 2) | 7dcbf2a63f7c24a0 |
| vs_init_audio | VS2022 | AUDIOINIT + AUDIOTICK | exit 0x005A6EA0 after 2.7 s: `STOP ... called original 0x005A6EA0 (DAT_006904e8 = 1, DAT_006904ec = 0)` | 8b0532a0bb873590 |
| ddk_init_audio | object build | AUDIOINIT + AUDIOTICK | exit 0x005A6EA0 after 2.0 s, same stop | 11d3618826d67290 |

Run 1 notes:
- vs_audio lost its log copy, because the data folder did not exist yet.
- ddk_audio timed out at 180 s at `HardwareShowIntroVideo`. The machine was over its memory budget: test-slot waited
  10 min, and committed memory was 26-36 GB against a 24 GB budget.
- Both controls passed on run 2.

The menu walk was not run: no menu code changed. No G-CTRL, no B and no captures, because this is a top-down row.

## Other files
- `verify/porttop_20261108/BINDINGS.tsv`:
  - 52 `missing` rows -> `matched` (or kept `missing` with the stop note).
  - 114 new level-3..7 rows for the closure.
  - 0x0045cbe0 carries the forced-stop reason.
- `mashedmod/exe_sources.rsp`: +118 TUs (117 `PortTop_<rva>.cpp` + `PortTop_AudioStops.cpp`).

## Handoff (next rows)
1. **Bodies for 0x005a6ea0 and 0x005a6f30** (RW audio, 0x005a60b0's callees).
   - 0x005a6ea0's float-to-int is inline fldcw/fistp, which the recorded DDK cl does not produce from a C cast. Needs a
     compiler/flag probe on the RW-audio unit, or a decision to run them as provisional bodies (L-126 PROVISIONAL.tsv) on
     the knob path.
   - Then re-run `audiotick_stops_arms.py vs_init_audio ddk_init_audio`. The next stop names the next body.
2. **The 28 other stops** stay until a run reaches them.
   - `tools/audiotick_bind_gen.py` regenerates every wrapper from hooks.csv + ADMITTED.tsv, so a newly matched rva binds by
     re-running it. Then run `tools/sync_rsp_allowlist.py` and `tools/bindings_update.py`.
3. **0x0045cbe0:** needs either a lint class for helper copies shared by two ORIGTOP-only sources, or one shared TU
   holding 0x0045c4e0.
