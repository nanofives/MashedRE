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
#include <cstdlib>      // B19a: std::free for WIC-decoded BGRA buffers
#include <cmath>        // R5: drive-demo steering waveform
#include <cwchar>       // R6: swprintf for the HUD result banner
#include <cstdio>
#include <cstring>
#include <climits>

#include "Piz/PizReader.h"
#include "Rws/RwsChunkWalker.h"
#include "Txd/TxdDecoder.h"
#include "D3d9Render/QuadRenderer.h"
#include "D3d9Render/TrackRenderer.h"
#include "D3d9Render/RwIm2DBridge.h"        // B15: RW Im2D -> D3D9 bridge
#include "D3d9Render/PngLoader.h"           // B19a: WIC PNG decode (bg/logo assets)
#include "D3d9Render/MpegVideoTexture.h"    // F1: frontend.mpg backdrop (DirectShow)
#include "D3d9Render/TextRenderer.h"        // B19b: GDI text -> BGRA (menu item strings, fallback)
#include "D3d9Render/MashedFont.h"          // B19: faithful FGDC20 glyph font
#include "D3d9Render/MenuStringTable.h"     // menu id->glyph-string (sprite-by-id)
#include "D3d9Render/DrawStreamDump.h"      // parity harness: MASHED_DBG_DRAWSTREAM
#include "Compat/StandaloneRvaThunks.h"     // B16: standalone RVA-thunk installer
#include "Frontend/MenuNavSM.h"             // standalone menu nav state machine (FUN_0043d2a0 port)

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
// Per-corner-color variant (TL/TR/BL/BR) for the chrome gradient quads
// recorded in log/menu_draw_dump.json (2026-06-12 ground truth).
extern "C" void __cdecl HudIm2DQuadCorners(
    std::int32_t tex_handle, float x, float y, float w, float h,
    std::uint32_t argb_tl, std::uint32_t argb_tr,
    std::uint32_t argb_bl, std::uint32_t argb_br, std::uint32_t* uv);

// B16 — MASHED's real per-frame menu-chrome drawer (0x0042e3a0,
// Frontend/MenuSpriteDispatch.cpp). It draws two gradient bands + two white
// divider lines by calling the C3 Im2D draws *by MASHED RVA*
// (TextGradientV0V1Override @0x00472f40, TextGradientV2V3Override @0x004730b0,
// ChromeBaseDraw @0x00472c60) and the screen-dim getters @0x0042b8b0/c0. Those
// RVAs are redirected to our standalone reimpls/stubs by the B16 thunk install
// below, so calling MenuChromeShellA() renders the actual menu chrome via the
// B15 bridge. The three Im2D draws are also declared so we can thunk to them.
extern "C" void __cdecl MenuChromeShellA(void);
// R2-4 animated logo overlay (verbatim FUN_00473ee0 port; Frontend/
// DrawQuadPrimitives.cpp). Coords 640x480-virtual; vscale folds the original's
// in-helper ScreenW/640, ScreenH/480 multiply.
extern "C" void __cdecl LogoOverlayFadeSet(int target, int cur);
extern "C" void __cdecl LogoOverlayDraw(float slide_x, float wave_t,
                                        float vscale_x, float vscale_y);
extern "C" void __cdecl ChromeBaseDraw(float x, float y, float w, float h, std::uint32_t argb);
extern "C" void __cdecl TextGradientV0V1Override(float x, float y, float w, float h, std::uint32_t argb);
extern "C" void __cdecl TextGradientV2V3Override(float x, float y, float w, float h, std::uint32_t argb);

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "psapi.lib")

// ===========================================================================
// B17 — image-pad: own the low MASHED arena via the exe IMAGE itself.
//
// Proven root cause of the cold-start lottery: at /BASE:0x10000000 the legacy
// 0x00400000..0x009fffff range is vacant, so Windows fills it bottom-up (before
// any user code) with NLS section mappings + the process heap + d3d9's heap, at
// per-run-varying addresses. The reimpls' hardcoded MASHED RVAs then land in
// foreign/READONLY memory ~half the time, gating boot and the faithful menu path.
//
// Deterministic fix: rebase the exe to /BASE:0x10000 and emit this ~10MB zero-
// init array. It makes the exe image's VA span cover the entire MASHED address
// arena (~0x00401000..0x009fffff). Every hardcoded MASHED RVA then resolves to
// OUR image's committed, writable, zero-filled memory — present from load,
// immune to NLS/heap/d3d9 squatting (which are forced above the image). This
// recreates MASHED's own low-memory layout (a low-based image owning the arena),
// which is exactly the environment d3d9's CreateDevice is built for.
//
// The Phase-G wedge + B16 RVA thunks still run, but now they re-protect already-
// committed image pages and write JMP thunks into our .bss — guaranteed to
// succeed. External linkage + the WinMain reference keep /OPT:REF from dropping
// the array. On-disk size is unaffected (.bss has no raw data).
// ===========================================================================
extern "C" char g_b17_low_arena_pad[0x00A00000];
char            g_b17_low_arena_pad[0x00A00000];

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
bool               g_depth_disabled = false;  // set if device fell back to no auto-depth

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

// B19a — real menu background + logo (PNG assets decoded via WIC). The frontend's
// bg is Perm.piz/BACKGROUND.PNG; the logo is Font36.piz/MASHEDLOGO.PNG. They're
// uploaded into QuadRenderer slots above the 8 atlas slots and registered with
// the bridge under their own handles so HudIm2DQuad draws them through D3D9.
constexpr std::uint32_t kSlotMenuBg     = 8;
constexpr std::uint32_t kSlotMenuLogo   = 9;
constexpr int           kHandleMenuBg   = 2;   // 1 = title texture (B15 demo)
constexpr int           kHandleMenuLogo = 3;

// F2: track-preview crossfade (MenuChromeShellB 0x0042e5b0, verified reimpl):
// 24 named previews (slot table 0x005f79d8) from SFX.piz/TRACKIMAGES.TXD.
// Slots 20..43, bridge handles 10..33.
constexpr std::uint32_t kSlotPreview0   = 20;
constexpr int           kHandlePreview0 = 10;
bool g_previews_ready = false;
int  g_chrome_spriteA = 0;   // DAT_0067e848 analogue
int  g_chrome_spriteB = 1;   // DAT_0067f0b8 analogue (forced odd)
unsigned g_chrome_frame = 0; // FrameCounter analogue
bool             g_menu_bg_ready   = false;
mashed_re::D3d9Render::MpegVideoTexture g_menu_video;  // F1 video backdrop
bool             g_menu_logo_ready = false;
// F6 (frontend-faithful): the frontend PHASE (DAT_0067eca4 analogue, the
// FUN_0043c5b0 draw ladder gate): 1 = title (logo only, no items), 2..3 =
// menu screens (items draw). Title-confirm pushes screen 1 (the original's
// title -> main-menu flow).
// Phase 0 = boot legal/copyright splash (#2; FUN_004288a0 over FUN_00428a30,
// ~8s auto-advance or any key). 1 = title. 2..3 = menu screens.
int              g_frontend_phase  = 0;
DWORD            g_splash_start_ms  = 0;   // set on first splash frame
// #8/#18/#19 confirm-dialog modal (FUN_00433f40). Active when step != 0; alpha
// fades 0->0xff (DAT_0067eab8). Title 0x41 'MASHED', body = a USA.DAT(piz) id,
// buttons Yes/No or Continue. Multi-step flow (real text from Font36.piz/USA.DAT):
//   Load Game: 10 warn 0x215 (Yes/No) -> 11 'Load Successful.' 0x1bc (Continue)
//   Save Game: 20 overwrite? 0x1ca (Yes/No) -> 21 'Saving game data.' 0x233
//              (auto ~0.8s) -> 22 'Save successful.' 0x279 (Continue)
int              g_modal_body_id    = 0;
int              g_modal_button_id  = 0;
int              g_modal_alpha      = 0;
int              g_modal_step       = 0;     // 0=none; 10/11 load; 20/21/22 save
bool             g_modal_yesno      = false; // true=Yes/No, false=single Continue
DWORD            g_modal_timer_ms   = 0;     // auto-advance timer for the saving step
std::uint32_t    g_menu_logo_w     = 0;
std::uint32_t    g_menu_logo_h     = 0;

// B19b — real menu item text. The language .DAT (Font36.piz/USA.DAT) is loaded
// into g_msg_dat and kept for the program lifetime; a message id resolves to a
// [u16 len][UTF-16LE text] record via the offset table (faithful to MASHED's
// FUN_00427780: str = base + *(u32*)(base + id*4)). Each main-menu item string
// is GDI-rasterized to a texture (slots 10+) and drawn through the bridge.
std::uint8_t*    g_msg_dat       = nullptr;
std::uint32_t    g_msg_dat_len   = 0;
constexpr std::uint32_t kSlotMenuItem0 = 10;   // menu item textures: slots 10..14
constexpr int           kHandleMenuItem0 = 4;  // handles 4..8
constexpr std::uint32_t kMenuItemFontPx  = 30;
struct MenuItemTex { int handle; std::uint32_t w, h; bool ready; };
MenuItemTex      g_menu_items[8] = {};
std::uint32_t    g_menu_item_count = 0;
std::uint32_t    g_menu_selected   = 0;   // B19c: keyboard-driven selection index

// B19 faithful font — MASHED's actual FGDC20 glyphs (Font36.piz). When loaded,
// the menu items are drawn glyph-by-glyph in this font instead of the GDI/Arial
// fallback textures. The decoded menu-item strings are kept for per-frame draw.
mashed_re::D3d9Render::MashedFont g_font;
// Menu id->glyph-string table (the menu records' "sprite-atlas-by-id"). Faithful
// to FUN_00427780 (id->entry) + FUN_004277a0 (control-char->special-glyph remap);
// loaded from the language .DAT (FUN_004274e0 streams <lang>.dat). The draw-loop
// port resolves each record's primary/secondary id through this, then draws the
// resulting string via DrawMashedString (FGDC20).
// RE: re/analysis/standalone_menu_sm/sprite_atlas_by_id_spec.md
mashed_re::D3d9Render::MenuStringTable g_menu_str;
constexpr std::uint32_t kSlotMenuFont   = 15;
constexpr int           kHandleMenuFont = 9;

// R2-5 — badges.txd named-sprite chrome. MASHED's boot loader (FUN_0040bbb0,
// 0x0040bbb0) opens sfx.piz and loads "badges.txd" into the dictionary at
// DAT_0063b8fc; SpriteLookupC (FUN_0040bb50, 0x0040bb50 = FUN_004c5c00(dict,
// name)) resolves named textures from it ("Button"/"Arrow"/"SemiC"...). The
// menu draw loop's highlight branch looks up "Button" (string @0x005cda7c)
// and submits it via FUN_004739f0 with width 13.0 (0x41500000 @0x0043cbaa)
// at the bar's left edge (X bias 13.0 = _DAT_005cd8d8). The standalone
// decodes BADGES.TXD with Txd::Dictionary (same chunk-0x23/ver 0x1803ffff
// container as FGDC20.TXD) and uploads "Button" (16x32 PAL8).
constexpr std::uint32_t kSlotMenuBadge   = 16;
// Item 13 fix (2026-06-12): handle 10 COLLIDED with kHandlePreview0 (the 24
// track previews register bridge handles 10..33), so the badge's handle-10
// registration was clobbered by Training1 -> the selected-row cap drew a
// preview texture / nothing instead of the navy 'Button' semi-circle. Move it
// clear of the preview range (10..33).
constexpr int           kHandleMenuBadge = 34;
bool             g_menu_badge_ready = false;
// #17 (user review): the slider/toggle side arrows are the BADGES.TXD "Arrow"
// sprite (16x16), NOT the '<'/'>' text glyphs the standalone was drawing. Loaded
// from sfx.piz/BADGES.TXD alongside "Button"; left arrow = U-flipped, right =
// normal (FUN_004368e0/sound-screen draw uses a 16x16 sprite at x=356/484).
constexpr std::uint32_t kSlotMenuArrow   = 55;
constexpr int           kHandleMenuArrow = 46;
bool             g_menu_arrow_ready = false;

// --- #25 Player Color Select (nav screen 4 = kT4, title id 0x130) ------------
// RE map (pool0 decomp 2026-06-12): the screen-content body FUN_004368e0 draws
// per-active-player car-color previews when FUN_00430760()==0 (game-mode global
// DAT_0067e9fc NOT in {2,3,4,5,10}). Each preview = FUN_0042fab0(carIdx) which
// maps idx 0..9 -> a named sprite, then FUN_004739f0(sprite,x,y,w,h,argb,...,
// 1,flip) where (x,y)=top-left, (w,h)=size in 640x480 VIRTUAL units (param_11==1
// multiplies by screenDim*1/640 / 1/480 = our kVScale 1.25x). The sprite names
// (FUN_0042fab0 cases, MASHED 0x0042fab0 jump table) and the "vs" separator
// (&DAT_005cda30) are textures in SFX.piz/INTERFACE.TXD (the global frontend
// sprite dict DAT_0063b904 that FUN_0040bb90/FUN_004c5c00 resolve).
//   2-player layout (DAT_0067ea64==0, DAT_0067ea7c==0, the canonical case):
//     P1 car  @ virtual (40,292) 112x112   (FUN_004368e0 LAB_004375d5)
//     P2 car  @ virtual (196,292) 112x112
//     "vs"    @ virtual (132,300) 88x88     (drawn last, on top, between cars)
// carIdx -> INTERFACE.TXD texture name (0x0042fab0 case order):
static const char* kCarColorNames[10] = {
    "NFLRed", "NFLBluejay", "NFLMelon", "NFLGold", "NFLPink",
    "NFLShadow", "NFLCopter", "NFLBomb", "NFLFugitive", "NFLSurvival"};
constexpr std::uint32_t kSlotCar0     = 44;   // slots 44..53 (10 cars)
constexpr int           kHandleCar0   = 35;   // bridge handles 35..44
constexpr std::uint32_t kSlotVs       = 54;   // "vs" separator
constexpr int           kHandleVs     = 45;
bool             g_carsel_ready = false;
// Per-player chosen car index. The standalone has no multiplayer setup flow yet,
// so these default to a representative 2-player config (Red vs Bluejay); LEFT/
// RIGHT on screen 4 cycles player 1's car. [residual: the per-player selection
// cursor is FUN_00431b80 + the full setup flow that writes DAT_007f1a14..4c.]
int              g_csel_p1_car = 0;
int              g_csel_p2_car = 1;

// R4 opener — track fly-through mode (env MASHED_TRACK_VIEW=<piz path or 1
// for Arctic>). Renders the cracked RW world (Track/TrackWorld) through the
// spike renderer (D3d9Render/TrackRenderer) with an auto-orbit camera.
bool                                  g_track_view = false;
mashed_re::D3d9Render::TrackRenderer  g_track;
constexpr float         kMenuTextHeight = 30.f;   // on-screen glyph height (px)
wchar_t          g_menu_msgs[8][64] = {};

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
        // ESC is no longer a hard quit here: the nav state machine consumes ESC
        // as a pop-back (and only quits via the DInput path when at the root).
        // Leaving a WM_KEYDOWN quit would defeat the pop-back demo.
        break;

    default:
        break;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

