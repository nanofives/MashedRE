# RESULT PORT-TOP-04G: 0x00402750 loader families off their stand-ins under MASHED_ORIG_TOP (L-96 top-down lane)

Branch `orch/PORT-TOP-04G` from `orch/PORT-TOP-04F` @ `accad7e60`, with `orch/integration` @ `c5efbd32d` merged in
(`752cabd0a`, `git -c merge.autoStash=false`). Family 1 commit `0a5f6ca06` (exe `a3c80f4d`).

Family 2 commit `7736239de` (exe `6fa41b1166947833`).

Verdict: HANDOFF at a family boundary. Families 1 and 2 landed GREEN on the brief's acceptance: the sprite lookups
0x0040bb50 / 0x0040bb30 and sfx.piz 0x0040bbb0 now run their matched bodies on the knob path. Family 4 (fonts) stays
on its stand-ins: librw has no Rt2d and the Rt2d tree is not matched (below). Families 3 (panels, powerups, perm
clumps) and 5 (the six teardown callees, which read family-3 state) are handed back with a sized plan.

0x00402750's 58 callees now: matched 15 (04F: 12), bound 6, stand-in 37 (04F: 40). Stand-ins in
Match/PortTop_00402750_Pieces.cpp: 30 (04F: 33), 28 reached in the knob boot (04F: 31). 0x0040bbb0 adds 18 level-3
stand-ins of its own (Match/PortTop_0040bbb0.cpp), each reached once.

## Scope landed vs handed back

| item from the brief | state |
|---|---|
| 1. sprite lookups 0x0040bb50 / 0x0040bb30 | **done** (family 1, `0a5f6ca06`): one TU per lookup in both targets, the .asi hand bodies removed, rva-lint OK, run matched on the knob path |
| 2. sfx.piz 0x0040bbb0 | **done** (family 2, `7736239de`): matched body, four dictionaries from sfx.piz, 18 level-3 stand-ins. Captures: 12/12 in 2 of 3 sets (below) |
| 3. panels, powerups, perm clumps 0x004025f0 | **handed back** (below): each tree has 10 to 90 nodes with RW frame / clump / atomic calls and C2 nodes |
| 4. fonts 0x00427ca0 / 0x004275d0 | **left on the stand-ins. Neither port nor librw binding fits this step** (below) |
| 5. six AppDestroy teardown callees | **handed back**: each reads state a loader of family 3 (or 0x004881d0) creates, and family 3 is not moved |
| 0x004924f0 | left on its stand-in (row PORT-TOP-04D-WALK04) |

## Family 1: sprite lookups 0x0040bb50 / 0x0040bb30

