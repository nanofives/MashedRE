# RESULT D4-WSB-C4-SOLV-TRK1: BLOCKED (audio endpoint), leg B done, racediff not run

Verdict: **BLOCKED 'audio endpoint'.** Leg B (heap-tracked write-set shadow) ran on all 8 rows. Then, before the next
launch (arm N for `0x00567f00`), `Get-PnpDevice -Class AudioEndpoint -PresentOnly` listed no endpoint (01:56:44Z), so
the step stopped under the lead's rule (b): no launch, no retry. The racediff leg did not run for any row. **No row
changes level** (PREREG: C4 needs both legs), no tracker edit, no source change. Default gates not run (no promotion
to gate, no source change on the branch).

Branch `orch/D4-WSB-C4-SOLV-TRK1` off `orch/integration` @ `da3604323`. PREREG `ea3f78389` (committed alone), harness
copies `971940ec1` (RGN's `drive_rgn.py` / `run_shadow6.py` / `run_bvh6.py` and SOLV-WS's `heap_pages.py`, data path /
label / runner names only).

## Build and root

- The shared STACKONLY build (`779bc9c3` / `c6a264a3`) is not this branch's source: `git diff --stat 0d1285dc7
  da3604323 -- mashedmod/` lists 20 files (D5 merges). So this branch built its own:
  `mashed_re.exe` `47737fe41cb76e4dced528bff54efd5cfcddbede3ff2ac7ed6c5eea59ac85531`,
  `mashed_re_dev.asi` `fbdef276614c0f1a5bf3978252b77426280f5445175412210edd3c0acbad2444` (`MASHED_NO_DEPLOY=1`,
  Build OK). The `.asi` and `.map` sit in this worktree's ignored `mashedmod/build/`.
- Private root `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root` = robocopy of `D:/MashedWT/D4-WSB-STACKONLY-C3_root`, its
  `original/mashed_re_dev.asi` replaced by `fbdef276...` (hash checked after the copy). The main `original/` was
  not touched.
- Lock and audio rule: `drive_trk1.py` takes one solo window per row (`mashed_lock.py run --solo`), checks the
  endpoint inside the window before the runner and after it, and reads the MASHED.exe Event 1000 records. Every leg B
  row: `OK Philips FTV (NVIDIA High Definition Audio)` before and after, 0 crash events, no U-9677 re-run. Wrapped in
  `test-slot` by hand.

## Leg B: heap-tracked write-set shadow (all 8 rows), `run_shadow7.py`

Census (track 0, mode 10, 4 cars, `HOLD=60`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`,
`SAMPLES_DONE=24`, `SHADOW_TAG=heap`, hook installed alone. Summaries `shadow_summary.tsv`, `heap_pages.tsv`.

| row | n / ndiff | pages o/n per sample | heap pages compared | stack/noise/ovf/ret max | flag lines | PATCHBYTE | leg B read-out | game PID |
|---|---|---|---|---|---|---|---|---|
| `0x005675d0` | 24 / 0 | 2..2 / 2..2 | 48 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 860 |
| `0x00567c00` | 24 / 0 | 1..1 / 1..1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 15904 |
| `0x00567c60` | 24 / 0 | 4..4 / 4..4 | 96 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 964 |
| `0x00567f00` | 24 / 0 | 0..0 / 0..0 | 0 | 0/0/0/0 | 0 | installed=e9 uninstalled=51 A/B-IS-REAL | CLEAN-NO-PAGES (arm N owed) | 6904 |
| `0x005684c0` | 24 / 0 | 3..3 / 3..3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 21312 |
| `0x00568560` | 24 / 0 | 3..3 / 3..3 | 72 | 0/0/0/0 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 2688 |
| `0x0056bdf0` | 24 / 0 | 1..1 / 1..1 | 24 | 0/0/0/0 | 0 | installed=e9 uninstalled=53 A/B-IS-REAL | **HEAP-CLEAN (PASS)** | 7428 |
| `0x005685f0` | 0 (no shadow log) | | | | | | **NO_LOG: not observed (NOT-EXEC)** | 17524 |

`0x005685f0`: the boot reached a running race (phase 3, 4 cars grounded, 60 s hold run to the end, transcript
`005685f0_heap/shadow_transcript.txt`) and the wrapper wrote no shadow log, so the sampled 60 s held 0 calls. This
matches the census NOT-EXEC row (`verify/wsbcensus_20261026/wsb_census.tsv`: `NOT-EXEC`, 0/3 menu, 0/3 race). It
stays C3 and is reported NOT-EXEC.

## Arm N (`0x00567f00`) and racediff (8 rows): not run

- Arm N, `drive_trk1.py shadow 00567f00` with `MASHED_SHADOW_NEED_PAGES=1 HOLD=150 SHADOW_TAG=needpages`: window
  opened 01:56:41Z, pre-launch endpoint check `<none>` at 01:56:44Z, BLOCKED, window closed. No game spawned.
