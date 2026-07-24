# account3 hand-off checklist — 2026-07-24

Consolidated, actionable checklist of work that **structurally cannot run on account2**
(needs canonical Frida, Ghidra MCP, or execution+verify) surfaced by the 2026-07-24 account2
sessions. Detailed evidence/tables live in the linked source docs — this is the run order.

Legend: `[ ]` todo · `[x]` done · **(blocker)** = do this first.

## A. C4 promotions — 19 C4-ready rewired containers

Full table + per-row callee C-levels: **`re/account3_c4_ready_batch_2026-07-24.md`**.
Gate = thunk-callee C4 ruling (`B5d_COUPLING_BRIDGE_2026-07-15.md` §7). Needs canonical-scenario
Frida with the inline-JMP **live** (not bypassed).

- [ ] **(blocker) Reboot** — 2026-07-23 spawn/kill cycles degraded the DirectShow boot state
      (`project_boot_hang_directshow_intro`); no canonical run boots until reboot.
- [ ] **(blocker) Re-enable `LobbySlotListRender` 0x00439210** — its `RH_ScopedInstall` is
      MASS-DISABLED (SpriteCluster.cpp:659); re-enable + rebuild before its diff. Other 18 are live.
- [ ] Menu/frontend leaves (16): 0x00423cc0/d50/dd0/e60, 0x00423ee0/f60/ff0/070, 0x0040b9a0/ba60,
      0x0040b890/b8e0, 0x00424100, 0x004241c0, 0x00428610, 0x00430b90 — boot-to-menu / menu-nav
      diff-original, hook LIVE.
- [ ] Save/stream (2): 0x004cc6e0, 0x004cc770 (RwStreamWrite_s2 path) — save/write scenario.
- [ ] 0x00439210 — after re-enable, menu/lobby scenario.
- [ ] `ProgressBarSetA` 0x00430b90 — only non-hooks.csv call is the RW driver vtable dispatch
      (external engine, no first-party gate); use a state where the bar renders, or `crash_equal_ok`.
- [ ] `re-classify` each GREEN one C3→C4 (cite canonical CSV + the thunk-callee ruling), ideally as
      **one transaction** while the account2 worker is idle (tracker-clobber avoidance).

## B. Harness backlog item 1 — arg_type frontier (the #1 C3 lever)

Full refined classification: **`re/analysis/plans/frontier_shape_refinement_2026-07-24.md`**.

- [ ] **Build the x87 80-bit ST0 float-return capture handler** (highest leverage — unlocks 6
      frontier rows + the deferred veccap `FUN_005667c0`). Shape: no ptr args, seed N input globals,
      call, capture x87 ST0 extended-precision return, compare. Regenerate `ARG_TYPES.md` after
      (`py -3.12 scripts\gen_arg_types_index.py`). Needs Frida to verify.
  - [ ] Then promote the 3 sine getters 0x00431b20 / 0x00431b50 / 0x00431b60
        (`fsin(global × global)`, one handler + 3 registry configs).
  - [ ] Then the 3 RwV3d bbox accessors 0x004c4270 (Y) / 0x004c42d0 (X) / 0x004c4360 (Z) — these also
        need a **manual x87 reimpl** (decompiler lift is false-empty), not a clean C++ transcription.
- [ ] Onboard 0x004b4550 (3D centroid, `void f(out_vec3, const float* pts, int n)`) to **veccap**
      (`veccap_registry.py`, `v_out_*` kind family) — **offline, no Frida** (could even run local),
      verify replay + Unicorn.
- [ ] Bespoke per-function reversing (not shape-handler wins): 0x0048b650 (particle spawn),
      0x0045c550 (list-state matcher), 0x004cbbd0 (frustum-AABB cull).

## C. Tracker hygiene (optional, not blockers)

- [ ] Clear stale active passthrough STUBS rows for C2+ callees so a mechanical stub-check doesn't
      false-block C4 (list in `account3_c4_ready_batch_2026-07-24.md` §4: S-0485/3574/3650, S-0920,
      S-3633, S-3910, S-3122/3998, S-3999, S-3658/3926/4006/4027/4060/4106, S-4398). Via re-classify.
- [ ] Refresh SpriteCluster.cpp in-file "Callee table" comments (stale C1 labels; hooks.csv has C2–C4).
- [ ] `0x00443300` is stale in `promote_frontier.tsv` (now C3/impl) — drops on next
      `scripts/promote_frontier.py` regen; no manual action.

## Context — done this session (account2, NO action needed, informational)

- [x] 6 stale-stub strikes (S-1483/S-3653/S-1442/S-2410 direct-call/inlined;
      S-3931/S-3932 inlined into C4 LogoOverlayDraw). Census 1,115/141 → 1,109/147. CHANGELOG 2026-07-24.
- [x] `ARG_TYPES.md` regenerated (1 stale registry-use count).
- [x] Both stub-resolution lanes (fnptr→C4 rewire, stale/inlined) now exhausted in current source —
      they need NEW C4 promotions (section A) to feed more account2 Lane-A work.