- **Sources.** `re/match/s02/0040bb50.c` and `0040bb30.c` declared the forwarders `void`, so a C++ caller could not use
  the result, which the original passes through in eax (0x00402750 stores `FUN_0040bb50("SemiC")` into DAT_008aa318).
  Both now `return FUN_004c5c00(...)` with `undefined4 *` / `char *`. That form is still byte-identical:
  `match.py --bytes`: `21 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL`, and `relocheck.py`:
  `OK checked=2 unchecked=0`, for both (the void form gave the same bytes, so the C4 rows' evidence still holds).
- **One TU in both targets.** `Match/MatchS02_0040bb50.cpp` (export `SpriteLookupC`, not installed, as the old body)
  and `Match/MatchS02_0040bb30.cpp` (export `Search40bb30`, `RH_ScopedInstall(Search40bb30, 0x0040bb30)`, registry key
  `search_40bb30` unchanged). The callee 0x004c5c00 binds per target: .asi -> the original address (it carries the
  installed JMP into Util/PromoLoop_sessionB.cpp Search4c5c00, the sibling Match/MatchS02_0040bb90.cpp does the same),
  exe -> `MashedRw_TexDictionaryFindNamedTexture_004c5c00` (rw::TexDictionary::find, null dictionary returns 0).
  The hand bodies left Frontend/SpriteCluster.cpp (SpriteLookupC, not installed since 2026-05-24) and
  Util/PromoLoop_sessionB.cpp (naked Search40bb30). rva-lint: `OK -- no NEW duplicate bodies (1 known)`.
- **Knob path.** Match/PortTop_00402750.cpp binds `FUN_0040bb50 -> SpriteLookupC`, `FUN_0040bb30 -> Search40bb30`;
  the two stand-ins left Match/PortTop_00402750_Pieces.cpp. With 0x0040bbb0 still a stand-in, the dictionaries are 0
  and both lookups return 0 (DAT_008aa318 / 314 and the lens-flare texture stay 0, as before).
- Not run: an .asi path1 diff of `search_40bb30` (the body went from naked asm to the compiled matched source). The
  worker rules forbid redeploying `original\mashed_re_dev.asi`, and the brief's acceptance is the exe's.

Acceptance, exe `a3c80f4da8c88fcd` (build1, `MASHED_NO_DEPLOY=1`, both targets OK, rva-lint OK, asm-lint OK):

| check | result |
|---|---|
| knob-ON boot (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | exit 0 after 40.4 s, `0x00402750 AppInitialiseOnBootup (matched) returned 1`, `WinMain 0x00492370 returned 0 after 3000 frames`. 0x00402750 stand-ins reached: 29 (04F: 31) |
| 12 nav-demo captures, runs knob ON / OFF / OFF / ON | 12 / 12 byte-identical across the four runs and equal to the 04E / 04F list (`cmp_caps.py`) |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref |
| port column (`backfill_exe_file.py`, `backfill_port.py`) | 2 rows: 0x0040bb30 none -> called (Match/MatchS02_0040bb30.cpp), 0x0040bb50 body -> called (Match/MatchS02_0040bb50.cpp; exe_main.cpp stays listed beside it, from its comments) |

## Family 2: sfx.piz 0x0040bbb0

`Match/PortTop_0040bbb0.cpp` (exe only) compiles `re/match/s02/0040bbb0.c` (match.py 100.0%, regs 100.0%). Its callees:

| callee | binding |
|---|---|
| 0x00495280 / 0x004952f0 piz mount / unmount | the piz file system (Match/PortTop_00402750_Pieces.cpp, 04E) |
| 0x0042a6b0 TXD search + load | exe body Match/MatchS06_0042a6b0.cpp, now with exe callees: 0x004b3d80 -> `Orig_004b3d80` (Match/PortTop_004b3d80.cpp), 0x004b3d20 -> stand-in |
| 0x0042a530 -> 0x0042a470 path search | exe bodies Match/MatchS06_0042a530.cpp / 0042a470.cpp. 0x004b5350 (-> RtFS file-exists 0x00550b00) -> `PortTop_FsFileExists_004b5350` (Match/PortTop_RwStream04F.cpp: mounted piz, then the disk under original/) |
| 0x0048bbe0 | matched, new Match/PortTop_0048bbe0.cpp = re/match/s14/0048bbe0.c: `RwImageSetPath("")`, `DAT_00722134 = RwTextureRead("sun")` |
| 0x0040bb30 (scorch, wfall, RWObjShad) | matched, Match/MatchS02_0040bb30.cpp (family 1) |
| RwTextureRead 0x004c5cb0, RwTexDictionarySetCurrent 0x004c5c80 | LibRw/RwApiBind.cpp (04F / TOP-03) |
| RwTexDictionaryAddTexture 0x004c5bc0 | **new** in LibRw/RwApiBind.cpp. re/match/s19/004c5bc0.c unlinks the texture from its old dictionary and links it at the list head (rwLinkListAddLLLink). librw `rw::TexDictionary::addFront` does the same. Reached only when `RwTextureRead("test")` finds a texture. It did not: there is no test.png / test.bmp |
| printf (`FID_conflict__wprintf`), exit 0x004a332b | `PortTop_Printf04G` (to the log), `PortTop_Exit04G_004a332b` (CRT exit). The "cant open sfx dic" branch only, not reached |
| 0x00401000 0x00413b80 0x004210b0 0x004759b0 0x00475a00 0x00476390 0x004775b0 0x00477e40 0x00485d90 0x004862d0 0x00487e00 0x00489290 0x0048a460 0x0048a830 0x0048ae00 0x0048e820 0x0048eac0 0x0048ff20 | stand-ins `PortTop_StandIn04G_<rva>` (log the first call, do nothing). The reason for each is in the TU and in BINDINGS.tsv: DFF / clump / atomic builders (RpClumpStreamRead 0x004e7420, RpAtomicCreate tree), .asi-only bodies, rows with no matched source (0x00475a00 C2, 0x0048e820 C2, 0x00413b80) |

Enabling changes:
- **RwGlobals string functions.** 0x0042a470 / 0x0042a6b0 call `strcpy` / `strcat` through RwGlobals +0xcc / +0xd4
  (re/match/s06/0042a470.c, 0042a6b0.c). The knob path's RwGlobals record (LibRw/RwApiBind.cpp, 04D) now carries them.
  Only these two slots are filled: they are the two the sources show. The record used to be installed after
  0x00402750 returned, and 0x0040bbb0 runs before piece 4. So `MashedRw_InstallRwGlobals` now also runs before the
  matched 0x00402750 (0x007d3ff8 was 0: the record goes in with its Im2D fields zero) and after piece 4 (the hand init
  points 0x007d3ff8 at the B15 device again: the device's +0x00..+0x33 are copied and the record goes back). The third
  call, after 0x00402750, finds the record in place.
- **One argument where the body takes three.** 0x0042a6b0 calls 0x004b3d80 with one argument, and the body passes its
  2nd / 3rd to RwStreamFindChunk as the length / version out pointers. In the original those are whatever the caller's
  stack holds. Declared deviation: the exe forwarder passes 0 / 0, so the find-chunk binding writes neither.
- **rdata seed.** `gen_rdata_seed.py` also seeds a `MGLOBAL(void *, ...)` whose address the source passes
  (`&PTR_DAT_005cf4a8` in 0048bbe0.c, read-only section only): 1 row, 0x005cf4a8 `sun`. The 34 earlier rows are
  unchanged (regenerated from the same 7 sources plus 0040bbb0.c, 0048bbe0.c, 0042a6b0.c, 0042a530.c, 0042a470.c).
  0x005cc4de, the image path, is a NUL byte in the original, the value the zero-filled mapping holds.
- **C++ argument types.** 0048bbe0.c passes an integer literal and `&PTR_...` where it declares `undefined4 *` /
  `undefined4`. The source is unchanged: the TU declares C++ overloads taking those types, as 04F did for 0x00425bc0.
- [UNCERTAIN] `PTR_DAT_005f65b8` (the extension 0x0042a6b0 appends when the name has no '.') is in .data, which the exe
  maps zero-filled, so `strcat(dst, 0)` would fault there. Every name 0x0040bbb0 passes has a '.', so that branch is
  not taken. Missing: a seed of .data pointers, or a binding.

Knob boot (exe `6fa41b11`): exit 0 after 26.3 s, `0x00402750 AppInitialiseOnBootup (matched) returned 1`,
`WinMain 0x00492370 returned 0 after 3000 frames`. The log shows sfx.piz mounted (30 entries), `fx.txd` 823308 bytes
-> 36 of 36 textures, `badges.txd` 165740 -> 23 of 23, `TrackImages.txd` 1961640 -> 39 of 39, `Interface.txd` 354816
-> 30 of 30, `RwTextureRead("sun")`: sun.png 128x128, then the unmount. AppDestroy's matched 0x0040bd00 (04C) destroys
all four dictionaries (`RwTexDictionaryDestroy(0x090396E0 / 0x09039760 / 0x09039700 / 0x09039A80)`, where 04F logged
four nulls). The "sun" texture (DAT_00722134) would be released by 0x0048bc10, a 04C stand-in in that teardown, so it
is not freed at exit.

The first family-2 knob boot exited 0xC0000005 after 12.5 s, inside RwEngineStart, before 0x00402750. Its log shows
`InitD3D9: CreateDevice(HAL/HW,depthA,vsdef) -> HUNG hr=0x80004005` and the REF fallback. That point runs no 04G code.
The re-run (HAL OK) is the evidence above.

Captures (`MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, knob ON / OFF / OFF / ON, three sets f2 / f2b / f2c):

| set | result |
|---|---|
| f2 (right after the gates) | knob run 1: 11 of 12 differ from the other three runs. Knob run 2 and both hand runs equal the 04E list |
| f2b | 12 / 12 identical across the four runs and equal to the 04E list |
| f2c | 12 / 12 identical across the four runs and equal to the 04E list |

So 5 of 6 knob-ON runs and 6 of 6 hand runs are byte-identical to the 04E list. The one outlier: `imgdiff.py` walk_02
vs hand run 1 gives 0 pixels over threshold 16 and a mean absolute difference of 0.1 to 0.2 in the bottom-left grid
cells only. Its log equals knob run 2's except the boot-movie timing: `empire.mpg` took 641 ms there (24 frames). In
the other 11 runs it took 141 to 297 ms. [UNCERTAIN] the cause: which draw in the bottom-left region follows timing was
not traced. Against the brief's acceptance, the 12-capture check passed in 2 of 3 sets.

Default gates (knob OFF), `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `6fa41b11`: all four arms exit 0 and
self-exit (H_DEF 120 s).

| gate | `6fa41b11` | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | same |
| B | `1d5bf4f9` x3 | same |

Build: `MASHED_NO_DEPLOY=1`, both targets OK, rva-lint `OK -- no NEW duplicate bodies (1 known)`, asm-lint OK.
Port column (`backfill_exe_file.py`, then `backfill_port.py`): **5 rows**: 0x0040bbb0 none -> called
(Match/PortTop_0040bbb0.cpp), 0x0048bbe0 none -> called (Match/PortTop_0048bbe0.cpp), and 0x0042a6b0, 0x0042a530,
0x0042a470 body -> called (Match/MatchS06_*.cpp). Totals: none 5213, body 553, called 385.
BINDINGS.tsv: `update_bindings_top04g.py` (runs the 04F updater, then writes 34 rows), 404 rows: matched 128, stand-in
108, port 40, bound 39, under-stand-in 31, rw 27, shim 16, missing 12, crt 3.

## Machine rules

Every build ran with `mashed_lock.py status` showing no PRIORITY-HOLD or SOLO. The family-2 build waited about 40 min
for train `20261109_133756`. Every game launch ran through `run_all_04g.py` inside
`mashed_lock.py run --label PORT-TOP-04G`, inside `test-slot`. Roots: `D:/MashedWT/PORT-TOP-04G_root` / `_rootK`
(copies of the 04F roots).

## Family 4: fonts 0x00427ca0 / 0x004275d0 stay on their stand-ins

Neither route fits this step:
- **librw binding: no target.** librw has no Rt2d (its `src/` holds charset.cpp, the debug bitmap font, and no 2D
  toolkit: no font, path or brush object), so there is nothing to bind 0x00552b60 .. 0x00556e90 to.
- **port: the tree is not matched.** 0x004275d0's tree (closure_04c.py, depth 8) has 22 nodes: 6 with no source
  (0x00552920 C2, 0x00553cf0 C2, 0x00552b50, 0x00554200, 0x005c4ad0, 0x005c4c60), 6 matched with no body, 6 only in
  .asi TUs. 0x00427ca0 calls 0x00556e90 (C2, no byte match) and the exe copy Match/MatchS06_00427ca0.cpp still forwards
  0x00552b60 / 0x00556d70 / 0x00556e90 to raw addresses. Porting Rt2d means exe bodies for C2 RenderWare toolkit
  functions (RE-axis MATCH rows first) plus a renderer for its glyph quads, a separate row.
- Recommendation: a MATCH row for the Rt2d C2 nodes (0x00552920, 0x00553cf0, 0x00554390, 0x00556e90), then an exe
  Rt2d port drawn through the exe's Im2D path. Until then the hand fonts (piece 5) draw the text and the knob path
  keeps the two stand-ins.

## Handed back (for the next row)

1. **Family 3: panels, powerups, perm clumps.** Sized with closure_04c.py (depth 8, unique nodes by state):

   | root | exe | asitu | gen | none | rw |
   |---|---|---|---|---|---|
   | 0x004025f0 perm clumps | 1 | 8 | 18 | 7 | 3 |
   | 0x0045bae0 powerups | 20 | 11 | 38 | 15 | 2 |
   | 0x00418980 powerups | 14 | 14 | 43 | 15 | 4 |
   | 0x0041b450 / 0x0041bec0 / 0x0041def0 panels | 11 / 3 / 4 | 2 / 2 / 5 | 16 / 9 / 22 | 1 / 1 / 4 | 2 / 1 / 3 |
   | 0x0041d8b0 / 0x0041c100 / 0x0041db90 / 0x0041a1e0 panels | 3 / 3 / 3 / 5 | 2 / 0 / 2 / 2 | 7 each | 1 each | 0 |
   | 0x0041eaa0 emitters / 0x00420d00 PanelSortInit | 4 / 1 | 4 / 1 | 4 / 1 | 0 / 0 | 7 / 0 |

   0x0041cb10 / 0x0041d6e0 already have exe bodies (check them for raw-address callees before binding).
   The recipe that worked for family 2: run the root's matched body, bind direct callees to exe bodies / RW bindings,
   and put level-2 stand-ins where a subtree has `none` nodes. The RW calls the 04F scope table names come first:
   RpClumpStreamRead 0x004e7420, RpClumpForAllAtomics 0x004e66d0 (the frame calls 0x004c0b30 / 0x004c0740 are bound
   already). 0x004b3fc0 has no source. The element-array helpers 0x004b5190 / 0x004b51d0 / 0x00543d40 have no body.
   0x004025f0 reads DFFs through 0x004b3bf0 (matched, s18), which needs RpClumpStreamRead.
2. **Family 5: the six teardown callees.** 0x004014b0 (perm clumps of 0x004025f0), 0x00489250 (0x004881d0's
   objects), 0x0041e0d0 / 0x0041ffb0 / 0x004196f0 (panels), 0x00412010 (under 0x0045b930). Each one after its loader.
   0x004881d0 is still a stand-in. Its "lensflare-fs8" lookup now returns a texture (the FX dictionary is loaded), so it
   can move next. Its exe TU Match/MatchS14_004881d0.cpp exists (check it for raw-address callees).
3. **0x0040bbb0's 18 level-3 stand-ins** (family 2 table) and the "sun" texture release 0x0048bc10 (04C stand-in).
4. **Fonts (family 4)**: a MATCH row for the Rt2d C2 nodes first (section above).
5. **The family-2 capture outlier** (5 of 6 knob runs identical): if it recurs, trace what draws in the bottom-left
   region of walk_02..12 and why it follows boot-movie timing.
6. Still open from 04F: 0x004caea0 SetRenderState through librw's cache, 0x004d41e0 identity, 0x004924f0 (row
   PORT-TOP-04D-WALK04), the save restore of 0x007f0a40.

## Raw data (D:/MashedWT/PORT-TOP-04G_data)

| file | sha256 |
|---|---|
| build1.log | f52eb6fd1720a4f0c3b9fb83a788235aa40ccdd0258f15aa7cf172d301895793 |
| f1_run.log | bb98972735894606a71cc77e6092895745c0fa6c1ad0fcb72d243c258f4f127d |
| f1/knob_on_1.log | 6b1b2cf576b9314400d81cef77866f2e223662a06246aa8324c6fe9b67c85d31 |
| f1/navdemo_knob1.log / f1/navdemo_knob2.log | 22db7f1c3cbb8c1214d3bde558e16b6de53d1f328bdecf5f2de49154fd285fad / 98d1c66645053a9fd5ed9f9492e27124cd4b33d07bd296933a2acc9c7e880c06 |
| f1/navdemo_hand1.log / f1/navdemo_hand2.log | 4215e54174ae0a68f8911088ce748c54de793039bc47b45d1ef8690e6a0d4d31 / 664a87b1828cef4a50b357f7d31d41b3fd1d9fda426223d23a81633a94b691d9 |
| f1/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| run_all_04g.py (driver) / cmp_caps.py | 560d6d37af4d6c48c6794556d3efdd95bb3d10c7d8927dc1cc3933c858d9cbd7 / 97016dc8612968fb38875d264f446dfccba5e81dd03272f046da0e34929783b3 |
| build2.log | 74cb54193da71831a5f773e9c3f6aff8c6aeb26494a6dfac0c41c220e30e3051 |
| f2_run.log / f2b_run.log / f2c_run.log | 106064fd743a51ef1ad04b6ea5d6e5e9018cf3667b1add63d91dc7b8278f30ee / c67f3ff1f23a61c236dfa7fd334a4123c22b2f06f8ffb96eda38e09055a4e35f / 5952766927d7ceea57ce67df6b6613dbd512eafc6be5ec990cdc9a495a4f4baf |
| f2/knob_on_1.log | c489fe91e7942a71f2a2cd86a5e06d749d08a7bb712f808b689ac8c30eab018e |
| f2/navdemo_knob1.log / f2/navdemo_knob2.log | 6bf371cb4157e5635e61cee9933beaab29176d977a181ce8beb67866e7a42e61 / 46cf8362e804947f3b622a97f82657bf5cb6008a1dc929eb59e6b10d07e0b01e |
| f2/navdemo_hand1.log / f2/navdemo_hand2.log | ac8ad412b728566cb1fc194d0a7a2eca7044eb07a96be573cd32f3144205d94a / c1ea561b1ef28b0b2a8b155cc9386702f50328df823fefbf2de07fdbaea9ff08 |
| f2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
