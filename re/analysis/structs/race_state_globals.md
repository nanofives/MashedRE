# Race State Globals — Static BSS Block

**Session:** struct_extract_phase5 (2026-05-12)
**Evidence sources:** race_state-20260503.md (8 C1 plates), race_state_d2-20260503-1737.md (4 C1 plates), leaderboard_d3 SESSION_END.md (30 C1 plates cross-referencing this block), leaderboard_d3/0x00403250.md
**Confidence:** C1 — fields mechanically confirmed from decompilation; no single root pointer (static BSS globals)

---

## Layout overview

These are **static BSS globals**, not a pointed-to struct. All addresses are direct absolute references. The block spans roughly 0x0067e000..0x0067f200, but is very sparse — only observed fields are listed.

No single `g_race_state*` pointer was found. Every caller accesses fields by hard-coded absolute address.

---

## Trigger / arm state (0x0067eca0 cluster)

| Address | Width | Type | First write RVA | First read RVA | Tentative name |
|---------|-------|------|-----------------|----------------|----------------|
| `0x0067eca4` | 4 | int32 | FUN_00432080 `0x004321cc` (write 0 on init), FUN_004331a0 (write 1 to arm) | FUN_00432080 (checked at top) | `g_raceTriggerArmed` |
| `0x0067ecac` | 4 | int32 | FUN_00432080 `0x004321cc`, FUN_004331a0 (from param_1) | — | `g_raceTriggerHandle` [UNCERTAIN U-1073] |
| `0x0067ecb0` | 4 | int32 | FUN_00432080 (write path?) | FUN_0042b930 (getter at 0x0042b930) | `g_raceStateModeDiscriminant` — returned value 3 routes to alt vehicle getter |
| `0x0067ecb4` | 4 | int32 | — | FUN_0042c2d0 (getter) | [UNCERTAIN] — read by getter only; no write site observed |
| `0x0067ecb8` | 4 | int32 | FUN_0042c2f0 (setter from param_1) | FUN_0042c2e0 (getter) | [UNCERTAIN] — setter/getter pair; semantic unknown |
| `0x0067f19c` | 4 | int32 | FUN_00432080 `0x004321e6` (write 2 = trigger fired) | [unknown readers] | `g_raceTriggerOutput` — 0=not fired, 2=fired; set in TRIGGER block |

---

## Race timer floats (0x0067ebb0 cluster)

Initialized to 210.0f (0x43520000 IEEE 754 single) by FUN_004331a0 at race init.

| Address | Width | Type | Init RVA | Tentative name |
|---------|-------|------|----------|----------------|
| `0x0067ebb0` | 4 | float | FUN_004331a0 body | `g_raceTimerA` [UNCERTAIN U-1075: semantic unknown — timer default? countdown limit?] |
| `0x0067ebb4` | 4 | float | FUN_004331a0 body | `g_raceTimerB` |
| `0x0067ebb8` | 4 | float | FUN_004331a0 body | `g_raceTimerC` |
| `0x0067ebbc` | 4 | float | FUN_004331a0 body | `g_raceTimerD` |

---

## Transition request block (0x0067eab0 cluster)

Written by FUN_0042bf30 (transition-request loader) when `g_raceTransitionGuard == 0`.

| Address | Width | Type | First write RVA | Tentative name |
|---------|-------|------|-----------------|----------------|
| `0x0067eab0` | 4 | int32 | FUN_0042bf30 body (arm guard) | `g_raceTransitionGuard` — 0=not armed; write arms the block |
| `0x0067eabc` | 4 | int32 | FUN_0042bf30 body (from param_2) | `g_raceTransitionCode` — consumed by FUN_0043d7c0 timer state machine |
| `0x0067ea5c` | 4 | int32 | [unknown writer] | `g_raceTransitionPropagateFlag` [UNCERTAIN U-1508] — if set in FUN_0042bf30, sets `0x0067ead4=1` and clears |
| `0x0067ead4` | 4 | int32 | FUN_0042bf30 body (set 1 when ea5c propagates) | [UNCERTAIN U-1508] — downstream meaning unknown |
| `0x0067eab8` | 4 | int32 | FUN_0042bf30 body (zeroed on arm) | [UNCERTAIN U-1507] — zeroed on arm; no reader seen |

