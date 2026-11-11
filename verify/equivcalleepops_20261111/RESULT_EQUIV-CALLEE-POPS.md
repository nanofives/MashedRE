# RESULT EQUIV-CALLEE-POPS: equiv.py pop counts for calls through a pointer

Row: EQUIV-CALLEE-POPS, branch `orch/EQUIV-CALLEE-POPS`, worker claude3 (Opus 5.5 medium), 2026-11-11.
Lead: MATCH (cmwcaoc5e2x1psc1buqo1zr9j). Tool: `re/tools/match/equiv.py`. Rules: `re/tools/match/equiv_rules.md`.

## Verdict

- **equiv.py can now prove functions that make `__stdcall` calls through a pointer** (COM vtable calls such as
  `call [ecx+0x6c]`, RW function-pointer slots). It no longer assumes the callee pops nothing. The new rule is
  **A6**: the original's pop counts are solved from the original's own stack equations, and our side copies
  the count of the paired original call. A4 stays as the fallback.
- **`stmxcsr` / `ldmxcsr` are modelled** as part of the floating-point state term. This adds no assumption.
- **Self-tests 55/55.** That is 38 old pairs, 11 new pairs (5 of them negatives) and the 6 reference rows.
  The mutation harness kills or classifies every mutant of the new pairs (51 killed, 2 proven-independent
  swaps). The 2 survivors overall are the same two as before this row (`enc_jmp`, `neg_reg_ret`).
- **No regression on the 1,106-row measurement.** All 133 EQUIV and 61 EQUIV-IF-CALLERS-IGNORE rows keep their
  verdict. 6 NOT-PROVEN rows now prove.
- **Driver sources (orch/RWC-DRIVER, 22):** 4 EQUIV, 1 EQUIV-IF-CALLERS-IGNORE, 16 NOT-PROVEN for other reasons,
  1 timeout. The 4 EQUIV are the byte-identical C4 bodies. Each still gave `ret-esp` before this row.
- **Nothing admitted.** A6 is a new proof rule for L-126 admissions, so it is filed as decision
  **EQUIV-CALLEE-POPS**. `ADMITTED.tsv` is untouched. The admission candidates the rule would open are listed
  below.

## The rule (A6) and why it is sound

The full text is in `equiv_rules.md` §4 (A4, A6) and §5 (MXCSR). In short:

1. **The original's counts.** `pin_pops` walks the original's control-flow graph. It tracks ESP as a linear form
   `k + sum P_i`, with one unknown P_i per call through a pointer. Direct calls use the callee summary's pop count.
   Imports use their decoration. Jump tables are followed.
   MSVC frame-pointer-omitted code keeps one ESP offset per instruction, has ESP at the entry offset at every
   `ret` and tail jump, keeps ESP at or below the entry offset, and its callees pop only inside the frame. Those
   facts give the equations and bounds.
   Every solution is enumerated: each P_i is a multiple of 4 in `[0, -k_i]`. The real counts are one of the
   solutions. A body that writes ESP any other way gets no A6, and A4 applies as before.
2. **One proof per solution.** With a single solution (`A6-pinned-pop`) the proof runs once. With several
   (`A6-pop-solution`) it runs once per solution, up to 32, and returns the weakest verdict. More solutions than
   that, or more than 4,096 candidate assignments, gives `NOT-PROVEN callee-pop-ambiguous`.
   Several solutions are a real limit of the bytes, not a tool gap. On one path, a cdecl call with a deferred
   `add esp` followed by a `__stdcall` call reads the same as the reverse. Test pair `com_ambiguous` covers this:
   it is proven once per solution. Test pair `neg_com_solution` is right under one solution only, and is refused.
3. **Our counts are copied, not assumed** (`A6-paired-pop`). Our k-th call in a region takes the count of the
   original's k-th call in the same region. That count is part of the call record, and the targets must be the
   same term. So both sides call the same function, and the copied count is that function's real count whenever
   the original's is.
   A wrong typing in our source (cdecl where the method is `__stdcall`) then shows up as our ESP unbalanced at
   return. Test `neg_com_conv` checks this and gets `ret-esp`. The source's prototypes are never read.
4. **Why the source prototype was not used** (design hint (a)). Mapping C call expressions to call instructions
   after optimisation is fragile. The pairing above gives the same check, that both sides agree on the count,
   without it.

