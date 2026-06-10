# SCRIBE_QUEUE fragment — batch_ak session 6 (ak_s6)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  ak_s6  bucket=re/analysis/bucket_ai_00452eb0_004c3df0  confidence=C1->C2  rvas=00452eb0,00455b40,0045a0f0,0046c730,0046c750,0046cc10,0046d4a0,0046d510,0046d570,0046d6a0,0046d6d0,0046d780,0046d7f0,004715a0,004783f0,00478660,00484c70,0048a630,004c3df0  subsystem_observed=ai(all 19)  library_skip=NONE  needs_function_create=NONE  note=high ai cluster; zero library hits (matches batch header prediction); see boundary-flags + render-label-drift below

## Notes for the sweep

- **Count**: 19 RVAs, 19 plates authored in the bucket dir. **None drift-skipped** —
  all 19 were `C1` in hooks.csv at session start (verified per-row against hooks.csv);
  none already ≥C2. All 19 decompiled cleanly (`function_at`/`decomp_function`
  returned a real body for every RVA) — **no `needs_function_create` for any RVA**.

- **⚠ POOL SLOT — fell back off the pre-assigned slot.** Pre-assigned `Mashed_pool6`
  is **leak-locked**: its on-disk `.lock` (dated 2026-05-28) is OS-busy/undeletable
  ("Device or resource busy" from `ghidra_pool.sh release 6`) while
  `program_list_open` reported **0** open programs — the signature of a leaked in-JVM
  lock (memory `feedback_mcp_leaked_project_lock`). Did NOT `acquire` and did NOT use
  `pool4`. Fell back to verified-free flat-layout clone **`Mashed_pool9`** (no
  `.lock`, intact `.rep`), opened read-only; `program_list_open` confirmed identity
  (`project_name=Mashed_pool9`, `read_only=true`, image_base 00400000). Every plate
  records `opened_in_slot: Mashed_pool9`. `.pool_slot_ak_s6` records both. No master
  writes; `program_close` to be called at end of session.
  **Sweep note**: pool6 remains poisoned for the shared server's lifetime — avoid it.

- **Subsystem confirmation — hypothesis HOLDS; all 19 = genuine `ai` application
  code; ZERO library reclass-OUT** (matches the batch_ak header: "ai (57): ZERO hits").
  Callers cluster entirely in the AI band: race-tick `FUN_004103a0`, AI targeting
  `FUN_00414xxx`/`00415xxx`/`00416xxx`/`00417xxx`, powerup `FUN_00415220`, targeting
  helpers `FUN_00463xxx`/`00464xxx`, and the track loader `FUN_00426e10` (for the two
  load-time AI-polygon builders).

- **⚠ RENDER-LABEL DRIFT (informational — already fixed in hooks.csv, confirm on
  master):** `004783f0` and `00478660` still carry `subsystem: render` in their
  *existing C1 plate frontmatter* (`re/analysis/track_loader_d3/`, `track_loader_d2/`),
  but hooks.csv already lists both as `ai` (d11007-reclass render→ai, 2026-05-18). The
  pool9 decomp confirms **ai**: both operate on the AI-polygon group array
  (`param_1+0x16244` / `param_1+0xc1d8`) and `00478660` logs *"Number of AI polygons
  is %d"*. The bucket plates record `subsystem_observed=ai`. No reclass needed; the
  C1-plate frontmatter is stale (harmless — sweep reads the bucket plate).

- **⚠ BOUNDARY-FLAGS (keep label, surface for sweep) — 2 RVAs:**
  - **`004715a0`** — ai-vs-physics. hooks.csv reclassed physics→ai (d11007); a known
    CONFLICT note already exists in hooks.csv (one prior row labels it "Physics
    scenario BSP linker", another "AI pathfinding graph rebuild" — *same* data
    structures `DAT_0086ed20`/`DAT_0086cbcc`). Single caller `FUN_00426e10` (track
    loader). **NOT library** (app globals + app callee `FUN_0047ce40`). Best call = ai
    (hooks.csv canonical); plated to C2 with the boundary marked `[UNCERTAIN]`.
  - **`004c3df0`** — library-confirm target #2. **Resolved: NOT vendored library** — a
    thin app-side dispatch thunk `(**(code**)(DAT_007d3ffc+0x14+DAT_007d3ff8))(a,b,c,d);
    return a;` reading the application's RW/D3D device-state globals and calling a
    vtable method at slot +0x14. No `Rw*/Rp*/rwID_*` symbol, no FidDB tag, no
    provenance string → app glue, plate C2 (do NOT reclass-OUT). 16 callers spanning
    ai/vehicle/object/camera → genuinely **cross-subsystem glue** (arguably util/
    render-glue rather than ai-specific); kept `ai` per hooks.csv, boundary-flagged.

- **Library-confirm rule (Session-6 special, RVAs 004715a0 / 004c3df0): BOTH cleared
  as application code.** Neither is RenderWare/qhull/CRT/PSGP library. No
  `third-party-library[...]` tags this session; `library_skip=NONE`.

- **U-IDs / S-IDs — minted NONE** (author-only). Plates mark new holes as bare
  `[UNCERTAIN]` / "do NOT mint". Pre-existing IDs **referenced** (not re-filed):
  U-1889, U-1891, U-1892, U-1893, U-1434, U-2194, U-2195, U-2650, U-2651, U-3584,
  U-3586; S-2643, S-0908, S-0909, S-0910 (S-0911 = `004783f0`-as-finalizer, resolved
  by this plate). One direct-CALL callee not yet in hooks.csv: `FUN_00422b50`
  (0x00422b50, called by `0046d780`; D-10562 in the C1 plate) — sweep should file the
  S-ID centrally from the reserved range (U-7500..7699 / S-6200..6399).

- **Strong C3 candidates** (clean fully-determined leaves, no external data semantics;
  noted for a future c3 batch, NOT promoted here): `00452eb0` (1-load float getter),
  `00455b40` / `0045a0f0` / `0046c730` / `0046c750` / `0046d6a0` / `0046d6d0`
  (bounds-checked scalar getters), `00484c70` (count+base getter), `0048a630`
  (pure positional AABB/sphere overlap, no globals), `004c3df0` (deterministic
  indirect-dispatch thunk).

- Files left UNTRACKED for central finalize: the 19 bucket plates, this fragment, and
  `.pool_slot_ak_s6`. One commit only (no re-classify, no build, no Frida, no master
  writes) — per author-only mission.
