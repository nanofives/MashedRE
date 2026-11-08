# RESULT PORT-STATE-01 (2026-11-08): port-state enabler, inventory + design + first slice

Branch `orch/PORT-STATE-01` off `orch/integration` @ `ea666edab`. PREREG `c72fea2a2` (committed alone), change
`37ef03382`.

**Verdict: GREEN.** 1 decision filed (**L-99**, how the exe gets the original's file-backed .rdata/.data values;
it needs original-binary bytes in the exe, so nothing of it is implemented). The first slice does not depend on it:
the frontend game-mode word moves to DAT_0067e9fc behind `MASHED_PORTSTATE_FRONTEND` (default OFF), and two of
today's NOT-SAFE rows, **0x00430830 SplitScreenTrackAssignment** and **0x004309b0 FrontendModeIndex**, are swapped to
their byte-matched sources in both targets on the default build. Default gates: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, unchanged; the knob-ON arms give the same hashes.

## 1. Inventory (`INVENTORY.tsv`, `ROWS.tsv`, `SEED_CANDIDATES.tsv`, generator `inventory.py`)

Rows: the 256 lane-A/B rows of `verify/portplan_20261108/lanes.tsv` (A-dual 7, A-swap-called 81, B-frontier 168),
WinMain 0x00492370, the run function 0x00492290, MainLoopInit 0x00492770 (lane TOP) and their 26 level-1 callees
(TOP-L1). 285 rows. For each row the body of its RVA is cut out of its matched source (from its `// MATCH: 0x<rva>`
line to the next one) and every `MGLOBAL` / `MGLOBALA` the body names is listed (663 references): address, size from
the declared type, PE section and file-backing from `original/MASHED.exe.unpatched` (`.rdata` 0x005cc000 raw 0x1e000,
`.data` 0x005ea000 raw 0x4d000, VirtualSize 0x32a704, so .data past +0x4d000 is BSS), whether the file bytes are
nonzero, and how the exe sources name the address (0x literal in code, an 8-hex identifier in code, comments only,
nowhere). `compiled` = the exe already compiles that matched source (`#include` of it in an exe TU).

Blocker definitions (a row can carry several):

| blocker | meaning |
|---|---|
| RDATA | the body reads a nonzero file-backed .rdata value. The exe's image pad (exe_main.cpp B17 `g_b17_low_arena_pad`, B7 `kMashedDataBase` 0x00500000) maps it as 0 and nothing seeds it |
| IDATA | the same for nonzero initialized .data (some are written later at run time by the port; the `exe` column shows whether the port names the address) |
| STATE | a BSS / zero-initialized global the exe names only in comments or as a private copy, never as an address literal: the port keeps that state elsewhere (a shadow) |
| NOWRITE | a BSS global the exe never names and the body does not assign: nothing in the exe maintains it |
| CALLEE | a `FUN_` callee whose `hooks.csv` port column is `none`, not one of the 6 exe thunks and not in the same matched file |

Counts. Open rows are the 224 whose matched source the exe does not compile yet; the 61 compiled rows are already
swapped (their STATE flags say what they read, they are not blockers any more).

| lane | rows | no blocker | RDATA | IDATA | STATE | NOWRITE | CALLEE |
|---|---|---|---|---|---|---|---|
| A-swap-called, open | 74 | 40 | 6 | 15 | 3 | 2 | 27 |
| B-frontier, open | 132 | 57 | 4 | 10 | 21 | 13 | 56 |
| TOP (3 rows, 1 compiled) | 3 | 0 | 1 | 0 | 0 | 0 | 3 |
| TOP-L1, open | 16 | 4 | 2 | 3 | 4 | 3 | 9 |
| **all open** | **224** | **101** | **13** | **28** | **28** | **18** | **94** |

Rows with exactly one blocker: CALLEE 50, STATE 12, IDATA 9, NOWRITE 4. Rows blocked only by data (RDATA/IDATA,
nothing else) = 9: 0x0040e470, 0x0040e480, 0x0040ce80 (all three `PTR_PTR_005f2770`, file value 0x005f2728), 0x004325c0,
0x004769f0, 0x0046dd80, 0x004769d0, 0x00476ae0, 0x00476b30. The 37 data rows read 100 distinct addresses, 1,430 bytes:
23 .rdata strings, 9 .rdata scalars, 66 .data scalars/arrays, 2 .data pointer tables (`SEED_CANDIDATES.tsv`). The port
names 19 of those 100 in code. The 94 CALLEE rows miss 225 distinct callees; the most shared are 0x004671a0 and
0x0045b350 (6 rows each), 0x004c15c0 (5), 0x004c1340 / 0x004e6ab0 / 0x004c3ac0 / 0x004950b0 (4 each) (RwFrame and
math families).

