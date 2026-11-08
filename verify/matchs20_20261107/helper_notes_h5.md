0x004cb190  298  100%  100%  cxx:ok  -  local copy of the stream pointer moves the spill to param_1 slot (98.3 -> 100), relocheck OK
0x004c9eb0  147  94.8%  94.8%  cxx:-  b  xor ebx placement; r2: best=0 in for-init / in-if / do-while / rate local (91.4-93.1), permuter on in-if variant 114 compiles, no gain
0x004cbb70  82  93.3%  96.7%  cxx:-  b  camera via eax + lea vs ecx + add; r2: const/int param, cam local, Frustum cast, inline loop/dot helpers, while(n--), dot local, flag sweep: no gain
0x004cba80  76  90.3%  90.3%  cxx:-  b  original pushes esi/edi before the empty-table test (no shrink-wrap); r2: break + if (i < n), break-in-loop, while, count/entries/param locals, signed i, inverted Release test, 8 extra flag sets (/Ox /G6 /O1 /Oy- /Ob2 /Gy /G7): no gain
0x004d40d0  159  89.1%  97.8%  cxx:-  b  result test cmp eax,edi vs test eax,eax (+ vtable/arg regs); r2: compare vs job->state, pointer/void*/char/short/unsigned return, if/else, visitor local, order of state=0, permuter 111 compiles: no gain
0x004cd500  71  83.3%  100%  cxx:-  b  load order of the two globals in the re-read; r2: 64 operand-order/type combos (int/uint/char*/uchar* base, int/uint/long offset, both orders), single return, raw *(void**) reads, volatile field/globals: all 83.3 or worse
0x004d1d70  162  72.1%  98.4%  cxx:-  b  rename-equivalent (regs 98.4): ecx/edx rotated (offset load, cType, vtable temp per case); r2: per-case COM vtable types, tex locals in every case subset (8 combos), ext inside the if, next/raster order, cType local/int cast, label order 4/0, default placement: no gain
0x004ccf20  312  100%  100%  cxx:ok  -  whole per-block body in one static __forceinline helper called in the loop and for DAT_007d45fc (87.7 -> 100), relocheck OK
0x004ce480  333  89.7%  89.7%  cxx:ok(r1)  b  NULL-return epilogue placement; r2: loop branch first in source (`if (!rel && path && *path)`) puts the epilogues right (84.9%) but leaves strcpy dest as lea ecx,[buf+len] vs original add ebp(len),ebx(buf); tried dst locals, &buf[len], len+buf, int/long len, inline MakePath helper (67-78%), permuter 154 compiles on that variant: no gain
0x004c9cd0  443  96.5%  96.5%  cxx:ok  b  r2: InsideScene-style tail (if hr>=0 {if !g {if BeginScene>=0 g=1}} else g=0; return g) + `hr == 0x8876086c && Windowed` as one condition (90.2 -> 96.5); residual push esi shrink-wrapped into the light-loop block; permuter 128 compiles no gain
0x004cb4c0  320  90.2%  90.2%  cxx:-  b  r2: FLAGS /O2 /Ob1 /G5 /Oy- reproduces the original push ebp/mov ebp,esp frame (83.7 -> 90.2); residual 16-step edx down-counter compare loop (tried compare orders, pointer do-while/while, byte offset, 16-i index)

Finding: /Oy- (frame pointer kept) is the flag set for 0x004cb4c0 (original has push ebp / mov ebp, esp with no locals). A static __forceinline helper that holds a body repeated in the source (0x004ccf20) and a local copy of a pointer parameter (0x004cb190, spill slot choice) each closed a function.

0x004cb190  298  100%  100%  cxx:ok  -  local copy of the stream pointer moves the spill to param_1's slot
0x004c9eb0  147  94.8%  94.8%  cxx:-  b  xor ebx placement
0x004cbb70  82  93.3%  96.7%  cxx:-  b  camera via eax + lea vs ecx + add
0x004cba80  76  90.3%  90.3%  cxx:-  b  pushes before the empty-table test
0x004d40d0  159  89.1%  97.8%  cxx:-  b  cmp eax,edi vs test eax,eax on the visitor result
0x004cd500  71  83.3%  100%  cxx:-  b  load order of the two globals in the re-read
0x004d1d70  162  72.1%  98.4%  cxx:-  b  ecx/edx rotation across the switch cases
0x004ccf20  312  100%  100%  cxx:ok  -  static __forceinline per-block helper
0x004ce480  333  89.7%  89.7%  cxx:ok  b  NULL-return epilogue placement vs strcpy dest register
0x004c9cd0  443  96.5%  96.5%  cxx:ok  b  push esi shrink-wrapped into the light-loop block
0x004cb4c0  320  90.2%  90.2%  cxx:-  b  /Oy- frame; edx down-counter compare loop
