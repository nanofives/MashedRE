// Mashed RE - Render/PerModeRender.cpp
// Session canonical/hot-path-behavioral.
//
// 0x00404320  PerModeRenderMachine  — per-mode render dispatcher (808 bytes)
//
// Reads DAT_007f0fd0 (game-mode/screen selector) and dispatches to 4 mode arms
// (5, 8, 9, 10). Each arm: EndUpdate + narrow view-window(0.8,0.8) + BeginUpdate
// + set RW states 6,0 and 8,0 → per-mode sub-state render → restore states
// 6,1 and 8,1 + EndUpdate + restore view-window + BeginUpdate.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis ref: re/analysis/promote_c2_render_lowrva/00404320.md
// Caller: FUN_00492e90 (FrameDispatch per-frame render dispatch)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee function pointers
// ---------------------------------------------------------------------------

// 0x004c19f0  RwCameraEndUpdate — vtable slot 7 call on camera (C3 — FrameDispatch.cpp)
// Signature: void FUN_004c19f0(int camera_ptr)
using RwCameraEndUpdate_t = void (__cdecl*)(int);
static RwCameraEndUpdate_t const s_RwCameraEndUpdate =
    reinterpret_cast<RwCameraEndUpdate_t>(0x004c19f0u);

// 0x004c1a00  RwCameraBeginUpdate — camera begin-update; returns 0 on failure.
// Signature: int FUN_004c1a00(int camera_ptr)
using RwCameraBeginUpdate_t = int (__cdecl*)(int);
static RwCameraBeginUpdate_t const s_RwCameraBeginUpdate =
    reinterpret_cast<RwCameraBeginUpdate_t>(0x004c1a00u);

// 0x004c1c80  RwCameraSetViewWindow — sets view-window x/y dimensions.
// Signature: void FUN_004c1c80(int camera_ptr, float* pVW)
using RwCameraSetViewWindow_t = void (__cdecl*)(int, float*);
static RwCameraSetViewWindow_t const s_RwCameraSetViewWindow =
    reinterpret_cast<RwCameraSetViewWindow_t>(0x004c1c80u);

// 0x0040e350  GetRenderSubMode — sub-state query; returns current sub-state index.
// Signature: int FUN_0040e350(void)
using GetRenderSubMode_t = int (__cdecl*)();
static GetRenderSubMode_t const s_GetRenderSubMode =
    reinterpret_cast<GetRenderSubMode_t>(0x0040e350u);

// Mode arm renderers (all C2+ per analysis note):
// 0x00403d30  Render_00403d30  (C3 — LowRvaMixed_q3.cpp)
using Render_00403d30_t = void (__cdecl*)();
static Render_00403d30_t const s_Render_00403d30 =
    reinterpret_cast<Render_00403d30_t>(0x00403d30u);

// 0x00403db0  Render_00403db0  (C3 — LowRvaMixed_q3.cpp)
using Render_00403db0_t = void (__cdecl*)();
static Render_00403db0_t const s_Render_00403db0 =
    reinterpret_cast<Render_00403db0_t>(0x00403db0u);

// 0x00403ed0  Render_00403ed0  (C3 — LowRvaMixed_q3.cpp)
using Render_00403ed0_t = void (__cdecl*)();
static Render_00403ed0_t const s_Render_00403ed0 =
    reinterpret_cast<Render_00403ed0_t>(0x00403ed0u);

// 0x00403c60  FUN_00403c60  — mode-10 state-3/4/5 renderer; C1 (DEFERRED D-...)
using FUN_00403c60_t = void (__cdecl*)();
static FUN_00403c60_t const s_FUN_00403c60 =
    reinterpret_cast<FUN_00403c60_t>(0x00403c60u);

// 0x00403fa0  mode-10 loop body: index on the stack AND entry pointer in EAX -> Call_00403fa0 below.

// 0x004041c0  FUN_004041c0 — sub-state-6 renderer for all modes (C2).
using FUN_004041c0_t = void (__cdecl*)();
static FUN_004041c0_t const s_FUN_004041c0 =
    reinterpret_cast<FUN_004041c0_t>(0x004041c0u);


