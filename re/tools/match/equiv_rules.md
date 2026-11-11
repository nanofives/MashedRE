# equiv.py rules (EQUIV-CHECK, 2026-11-11)

This file states exactly what `re/tools/match/equiv.py` proves and what it assumes. A rule that is not written here
is not applied. The tool never changes a C-level. Its verdict is a port-axis fact.

## 1. What a verdict means

`EQUIV` means the two bodies are proven to produce the same observables for every entry state, under assumptions
A1 to A5 below. The bodies are our DDK compile of a `// MATCH:` source and `original/MASHED.exe.unpatched` at the
RVA. The observables, all compared as values:

- Every call, in order: the target, every stack argument the callee reads, every register the callee reads, the
  non-private memory, and the x87 state.
- Memory at every return. Stores into the function's own dead frame are excluded.
- EBX, ESI, EDI, EBP, ESP, the x87 state, the return address and the `ret` pop count, at every return.
- EAX, EDX and ECX at every return.
- The outcome of every conditional branch, and the order in which calls and returns happen along every path.

`EQUIV-IF-CALLERS-IGNORE(r...)` means the same proof holds, except that the listed registers can differ at a return.
The verdict lists every call site of the RVA in the original and scans each one. The result is `ignores` when the
caller writes the register before it reads it, `reads` when it reads it, and `unknown` when the scan reaches a
branch it does not follow, a return or an indirect call. This verdict never becomes plain `EQUIV` on its own.

`NOT-PROVEN <kind>: <detail>` names the first difference, or the first construct, that the rules below do not cover.

## 2. How the proof works (the part every class relies on)

- **Terms.** Every register, flag, stack slot, memory state and x87 state is a hash-consed term over the shared
  entry symbols. Two equal terms denote equal values. Each canonicalisation is a 32-bit machine identity:
  - linear sums with wrap-around;
  - sorted operands for `and`, `or`, `xor`, `mul` and the high half of a product;
  - constant folding;
  - sub-register extract and insert;
  - comparison predicates in one normal form (`a > b` is written `b < a`, and `x - y == 0` is written `x == y`).
- **Regions.** From a pair of program points, each side runs symbolically until a conditional branch, an indirect
  jump or a return. Unconditional jumps are followed. A conditional branch whose condition folds to a constant is
  followed if its target is new.
- **Product exploration.** At a conditional branch the two conditions must be the same term. In that case the
  successors pair taken with taken. They may instead be negations of each other, and then the successors pair
  crosswise. A jump table pairs entry k with entry k. The index terms must be equal, and the table length is ours:
  our compiler's bound check precedes it, and the paths are paired branch by branch.
- **Joins.** Joins and loop heads use an equality partition. Optimistic value numbering is refined until it is
  inductive. A value that differs between paths becomes a fresh join symbol, which is shared by the two sides only
  when they are equal on every incoming edge.
- **Stack pointer.** ESP must be a known offset from the entry ESP everywhere.
- **Memory.** Memory is a chain of stores. A load skips a store only when the two addresses have the same symbolic
  base and constant offsets whose byte ranges do not overlap. Two adjacent stores are put in a canonical order only
  under that same condition. Loads from read-only sections of the image fold to their bytes, and so do loads from
  read-only sections of our object. The import address table does not fold.
- **Relocations.** These resolve to addresses as `relocheck.py` resolves them:
  - `MGLOBAL`, `FUN_`/`DAT_`/`PTR_`... names map to the address in the name;
  - jump thunks map to their final target;
  - CRT, import and RenderWare names map through `equiv_symmap.tsv`, which `equiv_symmap.py` builds from the
    byte-identical rows of `match_ledger.tsv` (a name with two addresses there is dropped);
  - our own read-only data used as a value resolves only when the aligned original operand points at equal bytes.

  An unresolved name gives NOT-PROVEN as soon as it matters.
- **Local callees.** A callee is identified by its name, so a call to `FUN_00430760` is a call to the original's
  0x00430760. The verdict then holds for the port only if the port links that name to an equivalent body. When the
  source itself defines the callee, the verdict lists it under `local_callees`. An admission must cover those
  bodies too: each one byte-matched, or EQUIV on its own row.
- **Calls.**
  - A callee's facts come from a walk of its original body:
    - the pop count (every `ret` agrees);
    - the stack arguments it reads;
    - the registers it reads before writing them. A push of an undefined register counts as a save, unless the slot
      is read, passed or popped into another register.
    - whether it writes EAX on every path;
    - whether it returns at all.
  - After a call:
    - EAX, ECX and EDX are terms of the call record. EAX also depends on its old value when the callee does not
      always write it.
    - Memory and x87 state become terms of the call record.
    - The callee's argument slots become terms of the call record.
    - Our frame below ESP becomes this side's own garbage.
    - Callee-saved registers keep their values. This is sound even for a callee that breaks the convention, because
      both sides then receive the same function of the same inputs.

