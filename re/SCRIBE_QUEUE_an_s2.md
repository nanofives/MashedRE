# SCRIBE_QUEUE fragment — batch_an session 2 (an_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  an_s2  bucket=re/analysis/bucket_gameplay_00458a40_0045ac40  confidence=C1->C2  rvas=00458a40,00458b10,00458c70,00458d00,00458dd0,00458e00,00458f20,00458f80,00458fa0,00458fd0,00459000,004593b0,00459400,00459480,00459540,004595c0,00459620,0045a0d0,0045a110,0045a130,0045a190,0045a3a0,0045a530,0045a590,0045a950,0045ac40

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in `re/analysis/bucket_gameplay_00458a40_0045ac40/`
  (filenames `0x<rva>.md`). All use the `## Mechanical description` heading
  (verified: 26/26, zero `## Purpose`). None drift-skipped — all 26 were C1
  `gameplay` (batch_w-era C1 plates dated 2026-05-19); none already C2+.
- **Pool slot ACTUAL**: pre-assigned **Mashed_pool2** — verified free (no `.lock`
  at session start; pool0/1/4/5/6 carried `.lock`s, pool1 = the concurrent an_s1).
  Opened read-only via `project_program_open_existing`
  (program_name="MASHED.exe" explicit). `program_close`d cleanly. Recorded in
  `.pool_slot_an_s2` (untracked marker).
- **needs_function_create**: none — all 26 have function objects (function_at +
  decomp succeeded for every RVA). No `function_create` required.
- **library_skip**: none — every member is a genuine game function. Library-band
  screen clean (no CRT/D3DX9/qhull hits in 0x00458a40..0x0045ac40). Note: this
  cluster *calls into* qhull (FUN_0057c210) and the render/RW bands, but no member
  RVA is itself a vendored primitive.

### Subsystem confirmation — ALL 26 recommend `gameplay`

