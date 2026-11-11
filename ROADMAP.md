# Mashed RE Roadmap v5 (2026-11-11)

Supersedes v4 (2026-11-07), archived verbatim at
[`re/analysis/archive/ROADMAP_v4_2026-11-07.md`](re/analysis/archive/ROADMAP_v4_2026-11-07.md) (a `git mv`, no edit).
v3 stays at [`re/analysis/archive/ROADMAP_v3_2026-08-15.md`](re/analysis/archive/ROADMAP_v3_2026-08-15.md), v2 at
[`re/analysis/archive/ROADMAP_v2_2026-06-09.md`](re/analysis/archive/ROADMAP_v2_2026-06-09.md).

> **Old citations.** A citation of `ROADMAP.md` phases M1..M5 or of the v4 "Port axis: phases D4 and D5" tables,
> written between 2026-11-07 and the landing of v5, points at the **v4 archive**. Citations from before 2026-11-07
> keep pointing at the v3 archive, as v4 said.

Source: the strategy review of 2026-11-11 (session `cmwbssxl22bg4sc1ba3ghf7dh`) and decisions L-96, L-111, L-120 to
L-127 in `log/orch/DECISIONS.md`. L-127 (Mariano, 2026-11-11) landed this file as written, with its freeze list and
the standing classes P1 and P2. Ledger rows: `DDKOBJ-*`, `RO-*`, `RW-SIZING`, `RWC-*`, `EQUIV-*`,
`PROVISIONAL-BODIES`, `PORTSTATE-IMAGE`, `ROADMAP-V5`.

**Goal (Mariano, 2026-11-11): a fully implemented game with most of its functions matched.**

---

## Why v5 exists

v4 split the work into an RE axis (C0..C4) and a port axis. Its port axis was still the hand-written exe: swap rows,
dual-copy burn-downs, D3 carried rows. One day later L-96 started replacing the exe's top level with the original's,
and L-111 made that the bulk route (`MASHED_ORIG_TOP`). v4 never mentions it. v5 makes the top-down route the port
axis, gives it milestones and its own way of being verified, and freezes the hand path it replaces.

What the review measured:

| Finding | Value | Source |
|---|---|---|
| A byte-matched source recompiled by VS2022 does not compute the original's values | `0x00443300`: differs on 14,039 of 20,000 inputs at x87 PC_24; the DDK object differs on 0 | `verify/ddkobj_20261111/RESULT_DDKOBJ-SPIKE.md` section 8 |
| The DDK objects link into the exe | 540 of the 583 matched sources the exe includes; 541 of 552 linked functions are byte-identical to `MASHED.exe.unpatched` in the image; boots, 3,000 frames on the top-down path, log and 12-capture walk equal to the VS2022 build | same, sections 3 and 5 |
| The original replays itself exactly under a tick clock | 6 of 6 run pairs identical on 2,724 of 2,724 race ticks; a one-word RNG change is caught at tick 7 | `verify/replayoracle_20261111/RESULT_REPLAY-ORACLE-DESIGN.md`, `RESULT_RO-WARP-NORMALIZE.md` |
| The default gates do not measure the top-down path | G-CTRL and B run with the knob off | `re/orchestrator/merge_train.json`, L-106 |
| The RenderWare boundary has no fixed end on librw | 180 matched game functions read RW state inline, 92 RW-band functions filed as game code run RW code on librw objects, 31 of 81 bindings are hand bodies | `verify/rwsizing_20261111/RESULT_RW-SIZING.md` |
| Nobody plays the game | no track x mode playthrough matrix; last recorded playtest 2026-09-06, six defects | `re/analysis/playtest_feedback_20260906.md` |

---

## Baseline (2026-11-11)