Called by FUN_0042c280 with hardcoded args `(0x27f, 0xff210000, 0, 0, 0, 0)`.

---

## Working buffer — zeroed at trigger / init

### Trigger-commit zeros (FUN_00432080 TRIGGER block, confirmed at 0x004321xx)

| Address | Width | Also zeroed by |
|---------|-------|----------------|
| `0x0067e844` | 4 | FUN_004331a0 |
| `0x0067e9f8` | 4 | FUN_004331a0 |
| `0x0067e914` | 4 | FUN_004331a0 |

### Race-init zeros (FUN_004331a0)

| Address | Width |
|---------|-------|
| `0x0067ebc8` | 4 |
| `0x0067ebcc` | 4 |
| `0x0067ec20` | 4 |
| `0x0067ec24` | 4 |
| `0x0067ea6c` | 4 |
| `0x0067ea08` | 4 |
| `0x0067ebd0..0x0067ec1f` | 80 (20 × 4) — REP STOSD fill |

### Bulk zero — FUN_0042d3a0

Called by both FUN_00432080 (trigger commit) and FUN_004331a0 (race init). Zeroes 832 bytes:

| Address | Size | Notes |
|---------|------|-------|
| `0x0067ed78..0x0067f0b7` | 832 (13 × 64) | race-phase working buffer; semantics of individual fields unknown |

---

## Lap/round timer trigger state (0x00636af0 cluster)

Used by FUN_00403250 (lap/round timer trigger, Race::Tick case-0xb). These are separate from the main race trigger block.

| Address | Width | Type | Notes |
|---------|-------|------|-------|
| `0x00636af0` | 4 | float | Accumulated input time; += param_1 each call |
| `0x00636af4` | 4 | float | Auto-advance threshold — triggers transition when `0x00636af0` exceeds this |
| `0x005cc574` | 4 | float | Minimum hold time before player-button trigger counts |

---

## Per-car snapshot buffers (FUN_004248b0)

| Address | Stride | Count | Notes |
|---------|--------|-------|-------|
| `0x008994c0` | 0x138 | 4 | Source: per-car race data struct (7 dword fields + lap counter) |
| `0x00899a40` | 0x138 | 4 | Dest: snapshot / next-lap buffer; lap counter copied + 1 |
| `0x008999a0` | [inline] | 4 | Next-lap target init (lap_counter + 1 written here) |

[UNCERTAIN U-1510/U-1511] Full layout of these 0x138-byte per-car structs not yet characterized.

---

## 100-loop stub globals (FUN_00448700)

| Address | Notes |
|---------|-------|
| `0x00897fe0` | Constant arg to FUN_004464c0 in 100-iteration loop |
| `0x00897ffc` | Receives param_1 after loop |
| `0x00898000` | Receives param_2 after loop |

---

## Open uncertainties

| U-ID | Address | Gap |
|------|---------|-----|
| U-1073 | 0x0067ecac | Handle type — stored as 32-bit int from param_1; actual type (object ptr? index?) not determined |
| U-1075 | 0x0067ebb0..ebbc | 210.0f floats — semantic meaning (timer default? countdown?) not determined |
| U-1507 | 0x0067eab8 | Zeroed on arm; no reader seen |
| U-1508 | 0x0067ea5c / 0x0067ead4 | What sets ea5c? What does ead4=1 trigger? |
| U-1510 | 0x008994c0 | Source struct at 0x008994c0, stride 0x138 — layout unknown |
| U-1511 | 0x00899a40 | Dest snapshot buffer at 0x00899a40, stride 0x138 — layout unknown |
