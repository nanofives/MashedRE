# RESULT EQUIV-ADMIT-1 (T0-TOOLS, run 37)

Row: L-126, admit the 33 top-down blocking functions that `re/tools/match/equiv.py` releases with no per-function
decision, run the seeded cross-check for the 9 with fewer than 8 conclusive runs, start `ADMITTED.tsv`, bind them on
the knob path. Branch `orch/EQUIV-ADMIT-1` off `orch/integration` 778b8b91d.

## Verdict

**GREEN.**

- All 33 rows are admitted in `re/match/ADMITTED.tsv`, the first L-126 record.
- `equiv.py` at tool commit be32d7f80 reproduces every verdict and class set of `measure_v4.tsv`: 32 EQUIV, plus
  `0x00555360` EQUIV-IF-CALLERS-IGNORE(EAX) with its 1 caller `0x00552b79` scanned `ignores`.
- **Cross-check:** 0 differences in 33 x 2 x 16 runs. Every row has 8 or more conclusive runs in at least one mode.
  The 9 rows short of coverage now have 9 to 16 conclusive seeded runs each.
- **Port axis:** admitted bodies count as runnable in `origtop_size.py`.
  - The blocking C0..C3 set on the top-down path drops from 326 to 293.
  - Rows runnable with no new family bound go from 1065 to 1105.
  - Rows runnable with every family bound go from 1236 to 1281.
- **Knob path:** no exe code changed.
  - `0x00475a00` is already bound (its caller `0x0040bbb0` is matched).
  - `0x00555360` is bound by NEARMATCH-555360, which waits on L-129.
  - The other 31 have no compiled caller on the path today, so there is nothing to swap. Each gets bound when its
    caller lands (section 4).
- **Checks:** both builds pass, `ddkobj.py check` gives 541 IDENTICAL, and the knob boot exits rc 0 after 3000 frames
  on both exes.
- **C-levels:** unchanged, nothing becomes C4 (L-126, L-128).

## 1. Files

| File | What |
|---|---|
| `re/match/ADMITTED.tsv` | The L-126 record, 33 rows (columns in section 3) |
| `re/tools/match/equiv_seeds.py` | Seed builders for 6 RVAs (`0x00485b30`, `0x004b61c0`, `0x005aaac0`, `0x005aab00`, `0x005ab410`, `0x005ae920`) |
| `re/tools/match/equiv_dyn.py` | Adds `--seeded` / `--seed`. Fixes the stub pop count of `call [IAT]`. An inconclusive run now reports where it stopped |
| `verify/portorigtop_20261109/origtop_size.py` | Reads `re/match/ADMITTED.tsv` (class `match`). Adds `--no-admitted` and `--out <dir>` |
| `verify/equivadmit_20261111/gen_admitted.py` | Writes `ADMITTED.tsv` from the evidence below. Refuses on any verdict mismatch, any difference, or fewer than 8 conclusive runs |
| `verify/equivadmit_20261111/rerun33.sh`, `equiv33.log` | `equiv.py` re-run per row at be32d7f80 |
| `dyn33_default.{tsv,log}`, `dyn33_seeded.{tsv,log}` | Unicorn cross-check of the 33 rows, 16 runs per mode |
| `seeded9_in.tsv`, `seeded9_s1.{tsv,log}` | First seeded pass over the 9 rows (before the seed builders) |
| `seeded_sensitivity.{py,tsv,log}`, `seeded_coverage.tsv` | Mutants of the original bodies and instruction coverage under the seeds |
| `s12_00475a00.log` | `equiv.py`, cross-check and `match.py` on the source the port binds for `0x00475a00` |
| `size_before/`, `size_after/` | `origtop_size.py --order` with `--no-admitted` and with the record |

## 2. The 9 rows short of cross-check coverage

`RESULT_EQUIV-CHECK.md` section 7 lists 9 of the 33 rows with fewer than 8 conclusive runs in 16:
`0x00485b30`, `0x004938e0`, `0x004b61c0`, `0x005a8890`, `0x005aaac0`, `0x005aab00`, `0x005ab410`, `0x005ad320`,
`0x005ae920`.

**What made them inconclusive.**

- **Uniform random pages.** These bodies walk circular lists whose end is the list head, or loop on a callee result,
  or take a count from memory. Random bytes never close a list, and a stub's hash return is never 0.
- **A harness defect, `0x005a8890`.** The original stubbed `call dword ptr [IAT slot]` with the pop count of the
  slot's file value, which is 0. `WaitForSingleObject` and `ReleaseSemaphore` are `__stdcall` (8 and 12 bytes), so
  every run reached the `ret` 20 bytes low and returned into stack data.
  - **Fix:** a call whose bytes are `FF 15 <slot>`, with the slot a relocated `__imp_` name, now pops the decoration's
    byte count and records the import by name.
  - **Effect:** `0x005a8890` goes from 0 to 16 conclusive runs in the default mode as well.

**Seeded mode (`--seeded`).**

