# RESULT PORT-STATE-03 (2026-11-08): frontend menu records at DAT_00898ac0 + the swaps the move unblocks

Branch `orch/PORT-STATE-03` off `orch/PORT-STATE-01` @ `9c653e93b` (PORT-STATE-01 is still in the merge train, not on
`orch/integration` @ `cc56a0a68`, so the brief's `merge orch/integration` step has not run yet: see "Hand-in").
PREREG `b4373fe39` (committed alone with the `run_nav.py --data` option, before any arm).

**Verdict: GREEN.** Under `MASHED_PORTSTATE_FRONTEND` (default OFF, exe only) the frontend menu records now live at
DAT_00898ac0. Three matched bodies run on that state with the knob ON: **0x0042d3e0** MenuEntryArrayInit,
**0x00497450** Pred497450 and **0x0042ad10** (a lane B-frontier row the move unblocked, port `none` -> `body`).
Each keeps today's hand path with the knob OFF. **0x004325c0 is not swapped**: it needs a decision (section 4).
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
| 0x004325c0 | FUN_004325c0 | re/match/s08/004325c0.c | (not touched) | | MenuAnimTickTwin.cpp / same | called -> called |

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

## 4. 0x004325c0: not swapped, a decision for the lead to file

The records move removes 0x004325c0's state blocker: the matched `re/match/s08/004325c0.c` walks `DAT_00898ac0[30]`,
which is now the frontend's records with the knob ON. Two things are still in the way, and neither is mine to settle:

1. **`MASHED_NO_ANIM_TRUNC`** (the L-32 opt-out) lives in the RVA's one body, `MenuAnimTickBody`
   (`Frontend/MenuAnimTickTwin.cpp`, both targets). The matched body truncates through `_ftol` (0x004a2c48), with no
   opt-out. So the exe cannot run only the matched body without deleting that knob. L-13 / L-14 are the precedent
   for deleting a knob as a decision.
2. A knob-ON-only swap would compile two bodies of 0x004325c0 into the exe: the matched one (knob ON) and
   `MenuAnimTickBody` (knob OFF, and ON with NO_ANIM_TRUNC). That is a new `DUP-IN-TARGET` allowlist line, and the
   allowlist header says "ADDING A LINE IS A DECISION". The allowlist has 1 live line today.

Further facts for whoever implements it. The matched body also reads DAT_007f1004 (dt), DAT_0067eca4 (phase),
DAT_0067e914 (gate) and DAT_0067ece0 (item count). The exe frontend holds these in `g_frame_dt`, `g_anim_phase`
(constant 2), `g_anim_gate` (constant 1) and the nav slot's `item_count`. Its only callee, FUN_0042ac50, is static
in the same source and reads no global, so STATE-01's one-call bridge (`ModeWordScope` shape) is valid for them.
The body compares record +0x30 to 0x005f7370 / 0x005f72a0, the screen-24 / screen-18 table addresses, and never
dereferences them. The exe writes 0 at +0x30, so the knob-ON populate would have to store those two addresses for
screens 24 / 18. Options for the decision row:

- (a) Delete `MASHED_NO_ANIM_TRUNC`, swap both targets to the matched source, give the exe's knob-OFF path a copy
  bridge (records to DAT_00898ac0 for the call), and write +0x30 for screens 18 / 24. One body. **Recommended**: it
  ends with one body and no knob-gated row.
- (b) Allowlist a knob-gated exe dual copy until the `MASHED_PORTSTATE_FRONTEND` flip, then delete the hand body.
- (c) Leave 0x004325c0 as is until the frontend flip decision.

Evidence: this file, `Frontend/MenuAnimTickTwin.cpp`, `re/match/s08/004325c0.c`, `re/tools/dual_copy_allowlist.txt`.
I did not file it with `roadmap.ps1 decision PORT-STATE-03`, because that moves this step's own row to `decision`.

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

`orch/PORT-STATE-01` is not on `orch/integration` yet (`git merge-base --is-ancestor` = no at hand-in), so
`git -c merge.autoStash=false merge orch/integration` has not been run. Once STATE-01 lands, the merge brings in
only `orch/integration`'s commits after `cc56a0a68`. This branch touches `MenuNavSM.{h,cpp}`, `PortStateFrontend.h`,
`exe_main.cpp` (`NAV_KEYS` line), `exe_sources.rsp`, `x87_tus.txt`, `hooks.csv` (3 rows) and `CHANGELOG.md`.

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
| hooks_before.csv | hooks.csv at `9c653e93b` (for the port before/after) |
