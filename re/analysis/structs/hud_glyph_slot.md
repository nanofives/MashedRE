# hud_glyph_slot (stride 0xd ints = 52 bytes)

**Location:** `DAT_00898ac0`. Array of HUD glyph slot records used by the
HUD-glyph PROMPT renderer at `0x00432b30`.

**Discovered:** batch-z-s6 (2026-05-19), bucket `re/analysis/bucket_0041dc30/`.
Source: HUD-glyph emitter cluster (`FUN_0042ad10` slot initializer +
`FUN_0042add0` mode-table lookup + `FUN_0042b920` const-getter +
`FUN_0042a9c0` mode-to-code map) and the 1327B PROMPT renderer at
`0x00432b30` which dispatches based on slot fields.

**Anchor SHA:** `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`

**Pool slot of discovery:** Mashed_pool4 (live session was a shared pool6
attach `3e9dbd91709d4509a5250b98cd48ffd5`; the slot allocator picked pool4).

## Layout (13 ints x 4 bytes = 52 bytes / 0x34)

Stride confirmed by the slot initializer at `0x0042ad10`: writes into
`&DAT_00898ac0` with the explicit pattern "slot index * 0xd ints" (13 dwords
= 0x34 bytes), and by the PROMPT renderer at `0x00432b30` which writes the
two trailing flag fields with stride 0x34.

| Offset | Type | Field          | Citation (RVA) | Notes |
|-------:|------|----------------|----------------|-------|
| +0x00  | u32  | glyph_id       | 0x0042ad10     | param_2 — primary glyph code (font-sheet index) |
| +0x04  | u32  | <gap_1>        | 0x0042ad10     | [UNCERTAIN] not written by initializer |
| +0x08  | u32  | <gap_2>        | 0x0042ad10     | [UNCERTAIN] not written by initializer |
| +0x0c  | u8[4]| rgba           | 0x0042ad10     | four bytes 0xff,0xff,0xff,0xff = opaque white; per-channel write at `DAT_00898acc..acf` |
| +0x10  | u32  | flag_a         | 0x00432b30     | values seen: `0xffffffff`, `0x1000`, `0x1` (no-display tombstone / mode bit / enable) |
| +0x14  | f32  | alpha_scale    | 0x0042ad10     | `0x3f19999a` = 0.6f default; written unconditionally by initializer |
| +0x18  | f32  | x              | 0x0042ad10     | screen-x as float; emitter passes integer `0x40` = 64.0 |
| +0x1c  | f32  | y              | 0x0042ad10     | screen-y as float; emitter passes integer `0x1ac` = 428.0 |
| +0x20  | u32  | <gap_3>        | 0x0042ad10     | [UNCERTAIN] not written by initializer |
| +0x24  | u32  | effect_id      | 0x0042ad10     | param_5 — effect / animation selector |
| +0x28  | u32  | <gap_4>        | —              | [UNCERTAIN] not attested in any caller |
| +0x2c  | u32  | <gap_5>        | —              | [UNCERTAIN] not attested in any caller |
| +0x30  | u32  | flag_b         | 0x00432b30     | values seen: `0x1ff`, `0x0` (mode mask or render-layer index) |

Total stride: `0x34` (52 bytes) = 13 ints. The +0x04, +0x08, +0x20, +0x28,
+0x2c slots are accounted for in the stride but are not touched by any
attested writer in this bucket.

## How it's used

- **Slot initializer (`0x0042ad10`, 118B):** writes 8 explicit fields into a
  slot at `&DAT_00898ac0 + slot_index * 0x34`. The slot index comes via
  `*in_EAX` (caller-set register, treated as a "next free slot" cursor).
- **PROMPT renderer (`0x00432b30`, 1327B):** the HUD-glyph PROMPT emitter for
  race-control button hints. Calls `FUN_0042add0` (current-mode table
  lookup, stride 0x40) to resolve the current button-id, then runs a 7-case
  outer switch over the page mode (1/2/4/5/6/8/10), each emitting a primary
  glyph via `FUN_0042ad10` at coords `(0x40, 0x1ac)` = (64, 428) — lower-left
  HUD. Each outer case has an inner 7-way switch over the resolved button-id
  that emits a modifier glyph. Glyph IDs observed: `0x42, 0x43, 0x225, 0x48,
  0x13, 0x58, 0x133`, plus the dynamic result of `FUN_0042a9c0` (returns
  `0x2d` when `DAT_007f0fd0 ∈ {5,7,8,9,10}`, else `0x16`).
- **Tombstone:** when `FUN_0042b920 == 0x16` (always true; it returns the
  constant 0x16) AND a case-1 mismatch is detected, the second glyph's
  `flag_a` field at `+0x10` is set to `0xffffffff` — the "do not display"
  sentinel.
- Coords `(0x40, 0x1ac)` = (64, 428) place the prompt in the lower-left HUD
  region.

## Related struct refs

- `[[per_player_score]]` — same bucket also documented this (stride 0x4e =
  78 bytes at `DAT_00899a40`, separate match-vs-cumulative blocks).
- `[[per_player_team_binding]]` — same bucket, stride 0x10 at
  `DAT_007f1a14` (4 entries: +0x00 is-active flag, +0x04 team_id).

The HUD-glyph slot is independent of per-player state — these are
screen-overlay glyphs (button-prompt icons), not per-player race data.

## Uncertainties

- [UNCERTAIN] Gap fields `+0x04, +0x08, +0x20, +0x28, +0x2c` — present in
  stride but no attested writer in this bucket. Needs caller-side scan of
  any other writer to `DAT_00898ac0 + N*0x34`.
- [UNCERTAIN] `flag_a` at `+0x10` semantics — three distinct constants
  written (`0xffffffff` = tombstone, `0x1000` = mode bit?, `0x1` = enable?).
  Tombstone meaning is the only firm reading.
- [UNCERTAIN] `flag_b` at `+0x30` semantics — `0x1ff` vs `0x0`; could be a
  render-layer index or per-glyph bitmask.
- [UNCERTAIN] The `+0x0c` RGBA region: byte-wise writes confirmed at
  `DAT_00898acc..acf` (per channel) but the field may also be addressable as
  a packed u32. Mark per-channel access as the canonical reading until a
  packed reader is observed.
