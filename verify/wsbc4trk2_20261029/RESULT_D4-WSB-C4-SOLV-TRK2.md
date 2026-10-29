# RESULT D4-WSB-C4-SOLV-TRK2: GREEN (10 of 12 rows C3 -> C4)

Verdict: **GREEN.** `0x0056c0a0`, `0x0056c310`, `0x0056c580`, `0x0056caa0`, `0x0056d070`, `0x0056e680`, `0x0056ef30`,
`0x0056efc0`, `0x0056f020` and `0x0056f1f0` pass both legs the PREREG fixes (`PREREG_D4-WSB-C4-SOLV-TRK2.md`, commit
`0106b027f`, committed alone) and move C3 -> C4 through re-classify. `0x0056c8e0` is DIVERGENT on the write-set leg
(2 of 24 samples, 1 ULP) and `0x0056dd40` writes no tracked page in the census (L-40). Both stay C3 with a note.
Default gates unchanged. No source change on the branch.

## Build, root, boot check

- Branch `orch/D4-WSB-C4-SOLV-TRK2` off `orch/integration` @ `34235b24c`. Own build, `MASHED_NO_DEPLOY=1`, Build OK:
  `mashed_re.exe` `cabc4efb2c391c2ca4f7c71c150d195ffce7e13f412f2fe39cca4cf95c502338`,
  `mashed_re_dev.asi` `827380d11d9c838933a3948703bb47e182d89faf1b4a9b53f7ca6704b1d8e411`.
- Private root `D:/MashedWT/D4-WSB-C4-SOLV-TRK2_root` = robocopy of `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root`, its
  `original/mashed_re_dev.asi` replaced by `827380d1...` (hash checked after the copy). The main `original/` was not
  touched and no `.asi` was deployed there.
- **Boot check (first leg)**, `bootcheck.py` under `mashed_lock.py run --solo`: endpoint
  `OK Philips FTV (NVIDIA High Definition Audio)` before launch, `MASHED_HOOK_ONLY=__none__`, PID 7320 alive at 25 s,
  0 MASHED.exe Event 1000 records, then killed (own PID). No AV at `0x005baf48`. PASS. A first attempt under a
  600 s lock wait expired in the queue (two other solo windows ahead) before any launch.
- **Audio rule:** `drive_trk2.py` checks `Get-PnpDevice -Class AudioEndpoint -PresentOnly` inside each row's solo
  window right before the runner, then again after it, and reads the Application log's MASHED.exe Event 1000 records.
  All 12 leg B windows, the arm N window, the 12 Quick Battle and 2 census racediff windows: endpoint present before
  and after, crash events none. No U-9677 re-run was needed. Same limit as RGN: a racediff invocation is an OFF and an
  ON boot in one call, so the ON boot's pre-launch check is the pair's post-check.

## Leg B: heap-tracked write-set shadow (12 rows), `run_shadow8.py`