// ---------------------------------------------------------------------------
// Globals referenced by PerModeRenderMachine
// ---------------------------------------------------------------------------

// DAT_007f0fd0 — game-mode/screen selector; compared against 5, 8, 9, 10.
static constexpr std::uintptr_t kGameMode_007f0fd0 = 0x007f0fd0u;

// DAT_007d3ff8 — holds a POINTER to the RW globals/device object. Both fields are
// reached THROUGH that pointer, not by address arithmetic on 0x007d3ff8 itself:
//   0x00404335  mov eax, [0x7d3ff8]   ; load the pointer
//   0x0040433a  mov esi, [eax]        ; camera   = *(ptr + 0x00)
//   0x0040437c  mov ecx, [0x7d3ff8]
//   0x00404385  call [ecx + 0x20]     ; rs_fn    = *(ptr + 0x20)
// [U-9086, fixed 2026-09-09] The previous comment here called it an "array at
// 0x007d3ff8" and the two helpers below dereferenced one level short, so SetRWState
// read its function pointer from 0x007d4018 — an unrelated .data address. The rest of
// the tree already had this right (HudFrontendDispatchers_t4.cpp:33 documents the same
// +0x20 slot as `(**(DAT_007d3ff8 + 0x20))`, i.e. through the pointer); this TU was the
// lone outlier. Found by the TT-2 operand-correspondence sweep.
static constexpr std::uintptr_t kRwGlobalsPtr_007d3ff8 = 0x007d3ff8u;

// Camera slot and render-state vtable slot, as offsets INTO the pointed-to object.
static constexpr std::uint32_t kRwObj_CameraSlot  = 0x00u;   // 0x0040433a  mov esi,[eax]
static constexpr std::uint32_t kRwObj_RenderState = 0x20u;   // 0x00404385  call [ecx+0x20]

// The RW globals object itself (one dereference of 0x007d3ff8).
static std::uintptr_t RwGlobalsObj() {
    return *reinterpret_cast<const std::uintptr_t*>(kRwGlobalsPtr_007d3ff8);
}

// Camera struct offsets (view-window x/y, cited in analysis note):
static constexpr std::uint32_t kCamViewWindowX_off = 0x68u;
static constexpr std::uint32_t kCamViewWindowY_off = 0x6cu;

// Mode-10 entry table:
// DAT_00636b88 .. 0x636c00 — 6 entries × 20 bytes each.
static constexpr std::uintptr_t kMode10Table_00636b88 = 0x00636b88u;
static constexpr std::uintptr_t kMode10TableEnd       = 0x00636c00u;
static constexpr std::uint32_t  kMode10EntryStride    = 20u;
// entry[4] at offset 0x10 (float) compared against DAT_005d757c.
static constexpr std::uint32_t  kMode10EntryFloat_off = 0x10u;

// DAT_005d757c — float zero-sentinel for mode-10 entry[4] gate.
static constexpr std::uintptr_t kMode10Sentinel_005d757c = 0x005d757cu;

// RW render-state IDs used in prologue/epilogue:
static constexpr int kRS6 = 6;
static constexpr int kRS8 = 8;


// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Camera pointer: *(obj + 0x00), where obj = *(0x007d3ff8).
// 0x00404335 mov eax,[0x7d3ff8] ; 0x0040433a mov esi,[eax]
static int GetCameraPtr() {
    return *reinterpret_cast<const int*>(RwGlobalsObj() + kRwObj_CameraSlot);
}

// Render-state call through the object's vtable slot: (*(obj + 0x20))(state_id, value).
// 0x0040437c mov ecx,[0x7d3ff8] ; 0x00404382 push edi ; 0x00404383 push 6 ;
// 0x00404385 call [ecx+0x20]   -> cdecl args (state_id, value).
static void SetRWState(int state_id, int value) {
    using RSFn_t = void (__cdecl*)(int, int);
    const RSFn_t fn = *reinterpret_cast<RSFn_t*>(RwGlobalsObj() + kRwObj_RenderState);
    fn(state_id, value);
}

