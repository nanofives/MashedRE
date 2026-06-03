# SCRIBE_QUEUE fragment — batch_ao session 3 (ao_s3)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  ao_s3  bucket=re/analysis/bucket_gameplay_00558100_0055a1f0  confidence=C1->C2  rvas=00558100,00558140,00558b40  library_skip=00558180:renderware,00558400:renderware,00558550:renderware,005586f0:renderware,00558860:renderware,00558c50:RenderWare-Physics-3.7,00558c80:RenderWare-Physics-3.7,00558ca0:RenderWare-Physics-3.7,00558cb0:RenderWare-Physics-3.7,00558d30:RenderWare-Physics-3.7,00559040:RenderWare-Physics-3.7,005592f0:RenderWare-Physics-3.7,00559560:RenderWare-Physics-3.7,005595d0:RenderWare-Physics-3.7,00559b50:RenderWare-Physics-3.7,00559ba0:RenderWare-Physics-3.7,00559bc0:RenderWare-Physics-3.7,00559be0:RenderWare-Physics-3.7,00559c00:RenderWare-Physics-3.7,00559c20:RenderWare-Physics-3.7,00559ee0:RenderWare-Physics-3.7,0055a140:RenderWare-Physics-3.7,0055a1f0:RenderWare-Physics-3.7

## Notes for the sweep

- **Count**: 26 candidates → **3 plated C1->C2** + **23 library_skip** (kept C1,
  reclass-OUT to third-party-library[<tag>] by central re-classify; NOT plated, NOT
  renamed on master). The header's high-library_skip prediction holds for this slice.
- **Pool slot**: pre-assigned `Mashed_pool9` opened read-only cleanly (no `.lock`
  present), `program_close`d at end. `opened_in_slot: Mashed_pool9` in all 3 plates;
  marker `.pool_slot_ao_s3`.
- **Drift check**: all 26 confirmed `gameplay C1` in hooks.csv via anchored `^<rva>,`
  grep at session start; none already >=C2; none drift-skipped. Existing C1 plates for
  all 26 live in `re/analysis/bucket_00557fb0/` (batch_w s5) — deepened, not restarted.
- **subsystem_observed**: the 3 plated RVAs stay **gameplay**:
  - 00558100/00558140 — callback-swap glue pair on game .bss table `DAT_00913274`
    (install/restore `LAB_00557b70` into object+0x48). Callers FUN_0044df80 +
    FUN_00475830 (game pages only).
  - 00558b40 — segment-vs-sphere hit test; callers FUN_004197e0 / FUN_00459620 /
    FUN_0045b390 (all gameplay; the latter two are batch_an s3's detonation citers).
    Mission question answered: **game-side utility math, not an engine primitive** —
    zero engine-region callers, no library statics in body; origin doubt recorded as
    bare [UNCERTAIN] in the plate per the when-in-doubt-plate rule.
- **library_skip evidence** (per-RVA confirm, not address-screen):
  - **RW-Physics farfield/broad-phase family (18)** — operates on the SAME scene/body
    structs as `00559c40 PhysicsSceneBodyRegister`, ALREADY
    `third-party-library[RenderWare-Physics-3.7]` (reclass-OUT batch_ak s3): slot bitmap
    scene+0x60, active count scene+0x4c, body+8 ushort slot index, 0xffff sentinels,
    sticky flag `DAT_007dc8c0` (named in the 00559c40 row), host-callback table
    `DAT_007d3ff8` (+0x10c free at 0x00558dce; +0x108/+0x114 cited in the
    batch_aj-confirmed library rows 0055dc70/00562520). Allocator tag **0x30900** at
    0x00559b6a (00559b50) is IDENTICAL to confirmed-library FUN_0055dc70's tag.
    Family breakdown: container/registry OO 00558c50/c80/ca0/cb0/d30 (dyn-array via
    0x005c4xxx helpers, virtual dtor, RW free); collision-geometry math 00559040
    (triangle 4-plane matrix, pure leaf, 0 callers) + 005592f0 (frustum-corner lerp →
    pixel-ray on a camera-shaped struct, offsets +0x124..+0x180; tag could arguably be
    plain renderware — flagged here, central re-classify may prefer renderware);
    farfield arena 00559560 (size calc 0x148/body + 0xa0/pair + 0xc11c overhead;
    C2-grade correction vs the old C1 plate: there is an ADDITIONAL `*(p+0x14)*0x30`
    term at 0x00559573 the C1 plate missed) / 005595d0 (carve) / 00559b50 (create) /
    00559ba0 (destroy via FUN_00564190); pair stacks 00559bc0/be0 (+0x28/+0x2c) and
    00559c00/c20 (+0x38/+0x3c, 0xffff sentinel reset); body unregister 00559ee0
    (exact inverse of PhysicsSceneBodyRegister 00559c40, same offsets); pair unlink
    0055a140 (hash-bucket walk, free-list at +0x1c, sentinel 0xffff); broad-phase tick
    0055a1f0 (10-arg pair generator, uses `DAT_007dc8c0` sticky flag directly).
  - **RtCharset-shaped buffered Im2D text toolkit (5)** — 00558180 (glyph metrics 8x14
    stride 9x15, 0x5b-char set 0x20..0x7a, returns static font bitmap `DAT_005e48d0`),
    00558400 (1bpp→byte bit-unpack of that font into a raster), 00558550 (batch flush:
    saves/restores 7 render states and issues Im2DRenderIndexedPrimitive THROUGH THE RAW
    DEVICE VTABLE `DAT_007d3ff8`+0x20/+0x24/+0x34 — not the public RW API), 005586f0
    (print: 0x400-glyph batching, strlen idiom, split-flush), 00558860 (per-glyph quad
    emission, 4 verts x 7 floats + 6 indices). NO game-state access; batch statics
    `DAT_007dc8b0/b4/b8/bc` + skip-char `DAT_00623f88` sit in the same static block as
    physics-library `DAT_007dc8c0`. Tagged **renderware** (toolkit-shaped);
    [UNCERTAIN] whether the link unit is RW core toolkit vs the RW-Physics module's
    debug text — either way vendored, kept C1. Game callers FUN_00444b60/FUN_00444ff0
    call it as an API, which does not make it game code.
- **needs_function_create**: none — all 26 had function objects in pool9.
- **C2 corrections vs the batch_w C1 plates** (plated RVAs):
  1. 00558b40: C1's [UNCERTAIN] on FUN_004c3910 RESOLVED — it is an in-place
     approximate normalize returning INVERSE magnitude (table-based invsqrt via
     `DAT_007d3ff8/DAT_007d3ffc`, same family as landed FastInvSqrt 0x004c3b90), so the
     direction vector IS normalized after the call and `*param_3 = dot(d,m)/|d|^2` =
     normalized closest-approach t (NOT a surface-entry t) in BOTH hit branches.
  2. 00558100: decompiler reports an unreachable block at 0x0055812e (not in C1 plate).
- **Uncertainties**: bare `[UNCERTAIN]` markers in plates only — none minted (reserved
  range U-8600..U-8899 untouched, central re-classify mints).
- **Stubs**: noted per-plate, none minted. LAB_00557b70 (installed callback, label not
  function) and FUN_004c3910 (resolved mechanically, no row mutation).
- Files in the atomic commit: 3 plates + this fragment. No hooks.csv writes, no
  re-classify, no master-Ghidra mutation (read-only MCP throughout).
