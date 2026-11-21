# RESULT PROV-INSITU-REF1 (run 53, 2026-11-21)

Worker cmwpwv99z5vg1nz1cawhxd9fa (claude2, claude-opus-5-5 medium), lead cmwpwu4745v0nnz1ci4gqg2l2.
Branch `orch/PROV-INSITU-REF1` from `orch/integration` `b24bba46c`. Row kind: race-path evidence. The replay runs inside
the original and binds nothing into the exe: no build, no ddkobj check, no G-CTRL, no B, no 12x4 captures. Method fixed
in `PREREG_PROV-INSITU-REF1.md` (committed before the first body run, `3372456f3`).

## Verdict: GREEN

All 309 sweep rows have a verdict, with every DIVERGED / CRASH confirmed by a repeat run. 54 bodies pass every L-128 leg
and are listed in `re/promotions/PROV-INSITU-REF1.tsv` (route `frida-replay`, `c4_reclass.py batch` dry run: 54 pass,
0 refused). No promotion was applied (no `--apply` on a branch).

| class | IDENTICAL-FOR-600 | DIVERGED | CRASH | NOT-EXECUTED | NOT-WIRED | total |
|---|---|---|---|---|---|---|
| game | 176 | 8 | 10 | 0 | 2 | 196 |
| RW-Physics | 43 | 2 | 11 | 0 | 3 | 59 |
| RW core | 38 | 4 | 10 | 0 | 2 | 54 |
| all | 257 | 14 | 31 | 0 | 7 | 309 |

- 302 bodies were replayed. 257 of them (85%) are IDENTICAL-FOR-600. Every batch was above half IDENTICAL (99%, 88%, 74%).
- **Write-set leg** for the 257 IDENTICAL bodies:
  - static leg (provlink store list): 44 pass (39 empty, 5 covered)
  - dynamic leg on the other 213 (first 10 calls single-stepped): 13 pass. The rest: 3 are covered with compared-image
    stores but their stepped run diverged, 38 store to the frame only (L-40, they stay), 131 escape, 28 had no
    stepped call.
  - 57 pass in all. 3 of those are held back for a long-race counterexample (below), which leaves the 54 promotions.
- C4 counts, reported apart: 54 replay-verified (`c4=frida-replay`), 0 byte-matched (`c4=match` / `c4=libid`) in this row.
- **Window caveat.** IDENTICAL-FOR-600 covers boot, menu, load and the first ~190 race ticks only. The 2026-11-11
  PROV-INSITU race ran 2,724 race ticks. 20 of the 24 batch-1/2 bodies that were wrong on 11-11 are IDENTICAL here,
  including late divergers (`0x00424eb0`@1108, `0x00448220`@1116). A verdict here does not overturn a long-race one.
  So no body with a standing long-race DIVERGED on its current source is promoted: `0x00469aa0` (DIVERGED@385, source
  unchanged), `0x0046e9e0` (DIVERGED@321 after its fix) and `0x004709a0` (DIVERGED@321, source unchanged). All three
  are `refused` in `re/match/PROVISIONAL.tsv` and pass the dynamic leg here, and all three stay.

## Scenario, reference, controls

- Reference ref1 = `verify/ro_20261116/REFERENCE_RO-REF-NAME.json` (D:/MashedWT/RO-REF-NAME_data/ref1, `ticks.bin`
  `ff55f400...7bf2`), pair ref2 (`ce8abf2e...3621`), both equal to `raw_sha256_RO-REF-NAME.txt`.
  - Boot mode: per-tick hash from the first main-loop tick, warp at tick 300, 600 ticks (300 boot/menu, then 300
    after the warp, of which the load is 108 clock calls).
  - Arms use the ref1 argv exactly (`run_arm.py`), plus `--prov` / `--prov-rvas` / `--prov-count` / `--prov-wlog` /
    `--crash-catch 20` (crash repeats only).
- Compare: `re/frida/prov_compare.py <run> ref1 ref2`, from tick 0. Allowed blocks are the blocks ref1 and ref2
  disagree on (10). Bodies are installed at boot, so menu ticks are compared too.
