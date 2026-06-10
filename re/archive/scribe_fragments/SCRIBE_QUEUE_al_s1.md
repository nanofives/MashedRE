# SCRIBE_QUEUE fragment — batch_al session 1 (al_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  al_s1  bucket=re/analysis/bucket_gameplay_00405400_00407620  confidence=C1->C2  rvas=00405400,00405430,00405540,00405730,00405780,004058b0,004058c0,004058e0,00405920,00405ab0,00405f60,00406070,00406130,00406160,00406370,00406410,004064c0,00406950,004069d0,00406ae0,004074a0,00407550,00407580,004075a0,004075b0,00407620

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. None drift-skipped
  (all were C1 gameplay in hooks.csv at session start; none already C2+).
- **Pool slot ACTUAL**: pre-assigned Mashed_pool0 was POISONED by a leaked
  in-JVM project lock (a parameterless `project_program_open_existing` errored on
  the missing `program_name` AFTER acquiring the lock — the known "MCP leaks
  project lock on failed open" mode; pool0 now carries a `.lock` with 0 server
  sessions). Fell back to **Mashed_pool1** (verified free, opened read-only,
  `program_close`d cleanly). Recorded in `.pool_slot_al_s1`. Do not `acquire`
  pool0 until the shared server restarts.
- **Subsystem confirmation**: all 26 CONFIRMED `gameplay` (no reclassifications).
  The "early game-state/setup" prompt hypothesis is too narrow — the bucket is a
  cohesive **particle/projectile-effect + AI-special-weapon cluster** built on
  two fixed-stride entity arrays plus a timed-event table and a "camera/active
  record" singleton:
  - **Subsystem A** @ `DAT_00639d80`, stride **0xec** (0x3b dwords), count
    `DAT_0063a5d0`: per-instance records with a tuning-default block (1.0/0.95/
    0.6/1.4/0.05/0.09/10.0/0.1/0.2/100.0) + a variable-length int list at +0xb4
    and a +0x44 key. Allocated by 0x00405780; health/durability field +0x5c
    decremented-by-key via 0x004058b0/004058c0/004058e0; index/key/count/pos/basis
    accessors 0x00407550/00407580/004075a0/004075b0/00407620; whole-array destruct
    via 0x004074a0.
  - **Subsystem B** @ `DAT_0063a490`, **10-slot ring**, stride **0x20**, head
    `DAT_0063a5dc`: projectile/tracer/particle spawner. Spawn 0x00406070, destruct
    walk 0x00406130, per-tick integrate+collide+impact 0x00406160, mass reset
    0x00406370. Also wiped by 0x004074a0.
  - **Timed-event table** @ `DAT_0063a220` (per-bucket, 10×0xc records) with counts
    `DAT_0063a478`: add 0x00405730, per-bucket tick 0x00406950.
  - **Active-record singleton** `DAT_00639d70..78`: reset 0x00405400, predicate
    0x00405430, RW-matrix angle decomposition (roll/pitch/yaw, debug-prints) 0x00405540.
  - **AI special/weapon tick** (this-ptr style, cooldown + LOS/range/angle gates):
    LOS+range+angle test 0x00405920, burst emitter 0x00405ab0, fire-or-cooldown
    0x00405f60, cooldown-gated spawner 0x00406410, two-state destruction/effect
    updater 0x004064c0, front/behind distance param 0x004069d0, master per-instance
    tick 0x00406ae0.
- **RW resolution noticed (callee, not a bucket member)**: 0x004074a0 now
  decompiles with **`RpClumpDestroy`** where the 2026-05-18 C1 plate had
  `FUN_004e6e00` flagged `[UNCERTAIN] RpClumpDestroy candidate`. The master project
  resolved it since; the new plate records the confirmation.
- **Branch-idiom resolution**: 0x00406410's cooldown gate
  `(*(this+0xa0) < c) == (*(this+0xa0) == c)` decodes to `*(this+0xa0) > c`
  (countdown while positive; fire+reset when ≤0). The C1 plate had this inverted
  and marked UNCERTAIN — now resolved.
- **Decomp refinement**: 0x004064c0's per-call RGBA is **two** PRNG byte calls
  (`FUN_004a2c48`) + G=copy-of-R + fixed A=0x80 (the C1 plate said "three byte
  calls").
- **library_skip**: none — every bucket member is a game function (function_at +
  decomp succeeded for all 26). Notable CRT/math callees referenced but not
  plated: `FID_conflict__wprintf` (0x00405540 debug print), `FUN_004a2b60`
  (struct/string init), `FUN_004a3384` (acos candidate), `FUN_004a2c48` (PRNG
  byte), `FUN_00472650`/`FUN_00472690` (random-in-range float/int).
- **needs_function_create**: none (all 26 have function objects).
- **Uncertainties**: filed as **bare `[UNCERTAIN]`** in the plates (NOT minted to
  U-IDs) per author-only mission. Recurring open items for central minting:
  effect-dispatcher cluster (FUN_00465ca0 / FUN_00465e80 / FUN_0045dce0 /
  FUN_004661a0 / FUN_00476860 / FUN_00487020 / FUN_0048f780 / FUN_00465940 /
  FUN_0048aa20 / FUN_0046bf50 / FUN_0046d740 / FUN_00453eb0); the
  collision/raycast triple FUN_0045bfe0 + FUN_004b4cd0/FUN_004b4d10 + FUN_0045c350;
  FUN_004173a0 (RNG/accessor, polymorphic signature); FUN_004c39b0 (RwV3dNormalize
  candidate, adjacent to the C3 Vec3Magnitude 0x004c3ac0); FUN_00458e00 /
  FUN_00458f20 / FUN_0045c330 (timed-event helpers); FUN_004c0ed0 / FUN_004c1340 /
  FUN_004c1480 / FUN_004899c0 / FUN_00489a40 / FUN_00489c30 / FUN_00489c60 /
  FUN_0047d150 / FUN_0057c210 / FUN_0055ac00 / FUN_0047cf20 / FUN_0047f290 (entity/
  clump/anim APIs). All are callees; non-blocking for C2 of the bit-identical leaves.
- **Stubs**: none encountered/minted this session.
- Files: bucket dir + this fragment committed this session (atomic path-scoped).
  `.pool_slot_al_s1` left untracked (session marker).

> DRAINED by sweep-20260603-0334 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