Census (track 0, mode 10, 4 cars, `HOLD=60`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`,
`SAMPLES_DONE=24`, `SHADOW_TAG=heap`, hook installed alone. Summaries `shadow_summary.tsv`, `heap_pages.tsv`.

| row | n / ndiff | heap pages per sample (o / n) | heap pages compared | stack/noise/ovf/ret max | flag lines | PATCHBYTE | leg B read-out | game PID |
|---|---|---|---|---|---|---|---|---|
| `0x0056c0a0` | 24 / 0 | 2 / 2 | 48 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **PASS** | 13344 |
| `0x0056c310` | 24 / 0 | 2 / 2 | 48 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **PASS** | 11140 |
| `0x0056c580` | 24 / 0 | 2 / 2 | 48 | 0/0/0/0 | 0 | installed=e9 uninstalled=83 A/B-IS-REAL | **PASS** | 5296 |
| `0x0056c8e0` | 24 / **2** | 1 / 1 | 24 | 0/0/0/0 | 1 (NOISY, discarded) | installed=e9 uninstalled=83 A/B-IS-REAL | **DIVERGENT** | 18060 |
| `0x0056caa0` | 24 / 0 | 3 / 3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=81 A/B-IS-REAL | **PASS** | 5500 |
| `0x0056d070` | 24 / 0 | 2 / 2 | 48 | 0/0/0/0 | 0 | installed=e9 uninstalled=83 A/B-IS-REAL | **PASS** | 16880 |
| `0x0056dd40` | 24 / 0 | 0 / 0 | 0 | 0/0/0/0 | 0 | installed=e9 uninstalled=55 A/B-IS-REAL | CLEAN-NO-PAGES, arm N | 8040 |
| `0x0056e680` | 24 / 0 | 3 / 3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=83 A/B-IS-REAL | **PASS** | 5944 |
| `0x0056ef30` | 24 / 0 | 1 / 1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **PASS** | 15172 |
| `0x0056efc0` | 24 / 0 | 1 / 1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **PASS** | 20160 |
| `0x0056f020` | 24 / 0 | 1 / 1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **PASS** | 16936 |
| `0x0056f1f0` | 24 / 0 | 12 / 12 | 288 | 0/0/0/0 | 0 | installed=e9 uninstalled=51 A/B-IS-REAL | **PASS** | 9532 |

### `0x0056c8e0` divergence (raw `0056c8e0_heap/shadow_ab.log` lines 17-20)

Both divergent samples are on heap page `0x0f5ff000`, call idx 1121. Format `offset:pre/orig/port`
(`Core/ShadowTrack.h:798-801`), with the float32 value of orig / port:

- log sample 15: `+0xf30: bf919cb8/bf6c91b9/bf6c91b8` (-0.924098551 / -0.924098492)
- log sample 17: `+0xf34: 3c4abbe0/bebf2bc0/bebf2bbf` (-0.373380661 / -0.373380631),
  `+0xf40: 3e6ecec6/3eacf6c8/3eacf6c7` (0.337820292 / 0.337820262),
  `+0xf48: be08b540/bf098b10/bf098b11` (-0.537278175 / -0.537278235)

Every divergent dword is 1 ULP from the original. The return and the caller stack window are equal on both samples
(`stack=0 ret=0`). One more sample was discarded as NOISY (`noise=43`, line 17). Which store in either body writes
`+0xf30..+0xf48` is not established by this run (no watch-write, no Ghidra on this account). `U-9020` (numeric, open)
already names `0x0056c8e0`'s 5-term reductions as retaining the printed FADD order, and the row note says the same.
No new U-row was filed: the open row covers the body, and this RESULT is the first runtime observation of a
difference. A follow-up that ties these offsets to a store would let a lead add this evidence to U-9020.

### Arm N, `0x0056dd40`

`MASHED_SHADOW_NEED_PAGES=1`, `HOLD=150`, `SHADOW_TAG=needpages`, PID 3540: **0 counted samples**; the log holds only
`EMPTY rv=00000000` discard lines (marks up to 750). The 150 s race ran to the end (phase 3, 4 cars grounded,
`shadow_transcript.txt`). Read-out NO-SAMPLES (`heap_pages_needpages.tsv`): not PASS, stays C3 under L-40.

## Racediff (12 rows), `run_bvh8.py` -> `verify/wsbsolvws_20261027/c4_racediff_v7.py` (unchanged)

Quick Battle, A4 calls 300..1200, OFF boot `--hooks __none__`, ON boot with the row alone (`installed` = ON entry byte
`0xE9`, `c4_racediff_v7.py:401`). Summary `racediff_summary.tsv` (census rows keyed `_census`).

| row | Quick Battle | census (`C4_HOLD=400`, A4 calls 1..3000) | read-out |
|---|---|---|---|
| `0x0056c0a0` | C4-CLEAN, 6/6 calls, 1 sig each side | not needed | PASS |
| `0x0056c310` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056c580` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056c8e0` | HOLD orig-not-observed, modded-not-observed, 0/0 calls, 2 boot pairs | C4-CLEAN, 6/6, 1/1 | PASS (leg B fails) |
| `0x0056caa0` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056d070` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056dd40` | C4-CLEAN, 6/6, 1/1 | not needed | PASS (leg B not met) |
| `0x0056e680` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056ef30` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056efc0` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056f020` | C4-CLEAN, 6/6, 1/1 | not needed | PASS |
| `0x0056f1f0` | HOLD orig-not-observed, modded-not-observed, 0/0 calls, 2 boot pairs | C4-CLEAN, 6/6, 1/1 | PASS |

No HOLD with a pointer-label (LAYOUT) difference occurred. Recorded, not interpreted: `0x0056c8e0` and `0x0056f1f0`
gave 24 shadow samples each in the census `HOLD=60` boot and 0 calls in the Quick Battle A4 300..1200 window.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_EXE` = this branch's `mashed_re.exe`
(`exe=cabc4efb` printed per arm), `A4_ROOT` = the private root, under the lock and the test slot.

| arm | file | sha8 | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `364350ae` | `364350ae` |
| H_DEF | ptrace.log | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` |

## Tracker transaction (re-classify)

| row | old -> new | scenario / frida_diff |
|---|---|---|
| `0x0056c0a0`, `0x0056c310`, `0x0056c580`, `0x0056caa0`, `0x0056d070`, `0x0056e680`, `0x0056ef30`, `0x0056efc0`, `0x0056f020` | C3 impl -> **C4 verified** | `c4_racediff_bvh:QuickBattle-f300-1200+shadow_tracked_heap:track0-mode10-cars4` / `verify/wsbc4trk2_20261029/racediff_summary.tsv` |
| `0x0056f1f0` | C3 impl -> **C4 verified** | `c4_racediff_bvh:census-a1-3000+shadow_tracked_heap:track0-mode10-cars4` / same |
| `0x0056c8e0` | stays C3 | dated DIVERGENT note (values above, U-9020 named) |
| `0x0056dd40` | stays C3 | dated L-40 note (0 pages, arm N 0 counted samples) |

- Stub sweep: no `STUB` marker in `RwpSolverIntegrate6.cpp`, `RwpSolverCore7.cpp`, `RwpSolverCore8.cpp`,
  `RwpSolverCore9.cpp`, no `STUBS.md` row for the ten RVAs.
- Uncertainty sweep of the ten plates: `0x0056c310` cites U-9019 (already RESOLVED). `0x0056caa0` cites U-9127 and
  `0x0056e680` cites U-9128 / U-9129: naming / intent only, each row's Blocks cell is `none`. U-9020 (numeric, printed
  FADD order) covers `0x0056c580` and `0x0056e680` and stays open, as it did for `0x0056ed60`'s C4 in RGN. No marker is
  unfiled. The C4 gate (`re/CONFIDENCE.md`) is a clean diff on a canonical scenario with the hook live, the write-set leg
  (L-38: CLEAN with more than 0 heap pages), and no stubs.
