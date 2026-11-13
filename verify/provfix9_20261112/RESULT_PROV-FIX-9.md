# RESULT PROV-FIX-9: the 9 PROV-CRASH-12 fixes land in the committed provisional sources

Date 2026-11-12. Worker session cmwdtmzvn6xi8sc1b146ton32 (claude3, claude-opus-5-5 medium), lead cmwdtllh06x6osc1bsekhxj96.
Branch `orch/PROV-FIX-9` from `orch/integration` `b34672ee2`. Row kind: top-down (re/match sources and
`re/match/PROVISIONAL.tsv` only, no `mashedmod/` file, nothing the default path executes). Diagnosis and the tested fix
text: `verify/provcrash_20261112/RESULT_PROV-CRASH-12.md`.

## Verdict

**GREEN.** All 9 fixes are in the committed sources. Each committed source, linked by PROV-INSITU's linker, replays
**IDENTICAL-FOR-2724** with its body live. Before this row, all 9 bodies crashed or hung.

| | Count |
|---|---|
| Fixed sources, replay IDENTICAL-FOR-2724 with the committed file | 9 of 9 |
| New `PROVISIONAL.tsv` rows (equiv NOT-PROVEN) | 8 |
| Byte-identical after the fix, C4 by route match (`re/promotions/PROV-FIX-9.tsv`) | 1: `0x00440bc0` |
| Replay-verified C4 (`c4=frida-replay`) | 0. The provisional bodies stay provisional, never C4 (L-137) |

**Self-test: PASS.**
- `re/tools/prov/provlink_selftest.py 1 40`: `selftest ok=40 bad=0 skip=0`.
- `0x00440bc0` committed source linked at its own RVA: 1,063 B, 0 bytes differing from `MASHED.exe.unpatched`
  outside the 69 blob-internal DIR32 sites (`selftest.txt`).

## The 9 fixes

The edits are the `FIXES` entries of `re/tools/prov/prov_fixlink.py` (PROV-CRASH-12), applied to the committed file
by that script's own text transforms. Each source also gets one header comment line naming the fix and its original
citation.
- **Same blobs as the tested fixes:** the code, `ext` and `abs` tables of the linked committed sources are identical
  to PROV-CRASH-12's `bodies_fix.json` / `bodies_fix_5a75b0.json` (checked before and after the comment line).

| rva | source | fix (original citation) | match.py before | after | equiv.py after |
|---|---|---|---|---|---|
| `0x005aa0c0` | s25/005aa0c0.c | sentinel `puVar1 == param_1 + 4` (`0x005aa108` / `0x005aa10b`) | 56.2 | 26.2 | NOT-PROVEN call-regargs |
| `0x005abfa0` | s25/005abfa0.c | `SH` 5 dwords (`0x005ab3cd` / `0x005ab3e4`) | 62.9 | 67.1 | NOT-PROVEN branch-condition |
| `0x005a7b60` | s25/005a7b60.c | `int hdr[5]` (same writer FUN_005ab380) | 53.5 | 60.9 | NOT-PROVEN branch-condition |
| `0x005aba20` | s25/005aba20.c | `param_4[2] = FUN_005b3b60(..)` (`0x005abacb` / `0x005abad0`) | 81.9 | 77.6 | NOT-PROVEN branch-condition |
| `0x005ad0b0` | s26/005ad0b0.c | `*piVar2 = *piVar1` (`0x005ad2b6` / `0x005ad2bf`) | 56.2 | 44.3 | NOT-PROVEN branch-condition |
| `0x005a9f10` | s25/005a9f10.c | FUN_005ae3a0 with 2 arguments, `param_2` (`0x005ae3b7`, tail jump `0x005a9f3a`) | 73.9 | 81.0 | NOT-PROVEN esp-unknown |
| `0x00481a30` | s13/00481a30.c | `FUN_004c1b40(FUN_004671a0(0), &sph)` (`0x00481bb5`) | 63.3 | 69.7 | NOT-PROVEN esp-unknown |
| `0x00440bc0` | tlr/00440bc0.c | `&FUN_x` at the 66 FUN_0047b980 handler arguments | 12.4 | **100.0, BYTE-IDENTICAL** | not run to the end (see below) |
| `0x005a75b0` | s25/005a75b0.c | 3 statement orders (`0x005a7655`, `0x005a777b`, `0x005a77e6`) | 51.6 | 43.5 | NOT-PROVEN x87-region |