This range is **three cohesive game-logic systems**, all gameplay glue that DRIVES
the render / audio / particle subsystems (whose primitives live in their own
already-classified address bands). This matches the am_s2 precedent exactly
("game-side pool drivers stay gameplay; the lower-level RW/audio primitives are
classified in their own bands"):

1. **Pickup/powerup-crate pool ("pool I")** — base `0x0068b198`, **stride 0x50**
   (20 dwords), **25 slots** (0..0x18), live count `DAT_0068b9a8`, sprite batch
   `DAT_0068b968`. Entry layout (reconstructed, consistent across all members):
   `+0x00` render-obj ptr; `+0x04/+0x08` spin angle/delta; `+0x0c` uv/color vec3;
   `+0x18` TTL (init 120.0f); `+0x1c` timer; `+0x20` state {1 active, 2 respawn,
   3 collected}; `+0x24` type id; `+0x28` flag; `+0x2c..0x34` spawn-origin pos;
   `+0x38..0x40` live pos; `+0x44..0x4c` velocity.
   - Members: 00458a40 (physics integrator, EDI=entry), 00458b10 (post-frame
     proximity submit + render-state flush), 00458c70 (reset-all), 00458d00
     (random pickup-type selector, two tables), 00458dd0 (re-skin by type),
     00458e00 (**spawn** — defines the layout; mode-2 gate via FUN_0042fe30,
     proximity-dedup, type 0x15 special), 00458f20 (per-slot state transition),
     00458f80 (broadcast transition to all 25), 00458fa0 (TTL setter), 00458fd0
     (spawn+TTL wrapper), 00459000 (**per-frame update+draw**: integrate, sprite
     draw via 0x476xxx, positional audio via FUN_00484cf0, respawn/fade state
     machine).
2. **Homing-projectile/weapon pool ("pool J")** — base `0x0068ba30`, **stride
   0x58** (0x16 dwords), **4 slots**, emitter `DAT_0068b9b8`. Entry fields:
   `+0x00` cone/range (180.0f), `+0x04/+0x08` target ids, `+0x0c` active, `+0x10`
   hit/lock flag, `+0x14` range, `+0x18` dist scalar, `+0x1c..0x24` aim vec3,
   `+0x30` owner-RGBA, `+0x34..0x3c` target pos, `+0x44..0x4c` origin pos, `+0x50`
   atomic ptr; clump ptr field at `DAT_0068ba30`.
   - Members: 004593b0 (teardown: RpClumpDestroy + emitter destroy), 00459400
     (trail-mesh **vertex animator**, cosine-wave; directly locks RW geometry),
     00459480 (distance-billboard resize, EDI=entry), 00459540 (deactivate, from
     weapon-spawn FUN_00455150), 004595c0 (activate), 00459620 (**large homing
     brain**: candidate scan over players+entities within cone/range, nearest
     pick, lead/predict, collision/raycast via FUN_0045bfe0/FUN_004b4cd0, qhull
     collision-mesh sweep via FUN_0047ce70/FUN_0057c210, transform commit; uses
     Vec3Magnitude 0x4c3ac0 + FastSqrt 0x4c3b30 + normalize 0x4c39b0), 0045a0d0
     (target-id accessor), 0045a110 (aim-vec accessor), 0045a130 (post-frame
     proximity submit), 0045a190 (**per-frame update+draw**: billboard, lock-on
     flare sprites, trail-anim via RpClumpForAllAtomics→FUN_00459400).
3. **Per-projectile hit-spark / impact-particle pool** — table base `0x0068bd00`,
   **[4 slots][5][5] grid** of **13-dword (0x34-byte)** records (row stride 0x41
   dwords); fields `+0x00..08` pos, `+0x0c..14` vel, `+0x18` age, `+0x28` angle,
   `+0x30` active. Members: 0045a3a0 (zero-init the whole grid + seed a caller
   transform), 0045a950 (per-frame **emit + knockback**: spawn spark record w/
   RNG angle/spread, target-pick via FUN_0040e180/FUN_00407a40, impulse via
   FUN_0046bf50), 0045ac40 (**depth-sort** the 5×5 grid by distance from an
   RwV3d ref, __thiscall). Plus 0045a590 (pickup-item highlight/glow animator —
   operates on an in-world pickup *item object*, distinct struct; pulses
   material color + emits type-0x16 light).

### Render/audio-adjacent members flagged for central subsystem decision

These DRIVE render/audio primitives directly; my evidence-based recommendation is
**keep gameplay** (they are the game-side per-frame tick/glue of the pools above,
exactly like pool I's siblings in am_s2), but central may prefer a reclass:
- **render-leaning**: 00459400 (RW geometry vertex mutation), 00459480 /
  0045a130 / 0045a190 (billboard/draw/submit), 0045ac40 (particle depth-sort).
- **audio-leaning**: 0045a530 (thin wrapper that builds an audio-event descriptor
  and calls FUN_00484cf0; sibling of FUN_0044bbc0/FUN_0044c490 per its C1 note —
  if those siblings are classified `audio`, this likely follows).
Recommendation: gameplay for all 26; if central reclasses, 0045a530→audio and the
five render-leaning draw helpers→render are the only plausible moves. They leave
gameplay C1 regardless.

### Uncertainties (filed as bare `[UNCERTAIN]` in plates — NOT minted; central mints from U-8300..U-8599)

Recurring open items for central minting:
- **RW render callees** (render band): FUN_004c1340/FUN_004c1520/FUN_004c15c0/
  FUN_004c13e0/FUN_004c1480 (material/transform setters), FUN_004c0ed0 (frame LTM),
  FUN_004c5010/FUN_004c45f0/FUN_004c0e50/FUN_004c5770/FUN_004c5930 (frame/clump
  ops), FUN_004b4080/FUN_004b42c0/FUN_004b4650/FUN_004b4cd0/FUN_004b5320/
  FUN_004b5580 (atomic/geometry), FUN_004e6680/FUN_004e8090/FUN_004e89e0/
  FUN_004e8a10 (draw/geometry lock), RpClumpDestroy/RpClumpForAllAtomics (resolved
  RW symbols).
- **sprite/particle callees** (0x476xxx + 0x465/0x474): FUN_004769f0/a0/d0,
  FUN_00476a30/d00/df0 (sprite set/emit/flush), FUN_004770a0/FUN_004768c0 (emitter
  flush/destroy), FUN_00465f40 (light emit type 0x16), FUN_00474d60/FUN_00474e70
  (atomic show / distance).
- **audio callee**: FUN_00484cf0 (positional audio-event submit; packet layouts
  documented in 0x00459000.md and 0x0045a530.md; callback labels LAB_00458920,
  LAB_0045a420).
- **math/collision callees**: FUN_004c3ac0 (Vec3Magnitude, C3), FUN_004c3b30
  (FastSqrt, C3), FUN_004c39b0 (normalize), FUN_004726f0 (dot), FUN_004a3384
  (acos/CRT), FUN_0045bfe0 (collision probe), FUN_0047ce70/FUN_0047d130/
  FUN_0057c210 (qhull collision-mesh iteration), FUN_0055bb70/FUN_0055bde0 (tri
  fetch/intersect).
- **entity/RNG/accessor callees out of bucket**: FUN_0042fe30 (game-mode),
  FUN_00472690/FUN_00472650 (RNG int/float), FUN_004671a0/FUN_004671d0 (camera),
  FUN_0040e370/FUN_0040e180/FUN_004075a0/FUN_004075b0/FUN_00407a40/FUN_00425fa0
  (entity enumeration/scoring), FUN_0041f030/f1c0/f220/f2c0/f300 (per-entity
  pos/lead/state), FUN_0046cb30/FUN_0046bf50 (per-id base / impulse), FUN_00458630
  (type→texture, Session-1 range), FUN_004576b0 (mode predicate, Session-1 range),
  FUN_00455150 (weapon-spawn caller, out of bucket).
- **data-semantic holes** (non-blocking for C2 of bit-identical leaves): pickup
  type-id catalog {7,9,0xa,0xb,0xc,0x10,0x11,0x12,0x13,0x15,0x16}, pool-I state
  codes {1,2,3}, pool-J flag bits / render-state enums (0x14, 0x40), the qhull-node
  flag bit `&4` + collision-layer bitset, and the hit-spark grid index formula.
- **Stubs**: none requiring minting — all unknowns are out-of-bucket callees noted
  as `[STUB]` in the plates; no in-bucket placeholder calls.

- Files: bucket dir (26 plates) + this fragment, committed this session (atomic,
  path-scoped). `.pool_slot_an_s2` left untracked (session marker).
