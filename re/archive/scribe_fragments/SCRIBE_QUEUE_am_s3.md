# SCRIBE_QUEUE fragment — batch_am session 3 (am_s3)

Author-only promote-c2 pass (gameplay campaign 2/~5, third slice 0x00421100..0x004223f0).
Bucket plates are the C2 deliverable; central finalize (ghidra-sweep + re-classify)
writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s3  bucket=re/analysis/bucket_gameplay_00421100_004223f0  confidence=C1->C2  rvas=00421100,00421160,004212b0,00421560,004215e0,00421630,00421690,004216b0,00421720,00421750,00421800,004218b0,004218d0,00421930,00421980,00421a90,00421ba0,00421c10,00421c50,00421c80,004220d0,00422140,00422160,00422290,004223b0,004223f0  note=subsystem_observed=render(debris-clump+smoke-particle effects) for 25/26 (recommend reclass gameplay->render); 004223f0 = util/math PRNG-float leaf (recommend gameplay->util, weak/optional); resolved RW imports RpAtomicGetWorldBoundingSphere(004216b0) RpClumpDestroy(00421c80) RpClumpForAllAtomics(004223b0); needs_function_create=NONE; library_skip=NONE; actual pool slot=Mashed_pool3

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** —
  all 26 were `gameplay/C1/mapped` per hooks.csv at session start (existing C1 plates
  in `re/analysis/bucket_0041e140/`); none already C2+. (Neighbors 0x004219c0 render-C2,
  0x004219a0 ai-C2, 0x00422120 util-C3, 0x004222d0 util-C2, 0x00422470 frontend-C2 are
  NOT in the candidate list — no collision.) Every plate uses `## Mechanical description`
  and `confidence_target: C2`; uncertainties are **bare `[UNCERTAIN]`** (NOT minted —
  sweep owns U-IDs, batch_am range U-8000..U-8299).

- **POOL SLOT**: pre-assigned **Mashed_pool3** opened cleanly (top-level
  `mashed_pool/Mashed_pool3.gpr`, no stale `.lock`; a separate nested
  `mashed_pool/Mashed_pool3/` clone exists and was left untouched). Opened read-only,
  `program_close`d cleanly. Recorded in `.pool_slot_am_s3`. No leaked-lock incident.

- **SUBSYSTEM — the "gameplay" hypothesis is WRONG for 25/26; recommend reclass to
  `render`** (debris-clump + smoke-particle *effects / rendering*). This slice is two
  cohesive RenderWare effect subsystems plus one stray math leaf — not game logic.
  Immediate caller `FUN_004219c0` is already classified **render** (`render_frame_d4`),
  which drives `FUN_00421720` over the 4-slot 0x208 debris pool. Two clusters:
  - **Smoke-particle ring** (128 entries, stride 0xb dwords at `&DAT_0063e590..`,
    cursor `DAT_0063fb88`, active bitmap `DAT_0063e4a8`, emitter handle `DAT_0063e548`):
    color-by-age `00421100`, spawn `00421160`, per-frame update+immediate-mode submit
    `004212b0` (calls Vec3Magnitude C3 `004c3ac0`; submit chain `0047xxxx`), renderstate-
    toggle render wrapper `00421560`.
  - **Debris-clump pool** (4 slots, stride **0x208** at `&DAT_0063fb90`, end `0x6403b0`;
    loads `debris0.dff..debris5.dff` via 6-entry table `PTR_s_debris0_dff_005f6124`):
    pool init `004220d0` → per-slot init `00421ba0` → per-lane atomic-split `00421a90`
    → create/attach child atomics `004218d0`/`004218b0`; per-frame visibility-cull+render
    `004216b0` + 2-lane wrapper `00421720`; per-frame lane fade/event tick `00422160` +
    2-lane wrapper `00422290`; frame-LTM writers `00421750`(vec3)/`00421800`(matrix);
    atomic geometry getter `00421930`; world-detach `00421980`; transform-and-forward
    `00421630`→`004215e0`; thunk `00421690`→FUN_00421060; AABB-fold `004223b0`;
    soft tear-down `00421c10` + 2-lane `00421c50`; hard tear-down `00421c80` + pool-wide
    `00422140`.
  - **Mixed note** `00422160`: dominantly render-bookkeeping (visibility-bit clear +
    material-color zero via `FUN_004c1340`), but its inactive-lane branch submits
    **gameplay event 0x21** (`FUN_00465c10(0x21, ownerId)`). Net recommendation render;
    flagged so re-classify can override to gameplay if event-submit is the gating signal.
  - **The one non-render**: `004223f0` is a standalone xorshift+quadratic-hash **PRNG
    float** leaf (no sibling here calls it; only caller is frontend `00422470`).
    Recommend **gameplay->util(math)**; weak/optional — keep gameplay if util-math
    leaves aren't being split out this campaign.

