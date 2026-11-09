# RESULT PORT-STATE-02 (2026-11-08): the L-99 (a) seed table, MASHED_PORTSTATE_SEED default OFF

Branch `orch/PORT-STATE-02` off `orch/PORT-STATE-01` @ `9c653e93b` (PORT-STATE-01 is not on `orch/integration` yet, so
no merge of integration was made; `git merge-tree` against `orch/integration` @ `16d6d524d` is clean). Commits: code
`84d098ac9` + `894f7a084`, PREREG `ffe0d994a` (alone), tooling `81088a5da` + `bbadf8afe`, this RESULT.

**Verdict: GREEN on every pre-registered arm, plus one open item, the default flip (filed as a decision).** The seed
puts the original's file-backed bytes for the 100 inventory addresses into the exe's pad. With it ON, every default
gate, D2, the 13-track table, boot-to-menu and a Quick Battle are unchanged. The lane-A retry shows what it unblocks:
the byte-matched `ConfigLoad` / `ConfigSave` (0x00498950 / 0x004989b0) run in the exe with the seed ON and give the
hand body's result. With the seed OFF they kill the process.

## 1. What landed

| file | content |
|---|---|
| `re/tools/gen_portstate_seed.py` | reads `original/MASHED.exe.unpatched` (refuses unless SHA-256 = the anchor `BDCAE093...3C0E`), maps each `SEED_CANDIDATES.tsv` address through the PE section headers, writes the table. Refuses: no section, the section's uninitialized tail (no file bytes), overlapping ranges, a string entry not ending on its NUL. `--check` fails when the table differs from the inventory (address/length list and the inventory's SHA-256, CRLF folded), from the binary, or from its own FNV-1a |
| `mashedmod/src/mashed_re/Compat/PortStateSeed.inc` | generated: 100 entries, 1,430 bytes, FNV-1a `0xdd093ea5` over (addr, len, bytes) in address order |
| `mashedmod/src/mashed_re/Compat/PortStateSeed.{h,cpp}` | exe only. `PortStateSeed_Apply()` is the first statement of `WinMain`: when `MASHED_PORTSTATE_SEED` is set (read once) it recomputes the FNV from the arrays (mismatch = no write), checks each entry lies inside `g_b17_low_arena_pad`, and copies. `PortStateSeed_Log()` after the log truncate writes one `[portstate-seed]` line with the counts and the FNV of the table and of the pad memory. OFF: no write, no log |
| `mashedmod/build.bat` | runs `gen_portstate_seed.py --check` after asm-lint. Skipped when `re\tools` is absent (the LAN git-archive build), binary leg skipped without the binary, `MASHED_SKIP_SEED_LINT=1` opt-out. No `.ps1` / `.sh` edit was needed |
| `exe_main.cpp`, `exe_sources.rsp` | the two calls and the TU |

Generator refusal paths exercised by hand: a BSS address (0x00700000: "reaches past .data file data (raw 0x4d000)"), the
patched `MASHED.exe` as input (anchor mismatch), a tampered table byte (FNV and 9 entries reported), a 49-row inventory
(51 extra entries reported), no binary (`--check` passes with the binary leg SKIPPED, the generator refuses).

Order: the seed is copied at `WinMain` entry. The pad is image `.bss` (committed at load), so nothing has to be mapped
first. Static constructors run before `WinMain`; PORT-STATE-01 found none reading an inventory address. The lint does
not scan `.CRT$XCU` constructors (not implemented, see section 5).

## 2. Arms (PREREG `ffe0d994a`)

