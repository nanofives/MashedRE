# RESULT PORT-STATE-03 (2026-11-08): frontend menu records at DAT_00898ac0 + the swaps the move unblocks

Branch `orch/PORT-STATE-03` off `orch/PORT-STATE-01` @ `9c653e93b` (PORT-STATE-01 is still in the merge train, not on
`orch/integration` @ `cc56a0a68`, so the brief's `merge orch/integration` step has not run yet: see "Hand-in").
PREREG `b4373fe39` (committed alone with the `run_nav.py --data` option, before any arm).

**Verdict: GREEN** (round 2, L-100 (a), section 4; the decision it raised, L-102, is answered (a), SSE2 kept). **0x004325c0** is now the byte-matched source as the one
body in both targets, and `MASHED_NO_ANIM_TRUNC` is deleted. Gates and the frontend state log are unchanged on the
committed build. One part of the brief is reverted: the `x87_tus.txt` entry for the wrapper. The bisect shows the
/arch:IA32 compile alone moves the state log (1 of 185 and 10 of 619 `rec=` lines). The SSE2 compile reproduces the
pre-swap logs exactly. Keeping it is the decision (section 4c).

Round 1 (GREEN): under `MASHED_PORTSTATE_FRONTEND` (default OFF, exe only), the frontend menu records now live at
DAT_00898ac0. Three matched bodies run on that state with the knob ON: **0x0042d3e0** MenuEntryArrayInit,
**0x00497450** Pred497450 and **0x0042ad10** (a lane B-frontier row the move unblocked, port `none` -> `body`).
Each keeps today's hand path with the knob OFF.
Default gates are unchanged: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3. The knob-ON arms give the same
hashes. The frontend per-frame state log, which now carries a record-table hash, is byte-identical OFF vs ON on
three scripts.

## 1. Commits

| commit | content |
|---|---|
| `b4373fe39` | PREREG + `run_nav.py --data <folder>` (default unchanged) |
| `095a24f05` | **records move.** `g_records[31]` -> `g_records_store[31]` (knob OFF) and `MenuRecord* const g_records` = DAT_00898ac0 (knob ON) / the store (OFF). Read once, the L-88 shape (`Vehicle/VehiclePhysicsRun.cpp:132`). Every writer and reader already goes through `g_records` (`MenuNavSM.cpp`) or `Nav_Records()` (`exe_main.cpp`). `NAV_KEYS` line gains `rec=` (FNV-1a 32 over records 0..29) and `n=` (record count) in both knob states |
| `65322fd95` | **0x0042d3e0.** Knob ON: `RecordsZero` calls the matched export `MenuEntryArrayInit` (`Match/MatchS07_0042d3e0.cpp`, already the RVA's one body, exe-called at boot B7) on DAT_00898ac0, then the host's documented wider +0x20 / +0x30 clear (`MenuNavSM.cpp`, open parity question unchanged). Knob OFF: `MenuEntryZeroBody` on the store |
| `7cdb2327d` | **0x00497450.** `MenuGameState::player_active[4]` -> `player_active_off[4]`. Knob ON: `Q_PlayerActive(i)` = `Pred497450(i) != 0` (`Match/MatchGFFU_00497450.cpp`, `[0x007e96fc + i*0x200]`). The port's extra `player < 0` guard is off that path: the only caller is the screen-0x1c loop, `i` in 0..3. Knob OFF: the store with the guard |
| `867516058` | **0x0042ad10.** New exe-only `Match/MatchS06_0042ad10.cpp` compiling `re/match/s06/0042ad10.c`, plus `exe_sources.rsp` and `x87_tus.txt` entries. Knob ON: `PromptStripAppend` calls it as `(&g_record_count, tag, 0x40, 0x1ac, 0)` in place of the hand `PromptRowInit` (the same 9 stores). Knob OFF: `PromptRowInit` on the store |
| `fe0a7b834` | re-classify leg (section 5) + `backfill_exe_file.py` / `backfill_port.py` output |

### Entry 30 is not part of the original array

The exe kept 31 records ("0x898ac0..0x899104" in `MenuNavSM.h`). The original array is **30**: `0x0042d3e0` loops
while `eax < 0x008990dc` with `eax = record + 4` (0x0042d413, `MenuEntryZero.h`). So 0x008990d8..0x0089910b are other
original globals. DAT_008990e4 among them is written (=1) by the exe-compiled matched 0x0042aad0 and tested and
cleared by the exe-compiled matched 0x00428140 (`re/match/s06/0042aad0.c`, `00428140.c`). With the knob ON, the exe
writes records 0..29 only: the capacity is 30 (`RecordCapacity()`, used by the populate loop and the prompt-strip
guard) and `RecordsZero` skips its entry-30 memset. No screen comes near 30 records (the most in these runs is 7), so
the bound changes nothing observable. Knob OFF keeps 31 and the memset.

Boot detail checked. Milestone B7 (`exe_main.cpp`) poisons 0x00898ac0..0x008990db with 0xDEADBEEF, then calls
`MenuEntryArrayInit`, which leaves +0x20 and +0x30 of each record poisoned. With the knob ON, those words are
DEADBEEF until the first `BuildRecords`. Before that, only the anim tick walks the records. On tag-0 records it takes
the float path, which reads +0x08 and writes +0x04 / +0x08 only, so the poison is never read. The first
`RecordsZero` clears both words. The OFF/ON `rec=` logs below are identical from their first line.

## 2. Matched bodies in scope

| rva | name | source | match.py (DDK 9176/9178, /O2 /GS /Ob1) | relocheck | bodies (exe / asi), `rva_body_scan.py` | port before -> after |
|---|---|---|---|---|---|---|
| 0x0042d3e0 | MenuEntryArrayInit | re/match/s07/0042d3e0.c | C4 by match (hooks.csv) | (C4 row) | MatchS07_0042d3e0.cpp / same | called -> called |
| 0x00497450 | Pred497450 | re/match/gffu/00497450.c | C4 by match (hooks.csv) | (C4 row) | MatchGFFU_00497450.cpp / same | called -> called |
| 0x0042ad10 | FUN_0042ad10 | re/match/s06/0042ad10.c | **100.0%, 32/32 ins, 118 B BYTE-IDENTICAL** (re-run on this branch) | **OK checked=9 unchecked=0** | MatchS06_0042ad10.cpp / none | **none -> body** |
| 0x004325c0 | FUN_004325c0 | re/match/s08/004325c0.c | round 2: 100.0%, 152/152 ins, 564 B BYTE-IDENTICAL | OK checked=24, unchecked __ftol | MatchS08_004325c0.cpp / same | called -> called |
| 0x0042ac50 | FUN_0042ac50 (Calc42ac50) | re/match/s06/0042ac50.c | (unchanged) | | MatchS06_0042ac50.cpp / same | body -> **called** (round 2) |

0x0042ad10 is `body`, not `called`, in `backfill_port.py`. That is the honest value here. `FUN_0042ad10` is
`static` in the matched source, so MSVC inlines it into the forwarder `PromptRowInit0042ad10` and the map has no
symbol for the RVA. Its one exe call is also knob-ON only. The .asi has no body for the RVA: the DDK passes `idx` in
EAX, so no cdecl export could be installed at 0x0042ad10. That is not a CROSS-TARGET pair, since no asi-only body
exists. **0x0042ad10 runs only with the default-OFF knob.**

0x0042d3e0 and 0x00497450 were already `called` through other exe call sites (B7 boot, the game-mode machine's
`SlotActiveThunk`). This step adds their frontend call sites, which run with the knob ON only.

## 3. Checks against the PREREG

Frozen build from `867516058` (`MASHED_NO_DEPLOY=1`, shared .asi not deployed): exe `590d9969...`, asi `1016ec76...`.
`=== Build OK ===` for both targets. Runs used the private root `D:/MashedWT/PORT-STATE-03_root`, `test-slot` outside
and `mashed_lock.py run --label PORT-STATE-03` inside. Gates and walks self-exited 0, apart from the first
multi-screen attempt (P3 row).

| id | check | result | holds |
|---|---|---|---|
| P1 | default `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 = `merge_train.json` refs | yes |
| P2 | knob ON `H_PSF B_PSF_K0 B_PSF_K1 B_PSF_K26` | same hashes as P1 | yes |
| P3 | `NAV_KEYS` per-frame log OFF vs ON | (a) STATE-01's key script (`MASHED_GOTO=1,2,4,6 MASHED_NAV_KEYS=d,u,d,u,e`, screen 6): 185 lines, **OFF, OFF, ON, ON all sha `4e5e35a2`**. `rec=` takes 91 values (records sliding), then settles. (b) Multi-screen walk (`MASHED_GOTO=1 MASHED_NAV_KEYS=e,d,e,b,b,d,e,d,e,b,b,d,d,e,b,b,u,u,u`): 619 lines over screens 1 -> 2 -> 4 -> 2 -> 1 -> 3 -> 4 -> 3 -> 1, depths 0..3, 388 distinct `rec=` values. **OFF, OFF, ON all sha `efc763d4`**. The instrument repeats run to run, so P3 is scored | yes |
| P3 reach | which new paths the walks ran | 0x0042d3e0: every `BuildRecords` (each screen change). 0x0042ad10: every navigation op, because each screen's record count = back + items + 1 prompt row (screen 1: 5 items, n=7; screen 2: 3, n=5; screen 3: 2, n=4; screen 4: 1, n=3; tables `kT1..kT4`, `MenuNavSM.cpp:72-75`). 0x00497450: **not reached** (screen 0x1c is not on either route). The first walk attempt had no `MASHED_GOTO` and never reached phase 3. All three of its runs timed out at 300 s with 0 lines, and only my own PIDs were stopped. Re-run with `MASHED_GOTO=1` above | 2 of 3 reached |
| P4 | demo walk (`MASHED_NAV_DEMO=1`), OFF vs ON | both self-exit 0 with 12 BMPs. `NAV_DEMO` lines (phase, depth, screen, cursor, nrec) identical, sha `376a1765`. The BMP sets differ (`510a92c2` / `948642d2`), as in STATE-01, where three OFF runs gave three hashes: not scored | as worded |
| P5 | static | `rva_body_scan.py`: one body per target for 0x0042d3e0, 0x00497450, 0x0042ad10 (exe only), 0x004325c0, 0x00430830, 0x004309b0. rva-lint `CROSS-TARGET=1 allowlisted=1 NEW=0`, asm-lint OK, no allowlist line added. Backfills: one row changed (0x0042ad10, `exe_file` set, port none -> body). Totals none=5310 body=588 called=253 | yes |

Committed extracts (one copy each, since the runs are identical): `PS03_navkeys_keyscript.txt` (P3a),
`PS03_navkeys_walk.txt` (P3b), `PS03_navdemo.txt` (P4).

## 4. 0x004325c0: swapped by L-100 (a) (round 2)

Round 1 left 0x004325c0 alone. It had two blockers: the L-32 rounding opt-out knob in its hand body, and the dual
copy that a knob-ON-only swap would create. Filed as **L-100**. Mariano's answer: "(a) delete MASHED_NO_ANIM_TRUNC,
matched body is the only body, lands only with G-CTRL/B and the frontend state log unchanged".

### 4a. What landed (`1496ead86`, x87 part reverted in `20142cd32`)

| step | change |
|---|---|
| 1 knob | `Frontend/MenuAnimTickTwin.cpp` (hand `MenuAnimTickBody` + the knob) and `Frontend/MenuAnimTick.h` deleted, both targets. No live code or tool names the knob any more (`grep` over `mashedmod/`, `scripts/`, `re/frida/`, `re/tools/`, `.claude/`). The remaining mentions are history and are not edited: dated `verify/` RESULTs and PREREGs, `re/analysis/CHANGELOG.md`, `hooks.csv` notes, the L-32 / L-100 rows in the ledger-owned `DECISIONS.md`, and the generated `scripts/ghidra/call_targets.json` (its `file` field still names the deleted TU) |
| 2 swap | `Match/MatchS08_004325c0.cpp` in both rsp lists: `re/match/s08/004325c0.c`, export kept as `MenuAnimTickTwin` (`re/frida/menu_anim_diff.py` finds it by that name), `RH_ScopedInstall(MenuAnimTickTwin, 0x004325c0)`. The source's static callee 0x0042ac50 sits in `#ifndef MATCH_PORT`. The port branch calls it by name (`#define FUN_0042ac50 Calc42ac50`, the one body in `Match/MatchS06_0042ac50.cpp`), so 0x0042ac50 keeps one body per target. DDK text unchanged: match.py `--bytes` **BYTE-IDENTICAL 564 B, 152/152 ins**, relocheck **OK checked=24, unchecked __ftol** (the same result as the unmodified source). src sha256 `5c4b5ab1d8e19e54` (was `95490871ca55ac22`) |
| 3 bridge | `Anim_Tick` (`MenuNavSM.cpp`) calls `MenuAnimTickTwin()` in both knob states. For the one call it puts `g_frame_dt`, `g_anim_phase`, `g_anim_gate` and the slot's `item_count` at DAT_007f1004 / DAT_0067eca4 / DAT_0067e914 / DAT_0067ece0 and restores the previous values after. With the knob OFF it also copies records 0..29 of the store to DAT_00898ac0, copies them back after the call, and restores what DAT_00898ac0 held. The body's one callee reads no global. Main thread only |
| 4 +0x30 | `BuildRecords` stores `0x005f7370` (screen 24) / `0x005f72a0` (screen 18) at item record +0x30, as 0x0043d2a0 stores the screen's table pointer (`re/match/s08/0043d2a0.c:302`). Other screens keep 0: the port has no original address for their tables (the original holds `PTR_DAT_005f7638[screen]`), and 0x004325c0 only compares with those two values |
| x87 | **reverted.** `MatchS08_004325c0` is not in `x87_tus.txt` (4c) |

`rva_body_scan.py`: 0x004325c0 has one body per target, `Match/MatchS08_004325c0.cpp` (exe and asi), and 0x0042ac50
one each in `Match/MatchS06_0042ac50.cpp`. rva-lint `CROSS-TARGET=1 allowlisted=1 NEW=0`, no allowlist line added.
Backfills: 0x004325c0 `exe_file` -> MenuNavSM.cpp | MatchS08_004325c0.cpp, port `called` -> `called`.
**0x0042ac50 port `body` -> `called`** (the matched tick calls it by name). Totals none=5310 body=587 called=254.
Re-classify `file` leg for 0x004325c0 -> `Match/MatchS08_004325c0.cpp` (C4 -> C4, CHANGELOG line with the new src sha).

### 4b. Checks (frozen builds, private root, `test-slot` outside, `mashed_lock` inside, every run self-exit 0)

| build | G-CTRL default / knob ON | B default / knob ON | NAV_KEYS key script (185 lines) OFF / ON | NAV_KEYS multi-screen walk (619 lines) OFF / ON | NAV_DEMO OFF / ON |
|---|---|---|---|---|---|
| reference (round 1, `590d9969`) | 8284be46/67931834/31fe043e both | 1d5bf4f9 x3 both | `4e5e35a2` / `4e5e35a2` | `efc763d4` / `efc763d4` | `376a1765` / `376a1765` |
| L-100 with x87 entry (`0a1ab369`, `1496ead86`) | 8284be46/67931834/31fe043e both | 1d5bf4f9 x3 both | **`2e8cc64f` / `2e8cc64f`** | **`dc862c7e` / `dc862c7e`** | `376a1765` / `376a1765` |
| **L-100 SSE2, committed (`44b3da06`, `20142cd32`)** | 8284be46/67931834/31fe043e both | 1d5bf4f9 x3 both | `4e5e35a2` / `4e5e35a2` | `efc763d4` / `efc763d4` | `376a1765` / `376a1765` |

In the x87 build only `rec=` moves (FNV of records 0..29), on 1 frame of the key script (f=3, the first animating
frame) and 10 frames of the walk. Every other field is equal, and `rec=` converges again on the following frames. The
bisect is one variable: same commit, x87 entry removed, everything else in place. It reproduces the references
exactly, so steps 1-4 are neutral on the state log and the /arch:IA32 compile alone moves it.

### 4c. Decision for the lead: the wrapper's float model

The matched 0x004325c0 has two float paths (`re/match/s08/004325c0.c`). Its slide counters are
`(int)(dt * speed * K)`, and its default path is `f8 = dt * 300.0f + f8`, clamped at 180.0f. Under /arch:IA32 the exe
holds the intermediate at x87 precision and rounds once. Under SSE2 each float operation rounds to 32 bits, as the hand
body did. The original runs that code on x87. Whether its precision control is 24-bit at the menu was not
measured in this step (open: the original's FPU control word during the frontend tick; a D3D9 device created
without `D3DCREATE_FPU_PRESERVE` sets 24-bit, not checked here). The memory note "asm bit-identity in the .asi is not
the standalone's" records PC_24 for MASHED.exe in the race.

- (a) **Keep SSE2 (committed).** The state log and gates are unchanged. The `x87_tus.txt` policy line for matched
  float code ("MATCH-S08 2026-11-01: port bodies compiled from byte-matched game code that uses float") is not applied
  to this one TU. **Recommended**: it is the value set the brief allows to land, and it equals 32-bit-per-op rounding,
  which a PC_24 original also produces.
- (b) Add it to `x87_tus.txt`, accept the moved `rec=` hashes (`2e8cc64f` / `dc862c7e`) as the new frontend
  reference, and measure the original's control word first.

Evidence: this section; raw logs listed below. Hash sets: reference / SSE2 `4e5e35a2`, `efc763d4`, `376a1765`; x87
`2e8cc64f`, `dc862c7e`, `376a1765`.

## 5. Re-classify leg (from PORT-STATE-01)

Through the re-classify skill. It has no helper for a `file`-only change, so I did its step 1 (`hooks.csv`) and step
4 (CHANGELOG) by hand. 0x00430830 `file` Frontend/MenuScoreSort.cpp -> `mashedmod/src/mashed_re/Match/MatchS07_00430830.cpp`.
0x004309b0 `file` Frontend/FrontendMode.cpp (deleted) -> `mashedmod/src/mashed_re/Match/MatchS07_004309b0.cpp`. Both stay
C4->C4. Two CHANGELOG lines go below `<!-- ENTRIES -->`. No U- or S- rows were created, and none are open on either
function.

## 6. Not done / follow-ups

- **0x00497450's knob-ON site is wired but unexercised, and returns 0 either way.** Nothing in the exe writes
  DAT_007e96fc. The exe keeps slot types in `exe_main.cpp` `g_input_slot_type[]` (FUN_00498510 types; the original
  stores them at `0x007e95c0 + i*0x200 + 0x13c` = DAT_007e96fc + i*0x200). Writing them there under the knob would
  make screen 0x1c's grey-out follow the input devices, as in the original. That is a behaviour change for its own
  PREREG.
- Record +0x30 holds 0 in the exe, where the original holds the screen's table pointer (section 4). +0x20 / +0x30
  wider clear: open parity question, unchanged.
- Next slice per STATE-01 2b: the NavSlot arrays (0x0067ed38..), which also hold DAT_0067ece0 and DAT_0067ed78
  (0x00432800).
- `Nav_GameStateReset` is unused, so it does not reset the knob-ON words. The same is true of STATE-01's game-mode
  word.
- `.ps1` / `.sh`: none edited.

## Hand-in

L-102 answered (a): SSE2 stays for the 0x004325c0 wrapper (a measuring row PORT-STATE-03-FPU follows).

PORT-STATE-01 landed (`orch/integration` @ `eee426759`). Merge `a1f3c5d49` (`git -c merge.autoStash=false merge orch/integration`):
one conflict, in `re/analysis/CHANGELOG.md`, where both sides had prepended entries; all of them are kept and nothing
is rewritten. `x87_tus.txt`, `exe_sources.rsp` and `asi_sources.rsp` auto-merged as a union with no duplicate lines.
`hooks.csv` keeps the 11-column shape, and `exe_file` / `port` were re-derived after the rebuild
(`backfill_exe_file.py`: 0 changed; `backfill_port.py`: 6 cells, all from integration's code: 00495120, 004950b0,
00495110, 0042b950, 00433240 none -> called, 00402f80 body -> called; totals none=5305 body=586 called=260).

Checks on the merged build (exe `0fa637ed...`, asi `692f74a0...`): `=== Build OK ===` for both targets, rva-lint
`CROSS-TARGET=1 allowlisted=1 NEW=0`, asm-lint OK.

| check | result |
|---|---|
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` (second run). The first H_DEF self-exited 0 after 8 s with no step dump, its log ending just after race start. Re-run alone it took 127 s and gave the reference hashes. No change in between, so the first run is a one-off |
| B (default K0/K1/K26) | `1d5bf4f9` x3 |
| NAV_KEYS key script OFF / ON | `4e5e35a2` / `4e5e35a2` |
| NAV_KEYS multi-screen walk OFF / ON | `efc763d4` / `efc763d4` |
| NAV_DEMO OFF / ON | `376a1765` / `376a1765` |

Raw data: `D:/MashedWT/PORT-STATE-03_data/exe_merge/`, `merge/run.log`, `merge/arms/` (first H_DEF), `merge/arms2/` (re-run),
`merge/nav/`, `build_merge.log`.

## Raw data (not committed), `D:/MashedWT/PORT-STATE-03_data/`

| file | sha256 |
|---|---|
| exe/mashed_re.exe | 590d9969ae80d644a87b1110971253fd25839c5820d1a5326d2bd3750c72be84 |
| exe/mashed_re_dev.asi | 1016ec76e177217ec81c18bfb936afde6fbbb92ca7751046582e1b1c47b0de99 |
| arms/H_DEF.step.csv, arms/H_PSF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv, arms/H_PSF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log, arms/H_PSF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_{DEF,PSF}_K{0,1,26}.step.csv (6 files) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| gates_run.log | 23f178f27e3f27da71c7357a55fea581f5735e5a876748004bad6a85231954f3 |
| build_c4.log | 85216046e26a4ba4bf95af7599440e8f371eada7c5bca6535e50732eb1d47539 |
| nav_run.log | 8a26209565a8eaa4fcc362c6a64daa887f5408bf666af84fbdec31ed5a74458a |
| nav_run3.log | aaf35103fd5fa402c2a43479689e2896e997bcbcdb900fa980b41a5f6d89434a |
| nav/keys_{off,off2,on,on2}.navkeys.txt | 4e5e35a2f1b2e96e986d678804c959b9b8683ef8499a7d7f0b1c68c3ba007d94 |
| nav/walk_{off,off2,on}.navkeys.txt | efc763d4e47cac7cdf755fc145b996f6892906b4ca5730eebcd127c7c5994f4b |
| nav/demo_{off,on}.navdemo.txt | 376a1765b940d73f306419c8ff44f16cdb3d9da2bef54df9a70b142854665ea1 |
| nav/keys_off/mashed_re.log | 4c4a8d077b7af0e52923a609e9469e6965e6998df4b6400f5cfcd9b123addad8 |
| nav/keys_on/mashed_re.log | 0bfa697e725f4d9aea735035a4c5487412f958c6824d5b0726905bfbe33c1fb1 |
| nav/walk_off/mashed_re.log | c99605873bb1ff9e93719c5a156a32305373256c38f8cde731272ac5355c202b |
| nav/walk_on/mashed_re.log | 8845eaaca38a39f455f770f10094db692485a9849ed2873b1dc925cba44ccfb2 |
| nav/demo_off/mashed_re.log | 0b6c7bad4edda1d633cc023edb52f1ef78b8e5c701339bf53d6782d88c086a41 |
| nav/demo_on/mashed_re.log | c3c67c2b06a586f9db433e16867206689b3a08e681a3584cc42294c1698a2b4a |
| exe_l100/mashed_re.exe (x87 build, not kept) | 0a1ab3692fc746ac41d7924af47df61261052b9cf580f1e3e3059f0e213df391 |
| exe_l100_sse/mashed_re.exe (committed build) | 44b3da068a8dc3f80370c27da58033556015f5a89a48aa07001b2f268875ec1f |
| exe_l100_sse/mashed_re_dev.asi | b1fb259cd26d745089da7979aab5a227b61d38b9254662e92b4a072a9fe218e7 |
| l100/nav/keys_off.navkeys.txt (= keys_on, x87) | 2e8cc64f6e0fb7e6e96c0d28ebd68e0ccbf56f5500beb9d110ecc7c6876c1bb4 |
| l100/nav/walk_off.navkeys.txt (= walk_on, x87) | dc862c7ed98ef02782b4ce069173265946799f3a15ee8f9918893106408f4035 |
| l100sse/nav/keys_{off,on}.navkeys.txt | 4e5e35a2f1b2e96e986d678804c959b9b8683ef8499a7d7f0b1c68c3ba007d94 |
| l100sse/nav/walk_{off,on}.navkeys.txt | efc763d4e47cac7cdf755fc145b996f6892906b4ca5730eebcd127c7c5994f4b |
| l100/run.log (x87 gates + logs) | e7f2c64212fa8771d2d579d7f65b9da659ea2ca813c5076b19f3c3ed622868a1 |
| l100sse/run.log (SSE2 gates + knob-ON logs) | 39086197fc02bc1c07d287834b4f0ca943f1ed60136f9dc6b5788f43ebfe6d76 |
| l100/arms, l100sse/arms | same sha256 per file as the round-1 arms above |
| build_l100.log / build_l100_sse.log | 14991b779063b42e / 94210e53b703f8f8 |
| hooks_before.csv | hooks.csv at `9c653e93b` (for the port before/after) |
