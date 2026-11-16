# RESULT RO-CTL-WARP-N (run 50, 2026-11-16)

Worker cmwil020d1tinp015fhqowg0g, lead cmwikyu1t1t3vp015u1ze18rm. Branch `orch/RO-CTL-WARP-N` from `orch/integration`
`9a44eee36`. Row kind: original-side runs plus diagnosis. No exe build, no G-CTRL, no B, no 12 captures, no tracker, mask,
hooks.csv or ROADMAP change. Nothing pushed. Evidence row read: RESULT_RO-REF-CONTROL-NOISE (branch `orch/RO-REF-CONTROL-NOISE`,
`b97bcef65`), sections 3 and 4. No reference race is named here (L-167 open).

## 0. Verdict: GREEN (both sources named, from measurement; what ends the two loops stays [UNCERTAIN], rows filed)

| question | source (measured) |
|---|---|
| N misaligned at record 0 | the **track-load length**: the number of main-loop iterations in phase 2 (warp to race anchor). `detorig_ticks.py` compares N relative to the race anchor (line 48, `N - N_anchor`), so a different load length shows from record 0. The boot clock-call difference cancels out of that compare. |
| what the load length writes | `0x007f1010` += 1/60 once per load iteration (`fld` `0x004291b6`, `fadd [0x005cd694]` = `0x3c888889` = 0.016666668 at `0x004291bc`, `fstp` `0x004291cf`, FUN_00428d30), and `0x007d7108` += 102 per load iteration (`add ecx, ebx` `0x004e1017`, `mov [0x7d7108], ecx` `0x004e1019`). The first is overwritten on the first race tick (`fstp [0x7f1010]` `0x0049344b`, FUN_00493390), the second persists to tick 600. |
| the "+9000" timer mode | **k extra boot main-loop iterations before the first tick**. Each costs one QPC at `0x00428af4` (FUN_00428a30 via FUN_004950b0) and one at `0x005a60bf` (FUN_005a60b0 via FUN_005a6ea0 / FUN_005ab040). With the boot clock at 15,000 Q per call that is 30,000 Q = +9,000 on `0x007f1030` / `0x007f102c` and +3 on `0x00771998` / `0x0077199c` per extra iteration. "+9000" is k = 1. Measured k = 6 (v2) and k = 41 (p1). |

Measured check of the first row: with the RO-REF-CONTROL-NOISE ignore set, the pair p2:v3 (both 108 load calls, k = 0)
scores **IDENTICAL-FOR-N 600 of 600, N aligned**. p2:w2 (108 / 109) and w2:p3 (109 / 111) are misaligned at 0 and end at
tick 300 on `0x007f1010`. The control's two clean pairs are the two with equal load length (c1:c5 109 / 109, c3:c4 110 / 110).
Every other control pair differs in load length.

## 1. What was run

- Root `D:\MashedWT\RO-CTL-WARP-N_root`: robocopy of `D:\MashedWT\RO-REF-CONTROL-NOISE_root`. `MASHED.exe` `b9977dab...fe87`,
  `gamesave.bin` `bd187881...`, `videocfg.bin` `f87df3c8...`, the same files as the control (checked). Anchor
  `MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E` (checked).
- Harness change (this branch): `detorig.py --call-log-boot` / `detorig.js cfg.callLogBoot` turn the existing clock-call log
  on at the install (PE entry point) and send it at the first clock call of tick 1 (`calllog_boot.bin`), then reset the index
  so `--call-log` (warp to race anchor) still starts at record 0. `detorig_calllog.py --boot` reads it. The log is the same
  64-byte record as `--call-log`: N, kind (0 timeGetTime / GetTickCount, 1 QPC), 14 stack dwords from the stub entry.
- `verify/ro_20261115/warpn_arms.sh`: the control's `ctl_arms.sh` with this row's root, data folder and lock label. Every
  run: `detorig.py --clock tick --boot --q0 0x40000000 --tick-raw <control ranges> --tick-cap 600 --warp-tick 300
  --warp-clock 300 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --call-log-boot --call-log`, under test-slot
  (outside) and mashed_lock (inside). All 9 runs exit rc 0, no piz-collision crash.
  - p1: pilot, no `--ckpt-ticks` (15 race checkpoints only).
  - p2, p3: plain, `--ckpt-ticks 0,1,17,18,299,301,600` (as the control).
  - w1, w2: as p2 plus `--watch 0x00771968,0x007f1010 --watch-from install` (writers of the phase byte and of `0x007f1010`).
  - v1..v4: as p2 plus o2's watch `--watch 0x00898c00,0x00604604 --watch-from install`.
