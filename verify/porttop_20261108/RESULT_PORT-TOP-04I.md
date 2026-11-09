# RESULT PORT-TOP-04I: the PTank pair 0x004881d0 / 0x004880a0 and their teardown 0x00489250 off their stand-ins under MASHED_ORIG_TOP (L-96 top-down lane)

Branch `orch/PORT-TOP-04I` from `orch/PORT-TOP-04H` @ `ae4e4e8aa`, with `orch/integration` merged in (`f1102ff38`,
`git -c merge.autoStash=false`). That merge brings the L-105 PC_24 default. PTank family commit `3c534fd8d` (exe
`e667f5ca703fc34a`).

Verdict: HANDOFF at a family boundary. The PTank family landed GREEN on the brief's acceptance: 0x004881d0, 0x004880a0
and their AppDestroy teardown 0x00489250 run matched bodies on the RW-layout PTank record the lead approved. Not
started: 0x00401000 + 0x004014b0, the powerup and panel loaders with their four teardown callees, and 0x00402240 /
0x00402590. The context is heavy (this session also ran 04H). The plan for each is below.

## PTank family

| rva | TU | source | callees |
|---|---|---|---|
| 0x004881d0 | Match/MatchS14_004881d0.cpp, now **one TU in both targets** | re/match/s14/004881d0.c (100%) | exe forwarders call the exe bindings, .asi keeps its raw forwarders (the 04G 0x0040bb30 pattern): 0x004c0b30 -> `MashedRw_FrameCreate_004c0b30`, 0x00534b60 -> `MashedRw_PTankAtomicCreate_00534b60`, 0x004e7e30 / 0x004e8090 -> matched, 0x004880a0 -> matched |
| 0x004880a0 | Match/MatchS14_004880a0.cpp, both targets | re/match/s14/004880a0.c (100%) | same four |
| 0x004e7e30 RpAtomicSetFrame | Match/PortTop_004e7e30.cpp (exe) | re/match/rwl02/004e7e30.c | 0x004c0740 binding, then privateFlags \|= 1 |
| 0x004e8090 RpMaterialSetTexture | Match/PortTop_004e8090.cpp (exe) | re/match/rwl02/004e8090.c | RwTextureDestroy binding (a C forwarder for the source's `int *` prototype) |
| 0x00489250 | Match/PortTop_00489250.cpp (exe) | re/match/s14/00489250.c (BYTE-IDENTICAL 56 B) | RwFrameDestroy 0x004c0c20 (bound), RpAtomicDestroy 0x004e6920 (new binding) |

The PTank record (LibRw/RwApiBind.cpp `MashedRwPTankAtomic`, every offset static_asserted) carries the fields the
matched callers touch, at their RW offsets:
- atomic +0x03 privateFlags (0x004e7e30), +0x04 frame, +0x18 geometry.
- geometry +0x20 material list -> material[0] +0x00 texture (0x004e8090).
- geometry +0x5c morph target, whose +0x04 sphere 0x004881d0 / 0x004880a0 write whole.
- the PTank block at +0x70.

Bindings:
- **RpPTankAtomicCreate 0x00534b60.** Decomp 2026-11-09: flag normalisation (the exe's `mashed_re::Rfx::PTankNormaliseFlags`),
  then FUN_00534d00. The binding allocates the record. The block gets the three FUN_00534d00 stores Render/RfxEmitter.h
  cites: +0x00 maxPCount (0x00534d67), +0x04 = 0 (0x00534d69), +0x40 = 0xffffffff (0x00534d79). [UNCERTAIN] the other
  FUN_00534d00 stores (+0xa0 / +0x3c, where the normalised flags go, the array allocation from 0x0053510c) are not
  decoded and stay 0. No knob-path code reads them.
- **RenderwareAttachPlugins 0x00493640 shim** now also stores 0x70 at 0x007dc57c (RpPTankPluginAttach's offset,
  written by FUN_00534a80 at 0x00534ac4 in the original), so OBJ(h) = *(atomic + [0x007dc57c]) reaches the block. The
  exe's own RfxEmitter stand-in atomic reads the same global. It accepts any aligned offset below 0x7c other than 0x18
  (Render/RfxEmitter.cpp Create), and 0x70 is one. The knob-ON captures (below) did not change.
- **_rwObjectHasFrameSetFrame 0x004c0740** now takes PTank records as well as cameras. It stores the frame at +0x04
  and calls rw::Frame::updateObjects (the matched body's dirty-mark tail).
- **RpAtomicDestroy 0x004e6920.** Decomp 2026-11-09: plugin destructors, instance data, RpGeometryDestroy (which
  destroys materials and their textures), frame unlink, free list. Binding: a PTank record drops its material texture
  (RwTextureDestroy, which undoes 0x004e8090's raise) and is freed. A librw atomic goes to rw::Atomic::destroy.
- librw rw::Texture has filterAddressing at +0x50 (0x004881d0 writes 2 there) and refCount at +0x54 (0x004e8090), the
  RW offsets (static_asserted).

Knob log (exe `e667f5ca`):
- boot: `[0x007dc57c] = 0x70`, `RpPTankAtomicCreate(512, 0x10000087 -> 0x10000087, 0) -> PTank record 08EBED78`,
  `RpPTankAtomicCreate(8192, ...) -> 08EBD7E8`.
- AppDestroy: `RpAtomicDestroy(PTank record 08EBED78): material texture 0F5DF438 released, record freed`, then the
  same for 08EBD7E8. Both carry texture 0F5DF438, the "lensflare-fs8" lookup. 0x00402a40 runs 0x00489250 before the
  dictionary teardown 0x0040bd00, so the count goes 3 -> 1 and the FX dictionary's destroy frees the texture.
- exit 0.

## Acceptance (exe `e667f5ca`)

| check | result |
|---|---|
| knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | exit 0 after 30.8 s, `WinMain 0x00492370 returned 0 after 3000 frames` |
| 12 nav-demo captures, knob ON / OFF / OFF / ON, 3 sets, under the merged PC_24 default | p1 / p1b / p1c: 12/12 each, equal to the 04E list |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref |
| rva-lint | `OK -- no NEW duplicate bodies (1 known)` |
| port column (`backfill_exe_file.py`, then `backfill_port.py --allow-stale`) | 5 rows none -> called: 0x004881d0, 0x004880a0 (Match/MatchS14_*.cpp), 0x004e7e30, 0x004e8090, 0x00489250 (Match/PortTop_*.cpp). Totals none 5203, body 554, called 394 |

Port column notes:
- `--allow-stale`: after the evidence build I reworded one comment in LibRw/RwApiBind.cpp. The rva scanner had read
  the old comment as a body of 0x004c0740, which gave a spurious 0x004c0740 none -> body. The edit is comment-only,
  so the map of the evidence exe still describes the code.
- One more delta comes in with the integration merge, not from this step: 0x00418860 called -> body
  (Ai/AiStandalone.cpp, changed by that merge). [UNCERTAIN] which merged commit: the integration's own backfill had
  not been run.

BINDINGS.tsv (`update_bindings_top04i.py`, runs the 04H updater first): 416 rows, of which matched 137, stand-in 105,
port 42, bound 38, under-stand-in 31, rw 27, shim 20, missing 12, crt 4.

**Knob-ON capture flake (PORT-TOP-KNOBON-CAPFLAKE): no recurrence, plus one data point.** All 6 knob-ON nav runs are
byte-identical to the 04E list. One of them (p1c navdemo_knob1) took 1047 ms for `empire.mpg` (13 frames), longer than
04G's 641 ms outlier, and its 12 captures are still identical. So a slow boot movie alone does not reproduce the
outlier. The other five took 219 to 312 ms.

Build attempts before the evidence exe:
- a C2732 linkage clash: the source's `int *` prototype had the same signature as the C++ overload, so that TU now
  defines the C prototype instead.
- an unresolved `PTankNormaliseFlags`: it lives in `mashed_re::Rfx`.

Machine rules: every build ran with no PRIORITY-HOLD / SOLO in `mashed_lock.py status` (case-sensitive line check).
Every game launch ran through `run_all_04i.py` inside `mashed_lock.py run --label PORT-TOP-04I`, inside `test-slot`.
Roots: `D:/MashedWT/PORT-TOP-04I_root` / `_rootK` (copies of the 04H roots).

## Handed back (for the next row), in the brief's order

1. **0x00401000 then 0x004014b0.** 0x00401000 is a 04G level-3 stand-in in Match/PortTop_0040bbb0.cpp, source
   re/match/s01/00401000.c.
   - Its DFF read is already bound (0x004b3bf0, 04H).
   - Still to bind: 0x004671a0 (matched, exe body), 0x004c1800 / 0x004c1840 (BBox helpers), RpWorldCreate,
     0x004b5320, 0x004b4080 / 0x0053fbb0 (only when DAT_00636ad0 != 0), RpLightCreate (2 and 0x82), 0x004e4900
     (light colour), RpLightSetConeAngle, 0x004e48e0 (radius), RpWorldAddLight, RwFrameAddChild (Ghidra's name for
     the world / clump call at the end).
   - The source writes `DAT_0063656c->flags = 1`: an RW-layout field on the light, so a light needs an RW-layout
     record or a view (the camera / PTank pattern).
   - Then 0x004014b0 (re/match/s01/raw/004014b0.c): RpWorldRemoveLight, RwFrameDestroy, 0x004c0740(light, 0),
     0x004e4d90 (RpLightDestroy), RwFrameRemoveChild, RpWorldDestroy (bound, 04C), RpClumpDestroy (bound).
2. **The powerup and panel loaders** (0x0045bae0, 0x00418980, 0x0041b450 / 0x0041bec0 / 0x0041def0 / 0x0041d8b0 /
   0x0041c100 / 0x0041db90 / 0x0041a1e0, 0x0041eaa0, 0x00420d00), then 0x0041e0d0 / 0x0041ffb0 / 0x004196f0 /
   0x00412010. 04G's closure table holds, and the trees have 10 to 90 nodes. RpClumpStreamRead and
   RpClumpForAllAtomics are bound (04H). A callback that reads RW atomic fields needs a view like 0x004b3f90's.
3. **0x00402240 / 0x00402590.** They need RpGeometryCreate 0x004e8c70, RpMaterialCreate 0x004e7fa0 and
   0x004e8910 / 0x004e8940 / 0x004e8640 / 0x004e8a10. FUN_00401fe0 / FUN_004022a0 write RW-layout geometry fields
   (+0x2c / +0x30 / +0x34 / +0x5c), so they need a geometry record or a librw-built triangle.

## Raw data (D:/MashedWT/PORT-TOP-04I_data)

| file | sha256 |
|---|---|
| build1.log | dca306ed7fdb29fce97f02ebd5ec23530a5bd4dfde7d62c54aff1fed73416370 |
| run_all_04i.py (driver) | c480cc5ff5b960347ef086db44c54d308e88d7fcd18a059dbdb33cb24257bd93 |
| p1_run.log / p1b_run.log / p1c_run.log | c4e75ac42d1beaed18cf1410f45ad126c8601ded681c873bf6c5aacea94491fe / b2c22518589ec2727ee14dea0a3b2941a5c36dc9da6ccfaee81554f217878c87 / 9530ff91deb8e874c00323a4362d2a29215b93a04029e5369d3ba136e59f4191 |
| p1/knob_on_1.log | f5e7c208a0b2e252d6c874ce6923d4a06640becd3b912f5aa70b8ee09b3d103d |
| p1/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| p1c/navdemo_knob1.log (the 1047 ms boot movie) | a25384221254b27b2daa1121c42d2172f25fd897704dc083f5482e4d094f9fbe |
| hooks_before_p1.csv | the hooks.csv copy the port deltas were computed against |
