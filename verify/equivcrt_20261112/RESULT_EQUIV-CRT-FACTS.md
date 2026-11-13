# RESULT EQUIV-CRT-FACTS (evidence for decision L-140, open)

Text row. No rule built, `re/tools/match/equiv.py` untouched, no build, no gates, no game run.
Base: `orch/integration` b34672ee2. Tool: equiv.py at that commit. Disassembly: `capstone` over
`original/MASHED.exe.unpatched` (read-only). Addresses are the VAs the trackers use.

## 1. The 17 rows

| Group | Rows | Blocker today |
|---|---|---|
| A | `0x00499400` (source `re/match/s16/00499400.c`, 90.1%, `PROVISIONAL.tsv` line 22) | `call-regargs` at call `0x496400`, EDI only (ours 0, original `esp0-4`) |
| B | 16 rows: `0x0051cfc0 0x00523170 0x005238c0 0x00523cd0 0x00523e40 0x005241f0 0x005245b0 0x00524f70 0x005253c0 0x00525870 0x00526310 0x00526630 0x00526910 0x00526bf0 0x00527560 0x00528810` (sources `re/match/s23/<rva>.c`, 76.7% to 97.8%) | `callee-pop-unknown` on callee `0x00517250` (15 rows) or `0x005172b0` (`0x00523170`) |

Source of the list: `verify/equivcheck_20261112/RESULT_EQUIV-NORETURN-RECHECK.md` section 3 (group A, and the 16 rows under "prefix to acos") and section 5. None is in `ADMITTED.tsv`.

## 2. Verdict today

All 17 re-run on 2026-11-12 with `py.exe -3.12 re/tools/match/equiv.py --only <rva> <src>`. Raw lines (cut at 330 chars) are in `equiv_today.txt`. All 17 are `NOT-PROVEN`, identical to `verify/equivcheck_20261112/measure_final.tsv`.

Group B extras: `0x00523170` also prints "(fallback: stack address inside and)", `0x00528810` also "(fallback: argument-area address escapes (stored to memory))". These are constructs the tool reports next, so they may block even after the callee fact is supplied.

[UNCERTAIN] for all 17: whether the proof closes once the fact is applied. Missing evidence: a run with the fact applied, which needs the rule to exist. Match of 76.7% to 93.5% means many differing bytes, so a later first difference is likely for the low ones (`0x0051cfc0`, `0x00526bf0`).

## 3. Library facts the rule would assume

### Group B: `_longjmp` is no-return (one fact, 16 rows)

- `0x00517250` and `0x005172b0` each: optional callback `call [esi+0x40]`, then `push 1; push esi; call 0x005c1ea0` (`0x0051726a`, `0x005172e0`), then `pop esi` and fall into the next function's bytes (`0x00517270`, `0x005172e6` padding then `0x005172f0`). They resemble a libpng-style error helper [UNCERTAIN: name not needed, no libpng source checked].
- `0x005c1ea0` is `_longjmp`: `equiv_symmap.tsv` line 131 (`_longjmp 0x005c1ea0 2`), `function_inventory.csv` line 5641. Body restores EBX/EDI/ESI from the jmp_buf, `mov esp,[edx+0x10]`, then `jmp dword ptr [edx+0x14]` at `0x005c1f15`. The `ret` at `0x005c1f18` follows that unconditional jmp and `0x005c1f19..1b` is int3 padding before the next function (`_setjmp3`, `0x005c1f1c`), so it is not reachable from `0x005c1ea0`.
- Why the tool says unknown today: `summary(0x005c1ea0)` fails with `r9` (state merge with different ESP at the indirect jump table walk, `equiv.py` lines 1073/1106), so `0x00517250` gets neither a pop count nor a no-return flag.
- Fact the rule asserts: a call to `0x005c1ea0` never returns. It ends the path (the existing `no-return` tag, `equiv_rules.md` section 2). Citation: the C `longjmp` contract plus the disassembly above.
- Assumption added: the original's function at `0x005c1ea0` is the CRT `longjmp`. Evidence: the symmap name and the VC20 jmp_buf magic `0x56433230` at `0x005c1ed0` and `0x005c1f34`. [UNCERTAIN: the VC6 CRT source is not in the repo, so the name rests on the symmap and that signature.]

### Group A: `0x00496400` callees

