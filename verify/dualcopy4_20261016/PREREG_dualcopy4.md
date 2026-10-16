# PREREG dualcopy4: (a) the identical twins, then (b) the real behaviour splits

Committed BEFORE the first measuring run of this lane. Branch `work/dualcopy4`, base `race/first-frame-parity` `9b0248128`.
Model claude-sonnet-5-5. Orchestrator brief (2026-10-16): continue ROADMAP D4 "Burn the dual-copy allowlist down to zero" from
`verify/dualcopy3_20261016/RESULT_dualcopy3.md` follow-ups. Order: (a) twins that own registry evidence, (b) real splits, then fix
`scripts/backfill_exe_file.py`. Skip 0x0045baa0, 0x004177b0, 0x0040e180 unless trivially isolated.

## 0. Start of lane (measured at 9b0248128 in this worktree, before any edit)

| metric | value | source |
|---|---:|---|
| allowlist lines (`re/tools/dual_copy_allowlist.txt`, `^(DUP|CROSS)`) | 39 | `lint_rva_bodies.py`: CROSS-TARGET 13, DUP-IN-TARGET 26, NEW 0 |
| honest measure: hooks.csv C3/C4 rows with `exe_file != file` | 172 of 1211 | `honest_base.txt` |
| (A) `file` is a .cpp | 38 | |
| (B) `file` is a plate path | 134 | |

Baseline images (built from 9b0248128 in this worktree, frozen): `images_sha.txt`, `images/base.exe`, `images/base.asi`. Every comparison is
base image vs new image, never against a rebuild (builds are not byte-reproducible, dualcopy3 finding).

## 1. What the tree says about group (a) (read before any run)

