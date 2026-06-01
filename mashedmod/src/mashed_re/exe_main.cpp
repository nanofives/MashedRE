// Mashed RE — standalone executable.
// Milestone A (done): opens a real Win32 window, runs a message pump, exits cleanly.
// Milestone B0 (done): adds a D3D9 device and clears the backbuffer to a
//                      teal color every frame.
// Milestone B1 (done): loads original/TOASTART/Common/Frontend.piz at
//                      startup via Piz/PizReader (C++ port of
//                      re/tools/piz_extract.py), writes a directory
//                      listing to mashed_re.log next to the exe, and
//                      appends the entry count to the window title.
// Milestone B2 (done): extracts the TEXTURES.TXD blob from the loaded
//                      Frontend.piz to extracted/TEXTURES.TXD next to the exe,
//                      walks its RWS chunk tree via Rws/RwsChunkWalker, and
//                      appends the tree to mashed_re.log.
// Milestone B3 (done): decodes the proprietary chunk-0x23 TXD format via
//                      Txd/TxdDecoder, emits a per-texture table (name, w, h,
//                      bpp, format, mip count, palette status) to
//                      mashed_re.log, and reflects the decoded texture count
//                      in the window title.
// Milestone B4 (done): uploaded one decoded ARGB8888/PAL8 texture (default:
//                      "main", 256x256) to a D3D9 IDirect3DTexture9 via
//                      D3d9Render/QuadRenderer and drew it as a centered
//                      screen-space quad each frame on top of the teal clear.
// Milestone B5 (prior revision): uploads ALL 8 decoded Frontend.piz textures
//                               into QuadRenderer slots 0..7 and draws them
//                               as a fixed 4x2 atlas in the 800x600 window
//                               (160x160 content per cell, cell pitch 200x270,
//                               row 0 starts at y=50, row 1 at y=320).
//                               Window title reflects atlas mode.
// Milestone B6 (Phase F gate per FRONTEND_ROADMAP):
//                               renders ONE textured 2D quad — the 'main'
//                               texture (256x256 PAL8 from Frontend.piz/
//                               TEXTURES.TXD) — centered at title-screen
//                               position scaled to 512x512. This closes
//                               the "RW replacement layer renders 1
//                               textured sprite" milestone (M6, smallest-
//                               viable-demo path). Atlas mode is retained
//                               as a build-time fallback via kTitleMode
//                               toggle — set to false for the diagnostic
//                               4x2 view.
// Milestone B11 (Phase H input slice): adds DirectInput8 keyboard polling.
//                               Per-frame reads 256-byte key state, updates
//                               the window title with arrow / Enter / Esc.
// Milestone B12 (this revision; Phase H render-feedback slice): connects the
//                               DirectInput state to the title-screen
//                               render. Arrow keys translate the 'main'
//                               texture quad around the window in 4-pixel
//                               steps; Enter resets to center; Esc exits.
//                               First end-to-end input -> visual loop in
//                               the standalone. Pre-requisite for full
//                               menu-nav: once Phase F supplies an
//                               RwIm2DRenderPrimitive replacement, the same
//                               poll loop can drive MASHED's menu state
//                               machine.
// Milestone B7 (this revision; Phase G wedge): VirtualAlloc-maps the
//                               MASHED.exe data-section address range
//                               (0x00500000..0x009fffff) into the
//                               standalone's process at startup so that
//                               reimpls referencing MASHED RVAs (e.g.
//                               MenuEntryArrayInit zeroing the array at
//                               0x00898ac4) can run without AV. This is
//                               the prerequisite for compiling the
//                               Boot/Frontend/HUD/etc. reimpl set into
//                               the exe target — Phase C's runtime gate.
//                               After the mapping is established, calls
//                               one pure-leaf reimpl (MenuEntryArrayInit)
//                               to verify the wedge. Failure to allocate
//                               is non-fatal; rendering still proceeds.
// Milestone B15 (Phase F/H; 2026-05-30): RW Im2D -> D3D9 bridge. Installs a
//                               fake RW device at *(0x007d3ff8) so MASHED's
//                               bit-identical C3 Im2D draw reimpls
//                               (Frontend/DrawQuadPrimitives.cpp: HudIm2DQuad,
//                               ChromeBaseDraw, ...) render through our D3D9
//                               device instead of AV'ing on a null RW device.
//                               Each reimpl writes the DAT_00898a20 4-vertex
//                               buffer + dispatches via device +0x20/+0x30; the
//                               bridge (D3d9Render/RwIm2DBridge.cpp) translates
//                               that into DrawPrimitiveUP. Verified: HudIm2DQuad
//                               renders a correctly-colored alpha-blended quad
//                               (verify/b15_bridge.png). Also hardened boot:
//                               exe target now builds /EHa (so the boot chain's
//                               __try/__except guards catch partial-wedge AVs)
//                               and ExecuteFrontendBootChain step 1 zeroes
//                               granule-by-granule.
// Milestone B16 (Phase C/F/H; 2026-05-31): menu CHROME. Renders MASHED's menu
//                               chrome (top/bottom bands + white divider lines,
//                               MenuChromeShellA's 640x480 layout scaled to
//                               800x600) through the B15 bridge. Two paths:
//                               (a) FAITHFUL — call MASHED's real MenuChromeShellA
//                               (0x0042e3a0); its by-RVA calls to the Im2D draws
//                               + screen getters are redirected by standalone RVA
//                               thunks (Compat/StandaloneRvaThunks: VirtualAlloc
//                               the MASHED .text granules RWX + write E9 JMP
//                               thunks -> our reimpls/stubs). Gated on cold-start
//                               granule availability (getter granule 0x00420000
//                               is often unclaimable). (b) RELIABLE — issue the
//                               same chrome via self-contained HudIm2DQuad (no
//                               .text-granule dep); renders on every boot.
//                               Verified: verify/b16_chrome.png.
//
// This file is intentionally self-contained. The Boot/*.cpp reimplementations
// (Window.cpp, VideoConfig.cpp, FrameDispatch.cpp, RwEngineInit.cpp, ...) read
// from MASHED.exe absolute globals (HWND at 0x007e9584, MSG buffer at
// 0x007e95a0, quit flag at 0x00773918, etc.). Those addresses are part of
// MASHED.exe's process image; they don't exist in mashed_re.exe's address
// space and cannot be reused as-is. The .asi target still loads them for hook
// verification — they prove our understanding of MASHED's boot is correct.
// For the standalone exe we replicate the *pattern* using our own
// locally-defined state.
//
// D3D9 pattern lifted from mashedmod/src/d3d9_shim/d3d9_shim.cpp (our own
// CreateDevice proxy) and from the canonical D3D9 init flow Mashed uses:
//   Direct3DCreate9(D3D_SDK_VERSION) -> IDirect3D9*
//   IDirect3D9::CreateDevice(0, HAL, hwnd, SOFTWARE_VERTEXPROCESSING, &pp,
//                            &device)
//   pp.Windowed = TRUE, BackBufferWidth/Height = 800x600,
//   pp.SwapEffect = D3DSWAPEFFECT_DISCARD,
//   pp.BackBufferFormat = D3DFMT_UNKNOWN (driver picks)
//
// What this does:
//   1. RegisterClassA on a custom WNDPROC.
//   2. CreateWindowExA at 800x600 (matches Mashed's canonical videocfg).
//   3. ShowWindow / UpdateWindow.
//   4. Direct3DCreate9 + CreateDevice (HAL, software vertex processing).
//   5. Message pump loop with RenderFrame() each iteration:
//        device->Clear(0, NULL, D3DCLEAR_TARGET, teal, 1.0f, 0)
//        device->BeginScene(); device->EndScene();
//        device->Present(NULL, NULL, NULL, NULL)
//   6. Device-lost handling: on D3DERR_DEVICELOST wait for D3DERR_DEVICENOTRESET
//      then Reset(); on Reset failure, exit.
//   7. WNDPROC handles WM_DESTROY (sets quit flag) and WM_KEYDOWN ESC.
//   8. On exit, Release(device) + Release(d3d) + DestroyWindow + UnregisterClassA.
//
// What this does NOT do:
//   - Load any Mashed asset (.piz, videocfg.bin, etc.).
//   - Initialise RenderWare (RwEngineInit and friends).
//   - Draw anything inside the cleared region.

#include <windows.h>
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <psapi.h>      // B17: GetMappedFileName — identify MEM_MAPPED squatters
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <climits>

#include "Piz/PizReader.h"
#include "Rws/RwsChunkWalker.h"
#include "Txd/TxdDecoder.h"
#include "D3d9Render/QuadRenderer.h"
#include "D3d9Render/RwIm2DBridge.h"        // B15: RW Im2D -> D3D9 bridge
#include "Compat/StandaloneRvaThunks.h"     // B16: standalone RVA-thunk installer

// B15 — the frontend's bit-identical C3 Im2D quad draw (0x00450b10,
// Frontend/DrawQuadPrimitives.cpp). Writes the DAT_00898a20 vertex buffer and
// dispatches through *(0x007d3ff8) — which RwIm2DBridge_Install() points at our
// fake RW device. Calling this from the standalone routes the draw into D3D9.
//   tex_handle: 0 = untextured (uses diffuse color); else a handle registered
//               via RwIm2DBridge_RegisterTexture.
//   uv: [u0_bits, v0_bits, u1_bits, v1_bits] as float32 bit patterns.
extern "C" void __cdecl HudIm2DQuad(
    std::int32_t tex_handle, float x, float y, float w, float h,
    std::uint32_t argb, std::uint32_t* uv);

// B16 — MASHED's real per-frame menu-chrome drawer (0x0042e3a0,
// Frontend/MenuSpriteDispatch.cpp). It draws two gradient bands + two white
// divider lines by calling the C3 Im2D draws *by MASHED RVA*
// (TextGradientV0V1Override @0x00472f40, TextGradientV2V3Override @0x004730b0,
// ChromeBaseDraw @0x00472c60) and the screen-dim getters @0x0042b8b0/c0. Those
// RVAs are redirected to our standalone reimpls/stubs by the B16 thunk install
// below, so calling MenuChromeShellA() renders the actual menu chrome via the
// B15 bridge. The three Im2D draws are also declared so we can thunk to them.
extern "C" void __cdecl MenuChromeShellA(void);
extern "C" void __cdecl ChromeBaseDraw(float x, float y, float w, float h, std::uint32_t argb);
extern "C" void __cdecl TextGradientV0V1Override(float x, float y, float w, float h, std::uint32_t argb);
extern "C" void __cdecl TextGradientV2V3Override(float x, float y, float w, float h, std::uint32_t argb);

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "psapi.lib")

