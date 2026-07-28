// Mashed RE - Frontend game-mode dispatch reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042ee40.md
//   re/analysis/c0_promotion_frontend_a/0x0042ee40.md (prior plate)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: ma3-frida-s7 (game-mode / settings UI dispatch).
// Other ma3-frida-s7 candidates REFUSED via callee-gate:
//   0x0042f0c0 (4/8 callees still C1)
//   0x0042fb70 (3/10 callees C1)
//   0x0042fe90 (3/10 callees C1)
//   0x00439210 (8/21 callees C1 + 2 C0; large dispatcher)
// Only 0x0042ee40 passed the callee gate (sole callee 0x0040bb90 is C4).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: FUN_0040bb90 - sprite lookup variant B (SpriteLookupTableB, C4)
// Analysis: re/analysis/frontend_promote_menus_b/0040bb90.md
// Signature confirmed via hooks_registry.py entry 'sprite_lookup_table_b':
//   pointer __cdecl FUN_0040bb90(int32 key)
// The dispatcher forwards its own param_1 through (Ghidra renders the call as
// FUN_0040bb90() with no args because the call is a tail-call/stack-reuse;
// the actual asm forwards [esp+4]).
// ---------------------------------------------------------------------------
// CORRECTED 2026-07-27: this read `void* (__cdecl*)(int)`. FUN_0040bb90 takes a
// `const char*` NAME (0x0040bb90 MOV EAX,[ESP+4] -> FUN_004c5c00(list, key), and
// FUN_004c5c00 walks `key` as a string). SpriteGate.cpp already had it right; the two
// files disagreed and this one was wrong. Kept only for reference — the dispatcher
// below now tail-jumps to 0x0040bb90 verbatim instead of calling through this.
typedef void* (__cdecl *SpriteLookupB_t)(const char* key);
static constexpr std::uintptr_t kFun0040bb90 = 0x0040bb90u;

// ---------------------------------------------------------------------------
// FrontendModeDispatch  --  0x0042ee40
//
// Original: FUN_0042ee40 (204 bytes, 0x0042ee40..0x0042ef0c)
// Signature: undefined4 FUN_0042ee40(int param_1)
//   Returns: result of FUN_0040bb90 forwarding, or 0 on default/miss.
//
// Decomp (cited from re/analysis/promote_c2_frontend_menus/0x0042ee40.md):
//   switch (DAT_0067e9fc) {
//   case 2:
//     return FUN_0040bb90();
//   case 3: case 4: case 5:
//     if (DAT_0067f17c > 9) {
//       if (param_1 == 0 || param_1 == 1 || param_1 == 2)
//         return FUN_0040bb90();
//     }
//     if (param_1 > 999) param_1 -= 1000;
//     if (param_1 == 0 || param_1 == 1 || param_1 == 2)
//       return FUN_0040bb90();
//     break;
//   case 6: case 7: case 8: case 9:
//     if (param_1 == 0) return FUN_0040bb90();
//     break;
//   case 10:
//     return FUN_0040bb90();
//   }
//   return 0;
//
// Globals (cited at analysis note):
//   0x0067e9fc  outer mode/screen selector (switched 2..10)
//   0x0067f17c  animation frame counter (threshold check > 9)
//
// Constants (cited at decomp):
//   999  param_1 offset cutoff for -1000 adjustment
//   1000 subtracted from param_1 when > 999
//   9    DAT_0067f17c threshold (>9 enables early dispatch for cases 3-5)
//
// Note on FUN_0040bb90 forwarding semantics:
//   Ghidra's decomp omits the param. The C1 plate of FUN_0040bb90 confirms it
//   takes one undefined4 arg used as a sprite-table key. The dispatcher
//   forwards its own (possibly -1000-adjusted) param_1 through, which is the
//   only ABI-consistent interpretation given that FUN_0040bb90 dereferences
//   the arg via the sprite table at DAT_0063b904.
// ---------------------------------------------------------------------------

// 0x0067e9fc: outer mode switch (cited at decomp).
static constexpr std::uintptr_t kModeSelector = 0x0067e9fcu;
// 0x0067f17c: animation frame counter (cited at decomp).
static constexpr std::uintptr_t kFrameCounter = 0x0067f17cu;

