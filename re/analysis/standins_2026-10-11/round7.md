# Stand-in promotion round 7 (2026-10-11): 0x004218b0, 0x004218d0

Port: `mashedmod/src/mashed_re/Physics/PromoLoop_standins_r7.cpp` (.asi only, exe_file empty); disassembly cited in
the header. Harness addition: block diffs in the `standin_object_call` observation.

Correction of round 1's deferral of 0x004218b0: 0x0057c220 does not write a global registry. `*(0x007dc8d8)` is an
OFFSET added to the target object (`lea ebx, [eax+edi]` at 0x0057c22e), so the store lands in a plugin slot of the
caller's object (measured at target+0xa4 in this process). A fixture object absorbs it.

## 0x004218b0 PhysBodyCreateAttach (was FUN_004218b0)

**Purpose.** With `src` in EAX and `target` on the stack: create a physics body from `src` (0x0057c300
RWP37BodyCreate, which takes a reference on src at +0x50 and copies src+0x4c into the body's sub-record), clear bits
2-3 of [body+8], attach the body to `target` (0x0057c220: plugin slot at target + *(0x007dc8d8), body+0x30 = target,
the body's sub-record +8 = target+4 plus 0x10) and return the body.

- Caller: 0x004218d0 (C3, this round). Callees: 0x0057c300 / 0x0057c220 (C2, RenderWare-Physics band, named).
- path1 GREEN 3/3, self-baseline GREEN 3/3 (refcounts 0, 5, 1 -> +1; target+4 = a block or NULL), path2 PASS 1/1.

## 0x004218d0 PhysWorldBodyBuild (was FUN_004218d0)

**Purpose.** Create a physics world (0x0057c500), apply a2 / a5 / a4 to the world's primitive [world] through
0x0055c540 / 0x0055c4f0 (friction) / 0x0055c4a0 (restitution) and a3 to the world through 0x0055b940 (mass, x87 result
discarded), attach a body built from the world to a1 through 0x004218b0, release the world (0x0057c550) and return
the body.

- Caller: 0x00421a90 (C2). Callees: 0x0057c500, 0x0055c540, 0x0055c4a0, 0x0055b940, 0x0057c550 (C2), 0x0055c4f0 (C3),
  0x004218b0 (C3).
- path1 GREEN 3/3, self-baseline GREEN 3/3, three float-argument sets giving three distinct arena digests;
  path2 PASS 1/1.

Fixture for both: heap stand-in, round-3 RwGlobals + engine +0x52c restore, freeze_main, target object 0x2000 bytes.
