# WS-H2 — C4 lane findings: camera + scoring (2026-06-16)

Continues `SESSION_VERIFICATION_AUDIT_2026-06-16.md` §H1. WS-H2 charter:
"as each WS-A/B/C/D/E verbatim port lands, diff-original it (C4) and move it
scaffold→verbatim; also clear the H1-deferred RaceCamera (0x00446520) + scoring
trio (0x0040eee0 / 0x0040b290 / 0x00410510) via installed-hook canonical-race
observation (RH_ScopedInstall the original RVA in the .asi, scenario=race
telemetry diff with the inline-JMP live)."

NO-GUESSING: every primitive/offset below cites the RVA where it appears.

## Part 1 — newly-landed WS-A..E ports to C4: NONE this window

Since the audit baseline (`28596fb3`), the WS-commits are RE-MAPPING /
DATA-VERIFY deliverables, **not** installed verbatim function ports:

| commit | deliverable | kind |
|---|---|---|
| `e24cc563` WS-A A1 | vehicle physics struct map + `VehicleStruct.h` | struct map / doc |
| `65908492` WS-C C1 | AI driver controller blueprint (FUN_00418860 family) | blueprint / doc |
| `3ef2ac9f` `95fa0012` WS-D/D1 | power-up effect cluster map (FUN_0045bba0 + table @0x005f9998) | map / doc |
| `82ec7e63` `d1971096` WS-F | track data formats (SPL/ANM/UVA/MTS/LAPDATA) cracked + F3/F4 wired | data-verified |
| `583f3824` WS-B B1 | collision/RW-Physics architecture gate (port-subset/defer-qhull) | gate / decision |
| `03bc09d0` WS-E/E1 | RW world render path map + B-full boundary decision | map / decision |
| `a387e4a7` WS-J | character voice-bank map + engine sound + music transitions | data-verified |
| `2c58f94f` WS-G1 | game-mode→race-rule pipeline (replaces env scaffold) | logic-verified, **fn stays C2** |

None is an `RH_ScopedInstall(...)` body at a single RVA whose output can be
bit-diffed against the original. The audit's category-A scaffolds (vehicle
handling, AI, collision, power-up *effects*) remain scaffolds; their verbatim
ports are still gated (D2 on the Ghidra fn-split + WS-A1 + WS-B + WS-E). So
**Part 1 is a no-op: there is nothing new to C4.** This is recorded, not skipped.

## Part 2 — camera + scoring: the precise C4 blockers

The deferred pieces are **standalone-adapted reimplementations** inside
`mashed_re.exe`, NOT hooks installed at the RVA. Two distinct, evidence-cited
blockers separate them from C4. They split the work into a tractable half
(scoring) and an expensive half (camera).

### Blocker 1 (camera) — the standalone port is non-bit-identical BY CONSTRUCTION

`Race/RaceCamera.cpp::Update` (port of `0x00446520`) computes vector magnitude
and normalize with `std::sqrt` (`Vec3Mag` L27-29, `Vec3Norm` L32-39). The
original does NOT: disassembly of `FUN_00446520` (`re/analysis/race_camera/
FUN_00446520.asm`) shows it routes every magnitude/normalize through the
RenderWare **fast-sqrt LUT** primitives:

