# U-9141 / U-9145 — the PLAYER-car physics regression since D2 closed

Session 2026-09-29, branch `race/first-frame-parity`, starting from `64095a44`.

**This section (§1 and §2) is written and committed BEFORE the first run of this session.**
Nothing in it changes after the first number lands. §3 onward is filled in afterwards.

---

## 1. The inherited facts (not re-derived here)

From `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` (pre-registration `1d0ca916`, results
`4938adba`) and `re/analysis/D3_DRIVE_FORCE_2026-09-29.md` (`9573f3a3..64095a44`):

- **The controlled D2 arm** `C-A8-1` = the `a8` held-full-lock recipe
  (`re/tools/statediff/a8_run_port.py`) + `MASHED_MEASURE_NOOPP=1` + `--max-lines 1080` on
  both reducers. It is **deterministic**: run-to-run spread on `slip 1500-2000` is **exactly
  0** on 3/3 runs at both ends (note §3.3). One run per commit is therefore decisive.
- On that arm, **`56ad3806` (the D2 close) gives `0.1916 / 0.2669 / 1932.09`** and the
  ORIGINAL's archived capture gives `0.1913 / 0.2498 / 1940.59`. Two of the three plus `av.y`
  reproduce the ROADMAP §D2 row to four decimals.
- **HEAD gives `0.1609 / 0.2296 / 691.01`** — `-16.0% / -14.0% / -64.2%`. Criterion C2 of the
  pre-registered rule FAILED, and the failure is a real HEAD-vs-`56ad3806` difference in the
  **player's own trace** that the uncontrolled recipe was masking.
- The 2026-09-28 conclusion *"no commit in `56ad3806..HEAD` edits the player's solver,
  therefore the drift is the instrument"* does **not** survive a controlled instrument. The
  bisect has to be redone on the controlled arm. That is task 1 here.
- **U-9145 (U-D2-OPPONENT-COUPLING)**: with the opponents present, the same HEAD build gives
  the player a driving-median of **2538** instead of 691 (3.7x), even though
  `VehicleCarCarContact` (`0x00469df0`, `Collision/CarCarContacts.cpp`) has **zero callers**
  anywhere in the port. The coupling is shared mutable state and is unidentified. That is
  task 3 here.

Both directions of the regression matter and they are not the same question: 691 is the
player alone at HEAD, 2538 is the player at HEAD with three opponents, 1932 is the player
alone at `56ad3806`, and 1940.59 is the ORIGINAL (which has three opponents).

## 1.1 The commits in range

`git log --oneline 56ad3806..HEAD -- mashedmod/` = **24** commits (60 in total; 36 touch no
build input). Newest first:

```
1d0ca916 U-9141 step 0: the D2 controlled arm and its PASS RULE          <- the knob itself
d165e6b4 re-classify: U-D3-DRIVE-FORCE
9573f3a3 U-D3-DRIVE-FORCE: the missing A6a start boost                   <- + Fi_GameMode 0->6
b5923d20 U-D3-DRIVE: criterion (e) FAILS, and the settle costs AI (b)
83a7b6ea U-D3-DRIVE: the spawn settle                                    <- candidate
a9da810a standalone: remove the placeholder free-fly camera; MASHED_TITLE
2e7a2b92 D3 powerups: port the dispatcher's BOX-STATE gate
079e2106 U-9138 re-check: PASS
c5bcf71c U-9138 FIXED: Vehicle::Rw_TransformPoints binds to the VECTORS form  <- candidate
e2dce8f1 D3 contact (c): MISSILE ported
bd69b3d8 D3 contact (c): MORTAR's own chain
7f6b44d3 D3 contact (c): port the shared target acquisition
f2afeb8b D3 contact (c): R_FLAME ported
5bb0d5e3 D3 contact (c): DRUM clean
7b1ba43a D3 contact (c): SHOTGUN clean
c20ca0ae D3 contact (c): port the OIL/P_MINE contact chain
f39747af D3 contact: bind the two Collision residuals
286d99a2 D3 AI (b): instrument the step inputs, port the RW RNG          <- candidate
4ff428ad D3 modes: oracle covers all 11 rules, APPEND fires, G-G1, G-G2  <- candidate
09a73dc6 D3 AI: port the FUN_00416250 inputs ... opponents drive         <- candidate
647a5e24 D3 powerups: measure all 9 types against the original
38dfeb17 D3 step 2: AI measured against the original
7f71e36f D3 step 1: audit of what a clean-env race actually runs
```

