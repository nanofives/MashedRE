# Struct cross-reference — promote_c2_vehicle_lowrva
Session: promote_c2_vehicle_lowrva-20260517 | Pool: Mashed_pool15 | Confidence: C2

Three repeated struct shapes emerged across the 40 candidates.

---

## 1. ReplayBuffer (DAT_0063bb04..bb2c — singleton; 6+ candidates)

Verified by xrefs in `FUN_004114e0`, `FUN_00411580`, `FUN_004115c0`, `FUN_00411600`,
`FUN_00411750`, `FUN_004117b0`, `FUN_00411870`, `FUN_00411ae0`, `FUN_00411ce0`,
`FUN_00411d60`, `FUN_00411d90` (replay_record band 0x00411170..0x00411f30).

The "replay-buffer object" pointed to by `DAT_0063bb10` (best) and `DAT_0063bb14`
(current). Offsets observed inside the heap-allocated object:

| Offset | Type | Meaning | Cited from |
|---|---|---|---|
| +0x174 | uint  | total length / final-time-stamp | 0x00411a44 (FUN_00411870), 0x00411b8c (FUN_00411ae0 via .174 cmp) |
| +0x17c | uint  | checkpoint-0 timestamp (0 = not hit) | 0x0041169a, 0x004115a0, 0x004117ac |
| +0x180 | uint  | checkpoint-1 timestamp | 0x004116f6, 0x004117a4 |
| +0x194 | uint  | first-frame-written flag (FUN_00411d60 reads it) | 0x00411d78 |

Singleton globals (RVAs are constant data writes):

