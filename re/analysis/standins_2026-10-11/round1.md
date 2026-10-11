# Stand-in promotion round 1 (2026-10-11): 0x00474db0, 0x004759b0, 0x00484170

Port: `mashedmod/src/mashed_re/Render/PromoLoop_standins_r1.cpp` (.asi only, exe_file empty). Harness:
`re/frida/js/standins.js`, arg_type `standin_object_call`, heap stand-in armed (rwmalloc slot `*(0x007d3ff8)+0x108`
carved from one arena reset per arm; arena slack byte 0xa5, block fill 0xcd). Disassembly with every address is in the
port's header comments; each row below adds Purpose, the anti-island halves and the evidence.

## 0x00474db0 RwMallocArrayZeroFill (was FUN_00474db0)

**Purpose.** Allocates `count * stride` bytes (low 32 bits of `imul` at 0x00474dba) through rwmalloc with hint 0
(0x00474dc3) and zero-fills them with ZeroFillWrapper 0x004b6520 (0x00474dcd, which is called even when the block is
NULL), returning the block. U-4391 (resolved 2026-09-11) established that 0x004b6520 is memset-to-zero.

- Callers (C2): 0x004759b0, 0x00475a00, 0x004770c0, 0x004892c0, 0x00489940. Callees: 0x004b6520 ZeroFillWrapper (C3),
  rwmalloc (RwGlobals memoryFuncs slot, identified RW API).
- path1 GREEN 6/6 (self-baseline GREEN 6/6): sizes 4, 0xc0, 0x100, 0, 0x15, and a scheduled rwmalloc failure (n = 0)
  returning NULL. For n = 0x15 the dumped block reads 0 for exactly 21 bytes, then the 0xa5 slack, so a fill that is
  short or long by one byte reads RED. path2 PASS 2/2.

## 0x004759b0 Arrays691600Alloc (was FUN_004759b0)

**Purpose.** Stores `n` at 0x00691600 (0x004759b8), then four RwMallocArrayZeroFill blocks of element sizes 0x40, 4, 4,
4 (U-4417, resolved) at 0x00691608, 0x00691604, 0x00691610, 0x00691614 (0x004759c6 / 0x004759d3 / 0x004759e0 /
0x004759ed). It returns the fourth block (EAX after the last call is not overwritten). 0x0069160c is not written.

- Caller: 0x0040bbb0 (C2). Callee: 0x00474db0 (promoted in this round; C2 before it).
- path1 GREEN 5/5 (self-baseline GREEN 5/5), n = 1, 5, 0x10, 0, 0x40, with `observe_globals` 0x00691600..+0x18
  (seeded with sentinels, observed, restored after each arm) and `freeze_main`. Observed: the four stores land on four
  distinct arena offsets in allocation order, and 0x0069160c keeps its sentinel `0x5eed0003`. path2 PASS 2/2.

## 0x00484170 Rec18Create (was FUN_00484170)

**Purpose.** Allocates a 0x18-byte record through rwmalloc (0x00484179) and returns 0 when that fails (0x00484184 ->
0x004841bf). Otherwise r[0], r[1] = a3[0], a3[1]; r[2] = a2[0]; r[3] = 1.0f / (float at a1+0x80) (`fdivr` with
0x005cc320 = 0x3f800000); r[4] = r[5] = 0. Returns the record.

- Caller: 0x00402750 AppInitialiseOnBootup (C2). Callee: rwmalloc only (identified RW API).
- path1 GREEN 6/6 (self-baseline GREEN 6/6): divisors 2.0, 0.5, 3.0 (0x3eaaaaab), -7.0 (0xbe124925), 0.001
  (0x4479ffff), and a scheduled rwmalloc failure returning NULL. path2 PASS 2/2.

## Naming

The globals 0x00691600..0x00691614 and the record fields have no semantic names; this is filed as U-9654 (naming only,
Blocks: nothing). The function names are mechanical.
