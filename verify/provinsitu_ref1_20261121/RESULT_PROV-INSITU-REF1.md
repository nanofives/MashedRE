# RESULT PROV-INSITU-REF1 (run 53, 2026-11-21)

Worker cmwpwv99z5vg1nz1cawhxd9fa (claude2, claude-opus-5-5 medium), lead cmwpwu4745v0nnz1ci4gqg2l2.
Branch `orch/PROV-INSITU-REF1` from `orch/integration` `b24bba46c`. Row kind: race-path evidence. The replay runs inside
the original and binds nothing into the exe: no build, no ddkobj check, no G-CTRL, no B, no 12x4 captures. Method fixed
in `PREREG_PROV-INSITU-REF1.md` (committed before the first body run, `3372456f3`).

STATUS: IN PROGRESS (batch 1 of 3 landed). Sections below are appended per batch.

## Scenario, reference, controls

- Reference ref1 = `verify/ro_20261116/REFERENCE_RO-REF-NAME.json` (D:/MashedWT/RO-REF-NAME_data/ref1), pair ref2.
  Boot mode: per-tick hash from the first main-loop tick, warp at tick 300, 600 ticks (300 boot/menu + 300 after the
  warp, of which the load is 108 clock calls). Arms use the ref1 argv exactly (`run_arm.py`).
- **Window caveat.** IDENTICAL-FOR-600 covers boot, menu, load and the first ~190 race ticks only. The 2026-11-11
  PROV-INSITU race had 2,724 race ticks. Bodies that diverged there late (for example `0x00424eb0` at tick 1108,
  `0x00448220` at 1116) are IDENTICAL here because ref1 ends first. A verdict here does not overturn the 11-11 one.
- Private root `D:\MashedWT\PROV-INSITU-REF1_root` (robocopy of `original\`): MASHED.exe `b9977dab...fe87`, unpatched
  `bdcae093...3c0e`, gamesave.bin `bd187881...2a70`, d3d9.dll `47901951...c7be`, mashed_re_dev.asi `906f8601...a8ad`,
  each equal to the ref1 root (`RO-REF-FIXED-LOAD_root`).
- **c0** (no body): IDENTICAL-FOR-N 600 of 600 against ref1/ref2 (menu 300, race 300), dn_warp 108, bootlen qpc 222
  (ref1: 108 / 222).
- **xneg** (`--negctl 1`, RW RNG word +1 at the race anchor): DIVERGED at tick 310 (race segment), block `0x00613000`.
  PASS.
- **Boot class.** Every run of this row, c0 included, carries a different kernel32 pointer at `0x00773d60` from ref1/ref2
  (`7601e8c0` against `75d8e8c0`; MASK:os in `re/frida/replay_masks.tsv:396`, changes per reboot). prov_compare notes
  "BOOT-CLASS mismatch". The timer word `0x007f1030` matches (`13419e9b`). c0 is IDENTICAL-FOR-600 under the same
  mismatch, so the mismatch does not by itself move a verdict. A second no-body run c1 gives a same-class pair; every
  non-IDENTICAL verdict is re-scored against c0/c1 as well.
- `provlink_selftest.py`: ok=40 bad=0 skip=0.
- census (`--prov-count`, 309 counters, no body): all 309 execute; 0 have zero calls after the warp. The census run itself
  diverges from tick 300 (counter install), it gives counts only.
- Linking (`provlink.py --rvas <309>`): 300 OK, 9 UNRESOLVED = NOT-WIRED (table below).

## Batch 1: score 80+ (76 bodies)

Groups of 6 (`prov_bisect.py --group 6 --any-segment`), the 6 bodies that were wrong on 2026-11-11 as singles.
16 runs.

| class | tested | IDENTICAL-FOR-600 | DIVERGED | CRASH | NOT-EXECUTED |
|---|---|---|---|---|---|
| game | 69 | 68 | 0 | 1 | 0 |
| RW-Physics | 6 | 6 | 0 | 0 | 0 |
| RW core | 1 | 1 | 0 | 0 | 0 |
| all | 76 | 75 | 0 | 1 | 0 |

- CRASH: `0x005af690` (88.9%), run `b1_023950_009_1`, game rc `0xC0000005` (`3221225477`) after checkpoints in phase 3.
  Confirmation run with `--crash-catch` queued (batch 2 driver).
- The 6 bodies wrong on 2026-11-11 are IDENTICAL-FOR-600 here (window caveat above): `0x00468d80` (21,556 calls),
  `0x0046e9e0` (21,556), `0x00448220` (2,722), `0x00424eb0` (2,719), `0x005aba20` (118, 11-11 HANG), `0x00481e00` (4).
- Static write-set of the 75 IDENTICAL: 9 empty, 1 covered, 65 not-covered.

## NOT-WIRED (9)

| rva | class | unresolved |
|---|---|---|
| `0x005b8c40` | game | `_ROUND`, `__pow` |
| `0x00564310` | RW-Physics | `__rand` |
| `0x005c2f84` | game | `@__fload_withFB@0`, `___math_exit`, `___startOneArgErrorHandling` |
| `0x0055b080` | RW-Physics | `_SQRT` |
| `0x004e9e40` | RW core | `_RwFrameDestroy`, `_RwFrameDestroyHierarchy` |
| `0x004e99b0` | RW core | `_RpWorldInstance` |
| `0x005185a0` | game | `__CIpow` |
| `0x00574920` | RW-Physics | `_cpuid_basic_info` |
| `0x00574a20` | RW-Physics | `_cpuid_Version_info`, `_strncmp` |

## Promotion legs (L-128; applies only to the rows in `re/promotions/PROV-INSITU-REF1.tsv`)

Replay reference race ref1, per-tick compare IDENTICAL-FOR-600 against ref1/ref2 with the inline JMP live:
IDENTICAL-FOR-600 (each listed row, run named in VERDICTS.tsv)
install-check: ok (prov_install.json jmp_ok true at every listed RVA: site E9 to the counting stub, stub to the body entry)
body-executed: yes (stub call counter > 0 in the verdict run, calls_run column of VERDICTS.tsv)
write-set: covered (static store list of the DDK body, provlink write_set: no store outside the frame (empty), or only absolute stores into the compared image (covered); L-54 / L-38)

Batch 1 rows: `0x004c3c60` Vec2Normalize (empty), `0x0055c2d0` (empty), `0x00578b20` (empty), `0x0055c230` (empty),
`0x004785e0` (empty), `0x005b73b0` AudioFindExtension (empty), `0x005b03e0` (empty), `0x005b6710` (empty),
`0x0040cea0` (covered), `0x004783f0` (empty).
Thin rows (fewer than 10 calls in the verdict run, the verdict covers only those calls): `0x005b73b0` 8, `0x005b03e0` 4,
`0x005b6710` 2, `0x0040cea0` 1, `0x004783f0` 1.

## Evidence

- `VERDICTS.tsv`: one row per sweep row (verdict, run, calls, install, static write-set, 11-11 verdict).
- `sweep309.tsv`, `select309.py`, `make_batches.py`, `batch_*.txt`, `run_arm.py`, `summarize.py`.
- Raw: `D:/MashedWT/PROV-INSITU-REF1_data` (per run folder + `.log`, `*_state.jsonl`, `lockstatus.log`). SHA-256 in
  `RAW_SHA256.txt` (written at the end).
- mashed_lock status before every arm: `D:/MashedWT/PROV-INSITU-REF1_data/lockstatus.log` (free, or HELD by another session's ordinary run that the lock then waited out; no PRIORITY-HOLD or SOLO seen).