extern bool g_nav_demo; // defined below; gates the background-park behavior.

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
    }
    // Item 1 (user review): the menu must NOT freeze when the window loses
    // focus. The old WaitMessage() park blocked the whole render loop until a
    // message arrived, freezing the video/animations until re-focus. Keep
    // rendering when unfocused; throttle with a short sleep so a background
    // window doesn't peg a CPU core (the frontend animates at the original's
    // 50ms tick cadence anyway, so ~16ms is well under the visible rate).
    else if (!g_active && !g_nav_demo) {
        Sleep(15);
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

// --------------------------------------------------------------------------
// Scripted in-process nav demo (verification harness for the menu state machine).
//
// Enabled only when env var MASHED_NAV_DEMO is set. Drives the menu purely
// through the standalone's OWN in-process input path: it writes edge events
// directly into the g_keys / g_keys_prev buffers that PollKeyboard fills and
// UpdateMenuSelection reads — i.e. the exact same code path real keystrokes take,
// with NO OS-level input injection (no keybd_event / SendInput / SetForegroundWindow).
//
// At scripted frames it dumps the D3D9 backbuffer to a 24-bit BMP in verify/ so
// the push/pop/cursor-move transitions are captured without any screen-grab.
// --------------------------------------------------------------------------
bool g_nav_demo = false;

// Dump the current backbuffer (post-Present is unavailable, so we grab the
// render target before Present, see RenderFrame) to a 24-bit BMP. Self-contained
// (no D3DX dependency). Returns true on success.
bool DumpBackbufferBMP(const char* path) {
    if (!g_device) return false;
    IDirect3DSurface9* rt = nullptr;
    if (FAILED(g_device->GetRenderTarget(0, &rt)) || !rt) return false;
    D3DSURFACE_DESC d{};
    rt->GetDesc(&d);
    IDirect3DSurface9* sys = nullptr;
    bool ok = false;
    if (SUCCEEDED(g_device->CreateOffscreenPlainSurface(
            d.Width, d.Height, d.Format, D3DPOOL_SYSTEMMEM, &sys, nullptr)) && sys) {
        if (SUCCEEDED(g_device->GetRenderTargetData(rt, sys))) {
            D3DLOCKED_RECT lr{};
            if (SUCCEEDED(sys->LockRect(&lr, nullptr, D3DLOCK_READONLY))) {
                const int W = static_cast<int>(d.Width), H = static_cast<int>(d.Height);
                const int rowBytes = W * 3;
                const int pad = (4 - (rowBytes & 3)) & 3;
                const int imgSize = (rowBytes + pad) * H;
                const int fileSize = 54 + imgSize;
                std::FILE* f = std::fopen(path, "wb");
                if (f) {
                    unsigned char hdr[54] = {};
                    hdr[0]='B'; hdr[1]='M';
                    *reinterpret_cast<int*>(hdr+2)  = fileSize;
                    *reinterpret_cast<int*>(hdr+10) = 54;
                    *reinterpret_cast<int*>(hdr+14) = 40;
                    *reinterpret_cast<int*>(hdr+18) = W;
                    *reinterpret_cast<int*>(hdr+22) = H;   // bottom-up
                    *reinterpret_cast<short*>(hdr+26) = 1;
                    *reinterpret_cast<short*>(hdr+28) = 24;
                    *reinterpret_cast<int*>(hdr+34) = imgSize;
                    std::fwrite(hdr, 1, 54, f);
                    // Source is X8R8G8B8 (BGRA little-endian). Write bottom-up BGR.
                    const unsigned char* base = static_cast<const unsigned char*>(lr.pBits);
                    unsigned char* rowbuf = static_cast<unsigned char*>(std::malloc(rowBytes + pad));
                    if (rowbuf) {
                        for (int i = 0; i < pad; ++i) rowbuf[rowBytes + i] = 0;
                        for (int y = H - 1; y >= 0; --y) {
                            const unsigned char* src = base + y * lr.Pitch;
                            for (int x = 0; x < W; ++x) {
                                rowbuf[x*3+0] = src[x*4+0]; // B
                                rowbuf[x*3+1] = src[x*4+1]; // G
                                rowbuf[x*3+2] = src[x*4+2]; // R
                            }
                            std::fwrite(rowbuf, 1, rowBytes + pad, f);
                        }
                        std::free(rowbuf);
                        ok = true;
                    }
                    std::fclose(f);
                }
                sys->UnlockRect();
            }
        }
        sys->Release();
    }
    rt->Release();
    return ok;
}

// Inject a key edge (held this frame, released last frame) into the DInput state
// buffers so UpdateMenuSelection sees a rising edge. In-process only.
void NavDemoTap(unsigned char dik) {
    g_keys_prev[dik] = 0x00;   // was up last frame
    g_keys[dik]      = 0x80;   // held this frame -> rising edge
}

// Per-frame scripted driver. `phase` counts RenderFrame iterations after the
// menu is up. Returns true when the script has finished (caller quits).
// Screenshots are saved to verify/msm_*.bmp at the documented transitions.
void NavDemoLog(int phase, const char* what, bool ok) {
    std::FILE* lf = std::fopen(kLogPath, "a");
    if (lf) {
        std::fprintf(lf, "NAV_DEMO phase=%d %s ok=%d depth=%d screen=%d cur=%d nrec=%d\n",
                     phase, what, (int)ok, mashed_re::Frontend::Nav_Depth(),
                     mashed_re::Frontend::Nav_ScreenId(),
                     mashed_re::Frontend::Nav_Cursor(),
                     mashed_re::Frontend::Nav_RecordCount());
        std::fclose(lf);
    }
}

bool RunNavDemoStep(int phase) {
    if (!g_nav_demo) return false;
    using namespace mashed_re::Frontend;
    // Let the window/device settle for the first ~60 frames, then run the script
    // with ~10-frame spacing so each transition is a distinct, settled frame.
    // Deep faithful path via the REVERSED push map (no heuristic):
    //   root (screen 0)                                        -- 5 items
    //     DOWN -> highlight item 1 ("0x27", action 0xff500000)
    //     ENTER -> push screen 8                               -- 5 items
    //       ENTER on item 0 ("0x156", action 0xff430000)
    //       -> push screen 0x13 (=19)                          -- 4 items (all 0xff450000)
    //         ESC -> pop back to screen 8
    //           ESC -> pop back to root
    // Each ENTER target is resolved by ItemActionCode + ActionToScreen, proving
    // the descriptor-table action codes drive real screen transitions.
    switch (phase) {
        case 60:  NavDemoLog(phase,"cap root",       DumpBackbufferBMP("verify/msm_tree_1_root.bmp"));       break;
        case 70:  NavDemoTap(DIK_DOWN);              NavDemoLog(phase,"tap DOWN",       true);               break;
        case 80:  NavDemoLog(phase,"cap root_item1", DumpBackbufferBMP("verify/msm_tree_2_root_item1.bmp")); break;
        case 90:  NavDemoTap(DIK_RETURN);            NavDemoLog(phase,"tap ENTER",      true);               break;
        case 100: NavDemoLog(phase,"cap screen8",    DumpBackbufferBMP("verify/msm_tree_3_screen8.bmp"));    break;
        case 110: NavDemoTap(DIK_RETURN);            NavDemoLog(phase,"tap ENTER",      true);               break;
        case 120: NavDemoLog(phase,"cap screen19",   DumpBackbufferBMP("verify/msm_tree_4_screen19.bmp"));   break;
        // R2-close: adjust Music Volume (RIGHT x2) on the Sound screen — the
        // settings model mutates + persists to mashed_re_settings.bin; the
        // post-adjust capture shows the slider moved.
        case 122: NavDemoTap(DIK_RIGHT);             NavDemoLog(phase,"tap RIGHT",      true);               break;
        case 124: NavDemoTap(DIK_RIGHT);             NavDemoLog(phase,"tap RIGHT",      true);               break;
        case 126: NavDemoLog(phase,"cap snd_adj",    DumpBackbufferBMP("verify/parity/sa_sound_adjusted.bmp")); break;
        case 130: NavDemoTap(DIK_ESCAPE);            NavDemoLog(phase,"tap ESC",        true);               break;
        case 140: NavDemoLog(phase,"cap back8",      DumpBackbufferBMP("verify/msm_tree_5_back_screen8.bmp"));break;
        case 150: NavDemoTap(DIK_ESCAPE);            NavDemoLog(phase,"tap ESC",        true);               break;
        case 160: NavDemoLog(phase,"cap back_root",  DumpBackbufferBMP("verify/msm_tree_6_back_root.bmp"));  break;
        // R2-close parity: capture screen 1 (the original's boot-time "Game Type
        // Select" — Single/Multi/Options/Bonus/Exit). It isn't reachable from
        // the screen-0 root via the push map, so push it directly (the original
        // enters it from the title-confirm flow).
        case 180: Nav(1, kNavPush);                  NavDemoLog(phase,"push screen1",   true);               break;
        case 190: NavDemoLog(phase,"cap screen1",    DumpBackbufferBMP("verify/parity/sa_gts.bmp"));         break;
        case 200: NavDemoLog(phase,"done",true);     return true;
        default: break;
    }
    return false;
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

// Menu navigation, now driven by the ported state machine (Frontend/MenuNavSM,
// a port of MASHED's FUN_0043d2a0 + the kv-iterator FUN_0042ad90). Up/Down move
// the per-screen cursor over the REAL descriptor-table items (rising-edge);
// Enter pushes the highlighted item's child screen; Esc/Backspace pops one level.
// Replaces the old hand-rolled g_menu_selected clamp. Input arrives ONLY through
// this in-process DirectInput path (no global input injection).
//
// Returns true if Esc was pressed AT THE ROOT (caller should quit the app);
// otherwise Esc was consumed as a pop-back.
// R2-close — persisted menu settings (Sound screen, id 19). The original's
// Sound screen renders value widgets (Music/SFX/Insults Volume sliders 0..10
// plus an Insults Off/On toggle, adjusted LEFT/RIGHT — see the parity capture
// verify/parity/orig_sound.png) and persists through the save path. The
// standalone keeps a settings model persisted to its OWN file (never touches
// original/): mashed_re_settings.bin = 'MRES' magic + 4 int32 values.
struct MenuSettings {
    // Boot defaults pinned to the original's clean baselines (all-zero
    // gamesave): scr19 fills w=70/70/70 -> volumes 7, scr30 fill w=30 ->
    // gamma 3 (verify/parity_caps/orig_scr19_*/orig_scr30_*).
    std::int32_t music   = 7;   // 0..10
    std::int32_t sfx     = 7;   // 0..10
    std::int32_t insults = 7;   // 0..10
    std::int32_t insults_on = 0;  // tri-state 0=Off 1=Auto 2=Manual (user review #21)
    std::int32_t gamma = 3;     // 0..10 (screen 30 slider, user review #22)
    std::int32_t autosave = 1;  // 0=Off 1=On (screen 32 toggle, user review #23)
};
MenuSettings g_settings;
constexpr char kSettingsPath[] = "mashed_re_settings.bin";
// #16 (user review): the slider bar must move CONTINUOUSLY, not snap between the
// 11 discrete 0..10 steps. Keep the faithful 0..10 model but animate the drawn
// fill: g_sliderDisp lerps toward the target value each frame (music/sfx/insults/
// gamma) so the orange bar slides smoothly.
float g_sliderDisp[4] = { 7.f, 7.f, 7.f, 3.f };

void SaveMenuSettings() {
    if (std::FILE* f = std::fopen(kSettingsPath, "wb")) {
        std::fwrite("MRES", 1, 4, f);
        std::fwrite(&g_settings, sizeof(g_settings), 1, f);
        std::fclose(f);
    }
}
void LoadMenuSettings() {
    if (std::FILE* f = std::fopen(kSettingsPath, "rb")) {
        char magic[4] = {};
        MenuSettings s;
        if (std::fread(magic, 1, 4, f) == 4 && std::memcmp(magic, "MRES", 4) == 0 &&
            std::fread(&s, sizeof(s), 1, f) == 1) {
            g_settings = s;
        }
        std::fclose(f);
    }
}

// LEFT/RIGHT on the Sound screen adjusts the highlighted value and persists
// immediately (autosave-style). Rows: 0..2 = volumes (0..10), 3 = toggle.
void AdjustSoundSetting(int row, int dir) {
    auto clamp10 = [](int v) { return v < 0 ? 0 : (v > 10 ? 10 : v); };
    switch (row) {
        case 0: g_settings.music   = clamp10(g_settings.music   + dir); break;
        case 1: g_settings.sfx     = clamp10(g_settings.sfx     + dir); break;
        case 2: g_settings.insults = clamp10(g_settings.insults + dir); break;
        case 3: { // tri-state Off/Auto/Manual, dir-aware wrap (user review #21)
            int m = g_settings.insults_on + (dir >= 0 ? 1 : -1);
            g_settings.insults_on = (m < 0) ? 2 : (m > 2 ? 0 : m);
            break;
        }
        case 100: g_settings.gamma = clamp10(g_settings.gamma + dir); break; // gamma slider (#22)
        case 101: g_settings.autosave = (dir != 0) ? (g_settings.autosave ? 0 : 1)
                                                   : g_settings.autosave; break; // autosave toggle (#23)
        default: return;
    }
    SaveMenuSettings();
}

// #18/#19: drive the Load/Save modal flow. step 0 dismisses; other steps set
// the body text id (Font36.piz/USA.DAT) + button mode. Keeps the current fade
// alpha so flow transitions don't re-fade.
void ModalGo(int step) {
    g_modal_step     = step;
    g_modal_timer_ms = 0;
    g_modal_button_id = 0x2d;            // "Continue" (default)
    switch (step) {
        case 10: g_modal_body_id = 0x215; g_modal_yesno = true;  break; // load warn
        case 11: g_modal_body_id = 0x1bc; g_modal_yesno = false; break; // "Load Successful."
        case 20: g_modal_body_id = 0x1ca; g_modal_yesno = true;  break; // overwrite?
        case 21: g_modal_body_id = 0x233; g_modal_yesno = false;        // "Saving game data."
                 g_modal_timer_ms = GetTickCount();              break; // auto-advance
        case 22: g_modal_body_id = 0x279; g_modal_yesno = false; break; // "Save successful."
        default: g_modal_body_id = 0; g_modal_step = 0; g_modal_yesno = false; break;
    }
}

bool UpdateMenuSelection() {
    using namespace mashed_re::Frontend;
    if (!g_kbd) return false;
    // #1 (user review): DirectInput is DISCL_BACKGROUND|NONEXCLUSIVE, so g_kbd
    // reads GLOBAL key state even when our window isn't focused — meaning keys
    // pressed in another app were driving the menu. Gate all input on window
    // focus (g_active, set by WM_ACTIVATE); the nav-demo driver bypasses this.
    if (!g_active && !g_nav_demo) return false;
    const bool up_now    = (g_keys[DIK_UP]        & 0x80) != 0;
    const bool up_prev   = (g_keys_prev[DIK_UP]   & 0x80) != 0;
    const bool down_now  = (g_keys[DIK_DOWN]      & 0x80) != 0;
    const bool down_prev = (g_keys_prev[DIK_DOWN] & 0x80) != 0;
    const bool ent_now   = (g_keys[DIK_RETURN]    & 0x80) != 0;
    const bool ent_prev  = (g_keys_prev[DIK_RETURN]& 0x80) != 0;
    const bool esc_now   = (g_keys[DIK_ESCAPE]    & 0x80) != 0;
    const bool esc_prev  = (g_keys_prev[DIK_ESCAPE]& 0x80) != 0;
    const bool bks_now   = (g_keys[DIK_BACK]      & 0x80) != 0;
    const bool bks_prev  = (g_keys_prev[DIK_BACK] & 0x80) != 0;
    const bool lft_now   = (g_keys[DIK_LEFT]      & 0x80) != 0;
    const bool lft_prev  = (g_keys_prev[DIK_LEFT] & 0x80) != 0;
    const bool rgt_now   = (g_keys[DIK_RIGHT]     & 0x80) != 0;
    const bool rgt_prev  = (g_keys_prev[DIK_RIGHT]& 0x80) != 0;

    // Phase 0 = boot legal splash (#2): any key advances to the title (the
    // original auto-advances after 24000000/3MHz = 8s OR on input via
    // DAT_0067d960; the 8s timeout is handled in RenderFrame).
    if (g_frontend_phase == 0) {
        const bool any = ent_now || esc_now || (g_keys[DIK_SPACE] & 0x80) ||
                         down_now || up_now;
        if (any) g_frontend_phase = 1;
        return false;
    }
    // F6: title phase (DAT_0067eca4 == 1): no item nav; confirm advances the
    // phase and pushes screen 1 (the original's title-confirm flow — the same
    // push the nav demo driver replicates at phase 180).
    if (g_frontend_phase < 2) {
        if (ent_now && !ent_prev) {
            // #12 (user review): the frontend nav is ALREADY rooted at the real
            // main menu (screen 1 = kT1 "Game Type Select": Single Player/Multi
            // Player/Options) by Nav_Init — see the root-cause note there. The
            // old code pushed screen 1 ONTO screen 0 (the in-race PAUSE menu
            // kT0: Continue/Restart/Quit), so backing out of any submenu
            // revealed the pause menu. Just reveal the menu; do NOT push.
            g_frontend_phase = 3;
            // Title->menu = the original's reload: raises the arc-wash fade
            // pair (permanent 0x60/0x78 haze at menu screens; clean baseline
            // verify/orig_backbuffer_f2100.bmp).
            LogoOverlayFadeSet(0xff, -1);
        }
        if (esc_now && !esc_prev) return true;          // quit from title
        return false;
    }
    // #8/#18/#19 confirm-dialog modal: while shown it swallows all nav input
    // (FUN_00433f40 is modal). Yes/No steps: Enter=Yes (advance), Esc=No
    // (dismiss). Continue steps: Enter/Esc dismiss. The 'Saving...' step (21)
    // auto-advances (handled in RenderFrame); ignore input there.
    if (g_modal_step != 0) {
        const bool yes = (ent_now && !ent_prev);
        const bool no  = (esc_now && !esc_prev);
        if (g_modal_step == 21) return false;        // auto-advancing; no input
        if (g_modal_yesno) {
            if (yes)      ModalGo(g_modal_step == 10 ? 11 : 21);  // load->done, save->saving
            else if (no)  ModalGo(0);                            // No: dismiss
        } else {
            if (yes || no) ModalGo(0);                           // Continue/dismiss
        }
        return false;
    }
    if (down_now && !down_prev) Nav_MoveCursor(+1);
    if (up_now   && !up_prev)   Nav_MoveCursor(-1);
    if (ent_now  && !ent_prev) {
        // Load Game / Save Game (Options screen 8, rows 2/3) open the real
        // confirm flow (#18/#19): Load -> warning 0x215 (Yes/No); Save ->
        // overwrite? 0x1ca (Yes/No) -> saving -> success. Other items nav.
        const int sid = Nav_ScreenId(), cur = Nav_Cursor();
        if (sid == 8 && cur == 2) {       // Load Game
            ModalGo(10); g_modal_alpha = 0;
        } else if (sid == 8 && cur == 3) { // Save Game
            ModalGo(20); g_modal_alpha = 0;
        } else {
            Nav_Select();                  // push child screen
        }
    }
    if ((bks_now && !bks_prev)) Nav_Back();             // Backspace = pop
    if (esc_now  && !esc_prev) {
        // #10 (user review): ESC at the main-menu ROOT must NOT quit the game.
        // In a submenu it pops one level; at the top (main menu) it returns to
        // the title/attract phase (the original goes back to the title loop, not
        // PostQuitMessage). Quitting only happens from the title (above).
        if (!Nav_Back()) { g_frontend_phase = 1; g_splash_start_ms = 0; return false; }
    }
    // Sound screen (19): LEFT/RIGHT adjusts the highlighted value (persisted).
    // Item 20 (user review): the volume sliders ramp CONTINUOUSLY while a key
    // is held (not one step per press). Rows 0..2 (volumes) ramp on key-HELD
    // with a short initial delay + repeat cadence; row 3 (Insults tri-state)
    // stays edge-triggered (discrete cycle).
    {
        const int sid = Nav_ScreenId();
        const int cur = Nav_Cursor();
        static int s_holdL = 0, s_holdR = 0;
        // ramp: act on the first frame, then EVERY frame after a short initial
        // hold so the value climbs continuously while held (#16) — paired with
        // the lerped bar fill, the slider slides smoothly instead of stepping.
        auto ramp = [](int& hold, bool down) -> bool {
            if (!down) { hold = 0; return false; }
            const int h = hold++;
            return h == 0 || h >= 6;
        };
        if (sid == 19 && cur >= 0) {              // Sound
            if (cur == 3) {                       // insults tri-state: discrete
                if (rgt_now && !rgt_prev) AdjustSoundSetting(3, +1);
                if (lft_now && !lft_prev) AdjustSoundSetting(3, -1);
            } else {                              // volumes: continuous ramp
                if (ramp(s_holdR, rgt_now)) AdjustSoundSetting(cur, +1);
                if (ramp(s_holdL, lft_now)) AdjustSoundSetting(cur, -1);
            }
        } else if (sid == 30 && cur == 0) {       // Gamma: continuous ramp (#22)
            if (ramp(s_holdR, rgt_now)) AdjustSoundSetting(100, +1);
            if (ramp(s_holdL, lft_now)) AdjustSoundSetting(100, -1);
        } else if (sid == 32 && cur == 0) {       // Autosave: discrete toggle (#23)
            if (rgt_now && !rgt_prev) AdjustSoundSetting(101, +1);
            if (lft_now && !lft_prev) AdjustSoundSetting(101, -1);
        } else if (sid == 4) {                     // Player Color Select (#25)
            // LEFT/RIGHT cycle player 1's car-color through the 10 NFL* sprites
            // (discrete, edge-triggered) — the visible interaction on screen 4.
            if (rgt_now && !rgt_prev) g_csel_p1_car = (g_csel_p1_car + 1) % 10;
            if (lft_now && !lft_prev) g_csel_p1_car = (g_csel_p1_car + 9) % 10;
        }
    }
    // Mirror the nav cursor into the legacy global for any code still reading it.
    if (Nav_Cursor() >= 0) g_menu_selected = static_cast<std::uint32_t>(Nav_Cursor());
    return false;
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
    // R4: depth buffer for the 3D track path (unused by the 2D menu path).
    g_pp.EnableAutoDepthStencil = TRUE;
    g_pp.AutoDepthStencilFormat = D3DFMT_D16;

    // Log the current adapter display mode (format/size) — a degraded desktop
    // (non-native res / changed bit-depth) is the usual cause of an otherwise-
    // valid windowed CreateDevice returning D3DERR_INVALIDCALL (0x8876086C).
    D3DDISPLAYMODE dm = {};
    if (SUCCEEDED(g_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm))) {
        if (std::FILE* log = std::fopen(kLogPath, "a")) {
            std::fprintf(log, "InitD3D9: adapter mode %ux%u fmt=%d refresh=%u\n",
                         dm.Width, dm.Height, dm.Format, dm.RefreshRate);
            std::fclose(log);
        }
    }

    // Robust device creation. Two param variants x several device types, each
    // retried, every HRESULT logged. Variant B drops the auto depth-stencil
    // (only the 3D track path needs it; the menu doesn't) and matches the
    // back-buffer format to the live desktop — this recovers the common
    // D3DERR_INVALIDCALL from an incompatible depth/format at a non-native mode.
    struct Attempt { D3DDEVTYPE type; DWORD flags; const char* name; };
    const Attempt attempts[] = {
        { D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, "HAL/SW" },
        { D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, "HAL/HW" },
        { D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING, "REF/SW" },
    };
    for (int variant = 0; variant < 2; ++variant) {
        if (variant == 1) {
            // Variant B: no auto depth-stencil, explicit live back-buffer fmt.
            g_pp.EnableAutoDepthStencil = FALSE;
            g_pp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
            if (dm.Format != D3DFMT_UNKNOWN) g_pp.BackBufferFormat = dm.Format;
        }
        for (const Attempt& a : attempts) {
            for (int retry = 0; retry < 3; ++retry) {
                HRESULT hr = g_d3d->CreateDevice(D3DADAPTER_DEFAULT, a.type,
                                                 g_hwnd, a.flags, &g_pp, &g_device);
                if (std::FILE* log = std::fopen(kLogPath, "a")) {
                    std::fprintf(log,
                        "InitD3D9: CreateDevice(%s,var%d) try %d -> hr=0x%08lX%s\n",
                        a.name, variant, retry, static_cast<unsigned long>(hr),
                        SUCCEEDED(hr) ? " OK" : "");
                    std::fclose(log);
                }
                if (SUCCEEDED(hr)) {
                    // Note if we fell back to no-depth (the track path must
                    // re-create its own depth surface if it ever runs).
                    g_depth_disabled = (variant == 1);
                    return true;
                }
                Sleep(120);
            }
        }
    }
    g_d3d->Release();
    g_d3d = nullptr;
    return false;
}

void ShutdownD3D9() {
    // Release D3D9-owned resources (textures, vertex buffers) before the
    // device itself.
    g_quad_renderer.Shutdown();
    if (g_device) { g_device->Release(); g_device = nullptr; }
    if (g_d3d)    { g_d3d->Release();    g_d3d    = nullptr; }
}

// Forward decl: defined ~L1400. Resolves a menu message id to its UTF-16 text
// (faithful to FUN_00427780). Used by the nav-driven menu draw loop in RenderFrame.
int GetMenuMessage(int id, wchar_t* out, int cap);

// B19 faithful font: draw `s` at horizontal anchor `cx`, top at `top_y`, glyph
// cell height `height_px`, tint `argb`, glyph-by-glyph through the bridge using
// MASHED's FGDC20 atlas UVs. No-op if the font isn't loaded. When `anchor_left`
// is true, `cx` is the LEFT edge (the original's FUN_00427680 left-justified
// layout); otherwise `cx` is the centre (legacy callers).
// grad_bot_frac < 1.0 draws each glyph with a top->bottom vertical ALPHA
// gradient (top = argb, bottom = argb with alpha*frac), matching the original's
// menu-item text (FUN_00428140 -> FUN_00556e90 sets top=base / bottom=faded).
// 1.0 = flat (the title/chrome path FUN_00427e00 passes all-same).
// Verbatim FUN_00554940 / FUN_00427680 text law (pool0 decomp + original
// backbuffer calibration 2026-06-12, verify/orig_backbuffer_f300.bmp):
//  - The original's text pipe works in a normalized [0,1]^2 space over the
//    FULL screen: print scale = menu_scale x 0.0708 (_DAT_005cd5fc), x via
//    1/640 (@0x005cd5a8), y via 1/480 (@0x005cc560). So the glyph cell is
//    0.0708*s of the screen WIDTH horizontally and of the screen HEIGHT
//    vertically — glyphs render 4/3 WIDER than the 33px atlas cell aspect.
//    (Pixel-verified: "Press button to start" scale 1.2 ink width 335px,
//    centered pen start 150.3 predicted / 150 measured @640.)
//  - Glyph quad: x = pen .. pen + rec.width (record float@+16, a unit-cell
//    fraction), y spans the full cell; UV = the record rect (FUN_00554940
//    vertex build off the 32-byte runtime record).
//  - Advance = (rec.width + tracking[cs+0x0c]) * cell_w. Unmapped codepoints
//    advance ZERO (FUN_00554940/FUN_005554d0 advance only when LUT idx >= 0).
//  - Anchor: `anchor_y` is the ORIGINAL's y argument (record y, cited layout
//    constants). Quad top = anchor_y + cell_h*(0.025/0.0708 + c8 - 1)
//    = anchor_y - 0.4954*cell_h  (0.025 @0x005cc9a4 via FUN_00427680;
//    c8 = charset+0x08 = 0.151515 from FGDC20.RWF). This independently
//    re-derives the bit-verified #10 plate centering (rec.y-13.5..+13.7 at
//    s=0.8 vs plate -12..+14).
void DrawMashedString(const wchar_t* s, float cx, float anchor_y,
                      float height_px, std::uint32_t argb,
                      bool anchor_left = false, float grad_bot_frac = 1.0f) {
    if (!g_font.ready() || !s) return;
    const float cell_h = height_px;
    const float cell_w = height_px * (4.0f / 3.0f);  // ScreenW/ScreenH (4:3 both sides)
    const float trk    = g_font.tracking();
    const float top_y  = anchor_y +
        cell_h * (0.025f / 0.0708f + g_font.baseline_c8() - 1.0f);
    // Codepoints 0..255 flow through: 0..127 hit the ASCII LUT, 0x80..0xFF the
    // FGDC20 extended table (FUN_00554390 font+0x12c) — the prompt strip's nav
    // glyphs (remapped 0x80..0x8f arrows/back) resolve there. Only codepoints
    // outside both ranges fall back to '?'.
    auto glyph_of = [](wchar_t c) -> unsigned char {
        return static_cast<unsigned char>((c >= 0 && c < 256) ? c : '?');
    };
    // Pass 1: measure (FUN_005554d0: sum of (width + tracking) per mapped glyph).
    float total = 0.f;
    for (const wchar_t* p = s; *p; ++p) {
        float uv[4], wf;
        if (g_font.Glyph(glyph_of(*p), uv, &wf)) total += (wf + trk) * cell_w;
    }
    // Pass 2: draw (centered = align-2: start at cx - total*0.5, the
    // _DAT_005cc32c = 0.5 path of FUN_00427680).
    float penX = anchor_left ? cx : (cx - total * 0.5f);
    for (const wchar_t* p = s; *p; ++p) {
        float uv[4], wf;
        if (!g_font.Glyph(glyph_of(*p), uv, &wf)) continue;  // no advance
        std::uint32_t uvb[4];
        std::memcpy(uvb, uv, sizeof(uvb));
        const float gw = wf * cell_w;
        if (grad_bot_frac < 0.999f) {
            const std::uint32_t a = argb >> 24;
            const std::uint32_t ab =
                static_cast<std::uint32_t>(a * grad_bot_frac);
            const std::uint32_t bot = (ab << 24) | (argb & 0xffffffu);
            HudIm2DQuadCorners(g_font.handle(), penX, top_y, gw, cell_h,
                               argb, argb, bot, bot, uvb);
        } else {
            HudIm2DQuad(g_font.handle(), penX, top_y, gw, cell_h, argb, uvb);
        }
        penX += (wf + trk) * cell_w;
    }
}

// Greedy word-wrap: draw `text` centered on cx, breaking at spaces so no line
// exceeds maxchars; lines stack downward by `lh`. Used for the modal body text
// (#18/#19), whose warning strings are too long for one line. Returns next y.
float DrawWrappedCentered(const wchar_t* text, float cx, float y, float h,
                          float lh, std::uint32_t color, int maxchars) {
    wchar_t line[256]; int ll = 0;
    wchar_t word[256]; int wl = 0;
    auto emitLine = [&]() {
        if (ll > 0) { line[ll] = 0; DrawMashedString(line, cx, y, h, color, false); y += lh; ll = 0; }
    };
    auto pushWord = [&]() {
        if (wl == 0) return;
        word[wl] = 0;
        if (ll > 0 && ll + 1 + wl > maxchars) emitLine();
        if (ll > 0 && ll < 254) line[ll++] = L' ';
        for (int i = 0; i < wl && ll < 254; ++i) line[ll++] = word[i];
        wl = 0;
    };
    for (const wchar_t* p = text; ; ++p) {
        if (*p == L' ' || *p == 0) { pushWord(); if (*p == 0) break; }
        else if (wl < 254) word[wl++] = *p;
    }
    emitLine();
    return y;
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

    // R4 opener: track fly-through mode (MASHED_TRACK_VIEW). Renders the
    // parsed RW world instead of the menu; dumps three orbit screenshots
    // for verification, then keeps flying.
    if (g_track_view && g_track.ready()) {
        g_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                        g_track.fog_color(), 1.0f, 0);  // horizon = fog color
        g_device->BeginScene();
        static DWORD s_t0 = GetTickCount();
        static DWORD s_prev = 0;
        const DWORD now = GetTickCount();
        const float t = static_cast<float>(now - s_t0) * 0.001f;
        const float dt = (s_prev == 0) ? 0.f
                       : static_cast<float>(now - s_prev) * 0.001f;
        s_prev = now;
        // Free camera: WASD move, Q/E down/up, arrows OR right-button mouse
        // look, R = back to auto-orbit.
        mashed_re::D3d9Render::TrackRenderer::CamInput ci;
        ci.dt = dt;
        if (g_kbd) {
            auto dn = [&](int k) { return (g_keys[k] & 0x80) != 0; };
            ci.move_fwd    = (dn(DIK_W) ? 1.f : 0.f) - (dn(DIK_S) ? 1.f : 0.f);
            ci.move_strafe = (dn(DIK_D) ? 1.f : 0.f) - (dn(DIK_A) ? 1.f : 0.f);
            ci.move_up     = (dn(DIK_E) ? 1.f : 0.f) - (dn(DIK_Q) ? 1.f : 0.f);
            ci.yaw_delta   = ((dn(DIK_RIGHT) ? 1.f : 0.f) -
                              (dn(DIK_LEFT)  ? 1.f : 0.f)) * 1.6f * dt;
            ci.pitch_delta = ((dn(DIK_UP)   ? 1.f : 0.f) -
                              (dn(DIK_DOWN) ? 1.f : 0.f)) * 1.0f * dt;
            ci.reset_orbit = dn(DIK_R);
        }
        {
            static POINT s_last{};
            static bool  s_had = false;
            if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                POINT p; GetCursorPos(&p);
                if (s_had) {
                    ci.yaw_delta   += static_cast<float>(p.x - s_last.x) * 0.005f;
                    ci.pitch_delta -= static_cast<float>(p.y - s_last.y) * 0.005f;
                }
                s_last = p; s_had = true;
            } else {
                s_had = false;
            }
        }
        // R5: drive the car. Arrow keys steer/accelerate; MASHED_DRIVE_DEMO
        // injects a scripted run (accelerate, then weave) and captures shots
        // proving motion + ground snap.
        static int s_frame = 0;
        ++s_frame;
        if (g_track.car_ready()) {
            mashed_re::D3d9Render::TrackRenderer::DriveInput di;
            di.dt = dt;
            static const bool s_drive_demo =
                GetEnvironmentVariableA("MASHED_DRIVE_DEMO", nullptr, 0) > 0;
            if (s_drive_demo) {
                // TIME-based schedule (fps varies with scene weight):
                // throttle 3s with a gentle weave from 1.5s, then coast —
                // keeps the run on the visible road, off the frozen bay.
                di.accel = (t < 3.0f) ? 1.f : 0.f;
                di.steer = (t > 1.5f) ? 0.35f * std::sin(t * 0.8f) : 0.f;
            } else if (g_kbd) {
                auto dn = [&](int k) { return (g_keys[k] & 0x80) != 0; };
                di.accel = (dn(DIK_UP) ? 1.f : 0.f) - (dn(DIK_DOWN) ? 1.f : 0.f);
                di.steer = (dn(DIK_RIGHT) ? 1.f : 0.f) - (dn(DIK_LEFT) ? 1.f : 0.f);
                // arrows belong to the car now; keep camera-look on the mouse
                ci.yaw_delta = 0.f; ci.pitch_delta = 0.f;
            }
            g_track.UpdateCar(di);
        }
        // R6 match flow: when a round ends, hold ~3s on the result then start
        // the next round (or finish the match).
        if (g_track.round_mode_ && g_track.round_winner() >= 0 &&
            g_track.match_winner() < 0) {
            static float s_round_end_t = -1.f;
            if (s_round_end_t < 0.f) s_round_end_t = t;
            if (t - s_round_end_t > 3.0f) {
                g_track.NextRoundOrEnd();
                s_round_end_t = -1.f;
            }
        }
        g_track.Render(g_device, t, &ci);
        // [SCAFFOLD] R6 HUD overlay — invented pips/banner; the REAL game
        // uses team badges + score bars + "+1/-1" points on a Current
        // Standings screen (verify/parity3d/orig_race_t03.png). Replace via
        // RE of the standings/score functions.
        // R6 HUD overlay (2D, on top of the 3D scene): countdown number,
        // per-car round-win scoreboard, round/result banner. The track
        // renderer leaves ZENABLE off on exit, so HudIm2DQuad/DrawMashedString
        // draw correctly here.
        if (g_track.round_mode_ && g_bridge_installed) {
            std::uint32_t uvf[4] = {0u, 0u, 0x3f800000u, 0x3f800000u};
            // scoreboard: one pip row per car, width = wins
            for (int i = 0; i < 4; ++i) {
                // (invented colors, packed-dword convention: the reimpl's
                // R<->B swap turns these into the intended screen colors)
                const std::uint32_t col[4] = {0xff4040e0u, 0xffffa040u,
                                              0xff60d040u, 0xff40c0e0u};
                const float y = 24.f + i * 26.f;
                HudIm2DQuad(0, 20.f, y, 22.f, 20.f, col[i], uvf);   // car tag
                // PORTED score data (FUN_0040b290 array DAT_008a94e0): one
                // bar segment per point (match win at >11, 0x00410510)
                for (int w = 0; w < g_track.score(i) && w < 12; ++w)
                    HudIm2DQuad(0, 50.f + w * 14.f, y, 10.f, 20.f,
                                col[i], uvf);
                // the signed +/- delta flash, 6000 ms (DAT_008a9520/10)
                if (g_font.ready() && g_track.delta_timer(i) > 0.f &&
                    g_track.score_delta(i) != 0) {
                    wchar_t d[8];
                    swprintf(d, 8, L"%+d", g_track.score_delta(i));
                    DrawMashedString(d, 240.f, y + 10.f, 24.f,
                                     g_track.score_delta(i) > 0
                                         ? 0xff80ff80u : 0xff8080ffu);
                }
            }
            if (g_font.ready()) {
                const float cd = g_track.countdown();
                if (cd > 0.f) {
                    const int n = static_cast<int>(cd) + 1;
                    wchar_t b[2] = {static_cast<wchar_t>(L'0' + (n > 3 ? 3 : n)), 0};
                    DrawMashedString(b, 400.f, 250.f, 120.f, 0xff80e0ffu);
                } else if (g_track.match_winner() >= 0) {
                    wchar_t b[24];
                    swprintf(b, 24, L"CAR %d WINS", g_track.match_winner() + 1);
                    DrawMashedString(b, 400.f, 60.f, 48.f, 0xff80e0ffu);
                } else if (g_track.round_winner() >= 0) {
                    DrawMashedString(L"CURRENT STANDINGS", 400.f, 60.f, 36.f,
                                     0xffffffffu);
                }
            }
        }
        g_device->EndScene();
        const bool car = g_track.car_ready();
        static bool s_shot[3] = {};
        if (!s_shot[0] && t >= 0.8f) {
            s_shot[0] = true;
            DumpBackbufferBMP(car ? "verify/r5/car_1_spawn.bmp"
                                  : "verify/r4/arctic_fly_1.bmp");
        }
        if (!s_shot[1] && t >= 2.2f) {
            s_shot[1] = true;
            DumpBackbufferBMP(car ? "verify/r5/car_2_drive.bmp"
                                  : "verify/r4/arctic_fly_2.bmp");
        }
        if (!s_shot[2] && t >= 3.6f) {
            s_shot[2] = true;
            DumpBackbufferBMP(car ? "verify/r5/car_3_weave.bmp"
                                  : "verify/r4/arctic_fly_3.bmp");
        }
        // R6 round telemetry/captures: each elimination + the winner + 2
        // periodic frames of the shared camera.
        if (g_track.round_mode_) {
            // match-aware captures: per round, grab the GO frame and the
            // round-result frame; grab the match-result once.
            static int  s_cap_round = 0;     // round we've GO-captured
            static int  s_res_round = 0;     // round we've result-captured
            static bool s_match_cap = false;
            const int rn = g_track.round_no();
            if (g_track.countdown() <= 0.f && s_cap_round != rn) {
                s_cap_round = rn;
                char pth[64];
                std::snprintf(pth, sizeof(pth), "verify/r6/round%d_go.bmp", rn);
                DumpBackbufferBMP(pth);
            }
            if (g_track.round_winner() >= 0 && s_res_round != rn) {
                s_res_round = rn;
                char pth[64];
                std::snprintf(pth, sizeof(pth), "verify/r6/round%d_result.bmp", rn);
                DumpBackbufferBMP(pth);
                std::FILE* lf = std::fopen(kLogPath, "a");
                if (lf) {
                    std::fprintf(lf, "R6 round %d OVER t=%.1fs winner=car%d "
                                     "(wins now:", rn, t, g_track.round_winner());
                    for (int i = 0; i < 4; ++i)
                        std::fprintf(lf, " c%d=%d", i, g_track.score(i));
                    std::fprintf(lf, ")\n");
                    std::fclose(lf);
                }
            }
            if (!s_match_cap && g_track.match_winner() >= 0) {
                s_match_cap = true;
                DumpBackbufferBMP("verify/r6/match_result.bmp");
                std::FILE* lf = std::fopen(kLogPath, "a");
                if (lf) {
                    std::fprintf(lf, "R6 MATCH OVER t=%.1fs winner=car%d "
                                     "(score>11 rule, 0x00410510)\n", t,
                                 g_track.match_winner());
                    std::fclose(lf);
                }
            }
        }
        if (car && s_frame == 340) {
            float cp[3]; g_track.car_pos(cp);
            std::FILE* lf = std::fopen(kLogPath, "a");
            if (lf) {
                std::fprintf(lf, "R5 drive t=%.1fs pos=(%.2f, %.2f, %.2f) "
                                 "speed=%.2f\n", t, cp[0], cp[1], cp[2],
                             g_track.car_speed());
                std::fclose(lf);
            }
        }
        g_device->Present(nullptr, nullptr, nullptr, nullptr);
        return true;
    }

    // Parity harness: frontend frame delimiter for MASHED_DBG_DRAWSTREAM
    // (same frame numbering as the BBDUMP counter below — both tick once per
    // frontend frame).
    mashed_re::D3d9Render::DrawStreamDump_OnFrameBegin();

    // Debug: MASHED_DBG_BBDUMP=1 dumps the backbuffer at frame ~200 to
    // verify/dbg_backbuffer.bmp - discriminates draw-side vs present-side
    // failures (window shows white while the bridge logs sane draws).
    {
        static int s_bb = -1;
        if (s_bb == -1) {
            char bbv[16] = {};
            if (GetEnvironmentVariableA("MASHED_DBG_BBDUMP", bbv, sizeof(bbv)) == 0) {
                s_bb = 0;
            } else {
                // "1" (or non-numeric) = legacy frame 200; N>1 = dump at frame N
                // (e.g. 700 to capture the title screen past the 8s splash).
                const int v = std::atoi(bbv);
                s_bb = (v > 1) ? v : 200;
            }
        }
        if (s_bb > 0 && --s_bb == 1)
            DumpBackbufferBMP("verify/dbg_backbuffer.bmp");
    }
    g_device->Clear(0, nullptr, D3DCLEAR_TARGET, kClearColor, 1.0f, 0);
    g_device->BeginScene();

    // F1 (frontend-faithful): the original's menu backdrop is VIDEO —
    // toastart/pc/movies/frontend.mpg played via DirectShow (the original's
    // own mechanism; ledger F1). When the graph runs, the current video
    // frame draws fullscreen; the B19a static PNG stays as the fallback.
    std::uint32_t uv_full[4] = { 0u, 0u, 0x3f800000u, 0x3f800000u };
    if (g_menu_video.ready()) {
        g_menu_video.Update();
        struct VRHW { float x, y, z, rhw; float u, v; };
        // Ground truth (log/menu_draw_dump.json draw 0): the original submits
        // the video as a 512x512 quad at the origin, 1:1 texel-to-virtual-px
        // (bottom 32 virtual px clip off the 480 backbuffer) — NOT stretched
        // to the full screen. Scaled 1.25x -> 640x640 on our 800x600 target.
        const float vw = 512.f * 1.25f, vh = 512.f * 1.25f;
        const VRHW q[4] = {
            {-0.5f,      -0.5f,      0.f, 1.f, 0.f, 0.f},
            {vw - 0.5f,  -0.5f,      0.f, 1.f, 1.f, 0.f},
            {-0.5f,      vh - 0.5f,  0.f, 1.f, 0.f, 1.f},
            {vw - 0.5f,  vh - 0.5f,  0.f, 1.f, 1.f, 1.f},
        };
        g_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        g_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        g_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        g_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        g_device->SetTexture(0, g_menu_video.texture());
        g_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
        g_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, q, sizeof(VRHW));
        g_device->SetTexture(0, nullptr);
        g_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        // Parity harness: this is the ONE frontend draw that bypasses the
        // bridge chokepoint (native DrawPrimitiveUP). Mirror it into the
        // draw stream as an Im2D-layout blob so the original's video quad
        // (its dump draw 0) has a counterpart instead of a phantom MISSING.
        {
            struct ImV { float x, y, z, w; std::uint32_t c; float u, v; };
            const ImV iv[4] = {
                {q[0].x, q[0].y, 0.f, 1.f, 0xffffffffu, q[0].u, q[0].v},
                {q[1].x, q[1].y, 0.f, 1.f, 0xffffffffu, q[1].u, q[1].v},
                {q[2].x, q[2].y, 0.f, 1.f, 0xffffffffu, q[2].u, q[2].v},
                {q[3].x, q[3].y, 0.f, 1.f, 0xffffffffu, q[3].u, q[3].v},
            };
            mashed_re::D3d9Render::DrawStreamDump_OnDraw(
                iv, 4, /*tex*/ -1, /*alpha*/ false, 5, 6, nullptr);
        }
    } else if (g_bridge_installed && g_menu_bg_ready) {
        HudIm2DQuad(kHandleMenuBg, 0.f, 0.f, 800.f, 600.f, 0xffffffffu, uv_full);
    }

    // ShellB runs the preview crossfade on EVERY frontend frame including
    // the title (titleframe dump 2026-06-12 contains the FUN_00474890 pair) -
    // no phase gate.
    if (g_bridge_installed && g_previews_ready && g_frontend_phase >= 1 &&
        GetEnvironmentVariableA("MASHED_DBG_NO_PREVIEWS", nullptr, 0) == 0) {
        static const int kCycle[16] = {0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3};
        const unsigned phase = (g_chrome_frame++) & 0x1ffu;
        if (phase == 0x1e0u) g_chrome_spriteA += 2;
        else if (phase == 0x0e0u) g_chrome_spriteB += 2;
        if ((g_chrome_spriteB & 1) == 0) g_chrome_spriteB += 1;
        int a6, a7;
        if (phase < 0x0e0u) { a7 = 0; a6 = 0xff; }
        else if (phase < 0x100u) {
            const int t = static_cast<int>(phase) - 0xe0;
            if (0xf < t) { a6 = (0x1f - t) * 0x10; a7 = 0xff; }
            else         { a7 = t * 0x10;          a6 = 0xff; }
        } else if (phase < 0x1e0u) { a6 = 0; a7 = 0xff; }
        else {
            const int t = static_cast<int>(phase) - 0x1e0;
            if (t < 0x10) { a6 = t * 0x10;          a7 = 0xff; }
            else          { a7 = (0x1f - t) * 0x10; a6 = 0xff; }
        }
        // FUN_00474890 draw form (pool0 decomp 2026-06-12): each preview layer
        // is TWO half-width quads — left half with corners 0/2 (TL/BL) faded
        // to alpha 0 (grad_flag path) so the layer fades IN over the video,
        // right half solid; w is halved inside (_DAT_005cc32c = 0.5). The
        // original pans the texture via the mode-1..5 UV switch over a
        // 6-sub-image layout — the standalone's per-track preview textures
        // draw full-UV for now [residual: UV-pan modes need the original's
        // preview atlas layout].
        const float pvx = 288.f * 1.25f, pvy = 64.f * 1.25f;
        const float pvh = 352.f * 1.25f;
        const float pvw2 = 176.f * 1.25f;            // 352 * 0.5 per quad
        auto draw_preview = [&](int handle, int alpha) {
            const std::uint32_t c =
                (static_cast<std::uint32_t>(alpha) << 24) | 0xffffffu;
            const std::uint32_t c0 = c & 0x00ffffffu;
            std::uint32_t uvL[4] = { 0u, 0u, 0x3f000000u, 0x3f800000u };
            std::uint32_t uvR[4] = { 0x3f000000u, 0u, 0x3f800000u, 0x3f800000u };
            HudIm2DQuadCorners(handle, pvx, pvy, pvw2, pvh, c0, c, c0, c, uvL);
            HudIm2DQuad(handle, pvx + pvw2, pvy, pvw2, pvh, c, uvR);
        };
        if (a6 != 0)
            draw_preview(kHandlePreview0 + kCycle[g_chrome_spriteA & 0xf] * 2, a6);
        if (a7 != 0)
            draw_preview(kHandlePreview0 + kCycle[g_chrome_spriteB & 0xf] * 2, a7);
    }

    // (2026-06-12 Wave-1 correction, ShellB decomp: the "white wash" quads at
    // 288/464 ARE the track-preview crossfade layer — FUN_0042e590 ->
    // FUN_00474890 two-quad pairs — drawn below by the preview block; and the
    // band-edge black fades at 384..512 are FUN_00473ee0's own first two band
    // quads (x = slide-128), drawn by the LogoOverlayDraw port. The static
    // duplicates that briefly lived here are REMOVED.)

    if constexpr (kTitleMode) {
        // B6: title-screen MVP — single centered 'main' texture quad. Suppressed
        // once the real menu background is loaded (B19a).
        if (g_atlas_slot_count > 0 && !g_menu_bg_ready) {
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
    // Chrome layer — FUN_00473ee0 (arc-wash + animated checker "race flag" grid
    // + band-edge fades). Drawn HERE — after previews, BEFORE the solid bands +
    // logo + press-button — so the animated flags render BEHIND the bars and the
    // wash does not veil the logo/press-button (#6/#7). slide_f settled = 512.
    if (g_bridge_installed && g_frontend_phase >= 1 &&
        GetEnvironmentVariableA("MASHED_DBG_NO_ARC", nullptr, 0) == 0) {
        using namespace mashed_re::Frontend;
        static int s_chrome_slide = 0;
        const float pre = static_cast<float>(s_chrome_slide);
        s_chrome_slide -= 0x10;
        if (s_chrome_slide < 0) s_chrome_slide = 0;
        static DWORD s_t0 = 0;
        const DWORD now = GetTickCount();
        if (s_t0 == 0) s_t0 = now;
        const float wave_t = static_cast<float>(now - s_t0) * (1.0f / 3000.0f);
        LogoOverlayDraw(pre + 512.0f, wave_t, 800.0f, 600.0f);
    }

    // B16: render MASHED's menu chrome (two dark bands + two white divider
    // lines) through the B15 bridge. Skipped during the phase-0 legal splash.
    if (g_bridge_installed && g_frontend_phase >= 1) {
        // B16 ShellA-bytes path RETIRED 2026-06-12: the dump-verbatim rebuild
        // below (log/menu_draw_dump.json draws 77-80) is bit-derived from the
        // original's vertex stream and renders identically on EVERY boot,
        // while the ShellA-bytes call depended on the cold-start granule
        // lottery (chrome=YES only some boots). One deterministic path wins.
        // (A white-window incident first blamed on ShellA turned out to be a
        // Present/monitor-level environment issue — backbuffer dumps proved
        // the draws correct. Verification now uses MASHED_DBG_BBDUMP.)
        if (false && g_b16_chrome_ready) {
            MenuChromeShellA();
        } else {
            // Reliable path: the SAME chrome layout read from MenuChromeShellA's
            // decomp (640x480 virtual, scaled 1.25x -> 800x600) issued via the
            // self-contained bridge draws. Geometry/colors now VERBATIM from
            // log/menu_draw_dump.json draws 77-80: bands are vertical alpha
            // gradients (ff000000 outer edge -> a0000000 inner edge), divider
            // lines are 1 virtual px at y=64/416.
            std::uint32_t uv[4] = {0u, 0u, 0x3f800000u, 0x3f800000u};
            HudIm2DQuadCorners(0, 0.f, 0.f, 800.f, 80.f,
                               0xff000000u, 0xff000000u, 0xa0000000u, 0xa0000000u,
                               uv);                                  // top band
            HudIm2DQuadCorners(0, 0.f, 520.f, 800.f, 80.f,
                               0xa0000000u, 0xa0000000u, 0xff000000u, 0xff000000u,
                               uv);                                  // bottom band
            HudIm2DQuad(0, 0.f, 520.f, 800.f, 1.25f, 0xffffffffu, uv); // bottom line
            HudIm2DQuad(0, 0.f,  80.f, 800.f, 1.25f, 0xffffffffu, uv); // top line
        }
    }

    // B19a: real MASHED logo (Font36.piz/MASHEDLOGO.PNG) over the chrome — fit
    // into a 560x360 box preserving aspect, top-centered below the top band.
    // F6: the big centered wavy logo is the TITLE-phase composition; menu
    // screens draw items without it (their small header chrome is the F2 pass).
    // --- #2 boot legal/copyright splash (phase 0). VERBATIM layout from
    // FUN_004288a0 (pool0 decomp 2026-06-12): MashedNEWLogo centered at
    // virtual (320,100,256x128), then the Empire/Supersonic copyright lines
    // (USA.DAT ids 0x1e5..0x1eb, scale 0.6, centered) at y=180/200/220/240/
    // 260/280, plus "Loading" (id 0x222) at (580,140). Shown ~8s
    // (24000000 / 3MHz timer) or until a key (handled in UpdateMenuSelection).
    if (g_frontend_phase == 0 && g_bridge_installed) {
        if (g_splash_start_ms == 0) g_splash_start_ms = GetTickCount();
        if (GetTickCount() - g_splash_start_ms > 8000u) g_frontend_phase = 1;
        // #2/#3 (user review): the original's boot LOADING/splash screen is on a
        // BLACK background (FUN_00428d30 black gradient), not the menu video bg.
        // Cover the backdrop with black so the logo + copyright + "Loading" read
        // on black. RE finding (FUN_00428d30 + FUN_004744a0 + full asset search):
        // the original's loader shows the MASHED logo + "Loading" text + the wavy
        // race-flag checker grid — there is NO separate "spinning disk" sprite
        // anywhere in the game's archives, so none is invented here.
        HudIm2DQuad(0, 0.f, 0.f, 800.f, 600.f, 0xff000000u, uv_full);
        if (g_menu_logo_ready) {
            const float lw = 256.f * 1.25f, lh = 128.f * 1.25f;
            HudIm2DQuad(kHandleMenuLogo, (320.f - 128.f) * 1.25f, (100.f - 64.f) * 1.25f,
                        lw, lh, 0xffffffffu, uv_full);
        }
        if (g_font.ready()) {
            const float th = 0.6f * 0.0708f * 480.f * 1.25f;  // scale-0.6 cell
            // The original flanks 0x1e6/0x1e7 around the 'bigE' icon on one
            // line (FUN_004282a0 width-measured). Without the icon we stack them
            // on their own lines to avoid overlap [residual: bigE icon + the
            // exact one-line flank].
            struct L { int id; float y; };
            static const L lines[] = {
                {0x1e5,180},{0x1e6,196},{0x1e7,212},{0x1e8,232},
                {0x1e9,248},{0x1ea,264},{0x1eb,280}};
            for (const L& ln : lines) {
                wchar_t t[128];
                if (GetMenuMessage(ln.id, t, 128) > 0)
                    DrawMashedString(t, 320.f * 1.25f, ln.y * 1.25f, th,
                                     0xffffffffu, false);
            }
            wchar_t lt[32];
            if (GetMenuMessage(0x222, lt, 32) > 0)
                DrawMashedString(lt, 580.f * 1.25f, 140.f * 1.25f,
                                 0.9f * 0.0708f * 480.f * 1.25f, 0xffffffffu, false);
        }
    }

    if (g_frontend_phase == 1 &&
        g_bridge_installed && g_menu_logo_ready && g_menu_logo_w > 0 && g_menu_logo_h > 0) {
        // (The MASHED logo quad itself is drawn AFTER the menu draw loop below
        // — parity adjudication 2026-06-12: the original's scr1 burst shows the
        // FUN_00428760 sprite-pipe logo as the LAST draw of the frame, on top
        // of the menu plates, and present on MENU screens too, not just the
        // title. The old phase==1-only draw here made it MISSING from settled
        // scr1 streams.)

        // Item 3 (user review): pulsing "Press button to start" — VERBATIM port
        // of FUN_00402fb0 (pool0 decomp 2026-06-12). alpha = 0x80 - (int)(sin
        // (phase) * -127.0) = 128 + 127*sin(phase); phase += 0.1/frame
        // (_DAT_005cc56c). Two centered draws (flag 2): black shadow at
        // (320,380) then white main at (316,376), scale 1.2 (string id 0x2a4).
        if (g_font.ready()) {
            static float s_pb_phase = 0.0f;
            const float a = 128.0f + 127.0f * std::sin(s_pb_phase);
            s_pb_phase += 0.1f;
            const std::uint32_t alpha =
                static_cast<std::uint32_t>(a < 0 ? 0 : (a > 255 ? 255 : a)) << 24;
            wchar_t pb[48];
            if (GetMenuMessage(0x2a4, pb, 48) > 0) {
                const float th = 1.2f * 0.0708f * 480.f * 1.25f;  // scale 1.2 cell
                DrawMashedString(pb, 320.f * 1.25f, 380.f * 1.25f, th,
                                 alpha /*black shadow*/, false);
                DrawMashedString(pb, 316.f * 1.25f, 376.f * 1.25f, th,
                                 alpha | 0xffffffu /*white*/, false);
            }
        }
    }

    // (#6/#7: the FUN_00473ee0 arc-wash + animated checker "race flag" layer is
    // now drawn EARLIER — right after the previews and BEFORE the bands/logo —
    // so the race flags sit BEHIND the bars and the wash no longer veils the
    // logo/press-button. See the LogoOverlayDraw call above the chrome bands.)

    // State-machine-driven menu draw — faithful port of FUN_0043c5b0's per-frame
    // draw loop. The nav stack (Frontend/MenuNavSM, port of FUN_0043d2a0 + the
    // kv-iterator FUN_0042ad90) has populated g_records from the REAL per-screen
    // descriptor table, with each record carrying its EXACT stored x/y/scale/color
    // fields (the 52-byte 0x898ac0 record layout). Here we reproduce the draw loop:
    //   - for each record, resolve its primary string id (MenuStringTable/
    //     GetMenuMessage = FUN_00427780/FUN_004277a0) and DrawMashedString at the
    //     record's stored x/y/scale/color (mirrors FUN_00428140(id,x,y,argb,scale)),
    //     with the +3.0f drop-shadow pass in opaque black (LAB_0043d185 of the
    //     original: shadow at +_DAT_005cc31c then base color);
    //   - draw the SELECTED item's highlight BACKGROUND quad (untextured solid +
    //     gradient overlay = FUN_00472c60 / FUN_00473540 in the -0xfc0000 branch);
    //   - draw the 0x224 special item's triangle border (FUN_00472dc0) — colored,
    //     centered.
    // Coords are MASHED's 640x480 virtual space; we scale by 1.25x to the 800x600
    // backbuffer (the standalone-safe analogue of FUN_00427680/ChromeBaseDraw's
    // screen-dimension scaling, whose RVA getters/scale globals are zeroed by the
    // image-pad and so cannot be called here — HudIm2DQuad takes absolute px).
    // F2: track-preview crossfade — verbatim phase math from the verified
    // MenuChromeShellB reimpl (0x0042e5b0): 512-frame cycle, A/B alpha ramps,
    // slot cycle 0..5 over the preview pairs, 352x352 at (288,64) virtual.
    // F2: the menu-screen chrome decal — FUN_0043c5b0 phase>=2 draws string
    // id 0x41 twice via FUN_00427e00 (shadow at 600,52 then white at 596,48,
    // scale 0.8; suppressed when FUN_0042b930()==0x21). Evidence:
    // FUN_0043c5b0_chrome.asm 0x0043c695..0x0043c6f3.
    if (g_bridge_installed && g_frontend_phase >= 2 && g_font.ready() &&
        mashed_re::Frontend::Nav_ScreenId() != 0x21) {
        wchar_t cs[64];
        if (GetMenuMessage(0x41, cs, 64) > 0) {
            // Scale 0.8, cell = 0.8 x 0.0708 x 480 virtual (the text law);
            // anchors are the ORIGINAL's cited y values (shadow 600,52 /
            // main 596,48 — FUN_0043c5b0_chrome.asm) now that DrawMashedString
            // applies the FUN_00427680 anchor law itself.
            const float csc = 0.8f * 0.0708f * 480.f * 1.25f;
            DrawMashedString(cs, 600.f * 1.25f, 52.f * 1.25f,
                             csc, 0xff000000u, false);
            DrawMashedString(cs, 596.f * 1.25f, 48.f * 1.25f,
                             csc, 0xffffffffu, false);
        }
    }

    if (g_bridge_installed && g_frontend_phase >= 2) {   // F6 phase gate
        using namespace mashed_re::Frontend;
        const MenuRecord* recs = Nav_Records();
        const int         nrec = Nav_RecordCount();
        const int         cur  = Nav_Cursor();
        const int         depth = Nav_Depth();

        // DEBUG (MASHED_DBG_MENU): one-shot dump of every record so the title/
        // back/prompt layout is verifiable offline.
        {
            static bool s_dumped = false;
            if (!s_dumped && GetEnvironmentVariableA("MASHED_DBG_MENU", nullptr, 0)) {
                s_dumped = true;
                if (std::FILE* lf = std::fopen(kLogPath, "a")) {
                    std::fprintf(lf, "\n[DBG records] screen=%d depth=%d nrec=%d cur=%d\n",
                                 Nav_ScreenId(), depth, nrec, cur);
                    for (int i = 0; i < nrec; ++i) {
                        const MenuRecord& d = recs[i];
                        wchar_t mm[128]; mm[0]=0;
                        const int mn = (d.prim_id >= 0) ? GetMenuMessage(d.prim_id, mm, 128) : 0;
                        char a[160]; int k=0;
                        for (int c=0;c<mn && k<150;++c){ wchar_t w=mm[c];
                            a[k++] = (w>=32 && w<127) ? (char)w : '.'; }
                        a[k]=0;
                        std::fprintf(lf, "  r%-2d tag=%08x prim=%d sec=%d x=%.0f y=%.0f sc=%.2f type=%d row=%d msg='%s'(n=%d c0=%04x)\n",
                            i, static_cast<unsigned>(d.tag), d.prim_id, d.sec_id,
                            d.x, d.y, d.scale, d.type, d.row_index, a, mn,
                            mn>0?(unsigned)mm[0]:0);
                    }
                    std::fclose(lf);
                }
            }
        }

        // Virtual(640x480) -> backbuffer(800x600) scale (FUN_00427680 analogue).
        constexpr float kVScale = 1.25f;

        // Faithful frame-rate-scaled slide animation: feed real elapsed ms into
        // the FUN_00493480 frame-clock port (the standalone clock stands in for
        // FUN_00493390), then run the verbatim FUN_004325c0 tick — per-tag
        // multipliers, 50ms tick quantization and settle/freeze states exactly
        // as the original computes them.
        {
            static DWORD s_last_ms = 0;
            const DWORD now_ms = GetTickCount();
            const int raw_ms =
                (s_last_ms == 0) ? 0 : static_cast<int>(now_ms - s_last_ms);
            s_last_ms = now_ms;
            Nav_FrameClockUpdate(raw_ms);
        }
        Nav_AnimTick();

        for (int r = 0; r < nrec; ++r) {
            const MenuRecord& rec = recs[r];

            // --- F4 CLOSED: footer prompt-strip rows (tags 0xff100000 /
            // 0xff110000 / 0xff230000), appended by the MenuNavSM port of
            // FUN_00432b30 (C4: PromptStripTwin diff GREEN 264/264). Draw rule
            // is the BIT-VERIFIED FUN_0043c5b0 record loop (MenuDrawLoopTwin
            // GREEN 20/20): prim_id while type is 0/2 (animating), sec_id
            // otherwise (settled/frozen); -1 skips (0x0043c885). The id renders
            // through GetMenuMessage -> FUN_00427780/004277a0 control-code
            // remap, turning prompt ids (0x42/0x43/0x48/0x13/0x58/0x133/0x225)
            // into their FGDC20 nav-glyph runs at the row's stored position
            // (virtual 64,428 scale 0.6 per FUN_0042ad10).
            {
                const std::uint32_t utag = static_cast<std::uint32_t>(rec.tag);
                if (utag == 0xff100000u || utag == 0xff110000u ||
                    utag == 0xff230000u) {
                    const int sid = (rec.type == 0 || rec.type == 2)
                                        ? rec.prim_id : rec.sec_id;
                    if (sid >= 0 && g_font.ready()) {
                        wchar_t stxt[128];
                        if (GetMenuMessage(sid, stxt, 128) > 0) {
                            const float sx = rec.x * kVScale +
                                static_cast<float>(Nav_RecordSlide(r)) *
                                    (260.f / 511.f);
                            const float sy = rec.y * kVScale;
                            const float sth =
                                rec.scale * 0.0708f * 480.f * kVScale;
                            const float ssh = 3.0f * kVScale;
                            DrawMashedString(stxt, sx + ssh, sy + ssh, sth,
                                             0xff000000u, true);
                            DrawMashedString(stxt, sx, sy, sth,
                                             static_cast<std::uint32_t>(rec.color),
                                             true);
                        }
                    }
                    continue;
                }
            }

            if (rec.prim_id < 0 &&
                static_cast<std::uint32_t>(rec.prim_id) != 0x224u) continue;  // -1 = no text
            const bool is_back = (static_cast<std::uint32_t>(rec.tag) == 0xff000000u);
            // #15 (user review, CORRECTS the old #11 handling): the back-row
            // record at (64,48) IS the screen TITLE (top-left) — "Game Type
            // Select" on the main menu (BuildRecords overrides screen 1 -> 0x43),
            // "Single Player"/"Options"/etc. on submenus. It was wrongly hidden
            // at depth<=1, which removed the title (user: "no Game Type Select
            // text at the top"). Draw it for every screen; records with a real
            // -1 title are already skipped above. The "Back" the user didn't want
            // (orig #11) is the FOOTER prompt strip, handled separately, not this
            // top title row.

            // The record's EXACT stored fields (FUN_0043c5b0 reads piVar9[-5]=X,
            // piVar9[-4]=Y, piVar9[-6]=scale, piVar9[-8]=color). Scale to 800x600.
            const float rx = rec.x * kVScale;
            const float ry = rec.y * kVScale;
            // On-screen glyph height: the original's text pipe multiplies the
            // record scale by _DAT_005cd5fc = 0.0708 (f32 0x3d90ff97 read from
            // MASHED.exe at 0x005cd5fc, 2026-06-12) as a fraction of the 480
            // virtual screen height -> cell = scale * 0.0708 * 480 virtual
            // (0.8 -> 27.2v, 0.6 -> 20.4v).
            const float text_h = rec.scale * 0.0708f * 480.f * kVScale;

            // Slide-in offset: map the record's slide counter (0x1ff..0, settled
            // = 0) to a horizontal entry offset so records slide in from the right
            // exactly as the anim tick drives them (faithful FUN_004325c0 motion).
            const float slideX = static_cast<float>(Nav_RecordSlide(r)) * (260.f / 511.f);

            const bool highlighted = (!is_back && rec.row_index == cur);
            const bool disabled = (!is_back && rec.row_index >= 0 &&
                                   !Nav_ItemEnabled(rec.row_index));
            const bool is224 = (static_cast<std::uint32_t>(rec.prim_id) == 0x224u);

            // --- selected-item highlight BACKGROUND quad (FUN_00472c60 solid +
            // FUN_00473540 gradient overlay). In the -0xfc0000 branch the original
            // draws this when iStack_3c == the per-screen cursor. Coords (640x480):
            //   x = 60.0, y = item.y - 13.0 + 1.0, w = 210.0 (list A=166.0),
            //   h = 26.0, fill 0xa0146ef0 (selected) / 0x40f8d0e8 (idle). We emit
            // the untextured solid via HudIm2DQuad (the standalone-safe
            // ChromeBaseDraw analogue; ChromeBaseDraw's own RVA scale-globals are
            // image-pad-zeroed) and a brighter top-half gradient band.
            // F2 — geometry/colors now mirror the BIT-VERIFIED draw loop
            // (MenuDrawLoopTwin GREEN 20/20): plate fill (idle 0x40f8d0e8 /
            // selected 0xa0146ef0), right-fade band at 60+plate_w (width 100),
            // and the 5-piece border kit on EVERY row (navy idle / orange
            // selected; unavailable rows grey). Color convention (parity
            // adjudication 2026-06-12): callers pass the ORIGINAL'S packed
            // dwords (the CONCAT13(alpha, 0xRRGGBB-swapped) forms the decomp
            // shows); the draw reimpls' cited byte-swap (U-3415) turns them
            // into the D3DCOLOR the original's vertex stream carries, and the
            // bridge submits the buffer raw — byte-identical to MASHED's
            // DAT_00898a20. Do NOT pre-swap constants here.
            // The back/header row draws NO plate chrome in the original — just
            // its bold white text on the top band (orig_options.png; the dump's
            // plate rows start at the first item, y=168). Items only.
            if (!is224 && !is_back && rec.prim_id >= 0) {
                const float px2 = 60.0f * kVScale + slideX;
                const float py2 = (rec.y - 13.0f + 1.0f) * kVScale;
                const float pw2 = 210.0f * kVScale;
                const float ph2 = 26.0f * kVScale;
                const float gx2 = (60.0f + 210.0f) * kVScale + slideX;
                const float gw2 = 100.0f * kVScale;
                const std::uint32_t fill =
                    highlighted ? 0xa0146ef0u : 0x40f8d0e8u;
                // Border navy: the original passes CONCAT13(alpha, 0x501513)
                // (device D3DCOLOR ff131550 after the reimpl's swap — matches
                // menu_draw_burst.json rows 14/18; disabled rows alpha 0x30).
                const std::uint32_t border =
                    disabled    ? 0x30501513u
                  : highlighted ? 0xff1050b4u : 0xff501513u;
                // Right-fades are true horizontal alpha gradients in the
                // original (menu_draw_dump.json draws 82/84/86: color -> same
                // color with alpha 00), not dimmed flats.
                const std::uint32_t fill0 = fill & 0x00ffffffu;
                const std::uint32_t bord0 = border & 0x00ffffffu;
                HudIm2DQuad(0, px2, py2, pw2, ph2, fill, uv_full);
                HudIm2DQuadCorners(0, gx2, py2, gw2, ph2,
                                   fill, fill0, fill, fill0, uv_full);
                const float bt = 2.0f * kVScale;
                HudIm2DQuad(0, px2, py2, pw2, bt, border, uv_full);
                HudIm2DQuadCorners(0, gx2, py2, gw2, bt,
                                   border, bord0, border, bord0, uv_full);
                HudIm2DQuad(0, px2, py2 + ph2 - bt, pw2, bt, border, uv_full);
                HudIm2DQuadCorners(0, gx2, py2 + ph2 - bt, gw2, bt,
                                   border, bord0, border, bord0, uv_full);
                HudIm2DQuad(0, px2 - 2.0f * kVScale, py2, bt, ph2, border,
                            uv_full);
            }
            if (highlighted && !is224) {
                const float hx = 60.0f * kVScale + slideX;
                const float hy = (rec.y - 13.0f + 1.0f) * kVScale;
                const float hw = 210.0f * kVScale;
                const float hh = 26.0f * kVScale;
                // R2-5: "Button" badge — the rounded left cap. The original's
                // highlight branch looks it up via SpriteLookupC("Button",
                // 0x0043cbbe) and submits via FUN_004739f0 with width 13.0
                // (0x41500000) at the bar's left edge (X bias 13.0 =
                // _DAT_005cd8d8), full UV, bar height, scale mode 1.
                if (g_menu_badge_ready) {
                    // F2 (bit-verified): the Button cap sits at x = 58-13 =
                    // 45, in the selected border color.
                    HudIm2DQuad(kHandleMenuBadge,
                                (58.0f - 13.0f) * kVScale + slideX, hy,
                                13.0f * kVScale, hh,
                                0xff1050b4u, uv_full);
                }
            }

            // --- 0x224 special centered item: triangle border (FUN_00472dc0,
            // 4 triangles framing the centered tile). Drawn as a colored border
            // rectangle outline (the standalone has no triangle primitive; the
            // bordered rect is the faithful colored-quad analogue, 0xa0146ef0).
            if (is224) {
                const float bw = 320.0f * kVScale, bh = 34.0f * kVScale;
                const float bx = (320.0f - 160.0f) * kVScale;  // centered on 320
                const float by = ry;
                const float t  = 2.0f * kVScale;               // border thickness
                const std::uint32_t bc = highlighted ? 0xa0146ef0u : 0x40f8d0e8u;
                HudIm2DQuad(0, bx,        by,        bw, t,  bc, uv_full); // top
                HudIm2DQuad(0, bx,        by+bh-t,   bw, t,  bc, uv_full); // bottom
                HudIm2DQuad(0, bx,        by,        t,  bh, bc, uv_full); // left
                HudIm2DQuad(0, bx+bw-t,   by,        t,  bh, bc, uv_full); // right
            }

            // --- the label (FUN_00428140 = id->string via FUN_00427780/004277a0,
            // then RtCharsetPrint). Resolve the id to its language string.
            wchar_t txt[128];
            const int n = is224 ? 0 : GetMenuMessage(rec.prim_id, txt, 128);
            if (!is224 && n <= 0) {
                // No string for this id; show the raw id so the record is still
                // visibly state-driven (NO-GUESSING about its label).
                wchar_t* q = txt; const wchar_t* pre = L"id ";
                for (const wchar_t* p = pre; *p; ++p) *q++ = *p;
                int v = rec.prim_id; wchar_t tmp[16]; int tt = 0;
                if (v == 0) tmp[tt++] = L'0';
                while (v > 0) { tmp[tt++] = static_cast<wchar_t>(L'0' + v % 10); v /= 10; }
                while (tt > 0) *q++ = tmp[--tt];
                *q = 0;
            }
            if (is224) continue;  // 0x224 has no string label (offset 0; border only)

            // Base color: the record's stored ARGB (piVar9[-8]); selected items
            // render in opaque-black-on-highlight as the original does in the
            // Text color — VERBATIM from FUN_0043c5b0 (pool0 decomp 2026-06-12):
            //   * LIST ITEMS (tag -0xfc0000) draw 0xff000000 OPAQUE BLACK for both
            //     selected AND idle (the highlight plate is the only selection cue,
            //     FUN_00428140 color arg 0xff000000 in both cursor branches);
            //   * BACK/HEADER rows (tag -0x1000000 etc., LAB_0043d185) draw the
            //     record's stored color (piVar9[-8] = white) with a black shadow;
            //   * GREYED rows: the dim path (DAT_008990e4) draws 0x80000000 with
            //     param_7=0x80 -> half-alpha black ("even more transparent").
            const bool is_item = !is_back;
            const std::uint32_t base_argb =
                disabled ? 0x80000000u                  // greyed: half-alpha black
              : is_item  ? 0xff000000u                  // list items: opaque black
                         : (static_cast<std::uint32_t>(rec.color) & 0xffffffffu);  // header: white

            // The original left-justifies text at the record's stored X (virtual
            // 64.0; FUN_00427680). Left-anchor at scaled X + slide offset so the
            // label sits inside the highlight bar (which starts at virtual x=60).
            const float lx = rx + slideX;
            if (g_font.ready()) {
                // Drop shadow (LAB_0043d185: FUN_00428140 at +_DAT_005cc31c=3.0f,
                // color 0xff000000) then the base color.
                const float sh = 3.0f * kVScale;
                // The record's y IS the original's anchor (FUN_0043c5b0 passes
                // it to FUN_00428140 raw); DrawMashedString applies the
                // FUN_00427680 anchor law (top = y - 0.4954*cell), which lands
                // the cell on the bit-verified plate span (#10) by itself.
                const float ty = rec.y * kVScale;
                // Only BACK/HEADER rows get the +3 black shadow (LAB_0043d185);
                // list items draw the single black pass (FUN_00428140, no shadow).
                if (is_back) {
                    DrawMashedString(txt, lx + sh, ty + sh, text_h, 0xff000000u, true);
                }
                // FUN_00428140 vertical alpha law: top = min(param_7, 0xff),
                // bottom = 0 (p7<=0x80) / (p7+0x80)&0xff (p7<0x180) / 0xff.
                // SETTLED items run p7 >= 0x180 -> bottom 0xff: SOLID, no
                // gradient (pixel-verified vs orig f1900 "Exit To Windows" —
                // the old hardcoded 0.5 washed out every glyph's lower half;
                // the gradient only exists mid fade-in). Disabled rows keep
                // the landed #18 half-fade (their exact p7 is unverified).
                const float gfrac = disabled ? 0.5f : 1.0f;
                DrawMashedString(txt, lx, ty, text_h, base_argb, true, gfrac);
            } else if (rec.row_index >= 0 && rec.row_index < 8 &&
                       g_menu_items[rec.row_index].ready) {
                const MenuItemTex& it = g_menu_items[rec.row_index];
                const float w = static_cast<float>(it.w), h = static_cast<float>(it.h);
                HudIm2DQuad(it.handle, (800.f - w) * 0.5f, ry, w, h, base_argb, uv_full);
            }

            // Sound-screen value widgets — VERBATIM geometry from the original's
            // screen-19 draw stream (log/sound_draw.json, 2026-06-12). Per
            // volume row (640x480 virtual): left arrow 16x16 at x=356; bar bg
            // x=374 w=106 h=16 color 0x7f000000; four 3px black borders
            // (ff000000) top/bottom/left/right; orange fill 0xffd88020 at x=377
            // y+3 w=(value/10*100) h=10; right arrow 16x16 at x=484. The Insults
            // row (3) has NO bar — only the two arrows + a text value (Off/Auto/
            // Manual). Bar Y is the row's plate-center band (the original aligns
            // the 16px bar to y=187 for the y=183 plate, i.e. plate_top+4).
            // Value widgets on the settings screens. Geometry verbatim from the
            // originals' draw streams (log/sound_draw.json + screens 30/32 probe,
            // 2026-06-12): a 16px bar at x=374 w=106 with 3px black borders and an
            // orange fill (0xffd88020), framed by 16x16 black arrow sprites at
            // x=356 / x=484 (sliders) or x=356 / x=411 (compact toggles, no bar).
            // (Value widgets moved to the POST-LOOP pass below: the original
            // draws them after the whole menu loop — clean scr19 baseline has
            // the widget rows A[110+] following the full plate stack.)
        }

        // Widget pass — runs AFTER the record walk, like the original's
        // per-screen content call (the FUN_00492e90 dispatcher invokes the
        // widget drawers after FUN_0043c5b0 returns).
        for (int r = 0; r < nrec; ++r) {
            const MenuRecord& rec = recs[r];
            const std::uint32_t wtag = static_cast<std::uint32_t>(rec.tag);
            // Footer prompt-strip rows reuse row_index 0..3 — they are not
            // widget rows (the record walk `continue`s them before widgets).
            if (wtag == 0xff100000u || wtag == 0xff110000u ||
                wtag == 0xff230000u)
                continue;
            const bool is_back = (wtag == 0xff000000u);
            const float slideX = static_cast<float>(Nav_RecordSlide(r)) * (260.f / 511.f);
            const int sid = Nav_ScreenId();
            const bool is_slider_screen = (sid == 19 || sid == 30);
            const bool is_toggle_screen = (sid == 32);
            if ((is_slider_screen || is_toggle_screen) && !is_back &&
                rec.row_index >= 0 && rec.row_index <= 3) {
                const float S = kVScale;
                const float by = (rec.y - 13.0f + 4.0f + 1.0f) * S; // plate band
                // Which kind of widget is THIS row?
                //   screen 19: rows 0-2 sliders, row 3 insults tri-state text
                //   screen 30: row 0 gamma slider
                //   screen 32: row 0 autosave On/Off toggle
                enum { kSlider, kTriText, kToggle, kNone } kind = kNone;
                int value = 0;
                if (sid == 19) {
                    if (rec.row_index <= 2) {
                        kind = kSlider;
                        value = (rec.row_index == 0) ? g_settings.music
                              : (rec.row_index == 1) ? g_settings.sfx
                                                     : g_settings.insults;
                    } else if (rec.row_index == 3) { kind = kTriText; }
                } else if (sid == 30 && rec.row_index == 0) {
                    kind = kSlider; value = g_settings.gamma;
                } else if (sid == 32 && rec.row_index == 0) {
                    kind = kToggle;
                }
                // Arrows modulate BLACK in the original (clean scr19/30/32
                // baselines: col=ff000000 at the sprite emitters 0x473c19/
                // 0x4739e7), and the stream order per row is LEFT arrow ->
                // bar pieces -> fill -> RIGHT arrow (A[110..117] scr19).
                // Tri-state rows put the right arrow at x=457 (A[135], after
                // the value text zone); toggles at 411 (scr32 probe).
                const float rax = (kind == kToggle  ? 411.0f
                                 : kind == kTriText ? 457.0f : 484.0f) * S;
                const float ay = by, asz = 16.0f * S;
                std::uint32_t uvL[4] = { 0x3f800000u, 0u, 0u, 0x3f800000u }; // U flipped
                std::uint32_t uvR[4] = { 0u, 0u, 0x3f800000u, 0x3f800000u }; // normal
                if (kind != kNone) {
                    if (g_menu_arrow_ready) {
                        HudIm2DQuad(kHandleMenuArrow, 356.0f * S + slideX, ay, asz, asz,
                                    0xff000000u, uvL);
                    } else if (g_font.ready()) {
                        DrawMashedString(L"\x3c", 356.0f * S + slideX, rec.y * S, asz, 0xff000000u, true);
                    }
                }
                if (kind == kSlider) {
                    // #16: lerp the drawn fill toward the target so the bar slides
                    // continuously rather than snapping between the 10 steps.
                    const int di = (sid == 30) ? 3 : rec.row_index;  // 0/1/2=mus/sfx/ins, 3=gamma
                    float disp = static_cast<float>(value);
                    if (di >= 0 && di < 4) {
                        g_sliderDisp[di] += (disp - g_sliderDisp[di]) * 0.28f;
                        if (std::fabs(disp - g_sliderDisp[di]) < 0.02f) g_sliderDisp[di] = disp;
                        disp = g_sliderDisp[di];
                    }
                    const float bx = 374.0f * S + slideX;
                    const float bw = 106.0f * S, bh = 16.0f * S, bt = 3.0f * S;
                    // Inner geometry pinned to the clean scr19/scr30 baselines:
                    // bottom border sits at bar+14 (A y=261 for bar 247) and
                    // the fill at bar+4 (A y=191/236) — one px below the old
                    // +3-based placement.
                    HudIm2DQuad(0, bx, by, bw, bh, 0x7f000000u, uv_full);      // bg
                    HudIm2DQuad(0, bx, by, bw, bt, 0xff000000u, uv_full);      // top
                    HudIm2DQuad(0, bx, by + bh - bt + 1.0f * S, bw, bt, 0xff000000u, uv_full); // bottom
                    HudIm2DQuad(0, bx, by, bt, bh, 0xff000000u, uv_full);      // left
                    HudIm2DQuad(0, bx + bw - bt, by, bt, bh, 0xff000000u, uv_full); // right
                    HudIm2DQuad(0, 377.0f * S + slideX, by + 4.0f * S,
                                (disp / 10.0f) * 100.0f * S,
                                10.0f * S, 0xff2080d8u, uv_full);              // fill
                } else if (kind == kTriText && g_font.ready()) {
                    static const wchar_t* kInsults[3] = { L"Off", L"Auto", L"Manual" };
                    const int m = (g_settings.insults_on >= 0 &&
                                   g_settings.insults_on <= 2)
                                      ? g_settings.insults_on : 0;
                    DrawMashedString(kInsults[m], 392.0f * S + slideX, rec.y * S,
                                     16.0f * S, 0xff000000u, false);
                } else if (kind == kToggle && g_font.ready()) {
                    DrawMashedString(g_settings.autosave ? L"On" : L"Off",
                                     380.0f * S + slideX, rec.y * S, 16.0f * S,
                                     0xff000000u, false);
                }
                // RIGHT arrow last (the original's per-row stream order).
                if (kind != kNone) {
                    if (g_menu_arrow_ready) {
                        HudIm2DQuad(kHandleMenuArrow, rax + slideX, ay, asz, asz,
                                    0xff000000u, uvR);
                    } else if (g_font.ready()) {
                        DrawMashedString(L"\x3e", rax + slideX, rec.y * S, asz, 0xff000000u, true);
                    }
                }
            }
        }

        // --- #25 Player Color Select content (nav screen 4 = kT4). CORRECTED
        // 2026-06-12: the color-select screen is kT4, whose back-row title id
        // 0x130 = "Player Color Select" in the table the game uses (Font36.piz/
        // USA.DAT) — the earlier move to screen 7 was based on the loose
        // English.dat where 0x130 is a different (prompt) string, so in the REAL
        // flow (which reaches screen 4) the cars never drew -> "empty" (user #25).
        // kT4's single item row is prim_id==-1 (the preview placeholder); the
        // original fills it via FUN_004368e0 drawing each active player's
        // car-color preview (FUN_0042fab0(carIdx) -> INTERFACE.TXD NFL* sprite)
        // with "vs"(&DAT_005cda30) separators. Representative 2-player layout
        // (FUN_004739f0 coords, 640x480 virtual -> *kVScale): P1 car @ (40,292)
        // 112^2, "vs" @ (132,300) 88^2, P2 car @ (196,292) 112^2. The title
        // "Player Color Select" renders via the back-row record (0x130). [residual:
        // full per-controller layout needs the absent MP controller/team state.]
        // #25 REBUILT to the clean scr4 baseline (verify/parity_caps/
        // orig_scr4_*.bmp + log/parity_burst_scr4.json — the first unpolluted
        // capture of the real Player Colour Select):
        //   * SIX colour tiles in a row: 64x64 car sprite (white modulate,
        //     emitter 0x433c9b sprite pipe) at (146 + i*70, 125) with a 69x20
        //     colour SWATCH (emitter 0x433c70) at (143 + i*70, 168). Swatch
        //     device colors ff983a3d/ff4e89ae/ff617656/ffdbc362/ffeba7a7/
        //     ff000000 == the first six NFL car liveries (Red, Bluejay, Melon,
        //     Gold, Pink, Shadow) — args below are the packed pre-swap forms.
        //   * THREE full-width player rows (emitters 0x42be53/8c/b4/dc/f21):
        //     plate 534x26 at (48, 193/227/261) fill device 7ff06e14 + four
        //     b45010 border pieces (left/top/bottom 538x2/right).
        // The old P1-vs-P2 layout was invented from the English.dat-era
        // misread; it never matched the original. [Residual: row/tile TEXT
        // (player names, ids) + selection state need the case-10 decomp.]
        if (Nav_ScreenId() == 4 && g_carsel_ready) {
            const std::uint32_t white = 0xffffffffu;
            static const std::uint32_t kSwatch[6] = {
                0xff3d3a98u,  // device ff983a3d (NFLRed)
                0xffae894eu,  // device ff4e89ae (NFLBluejay)
                0xff567661u,  // device ff617656 (NFLMelon)
                0xff62c3dbu,  // device ffdbc362 (NFLGold)
                0xffa7a7ebu,  // device ffeba7a7 (NFLPink)
                0xff000000u,  // device ff000000 (NFLShadow)
            };
            for (int i = 0; i < 6; ++i) {
                // Baseline stream order per tile: SWATCH then sprite
                // (A[81] swatch / A[82] sprite).
                HudIm2DQuad(0, (143.0f + 70.0f * i) * kVScale,
                            168.0f * kVScale, 69.0f * kVScale, 20.0f * kVScale,
                            kSwatch[i], uv_full);
                HudIm2DQuad(kHandleCar0 + i, (146.0f + 70.0f * i) * kVScale,
                            125.0f * kVScale,
                            64.0f * kVScale, 64.0f * kVScale, white, uv_full);
            }
            for (int r = 0; r < 3; ++r) {
                const float ry4 = (193.0f + 34.0f * r) * kVScale;
                const float bt2 = 2.0f * kVScale;
                HudIm2DQuad(0, 48.0f * kVScale, ry4, 534.0f * kVScale,
                            26.0f * kVScale, 0x7f146ef0u, uv_full);   // plate
                HudIm2DQuad(0, 46.0f * kVScale, ry4, bt2,
                            26.0f * kVScale, 0xff1050b4u, uv_full);   // left
                HudIm2DQuad(0, 46.0f * kVScale, ry4, 538.0f * kVScale,
                            bt2, 0xff1050b4u, uv_full);               // top
                HudIm2DQuad(0, 46.0f * kVScale, ry4 + 24.0f * kVScale,
                            538.0f * kVScale, bt2, 0xff1050b4u, uv_full); // bottom
                HudIm2DQuad(0, 582.0f * kVScale, ry4, bt2,
                            26.0f * kVScale, 0xff1050b4u, uv_full);   // right
            }
        }

        // --- bottom prompt strip: F4 CLOSED 2026-06-11. FUN_00432b30 is now
        // VERBATIM-ported in MenuNavSM.cpp (PromptStripAppend; jump tables from
        // binary dwords 0x433060/0x433088..178; C4 twin diff GREEN 264/264) and
        // its rows are appended to g_records by the Nav() Phase-7 call sites.
        // The rows render in the record walk above under the bit-verified
        // prim/sec type rule — nothing left to draw here. (The 2026-06-10
        // "Français" misrender was the 0xff080000 SCREEN-KIND code being
        // decoded as a string id; the kind now indexes the ported key tables
        // exactly as the original does.)
    }

    // MASHED logo (Font36.piz/MASHEDNEWLOGO.PNG) — FUN_00428760 sprite pipe,
    // drawn at virtual (80, 80, 480x240) full-white, after the menu loop.
    // TITLE ONLY (clean-baseline correction 2026-06-12): the frame dispatcher
    // FUN_00492e90 calls the title layer FUN_00403050 (logo + press-button)
    // iff the current screen global DAT_0067ecb0 == 0x21 — the title IS nav
    // screen 33. The earlier "logo draws on menu screens too" conclusion came
    // from a POLLUTED capture: the synthetic nav-push left DAT_0067ecb0 at
    // 0x21, so the title layer kept compositing over the pushed menu. The
    // clean baseline (harness fix + verify/orig_backbuffer_f2100.bmp) shows
    // NO logo at settled scr1. Our phase 1 == the title (screen 33).
    if (g_frontend_phase == 1 &&
        g_bridge_installed && g_menu_logo_ready && g_menu_logo_w > 0 && g_menu_logo_h > 0) {
        HudIm2DQuad(kHandleMenuLogo, 80.f * 1.25f, 80.f * 1.25f,
                    480.f * 1.25f, 240.f * 1.25f, 0xffffffffu, uv_full);
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

    // #8 confirm-dialog modal (VERBATIM layout from FUN_00433f40, pool0 decomp
    // 2026-06-12): drawn on top of the frozen menu when active. Three stacked
    // panels (black top 130,120,380x42 / dark-gray 0x202020 body 130,162,380x166
    // / black bottom 130,328,380x32) + white border, title 'MASHED' (id 0x41)
    // @140,142 scale 0.8, body text centered, button (id) @140,345 scale 0.4.
    // Alpha fades in (DAT_0067eab8 ramp). Coords virtual 640x480 -> x1.25.
    if (g_bridge_installed && g_modal_step != 0) {
        if (g_modal_alpha < 0xff) { g_modal_alpha += 0x11; if (g_modal_alpha > 0xff) g_modal_alpha = 0xff; }
        // #18/#19: the 'Saving game data.' step auto-advances to 'Save successful.'
        if (g_modal_step == 21 && g_modal_timer_ms != 0 &&
            GetTickCount() - g_modal_timer_ms > 800u) {
            ModalGo(22); g_modal_alpha = 0xff;
        }
        const std::uint32_t A = static_cast<std::uint32_t>(g_modal_alpha) << 24;
        const float S = 1.25f;
        std::uint32_t uvf[4] = { 0u, 0u, 0x3f800000u, 0x3f800000u };
        // #8: DARKEN the rest of the UI behind the modal (full-screen black at
        // ~65% of the fade alpha) so the dialog reads as modal.
        const std::uint32_t dimA =
            (static_cast<std::uint32_t>(g_modal_alpha * 0xa6 / 0xff) << 24);
        HudIm2DQuad(0, 0.f, 0.f, 800.f, 600.f, dimA, uvf);
        HudIm2DQuad(0, 130.f*S, 120.f*S, 380.f*S, 42.f*S,  A | 0x000000u, uvf); // top black
        HudIm2DQuad(0, 130.f*S, 162.f*S, 380.f*S, 166.f*S, A | 0x202020u, uvf); // body gray
        HudIm2DQuad(0, 130.f*S, 328.f*S, 380.f*S, 32.f*S,  A | 0x000000u, uvf); // bottom black
        // white border (1.5px) around 130,120 .. 510,360
        const std::uint32_t W = A | 0xffffffu; const float bx=130.f*S, by=120.f*S, bw=380.f*S, bh=240.f*S, t=1.5f*S;
        HudIm2DQuad(0, bx, by, bw, t, W, uvf); HudIm2DQuad(0, bx, by+bh-t, bw, t, W, uvf);
        HudIm2DQuad(0, bx, by, t, bh, W, uvf); HudIm2DQuad(0, bx+bw-t, by, t, bh, W, uvf);
        if (g_font.ready()) {
            wchar_t tt[64];
            if (GetMenuMessage(0x41, tt, 64) > 0)       // title "MASHED"
                DrawMashedString(tt, 320.f*S, 140.f*S, 0.8f*0.0708f*480.f*S, W, false);
            // Body: word-wrapped + centered (the warning strings span 2-3 lines).
            wchar_t bt[160];
            if (GetMenuMessage(g_modal_body_id, bt, 160) > 0) {
                const float bh2 = 0.6f*0.0708f*480.f*S;
                DrawWrappedCentered(bt, 320.f*S, 195.f*S, bh2, bh2 + 6.f*S, W, 34);
            }
            // Buttons row (~y=340): Yes/No for confirm steps, else Continue.
            const float byb = 340.f*S, bhh = 0.5f*0.0708f*480.f*S;
            if (g_modal_yesno) {
                wchar_t y1[32], n1[32];
                if (GetMenuMessage(0x2e, y1, 32) > 0)   // "(glyph) Yes"
                    DrawMashedString(y1, 250.f*S, byb, bhh, W, false);
                if (GetMenuMessage(0x2f, n1, 32) > 0)   // "(glyph) No"
                    DrawMashedString(n1, 390.f*S, byb, bhh, W, false);
            } else {
                wchar_t cb[48];
                if (GetMenuMessage(g_modal_button_id, cb, 48) > 0) // "(glyph) Continue"
                    DrawMashedString(cb, 320.f*S, byb, bhh, W, false);
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

// R2-5: load sfx.piz/BADGES.TXD (the named-sprite dictionary MASHED loads at
// FUN_0040bbb0 via FUN_0042a6b0("badges.txd",0,0) into DAT_0063b8fc), decode it
// with Txd::Dictionary, and upload the "Button" texture (16x32 PAL8 — the
// rounded left cap of the menu highlight bar, SpriteLookupC name @0x005cda7c)
// into kSlotMenuBadge / kHandleMenuBadge. The Archive and Dictionary are local:
// UploadFromTextureToSlot copies the pixels into the D3D9 texture in-scope.
// F2: load the 24 track-preview textures (slot table 0x005f79d8 names) from
// SFX.piz/TRACKIMAGES.TXD into slots 20..43 / bridge handles 10..33.
bool LoadTrackPreviews() {
    static const char* kNames[24] = {
        "Training1","Training2","Egypt1","Egypt2","Neustein1","Neustein2",
        "Timgidski1","Timgidski2","Highway1","Highway2","KeisterBay1",
        "KeisterBay2","SuperG1","SuperG2","TierraPiedra1","TierraPiedra2",
        "Storm1","Storm2","Forest1","Forest2","Landfill1","Landfill2",
        "Nukov1","Nukov2"};
    mashed_re::Piz::Archive piz;
    if (!piz.Load("original/TOASTART/Common/sfx.piz")) return false;
    const std::uint8_t* blob = nullptr;
    std::uint32_t blen = 0;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (_stricmp(piz.entry(i).name, "TRACKIMAGES.TXD") == 0) {
            blob = piz.blob(i, &blen);
            break;
        }
    }
    if (!blob) return false;
    mashed_re::Txd::Dictionary dict;
    if (!dict.Decode(blob, blen)) return false;
    int loaded = 0;
    for (int n = 0; n < 24; ++n) {
        for (std::uint32_t i = 0; i < dict.count(); ++i) {
            const auto& tex = dict.texture(i);
            if (_stricmp(tex.name, kNames[n]) != 0) continue;
            const std::uint32_t slot = kSlotPreview0 + static_cast<std::uint32_t>(n);
            if (g_quad_renderer.UploadFromTextureToSlot(slot, tex)) {
                mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                    kHandlePreview0 + n, g_quad_renderer.slot_texture(slot));
                ++loaded;
            }
            break;
        }
    }
    std::FILE* lf = std::fopen(kLogPath, "a");
    if (lf) { std::fprintf(lf, "F2 track previews: %d/24 loaded\n", loaded); std::fclose(lf); }
    return loaded > 0;
}

bool LoadBadgeSprites() {
    std::FILE* log = std::fopen(kLogPath, "a");
    mashed_re::Piz::Archive piz;
    if (!piz.Load("original/TOASTART/Common/sfx.piz")) {
        if (log) { std::fprintf(log, "\nR2-5: sfx.piz load FAILED: %s\n", piz.last_error()); std::fclose(log); }
        return false;
    }
    const std::uint8_t* blob = nullptr;
    std::uint32_t blen = 0;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (std::strcmp(piz.entry(i).name, "BADGES.TXD") == 0) {
            blob = piz.blob(i, &blen);
            break;
        }
    }
    if (!blob) {
        if (log) { std::fprintf(log, "\nR2-5: BADGES.TXD not found in sfx.piz\n"); std::fclose(log); }
        return false;
    }
    mashed_re::Txd::Dictionary dict;
    if (!dict.Decode(blob, blen)) {
        if (log) { std::fprintf(log, "\nR2-5: BADGES.TXD decode FAILED: %s\n", dict.last_error()); std::fclose(log); }
        return false;
    }
    // DEBUG (MASHED_DBG_DUMPBADGE=1): dump Button/SemiC decoded RGBA to a raw
    // file so the sprite content (esp. alpha) is verifiable offline — pins
    // whether the cap sprite decodes as the expected black ◖ shape (#13).
    if (GetEnvironmentVariableA("MASHED_DBG_DUMPBADGE", nullptr, 0) != 0) {
        for (std::uint32_t i = 0; i < dict.count(); ++i) {
            const auto& t = dict.texture(i);
            if (std::strcmp(t.name,"Button") && std::strcmp(t.name,"SemiC") &&
                std::strcmp(t.name,"SemiC2")) continue;
            const auto fmt = t.format(); const auto& mp = t.mips[0];
            char path[160];
            std::snprintf(path, sizeof(path), "verify/badge_%s.raw", t.name);
            if (std::FILE* bf = std::fopen(path, "wb")) {
                std::fprintf(bf, "P7 %u %u\n", t.width(), t.height()); // w h header
                for (std::uint32_t y=0; y<mp.height; ++y)
                for (std::uint32_t x=0; x<mp.width; ++x) {
                    std::uint8_t rgba[4]={0,0,0,0};
                    if (fmt==mashed_re::Txd::PixelFormat::ARGB8888) {
                        const std::uint8_t* s=mp.pixels+(std::size_t)y*mp.stride+x*4;
                        rgba[0]=s[0];rgba[1]=s[1];rgba[2]=s[2];rgba[3]=s[3];
                    } else if (fmt==mashed_re::Txd::PixelFormat::Paletted8 && mp.palette) {
                        std::uint8_t idx=mp.pixels[(std::size_t)y*mp.stride+x];
                        const std::uint8_t* p=mp.palette+idx*4;
                        rgba[0]=p[0];rgba[1]=p[1];rgba[2]=p[2];rgba[3]=p[3];
                    }
                    std::fwrite(rgba,1,4,bf);
                }
                std::fclose(bf);
                if (log) std::fprintf(log, "DBG dumped %s %ux%u fmt=%s -> %s\n",
                    t.name, t.width(), t.height(),
                    mashed_re::Txd::PixelFormatName(fmt), path);
            }
        }
    }
    bool ok = false;
    for (std::uint32_t i = 0; i < dict.count(); ++i) {
        const auto& tex = dict.texture(i);
        if (std::strcmp(tex.name, "Button") == 0) {
            ok = g_quad_renderer.UploadFromTextureToSlot(kSlotMenuBadge, tex);
            if (ok) {
                mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                    kHandleMenuBadge, g_quad_renderer.slot_texture(kSlotMenuBadge));
            }
            if (log) {
                std::fprintf(log, "\nR2-5: badges.txd 'Button' %ux%u upload %s "
                             "(dict has %u textures)\n",
                             tex.width(), tex.height(), ok ? "OK" : "FAILED",
                             dict.count());
            }
            break;
        }
    }
    // #17: also upload the "Arrow" sprite (slider/toggle side arrows).
    for (std::uint32_t i = 0; i < dict.count(); ++i) {
        const auto& tex = dict.texture(i);
        if (std::strcmp(tex.name, "Arrow") != 0) continue;
        if (g_quad_renderer.UploadFromTextureToSlot(kSlotMenuArrow, tex)) {
            mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                kHandleMenuArrow, g_quad_renderer.slot_texture(kSlotMenuArrow));
            g_menu_arrow_ready = true;
        }
        if (log) std::fprintf(log, "R2-5: badges.txd 'Arrow' %ux%u upload %s\n",
                              tex.width(), tex.height(), g_menu_arrow_ready ? "OK" : "FAILED");
        break;
    }
    if (log) std::fclose(log);
    return ok;
}

// #25: load SFX.piz/INTERFACE.TXD (the global frontend sprite dict
// DAT_0063b904 that FUN_0040bb90/FUN_004c5c00 resolve named sprites from) and
// upload the 10 NFL* car-color previews + the "vs" separator into the bridge.
// These are the sprites FUN_0042fab0(carIdx)/&DAT_005cda30 hand to FUN_004739f0
// on the Player Color Select screen (nav screen 4 / FUN_004368e0 LAB_004375d5).
bool LoadCarColorSprites() {
    std::FILE* log = std::fopen(kLogPath, "a");
    mashed_re::Piz::Archive piz;
    if (!piz.Load("original/TOASTART/Common/sfx.piz")) {
        if (log) { std::fprintf(log, "\n#25: sfx.piz load FAILED: %s\n", piz.last_error()); std::fclose(log); }
        return false;
    }
    const std::uint8_t* blob = nullptr;
    std::uint32_t blen = 0;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (_stricmp(piz.entry(i).name, "INTERFACE.TXD") == 0) {
            blob = piz.blob(i, &blen);
            break;
        }
    }
    if (!blob) {
        if (log) { std::fprintf(log, "\n#25: INTERFACE.TXD not found in sfx.piz\n"); std::fclose(log); }
        return false;
    }
    mashed_re::Txd::Dictionary dict;
    if (!dict.Decode(blob, blen)) {
        if (log) { std::fprintf(log, "\n#25: INTERFACE.TXD decode FAILED: %s\n", dict.last_error()); std::fclose(log); }
        return false;
    }
    int cars = 0;
    for (int n = 0; n < 10; ++n) {
        for (std::uint32_t i = 0; i < dict.count(); ++i) {
            const auto& tex = dict.texture(i);
            if (_stricmp(tex.name, kCarColorNames[n]) != 0) continue;
            const std::uint32_t slot = kSlotCar0 + static_cast<std::uint32_t>(n);
            if (g_quad_renderer.UploadFromTextureToSlot(slot, tex)) {
                mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                    kHandleCar0 + n, g_quad_renderer.slot_texture(slot));
                ++cars;
            }
            break;
        }
    }
    bool vs_ok = false;
    for (std::uint32_t i = 0; i < dict.count(); ++i) {
        const auto& tex = dict.texture(i);
        if (_stricmp(tex.name, "vs") != 0) continue;
        if (g_quad_renderer.UploadFromTextureToSlot(kSlotVs, tex)) {
            mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(
                kHandleVs, g_quad_renderer.slot_texture(kSlotVs));
            vs_ok = true;
        }
        break;
    }
    if (log) {
        std::fprintf(log, "\n#25 color-select: INTERFACE.TXD cars %d/10, vs %s "
                     "(dict has %u textures)\n", cars, vs_ok ? "OK" : "MISSING",
                     dict.count());
        std::fclose(log);
    }
    return cars > 0;
}