- One body per row: each TU is in both `asi_sources.rsp` and `exe_sources.rsp` and `exe_file` names it, so the level
  covers the shipping exe. The `file` column keeps the plate path, as on the STACKONLY C4 row `0x0056b7a0`.
- hooks.csv: 12 lines changed (Edit tool on the target rows, CRLF kept, `git diff --stat` 12/12). `re/analysis/CHANGELOG.md`:
  10 lines prepended below `<!-- ENTRIES -->`. No new U- / S- / D- rows.

## Re-pickup

- `0x0056c8e0`: a watch-write run (memory `find-record-writers-with-watch-write`) on heap page `0x0f5ff000` offsets
  `+0xf30..+0xf48` in one census boot of each side, to tie the 1-ULP dwords to a store, then a fix brief under U-9020 if
  the port's FADD order is the cause.
- `0x0056dd40`: a scenario where the call writes compared memory (its census calls all return 0 with no page written).

## Raw data

`D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data`. SHA-256 of every cited raw file (logs, 13 `shadow_ab.log`, 14 racediff
`c4_racediff_result.json` / `c4_racediff_samples.json`, the six gate files, `build.log`) in `raw_sha256.txt`.
Key ones: `bootcheck.log` `10e68223c651ca13dac5...`, `0056c8e0_heap/shadow_ab.log` `98415a433b96eb5b2988...`,
`0056dd40_needpages/shadow_ab.log` `5590c491f5ee60d882d7...`, `gates/H_DEF.step.csv` `5943c5f1c3e2f3f25101...`.

## Files

Committed: PREREG (alone, `0106b027f`), harness copies `drive_trk2.py`, `run_shadow8.py`, `run_bvh8.py`, `heap_pages.py`
(`542947bbd`), `bootcheck.py`, this RESULT, `shadow_summary.tsv`, `heap_pages.tsv`, `heap_pages_needpages.tsv`,
`racediff_summary.tsv`, `raw_sha256.txt`, `hooks.csv` (12 rows), `re/analysis/CHANGELOG.md` (10 lines).

Spawned game PIDs (all killed by their runner or self-exited, own only, none left running): 7320 (boot check); 13344,
11140, 5296, 18060, 5500, 16880, 8040, 5944, 15172, 20160, 16936, 9532 (leg B); 3540 (arm N); the racediff
`spawned_pids` in `racediff_summary.tsv` (one Quick Battle boot pair per row, two for the two HOLD rows; the table keeps the last pair; census 19300/192,
16340/7060); gate arms 9172 / 20764 /
5396 / 5616 (self-exited). No watcher left running.
