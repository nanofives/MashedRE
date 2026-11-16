# RESULT RO-REF-FIXED-LOAD (run 51, 2026-11-16)

Worker cmwiwa61n2p2bp015emg26zwg, lead cmwiw96jp2orzp015u321hacn. Branch `orch/RO-REF-FIXED-LOAD` from `orch/integration`
`7630e53ca`. Row kind: original-side runs plus a harness option (`re/frida/detorig.py`, `re/frida/js/detorig.js`). No exe build,
no G-CTRL, no B, no 12 captures, no tracker, mask, hooks.csv or ROADMAP change. Nothing pushed, nothing enqueued.
L-167 answer (a): the harness holds the track-load length and the boot count k fixed.

## 0. Verdict: GREEN

Three fresh plain runs on the reference recipe plus `--read-wait --read-wait-rtfs` (s1, s2, s3) have equal load length
(108 main-thread clock calls, warp to race anchor) and equal k (QPC 222 at tick 1, k = 0). **Every pair is IDENTICAL-FOR-N
600 of 600, N aligned** (no `--n-ok`), with the control ignore set (`--masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv
--ignore 0x00614400-0x00614800,0x00771800-0x00771c00`, RESULT_RO-REF-CONTROL-NOISE section 3 / RESULT_RO-CTL-WARP-N section 0):

| pair | load calls | QPC at tick 1 | N misaligned | IDENTICAL-FOR-N |
|---|---|---|---|---|
| s1:s2 | 108 / 108 | 222 / 222 | none | **600 of 600** |
| s1:s3 | 108 / 108 | 222 / 222 | none | **600 of 600** |
| s2:s3 | 108 / 108 | 222 / 222 | none | **600 of 600** |

(`fixload_pairs_s.txt`.) The only blocks listed are the ignored `0x00771800` and, in the two s2 pairs, `0x007dcc00` at tick 308,
marked `[masked]` by the tool (whole block dropped by its mask route), which does not end the window.

The recipe the lead should use for reference runs from now on:
`detorig.py --clock tick --boot --q0 0x40000000 --tick-raw <control ranges> --tick-cap 600 --warp-tick 300 --warp-clock 300
--normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --read-wait --read-wait-rtfs` (`verify/ro_20261116/fixload_arms.py`).
Each run's `meta.json` now carries `bootlen` (N, QPC and timeGetTime counts at tick 1) and the read-wait counters, so equal k
and load length can be checked per run (`race.dn_warp`, `bootlen.qpc`).

## 1. Cause of both counts: the piz read poll (static read plus measurement)

Disassembly of `FUN_004b67e0` (`MASHED.exe.unpatched`, anchor `BDCAE093...3C0E`, capstone via
`verify/ro_20261115/osword_rvadis.py 0x004b67e0-0x004b68e0`):

- `0x004b683e` `lea edx, [esp+0x10]` (the OVERLAPPED), `0x004b685d` `call [0x5cc1a4]` (ReadFile), `0x004b6863`
  `call [0x5cc0cc]` (GetLastError, result not tested), `0x004b686d` `mov ebp, [0x5cc100]` (GetOverlappedResult).
- Loop `0x004b6873`: `0x004b6875` `mov eax, [0x7d3e5c]`, `0x004b688f` `call eax` (the progress callback, `FUN_00429290`,
  registered at `0x004027c2`, RESULT_PROMOTE-DETORIG-LOADHOLD), then `0x004b68a2` `call ebp` with `push 0` (bWait 0) at
  `0x004b6899`, `0x004b68a6` GetLastError, `0x004b68b0` `cmp eax, 0x3e4` / `0x004b68b7` `jmp 0x4b6873`.
- So the callback runs once per poll, at least once per read (the loop is entered unconditionally after ReadFile), and the
  number of polls is how long the read takes.
- `FUN_00429290`: `0x00429292` `call 0x45d3f0` (to `FUN_005a60b0`, the QPC at `0x005a60bf`), `0x00429297` `call 0x4967e0`,
  `0x004292a4` `call 0x429240` (the `0x007f1010` += 1/60 write, RESULT_RO-CTL-WARP-N section 3).