| Measure | Value | Source |
|---|---|---|
| `hooks.csv` rows | 6,178: C4 4,244, C3 149, C2 1,772, C1 13 | `hooks.csv` at `ee3562e27` |
| Game rows (subsystem label is not `third-party-library[...]`) | 3,391, of which C4 2,614 | same, counted by label |
| `port` column | called 528, body 554, none 5,096. Game rows called: 507 | same |
| Match ledger | 4,085 rows, 2,979 PASS. Rows scoring 100: 334,968 B. Rows below 100: 715,883 B | `re/analysis/match_ledger.tsv` |
| Top-down reach from WinMain `0x00492370` | 2,316 functions, 1,653 C4 by match | `verify/portorigtop_20261109/SIZE.tsv` |
| Top-down path today | 215 matched bodies running, 1,065 runnable; boots and walks the 12 front-end screens; no race | `SIZE.tsv`, `RESULT_PORT-ORIGTOP-SIZE.md` |
| Object build (`MASHED_DDKOBJ=1`) | 540 units linked, 43 refused (38 static register convention, 5 wrappers that re-bind globals) | `RESULT_DDKOBJ-SPIKE.md` |
| RenderWare the original executes (RW-Physics excluded) | 668 functions, 446 C4 by match | `RESULT_RW-SIZING.md` |
| Default gate refs | read `re/orchestrator/merge_train.json` (`gctrl.ref`, `b.ref`). The file wins over any line here | merge train |

Recount these from the data when you quote them.

---

## The target

What the finished `mashed_re.exe` is:

1. **Game code is the matched source.** Every function the original executes runs from its `re/match` source where
   one exists (C4 by match). Where the source is not byte-identical, three cases (rule: L-126):
   - **admitted:** `re/tools/match/equiv.py` proves the compiled body equivalent to the original (same calls, same
     memory, same registers at return): verdict EQUIV in default mode, or EQUIV-IF-CALLERS-IGNORE with every caller
     scanned as ignoring, the exposed-frame rule included. No per-function decision. Recorded in `ADMITTED.tsv`.
     It counts on the port axis. Admitted is not C4 by that status alone (L-128): it becomes C4 only through the
     replay form of the frida route (`re/CONFIDENCE.md`, `c4=frida-replay`). x87 instruction order stays excluded.
   - **provisional:** the tool cannot prove it. The source may still run on the knob path in place of a hand
     stand-in so the path reaches the oracle. It is listed in `PROVISIONAL.tsv`, and it never counts toward the
     port DoD. About half of the unproven sources sampled have a real difference (wrong argument, register argument,
     call order), so a provisional body is expected to be wrong until the oracle or source work says otherwise.
     Provisional is not C4 by that status alone (L-128): a body that replays the reference identically with its JMP
     live in the original meets the replay form of the frida route and takes `c4=frida-replay`.
   - **stand-in:** no source yet. Hand code bound at the RVA, as today.
2. **Two builds from one tree.**
   - The **default build**: VS2022 compiles everything. It needs no old compiler. It is the fallback and what a
     contributor without `tools/vc7ddk` builds.
   - The **object build** (`MASHED_DDKOBJ=1`): the same tree, with the XP DDK object of each matched unit linked in
     place of its VS2022 compile. `re/tools/ddkobj/ddkobj.py check` compares every substituted function in the linked
     image with the original's bytes and lists the IDENTICAL ones. A unit the tool refuses keeps its VS2022 body.
   - Role of the object build (L-125): it is the build the replay oracle measures and the top-down path is
     developed against. Which build ships is decided at the flip (T4).
3. **Original data at original addresses.** The image pad `0x00401000..0x009fffff` holds the original's globals.
   File-backed `.rdata` / `.data` values come from the per-address seed today and from a whole image staged out of
   `MASHED.exe.unpatched` once `PORTSTATE-IMAGE` lands (L-121).
4. **Platform boundary, declared.** CRT (the exe's own, with the DDK math bodies for DDK-object callers: L-120,
   `DDKOBJ-CRTMATH`), Direct3D 9, DirectSound / DirectShow, the OS.
5. **RenderWare is game-side code (L-124).** The top-down path runs the matched RW core on d3d9, behind an in-exe
   device wrapper that carries the d3d9 shim's behaviours. RW functions follow the same rules as item 1. No new
   librw binding is added on the top-down path. The bindings written so far stay in the tree as the fallback until
   the first menu frame renders on the RW core (`RWC-CORE-RUN`). If the device layer cannot be brought up, the
   fallback to bindings is a decision row. RW-Physics is ported like game code under every route (L-98).
