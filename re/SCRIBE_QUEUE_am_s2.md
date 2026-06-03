# SCRIBE_QUEUE fragment — batch_am session 2 (am_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s2  bucket=re/analysis/bucket_gameplay_0041f060_004210f0  confidence=C1->C2  rvas=0041f060,0041f0d0,0041f100,0041f220,0041f290,0041f2c0,0041f300,0041f330,0041f360,0041f3d0,0041f4f0,0041f590,0041f710,0041f770,0041fe10,00420290,00420340,00420870,00420d80,00420da0,00420dc0,00420e00,00421060,00421080,004210b0,004210f0

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. All `## Mechanical
  description` heading (verified). None drift-skipped — all 26 were C1 `gameplay`
  in hooks.csv at session start (batch_w s1 origin, plates in
  `re/analysis/bucket_0041e140/`); none already C2+. (Note: a loose grep for
  `004210f0` also matches the unrelated 4-byte thunk row `00421590,
  thunk_FUN_004210f0` (boot C2) — the row to promote is `004210f0,FUN_004210f0,
  gameplay,C1`, hooks.csv line 3148.)
- **Pool slot ACTUAL**: pre-assigned **Mashed_pool2** — verified free (no `.lock`),
  opened read-only via `project_program_open_existing` (program_name="MASHED.exe"
  given explicitly to avoid the missing-program_name leaked-lock failure mode),
  `program_close`d cleanly. Recorded in `.pool_slot_am_s2` (untracked marker).
- **needs_function_create**: none — all 26 have function objects (function_at +
  decomp succeeded for all). No `function_create` required.
- **library_skip**: none — every member is a genuine game function. Library-band
  screen clean (no CRT/D3DX9/qhull hits in 0x0041f060..0x004210f0).

### Subsystem confirmation — ALL 26 CONFIRMED `gameplay` (no reclassifications)

This is a cohesive **per-vehicle on-screen visual / effects ORCHESTRATION cluster**
plus a **smoke-particle pool**. It is gameplay glue that DRIVES the render / particle /
audio subsystems (whose primitives live in their own address bands and are already
classified there). Evidence this stays `gameplay` rather than reclassing to
render/particle (despite the batch_am header's expectation, which held for the
al_s4/s6 render-primitive clusters but NOT here):
  - **Callers are gameplay/util/boot, none render/particle**: master per-frame fn
    0x00420870 ← 0x0040da50 (util C2); 0x0041fe10 ← 0x0041ffb0 (boot C2); the
    particle-record getters 0x00420d80/da0/dc0 ← 0x004212b0 / 0x00421100 /
    0x00422160 (all **gameplay C1**); 0x00421060 ← 0x00421690 (gameplay C1).
  - **The in-cluster smoke-particle loop siblings** (0x004212b0, 0x00421100,
    0x00421690) are themselves classified **gameplay C1**, not `particle` — so the
    `particle` subsystem in this project is the lower-level emitter API
    (0x0047xxxx: FUN_004770c0/FUN_004768c0/FUN_00490020), NOT this game-side pool
    driver. Same logic for `render`: the RW primitives these call
    (FUN_004c1520/FUN_004c1340/FUN_00540080-160 material-UV/color, FUN_004b5240
    atomic-visibility, FUN_004e6ab0/FUN_004b3fc0/RpClumpDestroy clump ops,
    FUN_004c0ed0 frame-matrix) are the render band; these 26 are the per-vehicle
    glue that calls them = gameplay. Matches the al_s1 precedent (a particle/
    projectile-effect cluster also stayed gameplay).
  - Central re-classify may still prefer a `render`/`vehicle` reclass for the
    RW-heavy members; flagging so the decision is explicit. My evidence-based
    recommendation is **keep gameplay** for all 26.

### Cluster anatomy (for the central record)

- **Per-slot vehicle-visual record** @ `DAT_0063d9e0`, stride **0x2ac** (684 bytes),
  128 child-atomic pointers at +0x00.., flags dword **+0x294** (bits
  {0x1,0x2,0x4,0x10,0x20,0x40,0x400,0x800}), state field **+0x298** (values 0/1/2),
  player index **+0x290**, wheel-rotation accumulator +0x220..+0x22c, ground-distance
  ring +0x200..+0x21c → averaged +0x284 / derived +0x288, sound-pitch material
  handle +0x27c (`DAT_0063dc5c`) + float +0x280 (`DAT_0063dc60`), clump pointers
  +0x58 (`DAT_0063da38`) / +0x26c (`DAT_0063dc4c`), AABB +0x230..+0x244, vec4 caches
  +0x248..+0x254 & +0x258..+0x264. Camera/view-matrix caches at `DAT_0063d910/d8e0`
  stride **0x40**.
  - matrix accessors: 0041f060 (+0x58), 0041f220 (+0x26c), 0041f290 (force LTM
    recompute, discards), 0041f2c0 (frame detach+reinit).
  - getters: 0041f0d0 (flag bit 0x400), 0041f100 (+0x284), 0041f300 (+0x28c).
  - predicate: 0041f360 (paintable child-index set, gated by DAT_00636ad0).
  - name lookup: 0041f330 (0x84-stride table @ DAT_005f3828; first string "Shuriken";
    [UNCERTAIN] vehicle/weapon/powerup name-by-ID — only member touching a non-slot
    data table).
  - UV/material anim: 0041f3d0 (4 wheels), 0041f4f0 (special atomic +0x4c),
    0041f770 (gauge/tachometer + sound pitch FUN_00544bf0).
  - ground-FX smoothing: 0041f590 (raycast FUN_00426dc0 + 8-ring average).
  - audio-event submit: 0041f710 (builds vec4+player|0xff00 → FUN_00484cf0;
    [UNCERTAIN] bucket 0x00484xxx audio-shaped).
  - clump rebuild: 0041fe10 (clone → atomic-type filter {4,6,8,9,0xa,0xc} →
    RpClumpDestroy → AABB).
  - color refresh: 00420290 (random lerp + clamp), 00420340 (push +0x274/+0x27c/
    +0x278 floats to RW material setters for paintable children, clear dirty bits).
  - **master per-frame**: 00420870 (timers, view-matrix push, distance-to-camera²,
    the 7-fn update chain, per-child atomic-visibility switch via state+flags).