// ===========================================================================
// B17 — earliest-possible low-arena reservation (TLS callback).
//
// Root cause of the cold-start lottery (proven by the B17 region maps): the exe
// is based at 0x10000000, vacating the legacy 0x00400000..0x009fffff range. The
// Windows bottom-up VA allocator then fills that range with NLS code-page section
// mappings (C_1252/C_850/l_intl/locale.nls — READONLY), the process heaps, and
// d3d9's heap (which grows during CreateDevice). Their placement varies per run,
// so our reimpls' hardcoded MASHED RVAs (.text getters/draws at 0x42xxxx/0x47xxxx,
// .data/.bss at 0x63xxxx/0x67xxxx/0x7dxxxx/0x7fxxxx/0x89xxxx, .rdata scale at
// 0x5cxxxx) land sometimes in free space (claimable), sometimes inside a foreign
// mapping (lost) — gating boot AND the faithful MenuChromeShellA path.
//
// Fix: a TLS callback runs in LdrpRunInitializeRoutines — before CRT init and
// before WinMain, the earliest user code in the process. It RESERVEs (no commit)
// the exact 64KB granules our reimpls need, claiming them while they are still
// free so the bottom-up allocator routes d3d9/heaps/CRT-init NLS around them.
// WinMain's existing commit paths (MapMashedDataSection for >=0x500000,
// StandaloneThunks_MapRange for 0x420000/0x470000/0x5c0000) then commit our
// MEM_RESERVE granules on demand. Granules already occupied by kernel/ntdll-early
// NLS at TLS time cannot be reclaimed; the WinMain region dump reports them.
//
// Reserving only the exact granules (512KB of VA, 0 committed) — not the whole
// arena — keeps d3d9's CreateDevice low-address space largely unobstructed, per
// the prior finding that a committed 4MB wedge before d3d9 caused ~60% failures.
// MUST use only kernel32/ntdll calls here: the CRT is not yet initialized.
// ===========================================================================
namespace {
const std::uintptr_t kB17ReserveBases[] = {
    0x00420000u,  // .text getters (0x0042b8b0/c0, credits 0x0042d5a0)
    0x00470000u,  // .text Im2D draws (0x00472c60/2f40, 0x004730b0)
    0x005c0000u,  // .rdata scale consts (1/640 @5cd5a8, 1/480 @5cc560)
    0x00630000u,  // .data boot params (0x00636ae8) + timer (0x0063d558)
    0x00670000u,  // .data race-state array (0x0067ea10)
    0x007d0000u,  // .data RW device slot (*(0x007d3ff8)) — B15 bridge
    0x007f0000u,  // .bss big buffer (0x007f0f60) + input (0x007f1044)
    0x00890000u,  // .bss menu-entry array + vtx buffer (0x00898a20/ac0)
};
volatile long g_b17_tls_reserved  = 0;  // granules we reserved at TLS time
volatile long g_b17_tls_seen_free = 0;  // granules that were FREE at TLS time
}  // namespace

extern "C" void NTAPI B17_TlsReserve(PVOID, DWORD reason, PVOID) {
    if (reason != DLL_PROCESS_ATTACH) return;
    for (std::size_t i = 0; i < sizeof(kB17ReserveBases) / sizeof(kB17ReserveBases[0]); ++i) {
        const std::uintptr_t base = kB17ReserveBases[i];
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(reinterpret_cast<LPVOID>(base), &mbi, sizeof(mbi)) == 0) continue;
        if (mbi.State == MEM_FREE) {
            ++g_b17_tls_seen_free;
            if (VirtualAlloc(reinterpret_cast<LPVOID>(base), 0x10000u,
                             MEM_RESERVE, PAGE_READWRITE) != nullptr) {
                ++g_b17_tls_reserved;
            }
        }
    }
}

// Register B17_TlsReserve as an EXE TLS callback. /INCLUDE:__tls_used forces the
// PE TLS directory to be emitted (x86 decoration); the CRT$XLB slot is one of the
// loader-invoked callback entries.
#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma section(".CRT$XLB", long, read)
extern "C" __declspec(allocate(".CRT$XLB"))
    PIMAGE_TLS_CALLBACK g_b17_tls_cb = B17_TlsReserve;

namespace {

// Local boot state. Replaces MASHED's globals at 0x007e9584 / 0x00773918 / etc.
HWND               g_hwnd          = nullptr;
MSG                g_msg           = {};
bool               g_quit          = false;
bool               g_active        = true;

// D3D9 state. MASHED keeps its IDirect3DDevice9* somewhere inside the
// RenderWare driver struct (DAT_007d3ff8 vtable + offsets); we own ours
// directly.
IDirect3D9*        g_d3d           = nullptr;
IDirect3DDevice9*  g_device        = nullptr;
D3DPRESENT_PARAMETERS g_pp         = {};

// B11 — DirectInput8 keyboard state. MASHED stashes its DInput device inside
// FUN_004997b0 / FUN_004998a0 callchain; we own ours directly.
IDirectInput8A*       g_dinput        = nullptr;
IDirectInputDevice8A* g_kbd           = nullptr;
unsigned char         g_keys[256]     = {};   // raw DInput key state, set 0x80 if held
unsigned char         g_keys_prev[256]= {};   // last frame, for edge detection

constexpr int   kWidth         = 800;
constexpr int   kHeight        = 600;
constexpr char  kClassName[]   = "MashedRE";

// Path to one Mashed asset archive. Resolved relative to the exe's working
// directory (Mashed itself does the same — TOASTART/Common/* paths). For B1 we
// pick Frontend.piz because it's the menu's asset bundle; once Milestone B
// proper starts drawing menus we'll already have its blobs in hand.
constexpr char  kFrontendPiz[] = "original/TOASTART/Common/Frontend.piz";

// Log path next to the exe.
constexpr char  kLogPath[]     = "mashed_re.log";

// Window title is built dynamically once Piz load completes.
char            g_windowTitle[256] = "Mashed RE (Milestone B1 - loading...)";
mashed_re::Piz::Archive g_frontend_piz;

// B3 keeps the decoded TXD dictionary alive at file scope so its `Texture::Mip`
// pixel pointers (which reference bytes inside g_frontend_piz's loaded blob)
// remain valid for B4's upload step. Frontend.piz must therefore stay loaded
// for the lifetime of the program — which it does.
mashed_re::Txd::Dictionary g_txd;

// B4/B5 — D3D9 textured-quad renderer. Built after device init; uploads
// decoded TXD textures and draws them each frame. B5 fills slots 0..N-1 with
// every texture in g_txd.
mashed_re::D3d9Render::QuadRenderer g_quad_renderer;

// B5 — atlas layout constants. The 8 Frontend.piz textures fit into a 4x2
// grid in the 800x600 backbuffer. Cell pitch is 200 px horizontally and
// 270 px vertically; content is a square of kAtlasContentSize px regardless
// of source aspect, so all textures render at the same on-screen size for
// visual comparison.
//
// Cell center x: (col * 200) + 100        -> 100, 300, 500, 700
// Cell center y: 50 + (row * 270) + 100   -> 150, 420
//
// Row 0 content occupies y = 70..230 (160 px high, centered on 150),
// row 1 content occupies y = 340..500. Both fit comfortably in 0..600.
constexpr std::uint32_t kAtlasCols        = 4;
constexpr std::uint32_t kAtlasRows        = 2;
constexpr float         kAtlasCellPitchX  = 200.f;
constexpr float         kAtlasCellPitchY  = 270.f;
constexpr float         kAtlasOriginX     = 100.f;     // center of col 0
constexpr float         kAtlasOriginY     = 150.f;     // center of row 0
constexpr float         kAtlasContentSize = 160.f;

// B5 — uploaded slot count, set once at startup by UploadAllTexturesForAtlas().
std::uint32_t g_atlas_slot_count = 0;

// B6 — title-screen mode (Phase F gate). When true (default), render a single
// centered 'main' texture instead of the 4x2 atlas. Set to false to restore
// the B5 diagnostic atlas view.
constexpr bool kTitleMode = true;

// B6 — title-screen layout constants. 'main' is 256x256 PAL8; we render it
// at 512x512 centered, which is half the backbuffer height (600 - 512 = 88
// vertical headroom). On 800x600 this leaves a 144-pixel left/right border.
// Slot 0 is reserved for the title texture; atlas slots start at 1.
constexpr std::uint32_t kTitleSlotIndex   = 0;
constexpr float         kTitleQuadCenterX = 400.f;
constexpr float         kTitleQuadCenterY = 300.f;
constexpr float         kTitleQuadSize    = 512.f;

// B12 — keyboard-driven offset applied to the title quad. Arrow keys
// translate the quad in 4-px steps; Enter resets to (0, 0). The center+offset
// is clamped to keep the half-quad on-screen (so its center can range from
// kTitleQuadSize/2 to kWidth-kTitleQuadSize/2 horizontally, ditto vertically).
float g_titleQuadOffsetX = 0.f;
float g_titleQuadOffsetY = 0.f;
constexpr float kTitleQuadStep = 4.f;

// B13 — currently-displayed atlas slot in title mode. PgUp/PgDn cycle through
// all uploaded textures (0..g_atlas_slot_count-1). Wraps around. Lets the user
// visually verify every Frontend.piz TEXTURES.TXD entry decodes + uploads
// cleanly, not just 'main'.
std::uint32_t g_titlePickedSlot = 0;
// Name of the texture to use as the title-screen logo. Picked because it's
// the only 256x256 PAL8 in Frontend.piz's TEXTURES.TXD — MASHED's menu code
// references this exact name at multiple call sites. Per the B3 log row 5.
constexpr const char    kTitleTexName[]   = "main";

// B7 — MASHED data-section emulation. MASHED.exe's .data starts shortly after
// our exe's image ends (we're loaded at 0x00400000 and end around 0x00470000
// for a 400KB exe). MASHED's data extends to ~0x008fffff. VirtualAlloc-map
// the gap so reimpls' deref of fixed RVAs (0x00500000+) succeed with zeroed
// memory rather than AV. Address range chosen to:
//   - skip our own image (0x00400000..~0x00470000 reserved by PE loader),
//   - cover MASHED's .data + .bss footprint,
//   - leave room for DLL imports the loader places between 0x70000000+.
constexpr std::uintptr_t kMashedDataBase  = 0x00500000u;
constexpr std::size_t    kMashedDataSize  = 0x00500000u; // 5 MB -> 0x00500000..0x009fffff
bool                     g_mashed_data_mapped = false;

// Distinctive teal clear color so we can confirm visually that D3D9 init
// worked. RGB(40, 80, 120) reads as a muted dark teal — clearly not the
// default desktop or window background.
constexpr D3DCOLOR kClearColor  = D3DCOLOR_XRGB(40, 80, 120);

// B15 — RW Im2D -> D3D9 bridge state. When the fake RW device is installed at
// *(0x007d3ff8), the frontend's C3 Im2D draw reimpls (HudIm2DQuad etc.) render
// through D3D9. The per-frame demo below calls HudIm2DQuad directly to prove the
// bridge end-to-end: one textured quad (the title texture, by handle) plus one
// untextured semi-transparent colored quad. Both are drawn BY MASHED's actual
// draw function, routed through our renderer.
bool             g_bridge_installed = false;
constexpr int    kBridgeTitleHandle = 1;    // handle we register slot 0's texture under
constexpr bool   kBridgeDemo        = false; // B15 proof quads (off; B16 chrome supersedes)

// B16 — set once MASHED's MenuChromeShellA can be driven in the standalone (its
// called RVAs are thunked to our reimpls and the scale constants are written).
bool             g_b16_chrome_ready = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        return 0;