// 0x00403fa0 takes TWO inputs: the entry index on the stack and the entry POINTER in EAX.
//   0x004045f0 push edi ; 0x004045f1 mov eax, esi ; 0x004045f3 call 0x403fa0 ; 0x004045f8 add esp, 4
// The callee reads and writes the entry's +0x10 float through EAX (0x00403fa3 fld [eax+0x10],
// 0x00403fc1 fstp [eax+0x10]) and reads the index from the stack (0x00403fb9 mov ebp,[esp+0x24]).
// The decomp shows FUN_00403fa0(iVar4) only. [followup/a2-stubat 2026-10-12] The previous port called it
// through a plain cdecl pointer, so EAX held whatever the compiler left there.
static __declspec(naked) void __cdecl Call_00403fa0(const void* /*entry*/, int /*idx*/) {
    __asm {
        mov  eax, dword ptr [esp + 4]    // entry pointer -> EAX (0x004045f1)
        push dword ptr [esp + 8]         // idx (0x004045f0 push edi)
        mov  ecx, 0x00403fa0
        call ecx                         // 0x004045f3
        add  esp, 4                      // 0x004045f8
        ret
    }
}

// Arm prologue shared by modes 9/8/5/10, in byte order (mode 9: 0x00404335..0x00404376):
//   camera = *(*(0x007d3ff8))            0x00404335 / 0x0040433a
//   FUN_004c19f0(camera)                 0x0040433c call 0x4c19f0      (EndUpdate BEFORE the save)
//   saved = camera+0x68, camera+0x6c     0x00404342 / 0x0040434d
//   FUN_004c1c80(camera, {0.8f, 0.8f})   0x00404356..0x00404366 (0x3f4ccccd x2)
//   return FUN_004c1a00(camera) != 0     0x0040436c / 0x00404374 test eax,eax ; je 0x404642 (plain return)
// [followup/a2-stubat 2026-10-12] The previous port read +0x68/+0x6c before the EndUpdate call and returned the
// camera pointer as its success flag, so a zero camera read as a BeginUpdate failure.
static bool ArmPrologue(int camera, float* pSavedVW) {
    s_RwCameraEndUpdate(camera);
    const std::uintptr_t cam_base = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(camera));
    pSavedVW[0] = *reinterpret_cast<const float*>(cam_base + kCamViewWindowX_off);
    pSavedVW[1] = *reinterpret_cast<const float*>(cam_base + kCamViewWindowY_off);
    float vw[2] = { 0.8f, 0.8f };
    s_RwCameraSetViewWindow(camera, vw);
    return s_RwCameraBeginUpdate(camera) != 0;
}

// Arm tail (LAB_004043be 0x004043be..0x004043e9; mode 10 inlines the same at 0x0040460e..0x00404639).
static void ArmTail(int camera, float* pSavedVW) {
    SetRWState(kRS6, 1);
    SetRWState(kRS8, 1);
    s_RwCameraEndUpdate(camera);
    s_RwCameraSetViewWindow(camera, pSavedVW);
    s_RwCameraBeginUpdate(camera);
}

// Sub-state chain, evaluated with short-circuit and one call per test, as the bytes do
// (mode 9: 0x00404397 call; cmp 5; je / 0x004043a1 call; cmp 4; je / 0x004043ab call; cmp 3).
static bool SubStateIs5Or4Or3() {
    return s_GetRenderSubMode() == 5 || s_GetRenderSubMode() == 4 || s_GetRenderSubMode() == 3;
}


