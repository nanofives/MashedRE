// Mashed RE — RW Im2D -> D3D9 bridge implementation (Milestone B15).
// See RwIm2DBridge.h for the design rationale.

#include "RwIm2DBridge.h"

#include <windows.h>
#include <intrin.h>
#include <cstdio>
#include <cstring>

#include "DrawStreamDump.h"

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
    std::uint32_t color;   // D3DCOLOR 0xAARRGGBB — the reimpls' cited byte-swap
                           // (U-3415) already produced the device color, same
                           // as the original's DAT_00898a20 stream. Submit raw.
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
struct TexEntry { int handle; IDirect3DTexture9* tex; bool point_filter; };
TexEntry          g_texMap[kMaxTexHandles] = {};
int               g_texMapCount = 0;
bool              g_curTexPoint = false;   // resolved with the texture each draw

IDirect3DTexture9* ResolveTexture(int handle) {
    g_curTexPoint = false;
    if (handle == 0) return nullptr;
    for (int i = 0; i < g_texMapCount; ++i) {
        if (g_texMap[i].handle == handle) {
            g_curTexPoint = g_texMap[i].point_filter;
            return g_texMap[i].tex;
        }
    }
    return nullptr;
}

// (Parity adjudication 2026-06-12: the old abgr_to_argb R<->B re-swap here is
// REMOVED. The original's RW D3D driver submits the Im2D vertex color raw —
// the device capture of MASHED's DAT_00898a20 holds D3DCOLOR bytes — so the
// bridge must too. Callers pass the original's packed dwords; the reimpls'
// cited swap (U-3415) is the only conversion, exactly as in MASHED.)

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

    // Parity harness (MASHED_DBG_DRAWSTREAM): record the raw source blob +
    // mirrored state before any conversion. No-op when the env var is unset.
    DrawStreamDump_OnDraw(verts, count, g_curTexHandle, g_alphaBlend,
                          g_srcBlend, g_dstBlend, _ReturnAddress());

    // One-shot draw logger (MASHED_DBG_DRAWLOG=1): dump the first 150 bridge
    // draws to the log so a white-flood painter can be identified exactly.
    {
        static int s_dbg = -1;
        if (s_dbg == -1)
            s_dbg = (GetEnvironmentVariableA("MASHED_DBG_DRAWLOG", nullptr, 0) != 0)
                        ? 150 : 0;
        if (s_dbg > 0) {
            --s_dbg;
            if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
                std::fprintf(f,
                    "DRAWLOG n=%d v0=(%.1f,%.1f) v3=(%.1f,%.1f) col=%08lx "
                    "blend=%d tex=%ld\n",
                    count, src[0].x, src[0].y,
                    src[count - 1].x, src[count - 1].y,
                    static_cast<unsigned long>(src[0].color),
                    g_alphaBlend ? 1 : 0,
                    static_cast<long>(g_curTexHandle));
                std::fclose(f);
            }
        }
    }
    D3DVert        dst[64];
    for (int i = 0; i < count; ++i) {
        // D3D9 pixel-center correction (-0.5): pre-transformed verts address
        // pixel CENTERS; the reimpls (like MASHED's own Im2D buffer) emit
        // edge coordinates, and the real RW D3D9 driver applies this shift
        // after the captured tap point. Without it, POINT-sampled textures
        // (the FGDC20 font) bleed the neighbor atlas cell's last column at
        // quad edges (the "leading tick").
        dst[i].x     = src[i].x - 0.5f;
        dst[i].y     = src[i].y - 0.5f;
        dst[i].z     = src[i].z;
        // RHW must be non-zero for pre-transformed verts; the reimpl writes 1.0f
        // into +0x0c, but guard against a zeroed buffer.
        dst[i].rhw   = (src[i].w != 0.0f) ? src[i].w : 1.0f;
        dst[i].color = src[i].color;
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
        // Per-texture filter: the FGDC20 charset renders POINT-sampled in the
        // original (FUN_00554940 sets render-state 9 from the raster's native
        // field on bind; pixel evidence verify/font_cmp_exit4x.png — the
        // original's glyph edges are blocky-solid, LINEAR thins the strokes).
        const D3DTEXTUREFILTERTYPE f =
            g_curTexPoint ? D3DTEXF_POINT : D3DTEXF_LINEAR;
        g_device->SetSamplerState(0, D3DSAMP_MINFILTER, f);
        g_device->SetSamplerState(0, D3DSAMP_MAGFILTER, f);
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

// B17: probe that the EXACT byte range [addr, addr+len) is committed and
// writable. A granule-base VirtualQuery is fooled by sub-granule NLS section
// mappings (e.g. a READONLY locale.nls view occupying the head of a 64KB granule
// while our target address sits in a free/foreign tail): the granule base reads
// COMMIT, but the real write address still AVs. This does a SEH-guarded
// read-modify-write of the first and last byte — the only reliable test that the
// address is genuinely ours to write. Restores the bytes it touched.
static bool ProbeWritable(std::uintptr_t addr, std::size_t len) {
    if (len == 0) return false;
    volatile std::uint8_t* lo = reinterpret_cast<volatile std::uint8_t*>(addr);
    volatile std::uint8_t* hi = reinterpret_cast<volatile std::uint8_t*>(addr + len - 1);
    __try {
        std::uint8_t a = *lo; *lo = a; *lo = a;
        std::uint8_t b = *hi; *hi = b; *hi = b;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool RwIm2DBridge_Install(IDirect3DDevice9* device) {
    g_device = device;

    std::memset(&g_fakeDevice, 0, sizeof(g_fakeDevice));
    g_fakeDevice.z_field      = 0;  // Z = 0.0f for screen-space pre-transformed verts
    g_fakeDevice.set_state_fn = reinterpret_cast<void*>(&Bridge_SetRenderState);
    g_fakeDevice.draw_fn      = reinterpret_cast<void*>(&Bridge_DrawPrimitive);

    // B17 robustness: the install writes the fake-device pointer to *(0x007d3ff8),
    // and every draw reimpl writes a 4-vertex buffer (4 * 0x1c bytes) at
    // kVBufBase=0x00898a20 and reads back through *(0x007d3ff8). If EITHER address
    // is not genuinely committed-writable (sub-granule NLS poisoning — see
    // ProbeWritable), installing would set up a bridge whose first per-frame draw
    // AVs in the render loop, OR the install write itself AVs here and kills the
    // process before it reaches the main loop. Probe the EXACT addresses and bail
    // gracefully (no bridge, no chrome — but the window still boots) if either is
    // unusable. This is the fix for the ~60% cold-start crashes, which the B17
    // region maps pinned to this exact write.
    if (!ProbeWritable(kRwDeviceSlot, sizeof(std::uintptr_t))) {
        return false;  // device-slot granule poisoned/uncommitted
    }
    if (!ProbeWritable(kVBufBase, 4u * sizeof(SrcVert))) {
        return false;  // vertex-buffer granule poisoned/uncommitted
    }

    __try {
        *reinterpret_cast<std::uintptr_t*>(kRwDeviceSlot) =
            reinterpret_cast<std::uintptr_t>(&g_fakeDevice);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}

void RwIm2DBridge_RegisterTexture(int handle, IDirect3DTexture9* texture) {
    if (handle == 0) return;
    for (int i = 0; i < g_texMapCount; ++i) {
        if (g_texMap[i].handle == handle) { g_texMap[i].tex = texture; return; }
    }
    if (g_texMapCount < kMaxTexHandles) {
        g_texMap[g_texMapCount].handle       = handle;
        g_texMap[g_texMapCount].tex          = texture;
        g_texMap[g_texMapCount].point_filter = false;
        ++g_texMapCount;
    }
}

void RwIm2DBridge_SetTexturePointFilter(int handle, bool point) {
    for (int i = 0; i < g_texMapCount; ++i) {
        if (g_texMap[i].handle == handle) {
            g_texMap[i].point_filter = point;
            return;
        }
    }
}

std::uint32_t RwIm2DBridge_DrawCount() { return g_drawCount; }

}  // namespace D3d9Render
}  // namespace mashed_re
