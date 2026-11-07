# RESULT D4-UNC-PHYS, run 29 (2026-11-07)

Step: physics standalone-input and coverage holes U-9783..U-9786. Branch `orch/D4-UNC-PHYS-2`, cut from `main` @
`51e8a7a30`. Earlier run (BLOCKED on audio): `verify/d4uncphys_20261027/RESULT_D4-UNC-PHYS.md`.

**Verdict: GREEN for U-9783 (resolved, knob corrected, every PREREG prediction holds, default gates unchanged). U-9784, U-9785 and U-9786 are narrowed, not closed.**

- **U-9783: resolved on the original side.** The handling row A3 reads in mode 10 is `0x0067ea94`, a setup-screen
  option that the original keeps apart from the course row. It is 0 on every harness and menu race on this
  install, so A3 runs type 6 (105.0) on every course. The standalone's constant 6 is that value. The default-OFF
  `MASHED_PORTSTATE_GAMEFLOW` knob coupled the two rows (`0x0067ea94 = k`), and this run removes that write.
- **U-9784: original half measured.** `0x0063ba8c` goes 1, 2, 3, 4, 5, 6 on recipe F. The original runs 624 A6a /
  A4 calls while the word is 3, 4 or 5. The port half (a per-tick `Fi_GameMode()` log) is still open.
- **U-9785 / U-9786: a natural witness track exists.** Of the 13 tracks, only track 7 (Roundabout) enters A5's
  random-surface branch on the original (454 entries in 20 s, 4 cars). A6b's motion-state leg ran 0 times on all 13.

Audio: every launch printed the present endpoints first. All runs listed 4 endpoints with status OK (VB-Audio
CABLE Input / Output / In 16ch, Philips FTV). No boot AV this session, and no piz-collision signature.

## 1. Tooling added (`f42e0c5ce`)

- `re/frida/scenario_launch.py`
  - `--pre-u32 rva=val,...`: CONTRIVED writes in `setup`, after the track / mode writes and before the launch poke.
    `--poke-u32` writes inside the hold, after A3 has run, so it could not do this.
  - `--watch-global rva,...` (max 4): hardware write watchpoints on image globals. They are armed after attach,
    re-armed at the menu and again before launch.
  - Each hit row has the hit number, the pc after the store (an image rva, or `module!offset`), the watched
    u32s, `0x0063ba8c`, tick `0x007f0ff4`, the `MASHED_COUNT_RVAS` counters, a backtrace and a stack scan.
  - A menu `--peek` before setup.
- `re/frida/statenav.py` (menu route), all default off:
  - `MASHED_NAV_PEEK`: plain reads at each nav milestone and every 10 s of the round.
  - `MASHED_NAV_POKE`: CONTRIVED writes on the track-select screen.
  - `MASHED_NAV_TRACK_SEQ` / `MASHED_NAV_GM_SEQ`: control-code sequences on the track-select and Game Mode screens.