// B19a: load a PNG asset from a .piz by entry name, decode it via WIC, upload to
// a QuadRenderer slot, and register it with the RW Im2D bridge under `handle` so
// HudIm2DQuad(handle, ...) draws it. Writes the decoded dims on success. The piz
// is loaded into a local Archive (freed on return) — the pixels are copied into
// the D3D9 texture before that, so the blob lifetime is fine.
bool LoadPngAssetToSlot(const char* piz_path, const char* entry_name,
                        std::uint32_t slot, int handle,
                        std::uint32_t* out_w, std::uint32_t* out_h) {
    std::FILE* log = std::fopen(kLogPath, "a");
    mashed_re::Piz::Archive piz;
    if (!piz.Load(piz_path)) {
        if (log) { std::fprintf(log, "\nB19a: %s load FAILED: %s\n", piz_path, piz.last_error()); std::fclose(log); }
        return false;
    }
    std::uint32_t idx = UINT32_MAX;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        if (n && std::strcmp(n, entry_name) == 0) { idx = i; break; }
    }
    if (idx == UINT32_MAX) {
        if (log) { std::fprintf(log, "\nB19a: %s not found in %s\n", entry_name, piz_path); std::fclose(log); }
        return false;
    }
    std::uint32_t blen = 0;
    const std::uint8_t* blob = piz.blob(idx, &blen);
    if (!blob || blen == 0) {
        if (log) { std::fprintf(log, "\nB19a: %s blob empty\n", entry_name); std::fclose(log); }
        return false;
    }
    std::uint32_t w = 0, h = 0;
    std::uint8_t* bgra = mashed_re::D3d9Render::DecodeImageToBGRA(blob, blen, &w, &h);
    if (!bgra) {
        if (log) { std::fprintf(log, "\nB19a: WIC decode FAILED for %s (%u bytes)\n", entry_name, blen); std::fclose(log); }
        return false;
    }
    bool ok = g_quad_renderer.UploadBGRAToSlot(slot, w, h, bgra);
    std::free(bgra);
    if (ok) {
        mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(handle, g_quad_renderer.slot_texture(slot));
        if (out_w) *out_w = w;
        if (out_h) *out_h = h;
    }
    if (log) {
        std::fprintf(log, "\nB19a: %s :: %s decode+upload %s (%ux%u) -> slot %u handle %d\n",
                     piz_path, entry_name, ok ? "OK" : "FAILED", w, h, slot, handle);
        std::fclose(log);
    }
    return ok;
}

