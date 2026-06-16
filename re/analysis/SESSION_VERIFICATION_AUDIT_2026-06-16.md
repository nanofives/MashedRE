# Verification audit — 2026-06-15/16 scaffold-to-port session (item 5)

The project's acceptance gate is **C4 = bit-identity vs the original via the
`diff-original` Frida lane** (re/CONFIDENCE.md; [[feedback-no-overclaiming-c-levels]]).
Almost everything built this session is **SCAFFOLD** (invented stand-ins that run
+ look right) — by definition NOT C4 and NOT `diff-original`-able, because there
is no 1:1 original function to diff against a scaffold. This itemizes the debt so
it is explicit + actionable (you cannot pay down debt you have not classified).

## Classification of this session's deliverables

### A. SCAFFOLD — invented; NOT faithful; cannot be C4'd as-is
Must be REPLACED by verbatim ports before they can be verified:
- Vehicle handling (TrackRenderer::UpdateCar) — kinematic; real = FUN_00470670
  cluster (RW-Physics). See vehicle_physics_cluster.md. **C4 blocked on the port.**
- AI driving (gate-ribbon lane follower) — real = **FUN_00418860 family** (per-frame AI
  tick → per-vehicle decide/steer/throttle → synthetic input block). [Corrected
  2026-06-16: "FUN_0040e480" was a mis-citation; that RVA is `CarSlotStateSet` (frontend
  leaf). Cluster mapped in `re/analysis/ai_controller.md` (WS-C C1 done).]
- Collision (ground raycast only) — real = RW-Physics contact system.
- Power-up EFFECTS (boost/shield/missile/mine/shock) — real = FUN_00430670 family.
- Particles, pickup orb visuals, lap/elim flow tuning, results screen layout,
  game-mode->rule mapping, car flat-lighting (approx, not RW RpWorld).
- Progression store (our own sidecar format, not the gamesave serialization).

### B. VERBATIM-PORTED — a specific RVA; C4-TARGETABLE (diff-original pending)
These CAN be `diff-original`-verified against the booted original:
- Race camera — Race/RaceCamera (0x00446520 / 0x00410d10). [C4 lane: open]
- Nav state machine — FUN_0043d2a0 + descriptor tables. [partially diff'd earlier]
- Elimination scoring (FUN_0040eee0 / FUN_0040b290 / 0x00410510) — partial.
- RW math primitives (Math/RwV3dTransform, RwV2d, RwSqrt) — leaf, diff'able.
**C4 QUEUE (next):** RaceCamera, the scoring trio, the RW math leaves — run
re/frida/run_diff.py per hook once the original is booted (the boot AV is fixed,
[[project-boot-crash-rw-nullderef-not-display]]).

### C. DATA-VERIFIED — format cracked + checked against the real asset bytes
Not C4 (no function), but verified that the PARSE matches the original data:
- RWS audio 0x809 + 0x80d IMA ADPCM — decoded WAVs + autocorr 0.84/0.96 vs noise.
- Course_Id<->area table — read directly from each COURSE.LUA.
- POWERUPS_GOLD.LUA placement — independent re-parse cross-check (below): MATCH.

## Verification run this session (data level)

- POWERUPS_GOLD.LUA (Arctic): C++ parser = 18 spawns; independent Python re-parse
  cross-check = 18, positions/types identical (see the cross-check log). PASS.
- (Audio) cdaudio 0x80d -> IMA -> WAV: autocorr +0.96 (real music), rate 44100
  from the 0x80e header. PASS (decode validity).

## Path to paying down the debt
1. Boot-original `diff-original` lane (run_diff.py) for the category-B verbatim
   pieces -> first real C4 datapoints from this session's ports.
2. As each scaffold (A) is replaced by a verbatim port, move it A->B and C4 it.
3. Category-C data parsers stay "data-verified" (no function to diff).
This audit is the item-5 starting point; see re/analysis/C4_REVALIDATION.md for
the historical C4 debt tracker.