| Global | Meaning |
|---|---|
| DAT_0063bb04 | replay-A slot (alloc'd by FUN_00482930) |
| DAT_0063bb08 | replay-B slot (alloc'd by FUN_00482930 or loaded via FUN_00483d10) |
| DAT_0063bb0c | playback-ghost slot |
| DAT_0063bb10 | best-lap pointer (currently-displayed ghost) |
| DAT_0063bb14 | current-lap pointer (writer target) |
| DAT_0063bb18 | A/B toggle index (0 or 1; used as &DAT_0063bb04[idx]) |
| DAT_0063bb1c | playback time-cursor (uint ticks) |
| DAT_0063bb20 | record-suspended flag (1 = stop appending frames) |
| DAT_0063bb24 | external-tick-source flag (FUN_00411ae0 overrides param_2 if set) |
| DAT_0063bb28 | render-ghost flag (FUN_00411ce0 gates FUN_00483a70) |
| DAT_0063bb2c | "best loaded from disk" flag (FUN_00411d90 → 1 on FUN_00483d10 success) |
| DAT_005f29c8 | save-once latch (FUN_004117b0 sets to 1 on first save) |

Mode gate: every replay-touching function except getters guards on
`FUN_0042f6a0() == 2` (game mode 2 = Time Trial).

---

## 2. PerCarState (PTR_PTR_005f2770 + i*4 + 0x34; i in 0..3; 3+ candidates)

Observed in `FUN_0040e180` (collision-pair finder), `FUN_0040e370` (bool-getter),
`FUN_00410d10` (DAMAGE_FN inner loop).

The base `PTR_PTR_005f2770` is a pointer at 0x005f2770; the table starts at offset
0x34 from that pointer and spans 0x34..0x44 in stride-4 steps — i.e. 4 entries
(cars 0..3). Each entry is a *pointer to a per-car struct*; non-null = car active.

When dereferenced via `FUN_0046d4a0(&out, idx)`, the resulting struct has:

| Offset | Type | Meaning | Cited from |
|---|---|---|---|
| +0x30 | float | world-X | 0x0040e1d6 (FUN_0040e180) |
| +0x34 | float | world-Y | 0x0040e1dd |
| +0x38 | float | world-Z | 0x0040e1e3 |

The base index runs 0..3 (4 cars) consistent across all three callers.

---

## 3. PerCarRaceProgress (DAT_008a9640 + i*0x30c; 4 cars stride 0x30c; 3+ candidates)

Observed in `FUN_00408a50` (getter), `FUN_00408a70` (setter, 5 writes per call),
`FUN_00410d10` (loops in race-end logic).

| Offset | Type | Meaning | Cited from |
|---|---|---|---|
| 0x008a9640 + i*0x30c | undefined4 | mirror-of-rounded race-pos (4-byte int) | 0x00408aa7 (FUN_00408a70) |
| 0x008a9644 + i*0x30c | undefined4 | mirror-of-rounded race-pos (duplicate) | 0x00408aac |
| 0x008a96e8 + i*0x30c | float       | raw race-pos float                       | 0x00408ab8 (write) + 0x00408a55 (read in FUN_00408a50) |
| 0x008a96f0 + i*0x30c | undefined4 | mirror-of-rounded race-pos (3rd dup) | 0x00408ab1 |
| 0x008a96f4 + i*0x30c | undefined4 | mirror-of-rounded race-pos (4th dup) | 0x00408ab6 |

The setter writes the integer floor (via `FUN_004a2c48`) to four separate slots
and the raw float to one — exact reason for the duplication is [UNCERTAIN] (likely
double-buffered or read by multiple consumer code paths).

---

## 4. PerCarSnapshot (DAT_008994c0..00899a98 stride 0x138; 4 cars; FUN_004248b0 only)

Observed in `FUN_004248b0` (init copy).

| Source | Dest | Field |
|---|---|---|
| 0x008994c0 + i*0x138 | 0x008999a0 + i*0x138 (+ 1) | lap counter (incremented on copy) |
| 0x00899560 + i*0x138 | 0x00899a40 + i*0x138 | 7-dword block copy (offsets 0x00..0x18) |
| 0x008995b0..0x008995bc | 0x00899a90..0x00899a9c | trailing 4 dwords |

Stride 0x138 / 4 entries = 0x4e0 bytes total. This is a per-car race snapshot
captured at start-of-lap.

---

## 5. RaceTransitionParams (DAT_0067eab0..ead4; FUN_0042bf30 only)

Observed in `FUN_0042bf30` and its caller `FUN_0042c280`.

| Global | Meaning |
|---|---|
| DAT_0067eab0 | request-armed flag (guard) |
| DAT_0067eab4 | param 1 |
| DAT_0067eab8 | (cleared on arm; purpose [UNCERTAIN]) |
| DAT_0067eabc | param 2 |
| DAT_0067eac0 | param 3 |
| DAT_0067eac8 | param 4 |
| DAT_0067eacc | param 5 |
| _DAT_0067ead0 | param 6 |
| DAT_0067ead4 | "previous-state-was-active" flag (1 if DAT_0067ea5c != 0 on arm) |
| DAT_0067ea5c | unrelated state flag cleared on arm |

---

## 6. RaceFinalizeState (DAT_0067ebb0..ec24, used by FUN_004331a0)

Observed in `FUN_004331a0` (race-end init) and partially-cleared in `FUN_0042c510`.

| Global | Meaning |
|---|---|
| DAT_0067ebb0 / b4 / b8 / bc | 4 floats all set to 0x43520000 (210.0f) on race-end |
| DAT_0067ebc8 | cleared (0) |
| _DAT_0067ec20 | cleared (0) — overlaps; "overlaps smaller symbols" warning |
| DAT_0067ebcc | cleared (0) |
| DAT_0067eca4 | "race-finalize already done" guard |
| DAT_0067ecac | param_1 store |
| DAT_0067e844, DAT_0067e9f8, DAT_0067e914 | cleared (0) — also cleared by FUN_00432080 |
| _DAT_0067ec24 | cleared (0) |
| DAT_0067ea6c | cleared (0) |
| DAT_0067ebd0[0..0x14] | 0x14-dword (80-byte) zero fill |
| DAT_0067ea08 | cleared (0) — race-step index, also driven by FUN_0042c510 |

210.0f start value [UNCERTAIN — likely max-distance or starting-pos sentinel].
