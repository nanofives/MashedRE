# PREREG D5-RFX-EMITTERS (rfx particle emitters, unblocks D5-ICONCUBE-GLOW)

Branch `orch/D5-RFX-EMITTERS` from `orch/integration` @ `0d1285dc7`. Committed alone, before any arm.

## 1. What is ported

The RpPTank batch API the game builds every particle effect on (18 create sites, 32 submit sites,
21 flush sites, 19 render sites; caller lists from `decomp_pc.py --callers`):

| RVA | role (mechanical) |
|---|---|
| `0x004770c0` | create: game flags -> PTank data flags (bits 1/2/4/8/0x10/0x20/0x40/0x200/0x400/0x800/0x1000 -> 1/4/2/8/0x20/0x80/0x100/0x8000/0x20000/0x80000/0x100000, base 0x10000000; 0 -> 0x807), `FUN_00534b60(cap, flags, 0)`, plugin `+0xac` = 1, `+0xa4` / `+0xa8` = `[0x006132c4]` / `[0x006132c8]` (5 / 6), `+0x40` \|= 0x10800000, `+4` = 0, matrix template `0x00692558..0x00692590`, per-flag arrays via `FUN_00474db0` |
| `0x00534b60` | PTank flag normalisation (pure), then `FUN_00534d00` (ARRAY layout: `+0xa0` = flags at `0x00534d6c`, rebuild only under 0x20000000 at `0x00534dce`) |
| `0x004769a0` `0x004769d0` `0x004769f0` `0x00476a10` `0x00476a30` `0x00476a40` `0x00476a80` | staging setters, NULL defaults `0x006925a8` / `0x00613288` / `0x00613290` / `0x00692558` / none / `0x00613294` / `0x006132a4` |
| `0x00476ae0` `0x00476b30` `0x00476b90` `0x00476be0` `0x00476c10` `0x00476c60` `0x00476cb0` | per-batch constant setters (plugin `+0xb8` / `+0xc4` (+ material `+4`) / `+0xf8` / `+0xc0` / `+0xd8` / `+0xd8` / `+0xa4,+0xa8`) |
| `0x00476d00` | submit: copy the staging globals into slot `count` of each allocated array if `count < cap`, `count++` |
| `0x00476df0` | flush: plugin `+0x40` \|= 0x800000, `+4` = count; if count != 0, lock each array (`FUN_00535700`, flags 1/4/2/8/0x20/0x80/0x100), copy, unlock (`FUN_00535910`), count = 0 |
| `0x00536780` | instance-callback selector on plugin `+0xa0` (position `+0x28`, uv `+0x2c`, colour `+0x30`, normal `+0x34`) |
| `0x00536e90` `0x00536fe0` | camera-facing position instancers (size; size + 2D rotate, x87 `fsincos`) |
| `0x00537b20` `0x00537d20` `0x00537d70` `0x00537e60` | colour (per particle / constant `+0xc4`) and uv (per particle / constant `+0xd8`) instancers |
| `0x004770a0` | render: draw only if plugin `+4` != 0 (librw wrapper `RaceFx_DrawEmitter`) |

Pure core: `mashedmod/src/mashed_re/Render/RfxEmitter.{h,cpp}` (exe). Draw wrapper in
`LibRw/RwRaceFx.cpp`. Position callbacks `0x00536940` `0x00536ad0` `0x00536c90` `0x005371f0` `0x00537460`
`0x00537610` `0x00537890` `0x005379c0` (matrix / USECENTER variants), colour `0x00537b80` `0x00537c40`,
uv `0x00537de0` `0x00537ed0` are not ported: `Instance()` returns false for them and nothing is drawn.

## 2. Knob

`MASHED_RFX_EMITTERS` (default OFF, not in FaithDefaults). It gates `RaceFx_DrawEmitter`. This step wires
**no caller**: the PickupTick / FUN_00458b10 consumer is D5-ICONCUBE-GLOW. So the knob changes nothing
in this step, and the default build only gains unreferenced code.

## 3. Arms and predictions

- **A1 (original bytes in Unicorn)**: `verify/rfx_emitters_20261028/emit_unicorn.py` runs the original's
  own bytes from `original/MASHED.exe.unpatched` (create with `FUN_00534d00` / `FUN_00474db0`'s allocator /
  `FUN_004e8090` / `FUN_004c0b30` / `FUN_004e7e30` stubbed; everything else verbatim, x87 CW 0x007f) and
  the port's core (compiled `emit_port.exe`) on the same cases:
  - C1 create, every caller's flags (0x807, 0x80c, 0x817, 0x427, 0x895, 0) + 256 random 13-bit flag words,
    capacities 1..300: emitter struct, plugin fields, allocated array sizes, matrix template.
  - C2 setter / submit / flush sequences (>= 200 sequences, NULL defaults included, submits past cap).
  - C3 selector on every flag word C1 produces.
  - C4 instancing, puglow flags 0x817 and the no-rotate set 0x807 with uv constant, >= 2000 particles,
    random positions, sizes, angles (including 0 and +-720 deg in radians) and camera vectors.
  **Prediction: 0 mismatches, compared bitwise (every dword), on every case.** Any mismatch = RED.
- **A2 (gates)**: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the branch's
  `mashed_re.exe`. **Prediction: G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`**
  (`re/orchestrator/merge_train.json`). A changed hash = RED (layout sensitivity is a known failure mode).
- Not run: no original-game boot. The draw wrapper's render state is checked against the original's
  PUGlow row (`verify/d1r1_20261016/evidence/training_orig_row_00884.draws.json`, event 225) by
  D5-ICONCUBE-GLOW, which owns the consumer.

## 4. Verdict rule

GREEN = A1 0 mismatches on all four case sets AND A2 hashes equal. Otherwise RED with the failing case.
