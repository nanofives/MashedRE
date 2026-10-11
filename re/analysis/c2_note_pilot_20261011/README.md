# Mechanical C2 plate generator: pilot (2026-10-11)

Plan item #5 of `do-you-think-we-foamy-pelican.md`: port SimGolf `re/tools/c2_note.py` + `xref.py` to Mashed,
pilot on 20 C1 rows, compare against independent witnesses, report defect classes. **No tracker was edited.**
The plates under `plates/` are pilot output, not promotion evidence yet. Promotion goes through `re-classify`
after review.

## What was built

| File | Role |
|---|---|
| `re/tools/ghidra_scripts/FuncFactsPC.java` | One headless pass over every function: name, calling convention, prototype, stack purge, thunk target, body ranges, references to the entry (call, jump and data). Takes an optional list of VAs and defines a function **transiently** at each one Ghidra has none for (read-only clone, nothing persisted). |
| `re/tools/xref.py` | Runs `FuncFactsPC.java` on a pool slot (acquire, `-readOnly -noanalysis`, release), caches `log/ghidra_funcfacts.jsonl`, builds the capstone call graph over `original/MASHED.exe.unpatched` (SHA-256 anchor checked). Passes every hooks.csv RVA as the transient-create list. |
| `re/tools/c2_note.py` | The generator. Emits the scribe plate schema: frontmatter (`rva`, `name`, `size_bytes`, `confidence_target`, `subsystem_observed`, `callees_depth1`, `callers_noted`, `opened_in_slot`, `session_date`, `generator`) + `## Mechanical description` / `## Constants` / `## Uncertainties` / `## Stubs encountered`. |
| `re/tools/c2_note_check.py` | Pilot instrument. Checks each plate against the Ghidra decompilation (callees, globals, hex constants, float literals) and against a hand-written C2 plate (hex tokens, `FUN_` callees), and re-runs the lint. |
| `re/tools/ghidra_scripts/DecompPC.java` | Bug fix found by the pilot (G4 below), same defect as in the new script. |

`## Mechanical description` subsections: signature and calling convention, reads, writes, `lea` address
computations, branches and other flag consumers, signedness of arithmetic, callees, callers.

- **Signature**: `ret N`, then registers read before any write in the straight-line entry block (all six GPRs, so AL/EAX inputs outside MSVC conventions show up), then incoming stack arguments with esp depth tracked over the CFG. Those facts select a convention from a fixed rule table. Ghidra's stored prototype is quoted beside it.
- **Reads/writes**: every memory operand, with its width, its x87/SSE type (`float32`, `int32`, control word) and any sign or zero extension. Stack operands are relabelled entry-relative (`arg1 (entry esp+0x8)`, `local entry esp-0x40`).
- **Flag consumers**: every jcc, setcc, cmovcc, sbb/adc and loop, with its signedness class, the flags it reads, and the instruction that set them. x87 compares are traced through `fnstsw`/`sahf` back to the `fcom*`.
- **Callees**: incremental thunks and `jmp [IAT]` stubs are resolved. `mov reg,[IAT]; call reg` resolves to the import. Jump tables are enumerated from the file bytes.
- **Callers**: taken from Ghidra's references and split into calls, jumps and non-call data references. The data references are how the Lua callbacks are reached: `push <fn>` in `0x00440bc0`.
- **Constants**: immediates (signed decimal, plus the float32 reading of the bits), absolute operands with their file bytes (section, read-only vs writable, BSS tail flagged as having no file bytes), and constants that have no operand (`fldz`, `fld1`, `xorps x,x`).

The plates never contain a `Purpose` section, a name or a role. `lint()` refuses hedge words and any Purpose header in the generated prose. Backtick spans are quoted data and are exempt. Files lacking the generator marker are never overwritten.

## Pilot set