- **Pages.** A page is zero except one dword in 8: a count 0..7, a pointer to another heap page, or a hash word.
- **Stubs.** A stub returns EAX = 0 when the first byte of the record hash is a multiple of 4.
- **Seed builders.** Six RVAs get a builder that writes well-formed inputs, read from each body's source and cited in
  `equiv_seeds.py`:
  - circular lists through the head (`0x005aaac0` on the global `0x007dccf0`, `0x005aab00`, `0x005ae920`);
  - a doubly linked pool (`0x00485b30`);
  - a geometry with small counts (`0x004b61c0`);
  - for `0x005ab410`, the 20-byte header that callee `0x005ab380` writes through its second argument. The bytes come
    from the record hash, so both bodies see the same values when they make the same call.
- **The comparison does not change.** It is still stub records, non-stack writes, registers and x87 state.

**Result for the 9** (`dyn33_seeded.tsv`, 16 runs):

| RVA | default (same / inc / diff) | seeded |
|---|---|---|
| `0x00485b30` | 7 / 9 / 0 | 16 / 0 / 0 |
| `0x004938e0` | 1 / 15 / 0 | 16 / 0 / 0 |
| `0x004b61c0` | 2 / 14 / 0 | 16 / 0 / 0 |
| `0x005a8890` | 16 / 0 / 0 (after the IAT fix) | 16 / 0 / 0 |
| `0x005aaac0` | 0 / 16 / 0 | 16 / 0 / 0 |
| `0x005aab00` | 0 / 16 / 0 | 16 / 0 / 0 |
| `0x005ab410` | 0 / 16 / 0 | 9 / 7 / 0 |
| `0x005ad320` | 0 / 16 / 0 | 16 / 0 / 0 |
| `0x005ae920` | 6 / 10 / 0 | 16 / 0 / 0 |

**Can the seeded runs see a difference?** `seeded_sensitivity.py` mutates the original body inside the Unicorn picture
only: one displacement + 4 or one immediate + 1, up to 8 per row. It then runs the seeded check of our body against
the mutant.

- **Detection.** 34 of 66 mutants are seen as DIFFERS. All 8 are seen in `0x00485b30` and all 8 in `0x005ae920`.
- **Coverage of the original body** (`seeded_coverage.tsv`, instructions hit / instructions in the body range, padding
  included):

  | RVA | Hit / in range |
  |---|---|
  | `0x00485b30` | 60/77 |
  | `0x004938e0` | 10/14 |
  | `0x004b61c0` | 215/221 |
  | `0x005a8890` | 68/69 |
  | `0x005aaac0` | 22/33 |
  | `0x005aab00` | 45/60 |
  | `0x005ab410` | 54/125 |
  | `0x005ad320` | 58/97 |
  | `0x005ae920` | 94/107 |

- **The 32 unseen mutants fall in three groups:**
  - argument loads of a value that only reaches a stub (and so is never observed);
  - `add esp, n` after a stub, where the mutant ends inconclusive rather than different;
  - paths the seeds do not reach. Most of these are in `0x005ab410`, whose success branch needs a callee to return
    the wanted header.

**What it cannot cover** is the same as EQUIV-CHECK section 5. Callees have no side effects except the one modelled
header write. Incoming pointers never alias the stack. Real callee precision control is not modelled.

The cross-check is a witness. The admission rests on the proof (L-126).

## 3. ADMITTED.tsv

Columns:

| Column | Content |
|---|---|
| `rva`, `name` | The function |
| `source` | The source the port binds. For `0x00475a00` this is `re/match/s12/00475a00.c`, which `Match/PortTop_00475a00.cpp` includes. The measured row used `re/match/pilot/a3_00475a00.c`. Both prove EQUIV [SIB], and the s12 source runs SAME 16/16 (`s12_00475a00.log`) |
| `flags` | The source's `// FLAGS:` line |
| `source_commit`, `tool_commit` | `tool_commit` is the last commit of `equiv.py`, be32d7f80 |
| `verdict`, `classes`, `rules` | The verdict line |
| `caller_scan` | For a conditional verdict, the callers and the per-site scan |
| `local_callees` | Bodies the source itself defines |
| `xcheck_default`, `xcheck_seeded` | Cross-check tallies |
| `port_binding` | Where the body runs on the path today |
| `evidence` | This file |

**Rows.**

- 32 EQUIV and 1 EQUIV-IF-CALLERS-IGNORE(EAX): `0x00555360`, caller `0x00552b79` ignores.
- None uses `--relax-icall`. None is x87-order.

**Local callees.**

- 3 rows have local callees in `measure_v4.tsv`: `0x00417180` (`_FUN_00414030`), `0x004332a0` and `0x00498ea0`.
- EQUIV-CHECK section 7 found every local callee of a proven row ledger PASS or proven.
- Those callees bind by name when the body is wired, as matched bodies.

**Not admitted here.**

- **The 6 conditional rows with an unsettled caller:** `0x00430670`, `0x00490020`, `0x004d8480`, `0x005aa7e0`,
  `0x005acd10`, `0x005b0cf0`. Each stays a per-function decision (L-126).

## 4. The knob path

Callers come from `re/console/pc_features.csv`, direct calls. Each caller's state is its kind in
`verify/porttop_20261108/BINDINGS.tsv`.

