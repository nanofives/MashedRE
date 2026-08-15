# Split-screen spike — preserved artifact (2026-08-14)

This directory holds the **only surviving copy** of the local split-screen multiplayer
spike. The branch it lived on (`promote-c4`, and its strict ancestor `ws-visual-polish`)
was retired on 2026-08-14 once every other feature on it had been ported to `main`.

Preserved here rather than kept as a branch because a branch is not a durable citation:
`DEFERRED.md` D-11063 cited commits `e5df32ef` and `1e4328bb`, and those hashes become
unreachable the moment the branch is deleted. A committed patch does not rot.

## Files

| File | What it is |
|---|---|
| `SPIKE_e5df32ef_1e4328bb.patch` | 272-line diff of the two spike commits against their fork point `c775573a`, limited to `TrackRenderer.{cpp,h}` and `exe_main.cpp` |
| `vp_split.png`, `vp_split2.png` | The spike's entire verification evidence (see the warning below) |

Original commits, for the record — **unreachable after branch deletion**:

- `e5df32ef` — "WS-mp: split-screen rendering — 2 viewports, 2 cameras (VERIFIED)"
- `1e4328bb` — "WS-mp: 2nd input + MP frontend (player-count toggle) — local split-screen complete"
- fork point: `c775573a` (2026-06-19)

## What the spike actually built

Measured 2026-08-14 against the code, because D-11063's original justification described
this work inaccurately. 103 insertions / 15 deletions across 3 files.

| Element | Verdict |
|---|---|
| Second viewport | **BUILT** — real dual `D3DVIEWPORT9` top/bottom with per-half aspect |
| Second camera | **BUILT, REDUCED** — a view/proj pair from a hardcoded chase cam on `ai_cars_[0]` (`back=7.f, up=3.f`), not a second `RaceCamera` |
| Second input | **BUILT, REDUCED** — `GetAsyncKeyState` arrows overwriting car slot 1's AI control bytes; `DriveInput` untouched |
| Second HUD | **NEVER BUILT** — `e5df32ef` itself says "full viewport restored for the 2D/HUD pass" |

## Warning: "(VERIFIED)" in the commit message is not project-standard verification

The evidence is the two PNGs in this folder and nothing else. There is no
`drawlist_diff.py` run, no `imgdiff.py` run, and no Frida diff anywhere in
`c775573a..promote-c4`. Per `CLAUDE.md` the gate for standalone visual work is the
parity harness, so **this spike never met it**. The second-input element was only ever
claimed "DONE (wired + builds)" — no runtime verification at all.

Treat the patch as a design reference, not as working verified code.

## Survivability if re-picked

As of `main` @ 2026-08-14:

- **Viewport** — transplants near-verbatim. Anchor `TrackRenderer.cpp:3877` is intact and
  there is no viewport code on main at all.
- **Camera** — partly. Main independently added `last_aspect_` (`TrackRenderer.cpp:3889`),
  which makes per-view aspect *easier*, but deleted the round-mode FOV branch the patch
  preserved and added a librw consumer of those frustum members.
- **Input** — this is now a **rewrite, not a port**. The `ctrl` read moved out of
  `TrackRenderer::UpdateCar` into verbatim-decompiled AI code at `Ai/AiController.cpp:167`.
- **HUD** — was never written; it is new work regardless.
- `MashedPlayerCount()` exists nowhere under `mashedmod/` on main; only the plan doc merged.

**Particle constraint** (the spike guarded this, and it still applies): the pool must
`Emit`+`Update` **once per frame** but `Render` **per view**. Main keeps `parts_.Update`
and `parts_.Render` as separate calls (`TrackRenderer.cpp:4449-4451`), so this remains
structurally possible. `EmitCarFx` was ported to main on 2026-08-14 and its call site is
already correctly placed once-per-frame before `parts_.Update` — see U-9038, which
established the symbol had never landed on main rather than having been lost.

## Related

- `DEFERRED.md` D-11063 — the deferral, justification corrected 2026-08-14
- `re/analysis/split_screen/SESSION_END.md:52-54` — U-1908, the load-bearing original-side
  unknown (per-player screen-quadrant/viewport-rect assignment mechanism unlocated)
- `re/analysis/MULTIPLAYER_PLAN_v1.1*` — the scope doc, already on main
