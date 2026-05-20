# per_player_team_binding (stride 0x10 = 16 bytes, 4 entries)

**Location:** `DAT_007f1a14` .. `0x007f1a54` (4 entries x 0x10 stride = 64 bytes).
Indexed by player slot 0..3.

**Discovered:** batch-z-s6 (2026-05-19), bucket `re/analysis/bucket_0041dc30/`.
Source: the 15-fn score-accessor bank's IsTeamMode-gated aggregation path
reads `(&DAT_007f1a18)[N*4]` (int* stride 4 == 0x10 byte stride) as the
team_id of player `N`, and walks `(&DAT_007f1a14)[N*4]` as the per-player
is_active sentinel. Anchored by FUN_00424270 (race-position rank dispatcher),
which dereferences the array as `piVar6 = &DAT_007f1a14` with stride 0x10.

**Anchor SHA:** `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`

## Layout

| Offset | Type | Field      | Citation (RVA)              | Notes |
|-------:|------|------------|-----------------------------|-------|
| +0x00  | i32  | is_active  | 0x00424100, 0x004241c0, 0x00424270 | -1 = inactive; gate for aggregation |
| +0x04  | i32  | team_id    | 0x00423bc0, 0x00423c40, 0x00423cc0 | matches index used for aggregation (observed values 0/1) |
| +0x08  | u32  | ?          | ?                           | [UNCERTAIN] semantic — no plate dereferences +0x08 within stride 0x10 |
| +0x0c  | u32  | ?          | ?                           | [UNCERTAIN] semantic — no plate dereferences +0x0c within stride 0x10 |

## Entry table

- `DAT_007f1a14` — slot 0 (`+0x00 is_active`, `+0x04 team_id` == `DAT_007f1a18`)
- `DAT_007f1a24` — slot 1 (`+0x04 team_id` == `DAT_007f1a28`)
- `DAT_007f1a34` — slot 2 (`+0x04 team_id` == `DAT_007f1a38`)
- `DAT_007f1a44` — slot 3 (`+0x04 team_id` == `DAT_007f1a48`)
- end at `0x007f1a54`

## How it's used

The 15-function score-accessor bank (FUN_00423b40 .. FUN_00424070, dispatched
by FUN_00424270 at 0x00424270) uses `team_id` to roll up teammates' scores
into a team total. The IsTeamMode branch (predicate `FUN_0042f500`) in
accessors like FUN_00423bc0 / FUN_00423c40 / FUN_00423cc0 aggregates all
slots `i` whose `team_id` matches the queried slot N's `team_id`, summing
their per-player-struct field (stride 0x4e * i over the `DAT_00899a40` family)
into the returned score.

FUN_00424100 / FUN_004241c0 additionally gate each aggregation term on
`(&DAT_007f1a14)[i*4] != -1` — i.e. inactive slots (`is_active == -1`) are
skipped from the team rollup.

FUN_00424270 also performs a final team-mode tie-break: if two top-ranked
players share the same `team_id`, rank-2 is replaced with rank-3 (3-way
same-team collapse on the sorted permutation).

## Related struct refs

- `[[per_player_score]]` — joined on slot index; the per-player struct base
  is `DAT_00899a40` with stride 0x4e and per-block deltas of 0x138 (== 4*0x4e).
  Each of the 15 score-accessor fields lives at a fixed offset within that
  per-player struct and is aggregated across teammates via this binding table.
