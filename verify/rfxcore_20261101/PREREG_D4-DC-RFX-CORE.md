# PREREG D4-DC-RFX-CORE (one shared RFX emitter core, L-51 a)

Branch `orch/D4-DC-RFX-CORE` from `orch/integration` @ `0e3d71a75`. Committed alone, before any arm runs.

## Change

- `Render/RfxEmitter.cpp` goes into BOTH `exe_sources.rsp` and `asi_sources.rsp`. It holds the one body of each
  of the 15 RVAs `0x004769a0 0x004769d0 0x004769f0 0x00476a10 0x00476a30 0x00476a40 0x00476a80 0x00476ae0
  0x00476b30 0x00476b90 0x00476be0 0x00476c10 0x00476c60 0x00476cb0 0x00476d00`, as the exported hooks with
  the export names `re/frida/hooks_registry.py` drives, plus their `RH_ScopedInstall` (no-op in the exe).
- The bodies are the .asi C3 bodies on the original's layouts: staging globals at their absolute addresses, the
  0x40-byte batch, the plugin block as `*(*(batch + 4) + [0x007dc57c])`. Two deliberate differences from the
  deleted .asi copies: (1) the null defaults at `0x00613288 / 0x00613290 / 0x00613294 / 0x006132a4` are
  literals (datarefs: no writer in the original, the exe image-pad holds 0 there); (2) `0x00476d00`'s +0x20
  array copy is the original's `fld` / `fstp` pair (0x00476d8e / 0x00476d94) in inline x87, where the deleted
  .asi body and the deleted exe body both made a plain 32-bit copy.
- `Render/FrameHelpers_q2.cpp` deleted (its five bodies moved), the ten `Util/PromoLoop_sessionB.cpp` bodies
  deleted, the 15 `CROSS-TARGET` rows deleted from `re/tools/dual_copy_allowlist.txt`.
- The exe's `Rfx::` API becomes call-throughs onto the 15 exports. `Rfx::PTank` / `Rfx::Batch` become the
  original's layouts (static_asserts at the cited offsets, {ptr, stride} array descriptors per FUN_00535700's
  jump tables); `Rfx::Emitter` owns the batch plus a stand-in atomic / geometry / material so the shared body
  resolves the plugin and material 0 as the original does. Create / Flush / Instance keep their logic on the
  new layouts (Flush copies at the lock stride, 0x00476e57..0x00476e83). Consumers updated: `Gameplay/PickupGlow.cpp`,
  `LibRw/RwRaceFx.{h,cpp}`. No knob added, none flipped: the exe path stays behind `MASHED_RFX_EMITTERS` (default OFF).

## Arms and pass criteria

- **L (lint).** `py -3.12 scripts/lint_rva_bodies.py`: `NEW=0`, and none of the 15 RVAs reported. Pass = both.
- **A1 (original bytes vs the new core).** `verify/rfx_emitters_20261028/emit_unicorn.py` with a port exe built from
  this branch's core (harness adapted to the new layouts only: the dumped values and their order unchanged; the
  harness maps the staging page `0x00690000` and `0x007dc000`, which the exe's image-pad provides). Same
  722 cases. Pass = 0 mismatching cases and port output SHA-256 equal to the committed original-side output
  `fdaf8b6a2db7d51804ea7a0c76d5c3acd12a1e3197308c95408d5843d9b1bce5`.
- **F (Frida path1, original boot).** `re/frida/run_diff.py` per hook through a private-root wrapper
  (`D:\MashedWT\D4-DC-RFX-CORE_root`, `mashed_lock.py`), this worktree's `.asi`, for the 15 registry keys
  `particle_emitter_set_position particle_emitter_set_velocity particle_emitter_set_colour copy_476a10
  particle_emitter_set_scalar particle_emitter_set_rgba copy_476a80 batch_476ae0 set_476b30 batch_476b90
  batch_476be0 batch_476c10 batch_476c60 batch_476cb0 multi_array_scatter_476d00`. Audio check
  (`Get-PnpDevice -Class AudioEndpoint -PresentOnly` lists VB-Audio) before every boot, else BLOCKED. Pass =
  15 / 15 GREEN. A piz-collision crash (U-9677) is a re-run.
- **F2 (path2, installer).** `re/frida/run_verify_hook.py` per hook through the same private root. Pass = 15 / 15
  installed. Reported, not a gate for the allowlist removal if the tool has no private-root path (then stated).
- **G (default gates).** `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this
  branch's exe. Pass = G-CTRL and B equal to `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`.

GREEN = L, A1, F, G all pass. Any F hook RED: the row is RED with that hook named, the allowlist rows are not
restored (the dual copy is gone either way); the fix is to the shared body.
