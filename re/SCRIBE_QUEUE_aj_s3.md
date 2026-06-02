# SCRIBE_QUEUE fragment — batch_aj session 3 (aj_s3)

Author-only promote-c2 pass (track cluster). Bucket plates are the C2 deliverable;
central finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

Pool slot: Mashed_pool2 (pre-assigned; opened read-only, closed cleanly).

## Queued

2026-06-02  aj_s3  bucket=re/analysis/bucket_track_00401630_0047c0f0  confidence=C1->C2  rvas=00401630,0040cea0,0040d020,0040d110,0040d440,00420230,00420420,00426340,00462500,00462510,00478200,00478cf0,00479030,004790e0,00479330,0047a0f0,0047a1e0,0047ab30,0047abd0,0047b980,0047b9e0,0047bcc0,0047be80,0047bf70,0047c0b0,0047c0f0

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. None drift-skipped — all 26
  were `C1` in hooks.csv at session start (verified by exact first-column lookup; the
  grep false-positives for 00401630/0040cea0/00420420/00462500/0047bf70 were resolved —
  each IS its own C1 row).

- **Subsystem confirmation**: 24 of 26 CONFIRMED `track` (no reclassification). They form
  the track/world load → collision-geometry → COURSE.LUA pipeline:
  - **Track/vehicle load (early cluster 0x0040–0x0042)**: 00401630 (LoadMapModel
    "map_%d.dff"), 0040d020 (load-by-index thunk), 0040d110 (post-load per-player assign,
    group base `/6*6`), 0040cea0 (vehicle surface setup, dirt.piz), 0040d440
    (Course::LoadCurrent), 00420230 (vehicle piz path builder), 00420420 (per-player
    vehicle+checkpoint init, stride 0x2ac), 00426340 (KTCScript.lua loader).
  - **Course asset loader / teardown (0x0047x)**: 00479330 (master CreateFromDescription,
    2893 B), 00478cf0 (full teardown), 00478200 (fatal-error exit), 004790e0 (post-load
    callback install) + 00479030 (the per-atomic callback it installs).
  - **COURSE.LUA handlers**: 0047a0f0 (Lua-init root, zero-fills 0x16264 B table),
    0047a1e0 (Sky_Filename), 0047ab30 (Setup_Fog), 0047abd0 (Modify_Fog), 0047b980
    (single-command register primitive).
  - **Collision geometry**: 0047bcc0 (half-edge adjacency), 0047be80 (face normals),
    0047bf70 (per-sector record from RpWorldSector), 0047c0b0 (zero-clear collision
    tables), 0047c0f0 (sector ingest). Stride 0x56 dwords / table `DAT_006bfca8`
    cross-checked across the trio + the hooks.csv byte counts (0x1580B/0x800B/0x40B/0x1DC4B).

- **TRACKER DRIFT — master already C2 ahead of hooks.csv (19 RVAs)**: every 0x0047x RVA
  carries a `[C2 2026-05-18]` master comment from the prior `promote_c2_render_midrva`
  (d11007-reclass render→track) session, yet hooks.csv still lists them `C1`. These are NOT
  drift-skips (hooks.csv = C1) — they are ready promotions; the sweep need only flip the
  csv C1→C2 and point `file` at the bucket plate (or keep the existing render_midrva
  canonical). Affected: 00478200, 00478cf0, 00479030, 004790e0, 00479330, 0047a0f0,
  0047a1e0, 0047ab30, 0047abd0, 0047b9e0, 0047bcc0, 0047be80, 0047bf70, 0047c0b0, 0047c0f0
  (15) + 0047b980 has no [C2] tag (sky_weather canonical) — treat normally.
  - **00479030 name change**: hooks.csv row is `LAB_00479030 ... "no Ghidra function object"`,
    but the master now has a real function object `FUN_00479030` (created via the 2026-05-18
    sweep create-fn). Sweep should update name LAB_→FUN_ and drop the "no function object" note.

- **SUBSYSTEM-BOUNDARY FLAG (needs a decision — 00462500 / 00462510)**: these two 10-byte
  flag setters (`_DAT_00603868 = 1` / `= 0`, the "audio course flag") are classified `track`
  in hooks.csv, but they sit *inside* the audio address cluster (batch_ah authored
  0x0042f760..0x00465b20 as `audio`; the immediately-adjacent 0x00462520 is an audio row).
  This author-only pass did NOT reclassify them — plates describe behavior mechanically and
  keep the existing `track` label. **The sweep / user should decide track-vs-audio.** Both
  are C2-eligible on behavior regardless of the label. (Plates: 00462500.md, 00462510.md.)

- **Stale-note correction (0047b9e0)**: the old render_c0_promote_b C1 note called it a
  "depth-3 callee… post-effects conditional (state≠7 && DAT_007f0f3c≠0); D-5056." Recovered
  body is a pure 3-instruction forwarding thunk to `FUN_00496c10` — no state/effects logic
  here. The 2026-05-18 master comment already supersedes; sweep should drop the stale prose.

- **Calling-convention / decompiler-recovery uncertainties (non-blocking, carry as data/CC)**:
  - 00420420: callers push a 2nd arg (`FUN_00420420(iVar5, iVar1+iVar2)` in FUN_0040d110) but
    the recovered signature is single-param `(int param_1)`; 2nd arg destination not visible.
  - 004790e0: decomp shows `(void)` but disasm proves 3 pushed args `(EAX, 0x479030, 0)` to
    FUN_004e5c70 (documented from listing).
  - 0047b980: `param_1` unused in recovered body; registers single command via param_2.
  - 0047bcc0: `FUN_0047bc90` returns 64-bit EDX:EAX consumed as (match_flag, slot_ptr);
    EDX-output contract not deep-verified (callee not decompiled this pass).
  - 0047be80: `unaff_EBX` (vertex base) supplied by caller FUN_0047bf70 via EBX.

- **Existing S/U/D IDs touched (NOT resolved — left to central sweep)**: S-3580..S-3583
  (0040cea0), S-3577..S-3579 (0040d110), S-0492/U-0496 (0040d440), S-3603..S-3611/U-3576
  (00420420), S-3601..S-3602 (00426340), S-0918/D-2625 (00478200), S-2497..S-2499/U-2493/
  U-2494/D-5744/D-5753 (00479030), U-1951/D-2626 (004790e0), S-0912..S-0918 (00479330),
  D-5056 (0047b9e0), U-2506/D-5752 (0047bcc0), U-2505/D-5751 (0047be80), S-0919/D-2640/
  U-1958..U-1961 (0047bf70), S-0919 (0047c0f0). **No new S-/U-/D-IDs minted this session.**

- **Cross-session note**: Session 4 (physics) shares the 0x0047–0x0048 region. The collision
  geometry functions here (0047bcc0/be80/bf70/c0b0/c0f0) are the *track-load-time* collision
  table builders (write `DAT_006bfca8`/`DAT_006c1228`); coordinate if Session 4 also touches
  these globals.

- Files left UNTRACKED until the session commit (bucket dir + this fragment + .pool_slot_aj_s3).
  No hooks.csv edit, no re-classify, no build, no Frida — per author-only mission.