- 20 C1 rows (`pilot_rvas.txt`, first 20 lines):
  - 14 first-party rows, drawn at random (seed 20261011) from the 27 non-library C1 rows. All are Lua-binding callbacks plus the registrar `0x004714f0`.
  - 6 library rows, one per library family: `0x0053ccc0` renderware, `0x0057f940` qhull, `0x00559ba0` RW-Physics, `0x005ca500` SEH funclet, `0x0050570a` d3dx9-psgp (1,368 bytes, SSE), `0x005ca2d0` `__aullshr`.
- 5 C2 rows that already have hand-written per-RVA plates (`hand_c2_plates.tsv`): `0x005be450`, `0x0045cc80`, `0x005b3e60`, `0x0048e180`, `0x005b8be0`.

Witness 1 is the Ghidra decompilation of all 25 rows (`decomp_witness.json`, `decomp_pc --create --callees` on `Mashed_pool0`). A scribe-transcriber plate is a transcription of that same decompilation, so this witness stands in for "what the scribe would produce" without spending an agent run per row. Witness 2 is the five hand plates.

## Result (final run, `check_output.txt`)

- Lint: clean on 25/25. No Purpose section, no hedge word.
- Callees: every callee the decompiler lists appears in the plate, for all 25 rows. Imports included.
- 18/25 plates have zero missing witness items. The other 7 have 21 items between them, and every one is classified below. None is a generator defect.
- Manual read against the disassembly: `0x0047b500`, `0x004917c0`, `0x0053ccc0` and `0x005be450`. Every esp-depth label, arg/local label and access direction checked out after fix G1.
- Robustness dry run over all 821 C1 rows (into scratch, not committed): 821 plates, 0 crashes, 0 lint failures. 351/821 carry at least one `## Uncertainties` line. The most frequent are an indirect `call [reg+X]` target, and an esp depth that was not determined after an indirect or import call.

## Defect classes

### Found in the generator during the pilot, fixed (re-verified on the final run)

