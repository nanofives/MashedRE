// PORT-OT-BIND-D3D (L-111 step 2): the original's DirectShow movie glue 0x004944c0, bound on the exe's own DirectShow /
// D3D9 layer (D3d9Render/MpegVideoTexture). EXE ONLY, reached on the MASHED_ORIG_TOP path through
// the matched 0x00494a80 (Match/MatchS16_00494a80.cpp). This is a binding, not a body of 0x004944c0.
//
// Identity of 0x004944c0, from its code (re/match/s16/004944c0.c, C2, 59.2%; the GUIDs read from
// original/MASHED.exe.unpatched):
//   CoCreateInstance(0x005d0c4c = CLSID_FilterGraph {e436ebb3-524f-11ce-9f53-0020af0ba770},
//                    0x005d0abc = IID_IGraphBuilder {56a868a9-...}) -> 0x00771a30
//   operator new(0x170) + __thiscall 0x00493c00: the original's texture renderer filter (a CBaseVideoRenderer, vtable
//     at 0x005cf9e8); its IBaseFilter (obj + 0xc) AddRef'd into 0x00771a24 (the old one released first)
//   IGraphBuilder::AddFilter (+0xc) "TEXTURERENDERER", AddSourceFilter (+0x38) on the path [0x006147c4 + idx * 4],
//     IBaseFilter::FindPin (+0x2c) "Output", IGraphBuilder::Render (+0x30) on that pin
//   QueryInterface into 0x00771a34 (0x005d0bcc = IID_IMediaControl {56a868b1-...}), 0x00771a2c (0x005d0bbc =
//     IID_IMediaPosition {56a868b2-...}), 0x00771a28 (0x005cfac4 = IID_IMediaEvent {56a868b6-...}), 0x00771a20
//     (0x005cfad4 = IID_IBasicAudio {56a868b3-...})
//   IMediaControl::Run (+0x1c), then 0x00771a00 = 0, 0x006147d8 = 1, return 0. Every failure returns its HRESULT
//     after a 0x00493f00 message; a failed CoCreateInstance / renderer returns 0x80004005.
//   The renderer's SetMediaType 0x00494820 (re/match/s16/00494820.c), run inside Render: two CreateTexture (device
//     0x00771a0c, vtable +0x5c) of D3DFMT_X8R8G8B8 (0x16) into 0x00771a10 / 0x00771a14, video and texture sizes into
//     the floats 0x007719ec / 0x007719f0 / 0x007719f4 / 0x007719f8 (powers of two when TextureCaps & 2), the format
//     into 0x007719fc.
// No game logic: the only game-side input is the path-table index, stored into 0x00771a1c.
//
// One owner (the exe). The exe's device (g_device) and the exe's graph (MpegVideoTexture: FilterGraph -> SampleGrabber
// RGB32 -> NullRenderer) own everything. The original's renderer object is never constructed, and its device pointer
// 0x00771a0c is neither read nor written. The binding PUBLISHES AddRef'd pointers into the original's globals, so the
// matched consumers work on live objects with balanced references:
//   the event poll 0x004942b0 ( IMediaEvent::GetEvent at 0x00771a28, IMediaPosition::put_CurrentPosition at 0x00771a2c)
//   the close 0x00494320 ( Stop + Release of 0x00771a34, Release of 0x00771a28 / 2c / 30 / 24 / 20, then an unconditional
//     Release of 0x00771a10 and 0x00771a14). Each published pointer carries its own reference, so that close
//     releases exactly what was published, and the exe object keeps its own references until it is closed.
// Port choices, each a difference from the original:
//   - 0x00771a24 holds the SampleGrabber's IBaseFilter (the filter that receives the frames), not a texture renderer.
//   - 0x00771a10 and 0x00771a14 both hold the exe's one texture (two references). The original double-buffers. The
//     texture is D3DUSAGE_DYNAMIC in D3DPOOL_DEFAULT (the original: usage 0, D3DPOOL_MANAGED); the format is the same
//     0x16. Rows stay in the decoder's bottom-up order (MpegOpenOptions::flip = false), as the original stores them.
//   - Frames reach the texture only when PortTop_MovieBind_Pump() runs (MpegVideoTexture::Update). The original's
//     renderer copies them on the DirectShow streaming thread. The knob path has no caller of the pump yet (see
//     RESULT_PORT-OT-BIND-D3D.md); the self-test calls it.
//   - Any failure returns 0x80004005 and logs the exe's reason instead of a message box.
//   - Audio: the exe renders the source's other output pins, as the original's IGraphBuilder::Render on the source
//     pin does.
#ifdef MASHED_STANDALONE
#include <windows.h>
#include <d3d9.h>
#include <dshow.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "../D3d9Render/MpegVideoTexture.h"

extern "C" IDirect3DDevice9* PortTop_ExeDevice();   // exe_main.cpp: the exe's device, the only device