    case WM_DESTROY:
        g_quit = true;
        PostQuitMessage(0);
        return 0;

    case WM_ACTIVATE:
        g_active = (LOWORD(wp) != WA_INACTIVE);
        return 0;

    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        break;

    default:
        break;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// Single-iteration message pump. Pattern lifted from WindowMsgPump
// (re/analysis/window_msgpump/00499690.md, reimpl Boot/Window.cpp at 0x00499690)
// but operating on local state. Returns true when the application should exit.
bool PumpOnce() {
    if (PeekMessageA(&g_msg, nullptr, 0, 0, PM_REMOVE) != 0) {
        if (g_msg.message == WM_QUIT) {
            return true;
        }
        TranslateMessage(&g_msg);
        DispatchMessageA(&g_msg);
    } else if (!g_active) {
        WaitMessage();
    }
    return g_quit;
}

// B11 — DirectInput8 keyboard init. Called after the window is created so we
// can set the cooperative level against g_hwnd. Background+NonExclusive lets
// us read keys without stealing focus, matching the title-screen UX. Failures
// are non-fatal — the frame loop still renders the title-screen quad, just
// without keyboard navigation.
bool InitDirectInput(HINSTANCE hInstance) {
    HRESULT hr = DirectInput8Create(
        hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8A,
        reinterpret_cast<void**>(&g_dinput),
        nullptr);
    if (FAILED(hr) || !g_dinput) {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                         "\nMilestone B11: DirectInput8Create FAILED hr=0x%08lX\n",
                         static_cast<unsigned long>(hr));
            std::fclose(log);
        }
        return false;
    }
    hr = g_dinput->CreateDevice(GUID_SysKeyboard, &g_kbd, nullptr);
    if (FAILED(hr) || !g_kbd) {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                         "\nMilestone B11: CreateDevice(SysKeyboard) FAILED hr=0x%08lX\n",
                         static_cast<unsigned long>(hr));
            std::fclose(log);
        }
        g_dinput->Release(); g_dinput = nullptr;
        return false;
    }
    g_kbd->SetDataFormat(&c_dfDIKeyboard);
    g_kbd->SetCooperativeLevel(g_hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    g_kbd->Acquire();
    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log,
                     "\nMilestone B11: DirectInput8 + keyboard device acquired "
                     "(cooperative = background+nonexclusive)\n");
        std::fclose(log);
    }
    return true;
}

void ShutdownDirectInput() {
    if (g_kbd)    { g_kbd->Unacquire(); g_kbd->Release(); g_kbd = nullptr; }
    if (g_dinput) { g_dinput->Release(); g_dinput = nullptr; }
}

// B11 — single-shot keyboard poll. Saves prior state, fetches new state.
// Re-acquires on focus loss (DIERR_INPUTLOST / DIERR_NOTACQUIRED) so the
// next poll succeeds.
void PollKeyboard() {
    if (!g_kbd) return;
    std::memcpy(g_keys_prev, g_keys, sizeof(g_keys));
    HRESULT hr = g_kbd->GetDeviceState(sizeof(g_keys), g_keys);
    if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
        g_kbd->Acquire();
        std::memset(g_keys, 0, sizeof(g_keys));
    } else if (FAILED(hr)) {
        std::memset(g_keys, 0, sizeof(g_keys));
    }
}

// B11 — per-frame title refresh showing current arrow/Enter/Esc state. This
// is the proof-of-concept: keystrokes change the window title in real time.
// Updates only when the key set changes (avoids SetWindowText spam).
void UpdateTitleFromKeyboard() {
    if (!g_kbd || !g_hwnd) return;
    const bool up    = (g_keys[DIK_UP]    & 0x80) != 0;
    const bool down  = (g_keys[DIK_DOWN]  & 0x80) != 0;
    const bool left  = (g_keys[DIK_LEFT]  & 0x80) != 0;
    const bool right = (g_keys[DIK_RIGHT] & 0x80) != 0;
    const bool enter = (g_keys[DIK_RETURN]& 0x80) != 0;
    const bool esc   = (g_keys[DIK_ESCAPE]& 0x80) != 0;
    // Pack into an int so we only refresh title on transitions OR quad
    // position changes (which happen while arrows are held) OR slot pick
    // changes (B13 PgUp/PgDn cycling). Without these, the title would freeze
    // mid-action.
    static int           s_last_packed = -1;
    static int           s_last_qx     = INT_MIN;
    static int           s_last_qy     = INT_MIN;
    static std::uint32_t s_last_slot   = UINT32_MAX;
    const int packed = (up<<0)|(down<<1)|(left<<2)|(right<<3)|(enter<<4)|(esc<<5);
    const int qx = static_cast<int>(g_titleQuadOffsetX);
    const int qy = static_cast<int>(g_titleQuadOffsetY);
    const std::uint32_t slot = g_titlePickedSlot;
    if (packed == s_last_packed && qx == s_last_qx && qy == s_last_qy
        && slot == s_last_slot) return;
    s_last_packed = packed;
    s_last_qx = qx;
    s_last_qy = qy;
    s_last_slot = slot;
    char buf[256];
    std::snprintf(buf, sizeof(buf),
                  "Mashed RE (B11 keys: %s%s%s%s%s%s)",
                  up    ? "UP "    : "",
                  down  ? "DOWN "  : "",
                  left  ? "LEFT "  : "",
                  right ? "RIGHT " : "",
                  enter ? "ENTER " : "",
                  esc   ? "ESC "   : "");
    const char* slot_name = "(none)";
    if (g_titlePickedSlot < g_txd.count()) {
        slot_name = g_txd.texture(g_titlePickedSlot).name;
    }
    if (!up && !down && !left && !right && !enter && !esc) {
        std::snprintf(buf, sizeof(buf),
                      "Mashed RE (B13 idle; slot %u/'%s' quad@%+d,%+d)",
                      g_titlePickedSlot, slot_name,
                      static_cast<int>(g_titleQuadOffsetX),
                      static_cast<int>(g_titleQuadOffsetY));
    } else {
        std::snprintf(buf, sizeof(buf),
                      "Mashed RE (B13 slot %u/'%s'; keys: %s%s%s%s%s%s; quad@%+d,%+d)",
                      g_titlePickedSlot, slot_name,
                      up    ? "UP "    : "",
                      down  ? "DOWN "  : "",
                      left  ? "LEFT "  : "",
                      right ? "RIGHT " : "",
                      enter ? "ENTER " : "",
                      esc   ? "ESC "   : "",
                      static_cast<int>(g_titleQuadOffsetX),
                      static_cast<int>(g_titleQuadOffsetY));
    }
    SetWindowTextA(g_hwnd, buf);
}

// B12 — translate the title quad based on held arrow keys. Enter resets. The
// clamp keeps the quad fully on the 800x600 backbuffer; without it, holding an
// arrow eventually walks the quad off-screen, leaving an empty teal frame
// (technically correct, visually misleading). 4-px step at 60 Hz = ~240 px/s.
void UpdateQuadFromKeyboard() {
    if (!g_kbd) return;
    const bool up    = (g_keys[DIK_UP]    & 0x80) != 0;
    const bool down  = (g_keys[DIK_DOWN]  & 0x80) != 0;
    const bool left  = (g_keys[DIK_LEFT]  & 0x80) != 0;
    const bool right = (g_keys[DIK_RIGHT] & 0x80) != 0;
    if (left)  g_titleQuadOffsetX -= kTitleQuadStep;
    if (right) g_titleQuadOffsetX += kTitleQuadStep;
    if (up)    g_titleQuadOffsetY -= kTitleQuadStep;
    if (down)  g_titleQuadOffsetY += kTitleQuadStep;
    // Reset on Enter rising edge so a held Enter doesn't pin offset at zero.
    const bool enter      = (g_keys[DIK_RETURN]      & 0x80) != 0;
    const bool enter_prev = (g_keys_prev[DIK_RETURN] & 0x80) != 0;
    if (enter && !enter_prev) {
        g_titleQuadOffsetX = 0.f;
        g_titleQuadOffsetY = 0.f;
    }
    // Clamp so the quad center stays within [half, dim-half] (half-quad-on-screen).
    const float halfQuad = kTitleQuadSize * 0.5f;
    const float minX = halfQuad - kTitleQuadCenterX;
    const float maxX = (static_cast<float>(kWidth)  - halfQuad) - kTitleQuadCenterX;
    const float minY = halfQuad - kTitleQuadCenterY;
    const float maxY = (static_cast<float>(kHeight) - halfQuad) - kTitleQuadCenterY;
    if (g_titleQuadOffsetX < minX) g_titleQuadOffsetX = minX;
    if (g_titleQuadOffsetX > maxX) g_titleQuadOffsetX = maxX;
    if (g_titleQuadOffsetY < minY) g_titleQuadOffsetY = minY;
    if (g_titleQuadOffsetY > maxY) g_titleQuadOffsetY = maxY;
}

// B13 — PgUp / PgDn cycle through uploaded atlas slots (the 8 Frontend.piz
// textures). Wraps. Reacts on rising edge only (otherwise a held key would
// blow through all 8 in two frames). Home key resets to slot 0.
void UpdateTitleSlotFromKeyboard() {
    if (!g_kbd || g_atlas_slot_count == 0) return;
    const bool pgup_now      = (g_keys[DIK_PGUP]      & 0x80) != 0;
    const bool pgup_prev     = (g_keys_prev[DIK_PGUP] & 0x80) != 0;
    const bool pgdn_now      = (g_keys[DIK_PGDN]      & 0x80) != 0;
    const bool pgdn_prev     = (g_keys_prev[DIK_PGDN] & 0x80) != 0;
    const bool home_now      = (g_keys[DIK_HOME]      & 0x80) != 0;
    const bool home_prev     = (g_keys_prev[DIK_HOME] & 0x80) != 0;
    if (pgdn_now && !pgdn_prev) {
        g_titlePickedSlot = (g_titlePickedSlot + 1u) % g_atlas_slot_count;
    }
    if (pgup_now && !pgup_prev) {
        g_titlePickedSlot = (g_titlePickedSlot + g_atlas_slot_count - 1u) % g_atlas_slot_count;
    }
    if (home_now && !home_prev) {
        g_titlePickedSlot = 0u;
    }
}