`0x00496400` (104 bytes) calls `0x004a42c5` at `0x00496437` and `0x004a4254` at `0x00496447`. With no summary every register counts as read, so EDI becomes a register argument of `0x00496400`, and the comparison finds ours 0 against original `esp0-4`.

- `0x004a4254` = `_fputs` (`equiv_symmap.tsv` line 130, `function_inventory.csv`). Summary fails `r7`: `ret` at `0x004a42ba` with ESP untracked (ebp frame from `__SEH_prolog` `0x004a5984`, exit through `call 0x004a59bf` = `__SEH_epilog`, which pops edi, esi, ebx and runs `leave`, `0x004a59ca..cd`).
- `0x004a42c5`: sprintf-shaped wrapper [UNCERTAIN: tracker name is `FUN_004a42c5`, not in the symmap. Shape: fake FILE struct `{ptr=buf, cnt=0x7fffffff, base=buf, flag=0x42}` at `0x004a42d9..eb`, `call 0x004a504f`, then `and byte [ptr],0`]. Summary fails `r10` through callee `0x004a504f` (1909 bytes, jump table at `0x004a50c9`, a join with different ESP depth, `equiv.py` line 1116).
- Facts the rule would assert for these entries:
  1. cdecl, pop count 0: plain `ret` at `0x004a42ba` and `0x004a431b`, caller does `add esp` (`0x004a42f3`).
  2. Reads no register at entry. `0x004a42c5` saves esi/edi by push (`0x004a42cb`, `0x004a42cf`) before use. `0x004a504f` pushes ebx (`0x004a5060`), esi/edi (`0x004a507e/7f`) and writes EAX/ECX before reading them (`0x004a5058..5074`). `_fputs` goes through `__SEH_prolog`, which pushes ebx/esi/edi (`0x004a599e..a0`) before use.
  3. Preserves EBX, ESI, EDI, EBP: pops at `0x004a57b1..bc` (`0x004a504f`), `0x004a4318..1a` (`0x004a42c5`), `__SEH_epilog` (`_fputs`).
  4. Clobbers EAX, ECX, EDX. EAX is the return value.
  5. Memory effect stays opaque, as the call record already treats callee memory [UNCERTAIN whether the rule must say more].
- Residual risk: `0x00499400` is 90.1% (269 of 657 bytes differ, source comment lists an inner listing loop residual, `re/match/s16/00498a00.c` line 19ff). [UNCERTAIN] whether EQUIV is reached after the EDI difference goes away.

### Not needed
`0x004a3f90`, `0x004a3fd2` (unwind helpers), `0x005c318c` (`__rt_probe_read4@4`): they sit behind `_longjmp`, which never returns.

## 4. What the rule would be (description only, not built)

A table (`equiv_symmap.py` or a new TSV): `rva name pop noreturn livein preserves eax_def`, consulted in `summary()` before the walk. Each entry carries the disassembly citation from section 3. A wrong entry makes the tool admit a body whose callee behaves differently, an unsound admission that L-126 would rely on. Hence the decision.
Candidate entries from this evidence: `_longjmp 0x005c1ea0` (noreturn), `_fputs 0x004a4254`, `0x004a42c5` and `0x004a504f` (cdecl, no register args, preserve EBX/ESI/EDI/EBP).
Effect: group B moves from `callee-pop-unknown` to its next first difference, group A from the EDI `call-regargs` to its next. How many become EQUIV: [UNCERTAIN], 0 to 17.

## 5. Recommendation for L-140 (not decided here)

If approved: scope to the 4 entries above, add a unit test per entry that mutates the fact (as `equiv_tests` does for A6), re-run the 1,106 rows to count real EQUIV gains before any `ADMITTED.tsv` line. If not: `0x00499400` stays provisional and group B stays NOT-PROVEN. Group B rows are also C2 at 76.7% to 97.8%, far from a byte match.

## Evidence files
- `verify/equivcrt_20261112/equiv_today.txt`: the 17 equiv.py lines from 2026-11-12.
- Inputs read: `verify/equivcheck_20261112/RESULT_EQUIV-NORETURN-RECHECK.md`, `measure_final.tsv`, `re/match/PROVISIONAL.tsv`, `re/tools/match/equiv_symmap.tsv`, `re/analysis/function_inventory.csv`, `log/orch/DECISIONS.md` line 200, `log/orch/run39_notes.md` line 17.
