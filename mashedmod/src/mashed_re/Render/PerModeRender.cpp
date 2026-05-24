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

// 0x00403fa0  mode-10 loop body; param = entry index.
// Signature: void FUN_00403fa0(int idx)
using FUN_00403fa0_t = void (__cdecl*)(int);
static FUN_00403fa0_t const s_FUN_00403fa0 =
    reinterpret_cast<FUN_00403fa0_t>(0x00403fa0u);

// 0x004041c0  FUN_004041c0 — sub-state-6 renderer for all modes (C2).
using FUN_004041c0_t = void (__cdecl*)();
static FUN_004041c0_t const s_FUN_004041c0 =
    reinterpret_cast<FUN_004041c0_t>(0x004041c0u);


// ---------------------------------------------------------------------------
// Globals referenced by PerModeRenderMachine
// ---------------------------------------------------------------------------

// DAT_007f0fd0 — game-mode/screen selector; compared against 5, 8, 9, 10.
static constexpr std::uintptr_t kGameMode_007f0fd0 = 0x007f0fd0u;

// DAT_007d3ff8 — RW globals base; [0] = camera ptr, [8] = render-state fn-ptr.
// Layout: uint32_t* array at 0x007d3ff8; element[0] = camera; element[8] = rs_fn.
static constexpr std::uintptr_t kRwGlobals_007d3ff8 = 0x007d3ff8u;

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

// Retrieve camera pointer from RW globals[0].
static int GetCameraPtr() {
    return *reinterpret_cast<const int*>(kRwGlobals_007d3ff8);
}

// Call the RW render-state function: ((fn**)(globals[8]))(state_id, value).
// DAT_007d3ff8[8] is a function pointer pointer at offset 8*4=32 bytes from base.
static void SetRWState(int state_id, int value) {
    using RSFn_t = void (__cdecl*)(int, int);
    // globals[8] = *(uint32_t*)(kRwGlobals_007d3ff8 + 8*4)
    const std::uintptr_t rs_fn_ptr_addr = kRwGlobals_007d3ff8 + 8u * sizeof(std::uint32_t);
    const RSFn_t fn = *reinterpret_cast<RSFn_t*>(rs_fn_ptr_addr);
    fn(state_id, value);
}

// Execute the shared prologue for modes 5/8/9:
//   1. Save camera view-window (offsets 0x68/0x6c)
//   2. EndUpdate
//   3. SetViewWindow(0.8, 0.8)
//   4. BeginUpdate → returns 0 on failure (early return)
//   5. SetRWState(6, 0); SetRWState(8, 0)
// Returns camera ptr, or 0 on BeginUpdate failure.
static int ModePrologue(float* pSavedVW) {
    const int iVar4 = GetCameraPtr();

    // Save current view-window x/y from camera struct offsets 0x68/0x6c
    const std::uintptr_t cam_base = static_cast<std::uintptr_t>(
        static_cast<std::uint32_t>(iVar4));
    pSavedVW[0] = *reinterpret_cast<const float*>(cam_base + kCamViewWindowX_off);
    pSavedVW[1] = *reinterpret_cast<const float*>(cam_base + kCamViewWindowY_off);

    // EndUpdate
    s_RwCameraEndUpdate(iVar4);

    // SetViewWindow(0.8, 0.8)  [0x3f4ccccd = 0.8f]
    float vw[2] = { 0.8f, 0.8f };
    s_RwCameraSetViewWindow(iVar4, vw);

    // BeginUpdate — early return 0 on failure
    if (s_RwCameraBeginUpdate(iVar4) == 0) {
        return 0;
    }

    // Set RW render states
    SetRWState(kRS6, 0);
    SetRWState(kRS8, 0);

    return iVar4;
}

