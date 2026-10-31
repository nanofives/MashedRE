# MATCH-S03: byte match of game slice 0x00412130..0x0041b520 (2026-10-31)

Branch `orch/MATCH-S03` off `orch/MATCH-MERGE`. Worktree `.worktrees/MATCH-S03`. Static legs held Ghidra pool slot
`Mashed_pool0` once (one `decomp_pc.py` batch of the 128 functions, released by the tool). No tracker, ledger-row
evidence, `original/` or master-Ghidra write. **No re-classify was run**: MATCH-RUBRIC is not merged into
`orch/integration` yet. This file carries every field re-classify's `match` route asks for.

## Verdict

**GREEN.** Both swap builds reproduce the merge-train references exactly. G-CTRL is `8284be46/ba407a16/31fe043e`
and B is `1d5bf4f9/1d5bf4f9/1d5bf4f9` (`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`). The 7-swap exe
(sha8 5b2fa647) and the 8-swap exe at the branch tip (sha8 b1b60643) give the same hashes. No hash moved, so there is
no L-n decision.

- **92 of the 128 slice functions are byte-identical** under `re/tools/match/match.py --bytes` (relocations
  masked), plus 2 functions just past the slice end (0x0041b630, 0x0041b660, same object file). 36 are not, 16 of
  them at 76-99%. Per-RVA table: `results.tsv` (rva, best %, byte_identical, source file, source sha256 over
  LF-normalized text, exact command). Reproduce: `py -3.12 verify/matchs03_20261031/collect.py`.
- **8 RVAs now use their matched source as the port body**, in both targets (every one has a body in exe and
  .asi): 0x00417730, 0x0041ae20, 0x0041b520, 0x0041ad60, 0x00414180, 0x00415020, 0x00413bc0, 0x00414120 (table below).
- The rest of the 92 have no exe body to swap, or the exe body is a different program (stub, partial, measurement
  probe). Those can't reach C4 by match without a port decision. The list and the reasons are below.

## Toolchain and flags (all rows)

- Compiler: XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 (`tools/vc7ddk/bin` of the main checkout), `match.py` toolchain `ddk`.
- Flags: `/O2 /GS /Ob1`, as C, x87 (no `/G5`), `/Gy` added by the tool. Header line `// FLAGS: /O2 /GS /Ob1` in every source.
- Original: `original/MASHED.exe.unpatched` (SHA-256 anchor BDCAE093...EFD3C0E).
- Command per row: `py -3.12 re/tools/match/match.py <source> --bytes` (column `command` of `results.tsv`).

## Port body swaps (C4 candidates for re-classify once MATCH-RUBRIC is merged)

How the swap works: the port TU `#include`s the matched `.c` file under an RVA comment, with `MATCH_PORT`
defined. Inside the file, `#ifndef MATCH_PORT` guards only the extern globals and callee prototypes the match side
needs. The port maps those names with `#define` onto the same addresses the old port body used, or the exe's own
equivalents. So the function text compiled into both targets is the matched text. `scripts/rva_body_scan.py` now
binds an include of a `re/match/` source under a run of RVA comments as those RVAs' body, so `exe_file == file` holds
and the one-body lint sees it. `lint_rva_bodies.py`: 16 known CROSS-TARGET, NEW=0.