- Scripts: `warpn_census.py` (per-tick N from `ticks.bin`, words of `0x007f1000` / `0x007d7000` at the tick checkpoints),
  `warpn_sites.py` (per-call-site counts per run, boot and load segments). Outputs committed:
  `warpn_census_control.txt` (the 7 control runs), `warpn_census_RO-CTL-WARP-N.txt`, `warpn_sites_RO-CTL-WARP-N.txt`,
  `warpn_ticks_pairs_RO-CTL-WARP-N.txt`.

## 2. Where N differs, and where it does not

From the control's own `ticks.bin` (`warpn_census_control.txt`) and this row's runs (`warpn_census_RO-CTL-WARP-N.txt`):

- Per-tick clock-call count `N[k+1] - N[k]` is identical in every run from record 0 to record 298 (histogram 11 x 588,
  12 x 9, 22 x 1 in all 15 runs). `N[299] - N[0]` = 3290 in all 15 runs.
- The runs differ in two places only:
  1. **Boot** (install to the first tick): the control's per-kind counters at the tick-1 checkpoint hold QPC = 222 in all 5
     plain runs and timeGetTime = 10..13. The timeGetTime surplus is one site (`warpn_sites`): `0x0049d089` (FUN_0049d080,
     hooks.csv `third-party-library[directshow-baseclasses]`) called from `0x0049cc16` (FUN_0049cbd0), 5..8 calls per run, plus
     1 call at `0x0049cfcd` (FUN_0049cfb0) from `0x0049cb70` in v4 only. These calls advance only T (timeGetTime), and
     `0x007f1000` is identical at ticks 0 / 1 / 17 / 18 / 299 between the k = 0 runs, so this part reaches the N column only.
     It cancels out of `detorig_ticks` (N is compared relative to the race anchor).
  2. **Load** (warp to race anchor, inside tick 300): 108..125 calls. Exactly one site varies: `0x005ab058` (FUN_005ab040) from
     `0x005a6ea9` (FUN_005a6ea0) from `0x005a60bf` (FUN_005a60b0, hooks.csv `audio`), 97..114 calls. The other 11 load calls
     are the same in all 9 runs (`warpn_sites`, `== calllog`).
- `dn_warp` per run: control c1 109, c2 108, c3 110, c4 110, c5 109, cw1 110, cw2 109; this row p1 119, p2 108, p3 111, w1 110,
  w2 109, v1 125, v2 111, v3 108, v4 108.

## 3. What the load length writes (the tick-300 divergence)

Watch hits (w1, w2, `watch.jsonl`, main thread):

- Phase byte `0x00771968`: written 0 at pc after `0x00492502` (N 2), 1 at pc after `0x0049279a` (N 222), 3 at pc after
  `0x00492a76`, i.e. `mov dword ptr [0x771968], 3` at `0x00492a6c`, right after `call 0x0040d440` at `0x00492a67`. w1 at N 3633,
  w2 at N 3632 (the race anchor's N, one call earlier than the anchor's own call).
- `0x007f1010` in phase 2: one write per load iteration at `fstp dword ptr [0x7f1010]` `0x004291cf` (FUN_00428d30), each
  +0.0166667 (`fadd dword ptr [0x5cd694]`, `.rdata` value `0x3c888889`). Stack dwords of each hit: `0x0042926f` (FUN_00429240),
  `0x004292a9` (FUN_00429290), `0x004b6891` / `0x004b69e6`. w1: 106 writes, w2: 105 writes. Before the warp `0x007f1010` is
  written only at `0x0049344b` (FUN_00493390), once per tick.
- On the first race tick FUN_00493390 overwrites it (`0x0049344b`): w1 and w2 both hold `0x42e0c81f` at tick 301, `0x42e0d0a7`
  at 302, `0x42e414ec` at 400. At tick 600 `0x007f1000` has no differing dword in any run of either set. So the
  `0x007f1000` NEW block is the one record that sees the end-of-load value (record 300, hashed at FUN_00493390's entry
  before its store).
