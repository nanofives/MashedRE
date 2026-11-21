# RESULT REPLAY-FALLBACK-8 (run 54, goal G5, 2026-11-21)

Worker cmwqebqxg72e9nz1c7wey2wtc (claude-opus-5-5 medium), lead cmwqea6x671wxnz1cgusoou57 (REPLAY-AUDIO).
Branch `orch/REPLAY-FALLBACK-8` from `orch/integration` `77dedfece`. Row kind: match-only / replay sweep. The replay
runs inside the original and binds nothing into the exe. One object build + `ddkobj.py check` for the IDENTICAL unit
that is on the head. No G-CTRL, no B, no 12x4 captures. Method: `PREREG_REPLAY-FALLBACK-8.md` (commit `14edc7ec9`,
before the first body run) = the PROV-INSITU-REF1 harness and recipe unchanged, every body alone.

## Verdict: GREEN for the scope that ref1 can test, 2 of 8 pairs replayed, 6 NOT-EXECUTED

| listed unit | source linked (ref) | link % | neighbour | census calls | verdict (run) | write-set leg | promotion |
|---|---|---|---|---|---|---|---|
| `0x004036a0` | `8b673edc4:re/match/s01/004036a0.c` | 77.4 | `0x00403910` | 0 | NOT-EXECUTED | static empty | none |
| `0x004969a0` | `8b673edc4:re/match/s16/00496c10.c` | 66.0 | `0x00496c10` | 0 | NOT-EXECUTED | static empty | none |
| `0x00495870` | `77dedfece:re/match/s16/00495fe0.c` | 98.9 | `0x00495fe0` | 0 | NOT-EXECUTED | not run | none |
| `0x00401690` | `77dedfece:re/match/s01/00401690.c` | 60.2 | `0x004025f0` | 13 (all before the warp) | IDENTICAL-FOR-600 (`fb8_100934_001_1`, 13 calls) | dynamic pass: 855 image stores in 10 stepped calls, stepped run `ws_00401690` IDENTICAL-FOR-600 | `re/promotions/REPLAY-FALLBACK-8.tsv` |
| `0x00498810` | `77dedfece:re/match/s16/00498810.c` | 86.7 | `0x00498860` | 0 | NOT-EXECUTED | static empty | none |
| `0x0041b7a0` | `332730a8f:re/match/s04/0041c090.c` (held copy) | 30.8 | `0x0041c090` | 0 | NOT-EXECUTED | not run | none |
| `0x0041c410` | `332730a8f:re/match/s04/0041c410.c` | 99.4 | `0x0041cc50` | 10,876 (all after the warp) | IDENTICAL-FOR-600 (`fb8_100934_002_1`, 10,876 calls) | dynamic pass: 310 image stores (210 frame) in 10 stepped calls, `ws_0041c410` IDENTICAL-FOR-600 | none: already C4 (`c4=frida-replay` in hooks.csv), `c4_reclass` SKIP |
| `0x0041ce00` | `332730a8f:re/match/s04/0041d830.c` (held copy) | 38.5 | `0x0041d830` | 0 | NOT-EXECUTED | not run | none |

