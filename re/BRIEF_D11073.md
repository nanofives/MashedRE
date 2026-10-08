# BRIEF: D-11073, port the race sub-state machine's pre-race phases (3 -> 4 -> 5 -> 6)

Written 2026-10-08 from static reads (Ghidra pool clone, capstone on `MASHED.exe.unpatched`) and
three read-only live polls of the original. Evidence index at the end. This is a **scope**, not a
pre-registration: each leg registers its own gates before it runs.

## 0. One paragraph

The port hardcodes `aib_game_sub_mode() { return 6; }` (`TrackRenderer.cpp:100`; handed to the AI as
`Ai::Host` at `:455`, plus one diagnostic column at `:4596`). The original passes through **three pre-race phases** before 6.
- **Phase 3:** a camera fly-in hold, **~652 frames** on Training, ended by a camera entry reaching
  its target.
- **Phase 4:** a one-frame transition.
- **Phase 5:** a held-car countdown, **~108-112 frames**, ended by a timer reaching 1.86 s.

The three symptoms the D-11073 row lists split by phase:
- `bias374` sawtooth (a) and the inert mode-5 branch (b) need **phase 5**.
- The CALLWISE anchor split (c) needs **all three**.

**Recommended order: leg 0 (measure, no code), leg 1 (phases 4+5), leg 2 (phase 3).** Leg 1 is
smaller, unblocks two of the three symptoms, and does not depend on leg 2.

## 1. The machine, as read

`FUN_004111c0` (C2) dispatches on `DAT_0063ba8c`. Live, Training, no injected input
(`o_cadence_nopress.csv`): `0 -> 2 -> 3 (630 samples) -> 4 (1) -> 5 (108) -> 6 -> 7`.

| state | handler (from `FUN_004111c0`) | what holds it | what releases it |
|---|---|---|---|
| 1 | spawn; writes `DAT_005f29b8 = 100000` and `DAT_0063ba8c = 2` | n/a | always |
| 2 | shared tail -> `3` if `DAT_005f29b8 == 100000`, else `4` with `DAT_005f29b8 = 12000` + `FUN_0040d470()`; forced `4` if `DAT_007f0fd0 == 6` | n/a | always |
| **3** | `FUN_004102f0` (172 B, C2) then `FUN_0040fc00` | flag `DAT_00897fe0` (set by `FUN_004430a0(1)` at `0x00410307`) | `FUN_00405460()==0 && FUN_004430b0()==0` -> `4`, `DAT_005f29b8 = 12000` |
| **4** | `FUN_0040dbd0` (123 B, C2) then `FUN_0040fc00` | none (1 frame) | always -> `5`; calls `FUN_0041d910` (zeroes the phase-5 timer at `0x0041d91a`) + a mode-dependent helper (`FUN_0041e080` / `FUN_0045b350` / `FUN_0041b520`) |
| **5** | `FUN_004103a0` (360 B, C2) then `FUN_0040fc00` | per frame: `FUN_00418860` + for each alive car `FUN_0046baa0` + `FUN_0046d7f0` (cars re-initialised = held) | `DAT_0063d588 >= _DAT_005ccdf4` (**1.86f**) -> `6`, `DAT_005f29b8 = 0`. `DAT_0063d588` += frame dt in `FUN_0041d930` (`FSTP` at `0x0041da76`) |

### Phase 3, fully mapped this session (`verify/d3_gatefire_20261008/RESULT_CADENCE.md`)

- **Per-frame path:** `FUN_0040fc00` -> `FUN_0040d470(0)` -> `FUN_00448220` -> `FUN_00446520(&DAT_00897fe0)`
  -> `0x004468f9` (sub-mode != 6) -> `FUN_004464c0` -> per entry by type.
- **Release:** `FUN_00445aa0`'s flag-set branch only (site (a), entry within **0.02f** of the
  target; site (b) is input). Measured: entry 0, all inputs 0, 625 / 628 frames over 2 runs.
