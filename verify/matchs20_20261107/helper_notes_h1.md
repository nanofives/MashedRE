# MATCH-S20 helper h1 notes (2026-11-07)

Findings for this range (RW core, D3D9 driver):
- RW 3.6 source shapes moved several functions far: 0x004cb190 is _rwD3D9SetStreams (37.8% -> 98.3%), 0x004cbe80
  RwStreamWrite, 0x004cf7d0 RwTexDictionaryStreamRead, 0x004cff20 _rwD3D9RasterLock.
- Duplicated D3D format probes (0x004d2e30, 0x004d1e30, 0x004d1640) are a file-static helper that only
  `static __forceinline` expands under /Ob1; plain `__inline` stays a call (33.5% vs 100% on 0x004d2e30).
- Switch case bodies are laid out in source order: 0x004d1e30 and 0x004d1640 order their cases 0/4, 5, then the rest.
- The original keeps some address-taken locals in a dead parameter's stack slot (0x004c9fb0, 0x004cf7d0,
  0x004cff20); our cl 13.00 does not. Reading/writing through &param after copying the parameter to a local
  reproduces it (0x004cf7d0 59.5% -> 84.2%).
- 0x004cb490 has an ebp frame and esi/edi saved at entry, the shape of an inline __asm body (class c).

0x004c7860  54  100.0  100.0  cxx:ok  -  ternary return gives neg/sbb/and
0x004c79f0  36  100.0  100.0  cxx:ok  -  device vtable +0x9c as __stdcall fn ptr
0x004c7a20  59  100.0  100.0  cxx:ok  -  read device global twice (a local gave 55%)
0x004c7a60  5  100.0  100.0  cxx:ok  -  jmp thunk to FUN_004cfa00(void)
0x004c7a70  2811  -  -  cxx:n/a  b  not attempted: above 1000 B and last in the size order, budget spent (raw converter did not compile); no file written
0x004c8740  180  100.0  100.0  cxx:ok  -  out-pointer reuse of loop counter q; permuter move of the windowed computation
0x004c9eb0  147  94.8  94.8  cxx:ok  b  ebx push/xor placement in the prologue (shrink-wrap)
0x004c9f60  67  100.0  100.0  cxx:ok  -  typed globals, __stdcall vtable slot
0x004caea0  300  77.5  86.2  cxx:ok  b  zero-store register (eax vs ebx) and RwGlobals fn-ptr load after the pushes
0x004cafd0  148  100.0  100.0  cxx:ok  -  branch orders swapped, ext->obj read per branch
0x004cb070  56  100.0  100.0  cxx:ok  -  cached device state, hr >= 0 keeps param
0x004cb0b0  51  100.0  100.0  cxx:ok  -  same shape, vtable +0x170
0x004cb0f0  51  100.0  100.0  cxx:ok  -  same shape, vtable +0x1ac
0x004cb130  90  100.0  100.0  cxx:ok  -  16-byte record cache, vtable +0x190
0x004cb190  298  98.3  98.3  cxx:ok  b  RW _rwD3D9SetStreams shape; spill goes to param_2 slot instead of param_1
0x004cb2c0  36  100.0  100.0  cxx:ok  -  cached device state, vtable +0x1a0
0x004cb2f0  50  100.0  100.0  cxx:ok  -  FUN_004d53b0() then device vtable +0x148, 6 args
0x004cb490  42  83.3  88.9  cxx:ok  c  ebp frame + esi/edi saved at entry (inline-asm signature); FLAGS /Oy- best
0x004c9fb0  424  81.3  85.6  cxx:ok  b  surface kept in param_2's slot (reproduced); prologue scheduling of hwnd/ret/device loads
0x004cbe80  445  78.6  93.1  cxx:ok  b  RwStreamWrite; register rotation and RWERROR block tail-merge/scheduling
0x004cf7d0  514  84.2  86.7  cxx:ok  b  RwTexDictionaryStreamRead; header in the param slot (reproduced); device-pointer placement / reg rotation
0x004d2e30  608  100.0  100.0  cxx:ok  -  depth-format probe as a static __forceinline helper (plain __inline 33.5%)
0x004cff20  842  62.5  68.2  cxx:ok  b  _rwD3D9RasterLock; frame 0x44 with param-slot packing vs ours 0x50 + shrink-wrap (offsets shift)
0x004d1e30  1270  87.5  87.5  cxx:ok  b  video-memory raster restore; case order + forceinline probe; cFormat<<8 byte narrowing in ours
0x004d1640  1735  92.4  92.4  cxx:ok  b  raster format check; forceinline probes + if-chain; one or ah,2 vs or esi,0x200 shifts jump offsets
