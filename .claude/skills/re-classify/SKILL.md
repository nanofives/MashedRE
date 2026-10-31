---
name: re-classify
description: Classify a Mashed function against the C0..C4 confidence rubric. Use whenever you finish reverse-engineering work on a function — it walks the rubric, updates hooks.csv, drops STUBS/UNCERTAINTIES rows, and refuses promotions that lack evidence. Triggers on "classify", "promote", "what's the confidence of", "mark as done", "C3", "C4", or after any /re or hook-author session.
---

# re-classify — function confidence pipeline

Single source of truth for moving a function up (or down) the confidence ladder. Read `re/CONFIDENCE.md` for the rubric; this skill is the **enforcement**.

## When to invoke

- After `/re` finished decomposing a function — promotes from C0/C1 to whatever the evidence supports.
- After `hook-author` has written a reimplementation — candidate for C3.
- After `diff-original` produced a green CSV — candidate for C4.
- When asked "what's the confidence of …" or "is this done?"
- When demoting after a regression.

## Inputs

The skill needs:
- **RVA** of the function (mandatory).
- **Subsystem** (mandatory; one of: boot, audio, ai, vehicle, track, render, hud, frontend, input, save, net, util, unknown).
- **Target confidence** the user is asking for (C0..C4). The skill validates evidence; never just stamps the request.
- **Evidence pointers** — analysis note path, source file path, Frida diff CSV path, etc.

If anything is missing, the skill **asks before acting**. No silent inference of evidence locations.

## Workflow

For each promotion request, the skill **must** check the corresponding gate from `re/CONFIDENCE.md` and refuse if not satisfied:

| Target | Gate (verbatim from rubric) |
|--------|------------------------------|
| C1 | function has name/strings/xrefs evidence; subsystem assigned |
| C2 | decomp read end-to-end; `re/analysis/<sub>/<func>.md` exists with shape (args/return/reads/writes), every const/offset cited; NO-GUESSING |
| C3 | semantics stated; reimplementation lives at `mashedmod/src/mashed_re/...`; build is clean; hooked via `RH_ScopedInstall`; runtime-toggleable; at least one caller AND one callee at C2+ |
| C4 | `diff-original` CSV produced; clean diff vs original; no stubs in this function's body |
| C4 (static) | `match` or `libid` evidence, see "Evidence types `match` and `libid`" below; no stubs in the body |

For each refusal, the skill states **exactly which gate criterion failed** and what evidence would unblock it.

## Side effects on success

When the skill promotes a function, it must **all in one transaction**:

1. **Update `hooks.csv`** — find or insert the row for this RVA; update `name`, `subsystem`, `confidence`, `status`, `file`, `scenario`, `frida_diff`, `notes`.
2. **Sweep `[UNCERTAIN]` markers** in the function's analysis note. For each unfiled marker, write a new row in `UNCERTAINTIES.md` (next `U-NNNN`), and replace the inline marker with `[UNCERTAIN U-NNNN]`. Refuse a C3 promotion if any `semantic` or `structural` uncertainty is unresolved.
3. **Sweep stubs** in the function's source. For each callee that resolves to a stub helper, ensure a row exists in `STUBS.md`. Refuse C4 promotion if any stub remains in the function's body.
4. **Prepend to `re/analysis/CHANGELOG.md`** — one line: `YYYY-MM-DD  RVA  name  oldC->newC  evidence`.
   Insert it **immediately below the `<!-- ENTRIES -->` marker** (the file is newest-first).
   NEVER rewrite or truncate the file: `2dee9c67` destroyed 477 entries by writing where it
   should have inserted. If a past entry was wrong, add a correcting entry — do not edit history.
5. **Demotion** is the same flow in reverse: lower the row, prepend to the changelog with an `oldC<-newC` arrow.

## Close or rewrite a DEFERRED row (no function C-level moves)

Use when a `DEFERRED.md` Active row's re-pickup condition is met, the work is done or judged
not applicable, or the row text is stale. Run the helper, never hand-edit the row:

