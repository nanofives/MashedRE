// Mashed RE - Frontend menu chrome: dim setter and fullscreen Im2D quad.
// Analysis notes:
//   re/analysis/frontend_promote_menus_a/0x0042aad0.md
//   re/analysis/frontend_promote_menus_a/0x0042aae0.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// MenuDimSet  --  0x0042aad0
//
// Original: FUN_0042aad0 (14 bytes, 0x0042aad0..0x0042aade)
// Non-standard calling convention: EAX at entry carries a pointer.
// No declared parameters; implicit in_EAX.
//
// Assembly (cited from Ghidra at 0x0042aad0):
//   MOV  byte ptr [EAX+3], 0x30    ; write 0x30 to byte at in_EAX+3
//   MOV  dword ptr [0x008990e4], 1 ; set global dim-enable flag
//   RET
//
// [UNCERTAIN U-0452] in_EAX source unknown at call site; struct at in_EAX+3 unresolved.
// [UNCERTAIN U-0453] extraout_ECX/EDX captured by caller FUN_0043c5b0 may be artifact.
// ---------------------------------------------------------------------------

// DAT_008990e4: dim-enable flag global.  (cited at 0x0042aad3-ish within body)
static constexpr std::uintptr_t kDimEnableFlag = 0x008990e4u;

// 0x0042aad0
// NAKED: EAX at entry is the implicit pointer arg; we must capture it before
// any standard prologue clobbers it. After capturing, implement the two writes.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MenuDimSet()
{
    __asm {
        // EAX at entry holds the implicit pointer (in_EAX).
        // Write byte 0x30 to *(in_EAX + 3).   (0x0042aad0 body)
        mov  byte ptr [eax+3], 0x30
        // Write int32 1 to DAT_008990e4.        (0x0042aad0 body)
        mov  dword ptr [kDimEnableFlag], 1
        ret
    }
}

RH_ScopedInstall(MenuDimSet, 0x0042aad0);  // re-enabled 2026-05-24 c3-frontend-a


// ---------------------------------------------------------------------------
// MenuIm2DQuad  --  0x0042aae0
//
// Original: FUN_0042aae0 (286 bytes, 0x0042aae0..0x0042abfd)
// Signature: void __fastcall FUN_0042aae0(undefined4 param_1)
//   param_1: forwarded as third arg to vtable call at DAT_007d3ff8+0x20.
//
// Renders a fullscreen dim-overlay quad via RwIm2D:
//   1. Render-state setup via vtable.
//   2. Build 4-vertex buffer at 0x0067ec30 (stride 28 bytes):
//      V0=(0,0), V1=(W,0), V2=(0,H), V3=(W,H) where W/H from DAT_0067ea54/56.
//   3. Per-vertex: Z_recip from vtable+0x18, W=1.0f, ARGB=0xd0808080.
//   4. Alpha from DAT_0067eca8 shifted to bits[31:24]; applied to all 4 vertices.
//   5. Draw call: vtable+0x30(4, &DAT_0067ec30, 4) if DAT_0067eca8 != 0.
//
// [UNCERTAIN U-0454] Vertex offsets +12..+24 not written here; UV/padding unresolved.
// [UNCERTAIN U-0455] vtable+0x30 consistent with RwIm2DRenderPrimitive; not confirmed.
// [UNCERTAIN U-0456] Render-state indices 10/11/12 not matched to named RW constants.
// ---------------------------------------------------------------------------

// Globals cited in body of 0x0042aae0:
static constexpr std::uintptr_t kVtableGlobal  = 0x007d3ff8u; // render vtable ptr
static constexpr std::uintptr_t kScreenWidth    = 0x0067ea54u; // screen width (int)
static constexpr std::uintptr_t kScreenHeight   = 0x0067ea56u; // screen height (int) -- note: int16 stored at int16 boundary
static constexpr std::uintptr_t kVertexBufBase  = 0x0067ec30u; // Im2D vertex buffer (4 verts x 28 bytes)
static constexpr std::uintptr_t kAlphaGlobal    = 0x0067eca8u; // alpha byte (0 suppresses draw)

// Vertex buffer layout (stride 28 = 7 dwords per vertex):
//   +0  (uint32): Z_recip from vtable+0x18
//   +4  (float ): 1.0f (W)
//   +8  (uint32): ARGB color
//   +12..+24: not written here [UNCERTAIN U-0454]
// Vertex 0 base: 0x0067ec30
// Vertex 1 base: 0x0067ec4c  (+0x1c = +28)
// Vertex 2 base: 0x0067ec68  (+0x38 = +56)
// Vertex 3 base: 0x0067ec84  (+0x54 = +84)
// Vertex X float at: ec30(v0), ec4c(v1), ec68(v2), ec84(v3)
// Vertex Y float at: ec34(v0), ec50(v1) [zero], ec6c(v2), ec88(v3)

// RW vtable function pointer types (inferred from call args):
typedef void (__cdecl* RwRenderState_t)(int state, int value);
typedef void (__cdecl* RwIm2DRender_t)(int prim_type, void* verts, int count);
typedef std::uint32_t* RwVtable_t;  // vtable is accessed as uint32 array

// Export alias: MSVC __fastcall decorates as @MenuIm2DQuad@4; linker pragma
// creates an undecorated alias so the Frida harness can find "MenuIm2DQuad".
#pragma comment(linker, "/export:MenuIm2DQuad=@MenuIm2DQuad@4")