- Racediff, `drive_trk1.py bvh <8 rows>`: started by the same command line right after (the exit code of the first
  call was masked by `tee`), window opened 01:56:45Z, pre-launch check `<none>` at 01:56:47Z, BLOCKED, window
  closed. No game spawned. This was the driver's pre-launch check, not a retry of a launch.
- Re-check at 01:56:59Z: `Get-PnpDevice -Class AudioEndpoint -PresentOnly` returns no object (CIM query found no
  `Win32_PnPEntity` with `PNPClass = 'AudioEndpoint'` and `Present = TRUE`). The AV at `0x005baf48` did not occur:
  no launch happened after the endpoint dropped.

## Per-RVA outcome

| row | leg B | racediff | level |
|---|---|---|---|
| `0x005675d0` | PASS | not run (BLOCKED) | stays C3 |
| `0x00567c00` | PASS | not run (BLOCKED) | stays C3 |
| `0x00567c60` | PASS | not run (BLOCKED) | stays C3 |
| `0x00567f00` | CLEAN-NO-PAGES, arm N not run (BLOCKED) | not run (BLOCKED) | stays C3 |
| `0x005684c0` | PASS | not run (BLOCKED) | stays C3 |
| `0x00568560` | PASS | not run (BLOCKED) | stays C3 |
| `0x0056bdf0` | PASS | not run (BLOCKED) | stays C3 |
| `0x005685f0` | NO_LOG, not observed (NOT-EXEC) | not run (BLOCKED) | stays C3, NOT-EXEC |

## Re-pickup (once an audio endpoint is present)

Same branch, same build, same root, PREREG unchanged:

1. `MASHED_ROOT=D:\MashedWT\D4-WSB-C4-SOLV-TRK1_root MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 MASHED_SHADOW_NEED_PAGES=1 SAMPLES_DONE=24 SHADOW_TAG=needpages HOLD=150 test-slot bash -c 'py -3.12 verify/wsbc4trk1_20261028/drive_trk1.py shadow 00567f00'`
2. `MASHED_ROOT=... test-slot bash -c 'py -3.12 verify/wsbc4trk1_20261028/drive_trk1.py bvh 005675d0 00567c00 00567c60 00567f00 005684c0 00568560 0056bdf0 005685f0'`,
   then `C4_SCENARIO=census C4_HOLD=400 C4_WINDOW="1 3000"` for rows with 0/0 calls.
3. Default gates on `47737fe4` (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`), then
   re-classify for rows with both legs PASS.

If `orch/integration` moves under `mashedmod/` before the re-pickup, merge it in, rebuild, and re-run leg B too (the
leg B evidence is tied to `fbdef276`).

## Raw data (`D:/MashedWT/D4-WSB-C4-SOLV-TRK1_data`, SHA-256)

| file | sha256 |
|---|---|
| `drive_trk1.log` | `b7765e9296a18b7a3d81705943cb7ea056c0e34f296615f09d1b34e343fe54dc` |
| `legB.log` | `b5c93814d8c93cb97b6fc5a72165524763eb020811c0994c8408d392508243b4` |
| `armN.log` (BLOCKED, no launch) | `16e6a6549102e82ac3c142b170213967a2e21ec4c3ef9caeddbda3e3e9e8f462` |
| `bvh.log` (BLOCKED, no launch) | `98637bc71d319674686f5f2e2ab3c984574edbf405d8d9fbd616e974002a2870` |
| `005675d0_heap/shadow_ab.log` | `d69758d953d58d2f483a6f9b11dbc21fc4425c011f678fb214c3c02514bc6ca7` |
| `00567c00_heap/shadow_ab.log` | `6fcd0dbe958d6292381e6691ddfefeda366e142d1482567e14503e3970353bae` |
| `00567c60_heap/shadow_ab.log` | `61962e47a625091f2112327cebead4a90d2afbc68cc557da0a24a5ed7bf4f0c6` |
| `00567f00_heap/shadow_ab.log` | `eacae21e20059c1b49aa2a0e0fc2617ebb605dd9b77bb1cbff4dd31f85f52347` |
| `005684c0_heap/shadow_ab.log` | `db5ef0305c81a2d206d4f5eab14cd87bc5aec8dd45b26d4e5c2ed92975f9710c` |
| `00568560_heap/shadow_ab.log` | `4ae0ccf75e89c73eb51bd784653f48eade2df8a1238a72ac3203b8f4bbe681dd` |
| `0056bdf0_heap/shadow_ab.log` | `a6eb03de981a07a97f3397c6b64bd3f336767cf13965acb2189ed2a2eee3d561` |

## Files

Committed: PREREG, this RESULT, `drive_trk1.py`, `run_shadow7.py`, `run_bvh7.py`, `heap_pages.py`,
`shadow_summary.tsv`, `heap_pages.tsv`. Spawned game PIDs (each killed by the runner's SAMPLES-DONE guard or
self-exited, all confirmed gone): 860, 15904, 964, 6904, 21312, 2688, 7428, 17524. No watcher left running.