// B14 — write DirectInput key state into MASHED's per-player input-byte
// globals at 0x007f1044 (active) / 0x007f1504 (processed). Layout per
// re/analysis (also documented in Frontend/MenuButtonDetect.cpp):
//
//   active byte:    0x007f1044 + col + player * 0x4c   (6 cols, 4 players)
//   processed byte: 0x007f1504 + col + player * 0x4c
//
// Per MenuButtonDetectA/B the menu code reads:
//   `active != 0 && processed == 0`  => "newly pressed, ready to consume"
//   After consuming, MASHED sets processed = active so the same press
//   isn't acted on twice.
//
// Provisional column-to-key mapping (player 0 only, on keyboard):
//   col 0 = UP arrow
//   col 1 = DOWN arrow
//   col 2 = LEFT arrow
//   col 3 = RIGHT arrow
//   col 4 = ENTER (select)
//   col 5 = ESCAPE (back)
//
// These are best-guesses — once we run actual frontend nav code we can
// observe which col the menu reads and remap. Wedge granule for
// 0x007f0000 must be live for these writes; if not, SEH catches the
// AV and the standalone keeps running (just with no input visible to
// future frontend code).
void WriteMashedInputGlobalsFromKeyboard() {
    if (!g_kbd) return;
    constexpr std::uintptr_t kActiveBase    = 0x007f1044u;
    constexpr std::uintptr_t kProcessedBase = 0x007f1504u;
    // Player 0 stride * 0 = 0. We only write for player 0 — multi-player
    // menus aren't supported in standalone yet.
    struct ColKey { std::uint32_t col; std::uint8_t dik; };
    constexpr ColKey kMap[] = {
        { 0u, DIK_UP     },
        { 1u, DIK_DOWN   },
        { 2u, DIK_LEFT   },
        { 3u, DIK_RIGHT  },
        { 4u, DIK_RETURN },
        { 5u, DIK_ESCAPE },
    };
    __try {
        for (const auto& m : kMap) {
            const bool down      = (g_keys[m.dik]      & 0x80) != 0;
            const bool down_prev = (g_keys_prev[m.dik] & 0x80) != 0;
            volatile std::uint8_t* active =
                reinterpret_cast<std::uint8_t*>(kActiveBase    + m.col);
            volatile std::uint8_t* processed =
                reinterpret_cast<std::uint8_t*>(kProcessedBase + m.col);
            if (down) {
                *active = 1u;
                // Rising edge: re-arm processed so menu sees a fresh press.
                if (!down_prev) *processed = 0u;
            } else {
                *active = 0u;
                *processed = 0u;
            }
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        // 0x007f0000 granule not in wedge. Silent — caller doesn't need
        // to know; the standalone keeps running without input wiring.
    }
}

bool InitD3D9() {
    g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_d3d) return false;

    g_pp.Windowed               = TRUE;
    g_pp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    g_pp.BackBufferWidth        = kWidth;
    g_pp.BackBufferHeight       = kHeight;
    g_pp.BackBufferFormat       = D3DFMT_UNKNOWN; // current display format
    g_pp.hDeviceWindow          = g_hwnd;
    g_pp.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;

    HRESULT hr = g_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &g_pp,
        &g_device);
    if (FAILED(hr)) {
        g_d3d->Release();
        g_d3d = nullptr;
        return false;
    }
    return true;
}

void ShutdownD3D9() {
    // Release D3D9-owned resources (textures, vertex buffers) before the
    // device itself.
    g_quad_renderer.Shutdown();
    if (g_device) { g_device->Release(); g_device = nullptr; }
    if (g_d3d)    { g_d3d->Release();    g_d3d    = nullptr; }
}

// Render one frame: clear to teal, present. Returns false on unrecoverable
// device loss (caller should exit).
bool RenderFrame() {
    if (!g_device) return false;

    HRESULT hr = g_device->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST) {
        // Device lost (e.g. screensaver came up). Yield and try again next frame.
        Sleep(50);
        return true;
    }
    if (hr == D3DERR_DEVICENOTRESET) {
        hr = g_device->Reset(&g_pp);
        if (FAILED(hr)) return false;
    }

    g_device->Clear(0, nullptr, D3DCLEAR_TARGET, kClearColor, 1.0f, 0);
    g_device->BeginScene();
    if constexpr (kTitleMode) {
        // B6: title-screen MVP — single centered 'main' texture quad.
        // Phase F (M6) gate: "RW replacement renders 1 textured sprite".
        if (g_atlas_slot_count > 0) {
            // B13: render whichever slot the user has cycled to. Clamp under
            // a wraparound from input handling (defensive — should never trip
            // if PgUp/PgDn modulo math is correct).
            const std::uint32_t slot =
                (g_titlePickedSlot < g_atlas_slot_count)
                    ? g_titlePickedSlot
                    : 0u;
            g_quad_renderer.RenderAt(slot,
                                     kTitleQuadCenterX + g_titleQuadOffsetX,
                                     kTitleQuadCenterY + g_titleQuadOffsetY,
                                     kTitleQuadSize, kTitleQuadSize);
        }
    } else {
        // B5 diagnostic atlas mode (4x2 grid of all uploaded textures).
        for (std::uint32_t slot = 0; slot < g_atlas_slot_count; ++slot) {
            const std::uint32_t col = slot % kAtlasCols;
            const std::uint32_t row = slot / kAtlasCols;
            const float cx = kAtlasOriginX + static_cast<float>(col) * kAtlasCellPitchX;
            const float cy = kAtlasOriginY + static_cast<float>(row) * kAtlasCellPitchY;
            g_quad_renderer.RenderAt(slot, cx, cy,
                                     kAtlasContentSize, kAtlasContentSize);
        }
    }
    // B16: render MASHED's menu chrome (two dark bands + two white divider
    // lines) through the B15 bridge.
    if (g_bridge_installed) {
        if (g_b16_chrome_ready) {
            // Faithful path: call MASHED's ACTUAL frontend function
            // MenuChromeShellA (0x0042e3a0). Its by-RVA calls to the Im2D draws
            // + screen getters are redirected by the B16 thunks to our reimpls.
            // Available only when the 0x00420000/0x00470000 .text granules + the
            // scale-constant granule were all claimable this cold-start.
            MenuChromeShellA();
        } else {
            // Reliable path: the SAME chrome layout read from MenuChromeShellA's
            // decomp (640x480 virtual, scaled 1.25x -> 800x600) issued via the
            // self-contained HudIm2DQuad bridge draw. No dependence on the flaky
            // MASHED .text getter-granule, so this renders on every successful
            // boot. Bands are flat (HudIm2DQuad) vs MASHED's gradient, but the
            // chrome frame + divider lines match.
            std::uint32_t uv[4] = {0u, 0u, 0x3f800000u, 0x3f800000u};
            HudIm2DQuad(0, 0.f,   0.f, 800.f, 80.f, 0xa0000000u, uv); // top band
            HudIm2DQuad(0, 0.f, 520.f, 800.f, 80.f, 0xa0000000u, uv); // bottom band
            HudIm2DQuad(0, 0.f,  80.f, 800.f,  2.f, 0xffffffffu, uv); // top divider line
            HudIm2DQuad(0, 0.f, 520.f, 800.f,  2.f, 0xffffffffu, uv); // bottom divider line
        }
    }

    // B15: prove the RW Im2D -> D3D9 bridge by drawing through MASHED's actual
    // C3 HudIm2DQuad reimpl. One untextured semi-transparent quad (top-left) and
    // one textured quad (the title texture, bottom-right) — both submitted via
    // the fake RW device at *(0x007d3ff8).
    if constexpr (kBridgeDemo) {
        if (g_bridge_installed) {
            // Full-texture UV [0,0]..[1,1] as float32 bit patterns.
            std::uint32_t uv_full[4] = { 0x00000000u, 0x00000000u,
                                         0x3f800000u, 0x3f800000u };
            // Untextured: bright semi-transparent magenta rectangle, top-left.
            HudIm2DQuad(0, 40.f, 40.f, 220.f, 140.f, 0x80ff20ffu, uv_full);
            // Textured: the title texture (registered under kBridgeTitleHandle),
            // bottom-right, opaque white modulate.
            if (g_atlas_slot_count > 0) {
                HudIm2DQuad(kBridgeTitleHandle, 520.f, 380.f, 200.f, 180.f,
                            0xffffffffu, uv_full);
            }
        }
    }
    g_device->EndScene();

    hr = g_device->Present(nullptr, nullptr, nullptr, nullptr);
    if (hr == D3DERR_DEVICELOST) {
        return true; // recoverable; try next frame
    }
    return SUCCEEDED(hr);
}

// Load Frontend.piz at startup and write a directory listing to mashed_re.log.
// Updates g_windowTitle with a status string. Returns true if the archive was
// loaded with at least one entry; false otherwise (we don't bail the exe on
// failure — Milestone B1's contract is "try to load and report", not "must
// have assets to start").
bool LoadFrontendPiz() {
    // Appends; the per-run truncate happens once at WinMain entry so the
    // log accumulates entries from all earlier phases (e.g. the wedge
    // walk) without being clobbered here.
    std::FILE* log = std::fopen(kLogPath, "a");

    bool ok = g_frontend_piz.Load(kFrontendPiz);
    if (!ok) {
        std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                      "Mashed RE (Milestone B1 - %s: %s)",
                      kFrontendPiz, g_frontend_piz.last_error());
        if (log) {
            std::fprintf(log, "Frontend.piz LOAD FAILED: %s\n", g_frontend_piz.last_error());
            std::fclose(log);
        }
        return false;
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B1 - Frontend.piz: %u entries, %s mode)",
                  g_frontend_piz.count(),
                  g_frontend_piz.mode() == mashed_re::Piz::OffsetMode::Raw ? "raw" : "sector");

    if (log) {
        std::fprintf(log, "Frontend.piz loaded OK\n");
        std::fprintf(log, "  path:    %s\n", kFrontendPiz);
        std::fprintf(log, "  size:    %zu bytes\n", g_frontend_piz.data_size());
        std::fprintf(log, "  version: %u\n", g_frontend_piz.version());
        std::fprintf(log, "  entries: %u\n", g_frontend_piz.count());
        std::fprintf(log, "  mode:    %s\n",
                     g_frontend_piz.mode() == mashed_re::Piz::OffsetMode::Raw ? "raw" : "sector");
        std::fprintf(log, "\n");
        std::fprintf(log, "  %-60s %10s %10s %10s\n", "name", "offset", "length", "id");
        std::fprintf(log, "  %-60s %10s %10s %10s\n",
                     "------------------------------------------------------------",
                     "----------", "----------", "----------");
        for (std::uint32_t i = 0; i < g_frontend_piz.count(); ++i) {
            const auto& e = g_frontend_piz.entry(i);
            std::fprintf(log, "  %-60s 0x%08X 0x%08X 0x%08X\n",
                         e.name, e.offset_bytes, e.length, e.id);
        }
        std::fclose(log);
    }
    return true;
}

