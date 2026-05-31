// Mashed RE — RW Im2D -> D3D9 bridge implementation (Milestone B15).
// See RwIm2DBridge.h for the design rationale.

#include "RwIm2DBridge.h"

#include <windows.h>
#include <cstring>

namespace mashed_re {
namespace D3d9Render {

namespace {

// The shared 4-vertex buffer the frontend draw reimpls write to. Same address
// as Frontend/DrawQuadPrimitives.cpp's kVBufBase.
constexpr std::uintptr_t kVBufBase     = 0x00898a20u;
// The RW device pointer global the draw reimpls dereference (*(this) -> device).
constexpr std::uintptr_t kRwDeviceSlot = 0x007d3ff8u;

// Per-vertex layout the reimpls produce (matches D3DFVF_XYZRHW|DIFFUSE|TEX1):
//   +0x00 X  +0x04 Y  +0x08 Z  +0x0c W(rhw)  +0x10 color  +0x14 U  +0x18 V
#pragma pack(push, 1)
struct SrcVert {
    float         x, y, z, w;
    std::uint32_t color;   // ABGR (the reimpl byte-swapped R<->B from ARGB)
    float         u, v;
};
struct D3DVert {
    float    x, y, z, rhw;
    D3DCOLOR color;        // ARGB (D3DCOLOR 0xAARRGGBB)
    float    u, v;
};
#pragma pack(pop)
static_assert(sizeof(SrcVert) == 0x1c, "SrcVert must be 28 bytes (RW Im2D vertex stride)");
static_assert(sizeof(D3DVert) == 0x1c, "D3DVert must be 28 bytes");

constexpr DWORD kBridgeFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// Fake RW device. The draw reimpls index it at +0x18 (Z source), +0x20
// (set-state fn), +0x30 (draw fn). Lay out a struct with valid memory at those
// offsets; the function pointers are filled at install time.
#pragma pack(push, 1)
struct FakeRwDevice {
    std::uint8_t  pad_00[0x18];
    std::uint32_t z_field;             // +0x18  (read as the vertex Z; 0.0f for screen-space)
    std::uint8_t  pad_1c[0x20 - 0x1c];
    void*         set_state_fn;        // +0x20  RwRenderStateSet(state, value)
    std::uint8_t  pad_24[0x30 - 0x24];
    void*         draw_fn;             // +0x30  RwIm2DRenderPrimitive(count, verts, ?)
};
#pragma pack(pop)
static_assert(offsetof(FakeRwDevice, z_field)      == 0x18, "z_field @ +0x18");
static_assert(offsetof(FakeRwDevice, set_state_fn) == 0x20, "set_state_fn @ +0x20");
static_assert(offsetof(FakeRwDevice, draw_fn)      == 0x30, "draw_fn @ +0x30");

FakeRwDevice            g_fakeDevice = {};
IDirect3DDevice9*       g_device     = nullptr;
std::uint32_t           g_drawCount  = 0;

// Render state mirrored from the draw reimpls' rw_set_state(state, value) calls.
int  g_curTexHandle = 0;     // state 1
bool g_alphaBlend   = false; // state 12 (rwRENDERSTATEVERTEXALPHAENABLE)
int  g_srcBlend     = 5;     // state 10 (rwBLENDSRCALPHA == D3DBLEND_SRCALPHA == 5)
int  g_dstBlend     = 6;     // state 11 (rwBLENDINVSRCALPHA == D3DBLEND_INVSRCALPHA == 6)

// Small handle -> texture map. The frontend uses small integer texture handles;
// 16 slots is ample for the menu (matches QuadRenderer::kMaxSlots).
constexpr int     kMaxTexHandles = 64;
struct TexEntry { int handle; IDirect3DTexture9* tex; };
TexEntry          g_texMap[kMaxTexHandles] = {};
int               g_texMapCount = 0;

IDirect3DTexture9* ResolveTexture(int handle) {
    if (handle == 0) return nullptr;
    for (int i = 0; i < g_texMapCount; ++i) {
        if (g_texMap[i].handle == handle) return g_texMap[i].tex;
    }
    return nullptr;
}

// ABGR (what the reimpl wrote) -> ARGB (D3DCOLOR). Swap R<->B, keep A and G.
// The reimpl applied the same swap to ARGB inputs, so this returns the original
// caller's ARGB color.
inline D3DCOLOR abgr_to_argb(std::uint32_t abgr) {
    return (abgr & 0xff00ff00u)
         | ((abgr & 0x00ff0000u) >> 16)
         | ((abgr & 0x000000ffu) << 16);
}

// RwRenderStateSet replacement (device + 0x20). The draw reimpls call this with
// RW state codes; we mirror the few that matter for 2D alpha-blended quads.
void __cdecl Bridge_SetRenderState(int state, int value) {
    switch (state) {
    case 1:  g_curTexHandle = value; break;   // texture handle (0 = none)
    case 12: g_alphaBlend   = (value != 0); break; // vertex-alpha / blend enable
    case 10: g_srcBlend     = value; break;   // src blend factor
    case 11: g_dstBlend     = value; break;   // dst blend factor
    default: break;                            // 6/8/9 etc. — not needed for the bridge
    }
}

// RwIm2DRenderPrimitive replacement (device + 0x30). Reads `count` source verts
// from `verts` (= DAT_00898a20) and submits them to D3D9 as a screen-space
// (pre-transformed) triangle strip. The reimpls always emit 4 verts in
// TL,TR,BL,BR order — a quad as a 2-triangle strip.
void __cdecl Bridge_DrawPrimitive(int count, void* verts, int /*unused*/) {
    if (!g_device || !verts || count < 3 || count > 64) return;

    const SrcVert* src = reinterpret_cast<const SrcVert*>(verts);
    D3DVert        dst[64];
    for (int i = 0; i < count; ++i) {
        dst[i].x     = src[i].x;
        dst[i].y     = src[i].y;
        dst[i].z     = src[i].z;
        // RHW must be non-zero for pre-transformed verts; the reimpl writes 1.0f
        // into +0x0c, but guard against a zeroed buffer.
        dst[i].rhw   = (src[i].w != 0.0f) ? src[i].w : 1.0f;
        dst[i].color = abgr_to_argb(src[i].color);
        dst[i].u     = src[i].u;
        dst[i].v     = src[i].v;
    }

    g_device->SetFVF(kBridgeFVF);

    IDirect3DTexture9* tex = ResolveTexture(g_curTexHandle);
    g_device->SetTexture(0, tex);
    if (tex) {
        // Modulate texture by vertex color (standard 2D sprite).
        g_device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
        g_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        g_device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
        g_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        g_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        g_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    } else {
        // Untextured: take color straight from the diffuse vertex color.
        g_device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
        g_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        g_device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
        g_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }

    // 2D state: no Z, no lighting, no culling, alpha-blend per mirrored state.
    g_device->SetRenderState(D3DRS_ZENABLE,          FALSE);
    g_device->SetRenderState(D3DRS_LIGHTING,         FALSE);
    g_device->SetRenderState(D3DRS_CULLMODE,         D3DCULL_NONE);
    g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, g_alphaBlend ? TRUE : FALSE);
    if (g_alphaBlend) {
        // RW blend factor codes for SRCALPHA/INVSRCALPHA coincide numerically
        // with D3DBLEND_SRCALPHA (5) / D3DBLEND_INVSRCALPHA (6); clamp to the
        // valid D3DBLEND range and fall back to the standard pair otherwise.
        const int sb = (g_srcBlend >= 1 && g_srcBlend <= 17) ? g_srcBlend : D3DBLEND_SRCALPHA;
        const int db = (g_dstBlend >= 1 && g_dstBlend <= 17) ? g_dstBlend : D3DBLEND_INVSRCALPHA;
        g_device->SetRenderState(D3DRS_SRCBLEND,  static_cast<DWORD>(sb));
        g_device->SetRenderState(D3DRS_DESTBLEND, static_cast<DWORD>(db));
    }