| # | Class | Witness | Fix |
|---|---|---|---|
| G1 | **Access direction wrong for stores.** Capstone 5 reports `fstp`/`fst`/`fistp m32`/`fnstcw` stores, `movups`/`movq` stores and `cmpxchg` as READ-only. `fstp qword ptr [esp]` at `0x0047b594` was listed under Reads. | manual read of `0x0047b500` | `mem_access()`: a direction table from instruction semantics. Capstone's `access` is no longer used. Probed on 33 forms. |
| G2 | **Field offsets reached only through `lea` were omitted** (sub-object pointers such as `lea ecx,[esi+0x340]` at `0x005be49e`, and table bases such as `lea esi,[edx+0x690a30]` at `0x00471501`). | hand plate `0x005be450`, decomp `0x004714f0` | New "Address computations (`lea`)" subsection. Image-range displacements also go to Constants. |
| G3 | **Constants with no operand were missing** (`xorps xmm0,xmm0` at `0x00505764` gives the decompiler's `0.0`; `fldz`/`fld1`/... likewise). | decomp `0x0050570a` | Listed under "Constants with no operand". |
| G4 | **Transient `createFunction` gave 1-byte bodies** when called before disassembly (`0x00449710`, `0x0044ca40`, `0x004714f0`, `0x004496a0`). This hit `FuncFactsPC.java` and the existing `DecompPC.java --create` alike: sizes came out as 1 and callee lists as empty. | facts dump + decomp JSON | Disassemble first in both scripts. Re-measured sizes: 108 / 36 / 60 / 111. The decomp witness was re-captured after the fix. |
| G5 | `mov edi,[IAT CloseHandle]; call edi` reported as an unresolved indirect call. | hand plate `0x005be450` | `reg_source()`: resolves when the register's last write in the same straight-line block is a load from an IAT slot. |
| G6 | Cosmetic: an empty "Absolute-address operands" header, and `?` as the name of a callee with no Ghidra function. | reading | The header is suppressed; the raw address is used instead. |

### Residual witness items: not generator defects (`check_output.txt`)

| # | Class | Instances |
|---|---|---|
| R1 | **Decompiler-composite constants.** The decompiler prints a constant that no instruction carries. The plate is instruction-level, so it carries the operands instead. | `0x0045cc80` `==0x3ff03 \|\| ==0x3ff04` is `sub eax,0x3ff03` (`0x0045cce6`) plus an unsigned compare. `0x004714f0` stride `0x21` is `imul edx,edx,0x84` (`0x004714fa`). `0x0047b500` `DAT_0086ee28`/`DAT_00871108`/`0x944`/`0x117` are `add ecx,0x86ece0` (`0x0047b547`) + `imul 0x2510`/`0x45c` + `[ecx+0x2428]`. `0x0048e180` `pfVar2[0xd]` is `[edi+0x34]`, and `0xff <` is `cmp esi,0x100` (`0x0048e1a0`). `0x0050570a` `[0xb]/[0xe]/[0xf]` are `+0x2c/+0x38/+0x3c`. `__aullshr` `0x3f <` is the `0x40` compare. |
| R2 | **Hand-plate prose tokens.** Derived or cross-reference values the hand author wrote, which are not operands. | `0x0045cc80` "stride 0x20". `0x0048e180` sibling `0x0048df60`, cited as "same pattern" and not a callee. |
| R3 | **Hand-plate citation error found by the generator.** | `0x005b8be0` hand plate cites a read at `0x005b8bf2`, which is not an instruction boundary. The reads are at `0x005b8bf1` (`mov ecx,[esi+0x120]`) and `0x005b8beb`. |

### Known limits (stated in each plate's `## Uncertainties`, not hidden)

- **Callee cleanup unknown** after indirect and import calls, so the esp depth is lost on that path. It is recovered at the next join from the other incoming path, and the plate lists that join explicitly.
- **No-return callees** are decided by one rule: a body with no `ret` whose last instruction is a call. Fall-through is not followed after them, and each case is listed.
- **Register inputs.** A register first read outside the straight-line entry block is listed as "address order only" (`__aullshr` eax/edx sit behind the `cl` test).
- **`__thiscall` vs one-register `__fastcall`** cannot be separated from the bytes. The plate says so instead of picking one.
- **Ghidra's stored prototypes are empty** (`undefined FUN_x()`, cc `unknown`) for most of the master. Signature facts come from the bytes, and the prototype is only quoted.
- **The plate transcribes the disassembly, not the decompiler output.** C-level expressions (R1) have to be recombined by the reader.

## Decisions needed before a batch

1. **Rubric wording.** `re/CONFIDENCE.md` C2 says "The decomp has been read and mechanically transcribed: every memory access, every branch, every callee is enumerated." These plates enumerate all three exhaustively, but from the disassembly. Confirm instruction-level plates satisfy C2.
2. **Library rows.** 794 of the 821 C1 rows are `third-party-library[*]`. 564 of those 794 carry the note "kept-C1 (not hand-plated)" from the 2026-06-04 reclass sweep. The library-skip ruling (2026-06-15) excludes library code from **C2→C3**, and says library rows are "kept C2", so C1→C2 is not barred. It adds no port value either. Bookkeeping only.
3. **Missing Ghidra functions.** The 27 first-party C1 rows have **no function in the master project**. Their plates rest on a transient `createFunction` in the read-only clone (each plate says so). Making them stick is a master write (`CreateMissedFunctions.java`), and that is a user decision.

## Proposed batch (not run)

- **Batch A (recommended): the 27 first-party C1 rows** (gameplay 11, track 9, particle 6, vehicle 1). These are the 14 from the pilot plus the other 13 Lua callbacks of the same shape.
  1. Write the plates to `re/analysis/c2gen/<subsystem>/<rva>.md` with `--c1`, filtered to the non-library rows.
  2. Capture a decomp witness for the same rows and require `c2_note_check.py` to report zero callee misses and lint clean.
  3. Hand the TSV to `re-classify` for C1→C2.
- **Batch B (optional, decision 2): the 794 library rows**, same pipeline. One headless decomp witness covers all of them in one pool session.
