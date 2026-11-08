# RESULT MATCH-RWCORE-G5, 2026-11-07

**Verdict: GREEN (match-only, 0 new identical).** Identical/attempted: **0/33**. No committed source below 100% in
the RenderWare core reaches 100% under `/O2 /Ob1 /G5` (no `/GS`), under 16 further flag sets, or under a bounded
permute of the 11 rows at >=85%. `re/match/rwcore_g5/` is not created (nothing to put in it). The brief for S20/S21
is `flags_brief.md`: `// FLAGS: /O2 /Ob1 /G5` for the whole range, which reproduces all 84 identical S19 rows with
`relocheck.py` OK.

Branch `orch/MATCH-RWCORE-G5`, cut from orch/integration eb7fe226b. Match-only: no body swap, no `mashedmod/src`
edit, no tracker edit, no existing `re/match/sNN` file changed. Compiler XP DDK cl 13.00.9176 / c2 13.00.9178,
`re/tools/match/match.py` and `relocheck.py` from this branch, original `original/MASHED.exe.unpatched`.

## 1. RW core address range

| bound | value | evidence |
|---|---|---|
| start | 0x004c0740 | last `third-party-library[lua-4.0]` row in hooks.csv is 0x004c0700 (Lua spans 0x004b6fc0..0x004c0700). 0x004c0740 is the next function and is 100% only with `/G5` (46.7% raw g2s in S19, 100% at `/O2 /Ob1 /G5`, below 100% at `/O2 /GS /Ob1`, `sweep_all.tsv`) |
| end | before 0x004ec760 | first `third-party-library[d3dx9-psgp]` row is 0x004ec760; the last `[renderware]` row of the segment is 0x004ebc30 |
| build | `Core built at Mar 17 2004 07:14:02` | string at 0x005d8b41 (CLAUDE.md build stamps) |

hooks.csv has further `[renderware]` segments at 0x004f0910..0x004f46a0 and 0x0052d8e0..0x00558df0 (plus single rows
at 0x00561ee0 and 0x005cb000..0x005cb42b). No committed match source lies in them and no slice has measured their
flags, so they are outside this step. [UNCERTAIN] that they share the core's flags: no matched function there.

Rich header (libid RESULT, "Flags found"): `28/9466 x373`, `28/9178 x8`, `29/9178 x49`; counts only, no per-object
address, so it does not bound the range. It is cited for the 9466 compiler residual only.

## 2. Committed sources in the range

`re/match/` per-directory RVA spans (first..last file): s01..s16 end at 0x0049ab10, `gffu` holds 0x00417740..0x005c9d00
(0x005c9d00 = GetRaceEndTrigger, `util`, 3 B, outside the RW segments), `t01g`, `s08fu`, `pilot` lie below 0x00499740.
**Only `re/match/s19` (117 files, 0x004c0740..0x004c7760) is in the RW core.** S17, S18 and S20 sources are not on
orch/integration yet.

Of the 117: 84 identical in S19 (80 scored plus 4 with no Ghidra size, 0x004c07b0, 0x004c0830, 0x004c4430,
0x004c4470, each 100% here with `--only`), **33 below 100%: the attempted set.** Old flags on line 1 of the 33: 11 at
`/O2 /GS /Ob1`, 21 at `/O2 /GS /Ob1 /G5`, 1 at `/O2 /Ob1 /G5` (0x004c6750).

## 3. Re-score

All runs on scratch copies in `D:/MashedWT/MATCH-RWCORE-G5_data` (match.py `--flags ... --only <rva>`, permuter on
copies with line 1 replaced by `// FLAGS: /O2 /Ob1 /G5`).

| leg | rows | result |
|---|---|---|
| `/O2 /GS /Ob1` vs `/O2 /Ob1 /G5` | 33 | reproduces the S19 flagsweep to the decimal; 0 at 100% |
| 16 more flag sets (`flags_brief.md` lists them) | 33 x 16 | 0 at 100%; no row >=85% moves; 8 low rows move (largest 0x004c51a0 14.7 -> 32.8, `/QIfist`) |
| bounded permute, `--rounds 4 --walk 40 --time 300` | 11 rows >=85% | 0 gains in 1,940 compiles |