Assumption added: A6, that the original keeps MSVC's stack discipline. It is the only new one.
Rules unchanged: A4 (fallback), A4' (`--relax-icall`, not used here) and the ECX/EDX default for calls through
a pointer.

## Self-tests (`py -3.12 re/tools/match/test_equiv.py`, `test_equiv.txt`)

| Pair | Expected | What it pins down |
|---|---|---|
| `com_ident` | EQUIV | byte-identical control with a `__stdcall` COM call |
| `com_stdcall` | EQUIV | COM call, early return, cdecl direct call, second COM call; encoding differences |
| `com_cdecl` | EQUIV | call through a pointer with caller cleanup (pins 0) |
| `com_loop` | EQUIV | COM call inside a loop (pinned by the loop-head join) |
| `com_ambiguous` | EQUIV, `A6-pop-solution` | two calls the equations cannot separate, proven per solution |
| `mxcsr` | EQUIV-IF-CALLERS-IGNORE(EDX) | `stmxcsr` / `ldmxcsr` |
| `neg_com_argcount` | NOT-PROVEN call-args | wrong prototype argument count |
| `neg_com_order` | NOT-PROVEN call-target | swapped call order |
| `neg_com_conv` | NOT-PROVEN ret-esp | ours cdecl, original `__stdcall` |
| `neg_com_solution` | NOT-PROVEN | ours right under one pop solution only |
| `neg_mxcsr` | NOT-PROVEN ret-x87 | MXCSR loaded from different words |

Mutation harness (`equiv_mutate.py --real 0`, `mutation_synthetic.tsv`): 171 killed, 8 proven-independent swaps,
4 flagged return registers, 2 survived with no dynamic difference. All of these numbers are the same as in
EQUIV-CHECK's `mutation.tsv`.

## The 22 driver sources (orch/RWC-DRIVER, fetched into scratch, not into this branch)