- `0x007d7108`: 35,226 at tick 299 in every k = 0 run. At tick 301: p2 / v3 / v4 (108 load calls) 46,134, w2 (109) 46,236, w1 (110)
  46,338, p3 (111) 46,440, v1 (125) 47,868, i.e. +102 per extra load call. The control matches (c2 108: 46,134, c1 / c5 109:
  46,236, c3 / c4 / cw1 110: 46,338). The value is unchanged from tick 301 to tick 600, so this word
  keeps the load length for the whole race. Referrers (`mask_referrers.py`, `MASHED.exe.unpatched`): write `0x004e0943`
  (FUN_004e0920, `third-party-library[renderware]`), read / write `0x004e0f29`, `0x004e0f4e` (`mov [0x7d7108], 0`),
  `0x004e0fea`, `0x004e100d`, `0x004e1019` (no Ghidra function at the last five).
- `0x007f1000` and `0x007d7000` are the two blocks the control saw NEW at tick 300. The other `0x007d7000` words that differ
  between runs hold values in the heap-pointer range and are zeroed by the tick hash (detorig.js tick CModule,
  `v >= 0x00995000 && v < 0x30000000 && !(v & 3)`), and `0x007d70c8` (0..3 in every run from tick 1). They do not decide the
  verdict: p2:v3 holds them different and scores 600 of 600. [UNCERTAIN] whether `0x007d70c8` is in a masked or pointer
  rule; it is not needed for the result.

## 4. The "+9000" timer mode

- The control's cw2 (and o2) hold QPC = 224 at the tick-1 checkpoint against 222 (control `meta.json` checkpoint 201 counters,
  `warpn_census_control.txt` source). Before the first tick the virtual QPC steps 15,000 counts per call (`bootQstep`), so 2 extra
  QPC calls = 30,000 Q. FUN_004950b0 scales Q to 3,000,000 / QPF (QPF = 10,000,000, `stats_end.qpf`), so 30,000 Q = 9,000 on
  the sample `0x007f1030` and the mark `0x007f102c`, and 9,000 / 3,000 = 3 on `0x00771998` / `0x0077199c` (the `pinTimer`
  relation in detorig.js, from `0x00492ecd`). That is the whole +9000 / +3 split of RESULT_RO-REF-CONTROL-NOISE section 4.
- Measured with the boot call log in this row:
  - v2: QPC 234, sites `0x00428af4` 96 (others 90) and `0x005a60bf` 90 (others 84). 6 extra iterations, 12 extra QPC, warp
    `timer.before` `[338121077, 335420109, 48932, 112707, 111773]` = +54,000 / +54,000 / 0 / +18 / +18 against k = 0.
  - p1: QPC 304, `0x00428af4` 131, `0x005a60bf` 125. 41 extra iterations, warp `timer.before[0]` 338,436,077 = +369,000 = 41 x 9,000.
  - All other sites are equal in count. So each extra boot iteration is one call at each of the two sites.
- `0x00428af4` is the return of `call 0x004950b0` at `0x00428aef` in FUN_00428a30. The next two instructions are
  `sub eax, [0x0067d858]` (`0x00428af4`) and `cmp eax, 0x16e3600` (24,000,000, `0x00428afa`).
- Watch association: timer offset k > 0 in o2, cw2, v2 (3 of 9 watched runs: o2, cw1, cw2, w1, w2, v1..v4) and in p1 (no
  watch, no tick checkpoints). It is not specific to the watch: p1 has the largest k with no watch.
- `0x007d7108` also carries k: +1,626 at tick 299 in cw2 (k = 1), +9,600 in v2 (k = 6), unchanged to tick 600.
- [UNCERTAIN] That o2's and cw2's 2 extra QPC calls are at `0x00428af4` and `0x005a60bf`. Evidence: the counter arithmetic
  (2 QPC = 9,000) and the measured k = 6 and k = 41 runs. Missing: a boot call log of a k = 1 run. It needs no separate row:
  RO-CTL-BOOTLEN below logs the boot of every run it makes.

## 5. Other observations (not part of this row's two sources)

- v4 against p2 and v3 (all three 108 load calls, k = 0): IDENTICAL-FOR-N 31 of 600, ended by RAW `0x0077391c` 1 / 0 at
  tick 31. `0x0077391c` is the focus-pause gate word the `no_focus_pause` patch reads (`0x004996d3`, CLAUDE.md). Filed as
  RO-CTL-FOCUSWORD.
- The control's `0x007dcc00` / `0x007dd000` / `0x007f0c00` single-record changes (301, 308, 313) are not classified here.

