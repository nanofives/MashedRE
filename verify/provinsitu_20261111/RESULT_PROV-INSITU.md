# RESULT PROV-INSITU: rough sources tested inside the original under the replay

Date 2026-11-11. Worker session cmwcaq05d2xf9sc1bcoy6s3fw (spawned on claude2 as claude-sonnet-5-5 medium, moved to
claude3, ran as claude-opus-5-5 medium; no step-up), lead cmwcao6vv2x0vsc1bcskik644.
Branch `orch/PROV-INSITU` from `orch/integration` `778b8b91d`. Review: **TEXT-ONLY / harness** (`re/frida/`,
`re/tools/prov/`, `verify/provinsitu_20261111/`; no `mashedmod/` file, no tracker, no ROADMAP edit).

## Verdict

**GREEN.** 307 blocking functions with a match source have a verdict. 188 of them were wired as their XP DDK object,
reached through an inline JMP at their RVA inside the original MASHED.exe, and replayed against the 2,724-tick reference.

| verdict | count | what it means |
|---|---|---|
| **IDENTICAL-FOR-2724** | **158** | every compared tick of the race equals the reference, with the body live |
| DIVERGED@tick | 18 | first tick where a compared block differs, confirmed in 2 runs (dwords below) |
| CRASH | 11 | access violation in a single-body run, confirmed in 2 runs |
| HANG | 1 | `0x005aba20`: the load never finishes (main thread stops at N=676), 3 of 3 runs |
| NOT-EXECUTED | 113 | 0 calls from the warp to the end of the replay (7 of them run only in the menu) |
| NOT-WIRED | 6 | 5 call a CRT name the symbol map lacks (`_sprintf`, `_ROUND`, `_fsin` x2, `_fpatan`), 1 is a boot-patch site (`0x00495870`, fix_joypad detour) |

By match score of the source:

| score | sources | IDENTICAL | DIVERGED | CRASH / HANG | NOT-EXECUTED | NOT-WIRED |
|---|---|---|---|---|---|---|
| 95+ | 56 | 31 | 2 | 0 | 22 | 1 |
| 80-95 | 113 | 65 | 3 | 1 HANG | 44 | 0 |
| 50-80 | 97 | 43 | 8 | 9 | 35 | 2 |
| under 50 | 41 | 19 | 5 | 2 | 12 | 3 |

(56 / 113 / 97 / 41 include 14 sources newer than `measure_v4.tsv`, from `re/match/racepath` and `re/match/tlr`,
4 / 3 / 1 / 6 per tier: 12 IDENTICAL, 1 DIVERGED, 1 CRASH. Without them the tiers are 52 / 110 / 96 / 35 = 293, the
blocking rows `measure_v4.tsv` gives a source. The row's own count, 296 (52 / 110 / 98 / 36), is 3 higher.)

**Write-set leg (L-38 / L-40 / L-54), for the 158 IDENTICAL bodies, from the body's own stores (static, capstone over
the DDK code):** 40 have no store outside their frame (empty), 4 store only to absolute addresses inside the compared
image outside the masked blocks (covered), **114 have at least one register-based store** whose target (heap, caller
stack or static image) the static list cannot place. Under L-128 those 114 stay where they are. **44 bodies are
IDENTICAL with an empty or covered write-set** (list in "C4 candidates").

**No promotion was made.** `C4-REPLAY-RULE` is still claimed, not merged (ledger, 2026-11-11), so this is the verdict
table only.

**For T2:** of the 326 blocking functions, 197 run in this scenario. 158 of them are IDENTICAL with their rough source
live, 30 are wrong (18 DIVERGED, 11 CRASH, 1 HANG), 1 is not wired (`0x005a6f30`, `_ROUND`) and 8 have no match source
(`0x0040be50`, `0x0040eee0`, `0x004128f0`, `0x00487280`, `0x004883b0`, `0x004a3384`, `0x004a34b0`, `0x004a3700`). The
90 hand `.cpp` bodies were not tested here: this harness loads DDK objects, not the VS2022 hand bodies.

## How it works

### The body in the original (`re/tools/prov/provlink.py`)

The row asked for "the DDK object of the source in a dev asi, inline JMP at its RVA". The asi build links against
the exe's symbol space. Inside the original, the same link can be done against the original image, with no build:

- The source is compiled exactly as `match.py` does (`compile_obj`, the file's `// FLAGS`, `// LANG`, `// TOOLCHAIN`,
  XP DDK cl 13.00.9176 / c2 13.00.9178).
- The target function's COMDAT and every section it references (`__real@` literals, strings, switch tables, static
  helpers) are copied into one blob.
- Relocations are resolved against the original:
  - `FUN_<va8>` -> that address (the ORIGINAL function, even when the source defines it, so each RVA tests one body)
  - `MGLOBAL` / `MGLOBALA` names -> their address in the source
  - `DAT_` / `PTR_` names -> their address
  - CRT helpers and `__imp__` -> `re/tools/match/equiv_symmap.tsv`, then the original's import table
  - absolute symbols -> their value
  - anything else -> UNRESOLVED, and the function is NOT-WIRED
- **Self-test** (`provlink_selftest.py`): 40 of 40 random byte-matched (ledger PASS) sources, linked as if placed at
  their own RVA, equal `MASHED.exe.unpatched` byte for byte outside the blob-internal literal-pool sites.

### The install and the run (`re/frida/detorig.py` / `js/detorig.js`, new options)

- `--prov FILE [--prov-rvas]`: at the install, at the menu, each body is copied behind a stub:
  - the stub is `inc dword [counter]; jmp body` (the call count, no Interceptor, no stack change)
  - an `E9 rel32` is written at the RVA
  - counters and code sit on separate pages (a counter on the code page cost a self-modifying-code clear per call,
    7x slower, run `x_b1`)
- **Install check:** sent at install time (`prov_install.json`), for every site:
  - the original first 5 bytes
  - the site after the patch
  - the jump from the site lands on the stub, and the stub's jump lands on the body entry

  All 188 tested bodies report `jmp_ok`, 0 failures.
- `--prov-count FILE`: native call counters only (Interceptor + CModule), the census. Counters are read at the warp,
  the race anchor and the end, so the table separates menu calls from race-window calls.
- **`--warp-clock K`** (new, required): sets the virtual T / Q to start + K ticks at the warp, before the existing timer
  pin.
  - **Why:** without it, the menu ran a run-dependent number of ticks before the warp, because the warp is scheduled
    on clock calls, not ticks. The timer pin (`0x007f102c` / `0x007f1030`) then landed one 50,000-count step apart
    between runs.
  - **The race reads that phase:** blocks `0x0068f400` / `0x006c2400` differed from tick 2 in every such run (h14,
    h14b, 5 group runs).
  - **With the pin:** references r1 / r2 are IDENTICAL-FOR-2724, and the 3-body group that flaked before
    (`0x00408610`, `0x00414300`, `0x0041af50`) replays identically.