// B19b: load the language message table (.DAT) from a .piz into a persistent
// buffer (kept for the program lifetime so message pointers stay valid).
bool LoadMessageTable(const char* piz_path, const char* entry_name) {
    mashed_re::Piz::Archive piz;
    if (!piz.Load(piz_path)) return false;
    std::uint32_t idx = UINT32_MAX;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        if (n && std::strcmp(n, entry_name) == 0) { idx = i; break; }
    }
    if (idx == UINT32_MAX) return false;
    std::uint32_t blen = 0;
    const std::uint8_t* blob = piz.blob(idx, &blen);
    if (!blob || blen < 8) return false;
    g_msg_dat = static_cast<std::uint8_t*>(std::malloc(blen));
    if (!g_msg_dat) return false;
    std::memcpy(g_msg_dat, blob, blen);
    g_msg_dat_len = blen;
    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nB19b: message table %s/%s loaded (%u bytes)\n",
                     piz_path, entry_name, blen);
        std::fclose(log);
    }
    return true;
}

// B19b: resolve a message id to its UTF-16 text. Faithful to FUN_00427780:
//   record = base + *(u32*)(base + id*4); record = [u16 len][len * u16 chars].
// Copies up to cap-1 chars into out (null-terminated). Returns the char count.
int GetMenuMessage(int id, wchar_t* out, int cap) {
    if (out && cap > 0) out[0] = 0;
    if (!g_msg_dat || id < 0 || cap <= 0) return 0;
    const std::uint32_t off_pos = static_cast<std::uint32_t>(id) * 4u;
    if (off_pos + 4 > g_msg_dat_len) return 0;
    std::uint32_t off;
    std::memcpy(&off, g_msg_dat + off_pos, 4);
    if (off + 2 > g_msg_dat_len) return 0;
    std::uint16_t len;
    std::memcpy(&len, g_msg_dat + off, 2);
    if (off + 2 + static_cast<std::uint32_t>(len) * 2u > g_msg_dat_len) return 0;
    int n = 0;
    for (int i = 0; i < len && n < cap - 1; ++i) {
        std::uint16_t ch;
        std::memcpy(&ch, g_msg_dat + off + 2 + i * 2, 2);
        // Item 12 fix: apply the FUN_004277a0 control-code -> FGDC20 nav-glyph
        // remap (verbatim, pool0 decomp 2026-06-12). USA.DAT prompt strings
        // embed control chars 8..0xe that the original rewrites to the extended
        // glyph slots before drawing; without this the nav arrow (8/0xa -> 0x81)
        // and friends rendered as missing/blank (the "select-arrow missing").
        switch (ch) {
            case 0x08: ch = 0x81; break;  // nav arrow
            case 0x09: ch = 0x7f; break;
            case 0x0a: ch = 0x81; break;  // nav arrow
            case 0x0b: ch = 0x8d; break;
            case 0x0c: ch = 0x80; break;
            case 0x0d: ch = 0x87; break;
            case 0x0e: ch = 0x8f; break;
            default: break;
        }
        out[n++] = static_cast<wchar_t>(ch);
    }
    out[n] = 0;
    return n;
}

