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
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
constexpr std::uintptr_t kDispatchArg_897fe0 = 0x00897fe0u;
constexpr std::uintptr_t kOut1_897ffc        = 0x00897ffcu;
constexpr std::uintptr_t kOut2_898000        = 0x00898000u;

// 0x004464c0  FUN_004464c0  CameraEntry::DispatchAll.
// Indirect slot so the CALL below clobbers no register beyond the ABI's scratch set.
void* const s_call_004464c0 = reinterpret_cast<void*>(0x004464c0u);
} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x00448700  VehicleSeedWritePair
// ─────────────────────────────────────────────────────────────────────────────
// BUGFIX 2026-07-27 — HANG (infinite loop). The C form of this loop was
//     for (int i = 100; i != 0; --i) CallDispatchAll(...);
// and MSVC allocated `i` to EAX *across the call*, emitting:
//     asi+0xc5c0  mov eax,0x64          ; i
//     asi+0xc5c5  push 0x00897fe0
//     asi+0xc5ca  call <thunk -> 0x004464c0>
//     asi+0xc5cf  add esp,4
//     asi+0xc5d2  sub eax,1             ; <-- EAX is the CALLEE'S RETURN VALUE here
//     asi+0xc5d5  jne  asi+0xc5c5
// Our port of 0x004464c0 (CameraEntryDispatchAll) is declared `std::uint32_t` and
// returns the entry count, which is 0 whenever the camera-entry array is empty
// (0x00898994 == 0 live at race entry). So each iteration reloaded EAX=0, `sub eax,1`
// gave 0xffffffff, and the loop never terminated — the main thread wedged inside it
// (IsHungAppWindow=true; 12/12 thread samples in this loop; ESI==0x32 identical across
// three separate processes). The original is immune because it keeps its counter in
// ESI (`MOV ESI,0x64` / `DEC ESI`) precisely because EAX does not survive a CALL.
// Transcribed verbatim so the counter lives in ESI exactly as the original.
// After `PUSH ESI` the incoming args sit at [ESP+8] / [ESP+0xc], matching the original.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl VehicleSeedWritePair(
        std::uint32_t /*param_1*/, std::uint32_t /*param_2*/) {
    __asm {
        push esi
        mov  esi, 64h                       // 100 iterations
    L_VSWP_LOOP:
        push 0897FE0h                       // kDispatchArg_897fe0
        call dword ptr [s_call_004464c0]    // routes through any live hook at the RVA
        add  esp, 4                         // __cdecl: caller cleans
        dec  esi
        jne  L_VSWP_LOOP
        mov  eax, dword ptr [esp+8]         // param_1
        mov  ecx, dword ptr [esp+0Ch]       // param_2
        mov  dword ptr ds:[0897FFCh], eax   // kOut1_897ffc
        mov  dword ptr ds:[0898000h], ecx   // kOut2_898000
        pop  esi
        ret
    }
}
RH_ScopedInstall(VehicleSeedWritePair, 0x00448700);
