# RESULT PORT-TOP-04K: TOP-04 split (k), the 0x004b52c0 / 0x004b52f0 CROSS-TARGET move and the startlights loader 0x0041d8b0

Run 44, 2026-11-13. Branch `orch/PORT-TOP-04K` from `orch/integration` @ `e47e10795`. Top-down row (WORKER_RULES):
both builds, `ddkobj.py check`, knob boots, self-test. G-CTRL / B / 12 captures not run (they run with the knob off and
cannot see the change). No menu code changed, so no menu walk.

**Verdict: GREEN for what landed. Everything else in the row title is HANDED BACK with its blocker (section 3).**

The row was planned from RESULT_PORT-TOP-04J (2026-11-09). Since then POWERUPINIT-A..C, ATOMIC-VIEW / -2 and the L-124 RW
core rows changed the ground, so the plan was re-checked callee by callee before any edit.

## 1. What landed

| rva | before | now | TU | source |
|---|---|---|---|---|
| 0x004b52c0 (.asi) | hand body `Or4b52c0` (Util/PromoLoop_sessionB.cpp) | matched body, export `Or4b52c0` kept | Match/MatchS18_004b52c0.cpp (asi list) | re/match/s18/004b52c0.c, BYTE-IDENTICAL 35 B, relocheck OK 0/0 |
| 0x004b52f0 (.asi) | naked hand body `Thunk4b52f0` | matched body, export `Thunk4b52f0` kept, callee 0x004b52c0 called through its original address as before | Match/MatchS18_004b52f0.cpp (asi list) | re/match/s18/004b52f0.c, BYTE-IDENTICAL 16 B, relocheck OK 1/0 |
| 0x004b51d0 (exe) | no body | `Orig_004b51d0` | Match/PortTop_004b51d0.cpp (exe list, ORIGTOP-ONLY) | re/match/s18/004b51d0.c, BYTE-IDENTICAL 98 B (`--bytes`; the 94.6% instruction score is Ghidra's short size), relocheck OK 2/0 |
| 0x0041d8b0 (exe) | stand-in `PortTop_StandIn04E_0041d8b0` | `Orig_0041d8b0`, called by the matched 0x00402750 | Match/PortTop_0041d8b0.cpp (exe list, ORIGTOP-ONLY) | re/match/s04/0041d8b0.c, BYTE-IDENTICAL 96 B, relocheck OK 11/0 |

**CROSS-TARGET move.** One RVA, one body per target: the .asi now compiles the matched sources, so 0x004b52c0 /
0x004b52f0 have the same source text in every target that has a body for them. Behaviour change in the .asi only: the
hand `Or4b52c0` stored the flags unchanged when the third argument was 0. The matched source stores `flags & ~mask`
there (re/match/s18/004b52c0.c, the `else` arm), as the original does (BYTE-IDENTICAL). This settles 04J's [UNCERTAIN]
on `Or4b52c0`: the hand body lacked the clear arm. No exe body changed for these two RVAs: the exe keeps the bindings
`MashedRw_AtomicGeometryFlags_004b52f0` / `_004b5300` (LibRw/RwApiBind.cpp, librw atomics). Their RW-core routes
(`RwCore_004b52f0` / `RwCore_004b52c0` / `RwCore_004b5300`) are on `orch/RWC-FRAME-CLUMP`, which is working now, so this row
leaves RwApiBind.cpp untouched.

**0x0041d8b0 callees, all existing, no new RW binding (L-124):** 0x0042a5d0 = `FUN_0042a5d0` (Match/MatchS06_0042a5d0.cpp),
0x004b6520 = `Orig_004b6520`, 0x004b51d0 = `Orig_004b51d0` -> 0x004b3fc0 = `Orig_004b3fc0` (librw atomics through the
RpClumpForAllAtomics binding) and 0x004b5190 = `MashedRw_AtomicPartCode_004b5190` (POWERUPINIT, user-data element as
int), 0x004b52f0 = `MashedRw_AtomicGeometryFlags_004b52f0`. The clump's frame is read at clump +0x04, which is librw
`object.parent` (asserted in RwApiBind.cpp since 04H). Its teardown 0x0041da80 (RpClumpDestroy(DAT_0063d57c)) was already
matched since 04C and now destroys a real clump.

## 2. Acceptance

| check | VS2022 exe `70e0b864` | object build `fc5acd36` |
|---|---|---|
| build | rc 0, rva-lint `OK -- no NEW duplicate bodies (237 known)` | rc 0 |
| `ddkobj.py check` | n/a | IDENTICAL 1194 / 1221. 0x004b51d0 and 0x0041d8b0 both IDENTICAL. The 12 DIFFERENT (0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00) are not units of this row. 4 refusals (wrapper-redefines-globals), also not this row's |
| knob boot `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000` | exit 0 after 48.4 s, `WinMain 0x00492370 returned 0 after 3000 frames` | exit 0 after 80.6 s, same line |
| self-test (below) | PASS | PASS |

The first build failed rva-lint DUP-IN-TARGET 0x004b51d0: the scanner bound a typed call-through wrapper in
PortTop_0041d8b0.cpp as a second body. Both wrappers were dropped. The callees now bind by name (extern "C", cdecl, every
argument a 4-byte word), as the POWERUPINIT TUs do.

**Self-test (knob log, both builds).** Checks that the matched body ran end to end on the librw objects:
1. `startlights.dff`: 5838 bytes from the piz, `RpClumpStreamRead -> rw::Clump::streamRead 10FA3D90 (5826 bytes, 6 atomics)`.
2. The 0x0041d8b0 loop calls 0x004b52f0 on DAT_0063d55c[0..5]. The binding logs only for a non-null atomic, and the log
   has 6 `0x004b5300(atomic ...)` lines for 6 different atomics, each `flags 0x0000000F -> 0x0000004F` (mask 0x40, set 1).
   So all 6 slots are non-null and distinct. With 6 atomics in the clump, the 0x004b5190 part codes map them one to one
   onto slots 0..5: no index outside 0..5, and no write before or after the array.
3. No `no user-data array` / `no element` line (MashedRw_AtomicPartCode_004b5190 returns -1 only with one of them).
4. AppDestroy: `RpClumpDestroy(0x10FA3D90) -> rw::Clump::destroy` (the matched 0x0041da80 on DAT_0063d57c).
5. The process exits 0 after 3000 frames.
Result: **PASS 5 of 5 on VS2022 and on the object build**.

**.asi Frida diffs: not runnable, existing gap.** `run_diff.py or_4b52c0` and `run_diff.py thunk_4b52f0` both stop with
FATAL: their registry arg_types (`deref_struct_set`, `thunk_cond_or`) have no handler in diff_template.js. That predates this
row: the hand bodies were not Frida-testable either. The evidence for the two moved .asi bodies is static: match.py
BYTE-IDENTICAL plus relocheck OK. Also, `or_4b52c0`'s registry seed is 0x00 with `observe` at +0x8, so its clear-arm vector
`[0xABCD, 0]` could not tell the hand body from the matched one anyway. Adding the two handlers is harness work for its own row.

Not done here: the hooks.csv `port` / `exe_file` backfill for 0x004b51d0 and 0x0041d8b0. That is a tracker edit, so it is left
for re-classify / the merge train. No C4 claimed (all four RVAs are already C4 in hooks.csv).

Machine rules: every build ran with `mashed_lock.py status` free. Every game launch ran inside `test-slot`, then inside
`mashed_lock.py run --label PORT-TOP-04K`. The private root is D:/MashedWT/PORT-TOP-04K_root (a copy of 04J's). The shared
`original\mashed_re_dev.asi` was not touched (`MASHED_NO_DEPLOY=1`). Only the PIDs knob_boot.py spawned were run, and none
was killed.

## 3. Handed back, with blockers (current state, callee by callee)

| target | match | blocker |
|---|---|---|
| 0x0041c100 KCPanel, 0x0041db90 RacePanel | 100% | write `atomic +0x18 -> geometry +0x20 -> material[0] +0x04` directly. The atomics 0x004b51d0 stores are librw rw::Atomic (geometry at +0x14). These need an RW-layout atomic / geometry view with +0x20 = librw `matList.materials` (librw Material color @ +0x04), and 0x004b5190 / 0x004b52f0 bindings that resolve views. That is an extension of the librw fallback for a new caller, which L-124 forbids ("add no new librw binding"). On the core route they need nothing new (all callees matched or RwCore_). Recommendation: land them on the core route once RWC-FRAME-CLUMP gets the core boot past powerup init. |
| 0x0041def0 stopclock2 + its teardown 0x0041e0d0 | 0x0041def0 BYTE-IDENTICAL (`--bytes`, 96.5% by instructions), 0x0041e0d0 100% | 0x0041def0 reads `*(atomic +4) + 0x40` (the RW frame's modelling-matrix position). On a librw rw::Frame the matrix is at another offset (object 8 B + inDirtyList 8 B + objectList 8 B before it), and 0x004c1340 has no librw binding. The teardown can only land with its loader: it reads +4 of each pointer of 0x0063d610 with no null check. Same L-124 position as the row PORT-OT-BIND-41DEF0 (blocked, superseded by L-124). |
| 0x0041eaa0 + teardown 0x0041ffb0 (-> 0x0041ec00 x4) | 100% / 100% / 100% | with DAT_00636acc == 0 (the knob path's state, 04C note) 0x0041eaa0 calls 0x004b48e0 (shadow camera + raster), and that needs RwCameraSetProjection 0x004c1c10 and 0x004c5a00. Neither has a librw binding (new binding, L-124). |
| 0x004196f0 teardown | 100% | its loader 0x0041a060 (behind thunk 0x00418980) has **no re/match source**, so it has no matched body. The teardown reads +4 of each 0x0063bde0 pointer with no null check, so it stays a stand-in until the loader lands. |
| 0x0041a1e0 | 91.7%, not byte-checked here | unmatched: equiv.py (L-126) first |
| family 3: 0x00402240 / 0x00402590 | 100% / 100% | RpGeometryCreate 0x004e8c70, RpAtomicCreate 0x004e67b0, RpAtomicSetGeometry 0x004e68a0, RpClumpCreate 0x004e6d80, RpClumpAddAtomic 0x004e6f80 and RpGeometryDestroy 0x004e8ea0 exist only as RwCore_ bodies, with no librw binding (new bindings, L-124). Their builders 0x00401fe0 / 0x004022a0 are C3 with no matched body (0x00401fe0: 350 differing bytes, 04J), so equiv.py / PROVISIONAL first. Recommendation: the core route, after RWC-FRAME-CLUMP. |
| 0x0045bae0, 0x00412010 | | already landed (POWERUPINIT-A) |
| 0x00420d00 | | already matched (PORT-OT-BOOT) |

So the rest of family 2 and all of family 3 wait on one question: whether the core boot reaches AppInitialiseOnBootup's
panel and perm loaders (RWC-FRAME-CLUMP, working, "crash loop from powerup init"). Until then, each of these targets needs a
new librw binding, which L-124 rules out. No user decision is needed while L-124 stands. A decision would be needed only to
reopen route (A) for these loaders.

## Raw data (D:/MashedWT/PORT-TOP-04K_data)

| file | sha256 |
|---|---|
| build1.log (VS2022 evidence build) | 34c9869f572180162b74e63c946a69f351073de115e55fd37c2c8ea320ed5823 |
| build_ddk.log (object build + ddkobj check) | 167c3f9b2eb9c556f3308bbda5293562d7789cf24e6178790a93ad8e142ae18b |
| knob_on_1.log (VS2022 knob boot) | 31bbe91a263bc15e29fd7fc52a8779a4da1a7aad5a2b2784d11f7bfe784828ba |
| ddk_knob_on_1.log (object build knob boot) | 3a56c0cd4c0fc990a09969025ef9d4a2a8b190e7ae21dac6f04dfbd576da3ec0 |
| diff_or_4b52c0.log / diff_thunk_4b52f0.log (FATAL, no handler) | 0db4c7ffa1ac70b1a41e1e9bcc63859da3bf3bba018332974ab1c59002604cfc / a5a7eaaf5667b08a298400ad8d093ef638c9dc6a3aee73357224dd4458e06c03 |
| mashed_re.exe / mashed_re_dev.asi / mashed_re_ddk.exe | 70e0b864dbe11845343bce42d543176d2d116cefa23a73c93f475b52dd5c6b58 / f6b9a24da2eb24fd83eeb5ac963970456b89146bec932cc4188822531d146b76 / fc5acd36f61c782ea37e0051afad4129c6926475f7658a6f7637f1e6d305f47d |