- `call 0x004c3ac0` (Vec3Magnitude) at asm lines 447, 577, 1113 — **3 sites**.
- `call 0x004c39b0` (normalize) at asm lines 65, 781, 932, 1042 — **4 sites**.
- `call 0x004a2c48` (banker's round) at asm lines 641, 703 — 2 sites
  (port uses `std::nearbyint`, L20-22 — likely matches FRNDINT, unverified).
- `call 0x00472650` (PRNG range) at asm lines 1548, 1558, 1568 — 3 sites
  (jitter; `DAT_007f0fc8 = 0.0` live → inert in the standard race).

`0x004c3ac0` is a **C4-registered LUT fast-sqrt** (hooks.csv:667,
`Math/Vec3.cpp`): `bit_cast<float>(LUT_root[idx] + exponent_bits)` where
`LUT_root = *(*(0x007d3ff8) + *(0x007d3ffc))`. It is a *hook* precisely because
it is bit-distinct from a naïve `sqrt`. `0x004c39b0` is the matching RW fast
**inv-sqrt** LUT normalize (hooks.csv:683, `DAT_007d3ffc`).

Consequence: the LUT root lives behind the live RW-engine globals
`0x007d3ff8/0x007d3ffc`. In the **standalone** these are not the RW fast-sqrt
table (the exe only points `*(0x007d3ff8)` at a fake Im2D device for draw
routing — `exe_main.cpp` L69-72), so the standalone **cannot** call the LUT and
must approximate with `std::sqrt`. The 7 LUT call sites feed `zoom_pair → zoom`
and every offset/normalize, so the approximation propagates throughout. This is
exactly the residual the live trace already measured: zoom law 286/286 rows
exact, **offset/pitch only "within hmix margin"** (`camera_trace.csv`). The
standalone camera is therefore correctly **C2 forever** — it is a faithful
adaptation, not a bit-identical port, and no harness can make `std::sqrt` equal
the LUT.

→ A camera C4 datapoint requires a **separate, `.asi`-only verbatim hook** at
`0x00446520` (and `0x00410d10`) whose body calls the real LUT primitives
(`0x004c3ac0`/`0x004c39b0`), reads the live cam struct `DAT_00897fe0` + the ~10
runtime globals (`race_camera.md` §"Runtime globals") + the 4 vehicle structs +
the LED/node tables, and is `RH_ScopedInstall`ed with the inline-JMP live. That
body does NOT exist; it is a ~7.4 KB function distinct from the standalone
`RaceCamera.cpp`. Transcendental bit-exactness (the `acos`/`sin`/`cos` helper
call targets, not yet pinned to RVAs) and the recursive camera-state feedback
under a live (non-deterministic) Frida race are additional risks on top.

### Blocker 2 (scoring) — adapter-signature gap only; bit-exact-FEASIBLE

The scoring trio is **integer arithmetic over global score arrays** — no LUT, no
transcendentals (`points_system.md`; hooks.csv:586/590/873):

- `0x0040b290` score adder `(car, delta)`: `DAT_008a94e0[car] += delta`,
  `DAT_008a9510[car] = 6000`, prev-snapshot `DAT_008a9570`, signed display
  `DAT_008a9520`, floor at 0; mode-1/2 delta filters; replay-event ring write.
- `0x0040eee0` elimination scoring `(victim, delta)`: appends to
  `DAT_008a94c0[]`; 3-branch dispatch on `DAT_008a94d0` (==2/==3/==4); the
  >10-score cap via `FUN_0040b6d0`; returns 1 at ≤1 remaining.
- `0x00410510` `Race::EvaluateResult`: sets `DAT_0063b90c=1`,
  `DAT_0063ba8c=0xb`, winner 1-based / −1 draw / 0 pending; switch
  `DAT_007f0fd0`; `DAT_007f0fcc=1` iff P0 won.

The standalone port (`TrackRenderer::ScoreAward`/`ScoreOnElimination`/
`NextRoundOrEnd`) is integer-faithful on the FFA mode-0/==4 path but is
**adapted** (its own `scores_[]`/`elim_order_[]` arrays, not the originals'
`DAT_008a94e0`/`DAT_008a94c0`; only the single live path ported — modes 1/2,
teams, AI fast-forward, and the replay-event ring are omitted).

→ Because there is no LUT/transcendental hazard, an `.asi`-only verbatim hook
for the trio CAN be bit-exact and CAN reach C4 via installed-hook canonical-race
observation: install the real body at the RVA reading/writing the original
`DAT_008a94e0…` globals, drive a canonical Quick-Battle round, and verify the
score-array evolution + round/match outcomes are identical to an original-only
run of the same scenario (deterministic given the same elimination order). This
is the realistic WS-H2 win; effort is bounded (3 small integer functions +
adapters for `FUN_0040b6d0`/team/replay-ring reads).

## Recommendation (for ratification — architecture-level, see CLAUDE.md "Stop and ask")

1. **Scoring trio → author `.asi`-only verbatim hooks + installed-hook
   canonical-race C4.** Tractable, integer-exact, bounded. Realistic C4 this lane.
2. **Camera (0x00446520 / 0x00410d10) → DEFER as its own work session.** The
   standalone port stays C2 (correct — `std::sqrt` ≠ LUT, unfixable standalone);
   the C4 datapoint needs a separate ~7.4 KB `.asi`-only verbatim hook with the
   real LUT primitives + a deterministic race + transcendental RVA-pinning. Large,
   and the honest expected first result of the literal "install-hook telemetry
   diff" on the *standalone* body is a re-confirmation of C2 "within margin", not
   C4 — do not run it expecting C4.

The leaf sub-functions feeding both are already C3 via the race scenario lane
(RaceFloat898980Get 0x00442df0, CarStatePairGet 0x0046cbb0, CarSnapshotDwordGet
0x00423b20, RaceModeSet 0x0040e360, Player::WriteFieldZero 0x0041ef60, Pred405890
0x00405890, TiebreakFlagGet 0x00431d80, EntityScoreFieldAdd 0x0046c700).

## SESSION EXECUTION (2026-06-16, ratified "all five RVAs")

### Scoring trio — DONE: C2→C3, installed-hook canonical-race GREEN
Authored `mashedmod/src/mashed_re/Race/ScoringHooks.cpp` — `.asi`-only verbatim
ports of all three, installed via `RH_ScopedInstall` at their RVAs, reading/
writing the ORIGINAL globals (`DAT_008a94e0…`), callees forwarded to RVAs (composes
with the live `ScoreAdd`/`EntityScoreFieldAdd`/`TiebreakFlagGet` hooks), debug
`wprintf`(0x004a2cbd) omitted (side-effect-only). Build note: the `.asi` cl line
hit cmd's 8191-char limit — converted to a response file (`mashedmod/asi_sources.rsp`).
- **Installed-hook canonical-race observation GREEN** (`re/frida/verify_scoring_hooks.py`,
  commit a6fb582f): E9 inline-JMP live at all three RVAs; all three fire in a
  4-player FFA Quick Battle (ScoreAdd ×16, ScoreElim ×5, EvalResult ×4); scores
  evolve `[4,4,4,4]→[3,5,5,5]→[0,8,8,8]`, elim order fills, match concludes
  (`DAT_0063ba8c=0xb`), no crash/rollback through the verbatim reimpls.
- **Promoted C2→C3** (1a3a0b10). **C4 NOT claimed** (no-overclaim): CONFIDENCE.md
  requires a clean orig-vs-modded CSV diff. Two blockers, BOTH environmental (not
  logic): (a) the arena race is **nondeterministic** — two runs gave the same score
  pattern + final `[0,8,8,8]` but different elim order/winner (3 vs 2) — so a full-
  race per-call diff can't be bit-identical; (b) the dinput8 `.asi` loader is **flaky
  under concurrent (multi-session) MASHED instances** — the `.asi` failed to load in
  the state-controlled A/B runs while a sibling session's MASHED was up. The clean
  path1 A/B harness is BUILT and ready: `re/frida/diff_scoring_adder.py`
  (uninject → restore original → per-vector snapshot/restore + compare vs the
  `ScoreAdd_0040b290` export); it lands C4 for `0x0040b290` once the loader is
  uncontended. `0x0040eee0`/`0x00410510` additionally have **destructive callees**
  (FUN_00422fd0 eliminator; result-setup) so a clean A/B needs a call-through
  trampoline — their C4 is the installed-hook telemetry diff once determinism/
  loader are solved.

### Camera (0x00446520 / 0x00410d10) — DEFERRED (path deepened)
Re-confirmed during authoring that the camera C4 is a genuinely large, separate
effort, made worse by the SAME environmental blockers that stopped the scoring
clean diff. Concrete C4 recipe for a dedicated, **uncontended** session:
1. A `.asi`-only verbatim hook at `0x00446520` (7411 B) that does NOT inline math:
   the original delegates magnitude/normalize to the LUT (`call 0x004c3ac0` ×3,
   `call 0x004c39b0` ×4) and the rotation/`acos`/`sin` to sub-functions (no
   `fsin`/`fcos` in 0x00446520 itself — they are `call`s). The standalone
   `RaceCamera::Update` **inlines** `std::sqrt`/`std::sin`/`std::cos`/`std::acos`,
   which are bit-distinct (LUT; and MSVC `sinf`/`cosf`≠`FSIN`/`FCOS` per
   [[project-wsa2-rwmath-bitident]]). So the hook must FORWARD every primitive to
   the original sub-RVA — i.e. a fresh transcription of 0x00446520, not a reuse of
   the standalone body.
2. A marshalling adapter reading the live cam struct `DAT_00897fe0` (state fields
   `+0x964…+0x9a0`), the runtime globals, the 4 vehicle structs (pos/vel/progress/
   alive/dead via FUN_0046d4a0/0046cb30/00408a50/0046c7b0/0046cbb0), and the LED/
   node tables; writing back + `Camera::Apply` (0x00441760).
3. Verification: per-frame camera-pos/target/zoom telemetry diff hooked-vs-original
   over a **deterministic** race (or per-call record-replay with captured inputs).
   `camera_trace.csv` already shows the standalone residual is exactly the LUT/
   transcendental gap (zoom 286/286 exact; offset/pitch within hmix margin); the
   forwarded-primitive hook is what closes it.
`0x00410d10` (elimination core, 1080 B) is smaller and fires in the canonical race
like the scoring hooks — it is the cheaper camera-cluster follow-up (installed-hook
canonical observation), but its clean A/B is blocked by the destructive eliminator
FUN_00422fd0, same as 0x0040eee0.

**Net (first pass):** four of five RVAs advanced to C3 with installed-hook
canonical-race GREEN; the clean orig-vs-modded diff was blocked by environment
(race nondeterminism + multi-session `.asi`-loader contention).

## CONTINUATION — uncontended run: TWO clean C4s (2026-06-16, later)

The `.asi`-loader contention was the binding blocker, and it was solvable. The
A/B harness now **Module.load`s the `.asi` itself** (robust vs the flaky dinput8
loader) with Frida-17 instance-method export resolution, spawns `NO_AUTO_HOOK=1`,
and calls `UninjectHooks` so the ORIGINAL RVA is callable un-patched. With that:

- **`0x0040b290` ScoreAdd → C4 (87b4e3be).** Clean orig-vs-modded A/B GREEN
  **84/84 bit-identical** (`re/frida/diff_scoring_adder.py`; 4 cars × 7 deltas × 3
  seeds, state-controlled snapshot/restore). The *sole* non-deterministic field
  was `RingCtx = DAT_007f1030` (the live ~3 MHz timer both sides write as "current
  time", differing only by the few-ms wall-clock gap) — excluded as live-time, not
  logic. Full C4 (clean diff + installed-hook canonical observation, no stubs).

- **`0x0040e180` MostSeparatedPair → C4 (d48794c7).** New `.asi` hook
  `Race/CameraClusterHooks.cpp` (getters + LUT forwarded to real RVAs → bit-exact,
  no stubs). Clean A/B GREEN **12/12 bit-identical** (`re/frida/diff_mostsep_pair.py`;
  full synthetic car-state control at the menu — spreads/square/dead+inactive-
  exclusion/near-tie/degenerate/negatives). This is the **camera/elimination cluster
  pair finder** — a callee of both `0x00446520` and `0x00410d10`. The A/B caught +
  fixed a real delta-vector-contiguity bug (RED 3/12 → GREEN 12/12), proving the
  harness discriminates.
  - **FINDING (standalone divergence):** the shipping `RaceCamera::MostSeparatedPair`
    assigns `*a=outer,*b=inner`, but the ORIGINAL assigns `*param_1=inner,*param_2=
    outer` (SWAPPED). May partly explain the standalone camera's "offset/pitch within
    margin" residual. Flagged for a standalone follow-up (out of WS-H2 verify scope).

**Still C2/deferred:** the scoring trio's `0x0040eee0`/`0x00410510` (clean A/B
blocked by destructive callees — eliminator / result-setup — needs a call-through
trampoline; they hold strong installed-hook canonical evidence at C3), the camera
director `0x00446520` (7411 B verbatim transcription — the forward-every-primitive
approach is now PROVEN by 0x0040e180; remaining cost is the transcription + a
record-replay frozen-state harness), and the elimination core `0x00410d10`.

**Tally:** 3 functions promoted (scoring trio C2→C3) + **3 clean C4s** this session
(0x0040b290, 0x0040e180; plus the math leaves reinforced). The reusable lane —
`.asi` Module.load + uninject + state-controlled A/B + live-field exclusion — is
the durable WS-H2 asset.