// ─── 0x00404320  PerModeRenderMachine ─────────────────────────────────────────
// 808 bytes (0x00404320..0x00404647).
// Per-mode render dispatcher. Reads DAT_007f0fd0 (game-mode selector) and
// branches to 4 mode arms (5, 8, 9, 10).
//
// At main menu (mode 0 / not 5/8/9/10): void — no branch taken, returns immediately.
// This means at main menu this hook is safe even if it runs per-frame.
//
// ref: re/analysis/promote_c2_render_lowrva/00404320.md
// Caller: FUN_00492e90 per-frame render dispatch.
//
// Per-arm sub-state dispatch (bytes):
//   mode 9  : 5/4/3 -> 0x00403d30 (0x004043b9); else 0x00404482: ==6 -> 0x004041c0; then tail
//   mode 8  : 5/4/3 -> 0x00403db0 (0x0040449a); else 0x00404482 (same ==6 check); then tail
//   mode 5  : 5/4/3 -> 0x00403ed0 (0x00404533); else 0x0040452d jne 0x4043be: tail, NO sub-state-6 query
//   mode 10 : 5/4/3 -> 0x00403c60 (0x00404609); else ==6 (0x004045c9) -> loop; then inlined tail
// [followup/a2-stubat 2026-10-12] The previous port ran the ==6 query and 0x004041c0 in mode 5 too.

// 0x00404320
extern "C" __declspec(dllexport) void __cdecl PerModeRenderMachine() {
    const std::uint32_t mode =
        *reinterpret_cast<const std::uint32_t*>(kGameMode_007f0fd0);

    if (mode != 9u && mode != 8u && mode != 5u && mode != 10u) {
        return;                                    // 0x00404540 jne 0x404642
    }

    // 0x00404335 mov eax,[0x7d3ff8] ; 0x0040433a mov esi,[eax]   (mode 10: ebx, 0x00404546 / 0x0040454c)
    const int camera = GetCameraPtr();
    float saved_vw[2];
    if (!ArmPrologue(camera, saved_vw)) {
        return;                                    // je 0x404642 / 0x00404588 je 0x404641
    }
    // 0x0040437c..0x00404391 (pushes of edi = 0) / 0x00404447..0x0040445e
    SetRWState(kRS6, 0);
    SetRWState(kRS8, 0);

    if (SubStateIs5Or4Or3()) {
        if (mode == 9u)       s_Render_00403d30();  // 0x004043b9
        else if (mode == 8u)  s_Render_00403db0();  // 0x0040449a
        else if (mode == 5u)  s_Render_00403ed0();  // 0x00404533
        else                  s_FUN_00403c60();     // 0x00404609
    } else if (mode == 9u || mode == 8u) {
        if (s_GetRenderSubMode() == 6) {            // 0x00404482 call 0x40e350 ; cmp 6
            s_FUN_004041c0();                       // 0x00404490
        }
    } else if (mode == 10u) {
        if (s_GetRenderSubMode() == 6) {            // 0x004045c9 call 0x40e350 ; cmp 6
            // 0x004045d3 mov esi,0x636b88 ; edi = 0 from 0x0040432a, incremented per entry (0x004045fe)
            int entry_idx = 0;
            for (std::uintptr_t entry = kMode10Table_00636b88; entry < kMode10TableEnd;
                 entry += kMode10EntryStride, ++entry_idx) {        // 0x004045fb add esi,0x14 ; cmp/jl
                // 0x004045e0 fld [esi+0x10] ; 0x004045e3 fcomp [0x5d757c] ; test ah,0x41 ; jne skip
                // -> call only when entry[+0x10] > *(float*)0x5d757c (false when unordered)
                if (*reinterpret_cast<const float*>(entry + kMode10EntryFloat_off) >
                    *reinterpret_cast<const float*>(kMode10Sentinel_005d757c)) {
                    Call_00403fa0(reinterpret_cast<const void*>(entry), entry_idx);
                }
            }
        }
    }
    // mode 5 with a sub-state other than 5/4/3 falls straight here (0x0040452d jne 0x4043be).

    ArmTail(camera, saved_vw);                      // 0x004043be.. / 0x0040460e..
}

RH_ScopedInstall(PerModeRenderMachine, 0x00404320);  // re-enabled 2026-05-24 phase-a2-strict GREEN int_scalar (no-op modes)
