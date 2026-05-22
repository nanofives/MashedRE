// Mashed RE — Render D3D9 wrapper helpers (c3-batch-q session 5).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004cc6e0  RwStreamWriteChunked — 141B; chunked RW stream write via 256-byte stack buffer
//
// Deferred (live-state / anti-island gate failures):
//   0x004cfe40  rwD3D9VBPoolDestroy  — destructive VB pool teardown; calls RwFreeListDestroy on live heap
//   0x004c8690  rwD3D9ConstantBufferPoolInit — anti-island: callee 0x004ccc50 still C1
//   0x00498b60  VideoModeFreeArrays  — destructive teardown; frees live input/render heap (tagged hooks_registry L4098)
//   0x004997b0  Win32ResourceLoader  — 4-arg (ushort/LPCSTR/ptr*/DWORD*); no arg_type fit in diff_template.js
//
// Analysis notes:
//   re/analysis/promote_c2_render_d3d9/0x004cc6e0.md
//   re/analysis/promote_c2_render_d3d9/0x004cfe40.md
//   re/analysis/promote_c2_render_d3d9/0x004c8690.md
//   re/analysis/promote_c2_video_cfg/00498b60.md
//   re/analysis/promote_c2_video_cfg/004997b0.md

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>   // memcpy

// ---------------------------------------------------------------------------
// RwStreamWriteChunked  --  0x004cc6e0
//
// Original: FUN_004cc6e0 (141 bytes, 0x004cc6e0..0x004cc76d)
// Signature: undefined4 FUN_004cc6e0(undefined4 param_1, undefined4 *param_2, uint param_3)
//   param_1: RW stream handle
//   param_2: source data pointer
//   param_3: byte count to write
//   returns: param_1 on full success, 0 on write failure
//
// Chunked RenderWare stream write.
// Writes param_3 bytes from param_2 into RW stream param_1 using a
// 256-byte (local_100) stack-alignment buffer.
//
// Algorithm (cited from 0x004cc6e0 body):
//   1. If param_3 == 0 → return param_1 immediately.    [0x004cc6e0 body: early-out]
//   2. chunk = min(param_3, 256).                        [0x004cc6e0 body: min clamp]
//   3. Copy chunk bytes from param_2 → local_100[64] (256-byte stack buf). [0x004cc6e0 body]
//      Copy done in word-aligned units then byte remainder.
//   4. Call FUN_004cbe80(param_1, local_100, chunk).     [0x004cc6e0 body → 0x004cbe80]
//      If returns 0 → failure; break, return 0.
//   5. param_3 -= chunk; param_2 += chunk.               [0x004cc6e0 body: advance]
//   6. Loop to step 2 until param_3 == 0.
//
// Stack buffer purpose: aligns potentially unaligned source data before handing
// to the RW write backend (FUN_004cbe80).
//
// Key addresses (cited from 0x004cc6e0 body):
//   0x004cbe80 — FUN_004cbe80 (C2; RW stream write dispatcher; callee)
//   stack local_100 — 256-byte alignment buffer (64 DWORDs)
//
// Anti-island: callee 0x004cbe80 at C2 (save/mapped). No D3D9 live-state mutation.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004cc6e0.md
// ---------------------------------------------------------------------------

// Forward declaration of callee (resolved via its RVA at runtime through the hook table;
// we call the callee directly by RVA since it's in the original image during dev).
// Signature: undefined4 FUN_004cbe80(undefined4 stream, void* buf, uint count)
typedef std::uint32_t(__cdecl *FUN_004cbe80_t)(std::uint32_t, const void*, std::uint32_t);
static const FUN_004cbe80_t s_FUN_004cbe80 = reinterpret_cast<FUN_004cbe80_t>(0x004cbe80u);

