# RESULT D4-DC-RFX-CORE (one shared RFX emitter core, L-51 a)

Branch `orch/D4-DC-RFX-CORE` from `orch/integration` @ `0e3d71a75`. PREREG `verify/rfxcore_20261101/PREREG_D4-DC-RFX-CORE.md`
(`88e4064b9`, committed alone). Code `30185a8e0` + `e141ddb88`, then `orch/integration` @ `ca02bde58` merged in (`4a805693d`, clean, run 29 resume
after the 14:23 reboot killed run 28's arm F). Tested build of `4a805693d` (`MASHED_NO_DEPLOY=1`, shared
`original\mashed_re_dev.asi` untouched): `mashed_re.exe` SHA-256
`9edac6fe7a1e043bbde81ba20addb84b7a8a0b3169d48971eeaf4eda4f7cfc20`, `mashed_re_dev.asi`
`a85cbcb32c555961109568a936a610b342dcb3474c983e853fb17b15e2f32214`. Arms F, F2 and G ran on this build.

**Verdict: GREEN.** One body per RVA for the 15 RFX emitter functions, compiled into both targets. The 15 CROSS-TARGET allowlist rows are gone (lint NEW=0, none of the 15 reported). A1: 0 / 722 cases mismatch the original's bytes. F: 15 / 15 Frida path1 GREEN on the shared body. F2 (reported): the inline JMP read back at all 15 sites, 5 / 15 full installer passes, the other 10 are arg types `run_verify_hook.py` cannot call. Default gates unchanged.

## 1. What changed

- `Render/RfxEmitter.cpp` is in BOTH `exe_sources.rsp` and `asi_sources.rsp` (asi line 213 replaces
  `Render\FrameHelpers_q2.cpp`). It holds the one body of each of the 15 RVAs as the exported hook (export names
  unchanged, so `hooks_registry.py` drives them as before) with its `RH_ScopedInstall` (no-op in the exe):
  `0x004769a0 0x004769d0 0x004769f0 0x00476a10 0x00476a30 0x00476a40 0x00476a80` (staging setters),
  `0x00476ae0 0x00476b30 0x00476b90 0x00476be0 0x00476c10 0x00476c60 0x00476cb0` (batch setters), `0x00476d00` (submit).
  The bodies are the .asi C3 bodies on the original's layouts (staging globals at their absolute addresses, which the
  exe's B17 image-pad owns; batch struct by offset; plugin block `*(*(batch + 4) + [0x007dc57c])`).
- Deleted: `Render/FrameHelpers_q2.cpp` (5 bodies) and 10 bodies in `Util/PromoLoop_sessionB.cpp` (one pointer
  comment each, no RVA-first comment, so the scanner binds nothing there).
- Two deliberate differences from the deleted copies, both toward the original:
  1. `0x00476d00`'s +0x20 array copy is the original's `fld dword [0x006924d8]` / `fstp` pair (0x00476d8e / 0x00476d94)
     in inline x87. Both deleted copies made a plain 32-bit copy, which differs from the original for a signalling
     NaN only (x87 quiets it).
  2. The null-pointer defaults `0x00613288 / 0x00613290 / 0x00613294 / 0x006132a4`: the .asi reads the original's
     own `.data` (as the deleted .asi bodies and the original do); under `MASHED_STANDALONE` the same body points at
     the file-backed values, because the exe's image-pad holds 0 there (same as the deleted exe core). Section 4 has
     why the first build used literals in both targets and what that cost.
- The exe's `Rfx::` API (`Render/RfxEmitter.h`) becomes call-throughs onto the 15 exports. `Rfx::PTank` (0x138 bytes)
  and `Rfx::Batch` (0x40) take the original's layouts with `static_assert`s at the cited offsets; the plugin arrays are
  `{ptr, stride}` pairs at +0x4c .. +0x93 per FUN_00535700's jump tables (0x005357f8 / 0x00535818, decoded:
  flag 1/8/0x10/4/2/0x40/0x20/0x80/0x100 -> +0x4c/+0x54/+0x5c/+0x64/+0x6c/+0x74/+0x7c/+0x84/+0x8c), +0x9c the stride
  override (0x005357c0), +0xa0 the data flags. `Rfx::Emitter` owns one batch plus a stand-in atomic / geometry /
  material, so the shared `0x00476b30` body resolves the plugin and material 0 (atomic +0x18 -> +0x20 -> [0] -> +4)
  exactly as in the original. Create (`0x004770c0`), Flush (`0x00476df0`, now copying at the lock stride as
  0x00476e57..0x00476e83 does), SelectCallbacks (`0x00536780`) and Instance keep their logic on the new layouts.
