import sys, pathlib
rvas_path = pathlib.Path('/c/Users/maria/Desktop/Proyectos/Mashed/.claude/worktrees/agent-add3c4d81afe45f55').parent.parent.parent / 'tmp' / 'rvas.txt'
# Cleaner: read RVAs from tmp file via absolute path
rvas = open('/tmp/rvas.txt', 'r', encoding='utf-8').read().strip()

# Read SCRIBE_QUEUE.md from the worktree (worktree main path same)
queue_path = '/c/Users/maria/Desktop/Proyectos/Mashed/re/SCRIBE_QUEUE.md'
with open(queue_path, 'r', encoding='utf-8') as f:
    content = f.read()

note = (
    "80 new C1 plates spanning 0x0041e140..0x00422440 (~17KB code, single contiguous bucket). "
    "Subsystem is the per-player VEHICLE-INSTANCE state-management + smoke/debris particle pools — "
    "aligned with the batch-v-s1 HUD/weapons hypothesis from the orchestrator. Cluster shape: "
    "(A) lap-timer module 0x0041e140-0x0041e4b0 (6 fns): 3-tuple<->hundredths converters FUN_0041e150/0041e170, "
    "best-lap state machine FUN_0041e1e0 with state codes {0,1,2,3,4} at +0x1b4, slot-loader FSM at +0x1c8 "
    "calling FUN_004115c0/FUN_00411580, palette-swap broadcast FUN_0041e4b0 writing global color table DAT_0063d5f8 "
    "into 12 child material slots. (B) vtable trampoline family 0x0041e8d0-0x0041e950 (7 fns) and matching "
    "getter family 0x0041e9f0-0x0041ea70 (8 fns) — both walk a global function-pointer table at DAT_0063d7e4 "
    "at offsets +0x1c..+0x3c, dispatching/reading 8 slots. (C) per-vehicle-slot state array DAT_0063d9e0 with "
    "stride 0x2ac = 4 instances; per-slot has 128 child-atomic pointer array at +0..+0x200, lap-timer triple "
    "at +0x188-0x190, ground-distance ring at +0x200-0x21c (8 floats), state-code at +0x1b4, sound-handle slots "
    "at +0x108-0x178 (12), flags dword at +0x294, derived state code at +0x28c, player-id at +0x290. "
    "0x0041ec00 = scene unload (RpAtomicDestroy chain at slot indices 0x3b-0x3e/0x40 + clump unload + memset 0x2ac); "
    "0x0041ecc0 = material palette swap for child indices {0..3, 0x21..0x24} vs others; 0x0041eeb0 = priority-arbiter "
    "flag setter (bits 0x40/0x80/0x800 + dominance from 0x10/0x20/0x400); 0x0041efe0 reads bit 0x08; "
    "0x0041f060/0041f220 extract 64-byte matrices via FUN_004c0ed0 (likely RwFrameGetLTM). "
    "0x0041f330 walks vehicle-name table at s_Shuriken_005f37a8 (stride 0x84, count DAT_005f5fe0) — "
    "**confirms this bucket is per-vehicle state**. (D) per-frame update chain anchored at FUN_00420870 (970b, "
    "largest fn) — calls FUN_0041f3d0 (4-wheel UV anim, +0x220 4-float rotation accumulator), FUN_0041f4f0 "
    "(special-atomic UV at +0x4c), FUN_0041f590 (8-float ring-buffer ground-distance averager, calls FUN_00426dc0 "
    "terrain ray-cast), FUN_0041f710 (audio submit FUN_00484cf0 with player-id|0xff00), FUN_0041f770 (engine-pitch "
    "envelope via FUN_00544bf0 + speedometer UV anim using DAT_0063e4a4 frame-counter), FUN_0041f880 (state-code "
    "decoder writing {0x3b..0x3e, 0x40} to +0x28c + audio FUN_00422aa0), FUN_00420340 (RW material-property setter "
    "with FUN_00540160/00540100/00540080 trio gated on flag bits 0x1/0x2/0x4); FUN_00420870 itself dispatches a "
    "massive child-visibility switch over 128 slots keyed on player-state {0,1,2} × flag-bits {0x10,0x20,0x40,0x400,0x800}. "
    "(E) smoke-particle subsystem at DAT_0063e548 (emitter) + 128-entry ring at DAT_0063e5a4 stride 0xb dwords + "
    "128-bit active bitmap at DAT_0063e4a8 + per-vehicle-slot velocity-pair pool at DAT_0063e4b8 stride 9 dwords "
    "(4 vehicles): init FUN_004210b0 with literal \"smoke\", FUN_00421080 clears ring (-1.0f sentinel), "
    "FUN_00421100 age-to-RGBA mapping (3 buckets: 0xdcdcdc78/0x8080a0c8/0x282828f0), FUN_00421160 spawn (6 "
    "random initializers covering color/velocity/size/rotation/angle), FUN_004212b0 per-frame update+emit "
    "(685b — calls C3 hook FUN_004c3ac0 Vec3Magnitude AND child-atomic-position-fetch FUN_0041f290 with index "
    "0x33 = 'exhaust position'), FUN_004210f0 destroy, FUN_00421560 render with DAT_007d3ff8+0x20 vcall "
    "render-state save/restore. (F) RW frame/atomic helpers 0x00421750/0x00421800 (set vec3/matrix on RpAtomic "
    "with frame-dirty bookkeeping via FUN_0055ad30+FUN_0055ac00 — canonical RW v3 LTM pattern); "
    "0x004218b0/0x004218d0 = RpAtomic create+attach (FUN_0057c300 + FUN_0057c220 + 4-property setter chain "
    "FUN_0055c540/4f0/4a0/b940 = RwMatrix.right/up/at/pos likely); 0x00421930 = atomic->geometry getter; "
    "0x00421980 = world-list-remove. (G) DEBRIS POOL at DAT_0063fb90 (4 slots stride 0x208, two lanes of 0x100 "
    "each, each lane has 10 atomics stride 6 dwords): init FUN_004220d0 loads 'debris0.dff'..'debris5.dff' "
    "from PTR_s_debris0_dff_005f6124 (modulo 6) via FUN_0042a5d0; per-slot init FUN_00421ba0 -> FUN_00421a90 "
    "sorts 20 enum-atomics by type-ID into lane 0 (0x50-0x59) vs lane 1 (0x5a-0x63), creates child atomics "
    "via sibling FUN_004218d0; per-frame visibility FUN_00421720->FUN_004216b0 (frustum test via "
    "FUN_004c1b40 + atomic-render vcall +0x48); per-frame event FUN_00422290->FUN_00422160 (lane-mode dispatch "
    "with event FUN_00465c10(0x21,player)); soft tear-down FUN_00421c50->FUN_00421c10; hard tear-down "
    "FUN_00422140->FUN_00421c80 (full RpAtomic + RpGeometry + RpClump destroy chain). (H) utilities: "
    "FUN_004223b0 = AABB initializer (-FLT_MAX/+FLT_MAX) feeding RpClumpForAllAtomics-style FUN_004e66d0 with "
    "callback LAB_004222f0 (Ghidra failed to promote this label to a Function — note for sweep); FUN_004223f0 "
    "= quadratic-polynomial xorshift PRNG (constants 0x3d73/0xc0ae5/0xd208dd0d masked 0x7fffffff scaled by "
    "_DAT_005cd314); FUN_00422440 = cosine-eased lerp (smoothstep, using PI/0.5/1.0 from _DAT_005cd310/32c/320). "
    "Recurring callees mapping out the dependency surface: FUN_004c0ed0 (RwFrameGetLTM-shape, 5x), FUN_004c1340/"
    "FUN_004c1520 (RW material/texture-matrix setter family, 6x), FUN_0053d090/0053d060/0053fba0/0053fea0/"
    "005401d0/0053fbb0/00540160/00540100/00540080 (RW material/atomic api, 12x), FUN_004e6e00/004e6920/"
    "004e6fb0/004e6ab0/004e7e30/004e5fc0/004e6100 (RW clump/geometry life-cycle, 11x), FUN_0057c210/0057c300/"
    "0057c220/0057c370/0057c500/0057c550 (RW handle alloc/attach/destroy, 7x — overlaps batch-v-s5 RW-cluster "
    "0x0057bf30..0x0057c550 plates), FUN_0055ad30/0055ac00/0055b650/0055c540/0055c4f0/0055c4a0/0055b940/"
    "0055dec0/00559ee0/0055dff0 (RW frame/world dirty-bookkeeping + 4-component matrix builder, 10x), "
    "FUN_004c3ac0 (Vec3Magnitude — confirmed C3 hook, called by FUN_004212b0), FUN_004c3d90 (RwV3dTransformPoints-"
    "shape), FUN_004c4dc0 (matrix-invert), FUN_004b3fc0 (RpClumpForAllAtomics-shape, 2x), FUN_004b5190 (atomic-"
    "type-id read, 2x), FUN_004b5240 (RpAtomicSetFlags-likely visibility), FUN_004b6480/004b6520 (CRT memset), "
    "FUN_004a2c48 (CRT _ftol2 — appears in random-byte chain), FUN_00472650/00472690 (random float/int range), "
    "FUN_00484cf0/00544bf0 (audio submit), FUN_00465c10 (event submit), FUN_0042f6a0 (NOT called here but the "
    "gates DAT_00636acc/00636ad0/00636ad4 are likely set by the same game-mode getter). Recurring globals beyond "
    "the per-slot 0x2ac record: DAT_007d3ff8 (RW engine ctx, +0x20 = render-state-set callback used by smoke "
    "render), DAT_0063d840 (sound-handle per slot), DAT_0063d910..DAT_0063d918 (last-camera position per slot "
    "stride 0x40 = 16 dwords), DAT_0063d8e0 (cached view matrix per slot stride 0x40), DAT_007f101c (frame angle "
    "accumulator with low-2-bits = current player), _DAT_005cc320 (1.0f), _DAT_005cc32c (0.5f), "
    "_DAT_005d757c (small threshold), _DAT_005cd058 (dt multiplier), _DAT_005cd270/cc9c0/cc99c (radian helpers). "
    "Cross-bucket hooks: confirms FUN_004c3ac0 Vec3Magnitude C3 hook is on the smoke/particle hot path (called "
    "once per active vehicle per frame in FUN_004212b0); confirms FUN_0057c210 / FUN_0057c300 / FUN_0057c500 "
    "(the unmapped 0x54-byte RW class constructor in batch-v-s5) is the foundation class for both vehicle "
    "atomic-build and debris atomic-build here. Library-residue: NONE — bucket 0x0041e140..0x00422440 sits well "
    "before the CRT band (0x004a0000) and outside all 6 known library ranges from the batch_w header. "
    "Uncertainties: ~28 [UNCERTAIN] markers filed inline across the 80 plates (mostly RW-catalog symbol identity "
    "for the FUN_004c1*/FUN_0053f*/FUN_00540*/FUN_004e6* clusters and the specific child-index-to-body-part "
    "mapping in FUN_00420870's large case-switch); NONE are above C2 so 5-uncertainty STOP-AND-ASK NOT triggered. "
    "Pool-acquisition: pre-assigned Mashed_pool1 per brief but ghidra_pool.sh acquire returned Mashed_pool3, "
    "which failed to open (LockException — another session held a transient Windows JVM channel-lock); "
    "fell back to Mashed_pool8 (only unlocked clone remaining); MCP open via project_program_open_existing"
    "(read_only=true) succeeded with session_id de4473b37bd34efc968c6f6fe9f884bd, health_ping ok; SHA-256 anchor "
    "verified BDCAE0...EFD3C0E; 80 plates total in re/analysis/bucket_0041e140/. Recommended sweep action: bucket "
    "dir name is neutral (bucket_0041e140) per skill rules; sweep may rename to vehicle_state_smoke_debris or "
    "similar after confirming subsystem identity from caller-side analysis (the four callers of FUN_0041ec00 "
    "— FUN_0040cf80 + FUN_0041ffb0 — and FUN_00420870 — FUN_0040da50 — and FUN_004220d0 — FUN_0040d110 — would "
    "pin the bucket as per-vehicle update path). Strong follow-up candidate: promote FUN_004c0ed0 (RwFrameGetLTM-"
    "shape) and FUN_004c1480/004c1520/004c1340 (RW material-matrix family) to C2 by reading 1-2 callers each "
    "— these are the single largest STUB cluster in this bucket and unblock semantic naming of FUN_0041f060, "
    "FUN_0041f220, FUN_0041f290, FUN_0041f3d0, FUN_0041f4f0, FUN_00420870. Also: LAB_004222f0 (the callback "
    "inside FUN_004223b0) is unpromoted by Ghidra — sweep should call function_create there to upgrade it to "
    "a proper function with refs. No commits."
)

row = "2026-05-18  batch-w-s1  rvas=" + rvas + "  bucket=re/analysis/bucket_0041e140  level=c1  pool=Mashed_pool8  subsystem_observed=vehicle-state-array-with-smoke-and-debris-particle-pools  note=" + note + "\n"

lines = content.split('\n')
out = []
inserted = False
for ln in lines:
    if not inserted and ln.startswith('## Drained'):
        out.append(row.rstrip('\n'))
        out.append('')
        inserted = True
    out.append(ln)
with open(queue_path, 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
print('inserted=', inserted)
print('row len=', len(row))