| rva | matched source | sha256 (LF) | port body file (exe + asi) | glue kept outside the matched text |
|---|---|---|---|---|
| 0x00417730 | re/match/s03/h1_00417730.c | 850bb793f6a53d89e3648d1356cb1bf2cc134ec5d7bdff1acc5eeddbcea910cd | Vehicle/VehicleState.cpp | none |
| 0x0041ae20 | re/match/s03/h1_0041ae20.c | 60b8487510e2dac7971d37c1c0dc24b87e6a33256cffc917f5bf991e51122447 | Race/SubStateCallees.cpp | `Copy41ae20`: 1-line `__fastcall` export forwarding ECX to the file-static body (the .asi install point) |
| 0x0041b520 | re/match/s03/h1_0041ae20.c | (same file) | Race/SubStateCallees.cpp | none |
| 0x0041ad60 | re/match/s03/h1_0041ad60.c | 5200d6925927745510fe3cda73804338cc920fb301fc804a6d78891ae2f52769 | Render/ParticleEmitterCtors.cpp | the existing 4-instruction naked EBX/EAX entry shim, unchanged |
| 0x00414180 | re/match/s03/h2_00414180.c | 49d653259c23af0d4726743cf08504305c9a57132c9588ef5a02e5ab66c1456d | Race/SubStateMachine.cpp | exe-only `[substate]` log line around the call; exe maps the two tables onto the embedded rows and FUN_00472650 onto `Ai::Ai_RandRange`, as before |
| 0x00415020 | re/match/s03/h2_00415020.c | b4a3ee041fd0afebde7ce37579e716d7d4751f2b6e2aa9c247d0aa086d9d5dcc | HUD/ScenarioWriters_sa2s2.cpp | none |
| 0x00413bc0 | re/match/s03/h2_00413bc0.c | 18ba2cbddcbf4b42b8e537bbe0a0bd1a43b898e8696676e21d31d661b5e4c425 | HUD/ScenarioLeaves_sa1.cpp | new 4-instruction naked ESI/ECX entry shim (push esi / push ecx / call / add esp,8); replaces the naked-asm transcription |
| 0x00414120 | re/match/s03/h1_00414120.c | 2fb057144da0726f3f1bd40b2a4315d978f96a95638aa5f843f159bb38728519 | Frontend/SmallLeaves_t1.cpp | none (Ghidra size 69 short, matched over the whole 81 B symbol) |

Each was re-checked at the committed sha: `BYTE-IDENTICAL (relocations masked)` over the whole symbol. The first 7
were built and gated at commit b8c5c882d (exe sha8 5b2fa647). 0x00414120 was added at 520044342 + 755be4a0e
(exe sha8 b1b60643, which carries all 8). The 7 included files are unchanged between the two.

Notes per swap, for the re-classify `notes` column:
- 0x00414180 is on the default gate path (SUBSTATE default ON). Dropped with the old exe arm: its "idx outside
  0..10" guard. The matched body, like the original, has none.
- 0x00413bc0 and 0x0041ad60 were matched in files with a stand-in caller (inside `#ifndef MATCH_PORT`, not
  shipped). Both are also byte-identical with their **real** caller in the same file: 0x00413bc0 in
  `h2_00413cb0.c` (caller 0x00413cb0), 0x0041ad60 in `h1_grp_0041ad60.c` (caller 0x0041b450).
- Callees and globals: identity masks relocations, so each row is evidence for its own body only (rubric (a)-(c)).
  In the exe, 0x00417730, 0x0041ae20/0x0041b520, 0x0041ad60, 0x00415020, 0x00413bc0 and 0x00414120 are not reached from the
  exe's own call graph by any caller this TU set shows (they are install points for the .asi). The gates exercise
  0x00414180 only.
- x87 vs SSE2: none of the 8 TUs is in `x87_tus.txt`. The float work in these bodies is exact under both: copy,
  `x+x`, int-to-float times 0.25, truncation of a float RNG draw, float compares. So no TU was added to the list.

## Matched but not swapped (no C4 by match without a port decision)

| rva | why not swapped |
|---|---|
| 0x00414060 | exe port (Race/AiDifficultyReset.cpp:37) runs only step 6 of 6 by design. The full matched body adds steps 1-5, a behaviour change: needs a default-OFF knob + PREREG + decision |
| 0x004148b0 | the exe reaches it only through measurement probes with an exit-site recorder (`LT_EXIT`, read by the H_DEF gates dump); a swap removes the recorder and moves gates.csv by construction |
| 0x00418860 | the exe copy (Ai/AiStandalone.cpp:2071) is a stub with the rubber band stubbed out; a full body is a behaviour change |
| 0x00414a70 | its matched file also defines 0x00414300 (97.8%, not matched); the exe arm (Ai/AiTargetClosest.cpp, MASHED_TGT3 default OFF) is a verbatim naked-x87 pair |
| 0x0041a3e0 | body tail-calls the file-static 0x0041c2d0 (EAX); the port's 0x0041c2d0 is a separate register-ABI body, so including the matched file would add a second 0x0041c2d0 |
| 0x00417cf0 | scanner binding only: the "exe body" is a comment at Ai/AiStandalone.cpp:1424 bound to ControlStepM8, not a port of 0x00417cf0 |
| the other 78 | no body in the exe: 26 have an .asi-only body (rubric (b) needs the exe), 52 have no port body at all |