Measured in the boot call logs (`fixload_bootstack.txt`, `fixload_bootstack.py`): the boot calls whose stack scan holds `0x00429297`
and those holding `0x00428af4` move together: p1 128 / 131, v2 93 / 96, p2 87 / 90 (RO-CTL-WARP-N runs). The records alternate
callback chain / `0x00428af4` (p1 records 151..159). So the k extra boot iterations of RESULT_RO-CTL-WARP-N section 4 are
extra polls of boot piz reads, the same mechanism as the load length.
[UNCERTAIN] that the `0x00428af4` call is made inside `FUN_00429290`'s call to `FUN_004967e0`: no record holds both return
addresses within its 14 stack dwords. Evidence: the counts move together and alternate. Not needed for the result: the
read wait fixes both counts (section 3). Register line added.

## 2. What did not work

- **f1, sleep holds** (`--load-sleep-ms 50 --boot-sleep-ms 50`, the `--boot-sleep-ms` option is new here): QPC 230 at tick 1
  (k = 4) and 131 load calls, both above every unheld run of RO-CTL-WARP-N (`fixload_census.txt`). A 50 ms sleep per clock
  call does not bound the read time. [UNCERTAIN] why the reads took longer than 50 ms in f1: the run started under the
  test-slot memory cap after a 10 min memory wait (26 to 28 GB committed). Not tested further: the read wait replaces the
  sleep. The option stays (default 0) because it is harmless and documented.
- **r1_av, c1, Interceptor form**: `Interceptor.attach(0x004b6894)` calling GetOverlappedResult(.., bWait 1) from JavaScript.
  Both runs crashed: `access-violation` at `0x004b6b40` (read of `0x015a1073`, `edi` = `0x0158100f`) right after
  `call 0x4b67e0` at `0x004b6b3b` returned (`c1_av/crash.jsonl`). [UNCERTAIN] the mechanism (edi is restored by `pop edi` at
  `0x004b68c9`). Not tested further: the IAT form below needs no mid-function hook.

## 3. The harness option