// Execute the shared epilogue for modes 5/8/9 (LAB_004043be):
//   1. Restore RS#6 = 1, RS#8 = 1
//   2. EndUpdate
//   3. Restore view-window from saved values
//   4. BeginUpdate (resume normal rendering)
static void ModeEpilogue(int iVar4, float* pSavedVW) {
    SetRWState(kRS6, 1);
    SetRWState(kRS8, 1);
    s_RwCameraEndUpdate(iVar4);
    s_RwCameraSetViewWindow(iVar4, pSavedVW);
    s_RwCameraBeginUpdate(iVar4);
}

// Dispatch sub-state for modes 5/8/9 (3-query chain).
// Returns true if a sub-state match was found and rendered.
static bool DispatchSubStateModes589(int mode) {
    float saved_vw[2];
    const int iVar4 = ModePrologue(saved_vw);
    if (iVar4 == 0) return true;  // BeginUpdate failed — prologue already done; skip

    // Sub-state query chain: check 3/4/5 first, then 6
    const int iVar1 = s_GetRenderSubMode();
    if (iVar1 == 5 || iVar1 == 4 || iVar1 == 3) {
        if (mode == 9)       { s_Render_00403d30(); }
        else if (mode == 8)  { s_Render_00403db0(); }
        else /* mode == 5 */ { s_Render_00403ed0(); }
        ModeEpilogue(iVar4, saved_vw);
        return true;
    }

    // Re-query for sub-state 6
    const int iVar1b = s_GetRenderSubMode();
    if (iVar1b == 6) {
        s_FUN_004041c0();
        ModeEpilogue(iVar4, saved_vw);
        return true;
    }

    // No matching sub-state — still restore
    ModeEpilogue(iVar4, saved_vw);
    return false;
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

// 0x00404320
extern "C" __declspec(dllexport) void __cdecl PerModeRenderMachine() {
    const std::uint32_t mode =
        *reinterpret_cast<const std::uint32_t*>(kGameMode_007f0fd0);

    if (mode == 9u) {
        DispatchSubStateModes589(9);
        return;
    }
    if (mode == 8u) {
        DispatchSubStateModes589(8);
        return;
    }
    if (mode == 5u) {
        DispatchSubStateModes589(5);
        return;
    }
    if (mode == 10u) {
        // Mode 10: prologue + per-entry loop + inlined epilogue
        float saved_vw[2];
        const int iVar4 = ModePrologue(saved_vw);
        if (iVar4 == 0) return;

        // Sub-state dispatch for mode 10
        const int sub = s_GetRenderSubMode();
        if (sub == 5 || sub == 4 || sub == 3) {
            s_FUN_00403c60();
        } else {
            // Re-query for sub-state 6
            const int sub2 = s_GetRenderSubMode();
            if (sub2 == 6) {
                // Per-entry loop over DAT_00636b88..0x636c00 (6 entries × 20 bytes)
                // entry[4] at offset 0x10 (float); gate: DAT_005d757c < entry[4]
                const float sentinel =
                    *reinterpret_cast<const float*>(kMode10Sentinel_005d757c);
                const std::uint8_t* entry =
                    reinterpret_cast<const std::uint8_t*>(kMode10Table_00636b88);
                const std::uint8_t* entry_end =
                    reinterpret_cast<const std::uint8_t*>(kMode10TableEnd);
                int entry_idx = 0;
                while (entry < entry_end) {
                    const float entry_float =
                        *reinterpret_cast<const float*>(entry + kMode10EntryFloat_off);
                    if (sentinel < entry_float) {
                        s_FUN_00403fa0(entry_idx);
                    }
                    entry += kMode10EntryStride;
                    ++entry_idx;
                }
            }
        }

        // Mode-10 has inlined epilogue (not the shared LAB_004043be)
        SetRWState(kRS6, 1);
        SetRWState(kRS8, 1);
        s_RwCameraEndUpdate(iVar4);
        s_RwCameraSetViewWindow(iVar4, saved_vw);
        s_RwCameraBeginUpdate(iVar4);
        return;
    }

    // No matching mode — void return (e.g. main menu, mode 0)
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(PerModeRenderMachine, 0x00404320);