namespace {

mashed_re::D3d9Render::MpegVideoTexture s_movie;   // the graph 0x004944c0 builds; one at a time, as the original

template <class T> T& At(unsigned a) { return *reinterpret_cast<T*>(static_cast<std::uintptr_t>(a)); }

void BindLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fputs("[OT-BIND-D3D] ", f);
        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(f, fmt, ap);
        va_end(ap);
        std::fputc('\n', f);
        std::fclose(f);
    }
}

// 0x006147c4: the five movie paths (frontend, empire, supersonic, renderware, intro), relative to the game root.
const char* const kMoviePath[5] = {
    "original/toastart/pc/movies/frontend.mpg",
    "original/toastart/pc/movies/empire.mpg",
    "original/toastart/pc/movies/supersonic.mpg",
    "original/toastart/pc/movies/renderware.mpg",
    "original/toastart/pc/movies/intro.mpg",
};

void Publish(unsigned slot, IUnknown* p) {   // the slot takes p's reference
    At<IUnknown*>(slot) = p;
}

}  // namespace

extern "C" {

// Binding of rva 0x004944c0: build and run the graph for movie [idx], publish its interfaces and texture.
int PortTop_Bound_004944c0(int idx) {
    IDirect3DDevice9* const g_device = PortTop_ExeDevice();
    At<int>(0x00771a1c) = idx;
    if (s_movie.ready()) s_movie.Close();   // the exe's own references of the previous movie
    if (!g_device || idx < 0 || idx > 4) {
        BindLog("0x004944c0(%d): no device or index out of 0..4, returns 0x80004005", idx);
        return static_cast<int>(0x80004005u);
    }
    D3DCAPS9 caps{};
    g_device->GetDeviceCaps(&caps);
    mashed_re::D3d9Render::MpegOpenOptions o;
    o.loop = false;
    o.audio = true;
    o.run = true;
    o.flip = false;
    o.pow2 = (caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0;   // 0x00494820: TextureCaps & 2
    char err[128] = {};
    if (!s_movie.Open(g_device, kMoviePath[idx], err, sizeof(err), o)) {
        BindLog("0x004944c0(%d) %s: open failed (%s), returns 0x80004005", idx, kMoviePath[idx], err);
        return static_cast<int>(0x80004005u);
    }
    IUnknown* graph = nullptr;
    IUnknown* filt = nullptr;
    IUnknown* ctrl = nullptr;
    IUnknown* pos = nullptr;
    IUnknown* ev = nullptr;
    IUnknown* audio = nullptr;
    s_movie.QueryGraph(IID_IGraphBuilder, reinterpret_cast<void**>(&graph));
    s_movie.QueryGrabberFilter(IID_IBaseFilter, reinterpret_cast<void**>(&filt));
    s_movie.QueryGraph(IID_IMediaControl, reinterpret_cast<void**>(&ctrl));
    s_movie.QueryGraph(IID_IMediaPosition, reinterpret_cast<void**>(&pos));
    s_movie.QueryGraph(IID_IMediaEvent, reinterpret_cast<void**>(&ev));
    s_movie.QueryGraph(IID_IBasicAudio, reinterpret_cast<void**>(&audio));
    Publish(0x00771a30, graph);
    if (IUnknown* old = At<IUnknown*>(0x00771a24)) old->Release();   // as the original, before the store
    Publish(0x00771a24, filt);
    Publish(0x00771a34, ctrl);
    Publish(0x00771a2c, pos);
    Publish(0x00771a28, ev);
    Publish(0x00771a20, audio);
    IDirect3DTexture9* tex = s_movie.texture();
    tex->AddRef();
    Publish(0x00771a10, tex);
    tex->AddRef();
    Publish(0x00771a14, tex);
    At<float>(0x007719ec) = static_cast<float>(s_movie.width());
    At<float>(0x007719f0) = static_cast<float>(s_movie.height());
    At<float>(0x007719f4) = static_cast<float>(s_movie.tex_width());
    At<float>(0x007719f8) = static_cast<float>(s_movie.tex_height());
    At<int>(0x007719fc) = D3DFMT_X8R8G8B8;   // 0x16, the format 0x00494820 requires
    At<int>(0x00771a00) = 0;
    At<int>(0x006147d8) = 1;
    BindLog("0x004944c0(%d) %s: graph %p ctrl %p pos %p event %p audio %p filter %p texture %p video %ux%u tex %ux%u",
            idx, kMoviePath[idx], static_cast<void*>(graph), static_cast<void*>(ctrl), static_cast<void*>(pos),
            static_cast<void*>(ev), static_cast<void*>(audio), static_cast<void*>(filt), static_cast<void*>(tex),
            s_movie.width(), s_movie.height(), s_movie.tex_width(), s_movie.tex_height());
    return 0;
}

// Frame upload into the published texture (MpegVideoTexture::Update). The original's renderer does this on the
// streaming thread; here it needs a render-thread caller.
void PortTop_MovieBind_Pump(void) {
    s_movie.Update();
}

// The matched bodies the self-test drives (exe-compiled: Match/MatchS16_00494a80.cpp, Match/MatchS15_00494460.cpp).
void FUN_00494a80(unsigned int param_1, unsigned int param_2, unsigned int param_3);
void IntroVideoClose(void);

}  // extern "C"

namespace {
ULONG RefCount(IUnknown* p) {
    if (!p) return 0;
    p->AddRef();
    return p->Release();
}
}  // namespace

// MASHED_DBG_MOVIEBIND_SELFTEST=1 (default off): open movie 0 (frontend.mpg: the four boot movies are 167,414-byte
// skip_intro stubs in a patched install) through the matched 0x00494a80, pump frames, poll the
// published IMediaEvent, then close through the matched 0x00494460 -> 0x00494320 and check the references.
extern "C" void PortTop_MovieBind_SelfTest(void) {
    char b[8] = {};
    if (GetEnvironmentVariableA("MASHED_DBG_MOVIEBIND_SELFTEST", b, sizeof(b)) == 0 || b[0] != '1') return;
    FUN_00494a80(0, 0, 0);
    IUnknown* graph = At<IUnknown*>(0x00771a30);
    IUnknown* tex = At<IUnknown*>(0x00771a10);
    const int flag04 = At<int>(0x00771a04);
    const int ras18 = At<int>(0x00771a18);
    BindLog("self-test open: 0x00771a04=%d 0x00771a18=%08x 0x00771a1c=%d 0x00771a00=%d 0x006147d8=%d "
            "a10==a14 %d floats %.0f %.0f %.0f %.0f fmt 0x%x; refs graph %lu texture %lu",
            flag04, ras18, At<int>(0x00771a1c), At<int>(0x00771a00), At<int>(0x006147d8),
            At<IUnknown*>(0x00771a10) == At<IUnknown*>(0x00771a14) ? 1 : 0,
            At<float>(0x007719ec), At<float>(0x007719f0), At<float>(0x007719f4), At<float>(0x007719f8),
            At<int>(0x007719fc), RefCount(graph), RefCount(tex));
    // Pump and poll as the original's per-frame 0x00494480 -> 0x004942b0 does: GetEvent on the published IMediaEvent.
    IMediaEvent* ev = At<IMediaEvent*>(0x00771a28);
    unsigned events = 0, complete = 0, nonblack = 0;
    for (int f = 0; f < 60 && ev; ++f) {
        PortTop_MovieBind_Pump();
        long code = 0;
        LONG_PTR p1 = 0, p2 = 0;
        while (ev->GetEvent(&code, &p1, &p2, 0) >= 0) {
            ++events;
            if (code == EC_COMPLETE) ++complete;
            ev->FreeEventParams(code, p1, p2);
        }
        Sleep(16);
    }
    if (IDirect3DTexture9* t = static_cast<IDirect3DTexture9*>(At<IUnknown*>(0x00771a10))) {
        D3DLOCKED_RECT lr;
        if (SUCCEEDED(t->LockRect(0, &lr, nullptr, D3DLOCK_READONLY))) {
            const unsigned w = static_cast<unsigned>(At<float>(0x007719ec));
            const unsigned h = static_cast<unsigned>(At<float>(0x007719f0));
            for (unsigned y = 0; y < h; y += 8)
                for (unsigned x = 0; x < w; x += 8)
                    if ((reinterpret_cast<const unsigned*>(static_cast<const unsigned char*>(lr.pBits) +
                                                           y * static_cast<unsigned>(lr.Pitch))[x] & 0xffffffu) != 0)
                        ++nonblack;
            t->UnlockRect(0);
        }
    }
    graph->AddRef();   // the test's own reference, to read the count after the close
    tex->AddRef();
    const ULONG g_before = RefCount(graph), t_before = RefCount(tex);
    IntroVideoClose();   // matched 0x00494460: 0x00494320 releases every published pointer, then RwRasterDestroy
    const ULONG g_after = RefCount(graph), t_after = RefCount(tex);
    BindLog("self-test pump: 60 frames, %u events (%u EC_COMPLETE), %u sampled texels nonzero", events, complete,
            nonblack);
    BindLog("self-test close: 0x00771a04=%d 0x00771a18=%08x slots a10 %p a14 %p a20 %p a24 %p a28 %p a2c %p a30 %p "
            "a34 %p; refs graph %lu -> %lu, texture %lu -> %lu",
            At<int>(0x00771a04), At<int>(0x00771a18), At<void*>(0x00771a10), At<void*>(0x00771a14),
            At<void*>(0x00771a20), At<void*>(0x00771a24), At<void*>(0x00771a28), At<void*>(0x00771a2c),
            At<void*>(0x00771a30), At<void*>(0x00771a34), g_before, g_after, t_before, t_after);
    graph->Release();
    tex->Release();
    s_movie.Close();
    // 0x00494320 releases 0x00771a10 / 0x00771a14 without a test and the matched 0x00494460 has run, so the published
    // texture slots are stale now. The original leaves them stale too; zero them so nothing reads a freed texture.
    At<void*>(0x00771a10) = nullptr;
    At<void*>(0x00771a14) = nullptr;
    BindLog("self-test done");
}

#endif  // MASHED_STANDALONE
