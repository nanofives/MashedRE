# RESULT MATCH-RACEPATH-FRESH4 (four race-path rows with no hand source), 2026-11-09

**Verdict: GREEN. 1 of 3 worked rows closed (0x004177b0, promoted to C4 by route match). Two partials kept above their
raw g2s start. 0x0040be50 not worked (handoff below).**

Branch `orch/MATCH-RACEPATH-FRESH4`, base `orch/integration` @ d09a76c1d. Every score uses `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's line-1 FLAGS, `/O2 /GS /Ob1` in every file. No `__asm`, no
naked body, no pragma. No mashedmod/ edit (AiPreTick.cpp was read only), no game run, no Ghidra write, no change to
`re/tools/match/` code. The new sources sit in `re/match/racepath/`, which nothing in `mashedmod/`, `scripts/` or
`re/tools/` references, so the default gates are unaffected (not run).

Decomp: `re/tools/decomp_pc.py --port --callees --json` on read-only pool slot `Mashed_pool0`. The script has to be run
from the main checkout copy: run from a worktree it resolves `mashed_pool/` under the worktree, which has no pool
(`Could not find project ...\.worktrees\MATCH-RACEPATH-FRESH4\mashed_pool\Mashed_pool0`). The "before" column is the raw
`g2s.py` score of that JSON. The original listings are capstone over `original/MASHED.exe.unpatched` (FRESH's
`tools/odis.py`), the constants were read from the same image with `tools/rdf.py` (new, prints a dword as hex, float and
double, and its section).

## Yield

Worked smallest first: 0x00415880, 0x004177b0, 0x00486830. 0x0040be50 was not worked (session budget).

| rva | subsystem | before (raw g2s) | after | source |
|---|---|---|---|---|
| 0x004177b0 | ai | 23.8% | 1339 B BYTE-IDENTICAL, relocheck OK | `re/match/racepath/004177b0.c` |
| 0x00415880 | ai | 28.7% | 40.3%, 889 of 1143 B differ | `re/match/racepath/00415880.c` (kept, not promoted) |
| 0x00486830 | particle | 26.4% | 46.7%, 1331 of 1671 B differ | `re/match/racepath/00486830.c` (kept, not promoted) |
| 0x0040be50 | gameplay | 22.2% | not worked | none |

0x004177b0 scores insn 98.7% (377/372) only because the Ghidra body (1338 B) stops before the `ret`. The whole-symbol
byte compare (1339 B) is identical, `relocheck.py` reports OK (93 checked, `__ftol` the one unchecked symbol), and
`c4_reclass.py batch` passes it (`c4_dryrun.tsv`).

## What moved the closed row (0x004177b0)

Each step was compiled and kept only when the semantics stayed the same (read for each one):

1. **One block for the AI globals** (23.8% raw, first hand source 69.9%, then 72.6 -> 88.4%). The per-car records
   (`0x0089a4c0` / `0x0089a4c4` / `0x0089a4c8`, stride 0x74) and `DAT_0089a368` / `36c` / `370` / `374` / `360` are
   fields of one struct at 0x0089a360, cars at +0x160, as in `re/match/s03/h4_00417180.c`. With separate symbols cl keeps
   0x0089a368 in a register across the zeroing loops. The original re-reads it (`cmp [0x89a368], ebx` at 0x00417c43).
2. **Constant 2 in EBP** (69.9 -> 72.6%). The mode-4 and mode-9 exits written as two `DAT_0089a368 = 2; return;`
   statements, as Ghidra shows them (cl tail-merges them to 0x00417cde). With one `||` statement cl promotes 1 to a
   register instead of 2.
3. **Parentheses keep two multiplies.** `((10.0f - level) * 0.1f) * 0.16666667f`: without them cl folds the two
   literals into one (0x004179da / 0x004179e0 are two `fmul`).
4. **Named float locals for a call result** (a20f: 72.6 -> 73.4% before step 1; r: 91.4 -> 91.7%). `a20f = (float)FUN_00407a20(v);` and
   `r = FUN_00408ad0(v);` give the call order and the `mov edx,[0x7f0fd0]` slot right after the call (0x004177f9).
5. **The table entry as an int local** (88.4 -> 91.4%), compared `n >= FUN_00472650(0.0f, 100.0f)`: the int copy to [esp+0xc] and
   the fild before the call (0x00417c5f..0x00417c73).
6. **A repeated compare against the .rdata float** (91.7% -> 0 B differ). Band 3 is nested under `if (d > 20.0f)`,
   and band 4 repeats `d > 20` reading 0x005ccd6c as a global. With the literal in both places cl folds the two
   compares into one and pops d early. The original compares twice, `fcom` at 0x00417bbc and `fcomp` at 0x00417be2,
   and takes the first test's false edge straight to 0x00417c15. 0x005ccd6c holds 20.0f, the value the literal has.
   The faithful oddity is kept: on one path the band variable is read uninitialised (`mov edx,[esp+0x10]` at
   0x00417b6c).

Literals read from the image: listed in the source header (22 constants, 0x005cc328 .. 0x005cd0f0).

## Not closed

- **0x00415880 (40.3%, 889 B).** Shape is complete (mode switch on the `int[][5]` table at 0x005f2fc0, 4-car scan,
  latched and unlatched paths, one float local shared by three values the original stores to [esp+0x14], the dot
  product order `pf[2]*n.z + (pf[1]*n.y + n.x*pf[0])`, `!(x < 0.2f)` for the `test ah,5 / jnp` compares, 8 literals).
  The residue is register allocation, and it shifts every register: the original keeps car*0x74 in EBP for the whole
  function (`sub [ebp+0x89a508],eax` at 0x00415893), car in ESI only before the loop, and compares the loop index
  against the stack copy of car (0x00415965). cl here keeps car in EBP and spills the offset. Tried with no change: a
  struct array, three field arrays, the AiBlock, an explicit byte offset, a timer pointer, `-=` inside the test, a
  `continue` form, unsigned car, a `me = car` copy, dy written into car's stack slot. Permuter, all kinds: 2332
  compiles to 52.5%, but every gain moved a statement after a `goto` or a `return` or swapped two calls, so all of it
  was discarded. Semantics-safe kinds only: 198 compiles, one hit, `timer <= -1` in place of `timer < 0` (49.0% insn
  but 896 B differ, and the original's `jns` is the `< 0` form). Not kept.
- **0x00486830 (46.7%, 1331 B).** Shape is complete from the listing: the 250-step loop over particle pool B (0x006fe790,
  150 x 0x7c B) and trail pool A (0x006fa5d0, 100 x 0x90 B, indexed `[i - 150]`), the inlined RwMatrixSetIdentity
  macro (chained stores, flags |= 0x20003), alpha bytes from the 0x005cf468..0x005cf478 globals, the bubble sort.
  Residuals: (1) the 8-aligned frame (`and esp,-8; sub esp,0x5e0`). Only `double dist2` produces it (48.6%), but the
  original stores dist2 as a dword at [esp+0x30], so that form is not faithful and was not kept. (2) The original
  walks six pointer IVs, ours one or two (the same blocker FRESH2 reports on 0x00412620). Separate symbols for the
  trail fields gave the src IV but 43.1%.

## Handoff: 0x0040be50 (not worked)

Raw g2s 22.2%. Decomp in `D:/MashedWT/fresh4_data/decomp.json`, listing `D:/MashedWT/fresh4_data/0040be50.dis`. What is
known from the listing (no source written):
- The slot index arrives in EAX (`mov esi, eax` at 0x0040be57). The sole caller is 0x0040c5b0
  (`re/analysis/bucket_gameplay_0040ad90_00412010/0x0040be50.md`, U-7845). Write it as a `static` function before a
  stand-in caller in the same file, as `re/match/pilot/a6_00419090.c` does.
- A 6-case jump table (`jmp [eax*4+0x40c58c]`, state = `DAT_007e9de4[i] - 3`, cases 3..8), EBX pushed after the range
  check (0x0040be92), and `car = DAT_007ea5e4[i] - 0xff00` in EDI.
- Records: `int [][8]` at 0x007ea9e4 (index shl 5), `float [][16]` at 0x007ec9e4 (index shl 6). Case 3 reads
  0x007ec9e8.. as the position and 0x007ec9e4 as a timer; case 4 reads 0x007ec9e4.. as the position.
- Three stack floats are initialised before the switch (0, -0.0005f `0xba03126f`, 0) and passed as `&local` to
  FUN_0048aa50.
- Case 3 contains the same inlined RwMatrixSetIdentity macro as 0x00486830.

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, approximate compile count, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run (1 PASS, 0 refused) |
| `relocheck.tsv` | relocheck of the closed source |
| `tools/` | `tryv.py`, `dblgreedy.py`, `odis.py` (copied from MATCH-RACEPATH-FRESH, unchanged), `rdf.py` (new) |

Raw data kept off the repo (`D:/MashedWT/fresh4_data/`):

| path | SHA-256 |
|---|---|
| `decomp.json` (decomp JSON, 4 rows) | `e05fbb70480e37fe69ef0301d14e5211ab518893feaf1418e806949e2a1a4a40` |
| `g2s/results.tsv` (raw g2s scores, the "before" column) | `7ef68815eabef356f54184da860de2c110576dc404e2431cc3e65eab817f7334` |
| `*.dis` (capstone listings), `try/`, `try/var/` (58 variants), `perm_15880.*`, `perm2_15880.*` (permuter runs) | not cited row by row |
