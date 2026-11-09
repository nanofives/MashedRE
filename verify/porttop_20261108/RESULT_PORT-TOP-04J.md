# RESULT PORT-TOP-04J: the cup world 0x00401000 and its teardown 0x004014b0 off their stand-ins under MASHED_ORIG_TOP (L-96 top-down lane, family 1)

Branch `orch/PORT-TOP-04J` from `orch/PORT-TOP-04I` @ `97de844b7`, with `orch/integration` merged in (`f231c3aa6`,
`git -c merge.autoStash=false`). Family 1 commit `736a1ff9d` (evidence exe `9be91155804db32d`).

Verdict: HANDOFF at a family boundary. Family 1 landed on the brief's acceptance: 0x00401000 and its AppDestroy teardown
0x004014b0 run their matched bodies, six matched leaves come with them, and the light is an RW-layout record.
- The knob-ON boot exits 0.
- The gates equal the refs.
- rva-lint is OK.
- 8 port rows went none -> called.
- 12-capture check: 2 of 3 sets fully identical. Set 1's first knob-ON run is the PORT-TOP-KNOBON-CAPFLAKE outlier
  again, with the 04G signature (below).

Not started: family 2 (the powerup and panel loaders plus their four teardown callees) and family 3 (0x00402240 /
0x00402590). The plan for each is below.

## Family 1: 0x00401000 + 0x004014b0

| rva | TU | source | callees |
|---|---|---|---|
| 0x00401000 | Match/PortTop_00401000.cpp (exe) | re/match/s01/00401000.c (BYTE-IDENTICAL 482 B) | below |
| 0x004014b0 | Match/PortTop_004014b0.cpp (exe) | re/match/s01/raw/004014b0.c (BYTE-IDENTICAL 182 B) | RpWorldRemoveLight 0x004e4860, RwFrameDestroy 0x004c0c20, 0x004c0740, RpLightDestroy 0x004e4d90, 0x004e45b0 (world, clump), RpWorldDestroy 0x004e5700, RpClumpDestroy 0x004e6e00: all bindings |
| 0x004c1800 RwBBoxInitialize | Match/PortTop_004c1800.cpp | re/match/s19/004c1800.c | none |
| 0x004c1840 RwBBoxAddPoint | Match/PortTop_004c1840.cpp | re/match/s19/004c1840.c | none |
| 0x004e4900 RpLightSetColor | Match/PortTop_004e4900.cpp | re/match/rwl02/004e4900.c (77 B) | none, writes the light record |
| 0x004e4b90 RpLightSetConeAngle | Match/PortTop_004e4b90.cpp | re/match/rwl02/004e4b90.c (55 B) | none, writes the light record |
| 0x004e48e0 RpLightSetRadius | Match/PortTop_004e48e0.cpp | re/match/rwl02/004e48e0.c (32 B) | RwFrameUpdateObjects 0x004c0e50 (bound) |
| 0x004b5320 | Match/PortTop_004b5320.cpp | re/match/s18/004b5320.c (43 B) | RpClumpForAllAtomics 0x004e66d0 (bound, 04H), callback 0x004b5300 (new binding) |

Both s01 sources went through `mglobal_convert.py` (MGLOBAL preamble). match.py re-run after the conversion: both
still BYTE-IDENTICAL. The other callees of 0x00401000:
- 0x004671a0: the matched exe body Match/MatchS11_004671a0.cpp. 0x004b3bf0 (cup.dff): the matched exe body of 04H.
- sprintf 0x004a2b60 (MSVC CRT, libid): the exe CRT's vsprintf (`PortTop_Sprintf04J_004a2b60`).
- 0x004b4080 / 0x0053fbb0: called only when DAT_00636ad0 != 0. Named stand-ins that log. DAT_00636ad0 is 0 on the
  knob path: neither logged.