- **Entries:** `FUN_00442600` (349 B, C2; once, guard `DAT_0089898c`).
  - Table by `FUN_00426c00()` track id: 13 tables at `0x005f7a48..0x005f9368`, all file-backed.
  - Count = `(int)table[0]` (`fld [esi]` at `0x00442695`, decomp hides it).
  - Rows of 0x30 B from `table+4` go through `FUN_00441b30` (329 B, C2).
  - **Training's 7 entries match only `0x005f8d50` (case `0x1e`, `[0]=7.0`)**. That is a match by
    count, not a measured track id; leg 0 confirms it.
  - Live types `0/1/1/1/2/2/3`. Only type 0 (`FUN_00445aa0`) can release. Types 1/2
    (`FUN_00441d40`, `FUN_00442440`) are visual only (neither writes the struct, checked).
- **`FUN_00405540`** (called by `FUN_00442600`) returns 0 when `DAT_00639d78 == 0`. That is the
  case live on 656/656 phase-3 samples, so **no camera-path clip code is needed**.
- **Target:** `FUN_00446520` copies `[0x00897fe0]+0x40..+0x48` -> `+4..+0xc` at `0x004481c9`
  (computed base, 0 static refs).

### What the standalone has today (`SURVEY_RACECAM.md`, spot-checked)

None of phase 3's release path:
- no `FUN_00442600`;
- no `FUN_00445aa0` body;
- no `0x004481c9` copy (`RaceCamera.h:106-107` merges `+0x40`/`[1..3]` into `pos_out_`);
- dispatch `Util/CameraEntryDispatch.cpp` is asi-only and calls the original;
- no offset-0 flag.

`RaceCamera.cpp` ports only FUN_00446520's sub-mode-6 pose math.

## 2. Legs

### Leg 0: measure and predict, NO CODE (memory `predict-a-ports-output-before-writing-it`)

1. **Survey (worker, read-only).** Port status and `.rsp` membership of every callee in §1's
   table: `FUN_0040dbd0`'s helpers, `FUN_004103a0`'s 16 callees (`FUN_0046baa0`, `FUN_0046d7f0`,
   `FUN_00418860` (ported), `FUN_0041da90`, `FUN_00426c90`, `FUN_0046c730/750/7b0`,
   `FUN_0046d780`, ...), `FUN_0041d910`/`FUN_0041d930`, and the port's `FUN_00426c00`
   equivalent. Also list every reader of `aib_game_sub_mode` and of `DAT_0063ba8c` in the exe
   build. Note `HUD/ScenarioLeaves_sa2.cpp:133-259` already models `DAT_0063d588`.
2. **Live, original.** One `orig_rampwatch.py --cam` capture. **MUST use `--statediff-out`**:
   without it `scenario_launch.py:3347` presses control 4 and phase 3 lasts 25 frames.
   Add columns `FUN_00426c00`'s global, `DAT_0063d588`, and `DAT_0089898c`. Predict, then check:
   - track id == `0x1e`;
   - `DAT_0063d588` ramps 0 -> 1.86 over phase 5 at `_DAT_007f100c` per frame;
   - entry 0's initial `+0x3c/+0x44` equals `FUN_00441b30` applied to row 0 of `0x005f8d50`
     (live: ~`(1.50, 34.5)` at the first phase-3 sample).
3. **Predict each phase's output from the port's current inputs.** Read every global the
   handlers read from a running standalone. If a faithful port would produce a value that
   disagrees with what the port does now, **stop and report** before writing code.

### Leg 1: phases 4 + 5 (the cheaper leg; unblocks symptoms (a), (b))

- A real `DAT_0063ba8c` in the port behind **default-OFF `MASHED_SUBSTATE`**:
  `aib_game_sub_mode` returns it only under the knob. Entry at state 4 (skip 3 in this leg).