- Consumers: `Gameplay/PickupGlow.cpp` (rotation through `SetRotateBits`, the debug dump through `TankElem`),
  `LibRw/RwRaceFx.{h,cpp}` (`RaceFx_DrawEmitter` takes the `Rfx::Emitter`, texture via `Rfx::Texture`).
- `re/tools/dual_copy_allowlist.txt`: the 15 `CROSS-TARGET` rows deleted, one note line in their place.
- No knob added or flipped. The exe path stays behind `MASHED_RFX_EMITTERS` (default OFF).

## 2. Static facts (original/MASHED.exe.unpatched; `datarefs_extract.txt`)

| address | image value | direct writers |
|---|---|---|
| `0x00613288` / `0x0061328c` | 1.0, 1.0 | none |
| `0x00613290` | 0xffffffff | none |
| `0x00613294` .. `0x006132a0` | 0, 0, 1.0, 1.0 | none |
| `0x006132a4` .. `0x006132c0` | 0, 1.0, 1.0, 1.0, 1.0, 0, 0, 0 | none |
| `0x006132c4` / `0x006132c8` | 5 / 6 | none |
| `0x007dc57c` | 0 | 1: `0x00534ac4` in FUN_00534a80 |
| `0x00692558` | 0 (BSS) | 1 direct: `0x004771cd` in FUN_004770c0 |
| `0x006925a8` | 0 (BSS) | none |

"No direct writer" is what Ghidra's reference list shows; a write through a computed pointer is not listed.

## 3. Arms

### L (lint): PASS

