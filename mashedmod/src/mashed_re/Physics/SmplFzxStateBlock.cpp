// Mashed RE — SmplFzx state-block getter by id, with log-on-miss (0x004853b0).
//
// Logging variant of 0x00485380 PhysicsObject_GetStateBlock.
// Disasm 0x004853b0 (verbatim):
//   push esi ; mov esi,[esp+8]          ; param_1 (id)
//   push esi ; call 0x485340 ; add esp,4 ; test eax,eax ; jne valid  ; FUN_00485340(id)
//   push esi ; push 0x5cf310 ; call 0x4987b0 ; add esp,8 ; xor eax,eax ; ret  ; log + 0
//  valid:
//   mov eax,[0x6e71cc] ; mov ecx,[eax+0xc]   ; base = *( *(0x6e71cc) + 0xc )  (DOUBLE deref)
//   shl esi,4 ; mov eax,[esi+ecx] ; ret       ; return *(base + id*0x10)
//
// Callees stay ORIGINAL:
//   0x00485340 FUN_00485340 (C3) PhysicsObject_ValidateHandle — int __cdecl(uint id)
//   0x004987b0 FUN_004987b0 (C2) variadic __cdecl debug-printf -> OutputDebugStringA
//   fmt string lives at .rdata VA 0x005cf310 ("SmplFzx : Not an active SmplFzx object %d\n").
// Ref: re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0/0x004853b0.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
// MATCH-S14: the body of 0x004853b0 (SmplFzxStateBlockGetLogged), its two raw-address call-throughs
// (0x00485340, 0x004987b0) and its shadow A/B entry moved to Match/MatchS14_004853b0.cpp, which compiles the
// byte-matched source re/match/s14/004853b0.c.