6. **One path.** The hand-written game layer is deleted at the flip. Until then it is the default path and what the
   LAN release ships.

---

## The default-build rule (kept)

> **A port capability counts only if it runs in `mashed_re.exe` with no `MASHED_*` variable set.**

Unchanged from v3 and v4, with one clarification. The top-down path lives behind `MASHED_ORIG_TOP` until T4, so by
this rule nothing on it is landed before T4. That is intended: T0 to T3 are measured on the knob path, and T4 is the
one decided flip that lands them together. A behaviour change on the default path still ships as a default-OFF knob
plus a committed PREREG, and its flip is a decision row.

---

## Definition of Done

### RE axis (unchanged from v4)

- **F-DoD (RE).** The row is C4 under `re/CONFIDENCE.md`: byte match with relocheck OK, library identity, or the
  route that file lists for code that cannot match.
- **S-DoD (RE)** and **P-DoD (RE)** as in v4.
- One addition: a C4-by-match row must also pass the exact-data-operand compare once `MATCH-EXACT-DATA` lands
  (`0x00414060` scores 100% in `match.py` with its `fld` / `fmul` operands swapped).

### Port axis (the project DoD), read on the top-down path

- **F-DoD (port).** `port = called` on the top-down path, and the body is one of:
  1. a matched source that the object build links and `ddkobj.py check` reports IDENTICAL
  2. a matched source the object build refuses, running as its VS2022 compile, listed with the refusal reason
  3. an admitted body: `equiv.py` verdict recorded in `ADMITTED.tsv` under the L-126 rule
  4. a declared platform binding (the families in "The target", item 4; a librw binding counts only while the
     fallback of item 5 is in use)

  A provisional body and a stand-in do not meet F-DoD (port).
- **S-DoD (port).** Every function of the subsystem that the original executes (`re/coverage/reach.tsv`) meets
  F-DoD (port). Functions the original never executes are dispatched in `DEFERRED.md`. Structs documented. Formats
  the game writes round-trip. No stand-in left in `BINDINGS.tsv` for the subsystem.
- **P-DoD (port).**
  1. Every subsystem S-DONE (port).
  2. The replay oracle reports IDENTICAL-FOR-N on the scenario matrix (T3), on the build L-125 names.
  3. The soak matrix completes: every track in every mode, unattended, no crash, results screen reached.
  4. The flip is done and the hand path is deleted (T4). `re/tools/dual_copy_allowlist.txt` is empty.
  5. The D5 shipping list is met on a clean machine.
  6. Scope kept from v4: CRT, libpng, zlib, Lua and qhull are outside the port count (L-67, L-70, L-97).
     Local multiplayer (`DEFERRED.md` D-11063) is re-examined at T3, because the original's multi-player code runs
     on the top-down path. It stays out of v1.0 unless Mariano decides otherwise then.
- **D-Gate.** From T4 on, every port demo runs on a clean environment: no `MASHED_*`, no manual step.

No port-axis line reads the C-level, and no RE-axis line reads the `port` column.

---

## Closed phases (history)

D0, D1, D2, D3 as listed in v4 (detail in the v3 archive). v4's RE phases M1 (rubric and port column), M2 (batch
promotion) and M4 (library identity) are done: C4 went from 254 on 2026-11-07 to 4,244.

---

## Port axis: milestones T0 to T4, then D5

### T0, foundations (active)