- Port `FUN_0040dbd0` (123 B) and `FUN_004103a0` (360 B); their unported callees per leg 0.
- **Gates to register:**
  - phase 5 length vs the original (108 samples / ~112 frames);
  - `bias374` sawtooths (`RESULT_RAMP.md`'s original shape);
  - the mode-5 branch of `FUN_00418560` becomes non-inert;
  - knob OFF is byte-identical to the current controls (step `c741a4c5`, gates `642af0ac`,
    ptrace `05984e30`).

### Leg 2: phase 3 (the hold)

- Port: the seed (`FUN_004111c0` case 1 already writes it; check the port's equivalent),
  `FUN_004102f0`, the flag (`FUN_004430a0`/`FUN_004430b0` are C3 in `PromoLoop_round8.cpp`,
  asi-only), `FUN_00442600` + `FUN_00441b30` + the 13 tables (or the Training one first, named
  as partial), `FUN_004464c0` type-0 arm, `FUN_00445aa0` (2,579 B), the `0x004481c9` copy (split
  `pos_out_`), the sub-mode != 6 path of `FUN_00446520`, an offset-0 flag on `RaceCamera`.
- **Gates to register:**
  - hold length vs 625/628 frames;
  - entry 0's approach curve vs `o_cadence_nopress.csv` (29.4 -> 0.02);
  - release attributed to site (a);
  - knob OFF byte-identical.
- **Defer:** types 1/2 arms (visual parity only); the camera-path clip module (idle live).

### Promotion leg (memory `fix-briefs-carry-a-promotion-leg`)

Every function ported here is written as **one body at its RVA**: RVA comment,
`RH_ScopedInstall`, runtime-toggleable, in a TU listed in **both** `exe_sources.rsp` and
`asi_sources.rsp`, with `rva-lint` NEW=0. Each gets a `hooks_registry.py` entry and
`run_diff.py` + `run_verify_hook.py` in the same session. Non-repeatable bodies (`FUN_00442600`
has a once-guard) use `re/CONFIDENCE.md`'s non-repeatable clause. C3 max from this lane.
Current levels: `004111c0` C2, `004102f0` C2, `0040dbd0` C2, `004103a0` C2, `0040fc00` C2,
`0040d470` C2, `00448220` C2, `00446520` C2, `004464c0` C3 (asi call-through), `00445aa0` C2,
`00442600` C2, `00448700` C2, `004430a0`/`004430b0` C3, `00441c80` C2.

## 3. Hazards, named

- **Every timeline moves.** Turning the knob on inserts ~760 pre-race frames (652 + 1 + ~110).
  Every capture, scorer window and frame-anchored constant (`(b)`/`(e)` windows, branch 2's
  643-660, `bias374` band frames) shifts. Register that the knob stays default-OFF until the
  scorers are re-anchored on sub-state 6 entry, not frame 0.
- **Held cars.** Phases 3 and 5 call `FUN_0046baa0` per car per frame (a ~70-field re-init).
  The port's physics must not integrate during the hold, or cars drift before the start.
- **`aib_game_sub_mode`'s consumers are uncounted.** It reaches the AI only through
  `Ai::Host` (`TrackRenderer.cpp:455`), so every `s_host.game_sub_mode` call site is a reader,
  plus whatever reads `DAT_0063ba8c` directly. Leg 0 counts them before anything flips.
- **Harness:** `--statediff-out` for any original-side phase-3/5 measurement. `MASHED_GF1`
  mutates `TimerAt`/`RankAt` (off for scoring). Default controls: step `c741a4c5`, gates
  `642af0ac`, ptrace `05984e30`.
- **`DAT_005f29b8` image value is `0xff`**, not 100000. The seed must come from the case-1 write,
  not the image.

## 4. Kickoff prompt (paste into a fresh session)

> Mashed RE, D-11073 port, LEG 0 only. Read `re/BRIEF_D11073.md` §0-§3, then
> `verify/d3_gatefire_20261008/RESULT_CADENCE.md` §0 and §6. No code this session.
> (1) Send §2 leg-0 item 1 to the account2 worker via delegate.ps1 -Repo Mashed.
> (2) Pre-register, then run one `orig_rampwatch.py --cam` capture with `--statediff-out` plus the
> three new columns; check the three predictions.
> (3) Read the port's live inputs to `FUN_0040dbd0` / `FUN_004103a0` and predict their outputs.
> Report whether leg 1 can start as scoped, or what blocks it. Do not write a port body.

## Evidence

`verify/d3_gatefire_20261008/`: `RESULT_CADENCE.md`, `PREREG_CADENCE.md`, `SURVEY_RACECAM.md`,
`SCOPE_SUBSTATE2.md`, `RESULT_RAMP.md`, `RESULT_M5.md`, `RESULT_CALLWISE2.md`, `RESULT_H4.md`;
captures `o_cadence*.csv`. Tracker row: `DEFERRED.md` D-11073.
