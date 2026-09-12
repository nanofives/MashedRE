# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-12, gating-drain session) — READ THIS FIRST

Branch `race/first-frame-parity`. **Zero worktrees, zero Ghidra pool locks.** `main` is
level with HEAD (`git rev-list --left-right --count main...HEAD` → `0 0`).

**Uncertainties, with the command that derives each number:**

| measure | count | command |
|---|---:|---|
| open rows in the Active section | 3,030 | rows matching `^\| *U-[0-9]+` between `## Active uncertainties` and `## Resolved`, minus `~~`-prefixed |
| of those, **actually gating** | **0** | same set, `Blocks` cell (index 7) exactly `C2->C3` or `C3` |
| gating at session start | 46 | (was 235 → 121 on 2026-09-11, → 46, → **0**) |
| rows with a non-canonical column count | 131 | pre-existing baseline, **unchanged** by this session's 47 edits |

**The gating bucket is empty. That is the headline, and it is also the thing most
likely to be misread** — see the next section before you act on it.

## ⇒ WHAT "0 GATING" DOES AND DOES NOT MEAN

**17 rows were resolved on evidence. 29 were DE-GATED, and de-gating is not resolving.**
Every de-gated row is still OPEN, still carries its evidence and its next command, and
records the value its `Blocks` cell replaced.

The de-gate rests on `re/CONFIDENCE.md`'s own C2→C3 wording: a field must be named
**"(or is explicitly marked `[UNCERTAIN]` with the marker recorded in `UNCERTAINTIES.md`)"**.
A properly recorded row *is* the rubric's sanctioned alternative to a name, not a blocker.
Owner-approved before applying.

**The test applied to each row, one at a time:** would a byte-for-byte verbatim
transcription of the row's own function be **wrong** without this answer? For all 29 the
answer was no — they ask what something is *called* (which joint is index `0x33`, what the
six emitter pools are named, which space a matrix maps between), and the port emits the
same bytes either way.

**Exactly one row in the 46 was a genuine transcription gate, and it dissolved on
inspection** — U-5588. See the corrections below.

**Consequence for the next promoter:** nothing in `UNCERTAINTIES.md` now blocks a C2→C3.
If you hit a function you cannot transcribe, that is a *new* finding — open a new row, do
not assume an old one covers it.

## What landed

### 1. Method — two Ghidra batches plus the two self-service lanes
`decomp_pc.py` ran twice (115 functions + 27 globals, then 26 + 8), which is the whole
Ghidra cost of the session. Everything else came from `memread.py` and `disasm_fn.py` over
the anchored `original/MASHED.exe.unpatched`. **The disasm lane settled the hardest rows**,
including all four of the strongest results below.

### 2. The 17 resolutions worth remembering