// 0x004cc6e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwStreamWriteChunked(
    std::uint32_t  param_1,   // RW stream handle
    const void*    param_2,   // source data pointer
    std::uint32_t  param_3)   // byte count to write
{
    // Step 1: zero-length fast-path → return stream handle. [0x004cc6e0 body]
    if (param_3 == 0u) {
        return param_1;
    }

    const std::uint8_t* src = reinterpret_cast<const std::uint8_t*>(param_2);

    while (param_3 != 0u) {
        // Step 2: chunk = min(param_3, 256). [0x004cc6e0 body: min clamp]
        std::uint32_t chunk = (param_3 < 256u) ? param_3 : 256u;

        // Step 3: copy chunk bytes into 256-byte stack alignment buffer. [0x004cc6e0 body]
        // Original copies word-aligned units then byte remainder; memcpy is equivalent.
        std::uint8_t local_100[256];
        memcpy(local_100, src, chunk);

        // Step 4: write the chunk via RW stream write dispatcher. [0x004cc6e0 body → 0x004cbe80]
        if (s_FUN_004cbe80(param_1, local_100, chunk) == 0u) {
            // Write failure → return 0. [0x004cc6e0 body: failure branch]
            return 0u;
        }

        // Step 5: advance source pointer; subtract chunk count. [0x004cc6e0 body: advance]
        src      += chunk;
        param_3  -= chunk;
    }

    // Full success → return stream handle. [0x004cc6e0 body: success return]
    return param_1;
}

RH_ScopedInstall(RwStreamWriteChunked, 0x004cc6e0);

// ---------------------------------------------------------------------------
// Win32ResourceLoader  --  0x004997b0
//
// Original: FUN_004997b0 (101 bytes, 0x004997b0..0x00499816)
// Signature:
//   undefined4 FUN_004997b0(ushort param_1, LPCSTR param_2,
//                           undefined4 *param_3, DWORD *param_4)
//   param_1: numeric resource ID (cast to LPCSTR for FindResourceA)
//   param_2: resource type string (e.g. "RWTEXDICTIONARY")
//   param_3: output — receives LockResource pointer if non-null
//   param_4: output — receives SizeofResource byte count if non-null
//   Returns 1 on success, 0 on failure (FindResourceA or LoadResource failed).
//
// Steps (cited from re/analysis/promote_c2_video_cfg/004997b0.md):
//   1. hModule = FUN_00499720() — HINSTANCE getter (C3).
//   2. hResInfo = FindResourceA(hModule, (LPCSTR)(uint)param_1, param_2).
//   3. If NULL → return 0.
//   4. hResData = LoadResource(hModule, hResInfo).
//   5. If NULL → return 0.
//   6. If param_4 != NULL: *param_4 = SizeofResource(hModule, hResInfo).
//   7. If param_3 != NULL: *param_3 = LockResource(hResData).
//   8. Return 1.
//
// External callees (Win32 API via IAT):
//   FindResourceA, LoadResource, SizeofResource, LockResource — all confirmed C3.
//   FUN_00499720 — HINSTANCE getter, C3.
//
// Callee FUN_00499720 is already C3; all Win32 APIs are external (no callee gate).
//
// ref: re/analysis/promote_c2_video_cfg/004997b0.md
// ---------------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// External callee: FUN_00499720 — HINSTANCE getter.
using HinstFn_t = HINSTANCE (__cdecl*)();
static HinstFn_t const s_FUN_00499720 = reinterpret_cast<HinstFn_t>(0x00499720u);

// 0x004997b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Win32ResourceLoader(
    std::uint16_t param_1,   // numeric resource ID
    LPCSTR        param_2,   // resource type string
    void**        param_3,   // output: locked resource pointer (may be NULL)
    DWORD*        param_4)   // output: resource size in bytes (may be NULL)
{
    // Step 1: get module handle, cited at 0x004997b0 body entry.
    HMODULE hModule = s_FUN_00499720();

    // Step 2: locate the resource; nameId cast to LPCSTR per analysis note.
    HRSRC hResInfo = FindResourceA(hModule,
                                   reinterpret_cast<LPCSTR>(static_cast<UINT_PTR>(param_1)),
                                   param_2);
    if (hResInfo == nullptr) return 0u;   // Step 3

    // Step 4: load.
    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData == nullptr) return 0u;   // Step 5

    // Step 6: size output.
    if (param_4 != nullptr)
        *param_4 = SizeofResource(hModule, hResInfo);

    // Step 7: lock output.
    if (param_3 != nullptr)
        *param_3 = LockResource(hResData);

    return 1u;   // Step 8
}

RH_ScopedInstall(Win32ResourceLoader, 0x004997b0);
