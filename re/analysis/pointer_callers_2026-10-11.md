# Pointer / installer caller recovery, 2026-10-11

Plan item #2 (`plans/do-you-think-we-foamy-pelican.md`). Port of SimGolf `re/tools/pointer_callers.py`.
Candidates only: no tracker was edited and nothing was promoted.

## Files

| File | What it holds |
|---|---|
| `re/tools/pointer_callers.py` | The scanner (raw image bytes + capstone), the `--precision` gate, and `--refresh` |
| `re/tools/ghidra_scripts/RefGraphPC.java` | Headless export of function ranges plus every memory reference (read-only pool slot, ~13 s) |
| `re/tools/pointer_callers_report.py` | Gate and mutator counts |
| `pointer_callers_2026-10-11.tsv` | 1,951 rows over 1,134 targets: `target_rva, installer_site_rva, kind, target_level`, then name, installer and slot detail |
| `pointer_callers_gate_candidates_2026-10-11.tsv` | The 142 held C2 rows with their qualifying installers |

Inputs: `original/MASHED.exe.unpatched`, with its SHA-256 checked against the anchor, plus the `RefGraphPC` cache in
`log/refgraph/` (Mashed_pool0, 6,313 functions, 135,700 refs).

## Kinds (rows)

| Kind | Rows | What it is |
|---|---:|---|
| vtable | 410 | Slot of a table that a `mov dword ptr [reg], tbl` stores (a constructor's vptr) |
| store_mem | 385 | `mov [mem], fn` |
| callback_arg | 372 | `push fn` followed by a call that is not a registration |
| fn_table | 217 | Slot of a table that code references in some other way |
| rw_register | 180 | `push fn` followed by a call to RwPluginRegistryAddPlugin `0x004d7de0` or one of its wrappers (`0x004e7d40` RpAtomicRegisterPlugin, ...) |
| load_reg | 87 | `mov reg, fn` or `lea` |
| ptr_call / ptr_load | 75 / 6 | Global function pointer, used by `call/jmp [g]` or `mov reg, [g]` |
| data_orphan / text_orphan | 196 / 23 | No installer site found (an unreferenced table, or code outside every Ghidra function) |

## Precision gate

**Witness.** Ghidra's own reference database. The scanner never reads it.
- A code row passes when Ghidra holds `site -> target`.
- A table row needs two edges: `slot -> target` and `site -> table start`.

**Held-out set.** Targets that Ghidra already holds a non-flow reference to. For these, the installer is already
known independently.

**Result: 1,688 / 1,696 = 99.53%** (gate is ≥95%).

| Kind | Confirmed | Rows | Precision |
|---|---:|---:|---:|
| callback_arg | 372 | 372 | 100% |
| rw_register | 180 | 180 | 100% |
| store_mem | 385 | 385 | 100% |
| load_reg | 84 | 84 | 100% |
| ptr_load | 6 | 6 | 100% |
| vtable | 407 | 409 | 99.51% |
| fn_table | 212 | 217 | 97.70% |
| ptr_call | 42 | 43 | 97.67% |

The 8 misses are table rows whose start-reference Ghidra did not record. They are listed by `--precision`.

**Rows behind the gate gains.** All 445 are confirmed on both edges.

**Hand audit.** 32 random rows plus 12 gain rows, checked by disassembly at the installer site: 44/44 are real
installs.

**Residual risk.** Mashed has no RTTI (the dword before each vtable is not a COL pointer). The boundary between two
adjacent vtables is therefore visible only through code references. A table start that no code references would
merge into the previous table. Against that:
- The sibling vtables `0x005cf9e8`, `0x005cfd70` and `0x005d0eb0` share the prefix `0049dc20, 004a1100, 004a1120`.
- All three end at slot 47.

A merge artefact would not repeat the same length across three independently referenced tables. The risk is not
measured beyond that.

## Anti-island gate (caller half only)

The current caller set comes from Ghidra `getCallingFunctions`, rebuilt from refs.tsv. It already contains every
function with *any* reference to the entry, so Ghidra-recognised `push fn` installers are counted today. A row
qualifies under the `caller_screen.py` rule.

| Measure | Count |
|---|---:|
| C2 rows | 3,883 |
| Held by the caller half | 652 (403 with no Ghidra caller at all) |
| Held rows with any installer row | 190 |
| **Held rows gaining a qualifying (C2+ or named) installer** | **142** (vtable 83, fn_table 64) |

Every gain comes from a table kind, which is what Ghidra's caller set cannot see.

**Open: rubric decision.** Counting an installer as the "caller" of the CONFIDENCE.md L23/L25 half is a rubric
decision. The callee half was not evaluated.

## Mutators (`plans/mutators.txt`, `write_surface.walk` depth 3)

**1,125 of 1,446 are indirect.** b77f6b42 recorded 1,126; hooks.csv starts moved since.

**(a) Literal count.** 92 of the 1,125 gain an installer row themselves. That does **not** unblock them. Their
blocker is an indirect site on the CALLEE side of their tree, and an installer of the mutator says nothing about
that.

**(b) Indirect sites by class.** 9,211 in total:

| Class | Sites |
|---|---:|
| `call [reg+disp]` | 6,579 |
| IAT import | 1,092 |
| `call reg` | 817 |
| switch `jmp [reg*4+tbl]` | 515 |
| `[abs]` mapped by this TSV (ptr_known) | 188 |
| Other `[abs]` | 20 |

**Followable now.** Of the 1,125, these are blocked only by sites that can now be followed statically:

| Blocked only by | Mutators |
|---|---:|
| switch | 25 |
| switch or ptr_known | 32 |
| ...of which need this TSV | 7 |
| switch, ptr_known or IAT | 101 |

IAT calls stay a blocker because an import writes through its arguments.

`write_surface.py` counts a switch jump as indirect dispatch. 25 mutators are blocked by nothing else, so that is a
classifier artefact independent of this tool.

**ptr_known caveat.** ptr_known maps a global's *initial* value only. A `store_mem` to the same global can replace it
at run time, so the target set needs that check before a snapshot set relies on it.

## Next uses
- Feed the 142 candidates to `caller_screen.py`'s callee half, after the rubric ruling on installers.
- Teach `write_surface.py` that `jmp [reg*4 + tbl]` is a switch (25 mutators), and to resolve `[abs]` sites from this
  TSV once the store_mem check above is in (7 more).

## Follow-up, 2026-10-11 (user rulings applied)

**Rubric.** An installer clause was added to `re/CONFIDENCE.md` after the L27 indirect-dispatch block. It is
C2→C3 only, the A/B evidence is still required, and it accepts `vtable` / `fn_table` rows whose boundary verdict is
`resolved`.

**Boundary test.** `pointer_callers.boundaries()` writes a new `boundary` column in the TSV:
- The first pass flagged 171 vtable rows. 168 of them were one artefact: two consecutive `RwInitNullStub`
  `0x0045b350` slots (a shared empty virtual) at slot 18 of `0x005cf9e8` / `0x005cfd70` / `0x005d0eb0` /
  `0x005d0dc0`, matching a referenced table that starts with the same two stubs. The remaining 3 sat behind a NULL
  slot of `0x005de68c`.
- With the pair required to be non-NULL and distinct, the test flags 0 rows.
- A hidden table with a novel prefix stays undetectable. The worst-case slot-0-only reading keeps 18 of the 142.

**write_surface.py.** `switch_table()` stops counting `jmp [reg*4 + tbl]` as indirect dispatch when the table's first
entry lands back in the same body. Two more changes go with it: a switch no longer ends the body, and decoding stops
at the table when it lies inside the body.
- Depth 3: **1,125 → 1,100** blocked mutators. Exactly 25 left the set and none entered it.
- Self-test: unchanged at 6/8 windows.
- 34 `[reg*4 + tbl]` sites remain indirect. Their tables do not point back into the body.
- 7 mutators are now blocked only by global function pointers that this TSV maps.

**Callee half for the 142.** Every one is satisfied, so none is blocking:

| Basis | Count | Requirement |
|---|---:|---|
| C2 callee | 87 | none beyond the A/B |
| Import | 18 | none beyond the A/B |
| C3 callee | 11 | none beyond the A/B |
| Leaf | 17 | L24: the A/B must cover a non-trivial domain |
| Indirect-only | 9 | L27: the A/B must exercise the dispatch |

The `split`, `callee_half` and `callee_basis` columns are in `pointer_callers_gate_candidates_2026-10-11.tsv`.