## Not matched (36): best % and class

From the helper notes (`notes/h1.md`..`h4.md`, and each source's header lists what was tried):
- 90-99%: 0x00412cf0 99.0 (b), 0x00414300 97.8 (a?), 0x0041af50 96.3 (b/a?), 0x00414f00 95.8 (pilot a?,
  `re/match/pilot/a1_00414f00.c`), 0x00412e30 94.7 (b), 0x00417640 94.6 (a?), 0x00415e20 93.8 (b).
- 76-89%: 0x00413cb0 89.3, 0x00419100 85.9, 0x00412f30 83.9, 0x00415220 79.4, 0x0041ac60 79.2, 0x00417180 76.8,
  0x0041a400 76.4 (a?), all (b) unless marked.
- under 76%: 0x00417450 73.6, 0x0041a9d0 73.0, 0x00417da0 66.5, 0x00416060 63.6, 0x00416a30 62.6, 0x00419a00 60.1,
  0x00416250 58.2, 0x0041aac0 29.2 (draft).
- not attempted (budget): 0x00412190, 0x00412620, 0x004128f0, 0x00414570, 0x00414c30, 0x00415880, 0x004177b0,
  0x00418560, 0x00418aa0, 0x00418bd0, 0x00418f50, 0x00419530, 0x004197e0, 0x0041a250.
- One shared residual dominates 0x00416250 / 0x00416a30 / 0x00417da0: the original keeps the 0x00415e20 steer
  result in a stack slot as well as on the x87 stack (every frame offset 4 bytes off). One source form fixes three.

## Findings (method, for the next slices)

1. Float constants at 0x005cxxxx are compiler literals (`__real@` pool), not globals. Declared as extern floats cl
   loads the constant first. As literals it gets the original's operand order. 0x005cc94c = 2^32 (unsigned fixup)
   shows it. Exception: 0x005d757c (0.0f) is read from memory and stays an extern.
2. Register-passed arguments came from file-static callees defined before the caller in every case (no /GL). A
   plain external-linkage leaf defined earlier in the TU also lets cl keep a caller value in ECX across the call
   (0x00417cf0). A stand-in callee must use its argument as the real one does, or cl picks another register.
3. `fld b / fsubr a` for `a - b` is source shape (`d = b; d = a - d;`). This weakens the pilot's (a) reading of
   0x00414f00-style residues.
4. Several globals must be one struct to match (0x00414120, 0x00417180: as separate symbols cl caches or splits).
5. Matcher caveat: on the original side an operand counts as an address unless the recompiled obj holds the same
   value as a plain literal. A literal loop bound equal to a relocated symbol address then reads as a mismatch
   (0x004196f0, fixed by writing the bound as `(int)&DAT_0063bf70`).
6. Ghidra's size is short for 0x00412130, 0x00412890, 0x00413f20, 0x00414120, 0x00417cf0, 0x0041a1e0. They count as
   matched only through `--bytes` over the whole symbol (pilot rule).
7. The cut holds: 0x0041b630 / 0x0041b660 (MATCH-S04's range) match in the same group file as 0x0041ad60..0x0041b520,
   so that object file straddles the S03/S04 boundary. S04 can take them from `h1_grp_0041ad60.c`.

## Tool changes (in this branch)

- `re/tools/match/g2s.py`: Ghidra partials of odd width (`x._0_3_`) no longer crash the converter.
- `scripts/rva_body_scan.py`: an `#include "<...>re/match/<...>.c"` directly under a run of RVA comments is those
  RVAs' body (multi-RVA allowed. Only the comment lines directly above count, so disassembly blocks further up are
  not bound).
- `verify/matchs03_20261031/collect.py`: per-RVA best score + sha256 + command table.

Needs Mariano: nothing. `build_objs.ps1` takes only `.cpp` entries, so the matched `.c` files are compiled through
the C++ TUs that include them. No `.ps1` / `.bat` change was needed.

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (the merge train's default set), under
`re/orchestrator/mashed_lock.py run`. Private root `D:/MashedWT/MATCH-S03_root` (robocopy of `original/`), `A4_EXE` set
to a frozen copy of the exe, `A4_OUT` under `D:/MashedWT/MATCH-S03_data/`. Builds:
`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmoduild.bat"`, Build OK, rva-lint OK (16 known, NEW=0), asm-lint OK.

| exe | swaps | G-CTRL (step / gates / ptrace) | B (K0 / K1 / K26) | arm exits |
|---|---|---|---|---|
| 5b2fa647 (b8c5c882d) | 7 | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | all exit=0 self_exited (H 491 s) |
| b1b60643 (755be4a0e) | 8 | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | all exit=0 self_exited (H 544 s) |

The gates exercise 0x00414180 only (see the swap notes). For the other 7 swapped RVAs the identical hashes show that
the swap did not disturb the default build. They are not a runtime witness of those bodies.

## Raw data (not committed)

`D:/MashedWT/MATCH-S03_data/`: `decomp.json` (Ghidra batch), `g2s/` (raw converter output + `results.tsv`),
`perm/` (permuter log), `build_base.log`, `build_swap2.log`, `exe_swap2/mashed_re.exe` (the gated exe),
`arms_swap2/` (gate arm outputs). SHA-256 of the cited raw files:

```
e0deeefb81149f0fbf28f885aadacd3abea473f6c691d3c60d2d0564bb89c0ff  decomp.json
e5ebd6e3a5a069437746177ec684d4067dfc01dee650075eaaf978631c13f8a0  g2s/results.tsv
5b2fa64743ffff257d940782db269fcb626f3f7701483888817115ec74455660  exe_swap2/mashed_re.exe  (7 swaps, b8c5c882d)
b1b606433e9f3e39d8cc2434b8f7ebfc1591420efa7ed99359f224bcbb118fa7  exe_swap3/mashed_re.exe  (8 swaps, 755be4a0e)
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_swap2/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_swap2/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_swap2/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_swap2/B_DEF_K0.step.csv (= K1 = K26)
arms_swap3/*: byte-identical to arms_swap2 (same six sha256 values)
```

## Helpers

Four in-session helpers (h1..h4) did the hand matching under `AGENT_BRIEF.md`. They wrote only `re/match/s03/h*_*.c`,
`notes/h*.md` and `log/permute/`. No helper ran git, a game, Frida or a tracker edit.

## HANDOFF (what is left in this slice)

- 36 functions are not byte-identical (list above). Sources with every attempt in their header comment are in
  `re/match/s03/`. Best next targets: the 7 at 90-99%. The shared steer stack-slot form would move 0x00416250,
  0x00416a30 and 0x00417da0 together.
- Swaps that need a decision first: 0x00414060 (exe port is partial by design), 0x00418860 (exe stub),
  0x004148b0 (measurement probe + gates dump recorder). Each would be a behaviour change behind a default-OFF knob.
- When several group files go into one C++ TU, the shared struct names (`Obj48`, `RwRGBA_`, `RwV3d_`, `Ent7f1a10`,
  the per-car block at 0x0089a4c4 as `CatchUp` / `AiPwr` / `AiCar74`) need one shared definition (helper notes h1, h4).
- re-classify (after MATCH-RUBRIC merges): the 8 swapped RVAs, route `match`, fields from the swap table above.
