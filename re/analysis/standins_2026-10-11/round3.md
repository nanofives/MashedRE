# Stand-in promotion round 3 (2026-10-11): 0x00402240, 0x00402590

Port: `mashedmod/src/mashed_re/Render/PromoLoop_standins_r3.cpp` (.asi only, exe_file empty). First rows of the
RenderWare-object class: every allocation inside the RW calls goes through the stood-in RwGlobals slots, so the whole
object graph (geometry, clump, two frames, atomic, mesh data) is carved from the arena and compared by digest.

## 0x00402240 ClumpGeom401fe0Create, 0x00402590 ClumpGeom4022a0Create (were FUN_00402240 / FUN_00402590)

**Purpose.** Build a geometry with the row's builder (0x00401fe0 / 0x004022a0), create a clump (0x004e6d80) with a
new frame at clump+4 (0x004c0b30), an atomic (0x004e67b0) with a second frame (0x004c0b30), attach that frame
(0x004e7e30) and the geometry (0x004e68a0, third argument 0) to the atomic, insert the atomic into the clump
(0x004e6f80), reparent the second frame under the clump's frame (0x004c1040), release the builder's geometry
reference (0x004e8ea0) and return the clump. The two bodies are byte-identical apart from the first call's target
(decoded call list compared 2026-10-11, 10 calls, 9 equal).

- Caller: 0x004025f0 (C2) for both. Callees: the geometry builders 0x00401fe0 / 0x004022a0 and 0x004e6d80,
  0x004c0b30, 0x004e7e30, 0x004e68a0, 0x004e6f80, 0x004c1040, 0x004e8ea0 at C2; 0x004e67b0 RpAtomicCreate (named RW API).
- Fixture: heap stand-in + `freeze_main`; the live RwGlobals struct `*(0x007d3ff8)..+0x12c` and the engine-instance
  dword `*(0x007d3ff8)+0x52c` observed and restored around each arm (no seeding).
- path1 GREEN 3/3 each, self-baseline GREEN 3/3 each, path2 PASS 1/1 each.

## Harness findings this round

- RwGlobals is heap-allocated at run time: `*(0x007d3ff8)` is not the static block 0x007d3ec8 (that block keeps the
  real slot values, e.g. +0x108 = 0x004ccef0, while the live block holds the stand-in stub). `observe_globals` gained
  `deref` / `off`.
- Without restoring the engine-instance dword +0x52c the self-baseline read RED on ONE arena dword (0x00010001 vs
  0x00020001: the high half advanced by one between arms). The offset was found by bisecting the restored window
  (+0x52c RED, +0x530 GREEN). Which RW module owns it is not determined here.
