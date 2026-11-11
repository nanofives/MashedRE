# RESULT EQUIV-CHECK (2026-11-11): equiv.py proves 168 of 1,106 near-match rows and 39 of the 326 blocking top-down functions

L2 worker `cmwbtmr6k2d1asc1bavzy28q8` for parent `cmwbssxl22bg4sc1ba3ghf7dh`. Branch `orch/EQUIV-CHECK` from
`orch/integration` `ee3562e27`. Tool, tests and this RESULT only: no tracker edit, no C-level change, no BINDINGS
change, nothing under `mashedmod/`. Built on claude2 (Python 3.12, capstone 5.0.7, unicorn 2.1.4, the DDK compiler
through `match.py`, MSVC `ml.exe` for the synthetic pairs).

## Verdict

**GREEN.** `re/tools/match/equiv.py` decides admission by proof, not by byte pattern.

- **Method.** It executes our DDK compile and the original symbolically from one shared entry state, and explores
  the two control-flow graphs as a product.
- **Accepts.** It answers `EQUIV` only when every observable is the same term on both sides. The observables are the
  call sequence with arguments, register arguments, memory and x87 state, plus memory and registers at every return.
- **Refuses.** Any byte it cannot explain gives `NOT-PROVEN` with the first unexplained difference.

**Over the 1,106 `REFUSED` ledger rows that have a source (default mode, `measure_v4.tsv`):**

| Verdict | Rows |
|---|---|
| EQUIV | 107 |
| EQUIV-IF-CALLERS-IGNORE | 61 (19 of them with every caller scanned as ignoring) |
| Byte-identical today (the ledger row is older than the source) | 26 |
| NOT-PROVEN | 912 |

At 95% or more there are 127 rows today: 34 EQUIV, 19 byte-identical, 74 NOT-PROVEN. 36 of those 74 are x87,
which is excluded on purpose.

**On the top-down path**, the 326 blocking C0..C3 functions:

- 32 EQUIV and 7 EQUIV-IF-CALLERS-IGNORE;
- 254 NOT-PROVEN;
- 33 with no source.

Of the 91 BINDINGS stand-in / under-stand-in rows, 78 are already ledger PASS. The tool releases 1 more
(`0x004d8480` RwErrSlotWrite, conditional on EDX/ECX).

**Evidence that nothing unsound got through:**

- **Mutation test:** 644 mutants of 19 synthetic and 21 real proven pairs, **0 escaped**. 558 are killed and 30
  are flagged as a return-register change. 43 swaps are correctly proven independent. 13 survive with no difference
  in 16 Unicorn runs, and each of those is equivalent on reading.
- **Dynamic cross-check:** all 194 proven rows (168 proofs plus 26 byte-identical), 16 random runs each,
  **0 differences**. 156 rows have 8 or more conclusive runs, 9 have 1 to 7, and 29 have none: every run hit the
  instruction or call cap, typically a list walk whose bound comes from random memory.
- **Caution on the counts:** every NOT-PROVEN count is a lower bound on what the tool could prove with more rules.
  In a sample of 452 NOT-PROVEN rows, the dynamic check observed a real difference in 215.

Three findings change the picture the brief started from:

1. **`0x0048e820`: the unedited source is not equivalent.** The admitted port used a wrapper that the unedited
   source does not have.
   - The tool's verdict is `NOT-PROVEN call-regargs`. `0x0048cee0` reads EDI as a register argument (Ghidra
     `unaff_EDI`). With ESI and EDI swapped, the unedited s15 source passes `0x3f800000` in EDI where the original
     passes the emitter index (`0x10`).
   - The Unicorn cross-check shows the same difference in 8 of 8 runs.
   - The admitted port is correct only because `PortTop_PS3_Call0048cee0` passes `i` explicitly
     (`RESULT_PARTICLE-3-SWAP.md`).
   - PARTICLE-3-SWAP found this by hand. The tool finds it mechanically, because it compares every register a callee
     reads at each call. A byte table alone does not show it.
2. **`0x00555360` is EQUIV-IF-CALLERS-IGNORE(EAX), not plain EQUIV.**
   - On one return path ours returns EAX = 0. The original returns the value left by the last call.
   - Its one caller, `0x00552b79`, does not read EAX. So the admission holds, but it rests on the caller, like
     `0x0045a3a0`.
