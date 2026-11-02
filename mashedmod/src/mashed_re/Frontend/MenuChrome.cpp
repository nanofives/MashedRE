// Mashed RE - Frontend menu chrome: dim setter and fullscreen Im2D quad.
// Analysis notes:
//   re/analysis/frontend_promote_menus_a/0x0042aad0.md
//   re/analysis/frontend_promote_menus_a/0x0042aae0.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

// MATCH-S06: the body of 0x0042aad0 (MenuDimSet) moved to Match/MatchS06_0042aad0.cpp,
// which compiles the byte-matched source re/match/s06/0042aad0.c.
// MATCH-S06: the body of 0x0042aae0 (MenuIm2DQuad) moved to Match/MatchS06_0042aae0.cpp,
// which compiles the byte-matched source re/match/s06/0042aae0.c. The old __fastcall export alias
// (/export:MenuIm2DQuad=@MenuIm2DQuad@4) went with it: the matched body is cdecl void(void).
