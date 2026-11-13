# equiv.py rules (EQUIV-CHECK, 2026-11-11)

This file states exactly what `re/tools/match/equiv.py` proves and what it assumes. A rule that is not written here
is not applied. The tool never changes a C-level. Its verdict is a port-axis fact.

## 1. What a verdict means

`EQUIV` means the two bodies are proven to produce the same observables for every entry state, under assumptions
A1 to A7 below. The bodies are our DDK compile of a `// MATCH:` source and `original/MASHED.exe.unpatched` at the
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

`NOT-PROVEN budget: <detail>` (EQUIV-5441F0-TIMEOUT, 2026-11-13) means the proof ran out of work budget before a
verdict: `--max-steps` (default 250000 instructions executed symbolically per proof run) or `--max-terms` (default
1000000 distinct terms per function). The detail names the side and instruction where it stopped, the region pair
last entered, the pair visits and the term count. It is a refusal, never evidence of a difference. The admitted rows
use at most 3792 steps and 2670 terms (2026-11-13). `--stats` prints both counts per function. Separately, every
rendered text (call texts, the x87 rule's instruction texts, details) expands at most `--show-budget` term nodes
(default 100000); past it a term renders as `<#id>`, its hash-cons id, so the text stays a function of the term.
Before this, `show()` re-entered at depth 0 through every tuple argument (`repr` -> `T.__repr__`), which is
exponential on a shared term DAG: 0x005441f0 spent 50 min in one `call` text over 897 terms.

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
- **no-return** (tag `no-return`): a call to a callee whose summary reaches no `ret` ends the path there, and nothing
  after the call is compared on either side. Tagged whenever any path ends this way, also when other paths return
  (since PROV-DIVERGED-18, 2026-11-11). A summary walk that runs into the next function-table entry is not a
  no-return callee: it gives no summary (`r11`). Before that fix, CRT acos `0x004a3384` (Ghidra size 9, it continues
  at the next table entry `0x004a338d`) was summarised no-return, so was `0x00408b00`, and `0x00448220` came out
  EQUIV with every instruction after its first `0x00408b00` call (`0x00448475`) unchecked, the tag absent. The
  replay diverged at tick 1116 (`verify/provinsitu_20261111/RESULT_PROV-DIVERGED-18.md`).
  - **Reported per path** (EQUIV-NORETURN-RECHECK, 2026-11-12): every path that ends at a no-return callee or at a
    trap (`int3`, `hlt`, `ud2`) is listed in the verdict's `halts` field (JSON; the text line appends
    `unchecked after no-return: ...`; `equiv_measure.py` writes it as the last TSV column). Each entry names the
    side, the call site, the callee and the summary's reason (`nr_why`): the callee's own path ends, nested callees'
    reasons inline, for example `orig 0x00448475 call 0x408b00: no-return, call 0x004a3384 @0x00408bcf (...)`.
  - **Tail jump to an unknown callee** (`r12`): a summary whose only path ends are tail jumps to callees with no
    summary (no pop count, not no-return) is not no-return either. Before, such a body reached no `ret`, so it was
    summarised no-return, the same failure as `r11`. It now gives an unknown pop count.