| RVA | bodies | which one is live | callers in src | registry keys |
|---|---|---|---|---|
| 0x0041eda0 / 0x0041f000 / 0x00420d40 | `Util/TimerInit.cpp` (TimerBitFieldSet / TimerSlotDataCopy / TimerStructArrayClear) and `Util/TimerSlot.cpp` (SlotBitSet / SlotDataCopy / SlotArrayClear) | TimerInit.cpp (its `RH_ScopedInstall` active); TimerSlot.cpp installs are commented "first-wins; superseded" | none in any exe or asi TU (grep) | live: `timer_bit_field_set`, `timer_slot_data_copy`, `timer_array_clear_d40`; stale: `slot_bit_set`, `slot_array_clear` (`slot_data_copy` already removed 2026-05-24) |
| 0x00422120 | `Util/TimerInit.cpp:TimerInitLoop` (naked, EAX-arg ABI) and `Util/TimerSubarrayInit.cpp:TimerSubarrayInit` (plain call, the "silently-broken" impl TimerInit.cpp:151 describes) | TimerInitLoop | none | `timer_init_loop` (negctl re-earn 2026-10-11 GREEN) |
| 0x00431b70 | `Frontend/MenuLeaves_af5.cpp:MenuFlagDat007f0f10Get` and `Frontend/BucketMixed_t3.cpp:GetDat007f0f10` | MenuLeaves_af5 (install active; BucketMixed_t3's is commented) | none | live key for MenuLeaves_af5 (hooks_registry.py:15173), stale STAGED key for GetDat007f0f10 (:12853) |
| 0x004c5c00 | `Util/PromoLoop_sessionB.cpp:Search4c5c00` (naked, installed) and `Frontend/SpriteCluster.cpp:LinkedListStringSearch` (asi-only, no install, one caller `SpriteLookupC`) | Search4c5c00 | `SpriteCluster.cpp:255` | `search_4c5c00` |
| 0x004b4650 | `Util/PromoLoop_sessionB.cpp:Lerp4b4650` (naked x87, installed), `Render/PromoLoop_round22.cpp:Vec3Lerp` (not installed, asi), and the exe's `Powerup/PowerupContact.cpp:Vec3Lerp` (C++, +`Log` call, called by 6 powerup TUs) | Lerp4b4650 | exe: powerup TUs | `lerp_4b4650` |

Facts that change the brief: the Timer bodies and 0x00431b70 have **no caller anywhere in the exe**, so in the shipping exe they are dead
code and merging them cannot move any exe behaviour; 0x004b4650 is NOT an identical-twin case (the exe copy computes `t*d + a` in SSE with a
float rounding between the multiply and the add, the original x87 chain adds the unrounded product), so it is handled in the group-(b) block.

## 2. Changes under test (group a)

1. **Timer set**: delete `TimerSlot.cpp`'s SlotBitSet / SlotDataCopy / SlotArrayClear (ZeroFillWrapper 0x004b6520 stays), delete
   `TimerSubarrayInit.cpp` (file + both rsp entries), retire registry keys `slot_bit_set` and `slot_array_clear`. Rows 0x0041eda0 / 0x0041f000 /
   0x00420d40: name, `file`, `exe_file` -> `Util/TimerInit.cpp`, evidence -> this lane's path1; 0x00422120: `exe_file` -> TimerInit.cpp.
2. **0x00431b70**: delete `BucketMixed_t3.cpp:GetDat007f0f10`, retire its STAGED registry key; `exe_file` -> MenuLeaves_af5.cpp.
3. **0x004c5c00**: delete `SpriteCluster.cpp:LinkedListStringSearch`; `SpriteLookupC` calls the original RVA through a function pointer like
   `SpriteGate.cpp:s_FUN_004c5c00` (in the asi that address is the live E9 into `Search4c5c00`). Row `file` -> `Util/PromoLoop_sessionB.cpp`.
4. **0x004b4650 (dead third copy only)**: delete `Render/PromoLoop_round22.cpp:Vec3Lerp` (never installed). The exe copy is a group-(b) item.

## 3. Gates and predictions (group a)

| gate | pass criterion | prediction |
|---|---|---|
| G-LINT | `lint_rva_bodies.py` NEW = 0, `lint_inline_asm_abs.py` OK, both targets build, asm-lint OK | PASS. Allowlist loses: 0041eda0 x2, 0041f000 x2, 00420d40 x2, 00422120 x2 (exe+asi lines), 00431b70 x2, 004c5c00 x1, 004b4650 x1 = 12 lines, **39 -> 27** |
| G-PATH1 per kept key (`timer_bit_field_set`, `timer_slot_data_copy`, `timer_array_clear_d40`, `timer_init_loop`, the MenuLeaves key for 0x00431b70, `search_4c5c00`) on the NEW asi, via the shared-asi window | `run_diff.py <key>` GREEN, non-degenerate (the CSV has more than one distinct original value, the sentinel/observed value moves) | GREEN. A degenerate or RED result on a kept key is reported and the row is NOT re-earned |
| G-PATH2 per kept key | `run_verify_hook.py <key>` PASS, E9 + rel32, entries >= 1 where the harness can drive the call | PASS; `NA`/0-entry on a 0-arg void key is reported as such, not as a pass |
| G-BOOT (asi) | `install_observe ALL --seconds 45` survives; the manifest lists every kept install once | PASS, 1290 -> about 1290 installed (the deleted copies were never installed) |
| G-NAV | nav boot full hook set reaches a race alive | PASS |
| G-CTRL (exe, TRAINING rule 4) | step / gates / ptrace sha8 equal base image vs new image | UNCHANGED `2b4d47bb / 356821c8 / f7a155e1`: nothing deleted in group (a) is reachable from the exe (no callers) |
| G-EXE-DIFF | the exe `.text` symbol set loses only the deleted bodies (`dumpbin /symbols` or the linker map) | the six Timer/Bucket symbols leave the exe; nothing else changes |
| M-CTRL | a mutant of the KEPT Timer bit-set body (flag test inverted) run through `run_diff.py timer_bit_field_set` | RED, so a GREEN on the real body means something |
| Rows | `exe_file == file` for 0x0041eda0, 0x0041f000, 0x00420d40, 0x00422120, 0x00431b70; 0x004c5c00 `exe_file` stays empty (the exe has no 4c5c00 body) | after the one transaction per RVA (tracker script, assertions read from the evidence files) |

## 4. Decision rule

A twin merge lands when G-LINT, G-PATH1 and G-PATH2 pass on the kept key, G-BOOT and G-NAV pass, and G-CTRL is unchanged. If the kept key is
degenerate or RED, the deletion is reverted for that RVA and the row is reported, not re-earned. A moved G-CTRL hash is a finding to be
explained before anything else is claimed; it is NOT expected in group (a).

Group (b) gets its own pre-registration addendum (`PREREG_dualcopy4_b.md`) committed before the first group-(b) run.