| Item | Rows | State |
|---|---|---|
| Object build exists and is byte-checked | `DDKOBJ-SPIKE` | done |
| Role of the object build | `DDKOBJ-ROLE` | done: L-125 = (a), the replay target |
| Object build runs in the merge train as a reported step | `DDKOBJ-TRAIN` | open |
| Refused units recovered | `DDKOBJ-REGCONV` (38), `DDKOBJ-CELLS` (5) | open |
| DDK math bodies for DDK-object callers | `DDKOBJ-CRTMATH` (L-120) | open; `BIND-CRT-MATH` held |
| Replay oracle, original side | `REPLAY-ORACLE-DESIGN`, `RO-WARP-NORMALIZE`, `RO-STACKROW-READERS`, `RO-AUDIO-ISOLATION` | done |
| RenderWare route | `RW-SIZING`, `RWC-ROUTE` | done: L-124 = (C) with fallback |
| Admission rule for unmatched bodies | `EQUIV-CHECK` (168 of 1,106 near-match rows proven, 39 of the 326 functions blocking the path), `EQUIV-RULE` | done: L-126 |
| First admissions and the provisional convention | `EQUIV-ADMIT-1` (33 blocking functions), `PROVISIONAL-BODIES` | open |
| Free C4s the checker found | `MATCH-LEDGER-REFRESH` (26 rows byte-identical today), `EQUIV-FU-48E820` | open |
| Stricter match compare | `MATCH-EXACT-DATA` | open |
| Whole data image | `PORTSTATE-IMAGE` (L-121) | open, design first |
| This file, the freeze, the standing classes | `ROADMAP-V5` | landed: L-127 |
| Conductor skill and ledger fixes | `HB-V5-SKILL` | open |

**Exit:** `ddkobj.py check` reported on every train pass, `ADMITTED.tsv` and `PROVISIONAL.tsv` exist with their first
rows, the refused units are recovered or each carries its reason.

### T1, the menus run the original's code

Today the top-down path walks the 12 front-end screens with 59 stand-in and 32 under-stand-in rows in
`BINDINGS.tsv`. T1 removes the stand-ins on the menu path and moves the menus onto the RW core.

Rows: `HUDINIT-A`, `HUDINIT-B`, `HUDINIT-C`, `NEARMATCH-555360`, `PORT-TOP-08`, `PORT-TOP-04K`,
`PORT-OT-SEED-CODEPTR`, `BIND-CRT-STDIO`, `BIND-CRT-STRING`, `BIND-D3D-REST`, `MATCH-RT2D-FONT`, and for the RW core
(L-124): `RWC-DEVICE`, `RWC-CORE-RUN`, `RWC-DRIVER`, `RWC-CENSUS`, then `RWC-MATCH`.

L-124 supersedes `PORT-BIND-RWCORE-2`, `PORT-OT-BIND-41DEF0` and `PORT-OT-SCREENSHOT` (new librw bindings). Their
lead re-reads `RASTER-555FF0`, `PORT-UNBLOCK-RWGLOBALS` and the HUDINIT chain against the RW core before staffing
them: what they bind on librw objects, the RW core provides itself.

**Exit:** no stand-in on the menu path; the menus render through the RW core; the nav walk matches the original's
draw lists under the parity harness (`re/analysis/parity_tooling.md`), or every remaining row is cited.

### T2, the first race, compared tick by tick

The race can be entered without the menus (track 0, mode 10, 1 player + 3 AI). What it needs
(`RESULT_PORT-ORIGTOP-SIZE.md`, "The race point"): the run-loop pieces `0x004929d0`, `0x00492d30`, `0x00492e90` off
their stand-ins; `0x004111c0` (13,730 B, C2) running; about 170 C0..C3 race functions (106 for race load, 64 per tick);
25 RW-Physics functions; the track loader chain `0x00426e10` (L-113); Lua for the powerup and course scripts (L-114).

Rows: `PORT-OT-RACE`, `PORT-TOP-05`, `PORT-TOP-06`, `PORT-TOP-07B`, `TL-PORT`, `TL-CALLER`, `TL-MATCH-NEAR`,
`LUA-PORT`, `PORT-BIND-LUA`, `PORT-RWP-02`, `PORT-RWP-WIRE`, `PORT-OT-GAMEFLOW-41A980`, `PORT-UNBLOCK-THRESHOLD`,
and the oracle's exe side: `RO-BIND-TIMER`, `RO-MASKS`, `RO-ORIG-ASI`, `RO-EXE-REPLAY-KNOB`, `RO-EXE-TICK-TRACE`,
`RO-FIRST-DIVERGENCE`, `RO-ORACLE-GATE`. `RO-TOLERANCE` is not needed while L-125 = (a) holds.