Frozen exe `8cdd450f` (built from `894f7a084`, `MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not deployed). Private
roots `D:/MashedWT/PORT-STATE-02_root`, `_root13`. Every boot: `test-slot` outside, `mashed_lock.py run --label
PORT-STATE-02` inside, `--solo` for D2. No PID was stopped by hand; every run self-exited except the D2 arms, which the
runner stops at its 110 s wall (own PID, by design).

| id | result | holds |
|---|---|---|
| S1 | `=== Build OK ===` both targets; rva-lint `CROSS-TARGET=1 allowlisted=1 NEW=0`; asm-lint OK; `[portstate-seed] OK: 100 entries, 1430 bytes, fnv 0xdd093ea5, binary checked`. `backfill_exe_file.py` wrote no change (git clean), `backfill_port.py` "hooks.csv already current" | yes |
| P1 | `H_DEF` `8284be46/67931834/31fe043e`, `B_DEF_K0/K1/K26` `1d5bf4f9` x3 = `merge_train.json` refs | yes |
| P2 | `H_PSS` `8284be46/67931834/31fe043e`, `B_PSS_K0/K1/K26` `1d5bf4f9` x3 | yes |
| P3 | the four ON arm logs carry `[portstate-seed] on entries=100/100 bytes=1430/1430 outside_pad=0 fnv table=dd093ea5 want=dd093ea5 mem=dd093ea5`; no OFF log has a `[portstate-seed]` line | yes |
| P4 | O42 band, all six `in`, H1 398/400 median 1333.9. OFF: drvmed 1919.62 / 1920.25 / 1919.62 (1637/1636/1637 lines). ON: 1919.62 / 1920.25 / 1920.25 (1637/1636/1635). slip1500 0.1934, slip2000 0.2515 in all six | band: yes. "ON triple = OFF triple to the printed digits": **no for run 3** (1919.62 vs 1920.25). Both values occur inside the OFF triple itself (runs 1 and 2), and the motion_diag files repeat across the arms (OFF_1 = OFF_3 = ON_1 `8449ab3b`, OFF_2 = ON_2 `a3e60503`), so it is the wall-clock variation the D2 arms carry, not a seed effect |
| P5 | all 26 track runs OK on the first attempt (TV on). Score tables OFF and ON byte-identical, and OFF = `verify/run13p3_20261029/tracks_score_sub_p3.txt` byte for byte (`2431afb1`, 0 RED). Raw captures: every BMP (104), CSV (26) and capture JSON (53) identical ON vs OFF. The 78 differing files are the scorer's `diff_*.{txt,json}`, which change when the same OFF folder is scored twice (their example-row text is not repeatable; the table is). ON has 2 more files than OFF: the exe's truncated capture names under `stand/r6/` keep one more character in the shorter `t13_on` path (`round1_g` + `round1_r` vs one `round1_` that takes the last write); the shared one is byte-identical | yes. No non-TRAINING row moved, so the layout control was not needed |
| P6 | route `MASHED_DBG_MENU=1 MASHED_NAV_KEYS=x`: OFF x2 and ON x2 all `B17-SUMMARY reached-main-loop chrome=YES thunks=6/6`, exit 0, NAV_KEYS sha8 `5eb64a8c` in all four | yes |
| P7 | route (fixed by OFF probes before any ON run): `MASHED_DBG_MENU=1 MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200 MASHED_NAV_KEYS_GAP=60` = main menu, screen 2, row 1, screen 4, screen 6, screen 24 (game mode 10, Quick Battle), race (`flow=2`, in-race captures), about 20 s, quit. OFF x2 and ON x2: NAV_KEYS 1687 lines sha8 `3998f57a` in all four, exit 0 | yes. The whole mashed_re.log is not a usable witness: OFF vs OFF already differs (intro movie ms, round-end lines reorder in this wall-clock race). The ON log has 30 more lines, all B17 region-map rows: the seed's writes turn 13 pad pages from WRITECOPY (0x08) to private READWRITE (0x04) in the dump |
| P8 | lane-A retry, experiment exe E `45550efe` (`E_matched_videocfg.patch`: `Match/MatchS16_00498950_exp.cpp` includes the byte-matched `re/match/s16/00498950.c`, its callees bound to the exe CRT and log cores, buffers at 0x007731e8 / 0x00773208; built with `MASHED_SKIP_RVA_LINT=1` because the shared TU still carries the hand bodies' RVA comments). Default exe: present `[videocfg] videocfg.bin present, no write`; absent `first-run write ret=1 modes=53 pick=1 'NVIDIA GeForce RTX 3070' '640 x 480 x 32 [Fullscreen]'`, file `7f130171`. E seed OFF: exit `3221226505` = `0xC0000409` in 0-2 s, before the menu, file present or absent (the matched `ConfigLoad` passes the zero bytes at 0x005cf010 as the fopen mode). E seed ON: present `videocfg.bin present, no write`, menu reached; absent `first-run write ret=1`, the same line as the default exe, file `7f130171` (byte-identical) | yes |

## 3. What the seed buys (and what it does not)

- It removes the data blocker of the 13 RDATA and 28 IDATA rows of PORT-STATE-01's inventory (37 rows read the 100
  addresses). P8 is the proof for one: 0x00498950 / 0x004989b0 are blocked only by `.rdata` strings plus CRT callees
  the exe already has, and with the seed they are a working swap.
- A real swap of those two rows needs the seed ON by default: the matched body cannot replace the hand body on today's
  default build (P8 seed OFF = process killed). So every RDATA swap of this kind waits on the flip.
- Rows also blocked by CALLEE / STATE / NOWRITE stay blocked. 9 rows are blocked only by data (RESULT_PORT-STATE-01
  section 1); those are the next candidates once the flip is decided.

## 4. Decision filed

`roadmap.ps1 decision PORT-STATE-02`: flip `MASHED_PORTSTATE_SEED` to default ON? G-CTRL and B do not move
(`8284be46/67931834/31fe043e`, `1d5bf4f9` x3, old = new), D2 in band, 13 tracks identical to the reference. The flip
puts 1,430 bytes of the original binary into `mashed_re.exe` (L-64 b and L-78 c are the precedents). Recommendation: flip,
with `MASHED_NO_PORTSTATE_SEED=1` as the opt-out and P8's swap of 0x00498950 / 0x004989b0 as the first consumer.

## 5. Notes for the lead

- No tracker leg: no RVA body moved in the committed tree.
- The lint checks the table against the inventory and the binary. It does not check that every matched body the exe
  compiles reads only seeded addresses (PORT-STATE-01's design sentence): that needs `inventory.py` to run in the
  build (it reads `hooks.csv` and the rsp files). A follow-up can regenerate `SEED_CANDIDATES.tsv` from `inventory.py`
  as a pre-step and let `--check` fail on a new unseeded address. `.CRT$XCU` constructor reads are not scanned.
- `E_matched_videocfg.patch` has LF line endings in the repo; `git apply --ignore-whitespace` if a CRLF checkout trips it.
- `PTR_FUN_005f99c4` (one of the two pointer entries) seeds an original `.text` address; in the exe a call through it
  is valid only where B16 put a thunk. No exe code calls through it today.

## Raw data (not committed)

Folder `D:/MashedWT/PORT-STATE-02_data/`:

| file | sha256 |
|---|---|
| exe/mashed_re.exe | 8cdd450f86390993e057e989f21919a05286bb5a1fcd44da8b5a3a21ddb587ff |
| exe/mashed_re_dev.asi | 2e1a8bb79bf9510be3426f12f62cced95ccac31da2fc879c5f1c83d5c4845f1b |
| exeE/mashed_re.exe (experiment E) | 45550efe1595c0d4141769bfb28d3657760fb241a3593dd04634b32c48bcd0e2 |
| arms/H_DEF.step.csv = arms/H_PSS.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv = arms/H_PSS.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log = arms/H_PSS.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_{DEF,PSS}_K{0,1,26}.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/D2_DEFH_1, D2_DEFH_3, D2_DEFHPSS_1 .motion_diag.log | 8449ab3ba707ad4e29a68fb806c862fbfb389c0d4a4aea72b5d30126a161c1fd |
| arms/D2_DEFH_2, D2_DEFHPSS_2 .motion_diag.log | a3e605030dfc65e6e36e34f012e93c2755b49f5900e3843d65cdf34eaf4ac1e9 |
| arms/D2_DEFHPSS_3.motion_diag.log | d90936e22891e2eece66c3907d155d84b6ea3443b465a3bcbc18ea9c2c9dd2f4 |
| d2_score.txt | 7f324d5ac10a7f4080177086e24dccc42e9d53095865e8cf63ad09879b9104a7 |
| t13_off_score.txt = t13_on_score.txt | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| videocfg_D_firstrun.bin = runs/P8b_E_ON_absent/videocfg.bin | 7f1301713a118a86372d06b89d503334ddeb6619b7f23088e09ed453d12bab88 |
| runs/P7_OFF_1/mashed_re.log | 4a7c36b3581473e2a3f6a5ea0f82234a7d76b2e4e29cab347c002bc6b853906f |
| runs/P7_ON_1/mashed_re.log | 0d9a06e7d51114cb7206c50e8671690651e8c959c6ab802f23735f23b98e3604 |
| gates_run.log | 65ea6938f75319313bc62ba6c5c33a74028634dd8f2e2f6962ab8657d2b74e57 |
| p6p8_run.log | 3d1193a6fa0e2baa687604c2368c785622682db78bd372341b3d46b476641c3a |
| p8b_run.log | edf29eab066bc5b9aa90a1442fb5eb60f7e341ed5ba1c08c41cbb30bc7949895 |
| build3.log | 4f39fe8934c9e2a5b84426e4eeec523ce322ed99dbe593c2415c7abecb0ba1fc |

Folders `t13_off/`, `t13_on/` (13-track captures), `runs/` (P6-P8 per-run logs), `arms/` (full arm set).

## 6. After L-101 (Mariano: "(a) flip default ON now, opt-out MASHED_NO_PORTSTATE_SEED=1, first consumer the 0x00498950 / 0x004989b0 swap")

**Verdict: GREEN.** Flip, swaps and merge leave the default gates where they were at every gated step.

### 6a. Commits and gates

Every gate set is `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` through `test-slot` + `mashed_lock.py run --label PORT-STATE-02`
on a frozen copy of that commit's `MASHED_NO_DEPLOY=1` build (`D:/MashedWT/PORT-STATE-02_data/exe_<tag>/`). Every
build: rva-lint `NEW=0`, asm-lint OK, seed `--check` OK.

| commit(s) | content | exe sha8 | G-CTRL | B |
|---|---|---|---|---|
| `257efb22a` | flip: `PortStateSeed_On()` = `MASHED_NO_PORTSTATE_SEED` unset. `MASHED_PORTSTATE_SEED` removed from code, header, generator doc and `run_arms.py` (stacks PSS / DEFHPSS retired, `NOPSS` = the opt-out). The verify drivers that ran the pre-flip arms (`run_p6p8.sh`, `run_p8b.sh`, `run_gates.sh`) and the PREREG keep the old spelling as the record of what ran | `3b3147b4` | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 |
| `a773cd75e` | 0x00498910 / 0x00498950 / 0x004989b0 to `re/match/s16/00498950.c` (later superseded by the merge, see 6b) | `64bb082a` | same | same |
| `92434b6a8`, `98f329780`, `6fa18d03c`, `23478abb4` | 0x004769d0, 0x004769f0, 0x00476ae0, 0x00476b30 out of `Render/RfxEmitter.cpp` into `Match/MatchS12_<rva>.cpp` (both targets) | `3eacdf8d` | same | same |
| `d6ca6ca5a`, `e0611e60a`, `78ef52ad9`, `c09d495da`, `c0806962b` | 0x0040e470, 0x0040e480 out of `Ai/CarSlotState.cpp` (the setter's standalone null guard goes: the seed makes `0x005f2770` non-null from boot), 0x0040ce80 out of `Util/PromoLoop_round49.cpp`, 0x0046dd80: the exe compiles `Match/MatchS11_0046dd80.cpp` and `GearConstGet` calls it; tracker backfill | `d5e59f26` | same | same |
| `dcf79ae80` | merge `orch/integration` @ `eee426759` (PORT-STATE-01 landed) | (built OK, gated with the next row) | | |
| `dbedbfcbe` | the seed consumer, see 6b | `70fa0d9e` | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 |

On `70fa0d9e` also: boot-to-menu NAV `5eb64a8c` and Quick Battle NAV `3998f57a` (both = the pre-flip traces), the
seed line `entries=100/100 ... mem=dd093ea5` with no env, videocfg present `present, no write`, absent
`first-run write ret=1`, file `7f130171` (= the hand body's). `MASHED_NO_PORTSTATE_SEED=1` alone: exit `0xC0000409`
at the first-run load (empty fopen mode). With `MASHED_VIDEOCFG_WRITE=0` as well: menu reached, exit 0. `NOPSS` in
`run_arms.py` carries both.

### 6b. Merge with integration and the consumer

`orch/integration` already held three of these swaps, done while the seed did not exist: PORT-SAVE-01 (`6136322d7`)
moved the whole config cluster (0x004963e0 / 0x00496400 / 0x00498910 / 0x00498950 / 0x004989b0) to its matched
sources (`Match/PortSave01_Config.cpp` + `Save/SettingsConfig.cpp`), PORT-FE-01 moved 0x0040e470, PORT-RENDER-01 moved
0x004769f0. The merge keeps integration's form for those three: my `Match/MatchS16_00498950.cpp` is deleted. Both
`MatchS02_0040e470.cpp` and `MatchS12_004769f0.cpp` take integration's text, and the duplicate rsp lines are dropped.
In the exe, integration bound the seeded values to private copies (`SettingsCfgGlobal` returned private strings for
0x005d013c / 0x005d0158 / 0x005cfe70 / 0x005cf010 / 0x005d00d8, and `g_rfxDefault00613290` for 0x00613290).
`dbedbfcbe` removes those: the matched bodies read the original addresses and the seed fills them. That makes the
config cluster and 0x004769f0 the seed's consumers in the default build. `RfxEmitter.cpp`'s copies for 0x00613294 /
0x006132a4 stay, because those addresses are not in the inventory.

### 6c. Port column, before (`304ac130b`) and after (`dbedbfcbe`)

| rva | name | port before | port after | exe body after |
|---|---|---|---|---|
| 0x00498910 | ConfigFilenameGet | called | called | `Match/PortSave01_Config.cpp` (integration), seeded strings |
| 0x00498950 | ConfigLoad | called | called | same |
| 0x004989b0 | ConfigSave | called | called | same |
| 0x0040e470 | CarSlotStateGet | called | called | `Match/MatchS02_0040e470.cpp` (integration) |
| 0x0040e480 | CarSlotStateSet | called | called | `Match/MatchS02_0040e480.cpp` |
| 0x0040ce80 | PtrTable5f2770Get | called | called | `Match/MatchS02_0040ce80.cpp` |
| 0x004769d0 | ParticleEmitter_SetVelocity | body | body | `Match/MatchS12_004769d0.cpp` |
| 0x004769f0 | ParticleEmitter_SetColour | called | called | `Match/MatchS12_004769f0.cpp` (integration), seeded 0x00613290 |
| 0x00476ae0 | Batch476ae0 | body | **called** | `Match/MatchS12_00476ae0.cpp` |
| 0x00476b30 | FUN_00476b30 | body | **called** | `Match/MatchS12_00476b30.cpp` |
| 0x0046dd80 | Float61313cGet | none | **called** | `Match/MatchS11_0046dd80.cpp` (new in the exe) |

`backfill_exe_file.py` and `backfill_port.py` on the final build: "rows whose old value changed: 0" and "rows
changed: 0" (the earlier changes are in `c0806962b` and the merge commit). The merge commit also takes the
backfill's port values for integration's own PORT-TOP rows (0x00495110 / 0x004950b0 / 0x00495120 / 0x0042b950 /
0x00433240 none -> called, 0x00402f80 body -> called), whose `hooks.csv` was older than its map.

### 6d. Dropped (not seed-only)

- **0x004325c0** (MenuAnimTickBody). The exe body runs on the port's `g_records` store and passed-in phase / gate
  values. The matched body reads 0x00898ac0, 0x0067eca4 and 0x0067e914 directly. That is the PORT-STATE-03 slice 2
  (`g_records` -> 0x00898ac0), not a data seed. It was not swapped and no hash was measured for it.

No row moved a hash. All game launches of this step ran under `mashed_lock.py run --label PORT-STATE-02` (inside
`test-slot`). Builds before the lead's 2026-11-08 lock rule did not check for a merge-train PRIORITY-HOLD / SOLO.
The experiment-E build and two rebuilds ran around 15:00-15:20 while the train held a SOLO window.

Tracker leg for the lead: the `hooks.csv` `file` column for 0x0040e480 / 0x0040ce80 / 0x004769d0 / 0x00476ae0 /
0x00476b30 / 0x0046dd80 still names the old TUs (re-classify, C4 unchanged).

Raw: `D:/MashedWT/PORT-STATE-02_data/arms_{flip,swap,rfx,slot,final}/` (`run.log` + arm files), `exe_{flip,swap,rfx,slot,final}/`.
