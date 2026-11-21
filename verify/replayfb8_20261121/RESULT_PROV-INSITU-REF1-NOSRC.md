# RESULT PROV-INSITU-REF1-NOSRC (run 54, 2026-11-21)

Worker cmwqebqxg72e9nz1c7wey2wtc (claude-opus-5-5 medium), lead cmwqea6x671wxnz1cgusoou57 (REPLAY-AUDIO).
Branch `orch/REPLAY-FALLBACK-8` (shared sweep branch with REPLAY-FALLBACK-8) from `orch/integration` `77dedfece`.
Row kind: race-path replay evidence, binds nothing, no build, no gates. Method: `PREREG_REPLAY-FALLBACK-8.md`
(commit `14edc7ec9`) = the PROV-INSITU-REF1 harness and recipe, every body alone, same c0 / census as REPLAY-FALLBACK-8.

## Verdict: GREEN, 7 of 7 IDENTICAL-FOR-600, 4 pass every L-128 leg, 3 promoted (1 already C4)

| rva | source (ref) | link % | census calls | verdict (run, calls) | write-set leg | result |
|---|---|---|---|---|---|---|
| `0x0048ae00` | `77dedfece:re/match/s14/0048ae00.c` | 98.3 | 1 | IDENTICAL-FOR-600 (`nosrc_101539_001_1`, 1) | dynamic pass: 104 image stores in 1 stepped call, `ws_0048ae00` IDENTICAL-FOR-600 | already C4 (`c4=match`), left out of the TSV |
| `0x00412190` | `a3ca5e0aa:re/match/nsa/00412190.c` | 86.1 | 4 | IDENTICAL-FOR-600 (`nosrc_101539_002_1`, 4) | dynamic pass: 24 image stores (46 frame) in 4 stepped calls, `ws_00412190` IDENTICAL-FOR-600 | promoted |
| `0x0040be50` | `a3ca5e0aa:re/match/nsa/0040be50.c` | 80.9 | 871 | IDENTICAL-FOR-600 (`nosrc_101539_003_1`, 177) | dynamic covered, frame only: 34 stores, 0 image, 10 stepped calls (L-40, stays) | not promoted |
| `0x0040eee0` | `a3ca5e0aa:re/match/nsa/0040eee0.c` | 75.2 | 8 | IDENTICAL-FOR-600 (`nosrc_101539_004_1`, 8) | static covered (provlink store list: 1 absolute store into the compared image, 0 outside, 0 indirect) | promoted |
| `0x005b8200` | `f9e024005:re/match/nsb/005b8200.c` (`// TOOLCHAIN rw9044`) | 83.7 | 4 | IDENTICAL-FOR-600 (`nosrc_101539_005_1`, 4) | dynamic escapes: 2,772 stores, 404 outside the frame and the image (caller stack +0x4 x784, heap 0x19c31b20...) | not promoted |
| `0x004883b0` | `f9e024005:re/match/nsb/004883b0.c` | 18.1 | 1 | IDENTICAL-FOR-600 (`nosrc_101539_006_1`, 1) | dynamic escapes: 6 stores, 1 image (`0x0086ca20`), 2 outside (heap 0x140ca038, 0x140c9ffc) | not promoted |
| `0x0047be80` | `f9e024005:re/match/nsb/0047be80.c` | 86.4 | 2 | IDENTICAL-FOR-600 (`nosrc_101539_007_1`, 2) | static empty | promoted |

- `0x0048ae00`: the head file `re/match/s14/0048ae00.c` (SHA-256 `66cfa6d5...`) is not the copy PROV-INSITU-REF1 replayed
  (`088f11fe...`, `RAW_SHA256_SOURCES.txt` of that row). This run replays the head file. It is already C4 by match, so
  no row is written.
- The 6 other sources are byte-equal to the copies PROV-INSITU-REF1 replayed (same SHA-256), so this confirms its
  IDENTICAL verdicts with a second run each.
- Thin rows (fewer than 10 body calls in the verdict run): `0x00412190` 4, `0x0040eee0` 8, `0x0047be80` 2. The verdict
  covers only those calls.
- Window caveat (as PROV-INSITU-REF1): boot, menu, load and the first ~190 race ticks.
- MATCH-NOSRC-A2 stays parked: `0x004128f0` (CRASH 2 of 2 in PROV-INSITU-REF1) and `0x00487280` (DIVERGED@300) were not
  re-run here.

## Promotions (L-128): `re/promotions/PROV-INSITU-REF1-NOSRC.tsv`, 3 rows

Replay reference race ref1, per-tick compare against ref1/ref2 with the inline JMP live:
IDENTICAL-FOR-600 (`0x00412190`, `0x0040eee0`, `0x0047be80`; runs above)
install-check: ok (prov_install.json `jmp_ok` true at each rva)
body-executed: yes (4 / 8 / 2 calls in the verdict run)
no STUB: checked by `c4_reclass.py batch` (dry run below)
write-set: covered (`0x00412190` dynamic, stepped run IDENTICAL-FOR-600; `0x0040eee0` static covered; `0x0047be80` static empty)

`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/PROV-INSITU-REF1-NOSRC.tsv` (dry run): 0x00412190, 0x0040eee0,
0x0047be80 `frida-replay PASS IDENTICAL-FOR-600`, `batch: 3 pass, 0 refused, 0 skipped`. `0x0040eee0`'s one static store
is `0x008a94c0[ebx]` (inside the compared range 0x005ea000-0x00915008).

C4 counts reported apart: 3 replay-verified (`c4=frida-replay`), 0 byte-matched in this row.

## Controls

Shared with REPLAY-FALLBACK-8 (same root, same c0 IDENTICAL-FOR-600, same census: all 7 execute). One
`mashed_lock.py status` line at 10:48:24 (arm `ws_00412190`) showed the merge train's PRIORITY-HOLD and the driver did
not wait on it (rule 4 slip, reported in both RESULTs). That run replayed IDENTICAL-FOR-600.

## Evidence (this folder)

`VERDICTS_nosrc.tsv`, `WRITESET_nosrc.tsv`, `SOURCES.tsv`, `SOURCES_SHA256.txt`, `RAW_SHA256.txt`.
Raw data: `D:/MashedWT/REPLAY-FALLBACK-8_data` (`bodies_nosrc.json`, `nosrc_state.jsonl`, `ws_stores_nosrc.tsv`, run folders).
