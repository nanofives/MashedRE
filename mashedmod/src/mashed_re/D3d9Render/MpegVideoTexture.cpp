// See MpegVideoTexture.h. DirectShow SampleGrabber path — qedit.h is gone
// from modern SDKs, so the grabber CLSID/interface are declared locally
// (binary-stable COM GUIDs; the filter still ships in Windows qedit.dll).
#include "MpegVideoTexture.h"

#include <dshow.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#pragma comment(lib, "strmiids.lib")

// ---- qedit.h compat (deprecated header; interfaces unchanged since DX8) ----
static const CLSID kCLSID_SampleGrabber =
    {0xC1F400A0, 0x3F08, 0x11D3, {0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37}};
static const CLSID kCLSID_NullRenderer =
    {0xC1F400A4, 0x3F08, 0x11D3, {0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37}};
static const IID kIID_ISampleGrabber =
    {0x6B652FFF, 0x11FE, 0x4FCE, {0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F}};

struct ISampleGrabberCB : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE SampleCB(double, IMediaSample*) = 0;
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double, BYTE*, long) = 0;
};
struct ISampleGrabberCompat : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMediaType(const AM_MEDIA_TYPE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(AM_MEDIA_TYPE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(long*, long*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(IMediaSample**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCallback(ISampleGrabberCB*, long) = 0;
};

// ---- PORT-TOP-KNOBON-CAPFLAKE: MASHED_DET_MPG_PIN=<ms> (default OFF) ----
// The R10b freeze below takes whatever buffer GetCurrentBuffer() holds at the
// first Update(), and how far the real-time graph has got by then depends on
// machine load: the 04G f2 / 04J p1 nav captures froze on a different
// frontend.mpg frame (diff only in the backdrop rect, x 0..509). With the knob,
// the grabber's BufferCB (streaming thread) copies the first sample whose start
// time is >= <ms>, and the graph runs with no reference clock (SetSyncSource
// NULL) so no sample is ever late or dropped. The frame is then a function of
// the file alone.
struct MpegDetPinCB : public ISampleGrabberCB {
    double target_s = 0.0;
    unsigned char* buf = nullptr;
    long len = 0;
    volatile LONG got = 0;        // 1 once buf holds the pinned frame
    double got_time = -1.0;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown) { *ppv = static_cast<IUnknown*>(this); return S_OK; }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 2; }    // owned by MpegVideoTexture
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE SampleCB(double, IMediaSample*) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE BufferCB(double t, BYTE* p, long n) override {
        if (got || !buf || !p || n <= 0 || t + 1e-4 < target_s) return S_OK;
        std::memcpy(buf, p, static_cast<size_t>(n < len ? n : len));
        got_time = t;
        InterlockedExchange(&got, 1);
        return S_OK;
    }
};

static unsigned Fnv1a(const unsigned char* p, long n) {
    unsigned h = 2166136261u;
    for (long i = 0; i < n; ++i) { h ^= p[i]; h *= 16777619u; }
    return h;
}

static int DetModeOn() {
    static int s_det = -1;
    if (s_det < 0) {
        char b[8] = {};
        s_det = (GetEnvironmentVariableA("MASHED_DETERMINISTIC", b, sizeof(b)) > 0
                 && b[0] == '1') ? 1 : 0;
    }
    return s_det;
}

// <0 = knob off. Read per Open() (cheap, a handful of calls per boot).
static double DetPinTargetSeconds() {
    char b[32] = {};
    if (GetEnvironmentVariableA("MASHED_DET_MPG_PIN", b, sizeof(b)) == 0 || !b[0])
        return -1.0;
    return std::atof(b) / 1000.0;
}

namespace mashed_re {
namespace D3d9Render {

bool MpegVideoTexture::Open(IDirect3DDevice9* dev, const char* mpg_path,
                            char* err, unsigned err_len,
                            const MpegOpenOptions& opt) {
    loop_ = opt.loop;
    ended_ = false;
    audio_hr_ = 0;
    auto fail = [&](const char* what, HRESULT hr) {
        if (err) std::snprintf(err, err_len, "%s hr=0x%08lx", what, hr);
        Close();
        return false;
    };
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IGraphBuilder, reinterpret_cast<void**>(&graph_));
    if (FAILED(hr)) return fail("FilterGraph", hr);

    IBaseFilter* grab_f = nullptr;
    hr = CoCreateInstance(kCLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void**>(&grab_f));
    if (FAILED(hr)) return fail("SampleGrabber", hr);
    graph_->AddFilter(grab_f, L"grab");
    grab_f->QueryInterface(kIID_ISampleGrabber, reinterpret_cast<void**>(&grab_));
    if (!grab_) { grab_f->Release(); return fail("ISampleGrabber", E_NOINTERFACE); }

    AM_MEDIA_TYPE mt;
    std::memset(&mt, 0, sizeof(mt));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB32;
    grab_->SetMediaType(&mt);
    grab_->SetBufferSamples(TRUE);
    grab_->SetOneShot(FALSE);

    IBaseFilter* null_f = nullptr;
    hr = CoCreateInstance(kCLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void**>(&null_f));
    if (FAILED(hr)) { grab_f->Release(); return fail("NullRenderer", hr); }
    graph_->AddFilter(null_f, L"null");

    wchar_t wpath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, mpg_path, -1, wpath, MAX_PATH);
    IBaseFilter* src_f = nullptr;
    hr = graph_->AddSourceFilter(wpath, L"src", &src_f);
    if (FAILED(hr)) { grab_f->Release(); null_f->Release(); return fail("AddSourceFilter", hr); }

    // src(video pin) -> grabber -> null. Connect() lets DirectShow insert the
    // MPEG-1 splitter + decoder (the same Windows filters the original used).
    IEnumPins* pins = nullptr;
    src_f->EnumPins(&pins);
    IPin* out = nullptr;
    bool connected = false;
    IPin* gin = nullptr;
    {
        IEnumPins* gp = nullptr;
        grab_f->EnumPins(&gp);
        IPin* p = nullptr;
        while (gp && gp->Next(1, &p, nullptr) == S_OK) {
            PIN_DIRECTION d;
            p->QueryDirection(&d);
            if (d == PINDIR_INPUT) { gin = p; break; }
            p->Release();
        }
        if (gp) gp->Release();
    }
    while (pins && pins->Next(1, &out, nullptr) == S_OK) {
        PIN_DIRECTION d;
        out->QueryDirection(&d);
        if (d == PINDIR_OUTPUT && gin &&
            SUCCEEDED(graph_->Connect(out, gin))) {
            connected = true;
            out->Release();
            break;
        }
        out->Release();
    }
    if (pins) pins->Release();
    if (gin) gin->Release();
    if (!connected) { grab_f->Release(); null_f->Release(); return fail("Connect src->grab", E_FAIL); }

    // grabber out -> null renderer in
    {
        IPin* gout = nullptr; IPin* nin = nullptr;
        IEnumPins* gp = nullptr; grab_f->EnumPins(&gp);
        IPin* p = nullptr;
        while (gp && gp->Next(1, &p, nullptr) == S_OK) {
            PIN_DIRECTION d; p->QueryDirection(&d);
            if (d == PINDIR_OUTPUT) { gout = p; break; }
            p->Release();
        }
        if (gp) gp->Release();
        IEnumPins* np = nullptr; null_f->EnumPins(&np);
        while (np && np->Next(1, &p, nullptr) == S_OK) {
            PIN_DIRECTION d; p->QueryDirection(&d);
            if (d == PINDIR_INPUT) { nin = p; break; }
            p->Release();
        }
        if (np) np->Release();
        hr = (gout && nin) ? graph_->Connect(gout, nin) : E_FAIL;
        if (gout) gout->Release();
        if (nin) nin->Release();
        grab_f->Release();
        null_f->Release();
        if (FAILED(hr)) return fail("Connect grab->null", hr);
    }

    AM_MEDIA_TYPE got;
    std::memset(&got, 0, sizeof(got));
    if (FAILED(grab_->GetConnectedMediaType(&got)) ||
        got.formattype != FORMAT_VideoInfo || !got.pbFormat)
        return fail("GetConnectedMediaType", E_FAIL);
    const VIDEOINFOHEADER* vih = reinterpret_cast<VIDEOINFOHEADER*>(got.pbFormat);
    w_ = static_cast<unsigned>(vih->bmiHeader.biWidth);
    h_ = static_cast<unsigned>(vih->bmiHeader.biHeight < 0
                                   ? -vih->bmiHeader.biHeight
                                   : vih->bmiHeader.biHeight);
    if (got.pbFormat) CoTaskMemFree(got.pbFormat);

    // [D5-INTROMOVIES] The original renders the source pin with
    // IGraphBuilder::Render (vtbl +0x30 at 0x00494e2c / FUN_004944c0), which also
    // builds the sound branch. Here the video branch is wired by hand above, so
    // render whatever output pin is still unconnected (the splitter's audio).
    if (opt.audio) {
        IEnumFilters* ef = nullptr;
        graph_->EnumFilters(&ef);
        IBaseFilter* f = nullptr;
        IPin* todo[8] = {};
        int ntodo = 0;
        while (ef && ef->Next(1, &f, nullptr) == S_OK) {
            IEnumPins* ep = nullptr;
            f->EnumPins(&ep);
            IPin* p = nullptr;
            while (ep && ep->Next(1, &p, nullptr) == S_OK) {
                PIN_DIRECTION d;
                p->QueryDirection(&d);
                IPin* peer = nullptr;
                if (d == PINDIR_OUTPUT && FAILED(p->ConnectedTo(&peer)) && ntodo < 8) {
                    todo[ntodo++] = p;
                    continue;
                }
                if (peer) peer->Release();
                p->Release();
            }
            if (ep) ep->Release();
            f->Release();
        }
        if (ef) ef->Release();
        // Render after the enumeration: Render() adds filters, which resets it.
        audio_hr_ = ntodo ? S_OK : S_FALSE;
        for (int i = 0; i < ntodo; ++i) {
            const HRESULT rh = graph_->Render(todo[i]);
            if (rh != S_OK) audio_hr_ = rh;
            todo[i]->Release();
        }
    }

    // 0x00494820 (renderer SetMediaType): the texture is the video size, or the
    // next powers of two (>= 2) when D3DCAPS9.TextureCaps has D3DPTEXTURECAPS_POW2
    // (test [esp+0x74],2 at 0x00494898; doubling loops 0x004948b0 / 0x004948c3).
    tw_ = w_;
    th_ = h_;
    if (opt.pow2) {
        unsigned a = 2, b = 2;
        while (a < w_) a <<= 1;
        while (b < h_) b <<= 1;
        tw_ = a;
        th_ = b;
    }
    if (FAILED(dev->CreateTexture(tw_, th_, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8,
                                  D3DPOOL_DEFAULT, &tex_, nullptr)))
        return fail("CreateTexture", E_FAIL);
    buf_len_ = static_cast<long>(w_ * h_ * 4);
    buf_ = new unsigned char[static_cast<unsigned>(buf_len_)];

    graph_->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&ctrl_));
    graph_->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&seek_));
    if (!opt.loop)   // only the non-looping streams read their events (Ended())
        graph_->QueryInterface(IID_IMediaEventEx, reinterpret_cast<void**>(&event_));
    if (!ctrl_) return fail("IMediaControl", E_NOINTERFACE);
    // PORT-TOP-KNOBON-CAPFLAKE knob (default OFF; see MpegDetPinCB).
    if (opt.loop && opt.run && DetModeOn()) {
        const double tgt = DetPinTargetSeconds();
        if (tgt >= 0.0) {
            pin_ = new MpegDetPinCB();
            pin_->target_s = tgt;
            pin_->len = buf_len_;
            pin_->buf = new unsigned char[static_cast<unsigned>(buf_len_)];
            IMediaFilter* mf = nullptr;
            graph_->QueryInterface(IID_IMediaFilter, reinterpret_cast<void**>(&mf));
            if (mf) { mf->SetSyncSource(nullptr); mf->Release(); }
            grab_->SetCallback(pin_, 1);   // 1 = BufferCB
        }
    }
    if (opt.run) ctrl_->Run();
    ready_ = true;
    return true;
}