// Write the blob backing entry `i` of the loaded Frontend.piz to disk at
// `out_path`. Returns true on success.
bool ExtractBlobToFile(std::uint32_t entry_index, const char* out_path) {
    if (!g_frontend_piz.valid()) return false;
    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(entry_index, &length);
    if (!bytes) return false;

    std::FILE* fp = std::fopen(out_path, "wb");
    if (!fp) return false;
    std::size_t wrote = std::fwrite(bytes, 1, length, fp);
    std::fclose(fp);
    return wrote == length;
}

// Counter context for the walker callback.
struct ChunkCounter {
    std::FILE*    log;
    std::uint32_t total;
    std::uint32_t containers;
};

bool ChunkCallback(const mashed_re::Rws::ChunkInfo& info, void* user) {
    auto* ctx = static_cast<ChunkCounter*>(user);
    ctx->total += 1;
    if (mashed_re::Rws::IsKnownContainer(info.section_id)) ctx->containers += 1;
    if (ctx->log) {
        const char* name = mashed_re::Rws::IdName(info.section_id);
        // Indent two spaces per depth level for readability.
        for (std::uint32_t d = 0; d < info.depth; ++d) std::fputs("  ", ctx->log);
        std::fprintf(ctx->log,
                     "[%u] 0x%08X (%s) size=0x%08X version=0x%08X @ 0x%08zX\n",
                     info.depth, info.section_id,
                     name ? name : "?", info.size, info.version,
                     info.offset_in_buffer);
    }
    return true;
}

// Extracts TEXTURES.TXD (Frontend.piz entry 0) to extracted/TEXTURES.TXD next
// to the exe, then walks its RWS chunk tree and appends the result to the
// existing mashed_re.log. Updates g_windowTitle with the chunk count.
// Returns true if extraction + walk succeeded.
bool ExtractAndWalkTexturesTxd() {
    if (!g_frontend_piz.valid() || g_frontend_piz.count() == 0) return false;

    // Sanity: entry 0 in Frontend.piz is TEXTURES.TXD (verified in B1 log).
    constexpr std::uint32_t kEntryIdx = 0;
    const char* kOutDir  = "extracted";
    const char* kOutPath = "extracted/TEXTURES.TXD";

    CreateDirectoryA(kOutDir, nullptr); // OK if it already exists

    if (!ExtractBlobToFile(kEntryIdx, kOutPath)) {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log, "\nTEXTURES.TXD extract FAILED to %s\n", kOutPath);
            std::fclose(log);
        }
        return false;
    }

    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(kEntryIdx, &length);
    if (!bytes) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nTEXTURES.TXD extracted to %s (%u bytes)\n", kOutPath, length);
        std::fprintf(log, "RWS chunk tree:\n");
    }

    ChunkCounter ctx{};
    ctx.log = log;
    std::int32_t result = mashed_re::Rws::Walk(bytes, length, ChunkCallback, &ctx);

    if (log) {
        if (result < 0) {
            std::fprintf(log, "\n  ! walk reported parse error\n");
        } else {
            std::fprintf(log, "\n  total chunks: %u (containers: %u)\n",
                         ctx.total, ctx.containers);
        }
        std::fclose(log);
    }

    // Refine window title to include chunk count (B2 line; B3 overwrites it
    // below if decode succeeds).
    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B2 - %u piz entries, TEXTURES.TXD = %u chunks)",
                  g_frontend_piz.count(), ctx.total);
    return result >= 0;
}

// B3: Decode the proprietary chunk-0x23 TXD container from TEXTURES.TXD into a
// flat list of Texture records and dump a per-texture summary to mashed_re.log.
// Refines the window title to include the decoded texture count.
bool DecodeAndDumpTexturesTxd() {
    if (!g_frontend_piz.valid() || g_frontend_piz.count() == 0) return false;

    constexpr std::uint32_t kEntryIdx = 0; // entry 0 = TEXTURES.TXD (verified in B1)
    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(kEntryIdx, &length);
    if (!bytes) return false;

    bool ok = g_txd.Decode(bytes, length);

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nTXD decode (Milestone B3):\n");
        if (!ok) {
            std::fprintf(log, "  FAILED: %s\n", g_txd.last_error());
            std::fclose(log);
            return false;
        }
        std::fprintf(log, "  OK: %u textures, deviceId=%u\n",
                     g_txd.count(), g_txd.device_id());
        std::fprintf(log, "\n");
        std::fprintf(log,
            "  %-3s %-20s %5s %5s %4s %-5s %4s %-7s\n",
            "#", "name", "w", "h", "bpp", "fmt", "mips", "palette");
        std::fprintf(log,
            "  %-3s %-20s %5s %5s %4s %-5s %4s %-7s\n",
            "---", "--------------------", "-----", "-----",
            "----", "-----", "----", "-------");
        for (std::uint32_t i = 0; i < g_txd.count(); ++i) {
            const auto& t = g_txd.texture(i);
            const auto fmt = t.format();
            const std::uint8_t* pal = t.mip_count ? t.mips[0].palette : nullptr;
            std::fprintf(log,
                "  %-3u %-20s %5u %5u %4u %-5s %4u %-7s\n",
                i, t.name, t.width(), t.height(), t.depth(),
                mashed_re::Txd::PixelFormatName(fmt),
                t.mip_count, pal ? "yes" : "-");
        }
        std::fclose(log);
    }

    if (!ok) return false;

    // Refine window title with decoded texture count (B4 overwrites this
    // once a texture is uploaded successfully).
    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B3 - %u textures decoded)",
                  g_txd.count());
    return true;
}