Limits of the text classifier (it is an inventory aid; the per-row reasons in the lane RESULTs are the authority):

- STATE is **undercounted** where an address is a literal in one TU and shadowed in another. DAT_0067e9fc is the case
  in point: the race side reads it literally (`TrackRenderer.cpp:239`, written by `MenuLaunchRace`), the frontend kept
  it in `g_game_state.game_mode`, so 0x00430830 / 0x004309b0 came out `none` although PORT-FE-01 found them blocked by
  exactly that shadow. The frontend rows verified by hand in PORT-FE-01 are 0x00430830, 0x004309b0, 0x0042d3e0 (port
  analogue `RecordsZero`), 0x00497450 (`Q_PlayerActive` on `g_game_state.player_active`), 0x004325c0 (`g_records`),
  0x00432b30.
- NOWRITE does not see writers that reach a global through a computed base.
- A file-backed .data global the port writes at run time before any matched read (the course table under
  `MASHED_PORTSTATE_COURSE`, `PTR_PTR_005f2770` seeded by `Ai_BridgeLoad`) still shows as IDATA.

Today's 16 NOT-SAFE rows against these blockers: CALLEE carries 8 (0x00410510, 0x00448940, 0x00448dc0, 0x004756e0,
0x00476df0, 0x00498a00, 0x004b4b60, 0x004c3c60), state/shadow 6 (0x00430830, 0x004309b0, 0x004325c0, 0x00432b30,
0x0042d3e0 analogue, 0x00497450; plus 0x00498e40 NOWRITE: the mode tables have no exe writer), data 2 (0x0040e480
`PTR_PTR_005f2770` with no null guard, 0x00498a00 .rdata format string). 0x004325c0 also has a behaviour knob.

## 2. Design

### 2a. Original constants and initialized data at their original addresses (decision L-99)

The pad is the right place: the exe's image already spans 0x00401000..0x009fffff at the original addresses
(B17), so a seeded value needs no relocation, pointer tables included (`PTR_PTR_005f2770` = 0x005f2728 points into
the same pad). What is open is where the bytes come from. Options (filed as L-99, evidence this file and
`SEED_CANDIDATES.tsv`):