Permuted rows (start = best): 0x004c0c20 98.9, 0x004c0f00 98.8, 0x004c3c60 97.3, 0x004c3040 97.1, 0x004c1b40 92.7,
0x004c3910 91.5, 0x004c4dc0 91.2, 0x004c4d20 90.9, 0x004c1a70 90.7, 0x004c5cb0 89.4, 0x004c5930 89.1. 0x004c5010
(84.7) is below the 85% bar and was not permuted. Every one of these was permuted in S19 under `/O2 /GS /Ob1 /G5`
with no gain; none has a cookie-eligible local, so dropping `/GS` does not change their code, and this leg confirms it.

Rows where the game flags beat `/O2 /Ob1 /G5`: 0x004c1f80 (17.4 vs 16.9), 0x004c32b0 (40.6 vs 37.7), 0x004c4680
(34.5 vs 29.7). All three are first structural passes under 41%; the difference does not place them in another project.

Per row: `results.tsv` (rva, size, old flags, old score, score at `/O2 /GS /Ob1`, new flags, new score, best other
flag set if higher, permute best and compiles).

## 4. Flag uniformity check (supports the brief)

All 117 S19 sources under three sets (`sweep_all.tsv`): 50 identical only with `/G5`, 34 identical with and without
it, 0 identical only without it, 33 identical under none. `/O2 /GS /Ob1 /G5` and `/O2 /Ob1 /G5` agree on which rows are identical; their scores differ on 3 rows only, each
higher without `/GS` (0x004c6580 34.0 -> 36.6, 0x004c6750 74.2 -> 83.3, 0x004c6ba0 14.9 -> 23.2).
`relocheck.py` on the 84 identical sources with line 1 replaced by `/O2 /Ob1 /G5`: **84 OK, unchecked=0**
(`relocheck.tsv`). Those 84 are already identical under their committed flags (S19); they are not new and are not
copied to `re/match/rwcore_g5/`.

## Gates

Not run: match-only, the merge train runs them. No build input changed.

## Decision

None. L-93: a 100% byte match with relocheck OK, not naked asm, is C4 on its own; C4R-MATCHBATCH-2 promotes. This
step adds no identical row, so it hands C4R-MATCHBATCH-2 nothing new.

## Raw data (not committed), `D:/MashedWT/MATCH-RWCORE-G5_data`

| file | sha256 |
|---|---|
| sweep_base.tsv (33+4 rows x 2 sets) | 73a433e3c64379a3f7a1ec04caf2cabe539dc20c130a8e02c4cb2a400ed97090 |
| sweep_ext.tsv (16 sets) | c763556778258f3f4068fe847c460a316826fc7c8eb1f0e731fc6c1199a4de37 |
| sweep_all.tsv (117 rows x 3 sets) | fb3bbed3837dea600be15417a2f5e0f477d066441be1f89b0b9cfc41b1124f5d |
| perm_all.log | 04af375f3b7ce8875836d4e328a7db5eda46a5aee8fe37ca4330a7ec9f357eb6 |
| relocheck_g5.txt | b3221847df4c27da14823a903ec7fa6c54bbfe8c27dc97488ae6f8d9dc0e1b97 |
| tools/sweep.py, tools/sweep_all.py, tools/perm.sh | 7d78e34dbdf40b0155c77de4acf208883c0bdc35d9f2719e7bcb42ed6f4d01ee, 03a21e849763d15e08eb89214c74d0d9a4f6f09da04d8d1846fb898938ecfd44, 3035eb0cf9de1a0385edad9d284cbf9dd3063d5d37032c495a4caa99b661117f |
| perm/<rva>.log (11) | listed in `perm_all.log`; best variants in the worktree's `log/permute/` (gitignored) |
