# RESULT D4-WSB-C4-SOLV-TRK1: GREEN, 7 rows C3 -> C4, `0x005685f0` stays C3 (NOT-EXEC)

Verdict: **GREEN.** Re-run on the L-56 flipped build. Leg B (heap-tracked write-set shadow) and the racediff both PASS
on 7 of the 8 rows, which go C3 -> C4 through re-classify. `0x005685f0` was observed by neither leg (0 calls in the
shadow, Quick Battle and census), so it stays C3 and is reported NOT-EXEC. Default gates on this branch's exe equal
the `merge_train.json` refs: G-CTRL `5943c5f1/306186d4/52052890`, B `3a03670c/9fcdbd3a/53778664`.

PREREG `ea3f78389` (committed alone), addendum 1 `e27925b9d` (flip re-run, committed alone before the re-run arms).
The prior run (`272043875`, BLOCKED audio endpoint after leg B) is superseded. Its leg B data stays under the `_heap`
tags and is not cited for any promotion.

## Build and root

- `orch/integration` @ `4ee1823e1` merged into `orch/D4-WSB-C4-SOLV-TRK1` with `git -c merge.autoStash=false merge`
  (merge commit `c65466382`, no conflict). 22 files under `mashedmod/` changed since `da3604323`, so every leg was re-run.
- Built in the worktree, `MASHED_NO_DEPLOY=1`, Build OK: `mashed_re.exe`
  `409ac992fb9b7465e87c04314c96f720f52dd675fab4aaed5a30f10566cdf371`, `mashed_re_dev.asi`
  `ee83d072235ef1aecf3863da86ef59ece224dbf5ec5d9ba38327a19f185ac5dd`. The main `original/` was not touched.
- Private root `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root`: `original/mashed_re_dev.asi` replaced `fbdef276` -> `ee83d072`
  (hash checked after the copy).
- Audio rule: `drive_trk1.py` checked `Get-PnpDevice -Class AudioEndpoint -PresentOnly` inside each solo window before
  and after every runner. Every row and every leg: `OK Philips FTV (NVIDIA High Definition Audio)` before and after,
  0 MASHED.exe Event 1000 records, no AV at `0x005baf48`, no U-9677 re-run. All runs wrapped in `test-slot` by hand.

## Leg B: heap-tracked write-set shadow (8 rows), `SHADOW_TAG=heapflip`