- 0x0040bbb0 now calls `Orig_00401000` (its 04G stand-in is gone), and AppDestroy 0x00402a40 calls `Orig_004014b0`
  (its 04C stand-in is gone).

**The light record** (LibRw/RwApiBind.cpp `MashedRwLight`, offsets static_asserted). The matched 0x00401000 writes
`DAT_0063656c->flags = 1` (+0x02) itself, and the matched leaves write +0x14 radius, +0x18 colour, +0x03 privateFlags
and +0x28 minusCosAngle. librw's rw::Light has these at other offsets, so RpLightCreate 0x004e4dd0 returns an RW-layout
record (the 04B camera / 04I PTank pattern) that owns a `rw::Light::create(type)`. The record gets the stores of the
matched 0x004e4dd0: type 3, subType, colour 1 x4, privateFlags 1, the self-linked +0x2c list, +0x3c = RwGlobals +0x0a
(int16) - 1, flags 3. Sync callback 0 (nothing calls it through the record).

Bindings:
- RpWorldCreate 0x004e5850: rw::World::create.
- RpWorldAddLight 0x004e4810: copies the record's radius, colour, minusCosAngle, flags and privateFlags into the
  rw::Light, then rw::World::addLight. 0x00401000 calls every setter before it.
- RpWorldRemoveLight 0x004e4860: rw::World::removeLight.
- RpLightDestroy 0x004e4d90: rw::Light::destroy, record freed.
- 0x004e4450 / 0x004e45b0 (hooks.csv names RwFrameAddChild / RwFrameRemoveChild, naming doubt aq_s4: world add /
  remove clump, as the matched sources show): rw::World::addClump / removeClump.
- _rwObjectHasFrameSetFrame 0x004c0740: new light case, frame at +0x04 and rw::Light::setFrame.
- 0x004b5300 (no matched source, disassembly 0x004b5300..0x004b531c): the ForAllAtomics callback of 0x004b5320. It
  calls 0x004b52f0(atomic, mask, set), which reads atomic +0x18 and calls 0x004b52c0 (geometry +0x08 |= mask, or
  &= ~mask). The binding does that on the librw atomic's geometry: librw Geometry::flags is at +0x08 too (asserted).
  **The two matched leaves 0x004b52f0 / 0x004b52c0 are not in the exe.** The first build failed rva-lint CROSS-TARGET:
  the .asi carries hand bodies of both (Util/PromoLoop_sessionB.cpp `Or4b52c0`, `Thunk4b52f0`). [UNCERTAIN] whether
  `Or4b52c0` is wrong: it has no clear arm, and the matched source (re/match/s18/004b52c0.c, BYTE-IDENTICAL 35 B) has
  `f &= ~param_2` when set is 0. Swapping the .asi to the matched TU changes .asi behaviour, so it is left for its own
  row.

**rdata seed.** 0x004e4b90 compares the angle with DAT_005d757c (0.0) and DAT_005cd108 (1.5707964), two .rdata
floats. The exe maps that range zero-filled, so with a 0 upper bound the cone store would be skipped.
`gen_rdata_seed.py` now also seeds a read-only `MGLOBAL(float, ...)` as its 4 bytes. Regenerated over the 8 sources
the .inc already covered plus re/match/rwl02/004e4b90.c: the 35 old rows are unchanged, 2 rows added.

