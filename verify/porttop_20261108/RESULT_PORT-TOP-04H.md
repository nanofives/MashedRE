# RESULT PORT-TOP-04H: perm clumps 0x004025f0 and the sun texture release 0x0048bc10 off their stand-ins under MASHED_ORIG_TOP (L-96 top-down lane)

Branch `orch/PORT-TOP-04H` from `orch/PORT-TOP-04G` @ `bb49e0b0b`, with `orch/integration` @ `4abc2ce60` merged in
(`2921f2925`, `git -c merge.autoStash=false`). Family 3a commit `29fe859b3` (exe `2038d5a6b02a02ef`). Item 3 commit
`b61410b98` (exe `3115efc03341442e`).

Verdict: HANDOFF at a family boundary. Two items landed GREEN on the brief's acceptance: the perm-clump loader 0x004025f0
(family 3a) and the sun texture release 0x0048bc10 (item 3). Not moved, with a sized plan below: 0x004881d0 (item 2: it
needs an RW PTank layer, and librw has none), the powerup and panel loaders (rest of family 3), and the six teardown
callees (family 5). None of the six has its loader running yet (table below).

## Scope landed vs handed back

| item from the brief | state |
|---|---|
| 1. family 3: perm clumps 0x004025f0 | **done** (`29fe859b3`): matched body, 13 clumps read on librw. RpClumpStreamRead 0x004e7420 and RpClumpForAllAtomics 0x004e66d0 are bound |
| 1. family 3: powerups 0x0045bae0 / 0x00418980, panels, emitters 0x0041eaa0, 0x00420d00 | **handed back** (sizes in 04G, not changed by this step) |
| 2. 0x004881d0 | **handed back**: RpPTankAtomicCreate 0x00534b60 and the PTank plugin offset (below) |
| 3. sun texture release 0x0048bc10 | **done** (`b61410b98`): matched body in the matched AppDestroy texture teardown 0x0040bd00, the texture is freed at exit |
| 5. six AppDestroy teardown callees | **handed back**: no loader of the six runs yet (table below) |
| fonts, 0x004924f0 | out of scope (MATCH-RT2D-FONT, PORT-TOP-04D-WALK04), untouched |

## Family 3a: perm clumps 0x004025f0

New exe-only TUs (callees bind by name, no raw-address call):

| rva | TU | source | binding of its callees |
|---|---|---|---|
| 0x004025f0 | Match/PortTop_004025f0.cpp | re/match/s01/00401690.c (0x004025f0 BYTE-IDENTICAL 211 B, relocheck OK 6/6) | 0x00401630 -> matched, 0x00401690 -> the static in the same source, 0x004b3f90 -> view binding, 0x004c3ac0 -> `Vec3Magnitude` (Math/Vec3.cpp, the RW sqrt LUT), 0x00402240 / 0x00402590 -> stand-ins |
| 0x00401630 | Match/PortTop_00401630.cpp | re/match/s01/raw/00401630.c (BYTE-IDENTICAL 87 B, relocheck OK) | sprintf 0x004a2b60 -> CRT vsprintf, 0x004b3bf0 -> matched |
| 0x004b3bf0 | Match/PortTop_004b3bf0.cpp | re/match/s18/004b3bf0.c (BYTE-IDENTICAL 98 B) | stream open / find chunk / close -> Match/PortTop_RwStream04F.cpp, 0x004e7420 / 0x004c0e50 -> RW bindings |

0x00401690 (the slot bounding box) is the C2 reconstruction in 00401690.c (match.py 60.2%). Its only residual is the
unroll factor of the vertex loop: x8 under 9176, x4 in the original. It is not a matched body.
backfill_port.py lists it as `called` because the body runs. The RE axis does not change.

New RW bindings in LibRw/RwApiBind.cpp:

