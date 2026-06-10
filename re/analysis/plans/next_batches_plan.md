# Next-batch plan: `batch_u` + `c3_batch_j`

Filed 2026-05-18 (post c3_batch_h + c3_batch_i + batch_t completion).

## Project state at planning time

- C4: 50 hooks (29 yesterday + 21 from main-menu sweep)
- C3: ~160 (today's c3_batch_i added 9 + 1 drift; minus 20 graduated to C4)
- C1 backlog: ~1,500 rows post-batch_t-promote-c2 drain
- C0/unmapped backlog: ~3,580 functions
- DEFERREDs blocking: D-11007 (subsystem drift — in cleanup agent now); D-11010/11/12 (harness gaps — in extensions agent now)

## Sequencing constraint

Both batches assume:
1. D-11007 cleanup agent has landed (frees subsystem-clean hooks.csv state)
2. Harness extensions agent has landed (frees ~5 new arg_types for c3_batch_j)
3. Skill upgrades for promote-c3-batch and discover-c1-batch (the 4 filter-implementation bugs reported by the c3_batch_i orchestrator) ideally land first

If D-11007 + harness extensions are in flight, generate `batch_u` first (Ghidra-side, no harness dependency). Generate `c3_batch_j` after the harness branch merges.

## `batch_u` — Ghidra-side discover-c1-batch (next surface-mapping cycle)

**Production shape:** 6 sessions × 60 RVAs (Opus 1M cap, matching batch_t)
- 4 first-pass sessions × 60 = 240 C0→C1
- 2 promote-c2 sessions × 40 = 80 C1→C2
- Total: ~320 RVAs

**CRT-band exclusions (per upgraded skill):**
- 0x004a0000..0x004b3fff (original CRT band from skill upgrade)
- **NEW band found this session: 0x005c1920..0x005c281e** (MSVC 2003 CRT residue — second half of cluster_005c). Add to skill's exclusion list in next upgrade.

**First-pass bucket candidates (highest unmapped density excluding CRT + drained):**
- 0x005b???? — adjacent to qhull/audio (likely audio remainder), 185 unmapped
- 0x004f???? — adjacent to csl pipeline (more RW middleware?), 157 unmapped
- 0x0055???? — unknown midrange, 155 unmapped
- 0x0048???? — unknown, 137 unmapped
- 0x0047???? — track_loader / physics range per s6 bucket-rename finding, 139 unmapped (already partially covered; could target the higher portion)
- 0x005c1920+? — NO, this is CRT band per the discovery; exclude

**Pick 4 for first-pass:** 0x005b, 0x004f, 0x0055, 0x0048 (spread across binary; highest density).

**Promote-c2 bucket candidates:**
- Audio C1 (deep pool 158 → smaller after batch_s s6 drain): target 0x005a??? remainder OR mid-band 0x0046xxxx (some audio there)
- Vehicle C1 highrva (untouched by batch_t s5 lowrva): 0x0047xxxx..0x004axxxx vehicle code
- Boot C1 (untouched): 135 C1, target 0x00492xxx..0x00496xxx range
- AI C1: 61 C1 total — small pool, might fit a 40-row session

**Pick 2 for promote-c2:** boot_lowrva + vehicle_highrva (largest untapped pools).

**Subsystem-prediction caveat (per upgraded skill):** label buckets RVA-anchored; let agents report observed subsystem.

**Recommended generation prompt:** invoke `discover-c1-batch` with N=6, mix=4-first-pass+2-promote-c2, K_first_pass=60, K_promote_c2=40. Skill auto-generates with CRT exclusion + .pool_slot_session_u_sN per-session naming.

## `c3_batch_j` — Frida-side promote-c3-batch (post-harness-extension cycle)

**Production shape:** 6 sessions × 10 candidates = 60 promotions (upsized for Opus 1M)

**Subsystem coverage (avoid frontend; spread across pools):**
- audio (depth: ~20 C2, several unblocked by new fmt_desc_pair_compare arg_type)
- save (depth: low post-c3_batch_i drain; might fit 1 session of ~5 candidates)
- vehicle (deep pool, post-batch_t s5 + s6 expansion)
- input (depth: ~30 C1, smaller C2 pool; some unblocked by eax_implicit_ptr)
- boot (deep pool, untouched since batch_g)

**Filter calibration baseline (per c3_batch_i):**
- Inline UNCERTAIN body (cataloged-U-ID exempt): saves ~25-30% of candidates from refusal
- Callee-at-C2+ gate: dominant eliminator (89% of refusals in c3_batch_i)
- arg_type compat: was 0% of refusals in c3_batch_i — should be 0 again post-harness-extensions
- Stale-impl drift: surfaced 1 row in c3_batch_i (VfsFileExists); expect 1-3 in batch_j

**Yield projection:** c3_batch_i was 50%; with arg_type gap removed and post-cleanup subsystem clarity, target 60-70%.

**Session-bucketing strategy:**
1. audio_c3_j1 — RWS leaves + fmt comparators (5-10 candidates)
2. audio_c3_j2 — RWS dispatchers (5-10 candidates)
3. vehicle_c3_j3 — vehicle_lowrva leaves
4. vehicle_c3_j4 — vehicle_lowrva near-leaves
5. boot_c3_j5 — boot lifecycle helpers
6. input_c3_j6 — DInput leaves (some EAX-passing, now unblocked)

**Recommended generation prompt:** invoke `promote-c3-batch` with N=6, K=10, subsystem=audio+vehicle+boot+input, complexity=mixed. Skill applies the 4 filters + new arg_type list.

## Order of operations (when ready)

1. D-11007 cleanup branch lands → `git merge chore/d11007-drift-cleanup`
2. Harness extensions branch lands → `git merge feature/harness-arg-types`
3. Skill upgrade pass: incorporate the 4 filter-implementation bugs from c3_batch_i orchestrator (~30 min)
4. Generate `batch_u` (invoke discover-c1-batch skill)
5. Spawn 6 batch_u fanout agents (background)
6. While batch_u runs: generate `c3_batch_j` (invoke promote-c3-batch skill, will run viability filters)
7. Spawn 6 c3_batch_j fanout agents (background)
8. As they complete: ghidra-sweep → frida-sweep → push

Wall time estimate: batch_u + c3_batch_j cycle is ~3-4 hours human-attended parallel work.

## Cross-batch dependencies

- `c3_batch_j` candidate selection BENEFITS from D-11007 cleanup completion (cleaner subsystem fields = clearer filtering)
- `c3_batch_j` REQUIRES harness extensions to be merged (else the new arg_types won't be in diff_template.js for the fanout sessions)
- `batch_u` is independent of both

If only one branch lands in time: run batch_u alone. If both land: full sequence.