void MpegVideoTexture::Update() {
    if (!ready_ || !grab_ || !tex_) return;
    // ── R10b residual fix (D1, 2026-08-15) ──────────────────────────────────
    // MASHED_DETERMINISTIC pins the frame INDEX, not wall-clock time. This
    // texture is fed by a live DirectShow graph via GetCurrentBuffer(), so the
    // frame it happens to be showing when Update() runs depends on how fast the
    // machine got here. That made 02_back_to_menu the last unstable shot in the
    // parity gate: two runs of the SAME BUILD differed on 17.30% of pixels while
    // every RELIGHT_CAP heading was bit-identical, i.e. the simulation agreed and
    // only the backdrop moved.
    //
    // Deterministic mode therefore freezes the backdrop after ONE upload: the
    // menu still shows real video rather than a black hole, and the frame is
    // reproducible. Outside deterministic mode nothing changes — the backdrop
    // animates exactly as before.
    const int s_det = DetModeOn();
    if (s_det && det_uploaded_) return;
    long sz = buf_len_;
    if (pin_) {
        // MASHED_DET_MPG_PIN: upload the pinned sample, then stop the graph
        // (nothing reads it again in deterministic mode).
        if (!pin_->got) return;
        std::memcpy(buf_, pin_->buf, static_cast<size_t>(buf_len_));
        det_pos_ = static_cast<long long>(pin_->got_time * 1e7 + 0.5);
        if (ctrl_) ctrl_->Stop();
    } else {
        if (FAILED(grab_->GetCurrentBuffer(&sz, reinterpret_cast<long*>(buf_))) || sz <= 0)
            return;
        if (s_det && seek_) {   // probe only: where the live graph was at the freeze
            LONGLONG cur = -1;
            if (SUCCEEDED(seek_->GetCurrentPosition(&cur))) det_pos_ = cur;
        }
    }
    if (s_det) det_hash_ = Fnv1a(buf_, buf_len_);
    // loop: rewind when the stream reaches its end
    if (loop_ && seek_) {
        LONGLONG cur = 0, dur = 0;
        if (SUCCEEDED(seek_->GetCurrentPosition(&cur)) &&
            SUCCEEDED(seek_->GetDuration(&dur)) && dur > 0 && cur >= dur) {
            LONGLONG zero = 0;
            seek_->SetPositions(&zero, AM_SEEKING_AbsolutePositioning, nullptr,
                                AM_SEEKING_NoPositioning);
        }
    }
    D3DLOCKED_RECT lr;
    if (SUCCEEDED(tex_->LockRect(0, &lr, nullptr, D3DLOCK_DISCARD))) {
        // RGB32 DIB rows are bottom-up; flip while copying
        const unsigned stride = w_ * 4;
        for (unsigned y = 0; y < h_; ++y)
            std::memcpy(static_cast<unsigned char*>(lr.pBits) +
                            y * static_cast<unsigned>(lr.Pitch),
                        buf_ + (h_ - 1 - y) * stride, stride);
        tex_->UnlockRect(0);
        det_uploaded_ = true;   // R10b: one upload is enough in deterministic mode
    }
}