| rva | binding | basis |
|---|---|---|
| RpClumpStreamRead 0x004e7420 | `MashedRw_ClumpStreamRead_004e7420` -> rw::Clump::streamRead on a StreamMemory at base + position of the knob path's memory stream, position moved by what librw read | both take the stream with the clump header consumed (0x004b3bf0 calls RwStreamFindChunk(0x10) first). Decomp 2026-11-09 |
| RpClumpForAllAtomics 0x004e66d0 | `MashedRw_ClumpForAllAtomics_004e66d0`: librw list walked backwards, stop at the first callback returning 0 | RW's 0x004e7420 links each atomic it reads at the list head, 0x004e66d0 (re/match/s21) walks from the head, and librw appends. The callback gets the librw atomic. No knob-path caller yet |
| RwFrameUpdateObjects 0x004c0e50 | `MashedRw_FrameUpdateObjects_004c0e50` -> rw::Frame::updateObjects | decomp: root +0xa0, privateFlags +0x03 & 3 test, dirty list at RwGlobals +0xbc, \|= 3 / \|= 0x0c. librw frame.cpp does the same. The matched body reads RwGlobals +0xbc, which the knob record lacks |
| 0x004b3f90 | `MashedRw_ClumpFirstAtomic_004b3f90`: the first atomic in RW order as an RW-layout view | decomp: RpClumpForAllAtomics(clump, 0x004b4070, &out), and 0x004b4070 is `*data = atomic; return 0`. 0x00401690 reads atomic +0x18 -> geometry +0x14 / +0x5c -> morph +0x14. librw has numVertices at +0x14 and MorphTarget::vertices at +0x14, but Atomic::geometry at +0x14 and Geometry::morphTargets at +0x48. The view carries those fields (static_asserts in the TU). One view per call, kept until process exit |

Sources edited for C++ only: the object code is unchanged, and `match.py --bytes` / `relocheck.py` were re-run after each edit.
- re/match/s01/raw/00401630.c: declared `void` but the original leaves 0x004b3bf0's result in eax, which 0x004025f0
  stores as the slot clump. Now `int ... return FUN_004b3bf0(...)`. The sprintf prototype is variadic (the call
  passes 3 arguments to a 2-parameter prototype). The g2s cookie lines, which read an `unaff_retaddr` and an extern
  cookie, are dropped: /GS emits the cookie. The preamble's `bool` / `wchar_t` / `true` / `false` are guarded for C++.
  Result: BYTE-IDENTICAL 87 B, relocheck OK.
- re/match/s01/00401690.c: `extern Slot DAT_00636578[13]` -> the MGLOBALA preamble form. 0x004025f0 still
  BYTE-IDENTICAL 211 B, relocheck OK 6/6. 0x00401690 still 60.2%, identical to before the edit.

Stand-ins added: 0x00402240 / 0x00402590 (log the first call, return 0). Each builds a one-atomic clump from
FUN_00401fe0 / FUN_004022a0, which write RW-layout geometry and material fields (+0x2c / +0x30 / +0x34 / +0x5c,
material +0x04 / +0x0c..+0x14) through RpGeometryCreate 0x004e8c70 / RpMaterialCreate 0x004e7fa0 and four more RW
geometry calls with no librw binding. They also call 0x004c1040, which has no source. Slots +0x40 / +0x50 stay 0, the
value they had before. The matched teardown 0x004015a0 (04C) calls RpClumpDestroy on all 13 x 12 slot entries, and the
binding skips 0.

Knob boot (exe `2038d5a6`): exit 0 after 20.7 s, `0x00402750 AppInitialiseOnBootup (matched) returned 1`,
`WinMain 0x00492370 returned 0 after 3000 frames`. The log shows all 13 `map_%d.dff` (ids 0, 2, 3, 11, 25, 26, 30, 33,
34, 36, 37, 38, 39) read from the piz. Each is one atomic, and librw consumed the chunk size minus the 12-byte header
(map_0: 7412 bytes from piz, 7400 read). Each one's view has its geometry (map_0 190 vertices ... map_33 274). At exit
the teardown issues 156 RpClumpDestroy calls (13 x 12), and the 13 clumps are real librw objects now.

## Item 3: sun texture release 0x0048bc10

Match/PortTop_0048bc10.cpp = re/match/s14/0048bc10.c (BYTE-IDENTICAL 13 B): `RwTextureDestroy(DAT_00722134)`, with a
C++ overload for the undefined4 -> `int *` argument (the 0x0048bbe0 pattern). The matched 0x0040bd00 (04C) now binds it
in place of `PortTop_StandIn04C_0048bc10`. 0x0040bd00 calls it before its four RwTexDictionaryDestroy calls, so
rw::Texture::destroy drops the count to 0 and unlinks the texture before the dictionaries go. Knob log (exe `3115efc0`):
`RwTextureRead("sun"): sun.png from piz, 128x128 -> rw::Texture 0F6EB998` at boot, `RwTextureDestroy(0x0F6EB998) ->
rw::Texture::destroy` at exit. No 0x0048bc10 stand-in line appears.

