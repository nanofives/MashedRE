# Stand-in promotion round 6 (2026-10-11): 0x004880a0, 0x004881d0

Port: `mashedmod/src/mashed_re/Sky/PromoLoop_standins_r6.cpp` (.asi only, exe_file empty), shared helper BuildInto;
store addresses cited in the port header.

## 0x004880a0 Obj7030acCreate, 0x004881d0 Obj7030a8Create (were FUN_004880a0 / FUN_004881d0)

**Purpose.** 0x004880a0(tex): create a frame (0x004c0b30), create an object with 0x00534b60(0x2000, 0x10000087, 0)
into the global 0x007030ac, set fields in the object's plugin block at *(obj + *(0x007dc57c)) (+0xa4 = 5, +0xa8 = 2,
+0x40 |= 0x10000000 then |= 0x800000, +4 = 0x200), attach the frame (0x004e7e30), write (0, 0, 0, 10.0f) at
[[obj+0x18]+0x5c]+4..+0x10, and when tex is non-NULL set byte [tex+0x50] = 2 and call 0x004e8090(*[[obj+0x18]+0x20],
tex); finally plugin +0xac = 1. Returns *(0x007dc57c).
0x004881d0(tex): create a frame; when the guard 0x007030b0 is non-zero return it (the frame is not used or released);
otherwise do the same build with count 0x200 into 0x007030a8, set the guard to 1 and return 0x004880a0(tex).

- Callers: 0x004881d0 calls 0x004880a0; 0x004881d0's caller is outside this note (hooks.csv C2 caller per the census).
  Callees: 0x004c0b30, 0x00534b60, 0x004e7e30, 0x004e8090 (C2).
- Fixture: round-3 fixture with a 0x200000 arena, plus the live globals 0x007030ac (and 0x007030a8..0x007030b0 for
  0x004881d0) sentinel-seeded, observed and restored per arm; texture argument = the fixture object.
- path1 GREEN 3/3 (0x004880a0) and 4/4 (0x004881d0), self-baseline GREEN on both, path2 PASS 1/1 on both. The texture
  vectors show the +0x50 byte mark and the refcount at +0x54 rising by one per 0x004e8090 call (0 -> 1 and 3 -> 4 for
  0x004880a0, 0 -> 2 for 0x004881d0). The guarded vectors return the sentinel 0x5eed0002 after two allocations (the frame).