**Scores:** four sources lose score (`0x005aa0c0`, `0x005aba20`, `0x005ad0b0`, `0x005a75b0`). The old, higher scores
were measured on wrong code: three crashed and one hung in the replay. These rows can now be re-tuned from a correct
base.

**`0x00440bc0`:**
- `match.py --bytes`: 1,063 B whole symbol, 0 differing unmasked bytes.
- `relocheck.py`: OK, checked 204, unchecked 3 (`_LAB_0047ad30`, `_LAB_004917a0`).
- `c4_reclass.py batch --tsv re/promotions/PROV-FIX-9.tsv` dry run: `[1/1] 0x00440bc0 match PASS byte-identical 1063 B,
  relocheck OK (204/2)`, 1 pass, 0 refused.
- Its equiv.py run was stopped after 45 min. A byte match does not need equiv, so the row goes through the promotion
  TSV and not `PROVISIONAL.tsv`.

`match_rescore.txt` and `equiv9.txt` hold the tool lines.

## Replay per body (committed source)

**Scenario:** the PROV-INSITU replay, as `verify/provcrash_20261112/run_fix.sh` (tick clock, `--q0 0xC0000000`,
`warp_normalize.tsv`, `--load-sleep-ms 0`, `--warp-clock 2000`, `--crash-catch 20`), one body per run. Each body is
linked from the committed file (`bodies_committed.json`, `provlink.py --rvas`). Scripts: `run_arm.sh`, `batch.sh`.

**Private root:** `D:/MashedWT/PROV-FIX-9_root`, a robocopy of `PROV-CRASH-12_root`. Binaries:
- `MASHED.exe` `b9977dab...`
- `mashed_re_dev.asi` `906f8601...`
- `d3d9.dll` `47901951...`

**References:**
- 6 no-body runs: `ref1`..`ref6`.
- Boot classes (prov_compare.py BOOT-CLASS, `0x006147d8`): 1 for ref1, ref2, ref3, ref6. 0 for ref4, ref5.
- Same-class controls: ref1 against ref2 and ref4 against ref5 are both IDENTICAL-FOR-2724 with 0 allowed blocks.

**Scoring:** each body run is scored against the two references of its own class. In the first pass all body runs
were scored against ref1 / ref2. The 5 class-0 runs carried the note "BOOT-CLASS mismatch: verdict not body evidence".
They were re-scored against ref4 / ref5, and all 9 rows below have a matching boot class.

| rva | refs | verdict | allowed blocks | body calls (to anchor) | jmp_ok |
|---|---|---|---|---|---|
| `0x005aa0c0` | ref4, ref5 | IDENTICAL-FOR-2724 | 13 | 32 (12) | true |
| `0x005abfa0` | ref1, ref2 | IDENTICAL-FOR-2724 | 18 | 12 (12) | true |
| `0x005a7b60` | ref4, ref5 | IDENTICAL-FOR-2724 | 13 | 1 (1) | true |
| `0x005aba20` | ref1, ref2 | IDENTICAL-FOR-2724 | 18 | 32 (24) | true |
| `0x005ad0b0` | ref4, ref5 | IDENTICAL-FOR-2724 | 13 | 16 (0) | true |
| `0x005a9f10` | ref4, ref5 | IDENTICAL-FOR-2724 | 13 | 16 (0) | true |
| `0x00481a30` | ref1, ref2 | IDENTICAL-FOR-2724 | 18 | 2,719 (0) | true |
| `0x00440bc0` | ref1, ref2 | IDENTICAL-FOR-2724 | 18 | 2 (2) | true |
| `0x005a75b0` | ref4, ref5 | IDENTICAL-FOR-2724 | 13 | 54,321 (105) | true |