3. **`0x005c4ad0` (held) proves EQUIV** under the exposed-frame rule A1'.
   - `match.py` shows an `imul` operand swap (`[esi+8]` and `[esi+0xc]`). The brief records a load and a stack store
     in opposite order. A local's address is passed to a callee.
   - The proof labels the row FRAME, VALUE-FLOW and uses A1'.
   - A1' is the stack rule the brief asks to be justified. Section 2.4 gives the justification.
   - The Unicorn cross-check finds no difference.

The reference rows otherwise come out as the brief states: `0x00475a00` EQUIV [SIB], `0x0045a3a0`
EQUIV-IF-CALLERS-IGNORE(EAX), and `0x00556e90` NOT-PROVEN x87-region.

## 1. Files

| File | What |
|---|---|
| `re/tools/match/equiv.py` | The checker. `py -3.12 re/tools/match/equiv.py <src.c> [--only 0x<rva>,...] [--json] [--relax-icall] [--x87-strict]` |
| `re/tools/match/equiv_rules.md` | The rule text: what a verdict means, each class's exact condition, assumptions A1 to A5, what is excluded |
| `re/tools/match/equiv_symmap.py`, `equiv_symmap.tsv` | Addresses of CRT, import and RenderWare relocation names, read from the 2,979 byte-identical ledger rows (137 names, 2 dropped as conflicting: `_free` / `__free` resolve to two different functions) |
| `re/tools/match/test_equiv.py`, `equiv_tests/cases.asm` | 38 synthetic pairs and the six reference rows (`[N/M]` per case, also runs under pytest) |
| `re/tools/match/equiv_mutate.py` | Mutation test |
| `re/tools/match/equiv_dyn.py` | Unicorn dynamic cross-check |
| `re/tools/match/equiv_measure.py` | The measurement over `match_ledger.tsv` (one subprocess per source, 240 s timeout) |
| `verify/equivcheck_20261111/measure_v4.tsv`, `measure_v4_relax.tsv` | Per-row verdicts (default, and with A4') |
| `verify/equivcheck_20261111/TABLES.md`, `crosstab.py` | The tables below, regenerable |
| `verify/equivcheck_20261111/dyn_proven.tsv`, `dyn_notproven.tsv`, `mutation.tsv` | Dynamic and mutation evidence |
| `verify/equivcheck_20261111/blocking_c0c3.tsv` | The 326 blocking C0..C3 functions, recomputed today with `origtop_size.py --order` (same count as SIZE.tsv) |

## 2. The rules as implemented

`re/tools/match/equiv_rules.md` is the normative text. Summary of what each part decides:

### 2.1 The proof (shared by every class)

- **Terms.**
  - Values are hash-consed terms over the shared entry symbols.
  - Canonical forms are 32-bit machine identities only: linear sums with wrap-around, sorted operands for the
    commutative bitwise ops and `mul`, constant folding, sub-register extract and insert, and one normal form for
    comparison predicates.
  - So equal terms denote equal values. That is the whole soundness argument.
- **Regions and the product.**
  - Each side runs to its next conditional branch, indirect jump or return.
  - The branch conditions must be equal, or each other's negation. In the second case the successors pair
    crosswise (LAYOUT).
  - Jump tables pair entry by entry.
  - Joins use an inductive equality partition, which handles loops.
- **Memory.**
  - A store chain. A load skips a store only for the same symbolic base and disjoint constant offsets. Adjacent
    stores are canonically ordered only under the same condition (ORDER).
  - Read-only image bytes fold to constants. The IAT does not fold.
- **Calls.**
  - The record is the target, every stack argument the callee reads, every register the callee reads, memory and
    x87 state.
  - Each callee's facts come from a walk of its original body: pop count, stack arguments read, registers read
    before written, EAX written on every path, returns or not.
  - The walk is order-independent. A result that touched a cycle is not cached. This matters: an earlier version
    cached order-dependent results, and `0x0044e190` changed verdict between processes.
- **Returns.**
  - Compared at every return: memory without the dead frame, x87, EBX/ESI/EDI/EBP/ESP, the return address and the
    pop count.
  - EAX, ECX or EDX differing at a return gives `EQUIV-IF-CALLERS-IGNORE`, with a scan of every caller in the
    original.

### 2.2 The six classes (labels on a proven difference)

| Class | Condition (exact text in equiv_rules.md section 3) |
|---|---|
| ENC | Same instruction, other encoding: the two `mov r, r` forms, imm8 against imm32, short against near jump. Also padding / NOP forms (`nop`, `lea r,[r]`, `lea r,[r+0]`, `mov r,r`) that one side has. Trailing padding is ignored. |
| SIB | `[a+b*1+d]` against `[b+a*1+d]`, scale 1 only |
| REG | One register bijection over the whole function makes the two lists equal. The ABI conditions are checked by the proof as observables: argument registers at entry, EAX/EDX/ECX/ST0 at every return, callee-saved registers restored, every register a callee reads compared at the call, values across calls only in callee-saved registers. |
| ORDER | An instruction moved within a block, past instructions it shares no register, flag or overlapping memory with. Memory: same base, disjoint constant offsets. Load against store through different bases only for a private frame slot (A1). |
| LAYOUT | Same CFG with blocks moved, polarity inverted with swapped targets, threaded jumps, duplicated or merged return blocks |
| COMM | `mov r,X; op r,Y` against `mov r,Y; op r,X` for add / imul / and / or / xor: same destination, same flags (the flags term of add and imul has sorted operands) |

Two more labels report what the proof needed:

- **FRAME**: the strict rerun, where the whole stack is ordinary memory, failed, so the private-frame model A1 was
  needed.
- **VALUE-FLOW**: proven, but not one of the six syntactic shapes. For example a value kept in a register on one
  side and reloaded from its argument slot on the other (`0x0045a3a0`), or renamings that are not one bijection.

### 2.3 The `0x0045a3a0` case

The brief asks for it to be reported as conditional. The tool reports `EQUIV-IF-CALLERS-IGNORE(EAX)` with the
caller list:

- `0x0045a741`: ignores;
- `0x0045a7d9`: ignores;
- `0x0045ac2a`: unknown. The scan reaches a branch it does not follow.

The verdict never becomes plain EQUIV on its own.

### 2.4 Assumptions, and why each is acceptable

- **A1, private frame.** Frame and argument slots whose address never escapes are reachable by no other pointer and
  no callee.
  - This is the guarantee C gives the compiler. The original's own code depends on it: it keeps a parameter in EBP
    where ours reloads it from the slot.
  - The strict rerun without A1 is what sets the FRAME label.
- **A1', exposed frame.** When the address `esp0+k` (k < 0) escapes, the frame from k up to the entry ESP becomes
  ordinary memory. Slots below k stay private.
  - Justification: C objects do not overlap, so an escaped object starts at k and lies inside our frame.
  - Refusals: an escape into the argument area, and an exposed range below an earlier call's ESP. Both fall back to
    the strict mode.
  - Stores into the dead frame are dropped from the memory compared at return.
  - This is the rule that admits `0x005c4ad0`. I consider it sound for compiled C. It would be wrong only for code
    that reaches a stack object through a pointer to a different object, which is undefined behaviour in C and
    which neither compiler emits.
- **A2.** EFLAGS are dead at return.
- **A3.** Both bodies start from the same machine state. That includes the frame bytes below ESP until a call could
  have overwritten them.
- **A4.** A call through a pointer is cdecl: it pops nothing.
  - A wrong guess unbalances ESP at the next return, and the return check catches it. The rule is refused when
    either side resets ESP from a register.
  - By default ECX and EDX count as read by such a call. `--relax-icall` (A4') drops that, and is reported apart.
- **A5.** For a callee whose argument count is unknown (varargs), the argument count is this side's cleanup, or else
  the pushes right before the call. The two sides must agree.

### 2.5 Excluded on purpose

- **x87.** In a region with an x87 instruction:
  - the x87 instructions must be identical: same operands, same order;
  - integer code must not cross an x87 instruction: mnemonic multisets per segment.

  `0x00556e90`, where moves are scheduled across x87 instructions, is NOT-PROVEN, and `--x87-strict` refuses any
  textual difference in such a region. The Unicorn cross-check finds no observable difference for `0x00556e90` in
  8 runs. The exclusion is policy, not a detected inequivalence.
- **Not modelled** (NOT-PROVEN `unsupported`): variable-count `rep` string operations, `scas`/`cmps`/`lods`,
  `pushf`/`std`, SSE, `lock`, segment registers other than FS, two-level jump tables.

## 3. Tests

`py -3.12 re/tools/match/test_equiv.py`: **44/44 PASS**.

**Synthetic pairs.** 38 pairs, a positive and its negatives per class:

- **ENC**: 4 positives, plus `neg_enc_imm`.
- **SIB**: 1 positive, plus `neg_sib_scale` (scale 2 is not swapped).
- **REG**: 2 positives (one keeps the renamed value across a call), and 4 negatives:
  - EAX against ECX at return gives IF(EAX,ECX), not EQUIV;
  - a register-argument callee under the swap: `call-regargs`;
  - a callee-saved register not restored;
  - the wrong register stored.
- **ORDER**: 3 positives, and 4 negatives: may-alias stores, overlapping stores, a load against a store through
  different bases, flags clobbered between `cmp` and `jl`.
- **LAYOUT**: 2 positives, plus polarity inverted without swapping the targets.
- **COMM**: 2 positives, plus `sub`.
- **FRAME**: 1 positive, plus an escaped argument slot modified by the callee.
- **x87**: 1 positive, plus 2 negatives (commuted `fmul`, a `mov` scheduled across `fld`).
- **Calls**: 3 negatives: argument, order, store before against after the call.
- **Loops**: 2 positives, plus 2 negatives (bound, stride).

**Reference rows.** All six give the verdicts in the Verdict section.

| RVA | Status | Verdict | Classes | Dynamic (Unicorn) |
|---|---|---|---|---|
| `0x00475a00` | L-115 admitted | EQUIV | SIB | no difference in 8 runs |
| `0x0045a3a0` | L-115 admitted | EQUIV-IF-CALLERS-IGNORE(EAX) | ENC, FRAME, VALUE-FLOW | (EAX ignored) |
| `0x00555360` | L-117 admitted | EQUIV-IF-CALLERS-IGNORE(EAX) | LAYOUT, VALUE-FLOW | (EAX ignored) |
| `0x0048e820` | L-118 admitted (with wrapper) | NOT-PROVEN call-regargs (EDI to `0x0048cee0`) | | differs in 8 of 8 runs: EDI `0x3f800000` against `0x10` |
| `0x005c4ad0` | held | EQUIV | FRAME, VALUE-FLOW (A1') | no difference in 8 runs |
| `0x00556e90` | held | NOT-PROVEN x87-region | | no difference in 8 runs (excluded by policy) |

## 4. Mutation test

`py -3.12 verify/equivcheck_20261111/mutate_real.py --n 25`, output in `mutation.tsv`.

**Scope.**

- The 19 proven synthetic positives.
- 25 real proven rows from `measure_v4.tsv`, taken in seed-7 order. Up to 30 mutants each, every kind represented.
  One process per pair, capped at 300 s. 21 of the 25 produced mutants: 3 hit the cap (`0x005b0bb0`, `0x004c1210`,
  `0x005b3670`), and `0x0047ba00` (a 5-byte tail jump) has no mutable site.

**Mutation kinds.** All on our body only, at instructions the proof executed:

- displacement + 4;
- immediate + 1;
- condition code negated;
- one register field of one instruction changed;
- two adjacent non-branch instructions swapped, one of them writing memory.

**Result: 644 mutants, 0 escaped.**

| Outcome | Mutants | Meaning |
|---|---|---|
| killed (NOT-PROVEN) | 558 | |
| flagged-return-register | 30 | The mutant changed a return register, and the tool says so: EQUIV became EQUIV-IF-CALLERS-IGNORE(r). Unicorn confirms that only r differs. |
| swap-proven-independent | 43 | The swapped instructions are provably independent, so EQUIV is the correct answer. |
| survived, no dynamic difference in 16 runs | 13 | Equivalent mutants (see below). |
| ESCAPED (EQUIV, but Unicorn sees a difference) | **0** | |

**The 13 survivors, read one by one.** Each is equivalent.

- **A dead frame store.** `0x00429bd0` `mov [esp+0x60], eax` became `mov [esp+0x60], ecx`. The slot is not an
  argument of the following call and is never read again.
- **An effect only on a register the verdict already lists as callers-ignore.**
  - `0x004e7e50`: `jne` became `je` between `xor eax, eax` and `mov eax, esi` returns.
  - `0x0052fe90`: `sbb eax, eax` became `sbb ecx, eax`, an EAX-only result.
  - `0x005ac540`: five mutants of its EAX/ECX loads.
  - `neg_reg_ret`.
  - `enc_jmp`: `test` before an unconditional jump.
- **A pushed value the callee does not read.** `0x005b66c0`: the callee's summary reads fewer stack arguments than
  are pushed.

**Kills per class.** A mutant counts once for every class its pair used:

| Class | killed | flagged-return-register | swap-independent | survived (equivalent) | escaped |
|---|---|---|---|---|---|
| ENC | 39 | 7 | 1 | 1 | 0 |
| SIB | 56 | 0 | 6 | 0 | 0 |
| REG | 29 | 3 | 1 | 1 | 0 |
| ORDER | 95 | 2 | 13 | 1 | 0 |
| LAYOUT | 192 | 16 | 15 | 3 | 0 |
| COMM | 15 | 0 | 0 | 0 | 0 |
| FRAME | 94 | 2 | 10 | 1 | 0 |
| VALUE-FLOW | 271 | 22 | 15 | 10 | 0 |
| byte-identical pairs | 84 | 4 | 6 | 0 | 0 |

No class let a mutant through, so every class is shippable by the brief's criterion.

**Development history.** One earlier harness build reported 2 "escapes". Both were mutants whose own verdict was
EQUIV-IF-CALLERS-IGNORE, which the harness compared against the unmutated pair's ignore list. They are now
classified as flagged-return-register. No version of the tool let a real difference through.

## 5. Dynamic cross-check (Unicorn)

**Setup.**

- One memory picture for both bodies: the original image, our patched section, a random stack, and pages mapped on
  first touch from a hash of their address.
- 16 random states per row. Half of the arguments are pointers.
- Calls leaving the body are stubbed. The stub records the target, the stack arguments and the register arguments
  per the callee summary, and returns a hash of that record.
- Compared: the stub records, every non-stack byte written, EBX/ESI/EDI/EBP/ESP, EAX/ECX/EDX (minus the listed
  callers-ignore registers), x87 registers and status.

**What it cannot see.**

- callee side effects (stubs write nothing);
- aliasing of incoming pointers with the stack (random pointers never hit it);
- paths random inputs do not reach;
- precision-control effects of real callees.

**Proven rows** (`dyn_proven.tsv`, final harness, 194 rows: 168 proofs plus 26 byte-identical):

| Outcome | Rows |
|---|---|
| at least one run, every conclusive run the same | 165 (156 of them with 8 or more conclusive runs) |
| every run inconclusive (3 s / 200,000 instructions per leg, or over 300 calls) | 29 |
| any run differs | **0** |

The 29 rows with no conclusive run are mostly RenderWare and audio list walkers: `0x004c0c20`..`0x004c1210`,
`0x004e5bf0`..`0x004e6760`, `0x005aaac0`..`0x005b3670`. Their loop bounds come from random memory. For those rows
the proof stands without a dynamic witness.

**Harness development.** Four harness defects were found and fixed on the way, and each is a harness bug, not a
tool bug:

1. Calls to a callee the source defines itself went to our own copy.
2. Summaries depended on query order. This was also a tool defect, fixed in equiv.py.
3. Jumps through a pointer into unmapped memory stopped as a fault at different stack depths.
4. Long loops were not capped.

Each "difference" they produced was read and traced to its harness cause before the fix. After the fixes, the same
rows are SAME.

**Reference rows:**

- `0x0048e820` (NOT-PROVEN) differs in 8 of 8 runs, on the EDI argument to `0x0048cee0`.
- `0x00556e90` (NOT-PROVEN, x87) shows no difference in 8 runs.

**NOT-PROVEN rows.** See section 6.4 for the sample.

## 6. Measurement

All 1,106 `REFUSED` rows of `re/analysis/match_ledger.tsv` that have a source, through the real DDK compile. Default
mode, A4 strict (`measure_v4.tsv`). Full tables in `TABLES.md`.

### 6.1 Verdicts

| | all 1,106 | ≥ 95% (127) | with `--relax-icall` (A4') |
|---|---|---|---|
| EQUIV | 107 | 34 | 105 |
| EQUIV (byte-identical today) | 26 | 19 | 25 |
| EQUIV-IF-CALLERS-IGNORE | 61 | 0 | 76 |
| NOT-PROVEN | 912 | 74 | 900 |

The `--relax-icall` column comes from the first v4 pass. That pass ran before the top-level summary memo, a change
that only affects speed, and 10 rows timed out in it against 5 in the final pass. Compare it with the 105 / 25 / 61 /
915 of that same pass: A4' adds 15 conditional rows.

The brief's "173 at 95% or more" is an older count. Today's ledger has 127 REFUSED rows at 95% or more, all of
them with a source.

**Proven rows by match percentage** (EQUIV, conditional and byte-identical, 194 rows):

| Match percentage | Rows |
|---|---|
| ≥ 95 | 53 (including the 19 byte-identical) |
| 90 to 95 | 25 |
| 80 to 90 | 51 |
| < 80 | 65 |

Instruction-level percentage says little about equivalence. `0x0047ba00` scores 0.0%: ours is a 5-byte tail jump,
the original is `push; call; add esp; ret`. `0x0045c480` scores 55%: zero stores to disjoint globals are reordered
and a `rep stosd` is moved.

### 6.2 EQUIV by class set

Each proven row counts once per class it uses. VALUE-FLOW and LAYOUT dominate.

| Class | Rows |
|---|---|
| VALUE-FLOW | 115 |
| LAYOUT | 82 |
| FRAME (A1 needed) | 59 |
| ORDER | 39 |
| REG | 24 |
| ENC | 19 |
| SIB | 8 |
| none (byte-identical) | 26 |

The full table of class sets (54 distinct sets) is in `TABLES.md`.

**Rules the proofs used** (from `rules`):

| Rule | Rows |
|---|---|
| jump threading | 58 |
| polarity inversion | 43 |
| A4 (indirect call) | 42 |
| A1' (exposed frame) | 20 |
| A5 fallback that compares every frame slot (`call-args-all`) | 13 |
| A5 (arity) | 5 |
| x87 barrier | 4 |
| jump table | 2 |

### 6.3 EQUIV-IF-CALLERS-IGNORE (61)

| Registers | Every caller ignores | Some caller reads or unknown |
|---|---|---|
| EAX | 8 | 10 |
| ECX | 5 | 10 |
| EDX, ECX | 3 | 12 |
| EAX, ECX | 1 | 6 |
| EDX | 1 | 2 |
| EAX, EDX, ECX | 1 | 1 |
| EAX, EDX | 0 | 1 |
| total | 19 | 42 |

Most `unknown` results come from a caller that returns right after the call. The scan does not follow a return into
the caller's caller.

### 6.4 NOT-PROVEN by first unexplained difference (912)

| Kind | All | ≥ 95% |
|---|---|---|
| x87-region (excluded on purpose) | 238 | 36 |
| branch-condition | 164 | 3 |
| call-args | 113 | 0 |
| call-regargs (a callee's register argument differs) | 103 | 2 |
| call-memory | 61 | 8 |
| unsupported | 56 | 8 |
| call-sequence | 32 | 1 |
| esp-unknown | 25 | 1 |
| call-target | 20 | 1 |
| switch-table / switch-index / indirect-jump | 26 | 5 |
| ret-memory | 17 | 1 |
| indirect-call-esp-reset (A4 refused) | 11 | 1 |
| unresolved-callee (a CRT name not in the symbol map, e.g. `_sprintf`) | 10 | 1 |
| timeout (240 s) | 5 | 1 |
| ret-esp (A4 guessed cdecl for a stdcall pointer) | 8 | 3 |
| other (no-annotation 5, callee-pop-unknown 4, call-x87 3, loop-without-branch 3, no-size 3, cfg-mismatch 3, ret-address 2) | 23 | 2 |
| total | 912 | 74 |

**`unsupported` by construct:**

| Construct | Rows |
|---|---|
| conditional jump out of the function | 13 |
| `rep movs` / `rep stos` with a variable count, or over 512 | 28 |
| `pushf` | 4 |
| `repne scasb` / `repe cmpsb` | 5 |
| SSE | 6 |

**Dynamic check on 452 NOT-PROVEN rows** (a sample, 8 runs each, `dyn_notproven.tsv`): 215 show an observable
difference, 209 show none, 24 are inconclusive, 4 are errors.

| First kind | Differs | No difference seen |
|---|---|---|
| x87-region | 54 | 67 |
| call-args | 34 | 23 |
| call-regargs | 29 | 8 |
| branch-condition | 22 | 13 |
| call-memory | 18 | 31 |
| callee-pop-unknown (v2 run) | 11 | 23 |

So about half of the refusals are real inequivalences. The other half are where more rules could reach, with these
caveats:

- 8 random runs do not cover the paths;
- callee side effects are not modelled by the stubs.

## 7. Functions released

**On the top-down path (326 blocking C0..C3 functions).** 39 have a proof.

- **32 EQUIV**: `0x00407be0`, `0x00408b00`, `0x0040b290`, `0x00417180`, `0x00417450`, `0x0041f770`,
  `0x00429310` (TimeTrial::Tick), `0x00429bd0`,
  `0x0042aa00`, `0x004332a0`, `0x00448220` (Frontend::PostRaceResultCamera), `0x0045c480`, `0x00475a00`,
  `0x0047cea0`, `0x00484de0`, `0x00485b30` (PoolRemove485b30), `0x0048a4e0`, `0x004938e0`, `0x00498ea0`,
  `0x004b61c0`, `0x004c18c0`, `0x004d4f00`, `0x005a6b70`, `0x005a8890`, `0x005aa7a0`, `0x005aaac0`, `0x005aab00`,
  `0x005ab410` (AudioRwsChunkTypeSeek), `0x005ace70`, `0x005ad320`, `0x005ae3a0`, `0x005ae920` (AudioPoolFree).
- **7 EQUIV-IF-CALLERS-IGNORE**: `0x00430670` (ECX), `0x00490020` (EDX, ECX), `0x004d8480` RwErrSlotWrite (EDX,
  ECX), `0x00555360` (EAX), `0x005aa7e0` (EDX, ECX), `0x005acd10` AudioFmtTableSearch (EAX, ECX), `0x005b0cf0`
  (EDX, ECX). Only `0x00555360` has every caller scanned as `ignores`. The other 6 have a caller the scan cannot
  settle.

Under the proposed rule (section 8), 33 of the 326 are released with no per-function decision (32 + `0x00555360`),
and 6 more need one.

- 24 of the 33 already meet the cross-check coverage (8 or more conclusive runs).
- The other 9 need a seeded-input run in the wiring slice: `0x00485b30`, `0x004938e0`, `0x004b61c0`, `0x005a8890`,
  `0x005aaac0`, `0x005aab00`, `0x005ab410`, `0x005ad320`, `0x005ae920`. Per-row classes and callers are in `TABLES.md` ("Released functions on the top-down path").

**Local callees.** 7 proven rows call a body their own source defines (8 callees, e.g. `0x004332a0` calls its own
`FUN_0042ac50` and `FUN_00430760`). Every one of the 8 is ledger PASS or itself proven, so none blocks an admission.

**BINDINGS.tsv stand-in rows.** Of the 91 stand-in / under-stand-in rows:

| Status | Rows |
|---|---|
| ledger PASS | 78 |
| no ledger row | 2 |
| NOT-PROVEN | 10 |
| EQUIV-IF-CALLERS-IGNORE | 1 (`0x004d8480`, EDX/ECX, callers unsettled) |

So the tool releases no stand-in row without a per-function decision.

## 8. Proposed rule text (for the decision row after L-122)

> **L-1xx (proposal).** A near-match body may run on the port axis with no per-function decision when
> `re/tools/match/equiv.py` at a recorded commit gives:
>
> 1. **EQUIV**, any classes, in default mode. Default mode means A4 strict and the x87 barrier rule. It must not use
>    `--relax-icall`.
> 2. **or EQUIV-IF-CALLERS-IGNORE(regs)**, when the caller scan says `ignores` for every call site in the original and
>    every listed register.
>
> Additional requirements:
>
> - **Local callees.** Every body listed under `local_callees` must itself be byte-matched or admitted by this rule,
>   and the port must link the name to that body.
> - **Cross-check.** The Unicorn cross-check (`equiv_dyn.py`, 16 runs) must find no difference, with at least 8
>   conclusive runs. A row with fewer conclusive runs gets a seeded-input run in the slice that wires it. That is test
>   work, not a decision. A difference is a tool bug: it stops the rule until it is fixed, and is not a per-function
>   call.
>
> For each admitted function, the record is the tool commit, the source path and flags, the verdict line (classes
> and `rules` used: A1-exposed, A4-indirect, A5-arity, switch, x87-barrier), the caller scan for a conditional
> verdict, the cross-check tally, and the TSV row. These go in an `ADMITTED.tsv` appended by the slice that wires the
> body.
>
> **The C-level does not change.** C4 stays byte match or library identity.
>
> **Still a per-function decision:**
>
> - EQUIV-IF-CALLERS-IGNORE with any caller `reads` or `unknown`;
> - any proof that needs `--relax-icall`;
> - any NOT-PROVEN row.
>
> A NOT-PROVEN row means the source needs work first. The x87 exclusion stays absolute.

## 9. Decision input for Mariano

**What the tool does.**

- It reads our compiled body and the original body side by side and proves that a caller cannot tell them apart:
  same calls with the same arguments, same memory, same registers at return.
- When it cannot prove that, it says NOT-PROVEN and names the first difference.
- It never changes a C-level.

**What it found.**

- **Proven: 168 of the 1,106 near-match rows.** 107 are equivalent outright. 61 are equivalent except for a scratch
  register at return, which matters only if a caller reads it. Another 26 rows are already byte-identical today;
  the ledger lists them as REFUSED because it predates the source.
- **On the top-down path: 39 of the 326 blocking functions.** 33 are releasable with no per-function decision.
- **0x0048e820 is not equivalent as its source stands.** The admitted port works only because of the
  `PortTop_PS3_Call0048cee0` wrapper. The tool catches the EDI argument on its own.
- **0x00555360 rests on its caller.** It is equivalent only because its one caller ignores EAX.

**What is yours to decide.**

1. **Admit the EQUIV verdicts on the port axis with no per-function decision.**
   - Applies to: any class, default mode, cross-check clean, local callees covered.
   - Recommendation: **yes**. The proof is semantic, the 44 tests and the mutation run agree, and the cross-check
     found no difference on any proven row.
2. **Admit EQUIV-IF-CALLERS-IGNORE when every caller is scanned as ignoring.**
   - That is 19 rows overall, and 1 on the top-down path (`0x00555360`).
   - Recommendation: **yes**. The other 42 stay per-function decisions.
3. **The exposed-frame rule A1'** (a local whose address is passed to a callee).
   - It admits `0x005c4ad0` and 20 rows in total.
   - Recommendation: **yes**. It relies only on C's rule that objects do not overlap. The option is to exclude
     those 20 rows.
4. **`--relax-icall` (A4': a call through a pointer reads no register).**
   - It would add 15 conditional rows.
   - Recommendation: **no**. Keep it a reported count, not a rule. No source in `re/match` declares a
     fastcall/thiscall pointer, but the original's targets cannot be checked.
5. **x87 stays excluded**, as the brief says. 238 rows stop on it, and in a sample 54 of 124 of them show a real
   observable difference. A separate row could widen it to "same x87 instructions, integer code free".

**What the tool does not do.**

- It does not shorten the path much on its own: 254 of the 326 stay NOT-PROVEN.
- About half of the refusals sampled are real differences in the source: wrong arguments, a register argument,
  call order. Those need source work, not a rule.

## 10. Evidence index

All files below are committed on `orch/EQUIV-CHECK`. Each one is small; no raw per-frame data exists for this row.

| File | Content |
|---|---|
| `measure_v4.tsv` | The verdict of every one of the 1,106 rows, default mode (A4 strict, x87 barrier). Columns: verdict, classes, regs, callers_all_ignore, first difference kind and detail, rules used, local callees. Tool = `re/tools/match/equiv.py` at this branch's commit. |
| `measure_v4_relax.tsv` | The same with `--relax-icall` |
| `TABLES.md` | `py -3.12 verify/equivcheck_20261111/crosstab.py measure_v4.tsv measure_v4_relax.tsv` |
| `blocking_c0c3.tsv` | The 326 blocking functions, recomputed from `origtop_size.py --order` on `ee3562e27`. The script rewrites `SIZE.tsv` / `order_full.txt` in `verify/portorigtop_20261109/`; those two files were restored and are unchanged on this branch. |
| `dyn_proven.tsv` | The Unicorn cross-check of every proven row (16 runs each) |
| `dyn_notproven.tsv` | The Unicorn check of a sample of 452 NOT-PROVEN rows (8 runs each). It was run with an earlier tool build, v2, on the rows that build refused. Its `v3_*` columns are from build v3. The builds differ in the x87 barrier rule, the exposed frame and the summaries. The sample shows how often a refusal is real, not the final verdicts. |
| `mutation.tsv`, `mutate_real.py` | One row per mutant. The tally per class is at the end. |

Measurement history, so the numbers can be compared:

| Build | EQUIV + conditional | What it added |
|---|---|---|
| v1 | 106 + 50 | |
| v2 | 109 + 53 | exposed frame A1', A5 arity |
| v3 | 109 + 60 | x87 barrier rule, indirect tail jumps in summaries |
| v4 first pass | 105 + 61 (10 timeouts) | order-independent summaries |
| v4 final (`measure_v4.tsv`) | 107 + 61 (5 timeouts) | a memo of top-level summary queries (speed only). Every verdict is identical to the first pass except 3 rows that timed out there and now prove EQUIV (`0x00407800`, `0x00429310`, `0x0044e0a0`; the last is byte-identical today), and 2 that timed out and now give another NOT-PROVEN kind. |

v4 is the final build. Its summaries are order-independent, which turned a few cached, order-dependent verdicts
into the conservative answer, e.g. `0x0044e190` from EQUIV to IF(EDX,ECX). v1 to v3 are not committed.
