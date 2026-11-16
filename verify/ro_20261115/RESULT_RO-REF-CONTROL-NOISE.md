# RESULT RO-REF-CONTROL-NOISE (run 49, 2026-11-15)

Worker cmwi62a900th7p0156fk8mkj5, lead cmwi60lew0t01p0150jyu4c1t. Branch `orch/RO-REF-CONTROL-NOISE` from `orch/integration`
`38a6f3ae5`. Row kind: original-side oracle runs (harness / measurement). No build, no G-CTRL, no B, no 12 captures, no mask
file, tracker, hooks.csv, ROADMAP or `.asi` change. Nothing pushed.

## 0. Verdict: DECISION (the control is not clean, so no reference race is named)

Seven fresh original runs of the RO-ORACLE-GATE reference race (5 plain, 2 with o2's install-time watch) plus o2 and og1 give
**no clean original-vs-original pair under the oracle's current rules**. Every plain pair diverges within ticks 0..9. The noise has
five named parts. Three of them repeat in every run and are run-to-run nondeterminism of the original or rules of the compare tool,
not o2-specific harness effects:

| # | source | where | seen in | kind |
|---|---|---|---|---|
| 1 | `0x006147d8` 0/1 flips per tick | block `0x00614400` | all 9 runs, every pair | original nondeterminism (window-message flag, already `MASK:os`), counted only because the tick verdict ignores a NEW block only when its whole 1 KB is masked |
| 2 | `0x00771a00` 0/1 flips per tick | block `0x00771800` | all 9 runs | original nondeterminism, unmasked; writers `0x00493ef0` and `0x00494794` sit next to the `0x006147d8` writers `0x00493eeb` / `0x0049479a` |
| 3 | per-tick clock-call count N misaligned at record 0, then blocks `0x007f1000` / `0x007d7000` NEW at tick 300 (the warp) | N column, `0x007f1000`, `0x007d7000` | 8 of 10 plain pairs | [UNCERTAIN] source, see section 3 |
| 4 | "+9000" timer mode: `0x007f1030` +9000, `0x00771998..0x007719a0` +3, `0x007f1010` +393 | `0x007f1000`, `0x00771800` | o2 and cw2 (2 of 3 watched runs), 0 of 6 plain runs | associated with the debug-register watch, [UNCERTAIN] cause |
| 5 | `0x0068bc2c` = 4 | `0x0068bc00` | og1 only (1 of 9) | the RO-CTL-68BC0C copied-matrix pad family, dword 15 of record 1, outside that row's two-word mask |

With (1) and (2) left out (`--ignore`) and N alignment not required (`--n-ok`), 2 of 10 plain pairs are IDENTICAL-FOR-N 600 of
600 (c1:c5, c3:c4). 7 are 300 of 600 (divergence at the warp, tick 300). 1 is 8 of 600 (c1:c3, block `0x007e9400` at tick 8).

**The run-46 control (o2 vs og1) carried parts 1, 4 and 5 together.** o2 is a watched run in the +9000 mode, and og1 holds the
pad-word outlier. Neither is a representative plain run.

## 1. What was run

- Root `D:\MashedWT\RO-REF-CONTROL-NOISE_root`: a robocopy of `D:\MashedWT\RO-ORACLE-GATE_root` (og1's root), so `MASHED.exe` is
  `b9977dab...fe87`, `gamesave.bin` `bd187881...`, `videocfg.bin` `f87df3c8...`, the same files as og1 and o2 (checked).
- Script `verify/ro_20261115/ctl_arms.sh`: `oraclegate/og_arms.sh` with this row's root, data folder and lock label. Each run is
  `detorig.py --clock tick --boot --q0 0x40000000 --tick-raw <og1 ranges> --tick-cap 600 --warp-tick 300 --warp-clock 300 --normalize
  re/frida/warp_normalize.tsv --load-sleep-ms 0 --ckpt-ticks 0,1,17,18,299,301,600`, under test-slot (outside) and mashed_lock (inside).
- Plain: c1, c2, c3, c4, c5. Watched like o2: cw1, cw2 with `--watch 0x00898c00,0x00604604 --watch-from install` (o2's `meta.json`
  `boot.install` shows that watch armed at the install, `watch_hits` 532; cw1 and cw2 also log 532 hits).
- All 7 exit rc 0, 600 records x 3245 blocks, 22 checkpoints. The first c1 / c2 attempts exited 2 on a path bug in my script
  (`cd ../../..` one level too far, no game launched) and were re-run. No piz-collision crash.
- Compared with o2 (`D:/MashedWT/RO-FIRST-DIVERGENCE_data/o2`) and og1 (`D:/MashedWT/RO-ORACLE-GATE_data/og1`).

Per-run facts (`census_all.txt`, from `meta.json` and the tick-1 dumps):

| run | watch | warp N | race N | timer.before[0] | `0x00771998` @1 | `0x007f1030` @1 | `0x0068bc2c` |
|---|---|---|---|---|---|---|---|
| o2 | yes | 3527 | 3636 | 338,076,077 | 107,698 | 323,076,547 | 0 |
| og1 | no | 3524 | 3637 | 338,067,077 | 107,695 | 323,067,547 | 4 |
| c1 | no | 3523 | 3632 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| c2 | no | 3522 | 3630 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| c3 | no | 3525 | 3635 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| c4 | no | 3522 | 3632 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| c5 | no | 3524 | 3633 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| cw1 | yes | 3524 | 3634 | 338,067,077 | 107,695 | 323,067,547 | 0 |
| cw2 | yes | 3528 | 3637 | 338,076,077 | 107,698 | 323,076,547 | 0 |

## 2. Parts 1 and 2: two 0/1 words that flip in every run

`dumpdiff_nonptr.txt` (`dumpdiff.py` over the tick 1, 17, 18, 299, 301, 600 dumps of the 8 fresh-root and og1 runs) lists, outside
pointer-range values (zeroed by the heap rule in the tick hash):

- `0x006147d8` and `0x00771a00` hold 0 or 1, with no pattern across runs or ticks. Example, tick 17: `0x00771a00` c1 0, c2 1, c3 1,
  c4 0, c5 1, og1 1, cw1 0, cw2 1.
- Nothing else in blocks `0x00614400` and `0x00771800` differs between the plain runs, apart from the part-4 counters.
- In raw range `0x00614400..0x00614800` (recorded every tick) the only differing word between plain runs over all 600 records is
  `0x006147d8` (`rawdiff.py`, pairs c1:c2, c1:c3, c4:c5, c2:c5).

Absolute-operand referrers (`mask_referrers.py`, the scan of `gen_replay_masks.py`, binary `MASHED.exe.unpatched`):

- `0x00771a00`: reads `0x00493c90`, `0x00493cb9`, `0x00493eb7`, `0x00493edf`. Writes `0x00493ef0` (`mov [0x771a00], ecx`) and
  `0x00494794` (`mov [0x771a00], ebx`, in FUN_004944c0, hooks.csv `util`). The four `0x00493cxx..0x00493efx` sites have no Ghidra
  function (`no-function`).
- `0x006147d8` is `MASK:os` in `replay_masks.tsv` with writers `0x00493eeb` and `0x0049479a` (RESULT_RO-ORACLE-GATE section 6).
- [UNCERTAIN] That `0x00771a00` belongs to the same window-message state as `0x006147d8`. Evidence: adjacent writers only. Missing:
  the disassembly of `0x00493e80..0x00493f00` and `0x00494780..0x004947a0`. Test row RO-CTL-OSWORD.

Why part 1 still counts although `0x006147d8` is masked: `detorig_ticks.py` `--masks` help (line 41) says a NEW block is ignored
only when its 1 KB range is entirely MASK dwords. [UNCERTAIN] for the code path (read, not stepped). Measured effect: adding
`--ignore 0x00614400-0x00614800,0x00771800-0x00771c00` removes both blocks from every pair (`tk/*_os+771a*.txt`).

## 3. Part 3: N misalignment, and the warp at tick 300

`detorig_ticks.py --masks warp_normalize.tsv,replay_masks.tsv --ignore <parts 1 and 2>`:

| pair | N aligned | IDENTICAL-FOR-N | with `--n-ok` | NEW blocks left (with `--n-ok`) |
|---|---|---|---|---|
| c1:c2 | misaligned at 0 | 0 | 300 | `0x007d7000`, `0x007f1000` @300 |
| c1:c3 | misaligned at 0 | 0 | 8 | `0x007e9400` @8, `0x007f1000` @300, `0x007dcc00` / `0x007dd000` @301 |
| c1:c4 | misaligned at 0 | 0 | 300 | `0x007d7000`, `0x007f1000` @300 |
| c1:c5 | aligned | **600** | 600 | (`0x007dcc00` / `0x007dd000` @313 listed, not ending the window) |
| c2:c3 | misaligned at 0 | 0 | 300 | `0x007f1000` @300, `0x007dcc00` / `0x007dd000` @301 |
| c2:c4 | misaligned at 0 | 0 | 300 | `0x007d7000`, `0x007f1000` @300 |
| c2:c5 | misaligned at 0 | 0 | 300 | `0x007d7000`, `0x007f1000` @300, `0x007dcc00` / `0x007dd000` @313 |
| c3:c4 | aligned | **600** | 600 | (`0x007dcc00` / `0x007dd000` @301 listed, not ending the window) |
| c3:c5 | misaligned at 0 | 0 | 300 | `0x007f1000` @300, `0x007dcc00` / `0x007dd000` @301 |
| c4:c5 | misaligned at 0 | 0 | 300 | `0x007d7000`, `0x007f1000` @300, `0x007dcc00` / `0x007dd000` @313 |

- Every N-misaligned plain pair diverges at tick 300 on `0x007f1000` (the frame-timer block), and both N-aligned pairs do not. So
  the race half (ticks 300..599) is clean only in the two pairs whose per-tick clock-call counts agree.
- [UNCERTAIN] What makes the per-tick clock-call count differ between plain runs of the same scripted boot, and why that reaches
  `0x007f1000` at the warp although `warp_normalize.tsv` pins the timer. Missing: a per-tick N listing of an aligned and a misaligned
  pair, and the `0x007f1000` / `0x007d7000` words at ticks 299 / 301 against the warp pins. Test row RO-CTL-WARP-N.
- `0x007dcc00` / `0x007dd000` / `0x007f0c00` change in one record (301, 308 or 313) of a pair, which matches RESULT_RO-ORACLE-GATE
  section 6 (record 301 only for o2:og1). Not classified here (part of RO-CTL-WARP-N).

## 4. Part 4: the +9000 timer mode

- o2 and cw2 (both watched) hold timer.before `[338076077, 335375109, 48932, 112692, 111758]`. The other 7 runs, cw1 (watched)
  included, hold `[338067077, 335366109, 48932, 112689, 111755]`. The differences are 9,000 / 9,000 / 0 / 3 / 3.
- The same split shows in `0x00771998` / `0x0077199c` / `0x007719a0` (+3, writers `0x00492ed5`, `0x0049334f`, `0x00492f19`,
  `0x00492ecd` in FUN_00492e90, hooks.csv `boot`), `0x007f102c` / `0x007f1030` (+9,000) and `0x007f1010` (+393 at ticks 17..299).
- The two +9000 runs have the two highest warp N (3527, 3528). The plain runs span 3522..3525.
- So the mode is not o2-only: it reproduced in cw2. It appeared in 2 of 3 watched runs and 0 of 6 plain runs.
- [UNCERTAIN] That the watch causes it: 3 watched samples, and cw1 did not show it. Missing: more watched and plain runs. Part of
  RO-CTL-WARP-N. A plain reference does not need the watch.

## 5. Part 5: `0x0068bc2c` in og1

- og1 holds `0x0068bc2c` = `0x00000004` (signed 4) at every record. o2 and all 7 fresh runs hold 0.
- `0x0068bc2c` = `0x0068bb60 + 1*0x68 + 0x64`, which is dword 15 of record 1's `mat` copy (record +0x28, 16 dwords, the `rep movsd` at
  `0x0045a736` in FUN_0045a6b0, RESULT_RO-CTL-68BC0C section 1).
- RESULT_RO-CTL-68BC0C section 3 found 4 / pointer values in the 4-dword and +0x2c pad words of record 1 in 160 of 411 runs, all in
  one window (2026-11-11), and masked only `0x0068bc0c` / `0x0068bc1c` in `prov_compare.py`. Its section 4 lists `0x0068bc2c`
  among the pad words that "stay scored", because it was 0 in every surveyed run. og1 is the first run seen with it non-zero.
- `detorig_firstdiv.py` / `detorig_ticks.py` read `replay_masks.tsv`, which has no row for `0x0068bc0c..0x0068bc2c`.
  [UNCERTAIN] whether the oracle should mask the pad family. That is a mask decision for the lead (row RO-CTL-PAD-68BC).

## 6. `gen_replay_masks.py` and `0x00914700 MASK:audio`

Measured on the current head, not only read:

- Regenerating (`py -3.12 re/tools/gen_replay_masks.py --facts <main checkout>/log/ghidra_funcfacts.jsonl --out <scratch>`, facts
  SHA-256 `8ee4c0d8...f057`) gives the committed `re/frida/replay_masks.tsv` plus exactly one row,
  `0x00914700 1 0 4 MASK:audio`, and the count line 1560 / 388 → 1561 / 389. RO-ORACLE-GATE run 46 reported the same.
- `mask_referrers.py 0x00914700 --rev e116c666b` (`mask_referrers_00914700.txt`). The dword has 8 absolute-operand referrers:
  - FUN_005a6190: `0x005a6199` (write), `0x005a61a9` (write).
  - FUN_005ac210: `0x005ac2ec` (read).
  - FUN_005b8200: `0x005b823d`, `0x005b82e3`, `0x005b83e2`, `0x005b8466`, `0x005b853d` (reads).
  - At `e116c666b` (RO-MASKS, the commit that generated the file) FUN_005b8200 was `third-party-library[d3dx9-fx]` in hooks.csv.
  - At the current head it is `audio`.
  - FUN_005a6190 and FUN_005ac210 were `audio` at both commits.
- The change is `870f7a5f8` (2026-11-13, MATCH-RWC-UNCERT-3: "refile 0x005b8200/0x005b8680 d3dx9-fx -> audio (P2, L-127)"), with
  the tracker leg redone in `3a86c8469`.
- `gen_replay_masks.py` lines 90-95 mask a dword only when every referrer maps to one bound family (`names` has no None and
  length 1). At RO-MASKS the referrer families were {audio, d3dx9}, so `0x00914700` was neither masked nor listed as shared
  (line 94 needs a None in `names`). After the refile all referrers are `audio` (line 30 maps the prefix `audio` to family `audio`),
  so the dword is masked.
- So the row is generator drift from a P2 refile, not a new finding about the dword. `0x00914700` holds `0x00000010` (16) in every
  run of this control at ticks 1 and 600, so it adds no noise to an original-vs-original compare.
- Not regenerated here. The committed file stays as RO-ORACLE-GATE left it. Regenerating is a one-line row (RO-MASKS-REGEN). That
  row should also say whether a P2 refile that moves mask membership needs a regenerate step.

## 7. Recommendation (filed as a decision)

Do not name a reference race until both of these hold:

1. **RO-CTL-OSWORD:**
   - Make the tick verdict drop MASK words inside partly masked blocks. It has to apply the masks per dword in the raw ranges and
     per dump-explained block, the same way `detorig_firstdiv` explains its CONSTANT blocks.
   - Mask `0x00771a00` as `MASK:os` once its writers are read.
2. **RO-CTL-WARP-N:** name why the per-tick clock-call count differs between plain runs, and what reaches `0x007f1000` /
   `0x007d7000` at the warp.

Then name a fresh **plain** run as the reference (not o2: watched and in the +9000 mode; not og1: pad-word outlier) and require a
second plain run against it to score IDENTICAL-FOR-N 600 of 600. Until then c1:c5 and c3:c4 show that clean plain pairs exist.
The reference stays undecided.

## Evidence

Committed in `verify/ro_20261115/`:

- this RESULT
- `ctl_arms.sh`, `ctl_census.py`, `dumpdiff.py`, `mask_referrers.py`
- `census_all.txt`, `dumpdiff_nonptr.txt`, `mask_referrers_00914700.txt`
- `raw_sha256_RO-REF-CONTROL-NOISE.txt`, which holds the SHA-256 of every raw file below, all in `D:/MashedWT/RO-REF-CONTROL-NOISE_data`

Raw, not committed:

- `{c1..c5,cw1,cw2}/meta.json` and `/ticks.bin`, with the run logs `*.log`
- `batch1.out`, `batch2.out`
- `census_all.txt`, `dumpdiff_plain.txt`, `rawdiff_c4_c5.txt`
- `tk/tk_*.txt` (detorig_ticks scorings), `fd/fd_*.md` (detorig_firstdiv reports)

Selected hashes:

| file | SHA-256 |
|---|---|
| `c1/meta.json` | `3de3724ffb25a912d68fcf915248cec02ded4b96183b091b500a23112612807d` |
| `c1/ticks.bin` | `ac707406d41f23cbbf6cb031e49022f5c5ee662c0d76b9f761e626b64772cb13` |
| `cw2/meta.json` | `af7166257c0242fce5eef02cdc63ea7cdcb43dc6f2130dc4538b604dba730914` |
| `census_all.txt` | `812c8e91ff7d71e2c2798d31ec94327cbe866760e77ba0efa1c931bd46d9bc25` |
| `dumpdiff_plain.txt` | `bf7dcdce5c436e76ee875414df32ac1d2e05a50672bd54e15ab375f6f7cd7844` |
| `rawdiff_c4_c5.txt` | `8b7cc4180f11356da6446d431099e1b0b69666e631bd9006c25c0363bfc95bbd` |

o2 and og1 raw hashes are in `verify/replayoracle_20261113/raw_sha256_RO-ORACLE-GATE_run46.txt`.

Gates: not run (original-side measurement row, no exe change).