## Acceptance

| check | family 3a (exe `2038d5a6`) | item 3 (exe `3115efc0`) |
|---|---|---|
| knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | exit 0, 20.7 s | exit 0, 32.4 s |
| 12 nav-demo captures, knob ON / OFF / OFF / ON, 3 sets | f3 / f3b / f3c: 12/12 each, equal to the 04E list | s3 / s3b / s3c: 12/12 each, equal to the 04E list |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref | same |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref | same |
| rva-lint | `OK -- no NEW duplicate bodies (1 known)` | same |
| port column (`backfill_exe_file.py`, then `backfill_port.py`) | 4 rows none -> called: 0x004025f0, 0x00401630, 0x004b3bf0 (Match/PortTop_*.cpp), 0x00401690 (Match/PortTop_004025f0.cpp, C2 body). Totals none 5209, body 553, called 389 | 1 row: 0x0048bc10 none -> called. Totals none 5208, body 553, called 390 |

BINDINGS.tsv (`update_bindings_top04h.py`, runs the 04G updater first): 415 rows, of which matched 132, stand-in 108,
port 42, bound 39, under-stand-in 31, rw 30, shim 17, missing 12, crt 4.

**Knob-ON capture flake (PORT-TOP-KNOBON-CAPFLAKE): no recurrence.** All 12 knob-ON nav runs in this step are
byte-identical to the 04E list. Boot-movie timing in those runs: `empire.mpg` 172 to 344 ms (13 to 26 frames). 04G's
outlier took 641 ms.

The first family-3a build failed to compile: the raw 00401630.c preamble typedefs `bool` / `wchar_t`, which is the
preamble guard above. The rebuilt exe is the evidence. Every build ran with `mashed_lock.py status` showing no
PRIORITY-HOLD or SOLO (the first build waited about 30 min for train `20261109_142615`). Every game launch ran through
`run_all_04h.py` inside `mashed_lock.py run --label PORT-TOP-04H`, inside `test-slot`. The f3 run's slot waited 10 min
for memory and then started under the cap. Roots: `D:/MashedWT/PORT-TOP-04H_root` / `_rootK` (copies of the 04G roots).

## Handed back (for the next row)

1. **0x004881d0 (item 2) and its sibling 0x004880a0.** The FX dictionary is loaded, so "lensflare-fs8" resolves. What
   blocks the move is the object it builds (re/match/s14/004881d0.c):
   - `FUN_00534b60(0x200, 0x10000087, 0)` is RpPTankAtomicCreate. librw has no PTank, and the exe's
     `PTankNormaliseFlags` (Render/RfxEmitter.cpp) is the flag part only.
   - `OBJ(h) = *(char **)(DAT_007dc57c + h)` reads the PTank plugin offset at 0x007dc57c, which RpPTankPluginAttach
     writes inside 0x00493640. On the knob path 0x00493640 is the plugin-set shim, so 0x007dc57c is 0 and OBJ(atomic)
     would dereference the atomic's first dword.
   - It writes atomic +0x18 -> geometry +0x5c -> morph +4 (bounding sphere) and geometry +0x20 -> material[0]
     (RpMaterialSetTexture 0x004e8090, matched rwl02, raises the texture count at RW +0x54).
   - 0x004e7e30 (RpAtomicSetFrame, matched rwl02) calls 0x004c0740, whose binding takes camera records only.

   Route: an RW-layout PTank atomic record (the camera-record pattern) from a 0x00534b60 binding, the 0x00493640 shim
   writing the record's plugin offset to 0x007dc57c, a generic object case in the 0x004c0740 binding, and
   RpMaterialSetTexture on the record's material view. Then 0x00489250 (its teardown) needs RpAtomicDestroy 0x004e6920
   (C2, no source, so it needs a binding) plus RwFrameDestroy (bound). No exe renderer reads 0x007030a8 / 0x007030ac,
   so the objects would be inert on the knob path. This is a layout decision for the lead: a PTank record shape the
   original's PTank code never sees.