// B6: upload the 'main' texture to slot 0 for the title-screen MVP. Looks up
// the texture by name in the decoded TXD; if not found, falls back to texture 0.
// Updates g_atlas_slot_count (set to 1 on success) + window title.
bool UploadTitleTextureForLogo() {
    if (!g_device || !g_txd.valid()) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nMilestone B6: title-screen mode — uploading '%s'\n",
                     kTitleTexName);
    }

    if (!g_quad_renderer.Init(g_device, kWidth, kHeight)) {
        if (log) {
            std::fprintf(log, "\nMilestone B6: QuadRenderer.Init failed: %s\n",
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    // Locate the title texture by name. Linear scan over the 8 TXD entries —
    // negligible cost on a one-shot init path.
    std::uint32_t title_idx = UINT32_MAX;
    for (std::uint32_t i = 0; i < g_txd.count(); ++i) {
        const auto& t = g_txd.texture(i);
        // Case-sensitive match. Names in Mashed's TXDs are stored as written.
        bool eq = true;
        for (std::size_t k = 0; k < sizeof(kTitleTexName); ++k) {
            if (t.name[k] != kTitleTexName[k]) { eq = false; break; }
        }
        if (eq) { title_idx = i; break; }
    }
    if (title_idx == UINT32_MAX) {
        if (log) {
            std::fprintf(log, "  '%s' not found in TXD; falling back to texture 0\n",
                         kTitleTexName);
        }
        title_idx = 0;
    }

    const auto& tex = g_txd.texture(title_idx);
    if (!g_quad_renderer.UploadFromTextureToSlot(kTitleSlotIndex, tex)) {
        if (log) {
            std::fprintf(log,
                         "  slot %u upload FAILED: '%s' %ux%u %s — %s\n",
                         kTitleSlotIndex, tex.name, tex.width(), tex.height(),
                         mashed_re::Txd::PixelFormatName(tex.format()),
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    g_atlas_slot_count = 1;

    if (log) {
        std::fprintf(log,
                     "  slot %u: '%s' %ux%u %s mips=%u OK (title-screen quad)\n",
                     kTitleSlotIndex, tex.name, tex.width(), tex.height(),
                     mashed_re::Txd::PixelFormatName(tex.format()),
                     tex.mip_count);
        std::fclose(log);
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B6 - title screen: '%s' %ux%u)",
                  tex.name, tex.width(), tex.height());
    if (g_hwnd) SetWindowTextA(g_hwnd, g_windowTitle);
    return true;
}

// B5: upload every decoded TXD texture into a separate QuadRenderer slot so
// the frame loop can draw them all as an atlas. Retained as fallback when
// kTitleMode is false. Stops at kMaxSlots (8 cells for the 4x2 layout).
// Per-texture upload failures are logged but don't abort the rest.
// Updates g_atlas_slot_count + window title.
bool UploadAllTexturesForAtlas() {
    if (!g_device || !g_txd.valid()) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nMilestone B5: uploading %u textures for atlas\n",
                     g_txd.count());
    }

    if (!g_quad_renderer.Init(g_device, kWidth, kHeight)) {
        if (log) {
            std::fprintf(log, "\nMilestone B5: QuadRenderer.Init failed: %s\n",
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    const std::uint32_t cap = kAtlasCols * kAtlasRows;
    std::uint32_t uploaded = 0;
    for (std::uint32_t i = 0; i < g_txd.count() && i < cap; ++i) {
        const auto& tex = g_txd.texture(i);
        if (g_quad_renderer.UploadFromTextureToSlot(i, tex)) {
            ++uploaded;
            if (log) {
                std::fprintf(log,
                             "  slot %u: '%s' %ux%u %s mips=%u OK\n",
                             i, tex.name, tex.width(), tex.height(),
                             mashed_re::Txd::PixelFormatName(tex.format()),
                             tex.mip_count);
            }
        } else if (log) {
            std::fprintf(log,
                         "  slot %u: '%s' %ux%u %s upload FAILED: %s\n",
                         i, tex.name, tex.width(), tex.height(),
                         mashed_re::Txd::PixelFormatName(tex.format()),
                         g_quad_renderer.last_error());
        }
    }

    g_atlas_slot_count = uploaded;

    if (log) {
        std::fprintf(log,
                     "  Milestone B5: %u/%u textures uploaded into atlas\n",
                     uploaded, g_txd.count());
        std::fclose(log);
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B5 - atlas: %u textures)",
                  uploaded);
    if (g_hwnd) SetWindowTextA(g_hwnd, g_windowTitle);
    return uploaded > 0;
}

// B17 — the set of 64KB granules the faithful frontend path needs MEM_COMMIT
// at once. Two live BELOW the wedge (the MASHED .text getter/draw granules,
// claimed by Compat/StandaloneRvaThunks); the rest are MASHED .data/.rdata/.bss
// granules inside the wedge range. Kept in one table so the diagnostic dumper,
// the deterministic reservation, and the wedge all agree on the target set.
struct NamedGranule { std::uintptr_t base; const char* what; };
constexpr NamedGranule kNeededGranules[] = {
    { 0x00420000u, ".text getters (0x0042b8b0/c0, credits 0x0042d5a0)" },
    { 0x00470000u, ".text Im2D draws (0x00472c60/2f40, 0x004730b0)"    },
    { 0x005c0000u, ".rdata scale consts (1/640 @5cd5a8, 1/480 @5cc560)" },
    { 0x00630000u, ".data boot params (0x00636ae8) + timer (0x0063d558)" },
    { 0x00670000u, ".data race-state array (0x0067ea10)"                },
    { 0x007d0000u, ".data RW device slot (*(0x007d3ff8)) — B15 bridge"  },
    { 0x007f0000u, ".bss big buffer (0x007f0f60) + input (0x007f1044)"  },
    { 0x00890000u, ".bss menu-entry array + vtx buffer (0x00898a20/ac0)"},
};

// B17 — walk the VirtualQuery region map across the whole low-address arena
// (0x00400000..0x00a00000) and log every region's state/type/protect. Called
// at WinMain entry, immediately before InitD3D9, and immediately after, so the
// cold-start lottery (which granules d3d9 / the heap / DLLs claim, and when)
// becomes a deterministic, diffable record instead of guesswork. Also prints a
// compact one-line state of each kNeededGranules entry, prefixed so a harness
// can grep it per phase.
void DumpRegionMap(const char* tag) {
    std::FILE* log = std::fopen(kLogPath, "a");
    if (!log) return;
    constexpr std::uintptr_t kLo = 0x00400000u;
    constexpr std::uintptr_t kHi = 0x00a00000u;
    std::fprintf(log, "\n=== B17 region map [%s] 0x%08X..0x%08X ===\n",
                 tag, static_cast<unsigned>(kLo), static_cast<unsigned>(kHi - 1));
    std::uintptr_t a = kLo;
    std::size_t free_bytes = 0, commit_bytes = 0, reserve_bytes = 0;
    while (a < kHi) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<LPVOID>(a), &mbi, sizeof(mbi)) == 0) break;
        const std::uintptr_t rbase = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::size_t    rsize = mbi.RegionSize;
        const char* state =
            mbi.State == MEM_COMMIT  ? "COMMIT " :
            mbi.State == MEM_RESERVE ? "RESERVE" :
            mbi.State == MEM_FREE    ? "FREE   " : "?      ";
        const char* type =
            mbi.Type  == MEM_IMAGE   ? "IMAGE"   :
            mbi.Type  == MEM_MAPPED  ? "MAPPED"  :
            mbi.Type  == MEM_PRIVATE ? "PRIVATE" : "-";
        // Only log regions that intersect our arena (clip the trailing one).
        if (rbase < kHi) {
            // For MEM_MAPPED regions, resolve the backing file so we know what
            // is squatting in the arena (NO-GUESSING). GetMappedFileName needs
            // a committed address inside the region.
            char fn[256] = "";
            if (mbi.Type == MEM_MAPPED && mbi.State == MEM_COMMIT) {
                char raw[MAX_PATH] = "";
                if (GetMappedFileNameA(GetCurrentProcess(),
                                       reinterpret_cast<LPVOID>(rbase),
                                       raw, MAX_PATH) > 0) {
                    // Keep just the trailing filename to stay readable.
                    const char* slash = std::strrchr(raw, '\\');
                    std::snprintf(fn, sizeof(fn), "  <%s>", slash ? slash + 1 : raw);
                } else {
                    std::snprintf(fn, sizeof(fn), "  <pagefile/anon-section>");
                }
            }
            std::fprintf(log, "  0x%08X +0x%08X  %s %-7s protect=0x%03lX%s\n",
                         static_cast<unsigned>(rbase),
                         static_cast<unsigned>(rsize),
                         state, type,
                         static_cast<unsigned long>(mbi.Protect), fn);
        }
        if (mbi.State == MEM_FREE)         free_bytes    += rsize;
        else if (mbi.State == MEM_COMMIT)  commit_bytes  += rsize;
        else if (mbi.State == MEM_RESERVE) reserve_bytes += rsize;
        // Advance to the next region; guard against zero-size loops.
        std::uintptr_t next = rbase + rsize;
        if (next <= a) next = a + 0x1000u;
        a = next;
    }
    std::fprintf(log, "  --- totals: free=%zuKB commit=%zuKB reserve=%zuKB ---\n",
                 free_bytes / 1024, commit_bytes / 1024, reserve_bytes / 1024);
    // Per-needed-granule one-liners (greppable: "B17-GRAN [tag]").
    for (const auto& g : kNeededGranules) {
        MEMORY_BASIC_INFORMATION mbi{};
        const char* st = "QUERYFAIL";
        if (VirtualQuery(reinterpret_cast<LPVOID>(g.base), &mbi, sizeof(mbi))) {
            st = mbi.State == MEM_COMMIT  ? "COMMIT"  :
                 mbi.State == MEM_RESERVE ? "RESERVE" :
                 mbi.State == MEM_FREE    ? "FREE"    : "?";
        }
        std::fprintf(log, "  B17-GRAN [%s] 0x%08X %-7s  %s\n",
                     tag, static_cast<unsigned>(g.base), st, g.what);
    }
    std::fclose(log);
}

// B7: VirtualAlloc-map the MASHED data-section address range so reimpls
// dereferencing fixed RVAs (e.g. 0x00898ac4 in MenuEntryArrayInit) succeed
// with zeroed memory instead of AV. Writes a log line on success/failure.
// Failure is non-fatal — the title-screen render still runs without the
// wedge; we just can't call most Boot/Frontend/HUD reimpls from the
// standalone yet.
bool MapMashedDataSection() {
    std::FILE* log = std::fopen(kLogPath, "a");
    // Probe each 64KB allocation-granule and find which ones are free vs
    // already mapped. The MASHED .data spans 0x00500000..0x009fffff but the
    // standalone may have DLL imports / loader-placed pages somewhere in
    // that range. Walk granule-by-granule and allocate each free one
    // separately. Any granule that's already mapped is logged but skipped
    // (a subsequent reimpl that derefs into it may still AV — but at least
    // we'll have covered everything we could).
    constexpr std::uintptr_t kGran = 0x10000u; // 64KB allocation granularity
    std::uintptr_t covered = 0, blocked = 0, blocked_first = 0;
    for (std::uintptr_t a = kMashedDataBase;
         a < kMashedDataBase + kMashedDataSize;
         a += kGran) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<LPVOID>(a), &mbi, sizeof(mbi)) == 0) {
            // Query failed — assume the granule is unavailable.
            ++blocked;
            if (!blocked_first) blocked_first = a;
            continue;
        }
        if (mbi.State == MEM_FREE) {
            void* p = VirtualAlloc(
                reinterpret_cast<LPVOID>(a),
                kGran,
                MEM_RESERVE | MEM_COMMIT,
                PAGE_READWRITE);
            if (p) ++covered;
            else { ++blocked; if (!blocked_first) blocked_first = a; }
        } else if (mbi.State == MEM_RESERVE) {
            // Lazy-reserved (typically by process heap). Commit on top of
            // the reservation so reimpls can write there.
            void* p = VirtualAlloc(
                reinterpret_cast<LPVOID>(a),
                kGran,
                MEM_COMMIT,
                PAGE_READWRITE);
            if (p) ++covered;
            else { ++blocked; if (!blocked_first) blocked_first = a; }
        } else {
            // MEM_COMMIT (image / mapped / explicit) — leave alone.
            ++blocked;
            if (!blocked_first) blocked_first = a;
        }
    }
    if (log) {
        std::fprintf(log,
                     "\nMilestone B7: granule walk over 0x%08X..0x%08X — "
                     "%zu granules covered, %zu blocked%s%s\n",
                     static_cast<unsigned>(kMashedDataBase),
                     static_cast<unsigned>(kMashedDataBase + kMashedDataSize - 1),
                     covered, blocked,
                     blocked ? " (first blocked at 0x" : "",
                     blocked ? "" : "");
        if (blocked) {
            std::fprintf(log, "Milestone B7: first blocked granule = 0x%08X\n",
                         static_cast<unsigned>(blocked_first));
        }
    }
    g_mashed_data_mapped = (covered > 0);
    if (log) {
        std::fprintf(log,
                     "Milestone B7: g_mashed_data_mapped = %s\n",
                     g_mashed_data_mapped ? "true (partial)" : "false");
        std::fclose(log);
    }
    return g_mashed_data_mapped;
}

// Forward declarations for reimpls in linked .cpp files. All of these are
// safe to call standalone provided the Phase G wedge covers the MASHED-data
// addresses they touch. Verified bit-identical via Frida diff under the .asi
// path; here they run against our VirtualAlloc'd window instead of MASHED's.
extern "C" void          __cdecl MenuEntryArrayInit();       // Frontend/MenuInit.cpp
extern "C" std::uint32_t __cdecl GameStateFlagGet();         // Boot/GameStateCluster.cpp
extern "C" int           __cdecl StatePhaseIsTwo();          // Boot/GameStateCluster.cpp
extern "C" int           __cdecl RaceEndConstGet();          // Boot/GameStateCluster.cpp
extern "C" void          __cdecl StatePhaseSubSet(std::uint32_t);  // Boot/GameStateCluster.cpp
extern "C" std::uint32_t __cdecl RaceInterruptFlagGet();     // Boot/GameStateCluster.cpp
extern "C" void          __cdecl RaceStateArrayZero();       // Boot/GameStateCluster.cpp
extern "C" void          __cdecl TimerSlotClear();           // Util/TimerInit.cpp
extern "C" void          __cdecl BootDefaultParamsInit();    // Boot/BootLowRvaCluster.cpp
extern "C" void          __cdecl DefaultParam_SetField04();  // Boot/BootLowRvaCluster.cpp
extern "C" void          __cdecl DefaultParam_SetField08();  // Boot/BootLowRvaCluster.cpp
extern "C" void          __cdecl DefaultParam_SetField00();  // Boot/BootLowRvaCluster.cpp
extern "C" int           __cdecl MainLoopInit();             // Boot/FrameDispatch.cpp

