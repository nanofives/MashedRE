# SCRIBE_QUEUE — batch_ak Session 2 fragment (AUTHOR-ONLY)

## Queued

2026-06-02  ak_s2  bucket=re/analysis/bucket_vehicle_004820e0_00485420  confidence=C1->C2  rvas=004820e0,00482140,004826d0,00482730,00482820,00482860,00482900,00482930,004829d0,00482ae0,00482c10,00483a30,00483a40,00483ca0,00483d10,00483ed0,00483fa0,00484000,00484060,004840b0,004840d0,004840f0,00485340,00485360,00485370,00485380,00485420  note=subsystem_observed=vehicle for all 27 (vehicle replay/ghost subsystem + vehicle collision-mesh build + vehicle physics-joint pool + physics-object accessors). NO library_skips: although many RVAs call into the 0x004cxxxx RW-Physics band and the 0x004exxxx RW-geometry band (and 0x00482140 calls RpAtomicCreate), every function in this bucket is genuine application/vehicle code that orchestrates RW/physics APIs — none is itself a named Rw*/Rp*/Rt* primitive. No needs_function_create (all 27 are real function objects). Pool slot actually used: Mashed_pool1 (opened read-only, no on-disk lock, clean open+close). CORRECTION to carry into hooks.csv: 004826d0 prior inline note ("FUN_00482140 set gravity enable flag") is WRONG — 00482140 is the collision-mesh strip builder returning an RpAtomic; the DAT_005d757c<*(+0x4c) expression is 00482140's build-mode param, not a gravity flag. Uncertainties carried (no U-IDs minted): zeroed .rdata consts (_DAT_005cc320, DAT_005d757c), prior U-3699 (replay +0x198 reset flag), U-3700 (single-alloc free), U-1567/U-3575/U-3576 (struct-semantic gaps), easing polynomial identity in 00482c10.

## Notes for the sweep

All 27 plated as **vehicle C1->C2**. None drift-skipped (all were C1 in hooks.csv). NO library_skip RVAs. NO needs_function_create RVAs. Pool slot used = **Mashed_pool1** (read-only).

Sub-clusters within the bucket:
- **Replay/ghost subsystem (Time Trial mode):** 00482860 (Replay::Reset), 00482900 (Replay::GetSize), 00482930 (Replay::New), 004829d0 (Replay::WriteFrame), 00482ae0 (Catmull-Rom 3D spline), 00482c10 (Replay::ReadFrame, quat->matrix + SLERP + spline blend), 00483a30 (Replay::Rewind), 00483a40 (Replay::Free), 00483ca0 (Replay::Save), 00483d10 (Replay::Load). All confirm/deepen the replay_record + replay_record_cont1 notes. Replay struct re-confirmed: +0x00 magic 0x10b, +0x04 size, +0x08 player id, +0x0c duration, +0x10 scale, +0x14 frame buffer base, +0x18 ring head, +0x1c cursor, +0x20 tail, +0x168 frame count, +0x16c count-2 (index 0x5b), +0x170 write ctr, +0x174 end time, +0x178 first-frame time, +0x194 has-started, +0x198 reset-ready flag. Frame node = 0x24 bytes (quat[0..0xc], pos[0x10..0x18], next[0x1c], time[0x20]).
- **Vehicle collision/visual mesh build:** 004820e0 (2D dir->UV mapper), 00482140 (collision mesh strip builder -> RpAtomic; calls RW geometry band 0x004e8xxx + RwV3dNormalize 0x004c39b0 + RpAtomicCreate), 004826d0 (PhysicsCollisionBodyCreate: body + mesh + activate), 00482730 (set RGBA + lighting on RW materials).
- **Physics contact callbacks:** 00482820 (registers 3 contact callbacks LAB_004827a0/c0/00482800 via 0x0055xxxx physics-registration band).
- **Physics joint pool (4 slots, DAT_006ce81c joint-handle pool + DAT_006ce82c shape-owner pool):** 00483ed0 (create joint+2 shapes; stiffness 0.1f=0x3dcccccd, limit 300.0f=0x43960000), 00483fa0 (destroy joint+shapes), 00484000 (alloc joint slot), 00484060 (free joint slot), 004840b0 (DAT_006ce82c getter), 004840d0 (DAT_006ce81c getter). 004840f0 (record-array ptr+count setter into DAT_006cec40 + DAT_006ce840 pools; __fastcall, array in EAX).
- **Physics-object accessors (manager DAT_006e71cc):** 00485340 (validate handle, bounds DAT_006e71c4, handle table @+0x10), 00485360 (DynamicObjectList count DAT_006fa0f8), 00485370 (DynamicObjectList base &DAT_006e87b8), 00485380 (get state block @+0xc, EAX-convention anomaly, error via FUN_004a332b(-11)), 00485420 (get world position: validate+state+state.+0x60 pos ptr).

Reclassification for central re-classify:
- 004826d0: the hooks.csv col-9 inline note is materially wrong (claims FUN_00482140 is a gravity-enable setter). Correct it: FUN_00482140 is the collision-mesh strip builder; the bool is its build-mode arg. Plate file 004826d0.md states the correction.

ABI / hook-author cautions:
- 00485380 and 00485420 rely on a register (EAX) hand-off between FUN_00485340->FUN_00485380 (Ghidra shows `in_EAX`); a faithful port must preserve this, or the validate result must be threaded explicitly.
- 004840f0 is __fastcall with the array pointer arriving in EAX (`in_EAX`), slot index in EDX.

Const-resolution still pending (zeroed in standalone image; cited by symbol only): _DAT_005cc320 (numerator in 004820e0), DAT_005d757c (epsilon in 004826d0/00482c10), the easing polynomial _DAT_005cc8f4.._DAT_005cc908 in 00482c10. These are non-blocking for C2 (mechanical descriptions are complete).