```
py -3.12 re\tools\deferred_close.py close --reason "<why>" --evidence "<RESULT path>" D-NNNN [D-NNNN ...]
py -3.12 re\tools\deferred_close.py rewrite --id D-NNNN --why "<cell>" --when "<cell>" --tag "<cell>" --reason "<why>"
```

- **close** strikes the ID and title cells (`~~D-NNNN~~`), sets "Why deferred" to
  `RESOLVED <date>: <reason> (evidence: <path>)`, sets "Re-pickup when" to `—`, and prepends one
  line per row to `re/analysis/CHANGELOG.md` below `<!-- ENTRIES -->`
  (`date  D-NNNN  DEFERRED  Active->struck  reason`). Struck rows stay in the Active table, as before.
- **rewrite** replaces only the cells you pass on a row that stays Active, and logs it the same way.
- Gate: a `--reason` is mandatory, and a close needs evidence (a RESULT path or hooks.csv fact)
  unless the reason says the row is not applicable. The helper refuses a missing or already struck ID.
- Only the `## Active` section is touched. The `## Cleared` table is not edited by this operation.
- After a batch, recount Active rows (`grep -c "^| D-" DEFERRED.md` inside the Active section) and fix
  any count quoted in `ROADMAP.md`.

## Add a DEFERRED row (no function C-level moves)

Use when work is deliberately parked and has no Active row yet (L-48, 2026-10-28). Run the helper,
never hand-edit `DEFERRED.md`:

```
py -3.12 re\tools\deferred_add.py add --title "<title>" --why "<why deferred>" --when "<observable re-pickup condition>" --tag "<phase tag>" --reason "<changelog reason>"
py -3.12 re\tools\deferred_add.py add ... --rvas-tsv <tsv> --rvas-id <id>   # census group: appends the TSV rows' RVAs to the title
```

- **add** takes the next monotonic ID (highest `D-NNNN` anywhere in the file, struck and Cleared rows
  included, plus one), appends the row after the last row of the `## Active` table, and prepends
  `date  D-NNNN  DEFERRED  added to Active  reason` to `re/analysis/CHANGELOG.md` below `<!-- ENTRIES -->`.
- `--rvas-tsv/--rvas-id` (columns `id` and `rva` by default, `--rvas-id-col/--rvas-col` to change) appends
  `(N rows): 0x<rva>, ...` in address order, so one census group is one row.
- Gate: title, why, when, tag and reason are all mandatory. The helper refuses a vague re-pickup
  condition (`later`, `someday`, `tbd`), a title already present in Active, a cell with ` | ` or a newline,
  and an `--rvas-id` that matches no TSV row. `--dry-run` prints the ID without writing.
- Line endings of both files are preserved. Only the `## Active` section grows.
- After a batch, recount Active rows and fix any count quoted in `ROADMAP.md`.

## Resolve an UNCERTAINTIES row (no function C-level moves)

Use when a cause is found and fixed (or the question is answered) for an `UNCERTAINTIES.md` Active
row (HB-RECLASS-UROW, 2026-10-28). Run the helper, never hand-edit the row:

```
py -3.12 re\tools\uncertainty_resolve.py resolve --reason "<why>" --evidence "<repo-relative RESULT path>" [--partial] U-NNNN [U-NNNN ...]
```

- **resolve** strikes the Type cell (`~~type~~`, the U-9201 / U-9205 convention), prefixes the Statement
  cell with `**RESOLVED <date>** (<evidence>): <reason>. Prior statement: <old text>`, sets "Evidence missing"
  and "Path to resolution" to `n/a`, "Blocks" to `nothing`, and prepends
  `date  U-NNNN  UNCERTAINTIES  Active->struck  <reason> (evidence: <path>)` to
  `re/analysis/CHANGELOG.md` below `<!-- ENTRIES -->`. The row stays in the Active table and keeps 7 cells.