| option | how | original bytes in the exe | covers |
|---|---|---|---|
| (a) per-address generated seed | a `re/tools` generator reads `original/MASHED.exe.unpatched` and writes a committed C table `{addr, bytes}` for exactly the inventory's addresses, with an FNV-1a check and a lint that fails when a matched body in the exe reads an unseeded file-backed address. The exe writes the table into the pad at boot, behind `MASHED_PORTSTATE_SEED` (default OFF), then a decided flip | yes, 1,430 bytes today, growing with the lanes | all 100 |
| (b) whole sections | the same for all file-backed .rdata (0x1e000) + .data (0x4d000) | yes, about 428 KB | everything, no list |
| (c) run-time read | read the values from `original/MASHED.exe(.unpatched)` when present | no | dev only: the LAN release ships no MASHED.exe (L-64 b) |
| (d) no original bytes | bind per body to port copies (PORT-SAVE-01's `SettingsCfgGlobal`), or hand-transcribe per row with a cite | no new | the 19 addresses the port already holds; the plan counts port-side rebinds as NOT-SAFE |

Precedents that already put original bytes in mashed_re.exe: L-64 (b) (RWTEXDICTIONARY/404 as a project asset) and
L-78 (c) MATCH-PORTSTATE-COURSE (`Track/CourseTableState.cpp`, four .data ranges transcribed by hand with an FNV
check). Recommendation: **(a)**. Every embedded byte is one a matched body reads, and the table regenerates from the
inventory as lanes A/B and the top-down lane grow. Not implemented here, per the brief.

Ordering detail for (a): a seed must land before the first reader. The pad is zero at load, so the seed copy belongs
at the top of WinMain, before B7 and before any static constructor that reads a seeded address (none found in the
inventory; the generator should check `.CRT$XCU` constructors' reads as part of the lint).

### 2b. Frontend state onto the original addresses

The exe frontend keeps the original's scattered globals in two port structures:

- `MenuGameState g_game_state` (`Frontend/MenuNavSM.h:77`): 27 fields, each one original global (or one row of one), not contiguous
  (`team_slot[4]` at 0x007f1a14 stride 0x10, `player_active[4]` at 0x007e96fc stride 0x200, the rest single words in
  0x0067e9fc..0x0067ed6c, 0x007f0ad4, 0x007f0f2c, 0x0067f184). The struct cannot be placed at one address, so the move
  is per field: each field gets a word accessor that returns the original address with the knob ON and the port store
  with it OFF (`Nav_GameModeWord()` is the first), and the field is renamed `<name>_off` so the compiler finds every
  direct use. Strided arrays get an accessor taking the index.
- `MenuRecord g_records[31]` (`MenuNavSM.cpp:376`, "0x00898ac0 analogue", 0x34 bytes each, contiguous): this one
  moves like the vehicle records of L-88: `g_records` becomes `MenuRecord* const`, the knob picks 0x00898ac0 or the
  port store. `NavSlot` (depth-indexed arrays at 0x0067ed38..0x0067edb4, stride 0x10) is the third group.

One knob for the whole frontend (`MASHED_PORTSTATE_FRONTEND`), grown field by field, each slice with its PREREG and
knob-ON arms; then one decided flip, as L-88. The gates' dev-race route runs no frontend, so the frontend slices are
scored on the menu walk (`MASHED_NAV_DEMO`, `MASHED_GOTO` + `MASHED_NAV_KEYS`), nav_coverage and the drawlist, with
G-CTRL / B as the regression guard.

Proposed next slices: (2) `g_records` -> 0x00898ac0 (unblocks 0x004325c0 and unifies `RecordsZero` with the matched
0x0042d3e0, whose wider clear is the open parity question at `MenuNavSM.cpp:398-402`); (3) `player_active` ->
0x007e96fc (0x00497450; drop `Q_PlayerActive`'s extra `player < 0` guard only with a cite); (4) the NavSlot arrays.

### 2c. The bridge for matched leaves that read a shadowed word

`ModeWordScope` (`Frontend/PortStateFrontend.h`) lets a matched **leaf** land on the default build before its state
moves: for one call it puts the value the hand core was passed into the original word and restores the old value
after. Conditions, all checked for 0x00430830 / 0x004309b0: the body has no callees, it runs on the main thread, and
the value is exactly what the hand core received. With the state move ON the witness counts how often the bridge
still had to write (`bridged=`); 0 at a site means the matched body reads the frontend's own word with no bridge. A
body with callees cannot use it (a callee could observe the bridged word).

## 3. Slice landed

| commit | content |
|---|---|
| `c72fea2a2` | PREREG (alone) + `run_arms.py` `STACK['PSF'] = ['MASHED_PORTSTATE_FRONTEND']` |
| `c69ee4505` | inventory (`inventory.py`, `INVENTORY.tsv`, `ROWS.tsv`, `SEED_CANDIDATES.tsv`) + `run_nav.py` |
| `37ef03382` | `Frontend/PortStateFrontend.{h,cpp}` (exe only): knob, bridge, witness. `MenuGameState::game_mode` -> `game_mode_off`, `Nav_GameModeWord()`; 15 sites in `MenuNavSM.cpp`, 5 in `exe_main.cpp` go through it. 0x00430830 / 0x004309b0: `Match/MatchS07_00430830.cpp`, `Match/MatchS07_004309b0.cpp` (MATCH-S07 wrapper shape, both targets), hand bodies removed (`MenuScoreSort.cpp` block, `Frontend/FrontendMode.cpp` deleted), `RowMovers.h` declares the two matched exports. Exe call sites: `RowAvail` (row movers), `SplitScreenArrowsLocked` (s24 row 3 arrows), `FrontendModeIndexGate` (challenge launch gate, on `launchMode`). `hooks.csv` from `backfill_exe_file.py` |

| rva | name | match.py (DDK 9176/9178, /O2 /GS /Ob1) | relocheck | bodies after (exe / asi) | port before -> after |
|---|---|---|---|---|---|
| 0x00430830 | SplitScreenTrackAssignment | 100.0%, 52/52 ins | OK checked=10 | MatchS07_00430830.cpp / same | called -> called |
| 0x004309b0 | FrontendModeIndex | 100.0%, 17/17 ins | OK checked=1 | MatchS07_004309b0.cpp / same | called -> called |

The .asi installs the matched bodies at both RVAs in place of the hand ports. Both hand exports read DAT_0067e9fc
and the 0x007f0a40 table at the same addresses with the same strides as the matched bodies (stride 0x30 for every
mode of 0x00430830 since the 2026-10-12 re-port), so the .asi's observable behaviour is unchanged; no .asi diff was
run (asi-side runs need the deployed .asi, which this worker must not do).

## 4. Checks and predictions

Frozen build: `mashed_re.exe` sha256 `1797934e...`, `mashed_re_dev.asi` `e4989f4e...` (built from `37ef03382` with
`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not deployed). Every game run used the private root
`D:/MashedWT/PORT-STATE-01_root`, `test-slot` outside and `mashed_lock.py run --label PORT-STATE-01` inside. Only
own PIDs were stopped (none needed stopping: every run self-exited with exit 0).

| id | check | result | holds |
|---|---|---|---|
| P1 | default `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9` = `merge_train.json` refs | yes |
| P2 | knob ON `H_PSF B_PSF_K0 B_PSF_K1 B_PSF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, the same as P1 | yes |
| P3 | knob ON witness | menu walk (`MASHED_NAV_DEMO`, real actions single player -> challenge select): `site=004309b0.gate word@0x0067e9fc=3 ret=1 bridged=0`, so the matched 0x004309b0 read the frontend's own word at DAT_0067e9fc with no bridge. Key script (`MASHED_GOTO=1,2,4,6 MASHED_NAV_KEYS=d,u,d,u,e`): `site=00430830.rowavail word=0 ret=0 bridged=0` (the row movers on screen 6; the GOTO jump sets no mode, so the word is 0) and `site=004309b0.gate word=3 bridged=1` (the gate passes `launchMode`, the standalone's 0 -> 3 default, which differs from the word on this dev route only). The s24 arrow site was not reached | yes at the two sites reached |
| P4 | menu walk frames OFF vs ON | **instrument not deterministic**: three knob-OFF walks give three set hashes (`6545e186`, `4c43f743`, `5a92186a`), two knob-ON walks `f4abbf5f`, `5e0d766d`. Changed-pixel counts OFF vs ON lie inside the OFF vs OFF spread (title 1513 in every pair; challenge select OFF/ON 146,831 vs OFF/OFF 150,604 / 157,281; options 98,623 vs 98,779 / 187,017). Replacement witness: the key script's per-frame `NAV_KEYS` state log (185 lines: depth, screen, cursor, challenge row, game mode, selection, flow) is identical OFF vs ON | not scorable as worded; replacement identical |
| P5 | static | `rva_body_scan.py`: one body per target, `Match/MatchS07_00430830.cpp` / `Match/MatchS07_004309b0.cpp` in exe and asi. rva-lint `CROSS-TARGET=1 allowlisted=1 NEW=0`, asm-lint OK, no allowlist line added. `backfill_exe_file.py` moved both `exe_file` cells to the wrappers; `backfill_port.py` "already current", both rows `called` before and after | yes |

Build: `=== Build OK ===` for both targets. Exports `SplitScreenTrackAssignment` / `FrontendModeIndex` present in both
binaries (pefile). nav_coverage not re-run: no screen table changed.

The inventory TSVs were generated on this branch before the slice's code commit (the exe-naming column reflects
`orch/integration` @ `ea666edab`).

## 5. Leg for the lead / re-classify

- `hooks.csv` `file` column still names `Frontend/MenuScoreSort.cpp` (0x00430830) and `Frontend/FrontendMode.cpp`
  (0x004309b0, file deleted). Move both to the `Match/MatchS07_*.cpp` wrappers through re-classify (C4 unchanged).
- L-99 answer -> PORT-STATE-02 (seed, per the chosen option). Frontend slices 2-4 above -> PORT-STATE-03..05.

## Raw data (not committed)

Folder `D:/MashedWT/PORT-STATE-01_data/`:

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/H_PSF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_PSF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_PSF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_PSF_K0.step.csv, B_PSF_K1.step.csv, B_PSF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| exe/mashed_re.exe | 1797934e500c7adfc46621dcf5b0e11188fc02cb8d34e6f98eff8682ae44a041 |
| exe/mashed_re_dev.asi | e4989f4e27d5cad8f634c32927908186d1882951156c22151ec3d63144e7767f |
| gates1_run.log | a400d4512cdf97d5ed07f57bc226fb98482dcd7b6eb37879bf03526dc64f4f40 |
| build1.log | 34ad6c211d98838673361dcbc6793dc7c5d6abc365d4e91bce8f1ac1208d6a25 |
| nav_run.log | 635f318437e22378a04af461508e6010ac68fde0cec0f17c0bad03c004b4ff1f |
| nav_run2.log | aafd07cc39d075a96347177bc6adb6f2c19b86f7c2e8f04e344058de2e34943c |
| nav/keys_off/mashed_re.log | 1a667ab5ba64457ef3c420d7db93f829c1b6f04cea8bc48013cd0af3b5f815ff |
| nav/keys_on/mashed_re.log | aab5d53b5621e871a39038345ec3163c15e61032425ad4e796ddfc94340d22f3 |
| nav/demo_on/mashed_re.log | 70d8b51e32905b17c44bc8734d65051dbc5b2f73fa8a550e66a3d25baaecca6b |

Walk BMPs: `nav/demo_off`, `demo_off2`, `demo_off3`, `demo_on`, `demo_on2` (12 each).
