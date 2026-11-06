# MATCH-S19 helper h1 notes

FLAG FINDING (h1, 2026-11-02): several functions in this slice match only with `/G5` added (`// FLAGS: /O2 /GS /Ob1 /G5`;
/G3 and /G4 give the same bytes). Signature: `mov eax,[g]; inc eax; mov [g],eax` instead of `inc dword ptr [g]`,
`cmp [mem],0` instead of load+test, call-argument pushes scheduled earlier. Not uniform: 0x004c1b40 (100% under the
game flags) drops to 92.7% under /G5, so try both per function.

0x004c0740  76  100.0%  100.0%  cxx:ok  -  /G5; _rwObjectHasFrameSetFrame shape (90% under game flags)
0x004c07b0  124  100.0%  100.0%  cxx:ok  -  /G5; module open (no Ghidra size, scored by hand-passed size)
0x004c0830  61  100.0%  100.0%  cxx:ok  -  /G5; module close (no Ghidra size)
0x004c0ad0  64  100.0%  100.0%  cxx:ok  -  /G5; raw body
0x004c0b10  14  100.0%  100.0%  cxx:ok  -  /G5; raw body
0x004c0b70  167  100.0%  100.0%  cxx:ok  -  /G5; raw body, callee typed void*
0x004c0c20  330  98.9%  100.0%  cxx:?  b  RwFrameDestroy; sibling-search loop base register
0x004c0e50  123  100.0%  100.0%  cxx:?  -  /G5; RwFrameUpdateObjects, flags byte as uint
0x004c0f00  333  98.8%  100.0%  cxx:?  b  RwFrameAddChild; same sibling-search loop base register
0x004c1300  52  100.0%  100.0%  cxx:?  -  /G5; RwFrameForAllChildren
0x004c1340  144  100.0%  100.0%  cxx:?  -  /G5; flags byte as uint
0x004c13e0  144  100.0%  100.0%  cxx:?  -  /G5; flags byte as uint
0x004c1480  144  100.0%  100.0%  cxx:?  -  /G5; flags byte as uint
0x004c1520  151  100.0%  100.0%  cxx:?  -  /G5; RwFrameRotate, float angle, flags byte as uint
0x004c1680  135  100.0%  100.0%  cxx:?  -  /G5; flags byte as uint, thunk callee by address
0x004c1740  190  100.0%  100.0%  cxx:?  -  /G5; RwBBoxCalculate
0x004c1800  52  100.0%  100.0%  cxx:?  -  /G5; RwBBoxInitialize
0x004c1840  125  100.0%  100.0%  cxx:?  -  /G5; RwBBoxAddPoint