- **Smoke-particle pool**: record @ `DAT_0063e4b8` stride **9 dwords** (TTL-pair
  getters 00420d80/da0/dc0); ring @ `DAT_0063e5a4` stride **0xb dwords** / 128 entries
  + cursor `DAT_0063fb88` + bitmap `DAT_0063e4a8` (init/clear 00421080, 004210b0
  texture "smoke" via FUN_004770c0, destroy 004210f0 via FUN_004768c0); spin/idle
  sprite-emit FX 00420e00 (8× FUN_00490020 + sound FUN_0048f590 + anim events). Small
  record zero-init 00421060.

### Decomp refinements vs the 2026-05-18 C1 plates (for central audit)

- **0041fe10**: `FUN_004e6e00` now decompiles as **`RpClumpDestroy`** — the master
  Ghidra project resolved the symbol since the C1 plate (which had it [UNCERTAIN
  RpClumpDestroy candidate]). Confirmed. (Mirrors the al_s1 0x004074a0 resolution.)
- **0041f590**: the C1 plate's final clamp was **INVERTED**. Correct behavior:
  `fVar1 < threshold(DAT_005d757c) → write 0`; `fVar1 >= 1.0 → write 1.0f
  (0x3f800000)`; else write `fVar1`. (Source idioms `(_DAT_005cc320 < fVar1 ==
  (_DAT_005cc320 == fVar1))` = `fVar1 < 1.0`; the `!=` form = `fVar1 >= 1.0`.)
  `_DAT_005cc320` confirmed = 1.0f.
- **00420870**: the visibility-switch state field is **+0x298** (`DAT_0063dc78`),
  NOT +0x2b0 as the C1 plate stated; it is the SAME field 0041f3d0/0041f770 read as
  the "wheel-array selector" (0/1/2). FUN_0041f3d0 is called here with **3 args**
  (record-ptr, param_2, slot-index); its body consumes only the first two.
  Sound-pitch handle +0x27c, float +0x280.
- **0041f770 / 00420290**: clamp idioms resolve to closed ranges
  `[_DAT_005cc33c, 1.0]` and `[DAT_005d757c, _DAT_005cc568]` respectively.
- **00420e00**: flag writes shown by Ghidra as `1.4013e-45` are the raw **int 1**
  stored in float-typed slots this[2..5]; constants `0x40000000`=2.0f (FUN_0048f590),
  `0x3f800000`=1.0f (FUN_00490020); left side uses FUN_0046be50 slot **5** / events
  0x33,0xe; right side slot **4** / events 0x34,0xf.

### Uncertainties (filed as bare `[UNCERTAIN]` in plates — NOT minted; central mints from U-8000..U-8299)

Recurring open items for central minting:
- RW-primitive callee identities (render band): FUN_004c0ed0 (frame-matrix /
  RwFrameGetLTM-shape), FUN_004c1520 / FUN_004c1340 / FUN_004c1480 (material UV/
  color/matrix setters), FUN_00540080 / FUN_00540100 / FUN_00540160 (material
  setters), FUN_004b5240 (atomic visibility), FUN_0053d090 / FUN_004b4e70 (frame
  detach/reinit), FUN_004e6ab0 / FUN_004b3fc0 / FUN_004b5190 / FUN_004e6fb0 /
  FUN_004e6920 / FUN_004e5fc0 (clump clone/enum/type/remove/destroy/rebuild).
- particle/audio callees: FUN_004770c0 / FUN_004768c0 / FUN_004770a0 (emitter API),
  FUN_00490020 (sprite emit), FUN_0048f590 (sound submit), FUN_00484cf0 (audio
  event submit), FUN_00544bf0 (pitch-shaped scalar setter).
- accessor/data callees just outside this cut: FUN_0046bd60/da0/bce0/bd20/cb30
  (per-(player,wheel) float reads), FUN_00426de0/df0 (random endpoints), FUN_00426dc0
  (raycast/ground query), FUN_0041f120/f1c0/f1e0/ede0/ee50/f880 (sibling helpers),
  FUN_00422470 (smoothing helper), FUN_004b6480/FUN_004b6520 (clear/memset family),
  FUN_0040bb30 (resource-by-name), FUN_004a2c48 (CRT float→int).
- data-semantic holes (non-blocking for C2 of bit-identical leaves): meaning of
  flag bits {0x1,0x2,0x4,0x10,0x20,0x40,0x400,0x800}, state values 0/1/2, the
  FUN_004b5190 type-ID set {4,6,8,9,0xa,0xc}, FUN_0041f360's 14-index paintable set,
  FUN_0041f300's state codes {0x3b..0x3e,0x40}, 0x817 (emitter init param),
  and the 0041f330 "Shuriken" name-table entry layout.
- **Stubs**: none encountered that require minting (all unknowns are out-of-bucket
  callees noted as [STUB] in the plates; no in-bucket placeholder calls).

- Files: bucket dir (26 plates) + this fragment committed this session (atomic,
  path-scoped). `.pool_slot_am_s2` left untracked (session marker).