Named as **candidates, not assumptions**: the start-boost port (`9573f3a3`, including its
`Fi_GameMode()` 0→6, which is global and is NOT gated on `slot != 0`), the spawn settle
(`83a7b6ea`), the U-9138 `Rw_TransformPoints` binding (`c5bcf71c`, which is on the player's
own transform path), the modes / race-length change (`4ff428ad`), and the AI tick / clock
globals (`09a73dc6`, `286d99a2`). Every one is *tested*, none is presumed.

---

## 2. PRE-REGISTERED — method, classification rule and pass rule

### 2.1 The measurement, identical at every commit

```
py -3.12 re/tools/statediff/a8_run_port.py verify/player_reg_20260929/<tag> 50 \
    -MASHED_REAL_PHYSICS MASHED_MEASURE_NOOPP=1 MASHED_TITLE="player regression <tag>"
py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/player_reg_20260929/<tag>/motion_diag.log --max-lines 1080
py -3.12 re/tools/statediff/a8_momentum.py  --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/player_reg_20260929/<tag>/motion_diag.log --max-lines 1080 \
    --orig-steer-min 33.0 --port-steer-min 0.9
```

`MASHED_MUTE=1` is in the recipe already, so every run is muted. Builds are made **in place**:
`git checkout <sha> -- mashedmod/` → `mashedmod\build.bat` → measure →
`git checkout HEAD -- mashedmod/`. **No worktree.** `original/` is never touched. Every
`mashed_re.exe` PID is spawned and killed by `a8_run_port.py`, by PID.

### 2.2 The knob at commits that predate it — stated now, not discovered later

`MASHED_MEASURE_NOOPP` was added at `1d0ca916` (`D3d9Render/TrackRenderer.cpp`, 30 lines
around the per-opponent update loop). At any commit older than that the checked-out tree has
no knob, so:

- **At `09a73dc6` and newer** the knob is re-applied as a **temporary local patch** to the
  checked-out `TrackRenderer.cpp` — the identical two-line substitution `1d0ca916` makes
  (`const int aiCarN = s_measureNoOpp ? 0 : (int)ai_cars_.size();` and the loop bound), with
  no other edit. The patch is reverted by the `git checkout HEAD -- mashedmod/` restore. Each
  commit where this was done is recorded in §3.
