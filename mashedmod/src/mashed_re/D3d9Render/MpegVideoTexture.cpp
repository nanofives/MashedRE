// See MpegVideoTexture.h. DirectShow SampleGrabber path — qedit.h is gone
// from modern SDKs, so the grabber CLSID/interface are declared locally
// (binary-stable COM GUIDs; the filter still ships in Windows qedit.dll).
#include "MpegVideoTexture.h"

#include <dshow.h>
#include <cstdio>
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

namespace mashed_re {
namespace D3d9Render {

bool MpegVideoTexture::Open(IDirect3DDevice9* dev, const char* mpg_path,
                            char* err, unsigned err_len) {
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

    if (FAILED(dev->CreateTexture(w_, h_, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8,
                                  D3DPOOL_DEFAULT, &tex_, nullptr)))
        return fail("CreateTexture", E_FAIL);
    buf_len_ = static_cast<long>(w_ * h_ * 4);
    buf_ = new unsigned char[static_cast<unsigned>(buf_len_)];

    graph_->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&ctrl_));
    graph_->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&seek_));
    if (!ctrl_) return fail("IMediaControl", E_NOINTERFACE);
    ctrl_->Run();
    ready_ = true;
    return true;
}

void MpegVideoTexture::Update() {
    if (!ready_ || !grab_ || !tex_) return;
    long sz = buf_len_;
    if (FAILED(grab_->GetCurrentBuffer(&sz, reinterpret_cast<long*>(buf_))) || sz <= 0)
        return;
    // loop: rewind when the stream reaches its end
    if (seek_) {
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
    }
}

void MpegVideoTexture::Close() {
    if (ctrl_) { ctrl_->Stop(); ctrl_->Release(); ctrl_ = nullptr; }
    if (seek_) { seek_->Release(); seek_ = nullptr; }
    if (grab_) { grab_->Release(); grab_ = nullptr; }
    if (graph_) { graph_->Release(); graph_ = nullptr; }
    if (tex_) { tex_->Release(); tex_ = nullptr; }
    delete[] buf_;
    buf_ = nullptr;
    ready_ = false;
}

}  // namespace D3d9Render
}  // namespace mashed_re