## 3. The named classes

The proof does not use the classes. After an `EQUIV`, an attribution pass labels the difference by aligning the
two instruction lists. The classes say what kind of difference was accepted. They are what a decision can admit
or exclude.

| Class | Exact condition |
|---|---|
| **ENC** | Same instruction, other encoding: the two `mov r, r` forms, imm8 against imm32, short against near jumps. Also padding and NOP forms that one side has and the other lacks (`nop`, `lea r, [r]`, `lea r, [r+0]`, `mov r, r`). Trailing padding after the last instruction is ignored. |
| **SIB** | A memory operand `[a + b*1 + d]` against `[b + a*1 + d]`: base and index swapped, scale 1. A scale other than 1 is never swapped. |
| **REG** | The two instruction lists become equal under one register renaming that is a bijection over the whole function. The proof checks these ABI conditions as observables, not as part of the label: argument registers at entry (an entry register read on one side is the same entry symbol on the other); EAX, EDX, ECX and ST0 at every return; callee-saved registers restored on both sides; every register a callee reads, compared at each call (the callee-summary live-in set, which is how `0x0048cee0`'s EDI argument is caught); values kept across a call only in callee-saved registers. |
| **ORDER** | An instruction moved within a block. A store moves only past another store or load with the same base and disjoint constant offsets. A register instruction moves only when no register or flag it defines is used or redefined by the instruction it crosses. Both conditions follow from the term equality; they are not separate checks. A load and a store through different base registers never pass each other, except a private frame slot (A1). |
| **LAYOUT** | The same control-flow graph with blocks in another order. Also: branch polarity inverted with the targets swapped, an unconditional jump threaded, a return block duplicated or merged. |
| **COMM** | `mov r, X ; op r, Y` against `mov r, Y ; op r, X`, with op one of add, imul, and, or, xor. The destination is the same and the flags are the same: the flags term of add and imul has sorted operands, and and/or/xor flags depend only on the result. |
| **FRAME** | The proof needed the private-frame model (A1). The strict rerun failed: in that rerun the whole stack is ordinary memory, with no assumption. |
| **VALUE-FLOW** | Proven, but none of the six syntactic classes explains the difference. Typical cases: a value kept in a register on one side and reloaded from its stack slot on the other (`0x0045a3a0`), several register renamings that are not one bijection, a different spill slot. |

An `EQUIV` with no class means the two bodies are byte-identical today. The ledger row is older than the source.

## 4. Assumptions (each one is reported in the verdict's `rules` field when the proof used it)

- **A1, private frame.**
  - Scope: our frame slots (below the entry ESP) and our argument slots (above it) whose address never escapes.
  - Statement: no other pointer and no callee can reach those slots.
  - Escape: a stack address is stored to memory, passed to a callee, returned, or hidden behind a join.
  - Justification: C gives the compiler the same guarantee. Both bodies were produced by that compiler, under that
    guarantee, so the original already relies on it.
- **A1', exposed frame.**
  - Trigger: a frame address `esp0 + k` (k < 0) escapes, or an indexed frame access starts at k.
  - Effect: the frame from k up to the entry ESP becomes ordinary memory.
  - What stays private: slots below k.
  - Justification: C objects do not overlap. An escaped object starts at k and lies below the entry ESP.
  - Refusals (strict fallback): an escape into the argument area (k ≥ 0), and an exposed range that reaches below
    the ESP of an earlier call on that path.
  - Dead stores: stores into the exposed frame are dropped from the memory compared at return, because that storage
    is dead after the return.
- **A2, flags are dead at return.** No MSVC-compiled caller reads EFLAGS after a call.
- **A3, entry machine state is shared.** Both bodies start from the same registers, stack and memory. This includes
  the frame bytes below the entry ESP: they keep their entry content until a call could have written them.
- **A4, calls through a pointer pop nothing (cdecl).** Since EQUIV-CALLEE-POPS (2026-11-11) this is the fallback
  for a call A6 gives no count for (A6 found no solution set, or the call is not paired with one of the original's).
  - Check: a wrong guess leaves ESP unbalanced at every later return, and the return check catches it. The rule is
    refused (`indirect-call-esp-reset`) when either side restores ESP from another register.
  - Registers: by default ECX and EDX count as read by such a call, so they must be equal.
  - `--relax-icall` (A4'): assumes they are not read. It is reported as a separate count, never as the default.
- **A6, the original keeps MSVC stack discipline** (EQUIV-CALLEE-POPS, 2026-11-11). Applies only to calls through a
  pointer (a vtable slot `call [ecx+0x6c]`, a function-pointer global, `call esi`). Direct calls and import calls
  keep their counts from the callee summary and the import decoration.
  - Statement: in the original's body ESP has one entry-relative offset at each instruction whatever path reaches
    it, ESP is at the entry offset at every `ret` and every tail jump, ESP is never above the entry offset, and a
    callee pops only slots inside this frame. This is the frame-pointer-omitted code MSVC emits. A body that writes
    ESP other than by push, pop, call, ret and add/sub imm gets no A6 (it is the `indirect-call-esp-reset` case).
  - Solving (`pin_pops`): ESP is walked over the original's control-flow graph as a linear form `k + sum P_i`, one
    unknown P_i per call through a pointer. The statement gives one equation per join and per return or tail jump.
    The tool enumerates every solution with each P_i a multiple of 4 in `[0, -k_i]` (`k_i` = the constant part of
    the ESP offset at the call) and ESP at or below the entry offset everywhere. The real pop counts are one of
    these solutions.
  - One solution (rule `A6-pinned-pop`): the proof runs once with it. Several (`A6-pop-solution`): the proof runs
    once per solution and the verdict is the weakest one. More than `POP_SOLUTIONS_MAX` (32) solutions, or more
    than 2^12 candidate assignments, gives `NOT-PROVEN callee-pop-ambiguous`. Several solutions happen when one
    path holds two such calls and no join or return separates them: a cdecl call with a deferred `add esp` and a
    `__stdcall` call read the same as the reverse.
  - Ours (`A6-paired-pop`): our call through a pointer takes the count of the original's call at the same index in
    the same region. The count is part of the call record, and the targets must be the same term, so both sides
    call the same function. The copied count is therefore that function's real count whenever the original's is.
    Ours needs no discipline assumption: a wrong typing in our source (a `__stdcall` COM method declared cdecl)
    shows up as an unbalanced ESP at our return (`ret-esp`) or as different argument slots.
  - Not used: the source's prototypes. The count is a fact of the original's bytes plus A6, never of our typing.
  - Callee summaries (`summary()`, the facts of an original callee) still use A4 for the calls through a pointer
    inside the callee. A callee that makes a `__stdcall` COM call therefore has no summary (`r7`): every register
    counts as read at a call to it, and its argument count comes from A5.
- **A5, arity of a callee whose argument count is unknown** (varargs, or a callee body the walk cannot follow).
  - Counted: the bytes this side's cleanup releases right after the call, or else the pushes right before the call
    in the same region.
  - Mismatch: if the two sides count differently, the call records differ (NOT-PROVEN).
  - Otherwise: when neither count exists, every frame slot between ESP and the entry ESP is compared.

## 5. Excluded on purpose

- **x87.** In any region that contains an x87 instruction:
  - The x87 instructions must be the same instructions, with the same operand text, in the same order.
  - No other instruction may cross an x87 instruction: the instructions between two x87 instructions, and between
    an x87 instruction and a call, must be the same mnemonic multiset on both sides.
  - With `--x87-strict`, the whole region must be identical text.

  `0x00556e90`, where moves are scheduled across x87 instructions, is NOT-PROVEN under both rules.
- **Not modelled (NOT-PROVEN `unsupported`):**
  - `rep` string operations with a variable count;
  - `scas`, `cmps`, `lods`;
  - `pushf`, `std`;
  - SSE, except `stmxcsr` / `ldmxcsr` (since EQUIV-CALLEE-POPS). MXCSR is part of the floating-point state term:
    `stmxcsr` stores a function of that term, and `ldmxcsr` makes the term a function of the old term and the
    loaded word. No assumption is added. Their order relative to x87 instructions shows up in the term;
    the x87 region rule does not apply to them.
  - `lock`;
  - segment registers other than FS (FS:[k] is plain memory);
  - a conditional jump out of the function;
  - two-level jump tables.

## 6. Tests

`py -3.12 re/tools/match/test_equiv.py` runs the following, one `[N/M]` line each:

- 49 synthetic pairs from `equiv_tests/cases.asm`, positives and negatives per class. The 11 A6 / MXCSR pairs
  (EQUIV-CALLEE-POPS): `com_ident` (byte-identical control), `com_stdcall` (a `__stdcall` COM call checked by an
  early return, then a cdecl call, then a second COM call), `com_cdecl` (caller cleanup), `com_loop` (a COM call in
  a loop), `com_ambiguous` (two calls the equations cannot separate, proven once per solution), `mxcsr`; negatives
  `neg_com_argcount` (wrong prototype argument count), `neg_com_order` (swapped call order), `neg_com_conv` (cdecl
  against `__stdcall`), `neg_com_solution` (ours right under one pop solution only), `neg_mxcsr`;
- the six reference rows.

`py -3.12 re/tools/match/equiv_mutate.py` mutates our side of every proven pair. Every mutant must come out
NOT-PROVEN, or EQUIV-IF-CALLERS-IGNORE when it changed a return register. A mutant the tool proves EQUIV goes to the
Unicorn cross-check, `equiv_dyn.py`.
