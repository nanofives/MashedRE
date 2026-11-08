0x004cefd0  395  100.0  100.0  cxx:ok  -  RwImageGammaCorrect, macro pixel loop with src/dst pointers
0x004cf160  325  45.6  46.8  cxx:ok  c  inline-asm fnstcw/fistp chop conversion (no C form in 9176, __asm banned)
0x004cf460  54  100.0  100.0  cxx:ok  -  RwTextureStreamGetSize, size accumulated statement by statement
0x004cf4a0  244  74.3  74.3  cxx:ok  b  RwTextureStreamWrite; per-site return-0 epilogues tail-merged by ours (S19 0x004c18c0 pattern)
0x004cfe40  149  100.0  100.0  cxx:ok  -  free-list teardown; no store of 0 after the drain loop
0x004cfee0  28  100.0  100.0  cxx:ok  -  raster plugin ext byte +8 via texture->raster
0x004d0660  251  64.9  64.9  cxx:ok  b  raster render-combination switch; basic-block order of the inner case 2 / final error vs case 0/4/5
0x004d1d20  67  100.0  100.0  cxx:ok  -  raster cType/cFlags/cFormat/depth setup; callee gets raster->cType
0x004d1d70  162  72.1  98.4  cxx:ok  b  raster list D3D release; ours tail-merges the identical Release+NULL case tails
0x004d38b0  51  100.0  100.0  cxx:ok  -  CheckDeviceFormat wrapper, adapter format local
0x004d38f0  93  100.0  100.0  cxx:ok  -  CheckDeviceFormat + CheckDepthStencilMatch, adapter format local
0x004d3950  87  100.0  100.0  cxx:ok  -  same, depth-stencil usage
0x004d39b0  93  100.0  100.0  cxx:ok  -  same, rtype 5
0x004d3a10  65  100.0  100.0  cxx:ok  -  caps-gated CheckDeviceFormat == D3D_OK (usage 0x400)
0x004d3a60  65  100.0  100.0  cxx:ok  -  same (usage 0x401)
0x004d3ab0  65  100.0  100.0  cxx:ok  -  same (rtype 5)
0x004d40d0  159  89.1  97.8  cxx:ok  b  job run; original tests the visitor result with cmp eax,edi (zero reg), ours test eax,eax
0x004d41e0  111  100.0  100.0  cxx:ok  -  container destroy, entry count local
0x004c9cd0  443  90.2  90.2  cxx:ok  b  D3D9 lost-device restore; push esi placement and return-0 block layout
0x004c9ad0  499  100.0  100.0  cxx:ok  -  D3D9 device-state reset; fixed compile, memset intrinsic + struct copy
0x004d3b00  585  30.6  55.6  cxx:ok  b  format re-selection; check helper needs __forceinline, original inlines FUN_004d39b0 in one of three expansions
0x004d0d00  695  35.5  67.8  cxx:ok  b  raster-to-camera blit; late ebx/ebp saves and param-slot reuse for the dst surface not reproduced
0x004c8800  1098  100.0  100.0  cxx:ok  -  D3DPRESENT_PARAMETERS setup; fixed compile, three-way if/else for the back-buffer format
0x004d2340  1548  37.3  43.5  cxx:ok  b  raster create; fixed compile, structure in place, dead first stores / per-error stack slots / case layout not reproduced