**Notes on the table:**
- The body call counts equal PROV-CRASH-12's fix runs, body for body.
- The class-0 compares carry the tool's TIMER-PHASE note: race tick 0 `0x7f1010` differs, load lengths 115 or 118
  against 127. That rule is part of the compare, and the verdict stays IDENTICAL.
- Every run exited rc 0. No piz-collision signature was seen.
- Table: `REPLAY_VERDICTS.tsv`.

## Builds, object build, knob boot

| Check | Result |
|---|---|
| Default build (`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`) | Build OK |
| Object build (`MASHED_DDKOBJ=1`) | Build OK, link rc 0 |
| `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 789/878, DIFFERENT 79, not-in-map 10, units linked 837/847, refusals 9 (wrapper-redefines-globals=9) (`ddkobj_check.txt`, `BYTECHECK.tsv`) |
| knob-ON boot, default build (`knob_boot.py knob_on_vs <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `WinMain 0x00492370 returned 0 after 3000 frames`, 9 `matched body returned` lines |
| knob-ON boot, object build (`KNOB_EXE=...\ddkobj\mashed_re_ddk.exe`) | `knob_on_ddk: exited rc=0 (0x00000000) after 33.9 s`, `WinMain 0x00492370 returned 0 after 3000 frames`, 9 `matched body returned` lines |

**ddkobj check:**
- None of the 9 RVAs is in the object build's unit map, so this row cannot change a BYTECHECK row.
- The DIFFERENT count of 79 equals the count in RO-BIND-TIMER and PROVISIONAL-BODIES, today on `orch/integration`.

**Gates:** G-CTRL / B and the 12 captures were not run. This is a top-down row: the edited files are re/match sources
the default path does not compile.

## Not done here (same family, still open)

- **`0x005ac210`:** the first-fault fix (callback addresses) reaches a 2nd fault, an AV at `0x005aded6` from
  `0x00462b5e`. The source is unchanged here.
- **`0x005b3b80`:** the first-fault fix reaches a 2nd fault (FUN_005b3760 argument arrays, `0x005b3bd7`..`0x005b3bee`).
  The source is unchanged here.
- **`0x00470c70`:** the register argument eax to FUN_00470670 (`0x00471073`) needs a calling form in DDK C. Not
  attempted.
- **`0x004c4eb0`:** the stepping death is on the harness side. It needs an out-of-process catcher run. Not attempted.

## Evidence (this folder)

| File | Contents |
|---|---|
| `match_rescore.txt` | match.py lines for the fixed and the old source, per body |
| `equiv9.txt` | equiv.py verdicts |
| `selftest.txt` | provlink self-test |
| `bodies_committed.json` | the linked committed bodies that ran |
| `REPLAY_VERDICTS.tsv` | the replay table above |
| `calls.txt` | body call counts |
| `ddkobj_check.txt`, `BYTECHECK.tsv` | object build check |
| `run_arm.sh`, `batch.sh` | run scripts |
| `RAW_SHA256.tsv` | SHA-256 of every cited raw file |

**Raw data, not committed** (`D:/MashedWT/PROV-FIX-9_data/`):
- `ref1`..`ref6` and `b_*`: run folders with `ticks.bin` and `prov.json`
- `*.verdict.json` and `*.c0.verdict.json`
- run logs
- `knob_on_vs.log`, `knob_on_ddk.log`
- `build_default.log`, `build_ddkobj.log`

**Runs:**
- 17 game runs: 15 replays and 2 knob boots.
- Each one under `test-slot`, with `mashed_lock.py run --label PROV-FIX-9` inside it.
- Only PIDs this session spawned.
- One stale knob-boot launcher of this session was stopped. It had been given a POSIX exe path and was still waiting
  in the lock queue.
