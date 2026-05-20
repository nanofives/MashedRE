# per_player_score (stride 0x4e ints = 0x138 bytes = 312 bytes)

**Location:** `DAT_00899a40` (match block; 4 player slots) and `DAT_00899f20`
(cumulative "season totals" block; same stride; +0x4e0 from match base = 4
slots × 0x138).

**Discovered:** batch-z-s6 (2026-05-19), bucket `re/analysis/bucket_0041dc30/`.
Source: 15-fn score accessor bank `0x00423b40..0x00424070` (all called by the
rank dispatcher `0x00424270`), the end-of-round adder `0x00424920`, and the
post-match stats printer `0x00424b80`. All accessors read the struct via
`(&DAT_xxxxxxxx)[param_1 * 0x4e]` where param_1 is the 0..3 player index
treated as an int-array index, so the **byte stride between adjacent players
is `0x4e * sizeof(int) = 0x138` bytes**. The task brief's "0x4e = 78 bytes"
collapses int-stride and byte-stride; cite `0x00423b40` for the indexing form.

**Phase 1 anchor commit:** `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`
(SHA-256 of `original/MASHED.exe.unpatched`).

**Confidence:** C1 — fields mechanically attested from decompilation; semantics
attested only where the post-match stats printer's format string fixes them.

---

## Match block layout (DAT_00899a40, player 0; player N at +N*0x138)

| Offset | Width | Type | Accessor RVA | DAT for player 0 | Mechanical semantics |
|-------:|------:|------|--------------|------------------|----------------------|
| +0x00  | 4 | int32 | `0x00423c40` | `DAT_00899a40` | field_at_+0x00; team-aware (sums across teammates when `0x0042f500` returns non-zero); no submode gate |
| +0x04  | 4 | int32 | `0x00423cc0` | `DAT_00899a44` | field_at_+0x04; submode-gated (returns 0 if `GetRaceSubMode == 4`) + team-aware |
| +0x08  | 4 | int32 | `0x00423dd0` | `DAT_00899a48` | field_at_+0x08; submode-gated + team-aware |
| +0x50  | 4 | int32 | `0x00423d50` | `DAT_00899a90` | field_at_+0x50; team-aware; no submode gate |
| +0x54  | 4 | int32 | `0x00423b40` | `DAT_00899a94` | field_at_+0x54; submode-gated; plain `arr[N]` (no team aggregation) |
| +0x58  | 4 | int32 | `0x00423b60` | `DAT_00899a98` | field_at_+0x58; submode-gated; plain `arr[N]` |
| +0x5c  | 4 | int32 | `0x00423bc0` | `DAT_00899a9c` | field_at_+0x5c; team-aware; no submode gate |

Note: the rank dispatcher `0x00424270` exposes **14 score-types** (cases 0..0xd
of the outer switch) but only the seven accessors above (plus their seven
cumulative-block twins, see next section) are called from this bucket. The
remaining seven score-types use accessors that live outside the score-accessor
bank or are inlined; [UNCERTAIN] which struct offsets they target.

Per `0x00424270`, cases **5 and 0xc** are "lower-is-better" (initialised with
sentinel `100000` and sorted ascending) — the rest are "higher-is-better"
(sentinel `-100`, descending). [UNCERTAIN] which offsets in this struct
correspond to cases 5/0xc; the dispatcher resolves them by score-type, not by
struct offset directly.

---

## Cumulative block layout (DAT_00899f20 = DAT_00899a40 + 0x4e0)

Same shape as the match block. `0x4e0 = 4 * 0x138`, i.e. the cumulative block
sits immediately after all four match-block slots, suggesting these are two
parallel 4-slot arrays rather than one 8-slot array (the accessor bank treats
them as distinct bases).

| Offset (from match base) | DAT for player 0 | Accessor RVA | Notes |
|------------------------:|------------------|--------------|-------|
| +0x4e0 | `DAT_00899f20` | `0x00423ee0` | cumulative twin of +0x00 |
| +0x4e4 | `DAT_00899f24` | `0x00423f60` | cumulative twin of +0x04 |
| +0x4e8 | `DAT_00899f28` | `0x00424070` | cumulative twin of +0x08 |
| +0x530 | `DAT_00899f70` | `0x00423ff0` | cumulative twin of +0x50 |
| +0x534 | `DAT_00899f74` | `0x00423b80` | cumulative twin of +0x54 |
| +0x538 | `DAT_00899f78` | `0x00423ba0` | cumulative twin of +0x58 |
| +0x53c | `DAT_00899f7c` | `0x00423e60` | cumulative twin of +0x5c |

---

## End-of-round accumulation (FUN_00424920)

`0x00424920` is a pure leaf with **no callees** that runs at end-of-round and
performs 32 explicit `block2[i] += block1[i]` writes — 8 fields × 4 players.
The plate at `re/analysis/bucket_0041dc30/0x00424920.md` confirms
`block1 = 0x00899a40`, `block2 = 0x00899f20`, inter-player delta `0x138`,
inter-block delta `0x4e0`. Seven of the eight per-player field offsets are
attested by the accessor bank above; the **eighth field is attested only by
this adder** and is not read by any accessor in this bucket:

| Match offset | Cumulative offset (+0x4e0) | Field | Citation |
|-------------:|----------------------------:|-------|----------|
| +0x00 | +0x4e0 | field_at_+0x00 | `0x00424920`, `0x00423c40` |
| +0x04 | +0x4e4 | field_at_+0x04 | `0x00424920`, `0x00423cc0` |
| +0x08 | +0x4e8 | field_at_+0x08 | `0x00424920`, `0x00423dd0` |
| +0x50 | +0x530 | field_at_+0x50 | `0x00424920`, `0x00423d50` |
| +0x54 | +0x534 | field_at_+0x54 | `0x00424920`, `0x00423b40` |
| +0x58 | +0x538 | field_at_+0x58 | `0x00424920`, `0x00423b60` |
| +0x5c | +0x53c | field_at_+0x5c | `0x00424920`, `0x00423bc0` |
| [8th] | [8th + 0x4e0] | [UNCERTAIN] | `0x00424920` (offset not attested in this bucket) |

[UNCERTAIN] the 8th field offset — `0x00424920` is 608 bytes of unrolled adds;
the exact offset of the 8th field requires re-decompilation and offset-by-offset
enumeration, not done in this bucket.

---

## Adjacent / out-of-bucket fields with score-like semantics

The post-match stats printer `0x00424b80` iterates a **different** per-car
struct at `DAT_008994c0` (stride 0x4e ints = 0x138 bytes, same as the score
block but a distinct array; cross-ref U-1510 in `race_state_globals.md`). Its
format strings confirm the existence of these per-player scoring quantities
**at the engine level**, but the offsets it cites are inside
`DAT_008994c0..0x008999c0`, NOT inside `DAT_00899a40..0x00899f1f`:

| String (per `0x00424b80`) | Player 0 DAT | Array base | Notes |
|---------------------------|--------------|------------|-------|
| `cleanest line %.2f` | `DAT_00899560` | `DAT_008994c0` | per-car raw quality value; +0x94 within per-car struct |
| `number of pants %d`  | `DAT_008995b0` | `DAT_008994c0` | per-car counter; +0xf0 within per-car struct |
| `number of crowns %d` | `DAT_008995b4` | `DAT_008994c0` | per-car counter; +0xf4 within per-car struct |

[UNCERTAIN] whether the per-car struct at `DAT_008994c0` and the per-player
score struct at `DAT_00899a40` are the same record shape with different
contents, or two parallel but distinct structs. The strides match (0x138) and
both have 4 slots, but the field-offset distributions differ
(0x00/04/08/50/54/58/5c here vs. 0x94/f0/f4 in the per-car struct). Resolution
requires cross-referencing every write site for both bases.

---

## Per-slot dispatch (FUN_00424270 race-position rank dispatcher)

The 1537-byte rank dispatcher `0x00424270` is the only known caller of every
accessor in the table above. It is called with `(int rank_out[4], int score_type, int direction)`:
- `score_type` 0..0xd selects one of 14 case branches; each case calls a
  matching accessor across the 4 players (active-flag gated via
  `(&DAT_007f1a14)[i*4]` != -1).
- After per-player fill, if `FUN_0042f500()` (is-team-mode) returns non-zero
  AND case is NOT in {0,1,7,8}, scores are collapsed across teammates using
  `(&DAT_007f1a18)[i*4]` (team_id table — see `per_player_team_binding.md`).
- Bubble-sort, with final 3-way same-team tie-break replacement at rank 2/3.

---

## Related struct refs

- `per_player_team_binding` (stride 0x10 at `DAT_007f1a14`) — joined on slot
  index by every team-aware accessor. The is-active flag (`-1` = inactive)
  lives at `DAT_007f1a14/24/34/44`; the team_id at `DAT_007f1a18/28/38/48`.
- `[[per_player_team_binding]]`
- `race_state_globals.md` U-1510 / U-1511 — the previously-uncharacterised
  per-car snapshot buffers at `0x008994c0` (source) and `0x00899a40` (dest);
  this doc partially resolves U-1511 (it is the per-player score MATCH block).
  U-1510 (per-car race data at `0x008994c0`) remains open: stats printer
  `0x00424b80` shows fields at +0x94/+0xf0/+0xf4 but the full layout is not yet
  characterised.

---

## Open uncertainties

| U-ID | Address | Gap |
|------|---------|-----|
| U-PPS-1 | `DAT_00899a40 + [?]` | 8th field per-player attested by `0x00424920` but not by any accessor in this bucket; offset unknown |
| U-PPS-2 | `0x00424270` cases 5 & 0xc | Which struct offsets correspond to "lower-is-better" score-types (sentinel 100000)? Likely lap-time or best-line, but offset mapping requires per-case decompilation |
| U-PPS-3 | 7 of 14 dispatcher cases | The remaining 7 score-types in `0x00424270` use accessors outside the bucket; their struct offsets are not attested here |
| U-PPS-4 | `DAT_008994c0` vs `DAT_00899a40` | Same shape or two distinct structs? Strides match but observed offset sets do not overlap |
| U-PPS-5 | `DAT_008999a0`, `DAT_00899e80` | Bases used by accessors `0x00424100` / `0x004241c0` — sit 0xa0 before `DAT_00899a40` and 0x440 into the block respectively; relation to this struct unclear |