- Private root `D:\MashedWT\PROV-INSITU-REF1_root` (robocopy of `original\`). Each binary equals the ref1 root
  (`RO-REF-FIXED-LOAD_root`): MASHED.exe `b9977dab...fe87`, unpatched `bdcae093...3c0e`, gamesave.bin
  `bd187881...2a70`, d3d9.dll `47901951...c7be`, mashed_re_dev.asi `906f8601...a8ad`.
- **c0** (no body): IDENTICAL-FOR-N 600 of 600 against ref1/ref2 (menu 300, race 300). dn_warp 108 and bootlen qpc 222,
  ref1 has 108 / 222. **c1** (second no-body run): 600 of 600 against ref1/ref2 and against c0.
- **xneg** (`--negctl 1`, RW RNG word +1 at the race anchor): DIVERGED at tick 310 (race), block `0x00613000`. PASS.
- **Boot class.** Every run of this row carries the kernel32 pointer `7601e8c0` at `0x00773d60`. ref1 and ref2 carry
  `75d8e8c0` (MASK:os, `re/frida/replay_masks.tsv:396`, changes per reboot).
  - The timer word `0x007f1030` matches (`13419e9b`).
  - c0 and c1 are IDENTICAL-FOR-600 under the mismatch.
  - Every DIVERGED run re-scored against the same-class pair c0/c1 gives the same first tick as against ref1/ref2,
    with the boot class matching (29 runs, `RESCORE_C0C1.tsv`).
- `provlink_selftest.py`: ok=40 bad=0 skip=0.
- census (`--prov-count`, 309 counters, no body): all 309 execute, and none has zero calls after the warp, so
  NOT-EXECUTED is 0. The census run itself diverges from tick 300 (counter install). It gives counts only.
- mashed_lock status before every arm (`D:/MashedWT/PROV-INSITU-REF1_data/lockstatus.log`): free, or HELD by another
  session's ordinary run that the lock then waited out. No PRIORITY-HOLD or SOLO was seen. `test-slot --status` was
  printed before each queue. Every queue ran under test-slot, with mashed_lock inside. No build was run.

## Per batch

Groups via `prov_bisect.py --any-segment`. A diverging group is split until each wrong body runs alone. Bodies wrong
on 2026-11-11 ran as singles.

| batch | class | tested | IDENTICAL | DIVERGED | CRASH | NOT-EXECUTED |
|---|---|---|---|---|---|---|
| 1 (80+, groups of 6) | game | 69 | 68 | 0 | 1 | 0 |
| 1 | RW-Physics | 6 | 6 | 0 | 0 | 0 |
| 1 | RW core | 1 | 1 | 0 | 0 | 0 |
| 2 (50-80, groups of 3) | game | 71 | 62 | 4 | 5 | 0 |
| 2 | RW-Physics | 16 | 14 | 0 | 2 | 0 |
| 2 | RW core | 15 | 14 | 0 | 1 | 0 |
| 3 (<50, groups of 2) | game | 53 | 45 | 4 | 4 | 0 |
| 3 | RW-Physics | 33 | 22 | 2 | 9 | 0 |
| 3 | RW core | 36 | 23 | 4 | 9 | 0 |
| NOT-WIRED leg (wired after the symmap rows) | game | 1 | 1 | 0 | 0 | 0 |
| NOT-WIRED leg | RW-Physics | 1 | 1 | 0 | 0 | 0 |

Batch totals: 1 = 76 tested / 75 IDENTICAL; 2 = 102 / 90; 3 = 122 / 90; NOT-WIRED leg = 2 / 2.

## DIVERGED and CRASH (every row confirmed by a repeat run)

First tick is the 600-tick record index; the warp is tick 300. For a CRASH, the fault comes from the `--crash-catch`
repeat: access, eip, where eip or the first body return address sits, and clock call n.

| rva | name | class | link % | verdict | first tick (segment) and blocks / fault | runs | 11-11 |
|---|---|---|---|---|---|---|---|
| `0x00441d40` | FUN_00441d40 | game | 69.2 | DIVERGED@302 | tick 302 race, `0x896400`, `0x896800` | 2 of 2 | DIVERGED@2 |
| `0x00443440` | FUN_00443440 | game | 58.4 | DIVERGED@496 | tick 496 race, `0x803000`, `0x89a400` | 2 of 2 | DIVERGED@196 |
| `0x0045de80` | FUN_0045de80 | game | 52.9 | DIVERGED@300 | tick 300 race, `0x68d400`..`0x68e800` (6 blocks); 1 call | 2 of 2 | - |
| `0x00467650` | VehicleWheelDrivetrainUpdate | game | 56.1 | DIVERGED@417 | tick 417 race, `0x881400`, `0x884000` | 2 of 2 | - |
| `0x0046f6c0` | VehicleWheelContactSolver | game | 44.3 | DIVERGED@301 | tick 301 race, `0x881400`..`0x883000` (6 blocks) | 2 of 2 | DIVERGED@1 |
| `0x00487180` | FUN_00487180 | game | 11.7 | DIVERGED@300 | tick 300 race, `0x6c3400`, `0x6c3800`, `0x6c3c00` | 2 of 2 | DIVERGED@0 |
| `0x004c51a0` | RwMatrixTranslate | game | 14.7 | DIVERGED@301 | tick 301 race, `0x881c00`..`0x884400` (5 blocks) | 2 of 2 | DIVERGED@1 |
| `0x005cb2a0` | RwDeviceMatrixMultiply | game | 15.6 | DIVERGED@302 | tick 302 race, `0x896400` | 2 of 2 | - |
| `0x00559040` | FUN_00559040 | RW-Physics | 11.4 | DIVERGED@302 | tick 302 race, `0x63e000` | 2 of 2 | - |
| `0x0055c000` | RwpGjkSupportMap | RW-Physics | 43.8 | DIVERGED@302 | tick 302 race, `0x913000`, `0x913400` | 2 of 2 | - |
| `0x004dc410` | FUN_004dc410 | RW core | 2.2 | DIVERGED@300 | tick 300 race, `0x6c3400`, `0x6c3800`, `0x6c3c00` | 2 of 2 | - |
| `0x0052e0e0` | FUN_0052e0e0 | RW core | 36.6 | DIVERGED@542 | tick 542 race, `0x623c00` | 2 of 2 | - |
| `0x005412d0` | FUN_005412d0 | RW core | 44.2 | DIVERGED@302 | tick 302 race, `0x7d5400`, `0x7d5800` | 2 of 2 | - |
| `0x00547230` | FUN_00547230 | RW core | 28.4 | DIVERGED@302 | tick 302 race, `0x6c3400` | 2 of 2 | - |
| `0x00420420` | FUN_00420420 | game | 79.8 | CRASH | read 0x8 at eip `0x004e66db` (ret into body +0x101 on the stack), n=3595 | 3 of 3 | - |
| `0x00470c70` | FUN_00470c70 | game | 69.8 | CRASH | read 0x9e4 at eip `0x00470680` (ret into body +0x413), n=3665 | 2 of 2 | CRASH |
| `0x00489dc0` | FUN_00489dc0 | game | 36.1 | CRASH | read 0x4 at eip `0x4` (no body address on the stack), n=3590 | 2 of 2 | - |
| `0x004c4680` | FUN_004c4680 | game | 34.5 | CRASH | read at eip `0xbf8001d3` (no body address on the stack), n=4940 | 2 of 2 | - |
| `0x005ac210` | FUN_005ac210 | game | 57.1 | CRASH | read at eip `0x8b` (ret into body +0x132), n=19 | 2 of 2 | CRASH |
| `0x005af690` | FUN_005af690 | game | 88.9 | CRASH | read 0xa74e7d04 at eip `0x005b10ad` (ret into body +0x48), n=7935 | 2 of 2 | - |
| `0x005b1260` | FUN_005b1260 | game | 63.0 | CRASH | read at eip `0x53` (no body address on the stack), n=3650 | 2 of 2 | - |
| `0x005b2190` | FUN_005b2190 | game | 61.0 | CRASH | read at eip `0x53` (no body address on the stack), n=3740 | 2 of 2 | - |
| `0x005b3b80` | FUN_005b3b80 | game | 43.4 | CRASH | read 0x1 in body +0x104, n=7949 | 2 of 2 | CRASH |
| `0x005ba7f0` | FUN_005ba7f0 | game | 18.9 | CRASH | read 0x30 at eip `0x005aabb4` (no body address on the stack), n=49 | 2 of 2 | - |
| `0x0055b4a0` | FUN_0055b4a0 | RW-Physics | 50.8 | CRASH | read 0x0 in body +0x82, n=3675 | 2 of 2 | - |
| `0x0055b940` | PhysicsBodySetMass | RW-Physics | 23.4 | CRASH | read at eip `0x0` (no body address on the stack), n=3571 | 2 of 2 | - |
| `0x0055bbf0` | FUN_0055bbf0 | RW-Physics | 15.9 | CRASH | read 0xc in body +0x1c, n=3571 | 2 of 2 | - |
| `0x0055f800` | FUN_0055f800 | RW-Physics | 65.3 | CRASH | read at eip `0x55` (no body address on the stack), n=3678 | 2 of 2 | - |
| `0x00568990` | FUN_00568990 | RW-Physics | 40.6 | CRASH | read 0x4148ace4 in body +0x32, n=13498 | 2 of 2 | - |
| `0x0056a450` | FUN_0056a450 | RW-Physics | 3.3 | CRASH | read 0x1154d76 at eip `0x004447d3` (no body address on the stack), n=13556 | 2 of 2 | - |
| `0x0056a7a0` | FUN_0056a7a0 | RW-Physics | 7.0 | CRASH | read 0xb3dbb84 in body +0x1d, n=13532 | 2 of 2 | - |
| `0x0056adb0` | FUN_0056adb0 | RW-Physics | 10.6 | CRASH | read 0xc in body +0x150, n=3676 | 2 of 2 | - |
| `0x0056f350` | FUN_0056f350 | RW-Physics | 15.0 | CRASH | read 0xc0033513 in body +0x15f, n=13533 | 2 of 2 | - |
| `0x00578610` | FUN_00578610 | RW-Physics | 22.1 | CRASH | read 0x3d0faaa3 in body +0x1ea, n=13499 | 2 of 2 | - |
| `0x00578ff0` | FUN_00578ff0 | RW-Physics | 13.8 | CRASH | read 0xf4af000 in body +0x610, n=13554 | 2 of 2 | - |
| `0x004cf2b0` | FUN_004cf2b0 | RW core | 47.8 | CRASH | read at eip `0x0` (no body address on the stack), n=3565 | 2 of 2 | - |
| `0x004dc370` | FUN_004dc370 | RW core | 2.0 | CRASH | read 0x1e at eip `0x0048214a` (no body address on the stack), n=3664 | 2 of 2 | - |
| `0x004e8a10` | FUN_004e8a10 | RW core | 58.6 | CRASH | write 0x0 in body +0x14b, n=215 | 2 of 2 | - |
| `0x004ea220` | FUN_004ea220 | RW core | 46.5 | CRASH | read at eip `0x41bc205c` (no body address on the stack), n=3556 | 2 of 2 | - |
| `0x004ea9a0` | FUN_004ea9a0 | RW core | 30.1 | CRASH | read 0x8 in body +0x1e4, n=3525 | 2 of 2 | - |
| `0x00534d00` | FUN_00534d00 | RW core | 23.4 | CRASH | read at eip `0xa1` (no body address on the stack), n=3635 | 2 of 2 | - |
| `0x00539900` | FUN_00539900 | RW core | 12.4 | CRASH | read 0xc0c00000 in body +0x2d, n=13500 | 2 of 2 | - |
| `0x00539ec0` | FUN_00539ec0 | RW core | 12.4 | CRASH | read 0x35f6fc in body +0xc1, n=3645 | 2 of 2 | - |
| `0x005405c0` | FUN_005405c0 | RW core | 23.8 | CRASH | read 0x43 at eip `0x004d6e19` (ret into body +0xa1), n=3636 | 2 of 2 | - |
| `0x00550130` | FUN_00550130 | RW core | 18.3 | CRASH | write 0x1 at eip `0x004cc6bd` (ret into body +0x10), n=2 | 2 of 2 | - |

- `0x00420420` faults at `MASHED.exe+0xe66db`, which is the U-9677 piz-collision signature.
  - It was run 3 times, and all 3 runs fault there.
  - The root is private: no other process loads this root's piz files.
  - The body's return address (+0x101) is on the stack.
  - It is counted as CRASH, not as a collision.
- Per-run raw verdicts: `*_state.jsonl` (raw folder). Run names per row: `VERDICTS.tsv`.

## NOT-WIRED leg (lead request)

provlink resolves CRT helpers through `re/tools/match/equiv_symmap.tsv`. Each name was looked up in the libid rows of
`hooks.csv`. The address the original body calls was taken from the original bytes (`notwired_calls.py`, capstone
over the Ghidra extent of `MASHED.exe.unpatched`).

| rva | class | symbol | resolved to | after |
|---|---|---|---|---|
| `0x00564310` | RW-Physics | `__rand` | `0x005c229b`, hooks.csv row `005c229b` libid identical `_rand` (libcmt rand.obj); original calls it at `0x00564317` | IDENTICAL-FOR-600 |
| `0x005185a0` | game | `__CIpow` | `0x004a2d70`, hooks.csv row `004a2d70` libid identical `__CIpow` (libcmt pow.obj); the original calls `_pow` `0x004a2d30` (15 sites), not this entry | IDENTICAL-FOR-600 |
| `0x00487280` (extra batch) | game | `__CIacos` | `0x004a3370`, hooks.csv row `004a3370` libid identical `__CIacos` (libcmt acos.obj); the original calls `0x004a3384` (row `CRT::acos`, C2, no libid) at `0x00487c2a` | DIVERGED@300 |
| `0x005b8c40` | game | `__pow` / `_ROUND` | `__pow` -> `0x004a2d30` (row `004a2d30` libid `_pow`; original call `0x005b91c2`); `_ROUND` unresolved (no libid row) | NOT-WIRED |
| `0x005c2f84` | game | `@__fload_withFB@0` / `___math_exit` / `___startOneArgErrorHandling` | `@__fload_withFB@0` -> `0x004a70d5` (row `004a70d5` libid `__fload_withFB`; original call `0x005c2f88`); `___math_exit` unresolved (the libid row `004a713b` is `__math_exit`, one underscore less, and the original body does not call it); `___startOneArgErrorHandling` unresolved | NOT-WIRED |
| `0x0055b080` | RW-Physics | `_SQRT` | unresolved | NOT-WIRED |
| `0x004e9e40` | RW core | `_RwFrameDestroy` / `_RwFrameDestroyHierarchy` | unresolved: not CRT libid rows (`004e4f70` RwFrameDestroy is C4 by match, `004e5020` is C2) | NOT-WIRED |
| `0x004e99b0` | RW core | `_RpWorldInstance` | unresolved (row `004e5300`, C2, not libid) | NOT-WIRED |
| `0x00574920` | RW-Physics | `_cpuid_basic_info` | unresolved | NOT-WIRED |
| `0x00574a20` | RW-Physics | `_cpuid_Version_info` / `_strncmp` | unresolved | NOT-WIRED |

- 5 rows were appended to `equiv_symmap.tsv` in their own commit (`6be909c6d`, `add_symmap.py`), as hand rows with
  the citation in a 4th column: `__CIpow`, `__CIacos`, `@__fload_withFB@0`, `__rand`, `__pow`.
  - **equiv.py verdicts were not re-run on these rows in this row.**
  - `equiv_symmap.py` regeneration does not produce them.
- 2026-11-11 names (`_sprintf`, `_ROUND`, `_fsin` x2, `_fpatan`): `_ROUND` is still unresolved (above). The other
  four belong to sources outside this sweep and were not looked up.
- Side item `0x00495870` (static helper in `re/match/s16/00495fe0.c`, 98.9%, the fix_joypad boot-patch detour site;
  the harness cannot place a JMP there): equiv.py (tool commit `1ed7ceb1a`, no diff from orch/integration
  `a9e5b4c8a`) gives `NOT-PROVEN indirect-call-esp-reset: orig: ESP restored from a register in a function with a call
  through ld32(M00(),(ld32(M00(),ld32(M00(),esi_in))+0x64))`. It was listed in `re/match/PROVISIONAL.tsv` as
  `provisional` with that line (commit `ec3d6a012`).

## Extra batch (lead: 0x0048ae00, MATCH-NOSRC-A @ a3ca5e0aa, MATCH-NOSRC-B @ f9e024005)

Sources were read with `git show` into `re/match/s14`, `nsa`, `nsb` of this worktree. They are not committed here.
Their SHA-256 are in `RAW_SHA256_SOURCES.txt`. They were linked by provlink, `0x005b8200` with its
`// TOOLCHAIN rw9044` (83.7%, the same score as match.py). Census `census_x` (9 counters): every body executes. The
batch ran before batch 3.

| rva | source | link % | verdict | body calls | detail |
|---|---|---|---|---|---|
| `0x0048ae00` | re/match/s14/0048ae00.c | 98.3 | IDENTICAL-FOR-600 | 1 | x_054703_001_1 |
| `0x00412190` | re/match/nsa/00412190.c | 86.1 | IDENTICAL-FOR-600 | 4 | x_054703_002_1 |
| `0x0040be50` | re/match/nsa/0040be50.c | 80.9 | IDENTICAL-FOR-600 | 177 | x_054703_003_1 |
| `0x0040eee0` | re/match/nsa/0040eee0.c | 75.2 | IDENTICAL-FOR-600 | 8 | x_054703_004_1 |
| `0x004128f0` | re/match/nsa/004128f0.c | 29.1 | CRASH, 2 of 2 | - | write 0x17 in body +0x1c, n=5985 (xrep_061207_001_1) |
| `0x005b8200` | re/match/nsb/005b8200.c | 83.7 | IDENTICAL-FOR-600 | 4 | x_054703_006_1 |
| `0x004883b0` | re/match/nsb/004883b0.c | 18.1 | IDENTICAL-FOR-600 | 1 | x_054703_007_1 |
| `0x0047be80` | re/match/nsb/0047be80.c | 86.4 | IDENTICAL-FOR-600 | 2 | x_054703_008_1 |
| `0x00487280` | re/match/nsb/00487280.c | 43.0 | DIVERGED@300, 2 of 2 | 276 | tick 300 (race), blocks `0x6c3400`, `0x6c3800`; wired after `__CIacos` (above) |

Extra batch: 9 tested, 7 IDENTICAL, 1 DIVERGED, 1 CRASH, 0 NOT-EXECUTED. Thin: 6 of the 7 IDENTICAL ran 1 to 8 calls.
No write-set leg was run for these, so none is promoted.

## Promotions (L-128): `re/promotions/PROV-INSITU-REF1.tsv`, 54 rows

Replay reference race ref1, per-tick compare against ref1/ref2 with the inline JMP live:
IDENTICAL-FOR-600 (each listed row; verdict run, calls and leg per row in PROMOTIONS.tsv)
install-check: ok (prov_install.json jmp_ok true at every listed RVA: site E9 to the counting stub, stub to the body entry)
body-executed: yes (stub call counter > 0 in the verdict run, calls_run column of PROMOTIONS.tsv)
write-set: covered (per row in PROMOTIONS.tsv: static provlink store list empty or absolute compared-image stores only, L-54 / L-38; or dynamic, first 10 calls single-stepped with every store in the frame or the compared image and at least one image store, in a stepped run that itself replays IDENTICAL-FOR-600, L-38 / L-40)

- game (34): `0x00547450`, `0x004c39b0` RwV3dNormalize, `0x004c3c60` Vec2Normalize, `0x004a2be9`
  __security_check_cookie (CRT, outside the P-DoD port count, L-67 / L-70), `0x00547bf0`, `0x005b10a0`
  AudioTableDispatchAt4, `0x005a6ea0`, `0x00481a30`, `0x005bba60`, `0x004785e0`, `0x004b53b0`, `0x005a9f10`,
  `0x005abfa0`, `0x004c4360` MatrixIdentityResidual4c4360, `0x005b73b0` AudioFindExtension, `0x005b3020`,
  `0x005ae170`, `0x005aeb90`, `0x005b03e0`, `0x005b09c0`, `0x005b6710`, `0x0040cea0`, `0x004783f0`, `0x005a7b60`,
  `0x00416250` [dyn], `0x00499d90` [dyn], `0x004c4eb0` RwMatrixInvert_CofactorPath [dyn], `0x004b6b20` [dyn],
  `0x0045a3a0` [dyn], `0x0040e590` [dyn], `0x00481e00` [dyn], `0x00424b80` [dyn], `0x00478660` [dyn], `0x0047a020` [dyn]
- RW-Physics (13): `0x0055c2d0`, `0x00564310`, `0x00578b20`, `0x0055bae0`, `0x0055c230`, `0x0056c8e0`, `0x0055b750`,
  `0x0055b650`, `0x00577be0`, `0x005672c0`, `0x00558cb0`, `0x00559560`, `0x005749b0`
- RW core (7): `0x004dc4a0`, `0x00536e90`, `0x004dc8a0`, `0x0053e5d0`, `0x004c4220`, `0x004dee40`, `0x0053d820`

Thin rows (fewer than 10 body calls in the verdict run; the verdict covers only those calls): `0x005b73b0` 8,
`0x005b3020` 5, `0x005ae170` 4, `0x005b03e0` 4, `0x005b09c0` 4, `0x0053d820` 3, `0x005672c0` 2, `0x005b6710` 2,
`0x0040cea0` 1, `0x004783f0` 1, `0x00558cb0` 1, `0x00559560` 1, `0x005749b0` 1, `0x005a7b60` 2, `0x0040e590` 4,
`0x00481e00` 4, `0x00424b80` 2, `0x00478660` 1, `0x0047a020` 1.

- Long-race consistency: `0x00481a30`, `0x005a9f10`, `0x005abfa0` and `0x005a7b60` were CRASH on 11-11, and
  `0x00481e00` was DIVERGED@2. PROVISIONAL.tsv records each as IDENTICAL-FOR-2724 after its fix. None of the 54 has a
  standing long-race DIVERGED or CRASH.
- `c4_reclass.py batch --tsv re/promotions/PROV-INSITU-REF1.tsv` (dry run): 54 pass, 0 refused, 0 skipped.

**Not promoted, write-set:**
- 3 bodies are covered with compared-image stores, but their stepped run diverged: `0x00420050`, `0x0046ddb0`
  (ws_g01, DIVERGED 300) and `0x00448220` (ws_g03, DIVERGED 302). Three of the 11 stepped groups diverge (ws_g01,
  ws_g03, ws_g05), although every member is IDENTICAL in its own verdict run.
  [UNCERTAIN] Which member or mechanism. No group was split. Register line "PROV-INSITU-REF1 (stepped-group divergence)".
- 28 bodies had no stepped call.
- Both sets are listed in `WS_CUT.tsv`.

## What this does not cover

- The window: ~190 race ticks (window caveat). NOT-EXECUTED is 0 because all 309 are T2 functions that run in this
  scenario.
- No diagnosis of any DIVERGED or CRASH row (first tick, blocks and fault address are given as the starting point).
- Write-set: 131 escapes and 38 frame-only bodies stay where they are (L-40). The 3 bodies from diverging stepped
  runs and the 28 with no stepped call are open (new row PROV-INSITU-REF1-WS2).
- The 7 NOT-WIRED rows (names above).
- Builds and gates: not run. No build input changed (`mashedmod/` untouched). This is a race-path replay row and
  binds nothing.

## Evidence (this folder)

- `VERDICTS.tsv`: one row per sweep row (verdict, run, calls, install, static write-set leg, detail, 11-11 verdict).
- `PROMOTIONS.tsv`, `WRITESET_TABLE.tsv`, `WS_CUT.tsv`, `RESCORE_C0C1.tsv`.
- `sweep309.tsv`, `t2_nonc4.tsv` (audit copy), `batch_*.txt`, `extra_rvas_ws.txt`.
- Tools:
  - `run_arm.py` (the ref1 arm), `select309.py`, `make_batches.py`, `summarize.py`, `make_tables.py`, `prom_table.py`
  - `ws_driver.py`, `ws_cut.py`, `crash_sites.py`, `rescore_c0c1.py`, `notwired_calls.py`
  - `add_symmap.py`, `add_prov_495870.py`
  - `re/tools/prov/prov_bisect.py` (now runs `.py` arms)
- Raw data: `D:/MashedWT/PROV-INSITU-REF1_data` (18 GB, 283 run folders, each with its `.log`).
  - `bodies309.json`, `bodies_extra.json`, `bodies_nw.json`, `*_state.jsonl`, `lockstatus.log`
  - SHA-256 of every state / bodies file and of each run's `ticks.bin`, `prov_install.json`, `prov.json`,
    `meta.json`, `crash.jsonl`, `wlog.bin`: `RAW_SHA256.txt` (1,259 lines).