| Case | Rows |
|---|---|
| Caller matched and bound, body already bound | `0x00475a00`. Its caller `0x0040bbb0` binds `FUN_00475a00` to `Orig_00475a00` (`Match/PortTop_0040bbb0.cpp:118`), a near-match since PORT-OT-RENDER-2 (L-115). It is now recorded under L-126 |
| Bound by another row | `0x00555360` in `0x00552b60` (NEARMATCH-555360 4503d7205, held behind L-129). Not duplicated here |
| Caller is a stand-in or under a stand-in | `0x0042aa00` (caller `0x0043dfd0`, under-stand-in), `0x0045c480` (`0x00466b50`, under-stand-in), `0x004d4f00` (`0x00496e40`, a stand-in: `HandTop_InitDataInput` does that job) |
| No caller compiled on the path | the other 28 |

So no admitted body can run on the knob path from this row.

- **Why no `PortTop_<rva>.cpp` bodies were added.** A body with no caller would be dead code. Its callee bindings
  depend on what the caller's slice binds. Three of the rows read RenderWare state, which L-124 routes to the RW
  core: `0x00498ea0`, `0x004d4f00` and `0x00555360` are in SIZE.tsv's RwGlobals caveat.
- **When a caller lands.** The slice that wires it binds `FUN_<rva>` to `Orig_<rva>` from the unedited source in
  `ADMITTED.tsv`, the `Match/PortTop_00475a00.cpp` form.
- **Port column.** A row moves to `port = called` only when that happens.

**Path size** (`origtop_size.py --order`, `size_before/` = `--no-admitted`, `size_after/` = with the record):

| | before | after |
|---|---|---|
| reach | 2316 | 2316 |
| match class in reach (C4-by-match + admitted) | 1653 | 1686 |
| game-c0c3 in reach (blocking) | 326 | 293 |
| runnable with no new family bound | 1065 | 1105 |
| runnable with every binding family bound | 1236 | 1281 |
| dual copies among those | 89 | 104 |

The "before" column reproduces `verify/portorigtop_20261109/SIZE.tsv` and `blocking_c0c3.tsv` (326).

## 5. Checks run

| Check | Result |
|---|---|
| default build (`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`) | `=== Build OK ===`, `mashed_re.exe` 3,238,400 B |
| object build (`set MASHED_DDKOBJ=1`) | link rc 0, `mashed_re_ddk.exe` 3,234,816 B |
| `ddkobj.py check` | IDENTICAL 541, DIFFERENT 8, not-in-map 3 (`BYTECHECK.tsv`). The same counts as DDKOBJ-SPIKE, because no exe source changed (see below) |
| knob boot, default exe | `knob_on_vs: exited rc=0 (0x00000000) after 76.3 s`, last line `WinMain 0x00492370 returned 0 after 3000 frames`, 1335 lines |
| knob boot, object build | `knob_on_ddk: exited rc=0 after 54.7 s`, the same last line, 1335 lines. It differs from the VS2022 log only in thread ids and commit totals |
| changed commands | `equiv_dyn.py` default and `--seeded` over the 33 rows. `origtop_size.py` with and without `--no-admitted` (before = the committed SIZE.tsv) |
| G-CTRL / B | not run: top-down row with no exe change (WORKER_RULES, L-127) |

**The 8 DIFFERENT rows in `ddkobj.py check`:** `0x00493900`, `0x00493710`, `0x0042f530`, `0x00475a00`, `0x0048e820`,
`0x00414060`, `0x004929d0`, `0x0045a3a0`.

- `0x00475a00` differs in 2 bytes at +0x41, the two SIB bytes its admission covers.
- The rest are the below-100% sources and `0x00414060` that DDKOBJ-SPIKE already lists.

Both boots ran through `test-slot` (waited 600 s for memory, then slot 5/8) and
`mashed_lock.py run --label EQUIV-ADMIT-1`. Root: `D:\MashedWT\EQUIV-ADMIT-1_root`. Each boot killed nothing: both
spawned PIDs (23332, 23240) exited by themselves.

**Raw files (not committed):**

| File | SHA-256 |
|---|---|
| `D:/MashedWT/EQUIV-ADMIT-1_data/knob_on_vs.log` | `0cd3a0bc64a7796f88cefa7015655cea26e929c5d87cac1c4719e92249847891` |
| `D:/MashedWT/EQUIV-ADMIT-1_data/knob_on_ddk.log` | `214be1b288c4278e86b8588b82c2f312a36f318f326425c6257fb2c801169622` |

The two build logs are `D:/MashedWT/EQUIV-ADMIT-1_data/build_default.log` and `build_ddkobj.log`.

## 6. Next

- When a slice wires a caller of an admitted row (for example `0x0040fc00` for `0x00429310`, or `0x0043c000` for
  `0x004332a0`), it binds the body from `ADMITTED.tsv` and moves the port column.
- A later admission appends rows with `gen_admitted.py`'s checks: same verdict on re-run, 0 differences, and 8 or
  more conclusive runs.
- PROVISIONAL-BODIES starts `PROVISIONAL.tsv`.