## 6. Rows filed (tests for the [UNCERTAIN] parts)

- **RO-CTL-LOADLEN**: name what decides the number of phase-2 iterations before `mov [0x771968], 3` at `0x00492a6c`
  (after `call 0x0040d440` at `0x00492a67`), 108..125 in this row. Test: a watch or call log on the reads that gate
  `0x00492a67` in a pair with different `dn_warp`.
- **RO-CTL-BOOTLEN**: name what decides the number of boot iterations of FUN_00428a30 / FUN_005a60b0 before the first entry of
  FUN_00493390 (90 / 84 in most runs, 96 / 90 in v2, 131 / 125 in p1). Test: boot call log plus a watch on `0x0067d858` and the
  first-tick gate, over runs until a k > 0 run is caught.
- **RO-CTL-FOCUSWORD**: `0x0077391c` 1 / 0 at tick 31 in v4 against p2 / v3.

## 7. What this means for the reference (no reference named, L-167)

A plain pair is clean only when its two runs have the same load length and the same k. Two harness choices remove the two
sources without masks: (a) a load hold that makes the phase-2 iteration count fixed (the `--load-sleep-ms` path exists,
default 50, run at 0 here and in the control) or a rule that compares `0x007f1010` / `0x007d7108` and the N column after
re-basing on the load length; (b) a boot rule that makes k fixed, or that re-pins `0x007f1030` / `0x007f102c` /
`0x00771998` / `0x0077199c` / `0x007d7108` from k. Choosing between them is the lead's decision. Not decided here.

## Evidence

Committed in `verify/ro_20261115/`: this RESULT, `warpn_arms.sh`, `warpn_census.py`, `warpn_sites.py`, `mask_referrers.py`
(copy from `b97bcef65`), `warpn_census_control.txt`, `warpn_census_RO-CTL-WARP-N.txt`, `warpn_sites_RO-CTL-WARP-N.txt`,
`warpn_ticks_pairs_RO-CTL-WARP-N.txt`, `raw_sha256_RO-CTL-WARP-N.txt` (SHA-256 of every raw file below, 57 rows).
Harness: `re/frida/detorig.py`, `re/frida/js/detorig.js`, `re/frida/detorig_calllog.py` (`--call-log-boot` / `--boot`).

Raw in `D:/MashedWT/RO-CTL-WARP-N_data` (not committed): `{p1,p2,p3,w1,w2,v1..v4}/meta.json`, `ticks.bin`, `calllog_boot.bin`,
`calllog.bin`, `watch.jsonl` (w, v runs), tick checkpoints, `*.log`, `pilot.out`, `batch1.out`, `batch2.out`,
`census_existing.txt`, `sites_batch1.txt`, `sites_all.txt`, `ticks_pairs.txt`. Control raw: `D:/MashedWT/RO-REF-CONTROL-NOISE_data`
(hashes in that row's `raw_sha256_RO-REF-CONTROL-NOISE.txt`).

| file | SHA-256 |
|---|---|
| `p1/meta.json` | `5946a77795b701392b8b91c289e201f7561a6f4c9a17d8b4c6d27c869dfc550d` |
| `p1/calllog_boot.bin` | `73a3fcb2bd1d7b515c002b339eb7231898258e511b9026892a7e34d547eb53d1` |
| `p2/meta.json` | `366f1e58bbd91b0100425bad0a60b204452958c99228e559df387c688d0eab76` |
| `p2/calllog_boot.bin` | `146d89d66712f8d63cef70222a3c1f635917a2e2d35534271e69ee9034e2bf04` |
| `v2/meta.json` | `481ec89dde30ff1a5f4fe96ab4cb969f6b23cf928efdff80492d06500c9b19a6` |
| `v2/calllog_boot.bin` | `a3f2159681ce569769beaac830608e022f36005fb259436d49838c029375d41c` |
| `v4/watch.jsonl` | `a20c074d18621bca3c1c0dc76cfb3ccc37327577706ebd57d967e4ab5ab44195` |
| `w1/watch.jsonl` | `48407d4af43954579e2b6bae1e101bb0719e260bdad23e1d4d74bfc83a372f64` |
| `ticks_pairs.txt` | `cf1887eaac511fab103707baf2455c73dd552882039218a0f450c0c8d7409277` |

Gates: not run (original-side measurement row, no exe change).