The files are `driver_before.tsv` (RWC-DRIVER's `final_scores_equiv.tsv`), `driver_after.tsv` and
`driver_sources.tsv`. The lead's "21" is these 22 rows less 0x004d6200, which never gives a verdict.

| rva | before | after | why |
|---|---|---|---|
| 0x00540340 | ret-esp | **EQUIV** (byte-identical) | `__stdcall` COM calls pinned |
| 0x004cb600 | ret-esp | **EQUIV** (byte-identical) | same |
| 0x004dc5b0 | ret-esp | **EQUIV** (byte-identical, 30 pop solutions) | RW pointer calls with `add esp` and a COM call share paths |
| 0x004d5bc0 | no verdict in 300 s | **EQUIV** (byte-identical, 727 s) | slow before this row too |
| 0x004d1640 | ret-esp | EQUIV-IF-CALLERS-IGNORE(EAX) | its one caller 0x004d1d3f reads EAX: not admissible |
| 0x004d1e30 | ret-esp | callee-pop-ambiguous (36 > 32) | with the cap at 64: `call-target`, the case-5 schedule residual |
| 0x004c7a70 | unsupported `stmxcsr` | call-memory | MXCSR now modelled. The next difference is ours `or ah, 0x80` against the original's `or eax, 0x8000` (orig 0x004c8447 path). The values are equal, but the term normaliser has no byte-insert-of-or rule. RWC-DRIVER lists more residuals after it |
| 0x004c90a0 | switch-index | switch-index | byte-index (two-level) jump table, excluded by §5 |
| 0x004d7480, 0x004dba60 | switch-table | switch-table | "no relocated entries at ours": our table is not read |
| 0x004db3e0, 0x004cb8a0, 0x004d2980 | call-regargs | call-regargs | only ECX/EDX differ at a call through a pointer (A4's register default). The `--relax-icall` question, not used |
| 0x004c9eb0 | call-target | call-target | real difference (prologue / ebx) |
| 0x004cff20, 0x004e0920, 0x005405c0 | call-args | call-args | real differences (entry saves, ebp/ebx, raw source) |
| 0x004cb4c0 | branch-condition | branch-condition | real (loop down-counter) |
| 0x004d0290 | call-args | branch-condition | now runs further. Real entry-save residual |
| 0x004cb330 | indirect-call-esp-reset | same | EBP frame: A6 and A4 both refuse a body that restores ESP from a register |
| 0x005412d0 | call-sequence | same | `fistp` against `__ftol` |
| 0x004d6200 | no verdict | no verdict in 900 s | performance, unchanged |

## Whole measurement (1,106 sources, `measure_after.tsv`)

Same rows and sources as EQUIV-CHECK's `measure_v4.tsv`, run on this branch, 300 s per row, under test-slot.

| before -> after | rows |
|---|---|
| EQUIV -> EQUIV | 133 |
| EQUIV-IF -> EQUIV-IF | 61 |
| NOT-PROVEN -> EQUIV | 4: 0x004cba80, 0x005be9f0, 0x005bf8a0, 0x00540340 |
| NOT-PROVEN -> EQUIV-IF | 2: 0x004cb600 (EAX, 3/3 callers ignore), 0x004d1640 (EAX, caller reads) |
| NOT-PROVEN -> NOT-PROVEN | 896 (20 changed kind: 5 now `callee-pop-ambiguous`, others moved past the pop wall to the next difference) |
| NOT-PROVEN -> timeout | 5, all already timeouts in measure_v4 |
| NOT-PROVEN -> ERROR | 5 sources with no `// MATCH:` annotation for the row (measure_v4: `no-annotation`) |

The gains use only `A6-pinned-pop` + `A6-paired-pop` (one solution each). A separate check with the final tool:
the 42 proven rows that relied on A4 and the 20 quick driver rows keep their verdicts.

Two build fixes are on this branch. Both came out of the measurement:
- **Import calls in the original.** `Resolver.import_pop` is filled when our side resolves an `__imp_`
  relocation. With the original now running first, our imports are resolved before solving. Without this fix,
  0x005bcda0 and 0x005a86a0 lost EQUIV, and they are back.
- **Time.** The solver needs every direct callee's summary, which caused 1,500 s+ hangs on 0x004111c0 and
  0x00481e00. It now runs only when the proof reaches a call through a pointer on the original's side, and not
  at all for a body without one. Both rows are back to their old verdicts.

## Admission candidates (not admitted: decision EQUIV-CALLEE-POPS)

If A6 is accepted, these become admissible under L-126 with no further decision row (caller scans as printed):

| rva | source (orch/integration) | verdict | note |
|---|---|---|---|
| 0x004cba80 | re/match/s20/004cba80.c (90.3%) | EQUIV [LAYOUT,FRAME,VALUE-FLOW] | |
| 0x005be9f0 | re/match/s28/005be9f0.c (80.0%) | EQUIV [ORDER,LAYOUT,FRAME,VALUE-FLOW] | |
| 0x005bf8a0 | re/match/s28/005bf8a0.c (95.7%) | EQUIV [ORDER] | |
| 0x004cb600 | re/match/rwl01/004cb600.c (78.6%) | EQUIV-IF-CALLERS-IGNORE(EAX), 3/3 ignore | moot once RWC-DRIVER merges: that source is a 100% match there (C4) |
| 0x00540340 | re/match/rwl03/00540340.c (99.4%) | EQUIV | moot once RWC-DRIVER merges (C4 by match) |

## Follow-ups

- Term normaliser: `ins32(x, or8(ext8(x,8), c), 8) == or32(x, c << 8)` (0x004c7a70). This is a 32-bit identity.
- Callee summaries (`summary()`) still use A4 inside callees. An original callee that makes a `__stdcall` COM
  call has no summary, so every register counts as read at calls to it. A6 could be applied there too.
- The RW function-pointer slots (`[RwGlobals + 0x118]` and so on) are behind most `callee-pop-ambiguous` rows. A
  writer scan of those slots would give their callees, and with them exact pops and registers.
- The ECX/EDX default at calls through a pointer blocks 0x004db3e0, 0x004cb8a0 and 0x004d2980. This is the
  existing `--relax-icall` question, a decision of its own.

## Files

- `re/tools/match/equiv.py`: `pin_pops`, `_table_targets`, `Ctx.pins` / `pin_callee`, A6 in `do_call`, original
  region first in `Explorer.run`, per-solution `check_sides`, `stmxcsr` / `ldmxcsr`.
- `re/tools/match/equiv_rules.md`: A4 note, A6, MXCSR, tests.
- `re/tools/match/equiv_tests/cases.asm`, `re/tools/match/test_equiv.py`: 11 pairs, RULES check.
- Evidence: `test_equiv.txt`, `mutation_synthetic.{tsv,txt}`, `driver_{before,after,sources}.tsv`,
  `measure_after.tsv`.
- No build, no gates: tool-only row.