- **A7, the stack is not inside the image** (`A7-stack-image`, EQUIV-NORETURN-RECHECK, 2026-11-12). An address
  `esp0 + k` and a constant address whose whole access lies inside the mapped image of `MASHED.exe` never overlap,
  so a load of a global passes stores into the frame (and the reverse), also when the frame is exposed (A1'). The
  thread stack is not mapped inside the PE image. Before, a load of `DAT_00773408` scheduled before or after the
  zero fill of an escaped `char line[64]` gave two different terms (`0x00499400`). Constants outside the image
  (heap pointers written as literals) are not covered.
- **Summary precision** (EQUIV-NORETURN-RECHECK, 2026-11-12). Both make a callee's live-in set smaller, so fewer
  register arguments are compared. Each one only removes a register the callee provably does not read:
  - `or r, -1` and `and r, 0` write a constant (result and flags), so `r` is not read. Before, they counted as a read:
    false live-ins (`0x0040e187`, `0x0040e18f`: EDI / EBP for `0x00441c80`) and false `call-regargs`.
  - Frame alignment `and esp, imm` under a frame pointer: ESP moves to a virtual base, so a register saved by a
    later `push` is still a save, not a read. Arguments are then reached through EBP only, and ESP must come back
    through EBP (`mov esp, ebp` / `leave`): a `ret` with ESP not at the entry is still `r7`. Before, `0x00498a00`
    (`push ebp; mov ebp, esp; and esp, -8; ...; push esi; push edi`) had false live-ins ESI / EDI.

## 5. Excluded on purpose

- **x87.** In any region that contains an x87 instruction:
  - The x87 instructions must be the same instructions, with the same operand text, in the same order. x87
    instruction order is never admitted (L-126). This part did not change with L-147.
  - No instruction may cross a call in such a region (NOT-PROVEN `x87-region`).
  - The region must hold the same integer mnemonic multiset on both sides (NOT-PROVEN `x87-region`).
  - **Integer motion across x87 (EQUIV-X87-INTEGER-MOTION, L-147 (a), 2026-11-13).** An integer instruction may sit
    on different sides of an x87 instruction in ours and in the original only when it is proven independent of every
    x87 instruction it crosses. Otherwise NOT-PROVEN `x87-motion`, naming the instruction, the crossed x87
    instruction and the dependency. The check:
    - *Which instructions moved.* Barriers (x87 instructions and calls) cut the region into segments. In each
      segment, an instruction whose exact text is not in the other side's same segment (multiset difference) is a
      moved instruction. An instruction with the same text in the same segment on both sides counts as staying.
    - *What it crosses.* Its partner is one of the other side's moved instructions with the same mnemonic. It is
      checked against every barrier from its own segment to the farthest segment holding such a candidate, so a
      wrong pairing only widens the check.
    - *Independence,* each crossed x87 instruction against the moved instruction, on that side's own state:
      - registers: the moved instruction writes no register the x87 instruction reads or writes, and reads none it
        writes. Registers are full width (AX / AH / AL count as EAX), implicit operands included (capstone
        `regs_access`): `fnstsw ax` writes EAX, `fcomi` / `fcomip` / `fucomi` / `fucomip` write EFLAGS, `sahf`
        reads EAX and writes EFLAGS, `push` / `pop` read and write ESP, address registers are read. EFLAGS is one
        register, so any flag writer (`add`, `sub`, `xor`, `test`, `cmp`) crossing `fcomi` is refused, and a flag
        writer or reader crossing an x87 instruction that does not touch EFLAGS (`fld`, `fadd`, `fstp`) is allowed.
      - memory: when either access writes, the two addresses must be provably disjoint (`disjoint`, the same test
        the store chain uses). Address terms are values computed on that side's state at that instruction, so the
        comparison holds whatever ran in between. Read-read is allowed. In default mode an access to a private
        frame or argument slot (A1, routed as memread / memwrite route it, exposed slots excluded) and an access
        that is not one never alias, the same split the proof makes. In the strict rerun no slot is private, so a
        move that needs A1 is reported with FRAME. The x87 direction comes from the
        mnemonic (`fst` / `fstp` / `fist*` / `fnstsw m` / `fnstcw` / `fnstenv` / `fnsave` write, the rest read),
        not from capstone's access flag, which reports `fstp m32` as a read. `fnstenv` / `fnsave` / `frstor` /
        `fldenv` count 108 bytes. String instructions, `pushad` / `popad` / `pushfd` / `popfd` / `leave` /
        `enter` and any instruction whose address the model cannot form count as touching all memory.
    - The values are still checked by the proof. The rule only decides whether a move may be left to it.
    Rule tag `x87-motion` in the verdict's `rules` when some instruction crossed an x87 instruction. Admission is
    still ADMITTED.tsv, never C4 by itself (L-128).
  - `fcmov*` (an x87 read of EFLAGS the x87 term does not take in) is NOT-PROVEN `unsupported`. `fcompi` /
    `fucompi` (capstone 5's spelling of the popping compares) now set the flags term like `fcomi` (before
    2026-11-13 they left the flags untouched in the model).
  - With `--x87-strict`, the whole region must be identical text.

  `0x00556e90` (frozen 97.5% source) is NOT-PROVEN `x87-motion`: ours loads `mov esi,[esp+0x18]` (slot esp0+0xc)
  after `fstp [esp+0x14]` writes that same slot in the original order, a real dependency. Before L-147 it was
  NOT-PROVEN `x87-region`. The two rows L-147 named, re-run on their branches' sources with this tool
  (2026-11-13): `0x00536e90` (orch/MATCH-EQUIV-536E90) EQUIV [ORDER, FRAME], `0x004c4a50`
  (orch/MATCH-NM-5441F0-4C4A50) EQUIV [LAYOUT, VALUE-FLOW]. Their admission is those rows' work.
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

- 57 synthetic pairs from `equiv_tests/cases.asm`, positives and negatives per class. The 11 A6 / MXCSR pairs
  (EQUIV-CALLEE-POPS): `com_ident` (byte-identical control), `com_stdcall` (a `__stdcall` COM call checked by an
  early return, then a cdecl call, then a second COM call), `com_cdecl` (caller cleanup), `com_loop` (a COM call in
  a loop), `com_ambiguous` (two calls the equations cannot separate, proven once per solution), `mxcsr`; negatives
  `neg_com_argcount` (wrong prototype argument count), `neg_com_order` (swapped call order), `neg_com_conv` (cdecl
  against `__stdcall`), `neg_com_solution` (ours right under one pop solution only), `neg_mxcsr`. The 9 x87
  motion pairs (EQUIV-X87-INTEGER-MOTION): `x87_move_indep` (load crosses `fld`, was `neg_x87_sched`),
  `x87_move_rr` (load of [G1] crosses `fld [G1]`, read-read), `x87_move_frame` (frame-slot load crosses `fstp`
  through a pointer, A1, FRAME); negatives `neg_x87_fnstsw` (`mov eax` crosses `fnstsw ax`), `neg_x87_sahf`
  (`sahf` crosses `fnstsw ax`), `neg_x87_alias` (store to [G3] crosses `fstp [G3]`), `neg_x87_eflags`
  (`xor` crosses `fcomip`), `neg_x87_slot` (argument-slot load crosses `fstp` into that slot), all
  `x87-motion`, and `neg_x87_call` (load crosses a call in an x87 region, `x87-region`);
- the seven reference rows (`0x005441f0`, frozen copy `equiv_tests/refs/005441f0_held.c`: must end NOT-PROVEN, it
  hung before EQUIV-5441F0-TIMEOUT);
- two budget checks: `show_budget` (64 levels of a doubled tuple render under the budget, a small term renders as
  before) and `step_budget` (`reg_call` with a 3-step budget ends `NOT-PROVEN budget` with its place);
- about the reference rows: `0x0048e820` runs on a frozen copy of the pre-EQUIV-FU-48E820 source
  (`equiv_tests/refs/0048e820_held.c`), because the live source is byte-identical since 58cc241be and no longer
  shows the EDI defect the row tests.

`py -3.12 re/tools/match/equiv_mutate.py` mutates our side of every proven pair. Every mutant must come out
NOT-PROVEN, or EQUIV-IF-CALLERS-IGNORE when it changed a return register. A mutant the tool proves EQUIV goes to the
Unicorn cross-check, `equiv_dyn.py`.