// FUN_004942b0: GetEvent (vtbl +0x20, timeout 0) on the IMediaEvent at
// DAT_00771a28; when the code is 1 (EC_COMPLETE) it calls +0x20 on DAT_00771a2c
// with (0, 0) and, with DAT_00771a08 == 0 (always, from FUN_00495350), clears the
// running flag DAT_00771a04; FreeEventParams (+0x30) either way. The original
// reads one event per frame. This drains the queue, so EC_COMPLETE is seen on the
// frame it is queued even behind other events.
bool MpegVideoTexture::Ended() {
    if (!event_) return ended_;
    long code = 0;
    LONG_PTR p1 = 0, p2 = 0;
    while (event_->GetEvent(&code, &p1, &p2, 0) == S_OK) {
        if (code == EC_COMPLETE) ended_ = true;
        event_->FreeEventParams(code, p1, p2);
    }
    return ended_;
}

void MpegVideoTexture::Close() {
    if (ctrl_) { ctrl_->Stop(); ctrl_->Release(); ctrl_ = nullptr; }
    if (event_) { event_->Release(); event_ = nullptr; }
    if (seek_) { seek_->Release(); seek_ = nullptr; }
    if (grab_) {
        if (pin_) grab_->SetCallback(nullptr, 1);
        grab_->Release(); grab_ = nullptr;
    }
    if (pin_) { delete[] pin_->buf; delete pin_; pin_ = nullptr; }
    if (graph_) { graph_->Release(); graph_ = nullptr; }
    if (tex_) { tex_->Release(); tex_ = nullptr; }
    delete[] buf_;
    buf_ = nullptr;
    ready_ = false;
}

}  // namespace D3d9Render
}  // namespace mashed_re
