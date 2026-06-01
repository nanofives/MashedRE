// Mashed RE — WIC image decoder implementation (B19a). See PngLoader.h.

#include "PngLoader.h"

#include <windows.h>
#include <wincodec.h>
#include <cstdlib>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace mashed_re {
namespace D3d9Render {

std::uint8_t* DecodeImageToBGRA(const std::uint8_t* data, std::size_t len,
                                std::uint32_t* out_w, std::uint32_t* out_h) {
    if (!data || len == 0 || !out_w || !out_h) return nullptr;

    // WIC needs COM. Initialize per call (no matching CoUninitialize — the
    // process owns COM for its lifetime; a redundant init returns S_FALSE and is
    // harmless). Apartment-threaded is fine for our single-threaded use.
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IWICImagingFactory*    factory = nullptr;
    IWICStream*            stream  = nullptr;
    IWICBitmapDecoder*     decoder = nullptr;
    IWICBitmapFrameDecode* frame   = nullptr;
    IWICFormatConverter*   conv    = nullptr;
    std::uint8_t*          result  = nullptr;

    do {
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                    CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                    reinterpret_cast<void**>(&factory))) || !factory)
            break;
        if (FAILED(factory->CreateStream(&stream))) break;
        if (FAILED(stream->InitializeFromMemory(const_cast<BYTE*>(data),
                                                static_cast<DWORD>(len)))) break;
        if (FAILED(factory->CreateDecoderFromStream(stream, nullptr,
                                                    WICDecodeMetadataCacheOnLoad, &decoder)))
            break;
        if (FAILED(decoder->GetFrame(0, &frame))) break;
        if (FAILED(factory->CreateFormatConverter(&conv))) break;
        if (FAILED(conv->Initialize(frame, GUID_WICPixelFormat32bppBGRA,
                                    WICBitmapDitherTypeNone, nullptr, 0.0,
                                    WICBitmapPaletteTypeCustom)))
            break;

        UINT w = 0, h = 0;
        if (FAILED(conv->GetSize(&w, &h)) || w == 0 || h == 0) break;
        const UINT stride = w * 4u;
        const UINT size   = stride * h;
        std::uint8_t* buf = static_cast<std::uint8_t*>(std::malloc(size));
        if (!buf) break;
        if (FAILED(conv->CopyPixels(nullptr, stride, size, buf))) {
            std::free(buf);
            break;
        }
        *out_w = w;
        *out_h = h;
        result = buf;
    } while (false);

    if (conv)    conv->Release();
    if (frame)   frame->Release();
    if (decoder) decoder->Release();
    if (stream)  stream->Release();
    if (factory) factory->Release();
    return result;
}

}  // namespace D3d9Render
}  // namespace mashed_re