Matching work for T2 is ranked by what it releases (`origtop_size.py`, "added if bound alone"), race path first.
The equivalence tool releases 33 of the 326 blocking functions with no per-function decision (`EQUIV-ADMIT-1`). The
other 293 need source work, or run as provisional bodies (`PROVISIONAL-BODIES`). The oracle then names the first
tick and address where a provisional body goes wrong, which is also the fastest way to find what its source gets
wrong.

**Exit:** `RO-ORACLE-GATE` on the menu-less race reports IDENTICAL-FOR-N for the 2,724 ticks the reference covers, or
every divergence is filed with its first tick, address and cause.

### T3, breadth

Every track, mode and vehicle on the top-down path.

- **Oracle scenario matrix.** One recorded reference per track and per mode family, entered through the menus once T1
  holds. Rows to file at T2's exit.
- **Soak matrix** (`SOAK-DRIVER`). An unattended driver that finishes races (candidate: the game's own AI on the
  player slot; `RO-INPUT-RECORD` gives recorded human input as the alternative). Every track in every mode, results
  screen reached.
  This also unblocks what `D4-SAVE-U3560` waited on (a natural TimeTrial lap).
- **Flows the hand path never had or got wrong:** pause overlay, unlock progression, cup and championship flow,
  TimeTrial ghost save and reload, points, per-car colours (playtest 2026-09-06). On the top-down path these are the
  original's code, so each is a check, not a port.
- **Audio.** The default path ships the AudioEngine stand-in. 380 of the 587 audio rows are C4. The top-down path
  runs the original's audio code down to DirectSound. Rows to file.
- **Save and config.** The matched save and config code writes `gamesave.bin`, `videocfg.bin`, `contcfg%d.bin`.
  Round-trip checks.
- **Local multiplayer.** See P-DoD item 6.

**Exit:** P-DoD (port) items 1 to 3.

### T4, flip and delete

`PORT-TOP-FLIP` as a decided flip. Its evidence is the oracle and the soak matrix, not G-CTRL / B equality.

1. Mariano decides the flip and which build ships (default VS2022, or the object build).
2. The top-down path becomes the default. The hand top level stays behind an opt-out knob for one release.
3. The gates change: G-CTRL, B, the D2 arms and the 13-track table retire with the hand path. The train's gates
   become the object-build byte check, the oracle gate and the menu parity walk. `merge_train.json` is rewritten in
   the flip's own pass.
4. The hand path is deleted: the ORIGTOP-ONLY allowlist class, the hand course loader (L-113), the hand vehicle
   model, AI, race and front-end layers, the librw default renderer and the librw bindings (L-124, unless the
   fallback was taken), and every knob that selected between them.

**Exit:** the default-build rule holds on the top-down path; `dual_copy_allowlist.txt` is empty.

### D5, v1.0 ship

v4's shipping list, unchanged in content: `D5-SHIM` and `D5-SHIM-STEP3-REMOVE` (no dev d3d9 shim, no ASI loader, no
boot patch), `D5-DEPS` (clean machine), `D5-KNOBS` (final pass). They were blocked on `D4-SDONE`. They are now
blocked on T4. A LAN release follows, with a release note that states how many functions in the shipped exe are the
original's bytes (from `BYTECHECK`), how many are admitted bodies and how many are platform bindings.

---

## RE axis: what is left

The ladder and its rule do not change. What changes is the order: matching work is ranked by the top-down path.