- `--partial` writes `RESOLVED (PARTIAL)` and leaves "Blocks" alone, for a residual that was split into a new row
  (file the new row first).
- Gate: `--reason` and `--evidence` are mandatory and the evidence file must exist in the repo. The helper refuses
  an unknown ID, an already struck ID (so a second run is a no-op that writes nothing), a row without 7 cells,
  and a cell containing ` | ` or a newline. Nothing is written unless every ID passes.
- Only `## Active uncertainties` is touched. Line endings are preserved per line (the file mixes LF and CRLF).
- Self-test: `py -3.12 -m pytest re/tools/tests/test_uncertainty_resolve.py` (fake repo root, real trackers untouched).

## Evidence types `match` and `libid` (C4 by byte match, 2026-10-31)

A C4 request may carry evidence type `match` or `libid` instead of a `diff-original` CSV. Gate text: `re/CONFIDENCE.md`,
"C4 routes by byte match". Required fields; refuse and name the missing one if any is absent:

| field | `match` | `libid` |
|---|---|---|
| tool command | the exact `match.py` command line | the exact `libmatch.py` command line |
| relocheck result | `re/tools/match/relocheck.py <src>` line for the RVA: `OK`, plus the names it lists as unchecked | n/a |
| compiler build | `cl` 13.00.9176 / `c2` 13.00.9178 (XP DDK) | same, or the DDK `libcmt.lib` member's `@comp.id` |
| flags | recorded flags (game code `/O2 /GS /Ob1`, C) | recorded flags (Lua `/O2 /Ob1 /G5`, qhull `/O2 /G5`) |
| matched source path + sha | reconstructed source file + sha256 | vendored library source file + sha256 |
| port body file | the TU in `mashed_re.exe` (and the `.asi` TU where hooked) that compiles that same source | the TU that compiles that vendored source |

Checks before stamping C4:
- `match`: the tool reports 100% (byte-identical after relocation masking) for that RVA **and** relocheck reports OK
  (a 100% source with relocheck BAD is refused: it can store to the wrong global); the port body file is the
  matched source or includes it (`exe_file == file`, lint clean, physics TU in `x87_tus.txt`); the merge-train gates
  are unchanged after the swap (cite the gate hashes). A matched source the port does not compile, or a naked
  inline-asm transcription, is refused.
- `match`, callerless body (no exe caller, `exe-callers=0`): **NOT C4** (L-68, Mariano 2026-10-31). Count the exe
  callers of the swapped-in body yourself. With 0, keep the swap and the match evidence, leave the level where it is,
  and write `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)` in `notes`. No
  `c4=match` token. A merged `match` C4 found with `exe-callers=0` is demoted to its previous level with that note.
  Rubric, "Callerless bodies".
- `libid`: status `identical` and not `ambiguous` in the libmatch TSV; the port body file compiles that vendored
  source. CRT rows never take `libid` C4: see "CRT rows (L-67)" below.
- The Write-set leg is not asked for on these routes.
- `hooks.csv` `notes` gets `c4=match` or `c4=libid` plus the source sha; the CHANGELOG evidence column cites the RESULT.

### CRT rows (L-67, Mariano 2026-10-31): waive, never promote

The port links the platform CRT by design, so CRT rows are waived at library level and come out of the P-DoD count.
They are **not C4** and keep their confidence level. Exe link unchanged. For each CRT row (MATCH-LIB runs this
over the CRT rows and the 197 CRT functions filed under other subsystems):

1. `status` = `waived-platform-crt`. The confidence cell is not changed.
2. `notes` starts `identified, waived: platform CRT (L-67)` when libmatch reports the row `identical` to a DDK
   `libcmt.lib` member, followed by the provenance: libmatch command, member name, `@comp.id`. An `ambiguous` row
   lists every candidate member. A row that is not identical starts `waived: platform CRT (L-67)`.
