# Stand-in promotion round 4 (2026-10-11): 0x00496d60, 0x00496db0

Port: `mashedmod/src/mashed_re/Render/PromoLoop_standins_r4.cpp` (.asi only, exe_file empty).

## 0x00496d60 PipelineFragment4eb3c0Create, 0x00496db0 PipelineFragment4eb9d0Create (were FUN_00496d60 / FUN_00496db0)

**Purpose.** Create a RenderWare pipeline (0x004d4170; NULL returns 0), lock it (0x004d4dd0), add one fragment whose
node comes from the row's getter (0x004eb3c0 / 0x004eb9d0) with 0x004d4f90(locked, 0, node, 0), unlock (0x004d4380)
and return the pipeline. When the lock or the unlock returns 0 the pipeline is destroyed (0x004d41e0) and 0 is
returned. The two bodies differ only in the getter call and three rel8 branch targets (compared instruction by
instruction 2026-10-11).

- Caller: 0x00496e40 PCVS_Init (C2). Callees: 0x004d4170, 0x004d4dd0, 0x004d4f90, 0x004d4380, 0x004d41e0 (C2),
  0x004eb3c0 / 0x004eb9d0 (C2, renderware band).
- Fixture as round 3 (heap stand-in with `exhaust: 'null'`, live RwGlobals + engine +0x52c restored, freeze_main).
- path1 GREEN 5/5 each, self-baseline GREEN 5/5 each: two normal builds (freelist entry 0x34, rwmalloc 0x2d00, rwrealloc
  0xaa8), failMask 1 (first allocation fails: NULL return, nothing freed), failMask 4 (the rwrealloc fails: the destroy
  arm frees the 0x2d00 block through rwfree and the 0x34 entry through RwFreeListFree), failMask 0x10 (beyond the
  allocation count: normal build). path2 PASS 1/1 each.