2. **Family 3, the rest.** The powerup roots 0x0045bae0 / 0x00418980 and the panels: 04G's closure table still holds,
   since this step changed no node of those trees. Two notes for them: RpClumpForAllAtomics is now bound (its callbacks
   get librw atomics, so a callback that reads RW atomic fields needs a view like 0x004b3f90's), and RpClumpStreamRead
   is now bound (DFF loads through 0x004b3bf0 work).
3. **0x00402240 / 0x00402590** (perm slots +0x40 / +0x50): they need librw bindings for RpGeometryCreate 0x004e8c70,
   RpMaterialCreate 0x004e7fa0 and 0x004e8910 / 0x004e8940 / 0x004e8640 / 0x004e8a10. FUN_00401fe0 / FUN_004022a0
   also write RW-layout geometry fields directly, so they would need a geometry record, or a port that builds the
   triangle on librw.
4. **Family 5, the six teardown callees.** Each waits for its loader:

   | teardown | reads | loader | loader state |
   |---|---|---|---|
   | 0x004014b0 | world 0x00636560, lights 0x00636570 / 0x0063656c, clump 0x00636564 | 0x00401000 (cup.dff, RpWorldCreate, 2 RpLightCreate) | 04G level-3 stand-in under 0x0040bbb0. Its DFF read now has a binding. RpWorldCreate, RpLightCreate and the light setters do not |
   | 0x00489250 | 0x007030a8 / 0x007030ac +4 | 0x004881d0 / 0x004880a0 | item 1 above |
   | 0x0041e0d0 | 0x0063d610 table +4 | panels | stand-in |
   | 0x0041ffb0 | DAT_0063e490 via 0x004b4880 | panels | stand-in |
   | 0x004196f0 (thunk 0x004189e0) | 0x0063bde0 table +4 | 0x00418980 powerups | stand-in |
   | 0x00412010 | DAT_0063bb38[i] +0x3c | under 0x0045b930 | stand-in |
5. Still open from 04G: 0x0040bbb0's 18 level-3 stand-ins. 0x00401000 is the one closest to moving, since its DFF
   read is bound now.

## Raw data (D:/MashedWT/PORT-TOP-04H_data)

| file | sha256 |
|---|---|
| build1.log / build2.log | b0d8497cc74e864b15583e803d96ca1802350e8f804f016e00cfa2bb03a931d4 / 9f17a8a8dab62396a77ad6f12a31bf92654c7b5cf86d07a1e7665e141ec9f929 |
| run_all_04h.py (driver) / cmp_caps.py | 0026dbc23e0617298e4d0903470521b6bdfe07b0e3bf18db07cf944babf5b2c5 / 97016dc8612968fb38875d264f446dfccba5e81dd03272f046da0e34929783b3 |
| f3_run.log / f3b_run.log / f3c_run.log | 46eccff22e16a02a66b86e5941f8a462b27317f1328302916a8d0b6e8065534a / 39fc9a5244f24e53f58d0cb62faf856ac63b883d94759659ce8d24b42aab4de5 / b4fbc43380078cd39c92ad16899c9979ead63c2ef434ee9e62e9bfb14eec93ad |
| s3_run.log / s3b_run.log / s3c_run.log | 5af561a1e1a5efb9fc3d2d478517eced4f26c7c936a5fac3530386f02f867197 / df2f2a1da201d5109dc70e2b5a0d61ccd50074ca886ee5a45508e144164b115a / 03896ccfb81d0cffafab1278f3d53dd21f2ee81918f3b27890154b311ba06983 |
| f3/knob_on_1.log / s3/knob_on_1.log | c0124ec9d9d939ef4fad6978ec6c0d0e6ae72414655d0e79217089a460b9f93b / 209600b162d487962bd1f7f35315f85ab006c07548d5680d2e0f9506df624be0 |
| f3/arms/H_DEF.step.csv = s3/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| f3/navdemo_knob1.log / s3/navdemo_knob1.log | fd03673784ec9fdcd886d3ae2172558a48761e72fa4739e9ad62ba79541a1252 / cdd0da0c3a1c4174fe3c1908c3c94a777141fd7dbdd48bb4d6272e82369c7453 |
| hooks_before_f3.csv / hooks_before_s3.csv | the hooks.csv copies the port deltas were computed against |