// BUGFIX 2026-07-27 — CRASHER. The previous port forwarded its integer param_1 to
// FUN_0040bb90, justified in the comment above as "the only ABI-consistent interpretation".
// The anchored bytes disprove it: like its sibling 0x0042ee00, this is an ARGUMENT-REWRITING
// TAIL-JMP thunk that maps (mode, param_1) onto a sprite-NAME STRING, overwrites its own
// stack slot with that string pointer, and jumps. FUN_0040bb90 is `void* (const char* key)`
// (0x0040bb90: MOV EAX,[ESP+4] / MOV ECX,[0x0063b904] / PUSH EAX / PUSH ECX / CALL 0x004c5c00),
// and FUN_004c5c00 dereferences that key unconditionally — so forwarding the int 0 handed it
// a NULL and AV'd at 0x004c5c00+0x2d (`MOV DL,[EDI]`, EDI=0).
// Found by a menu-navigated race: original 0x0043a102 PUSH 0 / 0x0043a104 CALL 0x0042ee40.
// scenario_launch.py cannot reach this path (it pokes DAT_00771968=2 past the menu loader).
//
// Verbatim bytes (0x0042ee40..0x0042ef0c). Jump table at 0x0042ef10 (modes 2..10):
//   mode 2      -> 0x0042ee58   mode 3,4,5 -> 0x0042ee65
//   mode 6..9   -> 0x0042eee8   mode 10    -> 0x0042eefd
//   0x0042ee40 a1fce96700    MOV EAX,[0x0067e9fc]
//   0x0042ee45 83c0fe        ADD EAX,-2
//   0x0042ee48 83f808        CMP EAX,8
//   0x0042ee4b 0f87b9000000  JA  0x0042ef0a                     ; unsigned -> return 0
//   0x0042ee51 ff248510ef4200 JMP [EAX*4 + 0x0042ef10]
//   0x0042ee58 MOV [ESP+4],0x005cd81c ; "TimeTrial"   -> JMP 0x0040bb90
//   0x0042ee65 CMP [0x0067f17c],9 / MOV EAX,[ESP+4] / JLE 0x0042eea7
//              EAX==0 -> "BronzeMedal" 0x005cd810 ; ==1 -> "SilverMedal" 0x005cd804
//              ==2 -> "GoldMedal" 0x005cd7f8 ; else fall to 0x0042eea7
//   0x0042eea7 CMP EAX,0x3e8 / JL -> skip / SUB EAX,0x3e8       ; signed compare
//              EAX==0 -> "BronzeCup" 0x005cd7ec ; ==1 -> "SilverCup" 0x005cd7e0
//              ==2 -> "GoldCup" 0x005cd7d8 ; else 0x0042ef0a
//   0x0042eee8 MOV EAX,[ESP+4] / TEST EAX,EAX / JNE 0x0042ef0a
//              -> "MultiPlayer" 0x005cd7cc
//   0x0042eefd MOV [ESP+4],0x005cd7c0 ; "QuickRace"  -> JMP 0x0040bb90
//   0x0042ef0a 33c0 XOR EAX,EAX / C3 RET
//
// Transcribed verbatim as naked asm — the arg-rewrite + tail-jmp reuses the CALLER's frame,
// which no __cdecl C function can express. The mode dispatch is written as an explicit
// compare chain rather than a jump table (same control flow, no new data emitted); the JMP
// to 0x0040bb90 goes through a memory slot so no register is clobbered.
static void* const s_jmp_0040bb90 = reinterpret_cast<void*>(0x0040bb90u);

static constexpr std::uintptr_t kStr_TimeTrial   = 0x005cd81cu;
static constexpr std::uintptr_t kStr_BronzeMedal = 0x005cd810u;
static constexpr std::uintptr_t kStr_SilverMedal = 0x005cd804u;
static constexpr std::uintptr_t kStr_GoldMedal   = 0x005cd7f8u;
static constexpr std::uintptr_t kStr_BronzeCup   = 0x005cd7ecu;
static constexpr std::uintptr_t kStr_SilverCup   = 0x005cd7e0u;
static constexpr std::uintptr_t kStr_GoldCup     = 0x005cd7d8u;
static constexpr std::uintptr_t kStr_MultiPlayer = 0x005cd7ccu;
static constexpr std::uintptr_t kStr_QuickRace   = 0x005cd7c0u;

// 0x0042ee40
extern "C" __declspec(dllexport) __declspec(naked) void* __cdecl FrontendModeDispatch(int /*param_1*/) {
    __asm {
        mov  eax, dword ptr ds:[067E9FCh]       // 0x0067e9fc
        add  eax, -2
        cmp  eax, 8
        ja   L_FD_NONE                          // unsigned: modes outside 2..10
        // mode 2 -> "TimeTrial"
        test eax, eax
        jne  L_FD_NOT2
        mov  dword ptr [esp+4], 05CD81Ch
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_NOT2:
        cmp  eax, 3                             // (mode-2)==3 -> mode 5
        jg   L_FD_HI                            // modes 6..10
        // modes 3,4,5  (0x0042ee65)
        cmp  dword ptr ds:[067F17Ch], 9         // 0x0067f17c
        mov  eax, dword ptr [esp+4]
        jle  L_FD_CUP
        test eax, eax
        jne  L_FD_M1
        mov  dword ptr [esp+4], 05CD810h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_M1:
        cmp  eax, 1
        jne  L_FD_M2
        mov  dword ptr [esp+4], 05CD804h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_M2:
        cmp  eax, 2
        jne  L_FD_CUP
        mov  dword ptr [esp+4], 05CD7F8h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_CUP:                                   // 0x0042eea7
        cmp  eax, 0x3e8
        jl   L_FD_C0
        sub  eax, 0x3e8
    L_FD_C0:
        test eax, eax
        jne  L_FD_C1
        mov  dword ptr [esp+4], 05CD7ECh
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_C1:
        cmp  eax, 1
        jne  L_FD_C2
        mov  dword ptr [esp+4], 05CD7E0h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_C2:
        cmp  eax, 2
        jne  L_FD_NONE
        mov  dword ptr [esp+4], 05CD7D8h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_HI:
        cmp  eax, 8                             // (mode-2)==8 -> mode 10
        je   L_FD_QUICK
        // modes 6..9  (0x0042eee8)
        mov  eax, dword ptr [esp+4]
        test eax, eax
        jne  L_FD_NONE
        mov  dword ptr [esp+4], 05CD7CCh
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_QUICK:                                 // 0x0042eefd
        mov  dword ptr [esp+4], 05CD7C0h
        jmp  dword ptr [s_jmp_0040bb90]
    L_FD_NONE:                                  // 0x0042ef0a
        xor  eax, eax
        ret
    }
}

RH_ScopedInstall(FrontendModeDispatch, 0x0042ee40);  // re-enabled 2026-05-24 c3-frontend-b