`py -3.12 scripts/lint_rva_bodies.py` on `30185a8e0`: `33 finding(s): CROSS-TARGET=18, DUP-IN-TARGET=15 | allowlisted=33 NEW=0`.
Before: 48 allowlist rows, after: 33. On the merged `4a805693d` (integration's HB-LINT-408A50 removed one more row):
`32 finding(s): CROSS-TARGET=18, DUP-IN-TARGET=14 | allowlisted=32 NEW=0`. None of the 15 RVAs is reported in either.

### A1 (original bytes in Unicorn vs the new core): PASS

`py -3.12 verify/rfx_emitters_20261028/emit_unicorn.py D:/MashedWT/D4-DC-RFX-CORE_data/emit_port.exe D:/MashedWT/D4-DC-RFX-CORE_data/a1`
with `verify/rfxcore_20261101/emit_port.cpp` (the D5 harness on the new layouts, same values in the same order; it
owns the arena with the exe's B17 pad trick, `/BASE:0x10000 /FIXED` + a 10 MB zero-init array) built from
`e141ddb88`'s `Render/RfxEmitter.cpp` with `/DMASHED_STANDALONE` (the exe's defaults). Log `A1_run.txt`.

| set | cases | mismatching |
|---|---|---|
| C1 create | 262 | 0 |
| C2 sequences | 240 | 0 |
| C3 selector | 146 | 0 |
| C4 instancing | 74 (2,459 particles) | 0 |

93,472 dwords compared. `port.txt` is byte-identical to the original side, which reproduces the D5 commit's hash:

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D4-DC-RFX-CORE_data/a1/cases.txt` | `5b20dc6d2f01edf6262782f161609622f53b7a7a103a71fab4f0bc1033aebd6b` (= D5) |
| `D:/MashedWT/D4-DC-RFX-CORE_data/a1/orig.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` (= D5) |
| `D:/MashedWT/D4-DC-RFX-CORE_data/a1/port.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` |
| `D:/MashedWT/D4-DC-RFX-CORE_data/emit_port.exe` | `c9c2de62ab63029d5cb3c3de099c311cbfbe91f1f2f429bd314ca58f10fd4bd8` |

The first A1 run (literal defaults in both targets, `30185a8e0`) was also 0 / 722; the A1 cases never seed the
default tables, so A1 cannot tell the two forms apart (section 4).

### F (Frida path1, original boot, this build's .asi): PASS, 15 / 15 GREEN

`py -3.12 verify/rfxcore_20261101/run_arm_f.py D:/MashedWT/D4-DC-RFX-CORE_data/armF4 --no-lock` inside one
`mashed_lock.py run --label D4-DC-RFX-CORE` hold (test-slot wrapped), private root, .asi `a85cbcb3...` (the merged build).
Five registry keys with a `diff_template.js` handler go through `run_diff.py` (`run_diff_private.py`), the ten
handler-less ones through the early-window lane they were earned on (`run_ew_private.py`, section 4). Audio check passed
before every boot. No piz collision, no retry.

| hook | RVA | lane | cases | verdict |
|---|---|---|---|---|
| particle_emitter_set_position | `0x004769a0` | run_diff (b10 fixture) | 4 | GREEN |
| particle_emitter_set_velocity | `0x004769d0` | run_diff | 10 | GREEN |
| particle_emitter_set_colour | `0x004769f0` | run_diff (g1_nullblind fixture) | 4 | GREEN |
| copy_476a10 | `0x00476a10` | early window | 2 | GREEN |
| particle_emitter_set_scalar | `0x00476a30` | run_diff | 10 | GREEN |
| particle_emitter_set_rgba | `0x00476a40` | run_diff (b10 fixture) | 4 | GREEN |
| copy_476a80 | `0x00476a80` | early window | 2 | GREEN |
| batch_476ae0 | `0x00476ae0` | early window | 2 | GREEN |
| set_476b30 | `0x00476b30` | early window | 2 | GREEN |
| batch_476b90 | `0x00476b90` | early window | 2 | GREEN |
| batch_476be0 | `0x00476be0` | early window | 2 | GREEN |
| batch_476c10 | `0x00476c10` | early window | 2 | GREEN |
| batch_476c60 | `0x00476c60` | early window | 2 | GREEN |
| batch_476cb0 | `0x00476cb0` | early window | 2 | GREEN |
| multi_array_scatter_476d00 | `0x00476d00` | early window | 3 | GREEN |

`particle_emitter_set_colour` and `particle_emitter_set_rgba`, RED on `30185a8e0` (section 4), are GREEN on the
`.data`-reading body. Every early-window run reports `mismatches=0`.

### F2 (path2, installer; reported, not a gate): JMP site verified 15 / 15, full pass 5 / 15 (one retry)

`run_arm_f.py D:/MashedWT/D4-DC-RFX-CORE_data/armF4p2 --path2 --no-lock` (same hold), `run_verify_hook.py` through the
private root (`run_verify_private.py`), auto-hook on.

| result | hooks |
|---|---|
| INSTALLED (opcode 0xE9, rel32 = reimpl - target - 5, reimpl fired, call legs pass) | position, velocity, colour, rgba |
| JMP verified (opcode and rel32 OK), call leg not drivable by `run_verify_hook.py` (`expected a pointer` / `bad argument count`, the same ten handler-less arg types as in arm F) | copy_476a10, copy_476a80, batch_476ae0, set_476b30, batch_476b90, batch_476be0, batch_476c10, batch_476c60, batch_476cb0, multi_array_scatter_476d00 |
| attach timeout (Frida `attach failed: timeout was reached`, no bytes read) | particle_emitter_set_scalar, retry below |

Retry (`armF4p2b`, inside the gate run's solo window, below): `particle_emitter_set_scalar` INSTALLED (opcode, rel32, reimpl fired, call legs pass). Final F2: JMP site verified 15 / 15, full pass 5 / 15.

So on this build the inline JMP of every hook but scalar was read back at its site and points at the shared body's
export. Run 28's earlier F2 (`armP2`, the `30185a8e0` .asi) had the same split with scalar INSTALLED (5 / 15).

### G (default gates): PASS, unchanged

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` with `A4_ROOT=D:/MashedWT/D4-DC-RFX-CORE_root`,
`A4_OUT=D:/MashedWT/D4-DC-RFX-CORE_data/gates`, test-slot + `mashed_lock.py run --solo` (run_arms prints `exe=9edac6fe`, the
merged build). Refs from `re/orchestrator/merge_train.json` on the merged tree.

| gate | got | ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | unchanged |
| B K0 / K1 / K26 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | 1d5bf4f9 x3 | unchanged |

Expected: the exe path stays behind `MASHED_RFX_EMITTERS` (default OFF), so the default race does not execute these bodies.

## 4. The first arm F run: two RED, cause, fix

Build `30185a8e0` used literal defaults in both targets (on the datarefs above). Arm F, run 1
(`D:/MashedWT/D4-DC-RFX-CORE_data/armF2/`): `particle_emitter_set_colour` RED (1 of 4 cases), `particle_emitter_set_rgba`
RED (2 of 4), position / velocity / scalar GREEN. The registry applies the negctl fixtures on import
(`hooks_registry.py` `_apply_negctl_fixtures`): `negctl_fixtures/g1_nullblind.py` seeds `0x00613290` with 0x11223344
and `negctl_fixtures/b10.py` seeds the `0x00613294` table before the NULL-path cases. Those fixtures are what holds
both rows at C3 (negctl re-earn 2026-10-11); they make the default READ the observable. A literal cannot follow a seed,
so the .asi body must read the address. Fixed in `e141ddb88` (section 1, item 2). In the running game the values equal
the image (no direct writer), so the two forms differ only when something writes those addresses.

The other ten in run 1 were refused by `run_diff.py` (`arg_type ... has no handler in diff_template.js`). Their C3
evidence was earned on the early-window lane (hooks.csv notes: "early_window GREEN"), so arm F now runs those ten
through `re/frida/early_window_leaf_diff.py` (`run_ew_private.py`).

## 5. Process notes

- All original boots: private root `D:\MashedWT\D4-DC-RFX-CORE_root` (robocopy of `original\`), audio check before
  every boot (`Get-PnpDevice -Class AudioEndpoint -PresentOnly`: VB-Audio Virtual Cable endpoints present each time),
  test-slot, `mashed_lock.py run --label D4-DC-RFX-CORE`. The per-boot lock lost every round to other sessions
  (run 1 got 1 boot in 25 min), so the 15 + 15 boots run inside one lock hold (`run_arm_f.py --no-lock`).
- The private root's `mashed_re_dev.asi` was replaced by this build's (`run_verify_hook.py` path2 boots with auto-hook
  on and checks the JMP against the dinput8-loaded copy). The root's previous file is kept at
  `D:/MashedWT/D4-DC-RFX-CORE_data/root_asi_before.asi` (`906f8601...`). The shared `original\` was not touched.
- Killed only my own PIDs (the first arm F run, stopped to restructure the lock hold; no MASHED.exe of mine was running).
- Run 29 resume (LIVING-PC rebooted 14:23 during run 28's arm F hold): merged `orch/integration`, rebuilt, re-copied the
  .asi into the private root, re-ran F + F2 in one lock hold. The gate run waited over an hour behind other sessions'
  solo windows (they block plain acquirers), so I stopped my own queued waiter (my py / bash PIDs only, no game was
  running) and re-queued it as `run --solo`. The first gate attempt in the F hold failed before scoring: `run_arms.py`
  does not create `A4_OUT` (folder created, re-run).
- No .ps1 / .bat edited or created (the A1 harness is built with one `cl` command, in section 3).

## 6. Trackers (re-classify, no C-level change)

Through the re-classify transaction (tracker-editor leg): the 15 `hooks.csv` rows keep C3, their `file` column moves to
`mashedmod/src/mashed_re/Render/RfxEmitter.cpp` (13 pointed at the deleted `Render/FrameHelpers_q2.cpp` /
`Util/PromoLoop_sessionB.cpp` bodies, 2 at an analysis plate), and each `notes` cell gets a `[D4-DC-RFX-CORE 2026-11-01]`
entry (one body for both targets, CROSS-TARGET row removed, arm F 15 / 15). `exe_file` already named the same file.
15 `C3->C3` lines prepended to `re/analysis/CHANGELOG.md`. No STUBS / UNCERTAINTIES / DEFERRED change. No C4: these
are synthetic Frida diffs (C3 evidence), not a canonical-scenario run.

## 7. Raw files (not committed)

All under `D:/MashedWT/D4-DC-RFX-CORE_data/`.

| file | SHA-256 |
|---|---|
| `a1/cases.txt` | `5b20dc6d2f01edf6262782f161609622f53b7a7a103a71fab4f0bc1033aebd6b` |
| `a1/orig.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` |
| `a1/port.txt` | `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5` |
| `emit_port.exe` | `c9c2de62ab63029d5cb3c3de099c311cbfbe91f1f2f429bd314ca58f10fd4bd8` |
| `armF4/summary.tsv` | `ac59e742d26caeec4efd23da5705ea461751e2fb77604d0b5bcd5e5ea90bfbd2` |
| `armF4/particle_emitter_set_position.log` | `752fb962c46397ba40a090a07f44066c47a56e2718f2f2aae6c73bd38c842137` |
| `armF4/particle_emitter_set_velocity.log` | `fd9c0a238bebde629ac00ee3519cfcf408d1ed09d752d056b4cb09cdfb1cdb64` |
| `armF4/particle_emitter_set_colour.log` | `6365ea5b178a8fdcb07d3f17b4b61121d6da297a1a86666e0d6ceff364c6c6f7` |
| `armF4/copy_476a10.log` | `e7b5a7001cd23d00522245528fa6b556dce613fc3bcfa14ee1679720036c3b14` |
| `armF4/particle_emitter_set_scalar.log` | `17f0a837755816bcbcf08b1323089480eccb5ae2fb996dd34d12d15482f834bf` |
| `armF4/particle_emitter_set_rgba.log` | `2c0816d2a67dc6dfee7d483804260ccf0cae81ae728aaa9d7cbf81950d3742db` |
| `armF4/copy_476a80.log` | `307a2c5ffbaa08f87fa7cbbbeb4b63784fef550a78c5b521f68bf475a88feca5` |
| `armF4/batch_476ae0.log` | `cea5de1440a8323689f89c52332f3271b47c7eea18cd8f64c86c431a9e8ebbc7` |
| `armF4/set_476b30.log` | `f77d6ca675f50aff1ca73694ae3a1282deb1f18895336d9597d8e61a40b250ff` |
| `armF4/batch_476b90.log` | `339751172cb1e3bdb56a84d06a71ce1df97bbcf53afcb818905b12ccd7203baf` |
| `armF4/batch_476be0.log` | `c3b2947a0a089fe4ef62c055440e24a13d57fbff8987dad96192187ef238583b` |
| `armF4/batch_476c10.log` | `b3e833d3dcde7039daaff37dc5b328cd69a0ac85a0122dfdfdd3f47a0f02dd79` |
| `armF4/batch_476c60.log` | `a978b6d54e89a90cdce53bf724f69133a468d932967224408eefb09170f71003` |
| `armF4/batch_476cb0.log` | `e901e8103ad90875b11ba7fb291722095b041b148c414d076a5f6b11a846bd22` |
| `armF4/multi_array_scatter_476d00.log` | `3e6fff84b4f4ef716717baf56046f9901ee69f301c60cd2da31ccb3f0d795653` |
| `armF4p2/summary.tsv` | `551348c6e424a406409fd56edf59edeaddff54fc77b8dd4cd96607b4093c04a1` |
| `armF4p2b/particle_emitter_set_scalar.log` | `8ee15f93375520d92aaf08b8058c518c602bf44bb2f0c3e5a2093298b8d2aa1e` |
| `gates/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `gates/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `gates/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates/B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |

Earlier, superseded runs kept for the record: `armF2/` (run 1 on `30185a8e0`, section 4), `armP2/` (run 28 F2 on
`30185a8e0`), `armF/` (stopped first attempt).