Knob log (exe `9be91155`, k1/knob_on_1.log):
```
RpWorldCreate(bbox 011CFD40) -> rw::World::create 090E1E68
0x004cc230(2, 1, "cup.dff"): 24976 bytes from piz -> memory stream 0902F638
RpClumpStreamRead(0x0902F638) -> rw::Clump::streamRead 0F6BAE40 (24964 bytes, 1 atomics)
0x004b5300(atomic 0F755B68): mask 0x40, set 1: geometry 0467C6F8 flags 0x00000037 -> 0x00000077
RpLightCreate(0x2) -> light record 091AFDC0 (rw::Light 090477F8)
RpLightCreate(0x82) -> light record 091AF730 (rw::Light 09047FE0)
RpWorldAddLight(world 090E1E68, light record 091AF730 type 0x82 flags 1 radius 100 colour 1/1/1/1 minusCos -0.707107)
RpWorldAddLight(world 090E1E68, light record 091AFDC0 type 0x02 flags 3 radius 0 colour 0.75/0.75/0.75/1 minusCos 0)
0x004e4450(world 090E1E68, clump 0F6BAE40) -> rw::World::addClump
... AppDestroy:
RpWorldRemoveLight x2, RpLightDestroy x2, 0x004e45b0 -> rw::World::removeClump, RpWorldDestroy(0x090E1E68),
RpClumpDestroy(0x0F6BAE40)
WinMain 0x00492370 returned 0 after 3000 frames
```
Nothing else in the exe reads 0x00636560..0x00636574: the one exe-compiled reader, CupSpinSpeedAndColor 0x00401340
(Match/MatchS01.cpp), has no exe caller, and HUD/ScenarioWriters_sa2s2.cpp only names the addresses.

## Acceptance (exe `9be91155`)

| check | result |
|---|---|
| knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | exit 0 after 30.8 s, `WinMain 0x00492370 returned 0 after 3000 frames` (p1/knob_on_1.log, and k1/knob_on_1.log before it) |
| 12 nav-demo captures, knob ON / OFF / OFF / ON, 3 sets, PC_24 default | p1b, p1c: 12/12 identical across the four runs and equal to the 04E list. p1: navdemo_hand1, navdemo_hand2 and navdemo_knob2 equal the 04E list. navdemo_knob1 differs on walk_02..walk_12 (capture flake, below) |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref |
| rva-lint | `OK -- no NEW duplicate bodies (1 known)`, after the 0x004b52c0 / 0x004b52f0 TUs were dropped (above) |
| port column (`backfill_exe_file.py`, then `backfill_port.py --allow-stale`) | 8 rows none -> called: 0x00401000, 0x004014b0, 0x004c1800, 0x004c1840, 0x004e4900, 0x004e4b90, 0x004e48e0, 0x004b5320 (Match/PortTop_*.cpp). Totals none 5195, body 554, called 402 |

Port column notes:
- `--allow-stale`: the first backfill_port pass gave 0x00401000 `body`, `unresolved`. The rva scanner had bound a
  comment line of Match/PortTop_00401000.cpp that started with `0x00401000` to the stand-in after it. I reworded the
  comment (comment-only, after the evidence build). After that the scanner binds the RVA to the included source's MATCH
  line (`FUN_00401000` -> `Orig_00401000`), and the second pass gave `called`.
- The first backfill_exe_file pass ran from a `--help` call (the script takes no `--help`). It is the same documented
  operation: 8 rows got an exe_file, and the second pass changed none.

BINDINGS.tsv (`update_bindings_top04j.py`, which runs the 04I updater first) has 429 rows: matched 145, stand-in 105,
port 42, bound 38, under-stand-in 31, rw 30, shim 22, missing 12, crt 4.

**Knob-ON capture flake (PORT-TOP-KNOBON-CAPFLAKE): it recurred, once.** One of the 6 knob-ON nav runs differs: p1
navdemo_knob1, the first nav run after the four gate arms. 11 of its 12 captures differ (walk_02..12), and walk_01
matches. The signature is the 04G one:
- `imgdiff.py` walk_02 against p1 navdemo_knob2: 0 pixels over threshold 16, mean absolute difference 0.03.
- Grid 4x4: 0.1 in the left column's lower three cells and the cell beside the bottom-left one, 0.0 elsewhere.
- In 04G the outlier was also knob run 1 of the set run right after the gates (f2).

Boot-movie timing (frames / ms), every nav run:

| run | empire | supersonic | renderware | intro | captures |
|---|---|---|---|---|---|
| p1 knob1 | 24 / 375 | 26 / 282 | 17 / 344 | 26 / 250 | 11 differ |
| p1 hand1 | 22 / 360 | 26 / 265 | 26 / 250 | 13 / 140 | = 04E |
| p1 hand2 | 25 / 296 | 26 / 234 | 14 / 156 | 14 / 141 | = 04E |
| p1 knob2 | 25 / 297 | 26 / 250 | 12 / 172 | 14 / 157 | = 04E |
| p1b knob1 | 25 / 297 | 27 / 250 | 14 / 141 | 13 / 140 | = 04E |
| p1b hand1 / hand2 / knob2 | 25 / 281, 24 / 297, 25 / 297 | 14 / 141, 14 / 141, 25 / 281 | 13-14 / 141 | 14 / 141 | = 04E |
| p1c knob1 | 25 / 313 | 14 / 125 | 14 / 141 | 14 / 141 | = 04E |
| p1c hand1 / hand2 | 25 / 297, 24 / 297 | 14 / 156, 26 / 234 | 14 / 141 | 14 / 141 | = 04E |
| p1c knob2 | 25 / 297 | 14 / 156 | 22 / 250 | 26 / 234 | = 04E |

The outlier has the longest movie total (1251 ms). p1c knob2 also ran long renderware / intro movies (937 ms total),
and 04I's 1047 ms empire.mpg run was identical, so movie length alone does not reproduce it. What both outliers share
is their slot: knob run 1 right after the gate arms. [UNCERTAIN] the cause. Which bottom-left draw follows timing is
still not traced, and no knob-OFF run has been the first nav run after the gates (the driver order is knob, hand, hand,
knob).

Machine rules: every build ran with no PRIORITY-HOLD / SOLO in `mashed_lock.py status` (case-sensitive check). I
waited out one merge-train hold, 17:42 to 18:05. Every game launch ran through `run_all_04j.py` inside
`mashed_lock.py run --label PORT-TOP-04J`, inside `test-slot` (two 10-minute memory waits, then starts under the
cap). Roots: `D:/MashedWT/PORT-TOP-04J_root` / `_rootK` (copies of the 04I roots). The shared
`original\mashed_re_dev.asi` was not touched (`MASHED_NO_DEPLOY=1`).

## Handed back (for the next row), in the brief's order

1. **Family 2: the powerup and panel loaders** (0x0045bae0, 0x00418980, 0x0041b450 / 0x0041bec0 / 0x0041def0 /
   0x0041d8b0 / 0x0041c100 / 0x0041db90 / 0x0041a1e0, 0x0041eaa0, 0x00420d00), then 0x0041e0d0 / 0x0041ffb0 /
   0x004196f0 / 0x00412010. What the small panels need, read from their matched sources (re/match/s04/0041d8b0.c,
   0041c100.c, 0041db90.c, re/match/s03/h1_0041a1e0.c):
   - The DFF load is 0x0042a5d0. Its exe body Match/MatchS06_0042a5d0.cpp calls 0x004b3bf0 through a raw-address
     forwarder (`MatchS06_0042a5d0_Call004b3bf0`). Under MASHED_STANDALONE it must bind to `Orig_004b3bf0`.
   - 0x004b6520 zero-fills the panel block.
   - 0x004b51d0, the element-array helper, has no body. It fills the block's atomic array from the clump.
   - Each panel then calls 0x004b52f0(atomic, 0x40, 1) on some of those atomics. Two of them (0x0041c100, 0x0041db90)
     also write `atomic +0x18 -> geometry +0x20 -> material[0] +0x04` (the colour) directly. librw atomics have the
     geometry at +0x14, and librw geometry +0x20 is not the material list. So the atomics in that array must be RW-layout
     atomic views: +0x18 -> a geometry view with +0x08 flags and +0x20 = the librw `matList.materials` array (librw
     Material::color is at +0x04, assert it). The flags written to the view must reach the librw geometry. A view whose
     +0x08 is written back, or a geometry view placed so that +0x08 is the librw `flags`, would do it.
   - 0x004b52f0 / 0x004b52c0: the exe cannot take the matched TUs while the .asi keeps its hand bodies
     (`Or4b52c0`, `Thunk4b52f0` in Util/PromoLoop_sessionB.cpp, rva-lint CROSS-TARGET). One option: swap the .asi to the
     matched TU in its own row (it adds the missing clear arm of `Or4b52c0`, so it is a behaviour change in the .asi).
     The other: a binding like 0x004b5300's.
   - Sizes (04G closure): 0x0045bae0 38 gen / 15 none nodes, 0x00418980 43 / 15. The four small panels have 7 gen and
     1 none each. Start with the small panels: they share the view and 0x004b51d0.