// B7: call a single pure-leaf reimpl to verify the data-section wedge works.
// Writes the first and last entry's offset-0 dword via the reimpl, then reads
// them back and confirms they're zero (the reimpl is a zero-fill). If the
// wedge is inactive or the reimpl crashes, the log line is missing.
void VerifyDataSectionWedgeViaReimpl() {
    std::FILE* prelog = std::fopen(kLogPath, "a");
    if (prelog) {
        std::fprintf(prelog,
                     "\nMilestone B7: wedge status = %s\n",
                     g_mashed_data_mapped ? "ACTIVE" : "INACTIVE (VirtualAlloc failed)");
        std::fclose(prelog);
    }
    if (!g_mashed_data_mapped) return;
    // Probe the MenuEntryArrayInit working range BEFORE poisoning to detect
    // blocked granules. If any 64KB granule in 0x00898000..0x008990df is
    // missing, the reimpl will AV. Log the per-granule state so a coverage
    // failure is visible.
    constexpr std::uintptr_t kArrayBase = 0x00898ac0u;
    constexpr std::uintptr_t kArrayEnd  = 0x008990dcu;
    std::FILE* clog = std::fopen(kLogPath, "a");
    if (clog) {
        std::fprintf(clog, "Milestone B7: checking MenuEntryArrayInit range "
                           "0x%08X..0x%08X granule coverage:\n",
                     static_cast<unsigned>(kArrayBase),
                     static_cast<unsigned>(kArrayEnd));
        // 64KB granule boundaries: 0x00890000, 0x008a0000.
        for (std::uintptr_t g = (kArrayBase & ~0xFFFFu);
             g <= ((kArrayEnd - 1) & ~0xFFFFu);
             g += 0x10000) {
            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery(reinterpret_cast<LPVOID>(g), &mbi, sizeof(mbi))) {
                const char* state =
                    mbi.State == MEM_COMMIT  ? "COMMIT"  :
                    mbi.State == MEM_RESERVE ? "RESERVE" :
                    mbi.State == MEM_FREE    ? "FREE"    : "?";
                const char* type =
                    mbi.Type  == MEM_IMAGE   ? "IMAGE"   :
                    mbi.Type  == MEM_MAPPED  ? "MAPPED"  :
                    mbi.Type  == MEM_PRIVATE ? "PRIVATE": "-";
                std::fprintf(clog, "  granule 0x%08X: %s/%s (protect=0x%lX)\n",
                             static_cast<unsigned>(g), state, type,
                             static_cast<unsigned long>(mbi.Protect));
            } else {
                std::fprintf(clog, "  granule 0x%08X: VirtualQuery failed\n",
                             static_cast<unsigned>(g));
            }
        }
        std::fclose(clog);
    }
    // Pre-poison the range the reimpl is supposed to zero so we can confirm
    // the writes actually happened. Both the poison write and the reimpl
    // are SEH-wrapped: if a granule in the range is blocked, the AV is
    // caught and the process survives. This used to be fatal back when we
    // committed every MEM_RESERVE granule blindly (which corrupted d3d9's
    // lazy-reserved memory). The current wedge only commits MEM_FREE
    // granules, so we accept that some reimpls will AV here.
    __try {
        for (std::uintptr_t a = kArrayBase; a < kArrayEnd; a += 4) {
            *reinterpret_cast<std::uint32_t*>(a) = 0xDEADBEEFu;
        }
        MenuEntryArrayInit();
        const std::uint32_t v0   = *reinterpret_cast<std::uint32_t*>(0x00898ac0u);
        const std::uint32_t v40  = *reinterpret_cast<std::uint32_t*>(0x00898ae8u);
        const std::uint32_t v29  = *reinterpret_cast<std::uint32_t*>(0x008990acu);
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                         "Milestone B7: MenuEntryArrayInit() returned without AV.\n"
                         "  entry 0 [0x00898ac0] = 0x%08X (expect 0)\n"
                         "  entry 0 [0x00898ae8] = 0x%08X (expect 0)\n"
                         "  entry 29[0x008990ac] = 0x%08X (expect 0)\n",
                         v0, v40, v29);
            std::fclose(log);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                         "Milestone B7: MenuEntryArrayInit() AV'd (target "
                         "granule 0x00890000 not in wedge — see coverage above).\n");
            std::fclose(log);
        }
    }
}