3. A CRT function filed under another subsystem (boot, d3dx9-shader-compiler, render, util, save, audio, frontend) is
   refiled to the CRT bucket its address range already uses (`third-party-library[msvc-crt-main]`, `-tail` or
   `[msvc-crt]`). Refuse the refile when the libmatch row is not `identical` or `ambiguous`: the subsystem then has
   no evidence, and the row stays where it is.
4. Changelog line per row: `date  RVA  name  Cn->Cn  waived-platform-crt (L-67) evidence: <RESULT>`.

Lua and qhull rows are not waived: they take `libid` C4 when the port compiles the vendored source.

Run it with the helper, never by hand (added MATCH-LIB 2026-10-31):

```
py -3.12 re\tools\libid_reclass.py crt-waive --tsv <libmatch tsv> --lib <main>\tools\vc7ddk\lib\wxp\i386\libcmt.lib --evidence <RESULT>
```

It applies steps 1-4 to every `third-party-library[msvc-crt*` row and every row at a TSV `identical-unlisted` CRT
address (the refile), reads each member's `@comp.id` from `--lib`, and skips rows already `waived-platform-crt`
(a second run writes nothing). Bucket by address: `0x004a0000..0x004b4000` -> `-main`, `0x005c0000..0x005d0000` ->
`-tail`, anything else -> `[msvc-crt]`.

### Library rows from libid: add missing rows, refile misfiled ones (no C4)

A libmatch TSV also lists library functions that are identical at an address with **no** hooks.csv row, or at a row
filed under a game subsystem. Helper (same file, same idempotence, one CHANGELOG line per row):

```
py -3.12 re\tools\libid_reclass.py add-rows   --tsv <tsv> --library lua-4.0|qhull-2002.1 --not-c4 "<why not libid C4>" --evidence <RESULT>
py -3.12 re\tools\libid_reclass.py add-rows   --tsv <tsv> --library msvc-crt --lib <libcmt.lib> --evidence <RESULT>
py -3.12 re\tools\libid_reclass.py refile-lib --tsv <tsv> --library lua-4.0|qhull-2002.1 --evidence <RESULT>
```

- **add-rows** appends a **C1** row per address (C1 gate: the identity names the function, the library fixes the
  subsystem). Lua / qhull: status `mapped`, notes `C1 by libid: identical <obj> <fn> (<flags>, <compiler>; <tsv>)`
  plus the mandatory `not C4: <why>`. CRT: status `waived-platform-crt` with the L-67 notes. A name the bytes do not
  determine (several functions identical there, MASM local labels, C++ mangled names) becomes `FUN_<rva>` with every
  candidate in notes. CHANGELOG `(none)->C1`.
- **refile-lib** moves a Lua / qhull row filed under a game subsystem to the library subsystem when its address is
  identical to exactly one library function. Confidence, status and file are kept; notes get the identity first.
- Neither operation promotes. A libid C4 still needs the gate above (the port compiles that vendored source with the
  recorded flags).
- Self-test: `py -3.12 -m pytest re/tools/tests/test_libid_reclass.py` (fake repo root, real trackers untouched).

## CSV row format reminder

```
rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes
00401234,CFoo::Bar,vehicle,C3,impl,mashedmod/src/mashed_re/Vehicle/CFoo.cpp,,pending,calls 00405678 (CFoo::Update)
```

RVA is hex without `0x`. Empty cells stay empty (not `na`).

## Anti-patterns the skill must reject

- "Promote to C3 because the user said so" without evidence pointers.
- "Promote to C4 because the build passes" — that's not the gate.
- Editing `hooks.csv` outside this skill.
- Filing an `UNCERTAINTIES.md` row without a path-to-resolution.
- Filing a `STUBS.md` row without the inline `// STUB S-NNNN` comment in source.

## Output to user

Always finish by printing:
- Old confidence → new confidence
- New tracker row IDs (U-NNNN, S-NNNN) created during the transaction
- Any open uncertainties or stubs still attached to this function (so the user knows what's left)
