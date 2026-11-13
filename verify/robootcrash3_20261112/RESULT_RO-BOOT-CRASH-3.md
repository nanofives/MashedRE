# RESULT RO-BOOT-CRASH-3 (2026-11-12)

Branch `orch/RO-BOOT-CRASH-3`, worktree `.worktrees/RO-BOOT-CRASH-3`. Data: `D:/MashedWT/RO-BOOT-CRASH-3_data`.
Private root `D:/MashedWT/RO-BOOT-CRASH-3_root` (robocopy of `original\`; patched `MASHED.exe` SHA-256 `B9977DAB...FE87`,
the same as `RO-MENU-REPLAY_root`; `MASHED.exe.unpatched` = the anchor `BDCAE093...3C0E`).

## Verdict: GREEN

The 3 boot CRASH bodies of RO-MENU-REPLAY now run through the menu replay with no fault. Each one is
**IDENTICAL-FOR-N 3324 of 3324** (menu 600 + race 2724), strict: one same-class reference given as both references,
0 allowed blocks. This holds in 3 class-matched runs per body.

| RVA | cause of the RO-MENU-REPLAY crash | fix | runs IDENTICAL-FOR-3324 (strict) |
|---|---|---|---|
| `0x0045d460` | source literal: the path compiled with `\p` / `\a` escapes | `re/match/s10/0045d460.c:80` backslashes doubled | f3, f5 (vs q3, q4, q7, q8 each), f6 (vs q5) |
| `0x0048e820` | old body (before EQUIV-FU-48E820); the new source is byte-identical | re-link of the current source, no edit | f5, f6 (vs q3, q4, q7, q8 each), f3 (vs q5) |
| `0x00499400` | provlink: the file-static `DAT_007731f8` in `.bss` linked as 0 bytes at the blob end | `re/tools/prov/provlink.py` (+ `match.coff` section size) | f4, f5, f6 (vs q3, q4, q7, q8 each) |

Each body is installed (`jmp_ok`) and executed: `calls_boot` 1, `calls_total` 1 in every run (`prov.json`). Every
body run (12 of 12) completed 3324 ticks with rc 0. The 3 runs not scored are boot-class mismatches (below).

No build input changed, so there were no builds, no `ddkobj.py check` and no gates. `re/match/s10/0045d460.c` is
not `#include`d by any exe TU: `grep -rl "0045d460\.c" mashedmod/` is empty, and `mashedmod/src/mashed_re/Match/`
holds no `MatchS10_0045d460.cpp`. The other changes are Python tools.

## Diagnosis (cited)

### `0x0045d460`: wrong path literal

- The original pushes `0x005ce68c` (`0x0045d46f push 0x5ce68c`): `toastaudio\pc\audio\pcdics\PERMDICT.RWS`.
- The old body pushes its own literal (body `+0x0f push <blob+0x340>`), which holds
  `toastaudiopc\x07udiopcdicsPERMDICT.RWS`. The source line was `FUN_004cc230(2,1,"toastaudio\pc\audio\pcdics\PERMDICT.RWS")`,
  so `\p` gives `p` and `\a` gives `0x07`.
- The fault chain:
  - body `+0x6d` calls `0x4cc5e0(h, 0x809, 0, 0)`, where `h` is the return of `0x4cc230`
  - `0x004cc5fb` calls `0x4cc400(h, ...)`
  - `0x004cc40f` calls `0x4cbd30(h, ...)`
  - `0x004cbd3a mov eax,[ebx]` with `ebx = h = 0`. This is the RO-MENU-REPLAY fault (read `0x0` at `MASHED.exe+0xcbd3a`).
- `[UNCERTAIN]` Which failure branch of `0x4cc230` returned 0 for the bad name (candidate: the path at `0x004cc31f`) is
  not traced. The missing evidence is a single-step of `0x4cc230` on the old body. The corrected literal is the only
  change, and it removes the fault in 4 of 4 runs.
- Literal bytes are relocated data, so `match.py` cannot see them: the score is 94.2% before and after the edit.
  - New tool `re/tools/prov/prov_escscan.py` lists match-source literals with unintended escapes.
  - Over `re/match` it finds this line only, and nothing after the fix.

### `0x0048e820`: stale body

- RO-MENU-REPLAY linked the body before EQUIV-FU-48E820 landed (`58cc241be`: static EDI callee, index bridge removed).
  The old fault was a write to `0xe0722470` in `0x4b6506`, reached from `0x0048cee0` (`0x48cf34`) at old body `+0x249`.
- The current `re/match/s15/0048e820.c`:
  - `match.py --only 0x0048e820`: 100.0% (124/124 ins, `/O2 /GS /Ob1`, ddk)
  - `relocheck.py`: OK (checked 48, unchecked 0)
- Re-linked from that source, it changes code relative to `verify/provinsitu_20261111/bodies_all.json`, and it runs clean.

### `0x00499400`: provlink `.bss` defect

- `re/match/s16/00499400.c` declares `static int *DAT_007731f8;` under the DDK. That puts `_DAT_007731f8` in a `.bss`
  section with no raw data.
- `provlink.place()` appended `sec["data"]` (empty), so the static took 0 bytes:
  - the old body read `mov eax,[blob+0x2a0]`, offset 672, which is the blob size
  - so it read 0 and faulted at `+0x5d mov ecx,[eax+ebp*4]`
- The original reads `0x00499458 mov eax,[0x7731f8]`, filled by the original `FUN_00498c00` at
  `0x00498c43 mov [0x7731f8],eax`.
- Fix in `provlink.link`:
  - a defined data symbol named for an original address (`DAT_<va8>`, `PTR_DAT_<va8>`...) resolves to that address, like
    the existing `FUN_<va8>` rule
  - a section with no raw data gets its `SizeOfRawData` in zeros (`match.coff` now records `size`)
- The new body reads `[0x7731f8]` at `+0x58`.
- Re-link of all 307 bodies of `bodies_all.json` with the new tool against the base tool, on the current sources:
  only `0x00499400` changes. 302 OK and 5 UNRESOLVED, the same counts as the base tool.

## Self-tests

- **References** (this root, class `39a80501` / `0x6147d8`=1 / `0x67d800` hash `3234824349`): q3, q4, q7, q8, all 6
  pairs IDENTICAL-FOR-N 3324.
- **Negative control** `fneg` (`--negctl 0x0067f17c --neg-tick 300`): class `39a80501` / hash `3234824349` (matches q3, q4, q7, q8). Each of q3, q4, q7, q8 as a single reference
  (strict) gives DIVERGED at tick **300** exactly, block `0x0067f000`, menu segment. PASS. Against q5 (other class)
  it diverges at tick 0 in `0x0067d800` + `0x007f1000`, as the class rule predicts.
- **provlink self-test** (`provlink_selftest.py`):
  - seed 1, n 40: ok=40 bad=0 skip=0
  - seed 7, n 200: ok=194 bad=2 skip=4
  - The 2 BAD rows link identically under the base tool, so they predate this row. Both call a `thunk_FUN_<x>`:
    - `0x004cbca0` (`rwl01`, `thunk_FUN_004cc820`): the original calls the thunk at `0x4cc9df`
    - `0x005ae330` (`s26`, `thunk_FUN_004522d0`): the original calls `0x5ae4b0`
  - `resolve_ext` maps `thunk_FUN_<x>` to `<x>`, not to the thunk the original calls. Not changed here.
  - Since this row's fix, address-named statics are absolute operands that the self-test compares. Before, they were
    blob-internal sites it skipped.

## Boot-class findings (tooling, for the replay oracle)

1. **References from another root never match.**
   - The module path is in the image at `0x00773c4c..0x00773c70` (`...RO-BOOT-CRASH-3_root\original\MASHED.exe` here,
     `...RO-MENU-REPLAY_root\...` in r3).
   - Class-matched q2 against r3/r4 diverged at tick 0 in block `0x00773c00` on those 10 dwords. r4 against r3 has 0
     there.
   - Rule: score only against references recorded in the same root.
2. **Block `0x0067d800` is a second boot-class marker.**
   - Its tick-0 hash splits runs that `0x007f1030` puts in one class.
   - Unmodified pairs differ there for ticks 0..599 exactly: q5/q6 against q3, RO-MENU r5 against r3, voided q1 against
     q2. The warp at tick 600 ends the difference.
   - `f4_0x0045d460` and `f4_0x0048e820` (timer class `39a80501`) carry hash `1291831651`. That is the value of no-body
     q5 (class `39a82829`) and of `f6_0x0045d460` / `f3_0x0048e820`.
   - So both f4 runs are class-mismatched, not body evidence, and are left unscored (`score_strict.py` keys the class on
     both markers).
   - `[UNCERTAIN]` which dword of `0x0067d800..0x0067dbff` carries it:
     - code from `0x004273c0` on reads and writes `0x67d830..0x67d84c`
     - the first full-image checkpoint is at tick 780, after the warp
     - the missing evidence is a `--menu-ckpt` capture from one run of each hash
   - Suggested follow-up (not done here): add the block hash to `prov_compare.BOOT_CLASS_DWORDS` handling.
3. Timer classes seen in this root's 18 clean runs: `39a80501` (13), `39a82829` (3), `39a891a1` (q6), `39a86e79`
   (`f3_0x00499400`, unscored: no reference of its class).

## Run hygiene (what went wrong and how it was handled)

- The first batch was stopped with TaskStop while it waited for a test slot. The stop ended the tool's shell but not
  `bash batch1.sh` (pid 10660). That orphan ran batch 1 a second time in parallel and wrote the same labels.
  `run_arm.sh` does `rm -rf $D/$L` before taking the game lock.
- The orphan tree (my own PIDs only: 10660, 11728, 10196, 10380, 8108, 9352, 19936, 12692, 19360) was killed at about
  17:25.
- All batch-1 directories (q1, q2, f1_*, f2_*) were moved to `D:/MashedWT/RO-BOOT-CRASH-3_data/voided_batch1/` and
  are not used as evidence. Every batch-1 log line still showed 3324 ticks and rc 0 for every label.
- The evidence is batches 2 and 3: unique labels q3..q8 and f3..f6, one writer each.
- `PROYECTOS_TEST_MEM_GB=4` per arm (32-bit `MASHED.exe` + Frida host). The default 8 GB request queued for minutes on
  the 24 GB budget.

## Files

- `run_arm.sh`: arm runner (copy of `verify/romenu_20261112/run_arm.sh` with this row's root, data dir and lock label)
- `bodies_fix3.json`: the 3 bodies linked with the fixed tool and sources (the bodies these runs executed)
- `score_by_class.py` / `score_strict.py`: class-matched scoring (pair form / strict single-reference form)
- `verdicts_strict.json`: every strict verdict above, with classes
- `block_extent.py`: per-tick extent of block differences between two runs
- `raw_sha256.txt`: SHA-256 of every raw file cited: ticks/meta/prov per run dir, run logs, the relink outputs and
  logs, the self-test logs, the batch scripts and logs
- Tool changes: `re/tools/prov/provlink.py`, `re/tools/match/match.py` (`coff` section `size`), new
  `re/tools/prov/prov_escscan.py`. Source change: `re/match/s10/0045d460.c:80`.

## Not done / next

- No tracker change. `0x0045d460` (94.2%) and `0x00499400` (90.1%) stay NOT-PROVEN provisional bodies:
  IDENTICAL-FOR-3324 in the menu replay is not C4 without the Write-set leg (L-38 / L-40 / L-54).
- `0x0048e820` is already C4 by match (EQUIV-FU-48E820). A `c4=frida-replay` tag would need that leg too.
- The `thunk_FUN_<x>` resolution in provlink (self-test BAD 2) is its own row.
