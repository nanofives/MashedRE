# Harness capability backlog — standing between-slices work

Created 2026-07-06 (foundation reset). Companion to `re/analysis/RE_MASTER_PLAN_2026-07.md` §6/§7.

**Why this list exists:** the 2026-06-09 audit found that *harness capability work is the only
reliably high-yield C3 lever* (harness-ext sessions 8/9, scenario-attach 8/8) after the flat
C2→C3 batch lane mined out. When a milestone slice stalls (blocked on a decision gate, a locked
desktop, another session's capture window, or plain fatigue), pull ONE item from here instead of
idling or reviving dead batch lanes.

**Rules**
- Pull one item, finish it, log the outcome in `re/analysis/CHANGELOG.md`, return here.
- Items are ordered by measured ROI (AUDIT_2026-06-09 + the c2c3 pipeline lessons).
- Refresh this list at milestone boundaries, same cadence as the master plan §1 counts.

## Backlog (ROI order)

1. **New `arg_type` handlers** — the #1 measured lever. Extend `re/frida/diff_template.js` for
   the bespoke-arg-shape long tail that blocked the last C2→C3 rounds; regenerate the index after
   every addition (`py -3.12 scripts\gen_arg_types_index.py`; lookups go through
   `re/frida/ARG_TYPES.md`, never the 232 KB template). Candidate source: rows whose promotion
   blocker was recorded as missing/unsupported arg shape (`promote_frontier.tsv`, c2c3 lessons).
   - **NEXT PULL (identified 2026-07-24, account2):** build the **x87 80-bit ST0 float-return
     capture handler** — it unlocks 6 of the 9 named-shape frontier rows (3 sin-getters
     0x00431b20/b50/b60 + 3 RwV3d bbox accessors 0x004c4270/42d0/4360) AND is the same capability
     veccap item 6 defers `FUN_005667c0` for. Full refined frontier classification (which rows are
     handler-blocked vs veccap-onboardable vs bespoke) in
     `re/analysis/plans/frontier_shape_refinement_2026-07-24.md`. Also flagged: 0x004b4550 (3D
     centroid) is a veccap-onboardable (item 6, offline, no Frida). Authoring the handler needs
     Frida to verify → account3 (account2 produced the target analysis only).
2. **Callee-gate cascade** — when a leaf lands C3, re-check its C2 callers whose only failing C3
   gate was "callee below C2+": each new C3 leaf can unlock a caller chain. Mechanical scan;
   route to a cheap model.
3. **Scenario-harness extensions** — extend `re/frida/scenario_launch.py` (warp / `--oracle` /
   `--poke-lap`) to race states it can't reach yet, and generalize the A/B snapshot/restore
   orchestrator lane (`AiControllerAB.cpp` pattern: window snapshot + RNG-ring restore +
   retry-once) to other live-state orchestrator families. This is what made the WS-R6 AI chain
   drain possible.
4. **Discovery drains** — demand map §4 lists **1,788 undiscovered RVAs** inside the race slice's
   static call-closure (no hooks.csv row). Run `ghidra-sweep` SCRIBE_QUEUE drains (11 queued as
   of 2026-07-03) and demand-map discovery sessions; budget discovery alongside promotion.
5. **Parity-harness coverage** — keep `nav_coverage.py` green on any newly wired screen; extend
   `drawlist_diff.py` / `imgdiff.py` recipes where new visual surfaces land (see
   `re/analysis/parity_tooling.md` "Known asymmetries" before trusting a RED).
6. **veccap offline verification lane (BUILT + registry-driven, 2026-07-16 — `re/tools/veccap/`)** —
   one live capture (args + read-region snapshot + in-process ground truth), then unlimited
   offline iteration via (a) the ported-TU replayer exe and (b) the Unicorn original-code differ
   (x87 verified bit-exact). Registry-driven (`veccap_registry.py`): the RW fast-sqrt family
   (6 fns / 3 signature kinds) is onboarded and GREEN (offline 6/6 both modes, Unicorn 6/6).
   **Next pulls:** (a) onboard B5e-class integrator/solver math leaves — **first 3 done 2026-07-17**:
   the pure void-return AABB/vector leaves `FUN_00566830` (perp-vector), `FUN_00565ef0` (AABB
   min/max merge), `FUN_00565fa0` (AABB span+scalar) from `r7/b5e-solver-island` cluster K1. Needed
   two new registry kinds (`v_out_in`, `v_out_2in` — void return, out-buffer compare; the 3 existing
   kinds are all float32-return so none fit) + the ported TU `Collision/RwpSolverLeaves1.cpp` copied
   verbatim from b5e (replay build needs the source; it is not yet on main). Result: **offline replay
   9/9 PASS both modes, Unicorn 9/9 PASS** (513 synthetic vectors each; menu-idle never calls physics
   so live_capture=False — ground truth via in-process original call). Evidence = per-leaf bit-identity
   toward C3, NOT the diff-original gate; leaves stay C1/main, C2/b5e. Remaining K1 leaves are pointer/
   list/`rand()` glue (out of veccap scope); the float10-return normalize `FUN_005667c0` is deferred
   (needs 80-bit ST0 return capture). **UPDATE 2026-07-24 (account2):** onboarded the 3 K2
   mat→quat Shoemake branches `FUN_00546bf0/c50/cb0` (`v_out_in` 11→4, `Collision/RwpSolverMath2.cpp`
   on main) — Unicorn 12/12 PASS + replay 12/12 both modes. These are the first **float10-chained**
   leaves → **closes the README Pilot-2 Unicorn caveat for float10 callees** (FSIN/FCOS transcendentals
   still open). NOTE the "registry entry only" claim held only for ≤3-float `v_out_in`; a wider leaf
   needed `synth_inputs` generalized for n_in>3, and a `.rdata`-const reader needs a `STATIC_READS`
   append + a `replay_offline.cpp` extern+`kExports` row (unicorn maps the whole image → no change).
   FUN_00566200 (AABB×matrix, `v_out_2in` n_in=22) was TRIED 2026-07-24 → **FINDING VECCAP-2**: the
   port is not x87-faithful (Unicorn PASS, replay FAIL 490/513 even bounded); buffers were widened
   (kept) but the entry is backed out pending a disasm-order/float10 re-transcribe on account3
   (`re/analysis/plans/veccap_finding_2026-07-24.md`). (b) wire a `capture_vectors` live
   arg-collection scenario for in-race functions (menu idle doesn't call physics — reuse
   `scenario_launch.py` to reach a race before the collect window); (c) TTD *recording* elevation lane: DEFERRED by owner 2026-07-17 ("the other two lanes are
   enough") — do not re-raise unless a task specifically needs fresh TTD tapes; query side
   already scripted on `log/ttd/*.run`. Finding VECCAP-1 (LUT validator rejected high-heap layouts → silent CPU
   fallback in 4 Math TUs) RESOLVED via Math/RwLutGuard.h; re-verified.

- **Chunked PHASE 2 install in `c4_navigate_batch.py` (`--on-chunk`)** — rescued 2026-08-14 from
  branch `harness/ext-ag-arg-types` (commit `1b8ae10b`) immediately before that branch was deleted.
  The branch was deleted as a net regression (it dragged a -238-line `hooks_registry.py` revert),
  so **re-author against main; do NOT try to recover the commit** — it is unreachable now, which is
  why the snippet is reproduced here verbatim rather than cited.

  Today main installs every RVA at once (`re/frida/c4_navigate_batch.py`, PHASE 2:
  `on_all = H.run(",".join(rvas), rvas, ...)`), so one bad hook in a large batch means a crash and
  a full-range bisect. The branch installed in groups of `--on-chunk` (default 16) and bisected only
  *within* the crashing group:

  ```python
  on_chunk = int(a[a.index("--on-chunk")+1]) if "--on-chunk" in a else 16
  bad = set(); jmp = {}
  for gi, group in enumerate(chunked(rvas, on_chunk)):
      res = H.run(",".join(group), group, None, nav, settle, dwell, seconds,
                  shot=(f"{shotdir}/c4nav_ON_g{gi}.png" if shotdir else None),
                  shotat=(seconds*0.6))
      if res["crash"] is None and res["alive"]:
          jmp.update(res["jmp"])
      else:
          gbad = bisect_bad(group, nav, settle, dwell, seconds)
          bad |= gbad
          good = [r for r in group if r not in gbad]
          if good:
              jmp.update(H.run(",".join(good), good, None, nav, settle, dwell, seconds).get("jmp", {}))
          for b in gbad: jmp.setdefault(b, None)
  ```

  Requires a `chunked()` helper (not on main). [UNCERTAIN] The branch version was never run to
  completion on record, so the ROI is reasoned (smaller blast radius per crash), not measured —
  treat the default of 16 as untuned.

- **T-ARCTIC — pose-matched ORIGINAL Arctic reference. BLOCKS shipping `race/geomlight`.**

  The librw prelit-ambient-fold fix (`race/geomlight` a62ee92c) takes TRAINING from
  18.47 mean / 42.35% over-threshold to **15.45 / 33.48%** (independently reproduced
  2026-08-30), and is RW-correct: `ROAD.DFF` carries GEOM flags `0x2008b`, no
  rpGEOMETRYLIGHT, so per RW it receives no runtime ambient. But it also darkens the
  Arctic sea (`0x1000f`, same non-lit prelit class) from lum ~24 to ~11, and there is
  **no pose-matched original Arctic frame** to say whether that is right. That single
  capture is the whole gate.

  **RESOLVED 2026-08-31 (branch `race/arctic-cap`).** A pose-matched ORIGINAL Arctic
  in-race reference was captured. Deliverables in `verify/arctic_ref/`:
  `orig_arctic.bmp` (confirmed by eye: night storm, rain, harbour, mountain, 4-car
  light cluster center-frame — an ARCTIC.PIZ in-race frame, not a menu),
  `orig_cambasis.txt` (12-float same-frame basis), `orig_lens.json`
  (viewWindow 0.60/0.45, fovy 48.46, near 0.10, far 70, `recip_ok`+`setupfov_ok`),
  `orig_frame.json`, `orig_track.txt` (ARCTIC, one piz open), plus
  `orig_arctic.bmp.draw3d.json` (draw_calls 115 / prims 43613 / verts 28406).

  **How Arctic was reached (resolves U-9059).** Arctic is unlock-gated in
  `original/gamesave.bin`, so the whole path was to (a) unlock a cup row on a SAVE
  COPY only and (b) drive the mode-3 Challenge-Cup flow to the right entry:

  1. Unlock: `re/tools/gamesave_edit.py <copy> -o <edited> --rows 0,1,2,3 --set c1=1,c11=1`
     flips the championship-span launch gates (col1 = mode-3, col11 = mode-10) on the
     4 Bronze-cup rows. Every ORIGINAL launch was wrapped in
     `re/tools/run_with_unlocked_save.py <edited> -- <cmd>`, which swaps the save for
     ONE command and restores + sha-verifies the reference in a finally.
     `original/gamesave.bin` was **never** hand-edited and is pristine at the end
     (sha `bd18788182b2343e5203eb98…`, re-checked after the run).

  2. Challenge-select index global = **DAT_0067f17c** (NOT the per-depth cursor
     `0x0067ee80` that `setsel()` writes — writing that had no effect, matching the
     2026-08-30 negative). With rows 0-3 unlocked the entries become selectable:
     down (code 12) steps `DAT_0067f17c` 0→1→2→3 and up steps it back, capping at 3,
     and the on-screen highlight + track preview follow it
     (`verify/nav_shots/chall_step{0..3}.bmp`). The launch consumes this index as
     `track` in `(&DAT_007f0a40)[FrontendModeIndex(mode)+track*0xc]`.

  3. **Bronze Cup 1 row/entry → area(.piz) map** (behaviourally confirmed, each entry
     launched under the unlocked save and the loaded `TRACKS\*.piz` read from
     `CreateFileA/W`):

     | challenge index (`DAT_0067f17c`) | preview | loaded .piz |
     |---|---|---|
     | 0 | dusty canyon (Battle, 1 opp) | **TRAINING.PIZ** |
     | 1 | desert arena (Battle, 2 opp) | **EGYPT.PIZ** |
     | 2 | snow (Battle, 3 opp) | **NEUSTEIN.PIZ** |
     | 3 | night storm / harbour (Race, 3 lap) | **ARCTIC.PIZ** |

  So a `col1=1` span edit on row 3 + navigating the challenge index to 3 is sufficient
  to reach Arctic — no cup progression, no reference-exe mod. `race_draw_burst.py` gained
  a `--challenge N` argument that forces the mode-3 flow and reaches entry N by N
  down-presses; the capture command was:
  `run_with_unlocked_save.py <edited> -- race_draw_burst.py --challenge 3 --settle 4.0 --out verify/arctic_ref/orig_arctic.bmp`.

  [UNCERTAIN] sub-frame roll drift: the basis is read via Frida one frame before the
  shim dumps the BMP, and the race camera rolls on a 1024-tick sine
  (memory `race-camera-rolls-30deg-sine`), so the basis and BMP can differ by ~1 frame
  of roll. This is the same same-frame method `race_draw_burst.py` uses for TRAINING;
  the drift is small but not zero.

  `race/geomlight` can now be gated on the Arctic-sea comparison against this reference.

  **geomlight GATE RESULT 2026-08-31 (`verify/arctic_ref/geomlight_cmp/RESULT.md`): FAIL —
  do NOT merge `race/geomlight` unscoped.** Ran its `mashed_re.exe` on Arctic
  (`MASHED_TRACK_SEL=0`) with three transplanted original bases, fold-removed (default) vs
  fold-restored (`MASHED_LIBRW_AMBFOLD=1`). Over the fold-affected `0x1000f` sea mask on
  two sea-dominant vantages: original sea luma 27.9 / 32.5; fold-removed (geomlight) 9.1 /
  9.8 (Δ18-23, crushed to near-black); fold-restored 30.1 / 27.7 (Δ2-5, matches). Visual:
  `sea_search/s8_3way_orig_geomON_geomOFF.png`. The blanket fold removal is right for the
  road (`0x2008b`, TRAINING's win) but darkens the Arctic sea (`0x1000f`) far below the
  original. Path: scope the fold by geometry class (drop for `0x2008b`, keep for
  `0x1000f`) and re-run. [NOTE] the START-GRID frame gave a false "supports shipping" read
  (sea only 2.5% of view, occluded by the foreground player car); always judge this on a
  sea-dominant pose.

  **CLASS-SCOPED FOLD PROTOTYPED + VALIDATED 2026-08-31
  (`verify/arctic_ref/geomlight_cmp/PROTOTYPE.md`, patch
  `verify/arctic_ref/geomlight_scoped_fold.patch`).** New `MASHED_LIBRW_AMBFOLD_SEA=1`
  folds ambient into the water class only (`numTexCoordSets<=1` = `0x1000f`), leaving the
  road (`0x2008b`) unfolded. Results: TRAINING trON 15.45 / trSEA 15.45 (geomON-vs-geomSEA
  = 0.000, byte-identical — road untouched, win kept); Arctic sea geomSEA luma 30.1/27.7 =
  fold-all = matches original (Δ2-5). So the fix keeps the win AND fixes the sea.
  **Next: parent applies the patch to `race/geomlight`, makes the scoped mode default,
  re-runs the parity harness on the other cup tracks, then merges.** T-ARCTIC's capture +
  gate + fix are done; the residual is the merge decision + a broad-track re-check.

  **BROAD-TRACK CHECK 2026-08-31 (`verify/geomlight_broadcheck/RESULT.md`):** the scoped
  fold is a byte-identical NO-OP on EGYPT (challenge 1) and NEUSTEIN (challenge 2) —
  geomON-vs-geomSEA = 0.000 on both, and both frames carry real terrain (sandy canyon /
  snow road) that already matches the original. So the scope key does NOT over-catch their
  terrain. Validated on 4 Bronze-Cup-1 tracks now: TRAINING (win kept), Arctic (sea fixed),
  EGYPT + NEUSTEIN (no-op/safe). Residual: the other 9 `kAreas[]` tracks (need a wider save
  unlock to reach) are still unchecked — any with a water body would be folded; parent
  should run the harness across them before shipping broadly.


## Done

- **T1 delegation-reach test — DONE 2026-07-06: SHELL-BLOCKED.** The account2 worker has no shell
  tool at all (Read/Grep/Glob only); the master plan §6 experimental lane is closed — verification
  and execution stay on account3. Verdict recorded in RE_MASTER_PLAN_2026-07.md §6/§7.