| row | outcome |
|---|---|
| U-4780 | Takes **five** stack args, not three. Full ESP accounting across four pushes: reads `E+4`, `E+8`, `E+0x10`, `E+0x14`; `[E+0xc]` (Ghidra's `param_3`) is never addressed by any instruction. Both halves of the old question settle at once. |
| U-4700 | The three blocks are DirectShow GUIDs — `MEDIATYPE_Video`, `MEDIASUBTYPE_RGB24`, `FORMAT_VideoInfo` — matched at `+0x00/+0x10/+0x2c` = `AM_MEDIA_TYPE` majortype/subtype/formattype. |
| U-4713 | A **full 4x4 carrying translation**, not a 3x3 inverse-transpose for normals. Exactly four `fchs` negate the entire first output row *including* `-pos.x`; a normal matrix would not carry that. |
| U-4715 | The four matrix outputs pinned by ESP accounting: product / the `0x00496ec0` matrix / identity. `0x004ed6ba` is `jmp dword ptr [0x6187b4]`, outside every IAT range — a runtime fn-ptr, not a named import. |
| U-4702 | Vtable slot 3 of `0x005cfd70`. The class name is **proven unavailable**, not merely unfound: the word at `0x005cfd6c` is `0`, so there is no RTTI locator. |
| U-4423 | The `+0xc0` reader is `FUN_00476440`, found via the **array base's** xrefs. |
| U-4514 | The constant at `0x005ceb10` is the string `"tyre"` — the function is a dictionary lookup, not a loader. |
| U-4726 | `+0xc4` = VertexShaderVersion, `+0xcc` = PixelShaderVersion — the fields name *themselves* in the writer's own log format strings. |
| U-4583 | `FUN_0048eac0` names all six pools with string literals: `animFire`, `smoke`, `fireball`, and three `exp_cloud` sinks separated only by capacity. |

(Also U-4388, U-4504, U-4509-adjacent, U-4701, U-4709, U-4716, U-5103, U-5595, U-5654.)

### ⚠ 3. THREE CORRECTIONS TO THE EXISTING RECORD — these matter more than the wins

- **U-5588: Ghidra's "Could not recover jumptable at `0x0041dec0`. Too many branches" is a
  MISCLASSIFICATION.** `0x0041dec0` is `jmp dword ptr [eax + 0x48]` — a vtable **tail call**,
  exactly parallel to the `call dword ptr [eax + 0x48]` six bytes above it. The decomp line
  was correct and complete all along. This was the only genuine transcription gate in the
  46 and it existed only because nobody disassembled the address.
  **Generalise it: a jumptable warning is a hypothesis, not a finding.**
- **U-4313: one of the four recorded write sites does not exist.** `0x0043f8d5
  mov [ebx+8],edx` does not land on `0x007f1a1c`. EBX is never loaded with that constant in
  `0x0043f890..0x0043f8e0`; the only materialization there is `0x0043f8b2 mov eax,0x7f1a1c`
  and EAX is used read-only. A `--datarefs` record attributing a register-indirect store is
  a **candidate**, not a fact, until the register's provenance is read.
- **U-4583's cited `DAT_006668f8` is a digit transposition** of `DAT_007668f8`; the former
  has `ref_total: 0`. Cheap to check, and it had been carried for months.

### 4. NEW ROW U-9134 — the library band may be over-broad
`hooks.csv` bands `004c0c20` as `third-party-library[lua-4.0]`, but the body dereferences
`DAT_007d3ff8` (RwGlobals) and frees through slot `+0x11c`. **The band was assigned by
ADDRESS RANGE** (`0x004b4a80..0x004c4000`), so every function in the span inherited the
label with no per-function evidence — and the note's own text says `lua-5.0` while the band
says `lua-4.0`. Only this one function was checked. **The span needs auditing before the
library-skip policy is trusted there**; it may be excluding functions that are ours.
Not fixed here — `hooks.csv` is mutated only through `re-classify`.

## PICK ONE (next session)

- **A. Audit the `0x004b4a80..0x004c4000` band (U-9134).** Sweep the span for bodies that
  dereference `DAT_007d3ff8` or call RW APIs. Highest value of the three: if the band is
  over-broad, there is reachable first-party work currently invisible to every tracker.
- **B. Promote.** Nothing gates a C2→C3 any more. `promote-c3-batch` / `/promote-round`.
- **C. The carried-over owner decisions below.** None was touched this session.

## Carried over, still needing YOUR call
- **`area/frontend` is the one unmerged branch, deliberately** — a WIP checkpoint whose
  `PanelSortInit` hook (`0x00420d00`) would go live in the dev ASI unverified.
- **D-11069** — 4 duplicate-RVA rows need `hooks.csv` to express "different implementation
  per build target", which the single `file` column cannot. Schema change, affects every parser.
- **U-9087** — 4 C4 rows may need demotion; their install proof is a byte the original already has.
- **G / rubric L37** — 387 C3 rows are shadow-generatable but `re/CONFIDENCE.md` names a
  Frida CSV as the C4 evidence. Amend, or keep C4 Frida-only.

## Tooling notes that cost time to learn
- **`re/tools/memread.py <va>...`** — section, file offset, raw bytes, dword, float. It also
  catches "the address is not data at all".
- **`re/tools/disasm_fn.py <start> <end>`** — capstone over the anchored binary. **Always
  start from the containing function's entry**; cited addresses are often mid-instruction
  and capstone will decode garbage from a bad start. This lane is best at **refuting**.
- **`decomp_pc.py --datarefs`** answers "who writes this global", split into writes / reads /
  other. Register-indirect writes it reports still need the register's provenance read.
- **Batch everything.** One `decomp_pc.py` invocation costs ~30-60 s of project-open
  regardless of how many addresses you pass.
- **Writing a resolution? Do not put a pipe in the cell.** A literal `|` splits the row and
  shifts every later column. The transaction script refuses any cell containing one — it
  fired twice this session on claims quoting a bitwise OR. Spell the operator `OR`.
- **A Python script that reads text and writes text will STRIP CRLF.** It showed as all
  3,240 lines changed. Read and write **bytes**, or convert back before committing —
  `git diff --stat` is the tell.

## Standing rules that bit earlier sessions
- Shadow lane: **single-boot verdicts are unreliable**. Require two independent boots.
- Run races with `--cars 4 --hold 60`; a 1-car race never fires the contact solver.
- Never `git worktree remove --force` — use `py -3.12 scripts/diag.py wt-remove`.
- Kill only PIDs you spawned; never blanket-kill MASHED by name.
- External web is unusable on the worker account (it fabricated results). Run web on account3.

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md` first —
> **gating uncertainties are at 0, but 29 of the 46 were de-gated rather than resolved**, so
> read "WHAT 0 GATING DOES AND DOES NOT MEAN" before treating the frontier as clear. Then
> pick A (audit the `0x004b4a80..0x004c4000` library band, U-9134 — likely hides first-party
> work), B (promote; nothing gates a C2→C3 now), or C (the carried-over owner decisions).