// B10 (Phase G): execute MASHED's frontend-boot data-init sequence using the
// VirtualAlloc'd wedge. Pure-leaf reimpls that only touch MASHED globals are
// safe; tunnel reimpls (functions whose body just JMPs to MASHED's RVA) are
// either skipped or inlined here with a standalone-safe replacement.
//
// Sequence per re/analysis/skeleton_call_tree.md WinMainEntry path:
//   1. DataZeroFill_StandaloneInline — zero 0xdce9 DWORDs at 0x007f0f60.
//      The packaged reimpl is a tunnel that calls MASHED 0x004924f0; we
//      inline the memset here so the standalone doesn't need to jump
//      into MASHED's code section.
//   2. BootDefaultParamsInit — writes 5 floats to the 0x00636ae8 cluster.
//   3. DefaultParam_SetField00/04/08 — writes 0.7f to 0x007f0f00/04/08.
//   4. RaceStateArrayZero — zeroes 12 DWORDs at 0x0067ea10.
//   5. MenuEntryArrayInit — zeroes 30 entries × 52 bytes at 0x00898ac0.
//   6. TimerSlotClear — zeroes 0x0063d558.
//   7. MainLoopInit — sets game-loop exit flag, game state, frame counters.
//
// Each step writes diagnostics to mashed_re.log. The chain halts on any
// AV (process dies) — log shows last good step.
void ExecuteFrontendBootChain() {
    std::FILE* log = std::fopen(kLogPath, "a");
    if (!log) return;
    std::fprintf(log, "\nMilestone B10: frontend boot chain start\n");
    std::fflush(log);

    // Step 1: DataZeroFill replacement. Original at 0x004924f0 zero-fills
    // 0xdce9 dwords (= ~220 KB) at 0x007f0f60. Reimpl in SubsystemInit.cpp
    // is a tunnel; for standalone we do the memset directly.
    constexpr std::uintptr_t kBigBufBase = 0x007f0f60u;
    constexpr std::size_t    kBigBufDw   = 0xdce9u;        // 56553 dwords
    constexpr std::size_t    kBigBufSz   = kBigBufDw * 4u; // 226212 bytes
    // The 226 KB span (0x007f0f60..+0x373a4) crosses 4 granules; the wedge maps
    // them only partially on most cold-starts (d3d9 placement variance — e.g.
    // 35/80 granules this run). The earlier single-granule VirtualQuery
    // pre-check passed when 0x007f0000 was committed but a LATER granule was
    // blocked, so the memset walked into unmapped memory and AV'd. SEH does not
    // reliably catch hardware faults under /EHsc, so don't rely on it: walk the
    // span granule-by-granule and zero ONLY committed, accessible granules. This
    // cannot AV regardless of EH model.
    {
        // B17: PAGE-granular (4KB), not 64KB-granule. The earlier granule-aware
        // version VirtualQuery'd the granule BASE but memset the whole sub-span —
        // which AV'd when a foreign sub-granule mapping (a READONLY NLS section
        // view spilling into the granule) poisoned a page mid-span (the B17 region
        // maps proved this; it was a remaining ~20% cold-start crash site). Query
        // and zero each 4KB page independently so a poisoned page is skipped, not
        // walked into. SEH-wrapped as belt-and-suspenders.
        std::size_t zeroed = 0, skipped = 0;
        const std::uintptr_t spanEnd = kBigBufBase + kBigBufSz;
        __try {
            for (std::uintptr_t p = kBigBufBase; p < spanEnd; p += 0x1000u) {
                const std::uintptr_t pe = (p + 0x1000u < spanEnd) ? (p + 0x1000u) : spanEnd;
                MEMORY_BASIC_INFORMATION gm{};
                const bool ok =
                    VirtualQuery(reinterpret_cast<LPVOID>(p), &gm, sizeof(gm)) != 0 &&
                    gm.State == MEM_COMMIT &&
                    (gm.Protect & (PAGE_READWRITE | PAGE_WRITECOPY |
                                   PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0 &&
                    (gm.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0;
                if (ok) { std::memset(reinterpret_cast<void*>(p), 0, pe - p); zeroed += pe - p; }
                else    { skipped += pe - p; }
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            // A page flipped state between query and memset — extremely unlikely,
            // but keep the chain alive.
        }
        std::fprintf(log, "  [1] DataZeroFill (page-granular): zeroed %zu, skipped %zu bytes\n",
                     zeroed, skipped);
    }
    std::fflush(log);

    // Step 2: BootDefaultParamsInit. Writes 5 floats at 0x636ae8 cluster.
    // Probe the target granule before the call so a failure here shows the
    // wedge coverage rather than just AV'ing silently.
    {
        MEMORY_BASIC_INFORMATION mbi2{};
        if (VirtualQuery(reinterpret_cast<LPVOID>(0x00630000u), &mbi2, sizeof(mbi2))) {
            std::fprintf(log, "  [2-pre] 0x00630000 state=0x%08lX protect=0x%08lX\n",
                         static_cast<unsigned long>(mbi2.State),
                         static_cast<unsigned long>(mbi2.Protect));
            std::fflush(log);
        }
    }
    __try {
        BootDefaultParamsInit();
        std::fprintf(log, "  [2] BootDefaultParamsInit OK\n");
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [2] BootDefaultParamsInit AV'd (target 0x00636ae8 not in wedge)\n");
    }
    std::fflush(log);

    // Step 3: DefaultParam_SetField00/04/08. Each writes 0.7f at 0x007f0f0X.
    __try {
        DefaultParam_SetField00();
        DefaultParam_SetField04();
        DefaultParam_SetField08();
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [3] DefaultParam_SetField00/04/08 AV'd\n");
        std::fflush(log);
    }
    std::fprintf(log, "  [3] DefaultParam_SetField00/04/08 OK; readback "
                 "0x007f0f00=0x%08X 04=0x%08X 08=0x%08X (expect 0x3f333333)\n",
                 *reinterpret_cast<std::uint32_t*>(0x007f0f00u),
                 *reinterpret_cast<std::uint32_t*>(0x007f0f04u),
                 *reinterpret_cast<std::uint32_t*>(0x007f0f08u));
    std::fflush(log);

    // Step 4: RaceStateArrayZero. Zeroes 12 dwords at 0x0067ea10.
    __try {
        RaceStateArrayZero();
        std::fprintf(log, "  [4] RaceStateArrayZero OK; readback "
                     "0x0067ea10=0x%08X 0x0067ea14=0x%08X (expect 0)\n",
                     *reinterpret_cast<std::uint32_t*>(0x0067ea10u),
                     *reinterpret_cast<std::uint32_t*>(0x0067ea14u));
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [4] RaceStateArrayZero AV'd (target 0x0067ea10 not in wedge)\n");
    }
    std::fflush(log);

    // Step 5: MenuEntryArrayInit — already verified by B7; call it again
    // here as part of the canonical boot order.
    __try {
        MenuEntryArrayInit();
        std::fprintf(log, "  [5] MenuEntryArrayInit OK; readback "
                     "0x00898ac0=0x%08X (expect 0)\n",
                     *reinterpret_cast<std::uint32_t*>(0x00898ac0u));
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [5] MenuEntryArrayInit AV'd\n");
    }
    std::fflush(log);

    // Step 6: TimerSlotClear. Zeroes 0x0063d558.
    __try {
        TimerSlotClear();
        std::fprintf(log, "  [6] TimerSlotClear OK; readback "
                     "0x0063d558=0x%08X (expect 0)\n",
                     *reinterpret_cast<std::uint32_t*>(0x0063d558u));
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [6] TimerSlotClear AV'd (target 0x0063d558 not in wedge)\n");
    }
    std::fflush(log);

    // Step 7: MainLoopInit. Sets game-loop exit flag, state=1, frame ctrs.
    // Internally calls FUN_00495110, FUN_0042b920, FUN_00433240, etc. via
    // raw RVA. Those land in our wedge but contain zero bytes — interpreted
    // as ADD [EAX],AL (also 00) which usually crashes. Wrap in SEH to keep
    // the chain alive even if MainLoopInit hits a callee tunnel.
    __try {
        const int r = MainLoopInit();
        std::fprintf(log, "  [7] MainLoopInit returned %d\n", r);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(log, "  [7] MainLoopInit AV'd (expected: tunnels into "
                     "MASHED code section via FUN_00495110 etc.)\n");
    }

    std::fprintf(log, "Milestone B10: frontend boot chain complete\n");
    std::fclose(log);
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Truncate the log at session start so subsequent fopen "a" calls
    // build a clean per-run trace.
    {
        std::FILE* clr = std::fopen(kLogPath, "w");
        if (clr) std::fclose(clr);
    }

    // B17: snapshot the low-address arena before ANYTHING runs (no piz heap,
    // no window, no d3d9). This is the pristine layout we get to reserve from.
    DumpRegionMap("winmain-entry");
    {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                "B17-TLS: reserved %ld/%ld needed granules at TLS time "
                "(seen-free; the rest were already occupied by kernel-early NLS)\n",
                g_b17_tls_reserved, g_b17_tls_seen_free);
            std::fclose(log);
        }
    }

    // Try to load the frontend asset archive before opening the window so the
    // initial title reflects load state. Failure is non-fatal.
    if (LoadFrontendPiz()) {
        // On success, also extract TEXTURES.TXD and walk its RWS chunk tree.
        // Both write to mashed_re.log; ExtractAndWalkTexturesTxd refines the
        // window title.
        if (ExtractAndWalkTexturesTxd()) {
            // B3: decode the TXD into a flat Texture array and log per-
            // texture metadata. Refines the window title with the decoded
            // texture count.
            (void)DecodeAndDumpTexturesTxd();
        }
    }

    // Wedge (B7) and boot chain (B10) are deferred until AFTER InitD3D9 —
    // see the post-InitD3D9 calls. d3d9 needs unobstructed low-address
    // space to settle CreateDevice, and running our 4-MB wedge before it
    // caused 60%+ cold-start failures.

    // Register the window class. Pattern from 0x00499ba0 (Window_Create).
    WNDCLASSEXA wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // we own the backbuffer; don't let GDI clear under us
    wc.lpszClassName = kClassName;
    if (!RegisterClassExA(&wc)) {
        return 1;
    }

    RECT rect = {0, 0, kWidth, kHeight};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowExA(
        0,
        kClassName,
        g_windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, nullptr,
        hInstance,
        nullptr);
    if (!g_hwnd) {
        UnregisterClassA(kClassName, hInstance);
        return 2;
    }

    // Show + paint. Pattern from 0x004996f0 (WindowShow).
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    // B17: snapshot the arena right before CreateDevice so we can diff what
    // d3d9 claims against what was free here.
    DumpRegionMap("pre-initd3d9");

    if (!InitD3D9()) {
        DestroyWindow(g_hwnd);
        UnregisterClassA(kClassName, hInstance);
        return 3;
    }

    // B17: snapshot again immediately after CreateDevice. The diff vs
    // pre-initd3d9 is d3d9's real low-address footprint — the granules we must
    // NOT pre-reserve, and the ones we may.
    DumpRegionMap("post-initd3d9");

    // B7 (deferred to post-InitD3D9): wedge over whatever low-address
    // granules d3d9 didn't claim. Then verify + run boot chain.
    (void)MapMashedDataSection();
    VerifyDataSectionWedgeViaReimpl();
    ExecuteFrontendBootChain();

    // B11: DirectInput8 keyboard. Failure is non-fatal — render loop still
    // works; the window title just won't reflect key state.
    (void)InitDirectInput(hInstance);

    // B14: one-time wedge probe — write a test byte to MASHED's input
    // active byte (0x007f1044) and read it back. If the wedge has the
    // 0x007f0000 granule committed, the readback will match; otherwise
    // the write AVs and SEH catches it. Either way, log the outcome.
    {
        std::FILE* log = std::fopen(kLogPath, "a");
        bool ok = false;
        std::uint8_t readback = 0;
        __try {
            *reinterpret_cast<volatile std::uint8_t*>(0x007f1044u) = 0xA5u;
            readback = *reinterpret_cast<volatile std::uint8_t*>(0x007f1044u);
            *reinterpret_cast<volatile std::uint8_t*>(0x007f1044u) = 0u;
            ok = (readback == 0xA5u);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            ok = false;
        }
        if (log) {
            std::fprintf(log,
                         "\nMilestone B14: MASHED input-globals wedge probe = %s "
                         "(wrote 0xA5 to 0x007f1044, readback=0x%02X)\n",
                         ok ? "OK" : "FAIL",
                         readback);
            std::fclose(log);
        }
    }

    // B5/B6: now that D3D9 is up and the TXD is decoded, upload textures into
    // QuadRenderer slot(s). B6 default uploads just 'main' to slot 0 for the
    // title-screen MVP; B5 fallback uploads all 8 textures into a 4x2 atlas
    // when kTitleMode is false. Failure is non-fatal — the frame loop still
    // runs and shows a teal clear, just without quads on top.
    // B13: always upload all textures so PgUp/PgDn can cycle through them in
    // title mode. The B5 atlas-grid layout vs. B6 title-screen layout is a
    // render-time choice (still controlled by kTitleMode), not an upload-time
    // one. UploadTitleTextureForLogo() is retained for the comment + slot-0
    // contract but UploadAllTexturesForAtlas() supersedes it.
    (void)UploadAllTexturesForAtlas();

    // B15: install the RW Im2D -> D3D9 bridge. Points *(0x007d3ff8) at a fake RW
    // device so the frontend's C3 Im2D draw reimpls (HudIm2DQuad, ChromeBaseDraw,
    // ...) render through our D3D9 device instead of AV'ing on a null RW device.
    // Requires the Phase-G wedge to cover the 0x007d0000 granule; logs the result.
    {
        g_bridge_installed = mashed_re::D3d9Render::RwIm2DBridge_Install(g_device);
        if (g_bridge_installed && g_atlas_slot_count > 0) {
            // Register slot 0's uploaded texture under the demo handle so the
            // textured bridge draw can bind it.
            mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                kBridgeTitleHandle, g_quad_renderer.slot_texture(0));
        }
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                         "\nMilestone B15: RW Im2D->D3D9 bridge install = %s "
                         "(fake RW device at *(0x007d3ff8); HudIm2DQuad routes to D3D9)\n",
                         g_bridge_installed ? "OK" : "FAILED (wedge gap at 0x007d0000)");
            std::fclose(log);
        }
    }

    // B16: install standalone RVA thunks so MASHED's real MenuChromeShellA
    // (0x0042e3a0) can run here. It calls the C3 Im2D draws + screen getters by
    // absolute MASHED RVA — unmapped in the standalone — so we map the .text
    // granules and write JMP thunks to our reimpls/stubs, plus restore the
    // .rdata scale constants (1/640, 1/480) the scaled draws read directly.
    {
        using namespace mashed_re::Compat;
        const bool map_text  = StandaloneThunks_MapRange(0x00420000u, 0x60000u); // 0x00420000..0x0047ffff
        const bool map_scale = StandaloneThunks_MapRange(0x005c0000u, 0x10000u);
        bool scale_ok = false;
        if (map_scale) {
            __try {
                *reinterpret_cast<volatile std::uint32_t*>(0x005cd5a8u) = 0x3ACCCCCDu; // 1/640
                *reinterpret_cast<volatile std::uint32_t*>(0x005cc560u) = 0x3B088889u; // 1/480
                scale_ok = true;
            } __except(EXCEPTION_EXECUTE_HANDLER) { scale_ok = false; }
        }
        const bool t1 = StandaloneThunks_Install(0x0042b8b0u, reinterpret_cast<const void*>(&Standalone_ScreenWidth));
        const bool t2 = StandaloneThunks_Install(0x0042b8c0u, reinterpret_cast<const void*>(&Standalone_ScreenHeight));
        const bool t3 = StandaloneThunks_Install(0x0042d5a0u, reinterpret_cast<const void*>(&Standalone_CreditsNoOp));
        const bool t4 = StandaloneThunks_Install(0x00472f40u, reinterpret_cast<const void*>(&TextGradientV0V1Override));
        const bool t5 = StandaloneThunks_Install(0x004730b0u, reinterpret_cast<const void*>(&TextGradientV2V3Override));
        const bool t6 = StandaloneThunks_Install(0x00472c60u, reinterpret_cast<const void*>(&ChromeBaseDraw));
        const bool thunks_ok = t1 && t2 && t3 && t4 && t5 && t6;
        g_b16_chrome_ready = g_bridge_installed && map_text && scale_ok && thunks_ok;
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                "\nMilestone B16: standalone RVA thunks — map_text=%d map_scale=%d scale_ok=%d "
                "thunks=%d/6 (b8b0=%d b8c0=%d d5a0=%d f40=%d 30b0=%d 2c60=%d)\n"
                "Milestone B16: MenuChromeShellA ready = %s\n",
                map_text, map_scale, scale_ok, static_cast<int>(StandaloneThunks_Count()),
                t1, t2, t3, t4, t5, t6,
                g_b16_chrome_ready ? "YES (real MASHED menu chrome via the B15 bridge)"
                                   : "NO (missing dependency — see flags above)");
            std::fclose(log);
        }
    }

    // B17: single greppable summary line. The harness keys boot-success on the
    // presence of this line (the process reached the render loop without an
    // uncaught AV) and chrome-readiness on the YES/NO field.
    {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log,
                "\nB17-SUMMARY reached-main-loop chrome=%s thunks=%u/6\n",
                g_b16_chrome_ready ? "YES" : "NO",
                mashed_re::Compat::StandaloneThunks_Count());
            std::fclose(log);
        }
    }

    // Main loop: pump messages + render until the user closes the window or
    // hits ESC, or the device gets into an unrecoverable state. Per frame we
    // also poll the keyboard (B11) and refresh the window title to reflect
    // current key state.
    while (!PumpOnce()) {
        PollKeyboard();
        UpdateTitleFromKeyboard();
        // B12: arrow keys translate title quad; Enter resets.
        UpdateQuadFromKeyboard();
        // B13: PgUp/PgDn cycle through uploaded atlas slots; Home -> 0.
        UpdateTitleSlotFromKeyboard();
        // B14: mirror DInput keyboard state into MASHED's input globals
        // (0x007f1044 active / 0x007f1504 processed, player 0). No frontend
        // code currently reads these in standalone, but the wiring exists
        // for B16's frontend_mode_dispatch loop.
        WriteMashedInputGlobalsFromKeyboard();
        // B11: ESC via DirectInput also exits the app (in addition to the
        // WM_KEYDOWN handler).
        if ((g_keys[DIK_ESCAPE] & 0x80) && !(g_keys_prev[DIK_ESCAPE] & 0x80)) {
            PostQuitMessage(0);
        }
        if (!RenderFrame()) {
            // Unrecoverable device loss; bail out.
            break;
        }
    }

    // Teardown.
    ShutdownDirectInput();
    ShutdownD3D9();
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    UnregisterClassA(kClassName, hInstance);

    return 0;
}
