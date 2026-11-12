# RESULT RO-MENU-DIV-43DFD0 (2026-11-12)

Branch `orch/RO-MENU-DIV-43DFD0`, worktree `.worktrees/RO-MENU-DIV-43DFD0`. Row kind: top-down / replay-oracle diagnosis.
Input: `verify/romenu_20261112/RESULT_RO-MENU-REPLAY.md` (`0x0043dfd0` DIVERGED at menu tick 1, `0x0043d2a0` at menu tick 16).
Data: `D:/MashedWT/RO-MENU-DIV-43DFD0_data`. Private root: `D:/MashedWT/RO-MENU-DIV-43DFD0_root` (robocopy of `original\`,
`MASHED.exe` SHA-256 `B9977DAB...FE87`, the same patched exe as `RO-MENU-REPLAY_root`).

## Verdict

**GREEN.** Both divergences come from defects in our match sources. Each one is fixed in its source. With the fixed
blobs both bodies run in the menu replay and are **IDENTICAL-FOR-N 3324 of 3324** in 4 of 4 class-matched runs each.

| RVA | first divergent value (before) | cause in our source | fix | after |
|---|---|---|---|---|
| `0x0043dfd0` | tick 1, `0x0067eca8` = `0x00000000` (ref `0x00000010`) | `__ftol` called with nothing on the x87 stack | `(int)(DAT_007f1004 * const)` at the 4 sites | IDENTICAL-FOR-3324, runs f6..f9 |
| `0x0043d2a0` | tick 16, `0x00898af4` = `0x00000000` (ref `0xff100000`) | `FUN_00432b30` called as 4-arg cdecl, while the original passes `EAX`/`ESI` plus 2 stack args | `FUN_00432b30` defined static in the same TU | IDENTICAL-FOR-3324, runs f1, f3..f5 |

The fixes do not make either body C4, admitted or byte-matched. Both stay NOT-PROVEN provisional sources (L-126):
`0x0043dfd0` is at 19.2% and `0x0043d2a0` at 54.3%.

## 1. 0x0043dfd0: first divergence and cause

Run `b_43dfd0` uses the PROV-INSITU blob (`verify/provinsitu_20261111/bodies_all.json`). It is class-matched to the
references (`0x007f1030` = `39a80501`, `0x006147d8` = 1) and reproduces DIVERGED@1 (blocks `0x0067ec00` + `0x007d6000`).
These are its raw dwords at tick 1 against ref1 (`rawdiff_extracts.txt`):

| VA | run | ref |
|---|---|---|
| `0x0067eca8` | `00000000` | `00000010` |
| `0x0067ec40`, `0x0067ec5c`, `0x0067ec78`, `0x0067ec94` | `00000000` | `10000000` |
| `0x007d62b4` | `00000002` | `00000004` |

At tick 2 the same dwords differ, and the reference has `0x0067eca8` = `0x20` and `0x0067ec40..94` = `0x20000000`.

Mechanics:
- `0x0067eca4` = 1 at ticks 0..4 in both runs, so the original takes the branch at `0x0043e08c` (`cmp eax, edi` with
  edi = 1, `jne 0x43e0f8` not taken).
- That branch computes `fld [0x007f1004]; fmul [0x005cc9fc]; call 0x004a2c48` at `0x0043e0a7..0x0043e0b3`.
  `0x004a2c48` is `__ftol` (`re/tools/match/equiv_symmap.tsv`). It then adds the result to `[0x0067eca8]` at
  `0x0043e0b8..0x0043e0c6`.
- The reference's `0x0067eca8` grows by `0x10` per tick, and at tick 17 the reference has `0x0067eca4` = 2. That is
  the `>= 0xff` path at `0x0043e0d2..0x0043e0e9`: it stores `0xff`, stores 2 to `0x0067eca4` and calls
  `FUN_0043d2a0([0x0067ecac], 0)`.
- In `b_43dfd0`, `0x0067eca8` stays 0 through tick 17, and `0x0067eca4` stays 1.

Source defect (`re/match/s08/0043dfd0.c`, raw converter body): every `__ftol` site was `iVar = FUN_004a2c48();`. The
float operand was lost, so the DDK compiled a bare `call` to `0x004a2c48` with no `fld`/`fmul` in front.
- Four such sites, matching the original's four:
  - `0x0043e0a7` (`* [0x005cc9fc]`)
  - `0x0043e101` (`* [0x005cda88]`)
  - `0x00440a16` (`* [0x005cc9fc]`)
  - `0x00440a5e` (`* [0x005cd8f8]`)
- `[UNCERTAIN]` Why the bare call returned 0 in the blob: the x87 stack contents at that call were not captured.

A second defect was fixed in the same edit. It is a static fix and was not bisected separately:
- `extraout_EDX` (the key-scan flag) was read at the `FUN_0045b350` call without ever being assigned.
- In the original, `edx` is zeroed at `0x0043e3f8` and set to `edi` (1) for keys `0xff100000` / `0xff230000` at
  `0x0043e427` / `0x0043e432`. It is tested at `0x0043e4a3` (`cmp edx, ebx; je 0x43fc74`).
- `FUN_0045b350` is a bare `ret` (`0x0045b350`).
- The source now sets the flag the same way.
- Not separately evidenced: whether the uninitialised read changed any compared state in this scenario. The fixed
  blob carries both changes.

Not traced: the writers of `0x0067ec40..94` and `0x007d62b4` (none is a direct operand in the `0x0043dfd0` listing).
In the fixed-blob runs they no longer differ at any tick (f6..f9 IDENTICAL-FOR-3324). `f1_43dfd0` is
class-mismatched. At ticks 1-2, inside `0x00660000..0x00800000` and leaving out the heap-pointer pair
`0x007739b4` / `0x007739bc`, its only raw differences against ref1 are the timer-phase dwords `0x007f1010` /
`0x007f102c` / `0x007f1030`.

## 2. 0x0043d2a0: first divergence and cause

Run `b_43d2a0` uses the PROV-INSITU blob. It is class-mismatched (`0x007f1030` = `39ae7959`), so it is not body
evidence. RO-MENU-REPLAY already reproduced DIVERGED@16 in 2 of 2 runs. Its raw dwords at tick 16 against ref1:

| VA | run | ref |
|---|---|---|
| `0x00898af4` | `00000000` | `ff100000` |
| `0x00898af8` | `00000000` | `00000001` |
| `0x00898b00` | `00000000` | `ffffffff` |
| `0x00898b08` | `00000000` | `3f19999a` |
| `0x00898b0c` | `00000000` | `42800000` |
| `0x00898b10` | `00000000` | `43d60000` |
| `0x00898b1c` | `00000000` | `ffffffff` |
| `0x00898b20` | `00000000` | `ffffffff` |

These are the fields `type` .. `s2` of the 0x34-byte item 1 of the array at `0x00898ac0`, the item the original
`FUN_00432b30` appends at `*cnt`.

Original call site:
```
0x0043d77b  push ecx              ; menu
0x0043d788  call 0x42ad90
0x0043d790  push edx              ; (short) result = sel
0x0043d791  mov eax, ebp          ; mode
0x0043d793  lea esi, [esp+0x24]   ; &nItems
0x0043d797  call 0x432b30
0x0043d79c  add esp, 8
```

Source defect (`re/match/s08/0043d2a0.c`):
- `FUN_00432b30` was declared `void FUN_00432b30(int mode, int *pn, int s, int menu);` (cdecl) and called with four
  stack args. `EAX` and `ESI` held whatever the body left there.
- provlink resolves `FUN_00432b30` to the original `0x00432b30` (`re/tools/prov/provlink.py` docstring).
- So the original body read `mode` from `EAX` and `cnt` from `ESI`, neither of which was set, and read the wrong
  stack slots for `sel` / `menu`.

Fix:
- `FUN_00432b30` and its two static callees `FUN_0042ad10` / `FUN_0042add0` are now defined `static` in `0043d2a0.c`,
  copied verbatim from `re/match/s08/00432b30.c`.
- `00432b30.c`'s header records the convention: mode in `EAX`, `int *cnt` in `ESI`, stack args `sel`, `menu`. Its own
  stand-in caller already used it.
- After the fix the DDK compile emits `push ecx ... call 0x42ad90; movsx edx, ax; push edx; lea esi, [esp+0x20];
  mov eax, edi; call 0x432b30; add esp, 8` (`match.py --show`), the same convention as the original.
- The score moved from 54.0% to 54.3% (regs 64.7% to 65.3%).

`0x0043d2a0`'s first call in the replay comes from `0x0043dfd0`'s fade-complete path (`0x0043e0e9`, tick 16 to 17
above), so the two rows are linked through `0x0067eca4` / `0x0067eca8`.

## 3. Replay verdicts (`final_verdicts.json`, scored by `re/frida/prov_compare.py` against ref1 + ref2)

Configuration: `run_arm.sh` = the RO-MENU-REPLAY arm (`--clock tick --boot`, warp at tick 600, cap 4000), plus raw
per-tick copies of `0x0067ec00..0x0067f000`, `0x007d6000..0x007d6400` and `0x00898800..0x00898c00`.

| run | body | class | verdict |
|---|---|---|---|
| ref2, ref3, ref4, ref5 | none | `39a80501` | IDENTICAL-FOR-3324 against ref1/ref2 |
| negm (`--negctl 0x0067f17c --neg-tick 300`) | none | `39a80501` | DIVERGED at tick **300**, block `0x0067f000`: control PASS |
| b_43dfd0 | old `0x0043dfd0` | `39a80501` | DIVERGED@1 (`0x0067ec00`, `0x007d6000`): reproduces |
| b_43d2a0 | old `0x0043d2a0` | `39ae7959` (mismatch) | not scored, raw extract only (section 2) |
| **f6, f7, f8, f9_43dfd0** | fixed `0x0043dfd0` | `39a80501` | **IDENTICAL-FOR-3324**, 600 calls each, all before the warp |
| f1..f5_43dfd0 | fixed `0x0043dfd0` | `39a8d7f1`, `39a86e79`, `39a82829`, `39b606f1`, `39a8fb19` | class mismatch, not used |
| **f1, f3, f4, f5_43d2a0** | fixed `0x0043d2a0` | `39a80501` | **IDENTICAL-FOR-3324**, 1 call each, before the warp |
| f2_43d2a0 | fixed `0x0043d2a0` | `39a82829` | class mismatch, not used |

Every body run logs `prov rows 1 executed 1 install_fail 0`. Call counts per run are in `prov_calls.json`.

Same-class cross-check (supporting only): `f3_43dfd0` and `f2_43d2a0` share class `39a82829`. Scored against each
other with the ref1/ref2 allowed blocks, both ways, each is IDENTICAL-FOR-3324.

## 4. Builds and knob checks (top-down row)

Two match sources changed, and `0043dfd0.c` is compiled in the port through `Match/ProvS08_0043dfd0.cpp`.
`re/tools/provbodies/gen_wrapper.py` regenerates that wrapper byte-identical after the edit, so the wrapper is unchanged.

- **Default build** (`MASHED_NO_DEPLOY=1`): Build OK (`build_vs.log`).
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. `ddkobj.py check` gives IDENTICAL 789 / 878, DIFFERENT 79, not-in-map 10,
  units linked 837 / 847, refusals 9 (`wrapper-redefines-globals`).
  - None of the edited units is checked: `0043dfd0.c` is `skip:provisional`, and `0043d2a0.c` / `00432b30.c` are not
    object-build units.
  - No DIFFERENT row comes from a changed file.
- **Knob self-test** (`run_checks.sh`, MASHED_ORIG_TOP=1, 600 frames):
  - `selftest_on`: rc 0, `[prov-43dfd0] self-test returned code=00000000 stub calls=5 distinct stubs=5 of 63`. This is
    the same line as PROVISIONAL-BODIES.
  - `selftest_off`: rc 0.
- **Menu walk**, 12 nav-demo captures, knob path, self-test OFF / ON / OFF / ON: 4 of 4 class E, 12 of 12 equal each,
  freeze `pinned pos=55 hash=28538c74`.
- **Gates**: not run, by the top-down rule (WORKER_RULES: the G-CTRL / B hashes are computed with the knob off).

## 5. Not done / for the lead

- `re/match/PROVISIONAL.tsv` row `0x0043dfd0` still reads `17.7` and "NOT-EXECUTED in the replay window". It now executes
  and is IDENTICAL-FOR-3324. Not edited here.
- `0x0043d2a0` has no PROVISIONAL.tsv row: it runs only as a PROV-INSITU blob.
- No promotion. A C4 by the replay form (L-128) would still need the write-set leg (PROV-WRITESET, `--prov-wlog`) and the
  install / no-STUB legs recorded for these two blobs. That is a follow-up row.
- The 3 CRASH bodies of RO-MENU-REPLAY (`0x0045d460`, `0x0048e820`, `0x00499400`) are out of scope here.

## Files

- Committed in this folder:
  - `run_arm.sh`, `batch1..4.sh`, `run_checks.sh`, `rawdiff.py`
  - `rawdiff_extracts.txt`, `final_verdicts.json`, `prov_calls.json`
  - `raw_sha256.txt`: SHA-256 of every cited raw `ticks.bin` / `meta.json` / `prov.json` / `prov_install.json`, plus
    `bodies_fix1.json` and the check logs
- Fixed blobs: `D:/MashedWT/RO-MENU-DIV-43DFD0_data/bodies_fix1.json`. Rebuild with
  `py -3.12 re/tools/prov/provlink.py --rvas 0x0043dfd0,0x0043d2a0 --out <file>`.
- Source commit: `01f650ae0`.