- Every source is linked from the file the object build compiles (the static held copies from their caller's source),
  with `git show <ref>:<path>` (`mkbodies.py`, `SOURCES.tsv`, `SOURCES_SHA256.txt`).
- NOT-EXECUTED = 0 calls in the census run (`--prov-count` over all 15 rvas of both steps, counters from boot over the
  whole 600 ticks, `D:/MashedWT/REPLAY-FALLBACK-8_data/census/prov.json`). Six of the eight listed units do not run in
  ref1's window (boot, menu, load, the first ~190 race ticks), so ref1 cannot give them a replay verdict.
- `0x00495870` (fix_joypad detour site): 0 calls with the counter JMP placed over the detour, so the detour caveat in
  the PREREG did not come into play.

## L-128 legs for `0x00401690` (the one new promotion)

Replay reference race ref1, per-tick compare against ref1/ref2 with the inline JMP live:
IDENTICAL-FOR-600 (`fb8_100934_001_1`, segments menu 300 / race 300 identical)
install-check: ok (prov_install.json `jmp_ok` true: site E9 to the counting stub, stub to the body entry)
body-executed: yes (13 calls in the verdict run, all before the warp)
no STUB: `c4_reclass.py batch` checks it (dry run below)
write-set: covered (dynamic: first 10 calls single-stepped, 855 stores all into the compared image, 0 frame, 0 outside,
in a stepped run that itself replays IDENTICAL-FOR-600; `WRITESET_fb8.tsv`)

`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/REPLAY-FALLBACK-8.tsv` (dry run): see "Checks" below.
Thin: 13 calls, all in boot/menu. C4 counts reported apart: 1 replay-verified (`c4=frida-replay`), 0 byte-matched.

## Object build: the neighbour back under byte check

Only `0x00401690` is both IDENTICAL and on the head. Its `re/match/PROVISIONAL.tsv` row (source `re/match/s01/00401690.c`,
NOT-PROVEN, DDKOBJ-UNLISTED-10) leaves the list on this branch as a replay-C4 body (run 54 merge bar), with
`prov_unlist.py`. Then `set MASHED_NO_DEPLOY=1 && set MASHED_DDKOBJ=1 && mashedmod\build.bat` (rc 0) and its
`ddkobj.py check`:

```
[ddkobj summary] IDENTICAL 1977/2037  DIFFERENT 33 [0x00401690 0x004332a0 ...]  not-in-map 27  units linked 1967/2231
  refusals 11 (externalized-static-register-convention=7,wrapper-redefines-globals=4)  provisional kept VS2022 253 units + 6 fns
  in linked units  dual body 32
BYTECHECK.tsv:
0x00401690  ___ddkreg_00401690  0x00021c10  1280  9  0  713  DIFFERENT  +0x6  False  re/match/s01/00401690.c
0x004025f0  _Orig_004025f0      0x00022110  224   4  2  0    IDENTICAL         False  re/match/s01/00401690.c
```

Against the saved head check `log\orch\run54_headcheck\build.log` (object build of 77dedfece): IDENTICAL 1976/2035 ->
1977/2037, DIFFERENT 32 -> 33, refusals 12 -> 11 (`fail:externalized-static-register-convention: re/match/s01/00401690.c`
is gone), units linked 1966 -> 1967, dual body 32 unchanged.
- Newly DIFFERENT, classified: `0x00401690` only = replay-C4 body (source `re/match/s01/00401690.c` is the replay-verified
  file, `fb8_100934_001_1`). No other unit moved.
- Pairs listed unit -> 100% neighbour still on its VS2022 body after this row: `0x00498810 -> 0x00498860` and
  `0x00495870 -> 0x00495fe0` (head; both NOT-EXECUTED on ref1, rows stay listed), and `0x004036a0 -> 0x00403910`,
  `0x004969a0 -> 0x00496c10`, `0x0041b7a0 -> 0x0041c090`, `0x0041ce00 -> 0x0041d830` (branch units, NOT-EXECUTED).
- `0x0041c410 -> 0x0041cc50`: IDENTICAL here, but its unit and its PROVISIONAL row (source `re/match/s04/0041c410.c`)
  exist only on `orch/PORT-TOP-06` 332730a8f. This row binds nothing, so the row's removal and the object-build line
  belong to the landing row PORT-TOP-LAND-06-RF, citing `fb8_100934_002_1` + `ws_0041c410`.
- Object-build outputs: `D:/MashedWT/REPLAY-FALLBACK-8_data/objbuild1/BYTECHECK.tsv`
  (265580764f1fd73842b53601018a622e0794c25ce99bc6955d38ace9ddfec718), `ddkobj_summary.json`
  (90492fe2091970bdd12df8ca6521543ba85990433af88fae7632a5a1647b4b6f), build log `build_obj.log`.

## Controls and scenario

- Private root `D:\MashedWT\REPLAY-FALLBACK-8_root` = robocopy of the ref1 root `RO-REF-FIXED-LOAD_root`. MASHED.exe
  `b9977dab...`, .unpatched `bdcae093...`, gamesave.bin `bd187881...`, d3d9.dll `47901951...`, mashed_re_dev.asi
  `906f8601...` equal to it (checked before the first run).
- c0 (no body): IDENTICAL-FOR-N 600 of 600 against ref1/ref2 (menu 300, race 300), bootlen qpc 222, dn_warp 108.
- Census: 15 counters installed, 0 failed, 9 executed.
- `test-slot` around every queue, `mashed_lock.py run` inside each arm, `mashed_lock.py status` logged before each arm
  (`lockstatus.log`). One line at 10:48:24 (arm `ws_00412190`, NOSRC step) shows the merge train's PRIORITY-HOLD; the
  driver logged it but did not wait on it itself (rule 4 slip, the arm ran inside `mashed_lock.py run`, finished 10:55).
  The object build started at a plain HELD by another session (no PRIORITY-HOLD or SOLO).

## Checks

- `c4_reclass.py batch --tsv re/promotions/REPLAY-FALLBACK-8.tsv` (dry run): `[1/1] 0x00401690 frida-replay PASS
  IDENTICAL-FOR-600`, `batch: 1 pass, 0 refused, 0 skipped`. `0x0041c410` was left out of the TSV because it is already
  C4 (a first dry run gave SKIP, no write).
- NOSRC TSV (same branch): 3 pass, 0 refused (`RESULT_PROV-INSITU-REF1-NOSRC.md`).

## What this does not cover / next

- 6 of the 8 units have no replay verdict: ref1's window never calls them. A replay on a reference that runs them
  (a longer race, or a scenario with the HUD weapons / render-quad / timer paths) is new scope: row
  REPLAY-FALLBACK-6-NOTRUN (proposed to the lead).
- No promotion was applied (no `--apply` on a branch).

## Evidence (this folder)

`SOURCES.tsv`, `SOURCES_SHA256.txt`, `VERDICTS_fb8.tsv`, `WRITESET_fb8.tsv`, `RAW_SHA256.txt` (SHA-256 of every cited
raw file: c0 ticks, census, bodies JSON, state JSONL, each verdict and write-set run's ticks.bin / prov.json /
prov_install.json / meta.json / wlog.bin), scripts `mkbodies.py`, `run_arm.py`, `driver.py`, `tables.py`, `prov_unlist.py`.
Raw data: `D:/MashedWT/REPLAY-FALLBACK-8_data`.