- **Before `09a73dc6`** the opponents never enter the physics/collision chain at all
  (`D3_DRIVE_2026-09-28.md` §3.4, last row: *"no knob needed — opponents never entered the
  chain"*), so the knob is a no-op there and is **not** applied. This is the same asymmetry
  the `56ad3806` end of `D2_CONTROLLED_ARM_2026-09-29.md` §2.1 already carries.

At `09a73dc6` itself both treatments are available, so **that commit is measured BOTH ways**
(patched knob ON, and unpatched) as the control that the patch is equivalent to the knob.

### 2.3 Classification rule — fixed here, before any run

Per commit, one run. Two statistics decide, and they must agree:

| verdict | `slip 1500-2000` | driving-median |
|---|---|---|
| **GOOD** (reproduces `56ad3806`) | ≥ 0.185 | ≥ 1700 |
| **BAD** (reproduces HEAD) | ≤ 0.170 | ≤ 1000 |
| **INDETERMINATE** | anything else, or the two disagree | |

The slip thresholds are `D2_CONTROLLED_ARM_2026-09-29.md` §3.5's own pre-registered
`≥ 0.185 / ≤ 0.170`, carried verbatim. The median thresholds are the matching split of the
measured 1932 vs 691 with the same proportional margin. An INDETERMINATE commit gets a second
run and, if still INDETERMINATE, is reported as a **partial** mover with its numbers rather
than forced into a bucket.

**Discard rule**, carried from the arm: any run producing fewer than 1080 parsed frames is a
truncated boot, not a sample. It is discarded, re-run, and the discard is reported.

**Search**: binary over the 24 commits. Because the regression may be the sum of more than one
commit, after the first-bad is found the bisect **continues** over `[first-bad .. HEAD]`
whenever the first-bad's numbers differ from HEAD's by more than the C1 bound, until every
transition is attributed.

### 2.4 The PASS RULE for the fix — the criteria of `1d0ca916`, unchanged

The fix is accepted only if the controlled arm at HEAD+fix satisfies the three criteria
pre-registered at `1d0ca916` §2.3, with the **same bounds**, none moved:

| # | criterion | bound |
|---|---|---|
| **C1** | instrument determinism — 3 runs at HEAD+fix agree on `slip 1500-2000` | ±0.005 absolute |
| **C2** | cross-commit agreement — HEAD+fix vs `56ad3806`'s controlled arm (`0.1916 / 0.2669 / 1932.09`) on all three statistics | ±2% |
| **C3** | D2 reproduction — `56ad3806`'s controlled arm vs the ROADMAP §D2 row (`0.1916 / 0.2668 / 1887`) | ±2%, **already measured at `4938adba` and NOT re-run**: PASS / PASS / +2.39% FAIL. Unchanged by this session. |

**C3 is inherited, not re-litigated.** Its one miss (driving-median +2.39%) is a recorded-value
question about the D2 row's `1887`, already flagged in the ROADMAP §D2 amendment, and
re-baselining it is a user decision. This session does not move it.

**And the overriding rule, stated above C2: the fix must make the port match the ORIGINAL, not
merely `56ad3806`.** For every commit the bisect indicts, §4 establishes from the original
binary (RVAs) and from the original's own archived D2 capture
(`verify/a8_steer_20260824/orig_steerR.msd`, `0.1913 / 0.2498 / 1940.59`) which of the two
behaviours is faithful. **If `56ad3806` is itself wrong somewhere, that is said in §4 and the
port is moved to the original's behaviour, not back to `56ad3806`'s** — in which case C2 is
reported as measured and the deviation is named, rather than the bound being bent.

### 2.5 U-9145 — the coupling hunt, method fixed here

Per-global A/B at HEAD on the controlled arm, one temporary diag at a time, removed after,
following `D2_CONTROLLED_ARM_2026-09-29.md` §3.5's next-command list. A candidate is the
coupling only if forcing its opponents-present value with the opponents OFF returns the
player's trace to the opponents-ON values. Named candidates, each already cited:
`g_torqueRingPhase` (`DAT_007f101c`), advanced `(phase + 1) & 0xf` once per A4 call and used
to index the steer ring `+0x1ac + phase*4` / `+0x270 + phase*4` (`0x00470670`);
`Collision::g_suspScratch` (`DAT_00881560`); the powerup dispatcher / pickup field. Any
further candidate found by search is added with its RVA before being tested.

### 2.6 Guards that must be re-run after the fix

Fixed here so they cannot be chosen afterwards:

| guard | requirement |
|---|---|
| criterion (e) — `re/tools/ai_speed_env.py --check` | must stay **PASS 3/3** (it is at 0.05% today) |
| AI (b) — `re/tools/ai_ctrl_window.py --check` | **reported as-is**, not gated |
| power-ups — `re/tools/pu_replay/sweep.ps1` | decision **CLEAN**; contact CLEAN except the known `g3` R_FLAME residue |
| modes oracle — one rule (rule 3) | **GREEN**, MISMATCH 0 |
| `mashedmod\build.bat` | both targets clean |

---

## 3. MEASURED — the bisect

*(filled in after §1-§2 were committed)*

## 4. Which side is faithful — from the ORIGINAL

*(filled in after §3)*

## 5. U-9145 — the coupling

*(filled in after §3)*

## 6. The fix, and the re-measurement

*(filled in last)*