2. **Family 3: 0x00402240 / 0x00402590.** re/match/s01/raw/00402240.c is BYTE-IDENTICAL (89 B). Its geometry builder
   0x00401fe0 is not: re/match/s01/00401fe0.c gives 350 differing bytes, and hooks.csv has it at C3, as 0x004022a0. So
   the builder has no matched body to port. It can be a binding or a hand body that builds the triangle in librw.
   Callees of 0x00402240 (disassembly 0x00402244..0x0040228a):
   - RpClumpCreate 0x004e6d80 and RwFrameCreate (bound). The matched body writes `*(clump + 4) = frame` directly:
     librw Clump +0x04 is `object.parent` (asserted in 04H).
   - RpAtomicCreate 0x004e67b0.
   - RpAtomicSetFrame 0x004e7e30 (matched exe body since 04I). Its 0x004c0740 call needs a librw-atomic case in the
     binding: today it takes cameras, PTank records and light records only, and logs anything else as ignored.
   - RpAtomicSetGeometry 0x004e68a0, RpClumpAddAtomic 0x004e6f80, 0x004c1040 (frame add child), RpGeometryDestroy
     0x004e8ea0.
   - 0x00402590 is the same shape over 0x004022a0.
3. **Open from this row:** the capture-flake trace (bottom-left region, first knob-ON nav run after the gates). Also,
   0x004b4080 / 0x0053fbb0 stay named stand-ins: 0x00401000 calls them only when DAT_00636ad0 != 0, and it is 0 on the
   knob path.

## Raw data (D:/MashedWT/PORT-TOP-04J_data)

| file | sha256 |
|---|---|
| build1.log (evidence build) | bcc6ff91db6def50e5ca39e253dbf69e18276bfea3bf3aed8ecc5620af978b36 |
| run_all_04j.py (driver) | e0f1859d04dabc26ed4d05ef660baa90a9174d9815b692f1c901635766578fbf |
| cmp_caps.py | 97016dc8612968fb38875d264f446dfccba5e81dd03272f046da0e34929783b3 |
| p1_run.log / p1bc_run.log | 69f3309ce8a74f11b4f63ba3c270452810770a365176fbde98634ced5f95887d / c3f7a1a8b8cfa3348b61248e5d5957ca467606e90740fad5bf58509c0748d6d5 |
| k1/knob_on_1.log (first knob boot, the log quoted above) | f72492ad35e61ab4a41e9763f124328a1214cc16e2f69f5b28b28b67482a9a8c |
| p1/knob_on_1.log | 4ded3f8f8d88a15e331530d9012a006ef7e4a6b239ffcec7b9719dffa116ab77 |
| p1/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| p1/navdemo_knob1.log (the outlier) | 9130e0020459d16412c09a785713f44069fb275e194b1489eb93486381074215 |
| hooks_before_p1.csv | 6db668b9aa201353844916e7391a57c7eddcea05d93ef0a9c2581ddd806b232b (the hooks.csv the port deltas were computed against) |
| port_report2.tsv | efe80bf119b49583f5dd253d282a56eddb60ef4a9cdf19b1b1db0e759e2f1134 |