// B19b: rasterize the main-menu item strings (real message ids) into textures and
// register them with the bridge. Called once after the assets + bridge are up.
void LoadMenuItems() {
    // Main-menu options (DEFINES.TXT ids): Single Player / Multiplayer /
    // Championship / Time Attack / Options. Real strings from the language table.
    static const int kMenuIds[] = { 33, 34, 35, 36, 39 };
    std::FILE* log = std::fopen(kLogPath, "a");
    g_menu_item_count = 0;
    for (int i = 0; i < static_cast<int>(sizeof(kMenuIds) / sizeof(kMenuIds[0])); ++i) {
        wchar_t txt[128];
        int n = GetMenuMessage(kMenuIds[i], txt, 128);
        if (n <= 0) continue;
        std::uint32_t w = 0, h = 0;
        std::uint8_t* bgra = mashed_re::D3d9Render::RenderTextToBGRA(txt, kMenuItemFontPx, &w, &h);
        if (!bgra) continue;
        const std::uint32_t slot = kSlotMenuItem0 + g_menu_item_count;
        const int handle = kHandleMenuItem0 + static_cast<int>(g_menu_item_count);
        bool ok = g_quad_renderer.UploadBGRAToSlot(slot, w, h, bgra);
        std::free(bgra);
        if (!ok) continue;
        mashed_re::D3d9Render::RwIm2DBridge_RegisterTexture(handle, g_quad_renderer.slot_texture(slot));
        // Keep the decoded string for faithful-font per-frame drawing.
        int c = 0;
        for (; c < n && c < 63; ++c) g_menu_msgs[g_menu_item_count][c] = txt[c];
        g_menu_msgs[g_menu_item_count][c] = 0;
        g_menu_items[g_menu_item_count] = { handle, w, h, true };
        if (log) {
            char a[128]; int k = 0;
            for (; k < n && k < 127; ++k) a[k] = (txt[k] < 128) ? static_cast<char>(txt[k]) : '?';
            a[k] = 0;
            std::fprintf(log, "B19b: menu item id=%d \"%s\" %ux%u -> slot %u handle %d\n",
                         kMenuIds[i], a, w, h, slot, handle);
        }
        ++g_menu_item_count;
    }
    if (log) { std::fprintf(log, "B19b: %u menu items ready\n", g_menu_item_count); std::fclose(log); }
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
    // Self-locating asset root. Every data path in this exe (and the log) is
    // relative to the REPO ROOT ("original/TOASTART/..."), so a launch whose
    // cwd is elsewhere (Explorer double-click binds cwd to the exe dir) used
    // to come up asset-less (title 'slot 0/(none)', empty blue panel). If the
    // marker archive is not visible from the inherited cwd, walk up from the
    // exe's own directory (mashedmod\build -> mashedmod -> repo root) until it
    // is, and chdir there. A cwd that already has the assets wins untouched.
    {
        const char kMarker[] = "original\\TOASTART\\Common\\Frontend.piz";
        if (GetFileAttributesA(kMarker) == INVALID_FILE_ATTRIBUTES) {
            char dir[MAX_PATH];
            const DWORD n = GetModuleFileNameA(nullptr, dir, MAX_PATH);
            if (n > 0 && n < MAX_PATH) {
                for (int up = 0; up < 7; ++up) {   // exe name + 6 parent hops
                    char* slash = std::strrchr(dir, '\\');
                    if (!slash) break;
                    *slash = '\0';
                    char probe[MAX_PATH];
                    if (std::snprintf(probe, sizeof(probe), "%s\\%s", dir,
                                      kMarker) < static_cast<int>(sizeof(probe)) &&
                        GetFileAttributesA(probe) != INVALID_FILE_ATTRIBUTES) {
                        SetCurrentDirectoryA(dir);
                        break;
                    }
                }
            }
        }
    }

    // Truncate the log at session start so subsequent fopen "a" calls
    // build a clean per-run trace.
    {
        std::FILE* clr = std::fopen(kLogPath, "w");
        if (clr) std::fclose(clr);
    }

    // Scripted nav-demo verification harness (in-process input only). Enabled
    // via env var MASHED_NAV_DEMO; drives push/pop/cursor + dumps backbuffer BMPs.
    g_nav_demo = (GetEnvironmentVariableA("MASHED_NAV_DEMO", nullptr, 0) != 0);

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
            std::fprintf(log,
                "B17-PAD: image-pad arena [0x%p .. 0x%p] (exe owns the MASHED RVA range)\n",
                static_cast<void*>(&g_b17_low_arena_pad[0]),
                static_cast<void*>(&g_b17_low_arena_pad[sizeof(g_b17_low_arena_pad) - 1]));
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

    // DPI awareness — MUST precede window creation. The compat layer gives
    // MASHED.exe HIGHDPIAWARE (setup_mashed_compat.ps1), so the original
    // renders 1:1 physical pixels. Without the equivalent here, DWM
    // bitmap-stretches our 800x600 window on scaled displays (>100% DPI):
    // the user sees blurred glyphs / fuzzy alpha edges / slight offsets that
    // NO backbuffer-level fix can touch, while the backbuffer dump stays
    // pristine — the exact failure mode of the 2026-06-12 font reports.
    // Per-monitor-v2 when available (multi-monitor correct), else system DPI.
    {
        using SetCtxFn = BOOL(WINAPI*)(HANDLE);
        HMODULE u32 = GetModuleHandleA("user32.dll");
        SetCtxFn setCtx = u32 ? reinterpret_cast<SetCtxFn>(
            GetProcAddress(u32, "SetProcessDpiAwarenessContext")) : nullptr;
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == (HANDLE)-4
        if (!setCtx || !setCtx(reinterpret_cast<HANDLE>(-4))) {
            SetProcessDPIAware();
        }
    }

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

    // B19a: load the real menu background + logo PNGs (Perm.piz/BACKGROUND.PNG,
    // Font36.piz/MASHEDLOGO.PNG) via WIC, upload them, and register them with the
    // bridge. Failure is non-fatal (the chrome + title quad still render). Needs
    // the QuadRenderer initialised (UploadAllTexturesForAtlas already did Init).
    if (g_bridge_installed && g_atlas_slot_count > 0) {
        g_menu_bg_ready = LoadPngAssetToSlot(
            "original/TOASTART/Common/Perm.piz", "BACKGROUND.PNG",
            kSlotMenuBg, kHandleMenuBg, nullptr, nullptr);
        g_menu_logo_ready = LoadPngAssetToSlot(
            "original/TOASTART/Common/Font36.piz", "MASHEDNEWLOGO.PNG",  /* F3: the rainbow title logo (MASHEDLOGO.PNG is the grey in-game variant) */
            kSlotMenuLogo, kHandleMenuLogo, &g_menu_logo_w, &g_menu_logo_h);
        // B19b: load the language message table + rasterize the main-menu items.
        if (LoadMessageTable("original/TOASTART/Common/Font36.piz", "USA.DAT")) {
            LoadMenuItems();
        }
        // R2-5: badge sprites (highlight-bar "Button" cap from badges.txd).
        g_menu_badge_ready = LoadBadgeSprites();
        g_previews_ready = LoadTrackPreviews();   // F2 crossfade textures
        g_carsel_ready = LoadCarColorSprites();   // #25 color-select car previews
        // F1 (frontend-faithful): the real menu backdrop — frontend.mpg via
        // DirectShow into a D3D9 texture (the original's own playback path).
        {
            char verr[128] = {};
            const bool vok = g_menu_video.Open(
                g_device, "original/toastart/pc/movies/frontend.mpg",
                verr, sizeof(verr));
            std::FILE* lf = std::fopen(kLogPath, "a");
            if (lf) {
                std::fprintf(lf, "F1 frontend.mpg: %s%s (%ux%u)\n",
                             vok ? "RUNNING" : "FAILED ",
                             vok ? "" : verr,
                             g_menu_video.width(), g_menu_video.height());
                std::fclose(lf);
            }
        }
        // R2-close: persisted menu settings (Sound screen values).
        LoadMenuSettings();
        // R4: MASHED_TRACK_VIEW=<1 | track name | piz path> -> fly-through.
        // A bare name (no slash, no .piz) resolves to TRACKS/<name>.piz.
        {
            char tv[260] = {};
            if (GetEnvironmentVariableA("MASHED_TRACK_VIEW", tv, sizeof(tv)) > 0) {
                char path[300];
                if (tv[0] == '1' && tv[1] == '\0') {
                    std::snprintf(path, sizeof(path),
                                  "original/TOASTART/TRACKS/Arctic.piz");
                } else if (!std::strchr(tv, '/') && !std::strchr(tv, '\\') &&
                           !std::strstr(tv, ".piz")) {
                    std::snprintf(path, sizeof(path),
                                  "original/TOASTART/TRACKS/%s.piz", tv);
                } else {
                    std::snprintf(path, sizeof(path), "%s", tv);
                }
                CreateDirectoryA("verify\\r4", nullptr);
                g_track_view = g_track.Load(g_device, path, kLogPath);
                // R5: MASHED_CAR=<1 | vehicle piz path> -> spawn the Advantage
                // car (or the named vehicle's *0.DFF) on the collision ground.
                char cv[260] = {};
                if (g_track_view &&
                    GetEnvironmentVariableA("MASHED_CAR", cv, sizeof(cv)) > 0) {
                    CreateDirectoryA("verify\\r5", nullptr);
                    const char* cpiz = (cv[0] == '1' && cv[1] == '\0')
                        ? "original/TOASTART/VEHICLES/Advantag.piz" : cv;
                    g_track.LoadCar(g_device, cpiz, "ADVANTAGE0.DFF", kLogPath);
                    // PORTED selection (FUN_0040d110): same piz, liveries
                    // 1..3 for the AI cars.
                    g_track.LoadCarLiveries(g_device, cpiz, "ADVANTAGE",
                                            kLogPath);
                    // R6: MASHED_ROUND=1 -> first-to-3-rounds match (countdown,
                    // elimination, score, results). MASHED_ROUND=N sets the
                    // round target.
                    char rv[16] = {};
                    if (GetEnvironmentVariableA("MASHED_ROUND", rv, sizeof(rv)) > 0) {
                        CreateDirectoryA("verify\\r6", nullptr);
                        const int n = std::atoi(rv);
                        g_track.StartMatch(n > 1 ? n : 3);
                    }
                }
            }
        }
        // R2-2 save-driven game state: load original/gamesave.bin and replay
        // Save::DeserializeFromBuffer's span restore (FUN_00404e80 step 1) so
        // the state-gated routing/grey-out branches see the real save (unlock
        // arrays, savedata/profile gates). A blank save (magic != 0xDEADBEEF,
        // as shipped) keeps the fresh-menu defaults.
        {
            bool save_ok = false;
            if (std::FILE* sf = std::fopen("original/gamesave.bin", "rb")) {
                static unsigned char buf[0x24FA0];
                const size_t n = std::fread(buf, 1, sizeof(buf), sf);
                std::fclose(sf);
                save_ok = mashed_re::Frontend::Nav_GameStateLoadSave(
                    buf, static_cast<unsigned>(n));
            }
            std::FILE* log = std::fopen(kLogPath, "a");
            if (log) {
                std::fprintf(log, "R2-2 gamesave load: %s\n",
                             save_ok ? "LOADED (DEADBEEF save)"
                                     : "blank/absent -> fresh defaults");
                std::fclose(log);
            }
        }
        // Initialize the ported nav state machine at the root screen (id 0).
        // Analogue of FUN_0043df00's FUN_0043d2a0(0,2) frontend-enter reload.
        // From here the menu is state-machine-driven (push/pop/cursor), replacing
        // the old hand-rolled g_menu_selected clamp.
        mashed_re::Frontend::Nav_Init();
        // Dev capture aid (acceptance side-by-sides): MASHED_GOTO=8,19 pushes
        // the screen chain at boot and skips the title phase. Capture-only.
        {
            char gv[64] = {};
            if (GetEnvironmentVariableA("MASHED_GOTO", gv, sizeof(gv)) > 0) {
                g_frontend_phase = 3;
                LogoOverlayFadeSet(0xff, -1);   // menu entry raises the wash pair
                char* tok = gv;
                while (*tok) {
                    const int sid = std::atoi(tok);
                    mashed_re::Frontend::Nav(sid, mashed_re::Frontend::kNavPush);
                    while (*tok && *tok != ',') ++tok;
                    if (*tok == ',') ++tok;
                }
            }
            // Capture aid: MASHED_DBG_MENU=1 jumps straight to the menu ROOT
            // (phase 3) with NO push — shows the true nav root (now the main
            // menu, screen 1) for verifying the #12 root-cause fix.
            else if (GetEnvironmentVariableA("MASHED_DBG_MENU", nullptr, 0) != 0) {
                g_frontend_phase = 3;
                LogoOverlayFadeSet(0xff, -1);   // menu entry raises the wash pair
            }
            // Capture aid: MASHED_DBG_MODAL=1 shows the Load warning flow, =2 the
            // Save overwrite flow (#18/#19) at boot for verification.
            {
                char mv[8] = {};
                if (GetEnvironmentVariableA("MASHED_DBG_MODAL", mv, sizeof(mv)) > 0) {
                    g_frontend_phase = 3;
                    ModalGo(std::atoi(mv) == 2 ? 20 : 10);
                    g_modal_alpha = 0xff;
                }
            }
        }
        // B19 faithful font: decode FGDC20.TXD atlas + FGDC20.RWF glyph metrics
        // and upload the glyph sheet. When this succeeds the menu draws in MASHED's
        // actual font (glyph-by-glyph); otherwise the GDI/Arial textures are used.
        {
            bool fok = g_font.Load(g_quad_renderer, kSlotMenuFont, kHandleMenuFont,
                                   "original/TOASTART/Common/Font36.piz");
            std::FILE* log = std::fopen(kLogPath, "a");
            if (log) {
                std::fprintf(log, "\nB19 faithful font (FGDC20): load %s (height=%.1f)\n",
                             fok ? "OK" : "FAILED", g_font.natural_height());
                std::fclose(log);
            }
        }
        // Menu id->glyph-string table (sprite-atlas-by-id for menu records). Load
        // the localized labels (English.dat carries the real text; USA.dat is a
        // sparse placeholder) + run a small self-check that known menu ids resolve.
        {
            bool sok = g_menu_str.LoadFile(
                           "original/TOASTART/Common/FONT/English.dat") ||
                       g_menu_str.LoadFile(
                           "original/TOASTART/Common/FONT/USA.dat");
            std::FILE* log = std::fopen(kLogPath, "a");
            if (log) {
                std::fprintf(log,
                    "\nMenuStringTable (id->glyph-string): load %s size=%u ids=%u\n",
                    sok ? "OK" : "FAILED", g_menu_str.size(), g_menu_str.id_count());
                // Self-check: the standalone main-menu record ids (FUN_0043d2a0
                // root table[0] expands to these) must resolve to real labels.
                static const int kCheck[] = { 0x21, 0x22, 0x23, 0x24, 0x27 };
                for (int id : kCheck) {
                    wchar_t w[64];
                    int n = g_menu_str.Decode(id, w, 64);
                    char a[64]; int k = 0;
                    for (; k < n && k < 63; ++k)
                        a[k] = (w[k] >= 32 && w[k] < 127) ? (char)w[k] : '.';
                    a[k] = 0;
                    std::fprintf(log, "  self-check id 0x%x len=%d \"%s\"\n", id, n, a);
                }
                std::fclose(log);
            }
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
    int nav_demo_phase = 0;
    while (!PumpOnce()) {
        PollKeyboard();
        // Scripted nav-demo: inject the next in-process key edge / capture a
        // backbuffer BMP (no-op unless MASHED_NAV_DEMO is set). Runs right after
        // PollKeyboard so injected edges feed UpdateMenuSelection this frame.
        if (g_nav_demo) {
            if (RunNavDemoStep(nav_demo_phase)) { PostQuitMessage(0); }
            ++nav_demo_phase;
        }
        UpdateTitleFromKeyboard();
        // B12: arrow keys translate title quad; Enter resets.
        UpdateQuadFromKeyboard();
        // B13: PgUp/PgDn cycle through uploaded atlas slots; Home -> 0.
        UpdateTitleSlotFromKeyboard();
        // State-machine-driven nav: Up/Down move the cursor, Enter pushes the
        // highlighted item's child screen, Esc/Backspace pop. Returns true only
        // when Esc is pressed at the ROOT (nothing left to pop) -> quit the app.
        if (UpdateMenuSelection()) {
            PostQuitMessage(0);
        }
        // B14: mirror DInput keyboard state into MASHED's input globals
        // (0x007f1044 active / 0x007f1504 processed, player 0). No frontend
        // code currently reads these in standalone, but the wiring exists
        // for B16's frontend_mode_dispatch loop.
        WriteMashedInputGlobalsFromKeyboard();
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
