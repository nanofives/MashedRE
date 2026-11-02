// Mashed RE — race-seed write pair (0x00448700).
//
// Disasm 0x00448700 (verbatim):
//   push esi ; mov esi,0x64
//   loop: push 0x897fe0 ; call 0x4464c0 ; add esp,4 ; dec esi ; jne loop  (x100)
//   mov eax,[esp+8]   ; param_1
//   mov ecx,[esp+0xc] ; param_2
//   mov [0x897ffc],eax ; mov [0x898000],ecx ; pop esi ; ret
//
// FUN_004464c0 (C2, CameraEntry::DispatchAll) is __cdecl(void* param_1) — the
// `add esp,4` after each call confirms the caller cleans. It iterates a DIFFERENT
// array (DAT_008964c0, stride 0xd8) and does NOT touch 0x00897ffc/0x00898000, so
// the post-loop writes are the sole producers of those globals.
// Ref: re/analysis/skeleton_prep_render/00448700.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
// The old naked body called 0x004464c0 through s_call_004464c0: the port CameraEntryDispatchAll
// (Util/CameraEntryDispatch.cpp) in the exe, the raw RVA (installed CameraEntryDispatchAll) in the .asi.
// The matched source calls it as FUN_004464c0, bound to CameraEntryDispatchAll in the wrapper.

// MATCH-S08: the body of 0x00448700 (VehicleSeedWritePair) moved to Match/MatchS08_00448700.cpp,
// which compiles the byte-matched source re/match/s08/00448700.c.