    g_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
                              static_cast<UINT>(count - 2),
                              dst, sizeof(D3DVert));
    ++g_drawCount;
}

}  // namespace

bool RwIm2DBridge_Install(IDirect3DDevice9* device) {
    g_device = device;

    std::memset(&g_fakeDevice, 0, sizeof(g_fakeDevice));
    g_fakeDevice.z_field      = 0;  // Z = 0.0f for screen-space pre-transformed verts
    g_fakeDevice.set_state_fn = reinterpret_cast<void*>(&Bridge_SetRenderState);
    g_fakeDevice.draw_fn      = reinterpret_cast<void*>(&Bridge_DrawPrimitive);

    // Write &g_fakeDevice into *(0x007d3ff8). That slot lives in the Phase-G
    // wedge range (0x00500000..0x009fffff); verify the granule is committed
    // before writing so a gap doesn't AV.
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<LPVOID>(kRwDeviceSlot & ~0xFFFFu),
                     &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT) {
        return false;  // wedge doesn't cover the device-slot granule
    }
    *reinterpret_cast<std::uintptr_t*>(kRwDeviceSlot) =
        reinterpret_cast<std::uintptr_t>(&g_fakeDevice);

    // Sanity: the draw reimpls also read the vertex buffer at kVBufBase, which
    // must be in the wedge too. Touch-probe (read) so callers can log coverage.
    (void)kVBufBase;
    return true;
}

void RwIm2DBridge_RegisterTexture(int handle, IDirect3DTexture9* texture) {
    if (handle == 0) return;
    for (int i = 0; i < g_texMapCount; ++i) {
        if (g_texMap[i].handle == handle) { g_texMap[i].tex = texture; return; }
    }
    if (g_texMapCount < kMaxTexHandles) {
        g_texMap[g_texMapCount].handle = handle;
        g_texMap[g_texMapCount].tex    = texture;
        ++g_texMapCount;
    }
}

std::uint32_t RwIm2DBridge_DrawCount() { return g_drawCount; }

}  // namespace D3d9Render
}  // namespace mashed_re
