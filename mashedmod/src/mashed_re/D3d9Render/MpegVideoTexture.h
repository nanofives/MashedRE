// Mashed RE — frontend-faithful F1: the menu backdrop is VIDEO.
//
// The original plays toastart/pc/movies/frontend.mpg (MPEG-1 PS, 512x512
// @30fps) through DirectShow ("Could not run the DirectShow graph!" error
// strings at 0x005cfb18..0x005cfc3c) behind the frontend. This class is the
// standalone equivalent: a DirectShow filter graph (source -> MPEG-1 decoder
// -> SampleGrabber RGB32 -> NullRenderer) whose frames are uploaded into a
// D3D9 texture each render tick. Looping via IMediaPosition rewind.
//
// This is the ORIGINAL'S OWN mechanism (Windows-shipped MPEG-1 decoder), not
// an approximation; only the graph plumbing is ours.
#pragma once

#include <d3d9.h>

struct IGraphBuilder;
struct IMediaControl;
struct IMediaSeeking;
struct ISampleGrabberCompat;

namespace mashed_re {
namespace D3d9Render {

class MpegVideoTexture {
public:
    // Build the graph for `mpg_path` and create the target texture on `dev`.
    // Returns false (and stays inert) if any DirectShow piece is missing.
    bool Open(IDirect3DDevice9* dev, const char* mpg_path, char* err,
              unsigned err_len);
    // Copy the current video frame into the texture; rewinds at EOS.
    void Update();
    void Close();

    IDirect3DTexture9* texture() const { return tex_; }
    bool ready() const { return ready_; }
    unsigned width() const { return w_; }
    unsigned height() const { return h_; }

private:
    IGraphBuilder*       graph_ = nullptr;
    IMediaControl*       ctrl_ = nullptr;
    IMediaSeeking*       seek_ = nullptr;
    ISampleGrabberCompat* grab_ = nullptr;
    IDirect3DTexture9*   tex_ = nullptr;
    unsigned char*       buf_ = nullptr;
    long                 buf_len_ = 0;
    unsigned             w_ = 0, h_ = 0;
    bool                 ready_ = false;
};

}  // namespace D3d9Render
}  // namespace mashed_re