The scenario is the oracle's:
- RO-WARP-NORMALIZE batch-3 configuration: tick clock, `--q0 0xC0000000`, `warp_normalize.tsv` pins, `--load-sleep-ms 0`
- QuickRace track 0, mode 10, player + 3 AI, scripted input
- private root `D:\MashedWT\PROV-INSITU_root`, a robocopy of `original\`. Binaries are identical to the oracle's:
  `MASHED.exe` `B9977DAB...`, `mashed_re_dev.asi` `906F8601...` (not hooking), `d3d9.dll` `47901951...`

### The compare (`re/frida/prov_compare.py`)

`detorig_ticks.py` does not count a block that already differs at tick 0 (CONSTANT). Bodies go live before the load,
so this compare counts every block from tick 0 on, with these allowances:
- **Allowed blocks:** blocks that differ between two unmodified references (r1 / r2 and x_ref / x_ref2), 14 blocks.
- **Per-dword masks:** in blocks copied raw every tick, the MASK rows and the HWND / WINMSG / FILEPTR rules of
  `warp_normalize.tsv` are dropped per dword. The raw copy now includes `0x007f0c00..0x007f1400`.
- **TIMER-PHASE rule (tick 0 only):** `0x007f1010`, `0x007f102c` and `0x007f1030` hold values set during the load.
  - At tick 0 they differ with the load length: run x_b1b, a body with 0 calls before the anchor, at 118 / 119 calls.
  - From tick 1 the block is equal again.
  - The rule applies only when that block is the one bad block and the raw words show nothing else in it.
- **FLAKE-T0 (`prov_table.py`):** a run is a harness run, not a verdict, when it shows this tick-0 divergence:
  - raw `0x006181a0..a8` = 0x3f000000 against 0
  - with `0x00623d34` 0 against 1 and a zeroed `0x006cf0a0` at the first checkpoint

  It appeared in 4 of 208 runs, for 4 different bodies, and never again for the same body:
  - `0x005a9e60`: IDENTICAL 2 of 2 afterwards
  - `0x00448220`: DIVERGED@1116 2 of 2
  - `0x00443440`: DIVERGED@196 2 of 2

**Controls:**
- **Negative control:** x_neg, +1 on the RenderWare RNG word at the anchor, is caught at tick 7, the same tick as the
  oracle design.
- **Determinism of body runs:** h14 against h14b, the same 14 bodies, is IDENTICAL-FOR-2724.
- **References:** r2 against r1, IDENTICAL-FOR-2724.

### Many bodies per run (`re/tools/prov/prov_bisect.py`)

Bodies the census saw execute in the race window are run in groups. A group that replays identically gives every
member IDENTICAL-FOR-2724. A group that does not is split in halves until each wrong body is alone.
- Group sizes: 95+ split from 29, 80-95 groups of 6, 50-80 groups of 2, under 50 singles.
- 172 verdict runs held 335 bodies (1 to 15 per run; attach-failure retries counted).
- Every DIVERGED / CRASH / HANG single was then run again (`rep`, `rep2`, `newrep`), and the table reports
  a verdict only when the runs agree (37 repeat runs in all).
- A Frida attach failure is retried; 1 single was first recorded as CRASH from one and re-ran as DIVERGED@2.

Whole campaign: 210 game runs.
- Every run was under `test-slot`, with `mashed_lock.py run` per game run inside it.
- Only PIDs this session spawned were killed.
- Twice, stopping the driver killed only the child and let `chain.sh` move to the next tier, which briefly ran 2 of
  my games at once. Both were killed, and the stale lock from my dead holder was broken with `mashed_lock.py break`.
  `chain.sh` now exits on INT / TERM.

## The non-identical bodies

The first tick and blocks come from the per-tick compare. "At that tick" lists words from the raw-copied ranges. The
checkpoint dwords come from the first full-image checkpoint at or after that tick (`prov_dwords.py`; checkpoints are
every 2,000 clock calls, about 181 ticks), as body value against original value.

| rva | score | verdict | detail (last confirming run) |
|---|---|---|---|
| `0x00424eb0` | 98.6 | DIVERGED@1108 | 45 blocks from 0x613000; at tick 1108 `0x007f0f64` body 0x447a4000 (1001.0) orig 0x447a0000 (1000.0); checkpoint tick 1270: `0x006130c4` 0x4507c35c / 0x45026285, `0x00624048` 5 / 4 |
| `0x00448220` | 97.0 | DIVERGED@1116 | blocks 0x68f400, 0x86ac00, 0x898000, intermittent (47 ticks between 1116 and 2706), 0 dwords differ at any checkpoint. **`equiv.py` proves this source EQUIV (classes LAYOUT, VALUE-FLOW)** |
| `0x00468d80` | 82.1 | DIVERGED@408 | block 0x881c00; checkpoint tick 543: `0x00881c64` 0x3d82ea2a / 0x3d82ea29 (1 ULP) |
| `0x0046e9e0` | 81.7 | DIVERGED@116 | blocks 0x881400, 0x881c00, 0x884400; checkpoint tick 180: `0x0063d928` 0x3d126547 / 0x3d126549 (2 ULP) |
| `0x00481e00` | 86.5 | DIVERGED@2 | block 0x6ce800; checkpoint tick 180: `0x006ce818` 0 / 0x75 |
| `0x005aba20` | 81.9 | HANG | load never finishes, N stops at 676, 3 of 3 runs |
| `0x00441d40` | 69.2 | DIVERGED@2 | blocks 0x896400, 0x896800; `0x00896620` 0x3e997982 / 3 |
| `0x00442e00` | 65.1 | DIVERGED@1132 | blocks 0x898000, 0x912800 |
| `0x00443440` | 58.4 | DIVERGED@196 | blocks 0x803000, 0x89a400; checkpoint tick 362: `0x0089a568` 0xb6 / 0xac |
| `0x00443dc0` | 57.5 | DIVERGED@2 | block 0x803000; `0x008032ec` 0x43ac58f7 / 0x43accce2 |
| `0x00445aa0` | 67.4 | DIVERGED@2 | block 0x896400; `0x0089650c` 0xbf719745 / 0xbf719777 |
| `0x00469aa0` | 60.0 | DIVERGED@385 | blocks 0x7ea800, 0x7eac00; `0x007eabdc` 0x008822a4 / 0x00469be2 |
| `0x004709a0` | 55.7 | DIVERGED@321 | 17 blocks; at tick 321 `0x006dcd44` 0xbff93c4f / 0xbff63b71 |
| `0x00479100` | 69.1 | DIVERGED@161 | block 0x63dc00, 0 dwords at the next checkpoint |
| `0x00469df0` | 30.1 | DIVERGED@385 | 10 blocks from 0x715800; `0x00715a5c` 0x3a87418d / 0x3a874191 |
| `0x0046ef70` | 43.9 | DIVERGED@408 | block 0x881c00 |
| `0x0046f6c0` | 44.3 | DIVERGED@1 | 11 blocks; `0x008815ac` 0x1900 / 0 |
| `0x004c51a0` | 14.7 | DIVERGED@1 | 5 blocks; `0x00881f24` 0 / 3 |
| `0x00487180` | 11.7 | DIVERGED@0 | 4 blocks from 0x6c3400; `0x006c36b0` 0xbf01a0c7 / 0xbef817d6 |
| `0x00470c70` | 69.8 | CRASH | 0xc0000005 after the race anchor |
| `0x00481a30` | 63.3 | CRASH | 0xc0000005 after the race anchor |
| `0x005a75b0` | 51.6 | CRASH | 0xc0000005 after the race anchor |
| `0x005a7b60` | 53.5 | CRASH | 0xc0000005 before the race anchor |
| `0x005a9f10` | 73.9 | CRASH | 0xc0000005 after the race anchor |
| `0x005aa0c0` | 56.2 | CRASH | 0xc0000005 before the race anchor |
| `0x005abfa0` | 62.9 | CRASH | 0xc0000005 before the race anchor |
| `0x005ac210` | 57.1 | CRASH | 0xc0000005 before the race anchor |
| `0x005ad0b0` | 56.2 | CRASH | 0xc0000005 after the race anchor |
| `0x005b3b80` | 43.4 | CRASH | 0xc0000005 after the race anchor |
| `0x00440bc0` | 12.4 | CRASH | 0xc0000005 before the race anchor |

Two of the DIVERGED rows are 1-2 ULP float differences (`0x00468d80`, `0x0046e9e0`), the form an x87 operand-order
residual takes. Diagnosis of any row (why, which instruction) was not attempted here. Each row names its first
tick, its blocks and the dwords, which is where a first-divergence leg would start.

**`0x00448220` is the one finding that crosses another tool:**
- `equiv.py` proves its source EQUIV, with classes LAYOUT and VALUE-FLOW.
- The replay diverges intermittently in 3 blocks from tick 1116, and never at a checkpoint.
- [UNCERTAIN] One mechanism that fits is a stale-stack reader that sees a different frame layout. No reader was
  traced, and it needs one (a `--watch` on a dword of 0x68f400 at tick 1116).

## C4 candidates (IDENTICAL-FOR-2724, write-set empty or covered, install check ok, body executed)

Not promoted: these wait for `C4-REPLAY-RULE`. Each one still needs re-classify's own checks (no STUB, the evidence
fields).

- 95+: `0x00414300`, `0x0041d930` (covered), `0x0041fcc0`, `0x00429310` (covered), `0x00477e60`, `0x00482ae0`,
  `0x00487020`, `0x0048a4e0`, `0x00411ae0` (covered)
- 80-95: `0x0041f770`, `0x0041faf0`, `0x00425ca0`, `0x00428450`, `0x004584e0`, `0x00458a40`, `0x0045a190`,
  `0x004647f0` (covered), `0x00474de0`, `0x00474e70`, `0x004770a0`, `0x00477f50`, `0x0047cea0`, `0x00480b70`,
  `0x0048aa50`, `0x004c1a70`, `0x004c1b40`, `0x004c4d20`, `0x00546b10`, `0x005aa7a0`, `0x005ae3a0`, `0x0040c690`
- 50-80: `0x00407be0`, `0x00416060`, `0x00458b10`, `0x00484f70`, `0x00556ca0`, `0x005a6d60`, `0x005aaac0`,
  `0x005aab00`, `0x005aab70`, `0x005acd10`, `0x005ad320`
- under 50: `0x0045c550`, `0x005acda0`

**Thin rows:** 17 IDENTICAL bodies ran fewer than 10 times in the race window: `0x0048a4e0` 1, `0x00405540` 1,
`0x0047d3c0` 1, `0x0045cd30` 1, `0x005ae650` 1, `0x00424b80` 2, `0x005ae250` 2, `0x005a9e60` 3, `0x0047a320` 4,
`0x0040e590` 4, `0x005a8890` 4, `0x00482140` 4, `0x005aaac0` 4, `0x00441b30` 7, `0x0040b290` 8, `0x005ae3a0` 8,
`0x005ae4c0` 8. Their verdict covers only those calls.

## What this does not cover

- **Write-set of the 114 not-covered bodies.**
  - I tried a Stalker store-target sampler: blob code only, the first 20 calls per body, effective address classed
    image / stack / other.
  - Its one run (ws1, 68 bodies) crashed the game with 0xc0000005. Not debugged; the code was reverted.
  - Reported as a failed attempt. A dynamic store sampler (or a per-body heap-tracked leg) is what would move them.
- **The 90 hand `.cpp` bodies** (the asi `--hooks` path, VS2022 objects) and the 8 executed functions with no source.
- **Scenario.** One race (track 0, QuickRace, 4 cars, 2,724 ticks). NOT-EXECUTED here means only "not in this race".
- **Builds and gates.**
  - Not run: no build input changed (`mashedmod/` untouched). The DDK objects are compiled by `match.py`'s
    compile path and live only in the original's process.
  - `ddkobj.py check` was not run for the same reason.
  - No G-CTRL / B (top-down rule).

## Evidence (this folder)

- `PROV_VERDICTS.tsv`: one row per source.
  - Columns: rva, source, score, equiv_v4, install, race-window calls, calls in the run, store summary,
    write-set leg, verdict, detail.
  - **This is the input for PROVISIONAL-BODIES.**
- `dwords.json`: the checkpoint dwords per DIVERGED body.
- `bodies_{95,80,50,0,new}.json`: the linked blobs, with relocations and the static store lists. `bodies_all.json`
  holds all of them.
- `state/`:
  - `*_state.jsonl`: every verdict run, with group, calls, install, first divergence
  - `census2_prov.json`: race-window call counts
  - `meta_{r1,r2,x_ref,x_ref2,x_neg,census2}.json`
  - `x_neg.verdict.json`
- `run_arm.sh`, `batch.sh`, `chain.sh`, `bisect_chain*.log`, `rep2.log`, `new.log`, `batch*.log`

Raw runs (ticks.bin, checkpoints) stay in `D:/MashedWT/PROV-INSITU_data/` (210 runs).

Tools: `re/tools/prov/provlink.py`, `provlink_selftest.py`, `prov_bisect.py`, `prov_dwords.py`, `prov_table.py`;
`re/frida/prov_compare.py`; `detorig.py` / `js/detorig.js` (`--prov`, `--prov-rvas`, `--prov-count`, `--warp-clock`).

## Next

- **C4-REPLAY-RULE lands:** run re-classify over the 44 candidates.
- **Opus-high diagnosis rows, one per DIVERGED body:** start at its first tick and dwords. `0x00448220` first, for the
  EQUIV disagreement.
- **A dynamic write-set leg** for the 114 not-covered IDENTICAL bodies.
- **More scenarios** (other tracks and modes) for the 113 NOT-EXECUTED.
- **The 5 NOT-WIRED CRT names:** add `_sprintf`, `_ROUND`, `_fsin`, `_fpatan` to the symbol map.
