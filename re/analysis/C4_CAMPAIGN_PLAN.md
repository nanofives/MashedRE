# C4 Promotion Campaign — "Uncover the Whole Game"

Created 2026-06-19 (branch `promote-c4`). Goal stated by user: *dedicate hard time to
mass-promote functions; find the highest-yield method to uncover the whole game.*

## The landscape (hooks.csv, 2026-06-19)

| Level | Count | Notes |
|---|---|---|
| C1 | 999 | **977 are library bands** (RenderWare 462, RW-Physics 288, qhull 184, SEH 43), pinned C1 by policy. First-party C1 ≈ 0. |
| C2 | 3,913 | **2,700 first-party** + 1,213 library (CRT/Lua/D3DX9/qhull). |
| C3 | 831 | "Understood" — reimplemented + hooked. |
| C4 | ~106 | "Verified" — canonical-scenario behavioral evidence. |

Library C1/C2 are **off the critical path** (identified third-party code we link, not
reimplement). Do not spend promotion effort on them. The real target = first-party.

First-party C2 concentration: render 796, audio 459, util 340, boot 246, gameplay 241,
particle 163, vehicle 94, frontend 86, hud 77, input 47, track 46, ai 46.
First-party C3 concentration: render 150, frontend 147, audio 125, gameplay 116, util 99,
vehicle 53, ai 31, hud 30, boot 27, particle 17, input 13.

## The key reframe (decisive)

The canonical C4 harnesses (`canonical_c4_sidediff.py`, `canonical_c4_leafdiff.py`)
attach the **ON run to the `.asi` export** — i.e., the reimplementation. So the canonical
loop only applies to functions that are **already reimplemented = C3**. Therefore:

- **Canonical C4 loop = C3 → C4 verification.** Immediate-eligible pool = **all 831 C3**.
  Playable-core C3 (render+audio+gameplay+vehicle+particle+hud) = **491**.
- **C2 → C3** is a *different* job (reimplement + semantically understand). It needs
  Ghidra/source work (Front B), NOT the Frida canonical loop. The 2,700 first-party C2.

"Uncover the whole game" = two tracks: **Front A** verifies the 831 C3 → C4; **Front B**
reimplements 2,700 C2 → C3 (which then feed Front A). User chose: **Front A first,
playable-core first.**

## Validated foundation (2026-06-19, this session)

- Branch `promote-c4`; `original/` **junction** → main-repo runtime (worktrees have no
  `original/`; harnesses resolve `ROOT/original`). Fresh faithful `.asi` redeployed.
- The C4-minting loop **works end-to-end here**: `canonical_c4_leafdiff.py --nav 4,4`
  booted OFF+ON, `.asi` loaded, 4 hooks live (`jmp=0xe9`), OFF==ON behavior →
  **4× C4-CLEAN** (re-confirm of already-C4 frontend leaves). `log/c4_leafdiff_result.json`.
- `MASHED_HOOK_ONLY` scoping installs only the target hooks → visual-polish experiments
  (collision FX, split-screen, prelight) stay **dormant** during captures. No clean
  rebuild needed; clean env = faithful defaults.

## Determinism tiers (bounds in-race yield)

Canonical diff certifies only when OFF and ON see **identical state**.

- **Tier 1 — invariant/maskable signature** (getters, config/setters, render-state +
  material/texture binders, dispatchers w/ constant args, stable drawers, most HUD/2D).
  Diffs cleanly in *any* scenario incl. a live race. Mint to C4 with current harness +
  auto-spec.
- **Tier 2 — data-dependent in a race** (per-car physics, transforms over live positions,
  AI over moving state). OFF≠ON run-to-run (RNG/timing/physics). Needs a
  **deterministic-race harness** (Frida-force RNG seed + scripted inputs + fixed timestep
  in BOTH runs) so traces are bit-identical. Master key to the data-dependent majority;
  prove feasibility on a 2-function canary first.

## The bottleneck + the tool to build

The proven `sidediff` captures (callee_rva, args) per target and `leafdiff` captures
(ret/buffer/writeflag); both require **per-function spec authoring** (callee lists, capture
kind). That is the throughput limiter for 831 functions. **Build the auto-spec tool.**

**Design — `canonical_c4_autodiff.py` (self-specifying):**
1. Spawn/nav/GATE/`MASHED_HOOK_ONLY` plumbing copied verbatim from proven `sidediff`.
2. **Discover pass (OFF, Stalker, no args — simple/robust):** gate to the target's first
   post-settle invocation, `Stalker.follow` with `events:{call:true}`, record the SET of
   callee RVAs executed within the target frame (catches **indirect/vtable** calls that
   static Ghidra callees miss → sound). `unfollow` on leave.
3. **Diff pass (OFF + ON, Interceptor — reuse PROVEN sidediff capture):** attach target +
   each discovered callee; record ordered (callee_rva, arg0..argN) sequence + EAX. Compare
   signature SETS OFF vs ON. Non-empty + equal + `jmp==0xe9` → **C4-CLEAN**.
   - Arg normalization: image pointers → `img+off`; stack/heap → `ptr` (masked);
     animated globals → delta-mask vs same-run base (the `rel/sub` trick already in
     sidediff). Anything that won't normalize → **HOLD** (conservative, never false-CLEAN).
4. Soundness bar: must capture **callee + args** (not callee-only) to match the established
   sidediff rigor — do NOT ship a weaker C4 gate (no-overclaiming rule).

This eliminates per-function authoring → turns the 831-function campaign into batched runs.

## Phases & cadence

- **Phase 1 (now):** auto-spec tool + fire-filter. Batch-verify boot/menu-reachable C3
  (render-2D/config, frontend, audio-init, util, boot) → C4. Deterministic scenario.
- **Phase 2:** deterministic-race harness (canary first) → unlocks in-race playable-core
  C3 (HUD, particle, vehicle, gameplay, in-race render) → C4.
- **Phase 3 (Front B, parallelizable):** Ghidra semantic + reimpl fanout C2 → C3 for the
  2,700 pool, prioritized by playable-core; feeds Front A.

**Per batch:** fire-filter (1 boot, chunk ≤~40 to avoid hot-path destabilization; flag
funcs >~50/s as hot → exclude from Interceptor, handle by sampling) → auto-spec discover
(1 boot) → diff (2 boots) → `re-classify` CLEAN→C4, HOLD stays w/ cited reason. All
mutations via `re-classify` only. Throughput anchor: R1-B did ~14/run on survival; C4
behavioral batches slower but mint the real DoD.

## Hazards (carry forward)

- Hot paths (>~1000/s, render inner loops) destabilize Interceptor (~6s) — exclude;
  sample one-at-a-time or observation-only (caps at C3, not lost).
- `original/` is the **shared** runtime; deploying `.asi` affects any other session's
  MASHED. Track spawned PIDs; kill only your own (multi-session rule).
- Front A and Front B can promote the same function with different evidence — keep them on
  separate RVA partitions (A=current C3, B=current C2) to avoid collisions.