`--read-wait` (`detorig.js` `readWaitArm`, armed at the install, the PE entry point in boot mode): the IAT slot `0x005cc100`
(GetOverlappedResult) points at a native stub. When the return address is `0x004b68a4` (the game's call in the piz poll loop)
it sets bWait (`[esp+0x10]`) to 1, so the first poll waits for the read. One callback per read, independent of the disk.
Other callers jump to the real API unchanged. A byte scan of `MASHED.exe.unpatched` for `00 c1 5c 00` finds exactly two
readers of the slot: `0x004b686f` (the `mov ebp` above) and `0x005510f1` (`call [0x5cc100]` at `0x005510ef`).

`--read-wait-rtfs` also forces the second caller, `0x005510ef` (return `0x005510f5`) in `FUN_00551090`, which polls when its
state `+0x20 == 3` (`0x005510a8`) and takes bWait from its argument (`mov edx, [esp+0x14]` `0x005510dd`). It is needed for
the race half:

| set | runs | k (QPC at tick 1) | load calls | pairs at 600 (N aligned) | where the others end |
|---|---|---|---|---|---|
| `--read-wait` only | r1..r4 | 222 in all 4 | 108 in all 4 | 0 of 6 | ticks 304..308, block `0x007dc400` (classes `MASK:rw-fs`, `PTR`, not covered by `--tick-raw`) and N misaligned at 307..310 (`fixload_pairs_r.txt`) |
| `--read-wait --read-wait-rtfs` | s1..s3 | 222 in all 3 | 108 in all 3 | **3 of 3** | none |

Counters per run (`fixload_census.txt`): 93 piz polls forced before tick 1 and 190 by the end in every r and s run (the number of
reads is constant). The number found still pending (the reads the wait actually blocked on) varies from 0 to 35, which is the
disk variance the option removes. s1..s3: 17 RtFS polls in every run, 4 of them pending.

Timing only, no game memory is written: the kernel completes the OVERLAPPED the game owns, as it would on a later poll.
Measured: r1 (`--read-wait`) against the control-era p2 (RO-CTL-WARP-N, no hold, 108 / k = 0) is IDENTICAL-FOR-N 600 of 600,
N aligned (`fixload_pair_r1_p2.txt`). So a held run equals an unheld run that happened to get the same counts.

With masks only (no `--ignore`), s1:s2 2 and s1:s3 5 of 600 end on `0x00771800`, as RESULT_RO-CTL-OSWORD section 4 predicts
(the block is not raw-covered, so the dump route cannot drop it at every tick). s2:s3 is 600 (`fixload_pairs_s_masksonly.txt`).
Adding `0x00771800-0x00771c00` to the recipe's `--tick-raw` is the fix OSWORD names. Not done here, the DONE line uses the
control ignore set.

## 4. Other observations

- s2 made 2 more timeGetTime calls before tick 1 (tgt 14 against 12, the DirectShow site of RESULT_RO-CTL-WARP-N section 2).
  It reaches only the N column and cancels out of the compare (N is compared from the race anchor). s2 scores 600 with both
  others.
- The tick-31 `0x0077391c` flip (RO-CTL-FOCUSWORD) did not occur in any of the 8 runs.
- RO-CTL-LOADLEN, RO-CTL-BOOTLEN and RO-REF-TICKRAW-771800 were not claimed. This RESULT answers what RO-CTL-LOADLEN and
  RO-CTL-BOOTLEN ask for in mechanism (section 1) at the level the reference needs, without a watch run.

## 5. What was run

- Root `D:\MashedWT\RO-REF-FIXED-LOAD_root`: robocopy of `D:\MashedWT\RO-CTL-WARP-N_root`. `MASHED.exe` `b9977dab...fe87`,
  `MASHED.exe.unpatched` `BDCAE093...3C0E`, `gamesave.bin` `bd187881...2a70` (checked, the control's files).
- `verify/ro_20261116/fixload_arms.py`: the RO-CTL-WARP-N p-run recipe (`warpn_arms.sh`, `--ckpt-ticks 0,1,17,18,299,301,600`,
  `--call-log-boot --call-log`) with env `RW` / `FS` / `LS` / `BS`. Each launch under test-slot (outside) and mashed_lock
  (inside, label RO-REF-FIXED-LOAD). Every run waited 10 min for memory and started under the cap. No piz-collision crash.
- Runs: f1 (LS 50, BS 50), r1_av (Interceptor form, AV), c1 (same plus `--crash-catch 30`, AV), r1..r4 (RW=1 FS=0), s1..s3
  (RW=1 FS=1). All r and s runs rc 0, 600 records, 22 checkpoints.

## Evidence

Committed in `verify/ro_20261116/`: this RESULT, `fixload_arms.py`, `fixload_bootstack.py`, `fixload_bootstack.txt`,
`fixload_census.txt`, `fixload_pairs_s.txt`, `fixload_pairs_r.txt`, `fixload_pairs_s_masksonly.txt`, `fixload_pair_r1_p2.txt`,
`raw_sha256_RO-REF-FIXED-LOAD.txt` (SHA-256 of 58 raw files). Harness: `re/frida/detorig.py` (`--read-wait`,
`--read-wait-rtfs`, `--boot-sleep-ms`, `bootlen` / `done` in `meta.json`), `re/frida/js/detorig.js` (`readWaitArm`, boot hold,
`bootlen` event).

Raw in `D:/MashedWT/RO-REF-FIXED-LOAD_data` (not committed): `{f1,r1_av,c1_av,r1..r4,s1..s3}/meta.json`, `ticks.bin`,
`calllog_boot.bin`, `calllog.bin`, checkpoints, `*.log`, `pair_*.json`, `pairs*.txt`, `census.txt`.

| file | SHA-256 |
|---|---|
| `s1/meta.json` | `28c51beaac18416b23c424b48cacee70f70a3e995fe76a124f7638a9261e7333` |
| `s2/meta.json` | `8b1f9e0d2048c5d2e082b6c1382ca7eebdc60da26346b7a6cad6667fdb50f4dc` |
| `s3/meta.json` | `68f70dc09a6c660d282c1f4531a4ddcb8a697dd52e4abb6cff2e18e7e194ea9b` |
| `s1/ticks.bin` | `1b698398c12d86ce0491e237655741de4e47a510159462e6b30312b5175ef934` |
| `s2/ticks.bin` | `e43467e640e0c8991e6067e2aad080d009587fdd6cad3d3f81b45d9068309254` |
| `s3/ticks.bin` | `51279caf387a3b717a5058ebdefdbba076f84be56a71fbeedf362ca877129e51` |
| `pairs_s.txt` | `1732802329315a6e535785dec39e277d351f1d2dee1e2b7102e2c381bba5fedf` |

Gates: not run (original-side row, no exe change).