- **Named RW imports resolved in the master since the 2026-05-18 C1 plates** (now
  recorded verbatim in the plates):
  - `00421c80`: C1 `FUN_004e6e00` → **`RpClumpDestroy`**.
  - `004216b0`: C1 `FUN_004e6100` → **`RpAtomicGetWorldBoundingSphere`**.
  - `004223b0`: C1 `FUN_004e66d0` → **`RpClumpForAllAtomics`**.
  These are call targets (named imports), NOT promotion candidates.

- **Live constants decoded (Mashed_pool3, IEEE-754 LE)** — now in the plates:
  - `00421100` color thresholds: `_DAT_005cd2b4 = 12.5f`, `_DAT_005cc9e0 = 25.0f`.
  - `004212b0`: `_DAT_005cd2c0 = 6.2831855f (2π)` sway freq; `_DAT_005cd2b8 = 0xa0000000
    ≈ -1.08e-19f` sway amplitude (anomalously ~0 → sway effectively OFF at this tuning;
    reported raw + `[UNCERTAIN]`); `_DAT_007f100c = 0.0f` static (address in writable
    .data → runtime frame-dt); `_DAT_005cc318 = 0.6f`, `_DAT_005cc328 = 0.01f`,
    `_DAT_005cc32c = 0.5f`, `_DAT_005cc320 = 1.0f` (age "expired" cap),
    `_DAT_005cc9c0 = 0.2f`, `_DAT_005cc99c = 0.3f`. `DAT_005d757c = 0.0f` (shared zero
    sentinel, consistent with [[project_batch_ag_swept_reclassify_pending]] batch_al).
  - `004223f0`: `_DAT_005cd314 = 2^-31 = 1/0x7fffffff = 4.6566e-10f` → **confirms** the
    PRNG returns a float in [0,1) (resolves the C1 plate's open scale hypothesis).

- **Shape recovered from decomp (not just C1 carry-over)**:
  - `004215e0`: the two `if/else` arms call `FUN_00420de0(param_2)` with **identical**
    args — codegen artifact; net effect is unconditional `FUN_00420de0(param_2)`, the
    `*(param_3+8)>0.0` test is dead for the result.
  - `00421ba0`: arg-less first `FUN_004b6520()` then `FUN_004b6520(local_50,0x50)` is a
    Ghidra arg-recovery artifact of one stack memset (asm: `push 0x50; lea eax,[ebp-0x50];
    push eax; call`).
  - `004223b0`: callback `LAB_004222f0` is an **unpromoted** function (Ghidra labelled
    LAB, not FUN) — the per-atomic AABB min/max fold; not in the candidate set.

- **needs_function_create = NONE** — all 26 returned valid function objects (decomp
  succeeded for all). **library_skip = NONE** — slice is below the CRT / D3DX9-PSGP /
  qhull bands; no RVA decoded as a named library primitive (only the three named RW
  *imports* above, which are call targets).

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no
  hooks.csv write.** Per author-only mission only the bucket dir, this fragment, and
  `.pool_slot_am_s3` were created/modified.

> DRAINED by sweep-20260603-0427 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
