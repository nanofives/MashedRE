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