// 0x0042aae0
extern "C" __declspec(dllexport) void __fastcall MenuIm2DQuad(int param_1)
{
    // Step 1: render-state call (DAT_007d3ff8 + 0x20)(0x14, 1, param_1)
    //   cited at 0x0042aae0 body entry.
    std::uintptr_t vtable_ptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
    auto* rs_fn = *reinterpret_cast<RwRenderState_t*>(vtable_ptr + 0x20);
    // Third-arg call: reinterpret as 3-arg variant inline.
    // The vtable function at +0x20 takes (state, value, extra) per decomp.
    typedef void (__cdecl* RwSetState3_t)(int, int, int);
    auto* rs3_fn = *reinterpret_cast<RwSetState3_t*>(vtable_ptr + 0x20);
    rs3_fn(0x14, 1, param_1);  // (20, 1, param_1) — cited at 0x0042aae0

    // Step 2: screen dimensions.
    //   DAT_0067ea54 = screen width int, DAT_0067ea56 = screen height int.
    //   Cited in body at float conversion.
    float W = static_cast<float>(static_cast<int>(*reinterpret_cast<std::int16_t*>(kScreenWidth)));
    float H = static_cast<float>(static_cast<int>(*reinterpret_cast<std::int16_t*>(kScreenHeight)));

    // Step 3: build vertex buffer. Stride 28 bytes. 4 vertices.
    //   V0=(0,0), V1=(W,0), V2=(0,H), V3=(W,H).
    //   Zero X/Y floats first, then set non-zero corners.
    *reinterpret_cast<float*>(0x0067ec30u) = 0.0f;  // V0.X = 0
    *reinterpret_cast<float*>(0x0067ec34u) = 0.0f;  // V0.Y = 0
    *reinterpret_cast<float*>(0x0067ec4cu) = W;      // V1.X = W
    *reinterpret_cast<float*>(0x0067ec50u) = 0.0f;  // V1.Y = 0
    *reinterpret_cast<float*>(0x0067ec68u) = 0.0f;  // V2.X = 0
    *reinterpret_cast<float*>(0x0067ec6cu) = H;      // V2.Y = H
    *reinterpret_cast<float*>(0x0067ec84u) = W;      // V3.X = W (copy of V1.X)
    *reinterpret_cast<float*>(0x0067ec88u) = H;      // V3.Y = H (copy of V2.Y)

    // Step 4: loop over 4 vertices: set Z_recip and W=1.0f and ARGB.
    //   puVar2 starts at 0x0067ec38, step +28, while < 0x0067eca8.
    //   Cited in body of 0x0042aae0.
    static constexpr float kVertW = 1.0f;                // 0x3f800000 cited at body
    static constexpr std::uint32_t kInitArgb = 0xd0808080u; // cited at body

    // Re-read vtable for Z_recip field (vtable+0x18 contains reciprocal Z/W)
    vtable_ptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
    std::uint32_t z_recip_bits = *reinterpret_cast<std::uint32_t*>(vtable_ptr + 0x18);

    for (auto* p = reinterpret_cast<std::uint32_t*>(0x0067ec38u);
         p < reinterpret_cast<std::uint32_t*>(0x0067eca8u);
         p = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(p) + 28u))
    {
        p[0] = z_recip_bits;                          // +0: Z_recip from vtable+0x18
        *reinterpret_cast<float*>(&p[1]) = kVertW;    // +4: W = 1.0f
        p[2] = kInitArgb;                             // +8: ARGB = 0xd0808080
    }

    // Step 5: render-state calls through vtable+0x20.
    //   (1,0), (6,0), (8,0), (0xc,1), (0xa,5), (0xb,6). Cited at body.
    vtable_ptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
    rs_fn = *reinterpret_cast<RwRenderState_t*>(vtable_ptr + 0x20);
    rs_fn(1,    0);
    rs_fn(6,    0);
    rs_fn(8,    0);
    rs_fn(0xc,  1);
    rs_fn(0xa,  5);
    rs_fn(0xb,  6);

    // Step 6: alpha from DAT_0067eca8 << 24; apply to all 4 vertex ARGB words.
    //   Cited at body of 0x0042aae0.
    std::uint32_t alpha_byte = *reinterpret_cast<std::uint32_t*>(kAlphaGlobal);
    std::uint32_t alpha_argb = alpha_byte << 24u;   // shift to bits[31:24]

    *reinterpret_cast<std::uint32_t*>(0x0067ec40u) = alpha_argb;  // V0.ARGB final
    *reinterpret_cast<std::uint32_t*>(0x0067ec5cu) = alpha_argb;  // V1.ARGB final
    *reinterpret_cast<std::uint32_t*>(0x0067ec78u) = alpha_argb;  // V2.ARGB final
    *reinterpret_cast<std::uint32_t*>(0x0067ec94u) = alpha_argb;  // V3.ARGB final

    // Step 7: draw call if alpha != 0.
    //   vtable+0x30(4, &DAT_0067ec30, 4).  [UNCERTAIN U-0455]
    if (alpha_byte != 0u) {
        vtable_ptr = *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
        auto* draw_fn = *reinterpret_cast<RwIm2DRender_t*>(vtable_ptr + 0x30);
        draw_fn(4, reinterpret_cast<void*>(kVertexBufBase), 4);
    }
}

RH_ScopedInstall(MenuIm2DQuad, 0x0042aae0);  // re-enabled 2026-05-24 c3-frontend-a