Census (track 0, mode 10, 4 cars, `HOLD=60`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`,
`SAMPLES_DONE=24`, hook installed alone. Tables `shadow_summary.tsv` (rows `*_heapflip`), `heap_pages_heapflip.tsv`.

| row | n / ndiff | heap pages o/n per sample | heap pages compared | stack/noise/ovf/ret max | flag lines | PATCHBYTE | read-out | game PID |
|---|---|---|---|---|---|---|---|---|
| `0x005675d0` | 24 / 0 | 1..1 / 1..1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 5184 |
| `0x00567c00` | 24 / 0 | 1..1 / 1..1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 19968 |
| `0x00567c60` | 24 / 0 | 4..4 / 4..4 | 96 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 5452 |
| `0x00567f00` | 24 / 0 | 0..0 / 0..0 | 0 | 0/0/0/0 | 0 | installed=e9 uninstalled=51 A/B-IS-REAL | CLEAN-NO-PAGES -> arm N | 8828 |
| `0x005684c0` | 24 / 0 | 3..3 / 3..3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 3736 |
| `0x00568560` | 24 / 0 | 3..3 / 3..3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 18052 |
| `0x0056bdf0` | 24 / 0 | 1..1 / 1..1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=53 A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 1592 |
| `0x005685f0` | 0 (no shadow log) | | | | | | NO_LOG: not observed (NOT-EXEC) | 13004 |

**Arm N, `0x00567f00`** (`MASHED_SHADOW_NEED_PAGES=1`, `HOLD=150`, `SHADOW_TAG=needpagesflip`,
`heap_pages_needpagesflip.tsv`): n 7, ndiff 0, heap pages 3..3 / 3..3, **21 heap pages compared**, stack/noise/ovf/ret
0, 0 flag lines, `installed=e9 uninstalled=51 A/B-IS-REAL`: **HEAP-CLEAN (PASS)**, game PID 8104.

Five of the seven `heapflip` shadow logs are byte-identical to the pre-flip `_heap` logs (same SHA-256). The log
carries per-sample counts only (no address, no build id, `00567c00`: 50 lines), so equal counts give equal bytes.
Each is a new file written 2026-10-29 by a new game PID on root `.asi` `ee83d072`.

## Racediff (8 rows), `C4_TAG=_flip`

`verify/wsbsolvws_20261027/c4_racediff_v7.py` unchanged, via `run_bvh7.py`. Quick Battle, A4 calls 300..1200, OFF boot
`--hooks __none__`, ON boot the row alone (jmp `e9`). Rows with 0/0 calls got one census run (`C4_SCENARIO=census`,
`C4_HOLD=400`, `C4_WINDOW="1 3000"`). Table `racediff_summary.tsv` (rows `*_flip`, `*_census_flip`).

| row | Quick Battle | census | off / on calls | sigs off / on | read-out | spawned PIDs (on/off) |
|---|---|---|---|---|---|---|
| `0x005675d0` | C4-CLEAN | | 6 / 6 | 1 / 1 | **PASS** | 4676/5204 |
| `0x00567c00` | C4-CLEAN | | 6 / 6 | 1 / 1 | **PASS** | 18768/13808 |
| `0x00567c60` | C4-CLEAN | | 6 / 6 | 1 / 1 | **PASS** | 21344/20028 |
| `0x00567f00` | C4-CLEAN | | 6 / 6 | 1 / 1 | **PASS** | 19816/11308 |
| `0x005684c0` | HOLD 0/0 (2 attempts) | C4-CLEAN | 2 / 6 | 1 / 1 | **PASS** | QB 18412/5400, census 8828/7464 |
| `0x00568560` | HOLD 0/0 (2 attempts) | C4-CLEAN | 2 / 5 | 1 / 1 | **PASS** | QB 4572/18716, census 6248/12772 |
| `0x0056bdf0` | C4-CLEAN | | 6 / 6 | 1 / 1 | **PASS** | 11424/3928 |
| `0x005685f0` | HOLD 0/0 (2 attempts) | HOLD 0/0 | 0 / 0 | 0 / 0 | not observed (NOT-EXEC) | QB 1756/16896, census 17340/476 |

The two census PASS rows sampled a different number of calls per side (2 off, 5 or 6 on) in frames 1..3000. The tool's
C4-CLEAN verdict compares the signature set, and both sides hold the same single signature. The count is reported
here as measured. It is not a read-out field in the PREREG.

Harness fault, not a finding: the first census attempt for `0x005684c0` died in `run_bvh7.py` before any launch
(`UnicodeEncodeError` on cp1252 printing the lock's `WAITING` line, raised because this step's gate run held the lock).
No game spawned for it. It was re-run with `PYTHONIOENCODING=utf-8` (environment only, harness unchanged): C4-CLEAN.

## Default gates (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

`A4_EXE` = this branch's `mashed_re.exe` `409ac992`, `A4_ROOT` = the private root, under `mashed_lock.py` and
`test-slot`. Every arm self-exited 0.

| arm | file | sha8 | ref (`merge_train.json` @ `4ee1823e1`) |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | `5943c5f1` / `306186d4` / `52052890` | `5943c5f1` / `306186d4` / `52052890` |
| B_DEF_K0 / K1 / K26 | step.csv | `3a03670c` / `9fcdbd3a` / `53778664` | `3a03670c` / `9fcdbd3a` / `53778664` |

## Per-RVA outcome (re-classify, CONFIDENCE.md "Write-set leg for C4")

Every row has stores outside its own frame (SCOPE list in the PREREG), so the C4 gate is the L-38 heap writer form: a
CLEAN heap-tracked shadow with more than 0 heap pages compared, next to a clean race diff. Every PASS row wrote to heap
pages in its sampled calls (stack 0), so L-40 does not apply. No stub in any body (`STUBS.md` has none of these RVAs,
no `STUB` marker in the four TUs), no `[UNCERTAIN]` marker in the eight notes. One body each: `RwpSolverLeaves1.cpp`,
`RwpSolverCore9.cpp`, `RwpSolverCore11.cpp`, `RwpSolverIntegrate6.cpp` are in both `asi_sources.rsp` and
`exe_sources.rsp`. The other hits for these RVAs (`RwpSolverPartition13.cpp`, `RwpSolverCore23.cpp`) are `extern`
call-site declarations.

| row | leg B | racediff | level |
|---|---|---|---|
| `0x005675d0` | PASS (24 pages) | PASS (QB) | **C3 -> C4** |
| `0x00567c00` | PASS (24 pages) | PASS (QB) | **C3 -> C4** |
| `0x00567c60` | PASS (96 pages) | PASS (QB) | **C3 -> C4** |
| `0x00567f00` | PASS via arm N (21 pages) | PASS (QB) | **C3 -> C4** |
| `0x005684c0` | PASS (72 pages) | PASS (census) | **C3 -> C4** |
| `0x00568560` | PASS (72 pages) | PASS (census) | **C3 -> C4** |
| `0x0056bdf0` | PASS (24 pages) | PASS (QB) | **C3 -> C4** |
| `0x005685f0` | NO_LOG (not observed) | HOLD 0/0 QB and census | stays C3, NOT-EXEC |

Trackers: `hooks.csv` (7 rows: confidence, status `verified`, scenario, `frida_diff`, a C4 note), 7 lines prepended
to `re/analysis/CHANGELOG.md`. No `UNCERTAINTIES.md` / `STUBS.md` / `DEFERRED.md` row created.

## Raw data (`D:/MashedWT/D4-WSB-C4-SOLV-TRK1_data`, SHA-256)

| file | sha256 |
|---|---|
| `build_flip.log` | `61b45f50c8e1dfb9f220dd5ac0b875a425b638af9da9d14ce8b98b994c7c9a8a` |
| `drive_trk1.log` | `45e6f94faf0d3f47b8f8cfaa832cbbedc84cdc4138ce0ed9c02a256d15ffdfb0` |
| `legB_flip.log` | `9489dd938ccacb5703256526b202cd2e87c84da70cd8083260410a886655d429` |
| `armN_flip.log` | `3d3c9833183e3aaf026640159f3eee5c8ca059b163bb3d5771c0b3427ef2f4b7` |
| `bvh_flip.log` | `4e195287756672a849076b40039eb7216e0361a70b5ddfb2d2fa3042cd53a864` |
| `bvh_census_flip.log` | `0fa37bcb87620d169a3d2929b9452c9cdf6d2eec773874ca657e3bb7fa3732dc` |
| `bvh_census_flip_005684c0.log` | `b5c390802cb55ef7b07f7643afeb973b08ededd63739fb5d039a668a1c9bf258` |
| `gates_flip.log` | `2b540a22285c7c0bcd0328767723eac1b87977d7c355d947ef88724f58c466a4` |
| `005675d0_heapflip/shadow_ab.log` | `337fb3726ae83706f30e2cfd9d73597205d8193a8a777b8fbd6dc9bface3ee50` |
| `00567c00_heapflip/shadow_ab.log` | `6fcd0dbe958d6292381e6691ddfefeda366e142d1482567e14503e3970353bae` |
| `00567c60_heapflip/shadow_ab.log` | `031d675eb8d82e1c53f11b0013f6a7af978dc664a0c8cb4056ee618110427dd9` |
| `00567f00_heapflip/shadow_ab.log` | `eacae21e20059c1b49aa2a0e0fc2617ebb605dd9b77bb1cbff4dd31f85f52347` |
| `00567f00_needpagesflip/shadow_ab.log` | `707f736c6e444864ba2fef91adc4bbb15feec4b2ca5518b12114244a7ffc8d2a` |
| `005684c0_heapflip/shadow_ab.log` | `db5ef0305c81a2d206d4f5eab14cd87bc5aec8dd45b26d4e5c2ed92975f9710c` |
| `00568560_heapflip/shadow_ab.log` | `4ae0ccf75e89c73eb51bd784653f48eade2df8a1238a72ac3203b8f4bbe681dd` |
| `0056bdf0_heapflip/shadow_ab.log` | `a6eb03de981a07a97f3397c6b64bd3f336767cf13965acb2189ed2a2eee3d561` |
| `005675d0_flip/c4_racediff_samples.json` | `da1ae178f2c7555f61710d7ba566728e87cea41606cf2e7ab33f59f36aab1e75` |
| `00567c00_flip/c4_racediff_samples.json` | `2bdcb14dac82f8dbe1a06b4bacca1a339b0b012cc30ce4534c391ed14038472d` |
| `00567c60_flip/c4_racediff_samples.json` | `daefbe81edf3bc81a564dc0d695d1f09a9d515bd020693bcd659373336882838` |
| `00567f00_flip/c4_racediff_samples.json` | `280abc6be5f37b286ef29ac3445adb189586e60d1bf3c88cb9d874696aead6c4` |
| `005684c0_census_flip/c4_racediff_samples.json` | `d394be66822df3e0c3b3cacb9ed810e8a5c8296004f08c6dd363913a385aa37d` |
| `00568560_census_flip/c4_racediff_samples.json` | `7d5f30ad1b25a056716cebd92d9d0293d32108f57574ca24925ccf662b8218b1` |
| `0056bdf0_flip/c4_racediff_samples.json` | `535ece115bab7a48449395801e5eeb20d2cf4f92e000d768659fcedac627bf15` |
| `{005684c0,00568560}_flip`, `005685f0_{flip,census_flip}` `c4_racediff_samples.json` (empty, 0/0) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `gates_flip/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates_flip/H_DEF.gates.csv` | `306186d44ce7193b93b53a12e1658b871e90bb276480c2add7d8c9f7c08e6a2e` |
| `gates_flip/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates_flip/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates_flip/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates_flip/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

## Files

Committed: PREREG, addendum 1, this RESULT, the harness copies (unchanged), `shadow_summary.tsv`,
`heap_pages_heapflip.tsv`, `heap_pages_needpagesflip.tsv`, `racediff_summary.tsv`, `hooks.csv`,
`re/analysis/CHANGELOG.md`. No source change on the branch. Spawned game PIDs were all killed by their runners or
self-exited, and none remained at the end (the one MASHED.exe left running belongs to `D2-BAND-42S_root`, not this
step). No watcher left running.