| Item | Rows | Note |
|---|---|---|
| Unmatched functions the original executes on the race path | T2 list, `TL-MATCH-NEAR`, `MATCH-RT2D-FONT`, `MATCH-S14-FU` | ranked by rows released |
| Near-miss pool | `MATCH-X87-CASTLEVER`, `MATCH-REALLIT-SCREEN`, `MATCH-SIG-SWEEP` | 173 rows at 95% or more |
| Stricter compare | `MATCH-EXACT-DATA` | re-check C4 rows it refuses, through re-classify |
| Rows the ledger undercounts | `MATCH-LEDGER-REFRESH` | 26 REFUSED rows are byte-identical today |
| Retail compiler | `MATCH-9466`, `C4R-CL9466` (blocked) | 442 Lua / qhull / CRT rows are filed as the 9176 to 9466 difference. No torrents. A second-hand retail VS .NET 2002 Professional is the only legitimate source known |
| RenderWare core and its D3D9 driver (L-124) | `RWC-DRIVER`, `RWC-MATCH`, `RWC-CENSUS` | 222 unmatched executed functions, 131,971 B; the device layer is 48 functions, 23,030 B unmatched |
| Unattended match driver | to file | select, convert, match, permute over a list of RVAs with no session per slice |
| Function discovery | `HB-4` (1,788 race-closure RVAs), `MATCH-PCFUNCS-REGEN` | 229 RW-band functions have no census row |

**RE axis target:** P-DoD (RE), as in v4.

---

## How a row is verified

| Row kind | The worker runs | The merge train runs |
|---|---|---|
| Top-down row (touches only PortTop wrappers, bindings, `re/match` sources or other files the default path does not execute) | both builds, `ddkobj.py check`, the knob boot, its self-test, and the menu walk when menu code changed | the default gates once per batch, as today, to protect the default path until T4 |
| Race-path row, from T2 on | the above plus the oracle on the menu-less race | the oracle gate |
| Default-path row | as today: default gates unchanged, PREREG for a behaviour change | as today |
| Match-only, tracker and text rows | no build, no gates | text-only pass |

A top-down row no longer runs G-CTRL, B or the 12 captures x 4 in the worker: those hashes are computed with the knob
off and cannot see its change. The train still runs them per batch.

---

## Frozen until T4 (hand path)

Rule: no new work on code that T4 deletes. Applied to the ledger on 2026-11-11 (L-127): 37 rows set to `blocked`
with the note "FROZEN by ROADMAP v5", each keeping its previous stage and note. None is deleted. Mariano can
unfreeze any of them by name, for example to fix a defect in a LAN release.

- **Swap lanes into hand code:** `PORT-UTIL-01`, `PORT-GAMEPLAY-01`, `PORT-VEH-01`, `PORT-AI-01`, `PORT-FRONT-FE-01`,
  `PORT-FRONT-RENDER-01`, `PORT-FRONT-SIM-01`, `MATCH-S09-FU2`, `MATCH-S11-FU-VEHBASE`, `MATCH-S12-FU-RFXDEF`,
  `MATCH-S19-FU-MATHSAFE`, `MATCH-S19-FU-RASTER-SELFTEST`, `MATCH-RECORDS-SWAP-46DBE0`.
- **Dual-copy burn-down on the hand path:** `D4-DC-77b0`, the seven `D4-DC-INC-*` rows, `D4-DC-4d20-GRID-FLIP`,
  `D3-CF-D11079`, `PORT-AITICK-ONEBODY`.
- **Hand-path behaviour:** `D3-CF-MODEXIT`, `D3-CF-D11078`, `D3-CF-R7INPUT`, `D4-ST5LEG2-MODE7`, `D4-FLIP-ST5-LEG2`,
  `D4-UNC-PHYS-3`, `D4-SAVE-U3560`, `MATCH-PAUSE-OVERLAY`, `PORT-COLOURSEL-LAUNCHSTORES`, `HB-COLOURSEL-DRAWORDER`.
- **Hand-path state at original addresses:** `PORT-STATE-04`, `PORT-STATE-05`.
- **`D4-SDONE`** is replaced by T3's exit.

Where the top-down path needs the same subject (mode exits, the ghost block, the pause overlay), it is re-filed as a
T3 check on the original's code.

The LAN release keeps shipping the default path until T4. It gets no new features in that time.

---

## Decisions

Answered on 2026-11-11 (Mariano), full text in `log/orch/DECISIONS.md`:

| Id | Question | Answer |
|---|---|---|
| L-120 | Reopen L-119 (CRT math on the top-down path) | reopened toward the DDK libc math bodies; final form with `DDKOBJ-CRTMATH` |
| L-121 | Reopen L-99 (per-address data seed) | reopened toward a whole image from a staged data file; final form with `PORTSTATE-IMAGE` |
| L-122 | Reopen L-111 matched-only and the per-function admissions | reopened; final form is L-126 |
| L-123 | Reopen L-98 (RW core on librw) | reopened; final form is L-124 |
| L-124 | RenderWare route on the top-down path | (C) matched RW core on d3d9, librw bindings kept until the first menu frame renders on it |
| L-125 | Role of the object build | (a) the build the oracle measures and the path is developed against |
| L-126 | Admission rule for unmatched bodies | admit EQUIV and EQUIV-IF-CALLERS-IGNORE where every caller ignores, exposed-frame rule adopted, x87 excluded, `--relax-icall` a count only; provisional bodies allowed on the knob path |
| L-127 | This file, the freeze list, the standing classes | landed as written; P1 and P2 adopted |
| L-128 | Does a rough source that replays the reference race identically inside the original, JMP live, count as C4 by the frida route? | yes ("promote it"): replay form of the frida route, tag `c4=frida-replay`; install check, body executed, no STUB and the Write-set leg stay; admitted and provisional (L-126) are not C4 by that status alone; byte-matched and replay-verified counts reported apart; rubric text landed by `C4-REPLAY-RULE` |

Still to come back as decision rows, each with its evidence: the final form of L-120 (after `DDKOBJ-CRTMATH`), the
final form of L-121 (after the `PORTSTATE-IMAGE` design), a fallback from the RW core to bindings if `RWC-CORE-RUN`
reports one is needed, the T4 flip and the choice of shipping build, and the v1.0 scope of local multiplayer at T3.

**Standing classes** (L-127). Every use is logged in `DECISIONS.md` as "auto, class Pn" with its evidence path, and
Mariano can reverse any of them:

- **P1.** A knob flip on the knob path whose captures and logs are unchanged ON against OFF, on OFF / ON / OFF / ON
  passes, lands without a decision row. Default-path flips and anything that moves a gate hash stay decisions.
- **P2.** A refile of a function to another subsystem on identity evidence (the L-116 pattern).
- Tool-decided admissions are not a class of their own: they are the L-126 rule.

Between 2026-11-07 and 2026-11-11 there were 27 decision rows (L-93 to L-119). Most were flips with unchanged gates
where the recommendation was taken. P1 and L-126 are meant to remove that kind of row.

---

## How the work runs

- **Ledger:** `log/orch/roadmap.json`, written only through `re/orchestrator/roadmap.ps1`. Worker rules:
  `re/orchestrator/WORKER_RULES.md`. Conductor: `.claude/skills/roadmap-loop/SKILL.md`. Decisions:
  `log/orch/DECISIONS.md`. Briefs for the review rows: `log/orch/briefs/`.
- **Gate refs:** `re/orchestrator/merge_train.json` is the only source. A hash-changing change is a decision row.
- **Trackers:** only through `re-classify`. Session kickoff: `re/NEXT_SESSION.md`. Harness work:
  `re/HARNESS_BACKLOG.md`.
- **Process fixes:** `WORKER_RULES.md` carries the "How a row is verified" table, the object-build step, the L-126
  rule and the standing classes since this file landed. Row `HB-V5-SKILL` holds the rest: the conductor skill takes
  the same rules and the right test-slot count (it says 6, the workspace rule is 8), a run tag on ledger rows (rows
  per run cannot be counted today), `INTEGRATION_LOG.md` records RED passes too, and the six rows stuck on a merge
  conflict since run 29 are re-merged or closed.
- **Process rules carried from v3 and v4:** branch teardown with `git branch -d`, worktrees removed only with
  `py -3.12 scripts/diag.py wt-remove`, cited evidence is committed as summaries, append-only trackers are never
  overwritten.

## What v5 does not change

The C4 rule (L-93), the NO-GUESSING rule, the tracker discipline, the default-build rule, the LAN release rule, the
MASHED process hygiene rules, and the version anchor.