All runs used the no-hook original (`MASHED_RE_NO_AUTO_HOOK=1`) from the private root `D:\MashedWT\D4-UNC-PHYS_root`
(its `MASHED.exe`, `d3d9.dll`, `d3d9_real.dll`, `videocfg.bin` and `gamesave.bin` hash equal to `original\`).
Every run was wrapped in `test-slot` and `mashed_lock.py run --label D4-UNC-PHYS-2`.

## 2. U-9783, harness route (`u9783_harness_legs.txt`)

`--peek` of the chain and the four handling globals, plus `--watch-global 005f2770,0067ea94,00613108,0067e9fc`,
mode 10, 1 car, hold 10 s.

| run | track | car | `--pre-u32` | `0067ea94` | `00613108` | `00613114` | `00613130` | `0061313c` |
|---|---|---|---|---|---|---|---|---|
| t0b | 0 | 0 | none | 0 | 105.0 | 40000 | 1.0 | 1.5000001 |
| t3 | 3 | 0 | none | 0 | 105.0 | 40000 | 1.0 | 1.5000001 |
| t10 | 10 | 0 | none | 0 | 105.0 | 40000 | 1.0 | 1.5000001 |
| t11 | 11 | 0 | none | 0 | 105.0 | 40000 | 1.0 | 1.5000001 |
| t0c2 | 0 | 2 | none | 0 | 105.0 | 40000 | 1.0 | 1.5000001 |
| t3p | 3 | 0 | `0067ea94=3` | 3 | 100.0 | 42000 | 1.0 | 1.5000001 |
| t10p | 10 | 0 | `0067ea94=a` | 10 | 95.0 | 40000 | 1.0 | 1.5000001 |
| t11p | 11 | 0 | `0067ea94=b` | 11 | 100.0 | 45000 | 1.0 | 1.4000001 |

Every value matches the 13-row table in the 2026-10-27 RESULT, section 2 (`v` = 0 / 3 / 10 / 11 gives types
6 / 18 / 12 / 66).

- **The table is live and static.** At the menu and through the race:
  - `[0x005f2770]` = `0x005f2728` (6235944).
  - The 13 pointers `0x005f2728..0x005f2758` and the 13 record `+4` words keep their image values (types 6, 30,
    48, 18, 24, 0, 36, 42, 54, 60, 12, 66, 72 for `v` = 0..12).
  - No write to `0x005f2770` was trapped from attach through the race on any run. This is the live second witness
    the static scan asked for.
- **A3 is the only writer of `0x00613108` in the race.** Each run trapped 6 writes, 3 A3 calls (`0x0063ba8c` = 1,
  2, 3), each a pair:
  - First 100.0, from A3's prologue store `mov dword ptr [0x613108], 0x42c80000` at `0x0046b544`. The trapped pc is
    off-image (`0x...0a28`, a different base per run): the harness's own `Interceptor` on A3 entry (`SPAWN_RVA` =
    `0x0046b540`, `scenario_launch.py:55`) relocated that store into its trampoline.
  - Then the table value from `fstp dword ptr [0x613108]` at `0x0046b5a7` (pc after it = `0x0046b5ad`).
- **`0x0067ea94` has no game writer on this route.** It is 0 at the menu and in the race. Only the `--pre-u32`
  value appears, and it was written from Frida's own thread, which is not trapped.
- `_DAT_0061313c` after A3 is 1.5000001 (`(float)1500 * 0.001f`, `0x0046b5c7`), not the prologue's 1.5
  (`0x0046b562`). The standalone computes the same product (`Vehicle/VehicleInit.cpp`, `kScale001`).

**So the harness's 13-track references were taken at type 6 (105.0) on every track.** The open item (2) of U-9783
is answered yes.

## 3. U-9783, menu route (`u9783_menu_legs.txt`)

`statenav.py`: title, single player, Quick Battle, colour, track-select (depth 5, screen id `[0x0067ee7c]` = 6),
then the Game Mode screen (depth 6, screen id `[0x0067eebc]` = `0x18`), then start.

- **nav0, defaults.** `0x0067e9fc` = 10. `0x0067ea94` = `0x0067f17c` = `0x0063ba7c` = `0x0063ba78` = 0.
  `0x00613108` = 100.0 in the menu and 105.0 in the round. Same as the harness.
- **Where `0x0067ea94` is stepped.** `FUN_0043dfd0` (decomp `decomp_43dfd0.txt`, lines 1451-1530 and 1687-1760):
  - Screen `0x12`, cursor row 4: `0x0044024d..0x00440268` (left, -1, wrap to `0xc`) and `0x0044079d..0x004407b8`
    (right, +1, wrap to 0).
  - Screen `0x18`, cursor row 3: its own left / right arms (decomp lines 1508-1530 and 1740-1760), same step and
    wrap.
  - Each step skips rows where `FUN_00430830(i) == 0`. In mode 10 that is `[0x007f0a44 + i*0x30]`
    (`0x00430848..0x00430852`).
  - Left is gated by `FUN_0042b310` (control byte `+0x2c`, `0x007f1044 + slot*0x4c`), right by `FUN_0042b540`
    (`+0x2d`).
- **navp4.** The availability words read `[0x007f0a44]` = 1 and rows 1..12 = 0 on this install's save. The cursor
  skips row 3 (it goes 0, 1, 2, 4, 5), so the option cannot leave row 0. Every menu Quick Battle on this save runs
  `v` = 0, type 6, on any course.
- **navp5..navp7, CONTRIVED.** `0x007f0a74 = 1` and `0x007f0ad4 = 1` (rows 1 and 3 available) were poked on the
  track-select screen. On screen `0x18` the cursor then reaches row 3.
  - Control 9 steps left: 0 to 3, through 12..4, which are unavailable. Control 10 steps right: 3 to 0. Controls
    0-3, 6-8 and 13-20 do nothing there. Control 5 goes back one screen (navp2).
  - navp7 picked row 3 with control 9 and started. In the round `0x0067ea94` = 3, and A3 gave 100.0 / 42000.0
    (type 18).
  - In the same round `0x0067f17c` = `0x0063ba7c` = `0x0063ba78` = 0. `0x0063ba78` is written at `0x0040d45b` from
    `0x0063ba7c` in the course-load case `FUN_0040d440`.
  - So the course row stayed 0 while the handling row was 3.
- **`0x0067ea94` has other readers.** `FUN_0040d110` (called from the course loader `FUN_0040d270`) reads the same
  mode-10 row: `iVar2 = *(int*)(table[ FUN_00431d70() ] + 4)`. It passes `(iVar2 / 6) * 6` and
  `[0x007f1a1c + s*0x10] + (iVar2 / 6) * 6` to `FUN_00420230` and `FUN_00420420(s, ...)` for the 4 slots
  (`decomp_40d0.txt`). `FUN_0040d040` has the same arm. The exe's frontend names the word "vehicle"
  (`Frontend/MenuNavSM.h:115`). This run does not name it: the record `+4` word drives both the handling row and
  that per-slot value.

## 4. U-9783, the standalone

- Default build: `VehicleInit` uses the constant type 6 (`HandlingTypeIndex`, `Vehicle/VehicleInit.cpp:109`).
  Sections 2 and 3 show the original runs type 6 on every course of both routes on this save. The default is
  therefore what the original runs, not an approximation. A non-zero row needs a save with more rows available,
  and the exe's frontend then has to carry the setup option.
- `MASHED_PORTSTATE_GAMEFLOW` (default OFF) runs the original's selector chain (`GameModeState_HandlingType`).
  Before this run, `GameModeState_OnCourseLoad` also wrote `0x0067ea94 = k` (the course's row), so Arctic got type
  18 (MATCH-PORTSTATE-GAMEFLOW P5). Section 3 shows the original does not do that.
- Change `1b2bc74a5`: `OnCourseLoad` no longer writes `0x0067ea94`. The word keeps what the exe's frontend wrote
  (`Frontend/GameModeCarSelect.cpp`, the `0x00431d00` port, writes 0). Comment updates are in
  `Race/GameModeState.h` and `D3d9Render/TrackRenderer.cpp`. Exe only, knob only.
- PREREG `4b16fd21a` (`PREREG_D4-UNC-PHYS-2.md`) was committed alone before the arms.

### Arms

Exe `663642b5` (built from `1b2bc74a5`, `MASHED_NO_DEPLOY=1`): `=== Build OK ===`,
`[rva-lint] ... allowlisted=1 NEW=0`, `[asm-lint] OK`. Command: `verify/savelink_20261023/run_arms.py H_DEF
B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSG B_PSG_K0 B_DEFA_K0 B_PSGA_K0`, `A4_ROOT=D:/MashedWT/D4-UNC-PHYS_root`. All 8 exited
0. Witness lines: `witness_arms.txt`.

| id | prediction | measured | holds |
|---|---|---|---|
| P1 | H_DEF `8284be46 / 67931834 / 31fe043e`, B_DEF_K0/K1/K26 `1d5bf4f9` x3 | H_DEF `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9` | yes |
| P2 | H_PSG / B_PSG_K0: `qbrow=0 ... v_key=6`, `a3 mode=10 v=0 type=6`, hashes = P1 | `course=30 k=0 mode=10 row=0 qbrow=0 sel=0 ld=0 v_rec_id=30 v_key=6`, `a3 mode=10 v=0 type=6` on both. H_PSG `8284be46 / 67931834 / 31fe043e`, B_PSG_K0 `1d5bf4f9` | yes |
| P3 | B_PSGA_K0 (Arctic): `course=0 k=3 ... qbrow=0 ... v_key=6`, `a3 mode=10 v=0 type=6` | `course=0 k=3 mode=10 row=3 qbrow=0 sel=3 ld=3 v_rec_id=30 v_key=6`, `a3 mode=10 v=0 type=6` | yes |
| P4 | report only. H0: B_PSGA_K0 = B_DEFA_K0 | both `b030be28` (397 lines) | H0 holds |

P4 shows that the handling row was the only difference GAMEFLOW made on Arctic's first 1200 frames. The
MATCH-PORTSTATE-GAMEFLOW P5 difference (B_DEFA_K0 `b030be28` vs B_PSGA_K0 `f28b5695`) came from the
`0x0067ea94 = k` write this run removes. With it removed, the knob gives the original's harness / menu value on a
non-Training course.

## 5. U-9784 (`u9784_gm_word.txt`)

Original, recipe F (track 0, 4 cars, rule 0, mode 10), `--watch-global 0063ba8c`, `MASHED_COUNT_RVAS` on A6a
`0x00467650` and A4 `0x00470670`, hold 15 s. Every write to `0x0063ba8c` (pc = the instruction after the store):

| hit | new value | pc | tick `0x007f0ff4` | A6a calls so far | A4 calls so far |
|---|---|---|---|---|---|
| 1 | 1 | `0x0040d3f1` | 0 | 0 | 0 |
| 2 | 2 | `0x004100e7` | 0 | 0 | 0 |
| 3 | 3 | `0x00410283` | 0 | 0 | 0 |
| 4 | 4 | `0x00410391` | 2150 | 172 | 172 |
| 5 | 5 | `0x0040dc4a` | 2200 | 176 | 176 |
| 6 | 6 | `0x004104ed` | 7800 | 624 | 624 |

End of the hold: 3647 calls each.

- The original runs A6a and A4 while the word is below 6: 172 calls at 3, 4 at 4, 448 at 5, so 624 before 6.
- A6a branches on `== 6` at `0x00467cfe` (the `+0xbf4` decrement), so those 624 calls skip it on the original.
- The standalone's `Fi_GameMode()` returns 6 except on the d2engage hold tick (5, `VehiclePhysicsRun.cpp:1292`).
- **Open:** whether and when the standalone runs A6a / A4 in its own countdown. That needs a port-side per-tick log
  of `Fi_GameMode()` and the A6a call count, which this run did not add.

## 6. U-9785 / U-9786 (`u9785_u9786_scan.txt`)

Original, `scenario_launch.py --track N --cars 4 --rule 0 --mode 10 --hold 20` for N = 0..12, no hooks. All 13
reached a race, and each launch listed 4 OK audio endpoints. `MASHED_COUNT_RVAS` counted entries at three
addresses:

- `0x0046e78c`: the first instruction of A5's random-surface branch, after `jne 0x0046e7d5` at `0x0046e78a`. The
  branch holds the two `FUN_00472650` calls at `0x0046e7ac` / `0x0046e7c3`, which store into `+0xb00` / `+0xb08`.
- `0x00468980`: A6b entry.
- `0x00468abb`: A6b's motion-state != 0 leg.

| track | `0x0046e78c` | `0x00468980` | `0x00468abb` |
|---|---|---|---|
| 0 | 0 | 4848 | 0 |
| 1 | 0 | 4619 | 0 |
| 2 | 0 | 3544 | 0 |
| 3 | 0 | 4424 | 0 |
| 4 | 0 | 4974 | 0 |
| 5 | 0 | 4653 | 0 |
| 6 | 0 | 3893 | 0 |
| **7** | **454** | 3989 | 0 |
| 8 | 0 | 4879 | 0 |
| 9 | 0 | 4850 | 0 |
| 10 | 0 | 4964 | 0 |
| 11 | 0 | 4530 | 0 |
| 12 | 0 | 4306 | 0 |

- **U-9785 / U-9786 (random-surface path).** Track 7 is the only track here whose race enters the branch. So
  "a track with surface key `0xffff32ff`" now has a candidate: track 7 (`scenario_launch.py` names it Roundabout).
  The record and wheel that took it were not logged.
- **Next:** bind `Fi_RandRange` to the shared PRNG stream, then trace track 7 on both sides (U-9785). Run an A5 A/B
  on track 7 with the hook installed (U-9786, random-surface path).
- **U-9786 (A6b motion-state leg).** 0 entries on every track with no driving input. Natural air still needs a
  driven launch (Warzone jumps, a powerup), as the row says.
- The `MASHED_A5_CONTACT_INT` / `0x80000000` contact-word leg was not measured.

## 7. Trackers, decision, next

- **U-9783 resolved** with `re/tools/uncertainty_resolve.py` (re-classify's documented operation). No function
  changes level: A3 `0x0046b540` is already C4 on its shared body. The selector input is now measured, and the
  standalone default runs the original's value.
- The `[UNCERTAIN U-9783]` comment in `Vehicle/VehicleInit.cpp` was rewritten to cite this result. The exe
  rebuilt after that edit (`1dbb23fd`) differs from the measured exe `663642b5` in 4 bytes only: the PE header
  and debug-directory timestamps (file offsets `0x128` and `0x283808`). The code bytes are identical.
- **U-9784, U-9785 and U-9786 stay Active.** re-classify has no narrowing operation (only resolve, or partial
  with a split row), so this RESULT carries their new evidence (sections 5 and 6).
- **Decision: none.** The code change is inside the default-OFF `MASHED_PORTSTATE_GAMEFLOW`, and no default hash
  moved. For whoever files the GAMEFLOW flip: the knob no longer changes the handling row on non-Training courses.
  On this exe's first 1200 Arctic frames it is equal to the default (P4).
- **Next:**
  1. U-9784: a port-side per-tick log of `Fi_GameMode()` and the A6a / A4 call count over recipe F's countdown,
     next to section 5's original table.
  2. U-9785 / U-9786: track 7 on both sides, `Fi_RandRange` on the shared stream, and an A5 A/B on track 7.
  3. U-9786: natural air with a driven launch.

## Files

Committed (`verify/d4uncphys_20261107/`): `PREREG_D4-UNC-PHYS-2.md`, this RESULT, `u9783_harness_legs.txt`,
`u9783_menu_legs.txt`, `u9784_gm_word.txt`, `witness_arms.txt`, `u9785_u9786_scan.txt`, `raw_sha256.txt` (SHA-256 of every raw log, decomp dump and
arm file cited here, all under `D:/MashedWT/D4-UNC-PHYS-2_data/`).
