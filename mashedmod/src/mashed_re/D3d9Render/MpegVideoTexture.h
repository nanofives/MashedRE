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
struct IMediaEventEx;
struct ISampleGrabberCompat;
struct MpegDetPinCB;

namespace mashed_re {
namespace D3d9Render {

// [D5-INTROMOVIES] Open() options. The defaults are the frontend.mpg backdrop
// (unchanged). The boot intro movies (FUN_004944c0) use loop=false, audio=true;
// small.mpg (FUN_00494c80) uses run=false.
struct MpegOpenOptions {
    bool loop  = true;   // rewind at end of stream (frontend backdrop)
    bool audio = false;  // also Render() the splitter's other output pins (sound),
                         // as the original's IGraphBuilder::Render on the source pin does
    bool run   = true;   // false: build the graph only, never IMediaControl::Run
    bool pow2  = false;  // texture padded to powers of two (0x00494820, TextureCaps & POW2)
};

class MpegVideoTexture {
public:
    // Build the graph for `mpg_path` and create the target texture on `dev`.
    // Returns false (and stays inert) if any DirectShow piece is missing.
    bool Open(IDirect3DDevice9* dev, const char* mpg_path, char* err,
              unsigned err_len, const MpegOpenOptions& opt = MpegOpenOptions());
    // Copy the current video frame into the texture; rewinds at EOS when looping.
    void Update();
    // Non-looping streams: drain the graph's event queue and return true once
    // EC_COMPLETE has been seen (FUN_004942b0; sticky).
    bool Ended();
    void Close();

    IDirect3DTexture9* texture() const { return tex_; }
    bool ready() const { return ready_; }
    unsigned width() const { return w_; }
    unsigned height() const { return h_; }
    unsigned tex_width() const { return tw_; }
    unsigned tex_height() const { return th_; }
    long audio_hr() const { return audio_hr_; }   // last Render() result on the extra pins
    // PORT-TOP-KNOBON-CAPFLAKE probe: under MASHED_DETERMINISTIC, the frozen
    // frame's FNV-1a hash and stream position (100 ns units; pinned mode: the
    // sample start time) once the one upload has happened.
    bool det_uploaded() const { return det_uploaded_; }
    unsigned det_hash() const { return det_hash_; }
    long long det_pos() const { return det_pos_; }
    bool det_pinned() const { return pin_ != nullptr; }

private:
    IGraphBuilder*       graph_ = nullptr;
    IMediaControl*       ctrl_ = nullptr;
    IMediaSeeking*       seek_ = nullptr;
    IMediaEventEx*       event_ = nullptr;
    ISampleGrabberCompat* grab_ = nullptr;
    IDirect3DTexture9*   tex_ = nullptr;
    unsigned char*       buf_ = nullptr;
    long                 buf_len_ = 0;
    unsigned             w_ = 0, h_ = 0;
    unsigned             tw_ = 0, th_ = 0;   // texture size (= w_/h_ unless pow2)
    bool                 loop_ = true;
    bool                 ended_ = false;
    long                 audio_hr_ = 0;
    bool                 ready_ = false;
    // R10b residual: under MASHED_DETERMINISTIC the frame COUNT is pinned but
    // wall-clock is not, and this texture is fed by a real-time DirectShow graph.
    // So the backdrop lands on a different video frame every run. Freeze after the
    // first upload in deterministic mode; see Update().
    bool                 det_uploaded_ = false;
    unsigned             det_hash_ = 0;
    long long            det_pos_ = -1;
    // MASHED_DET_MPG_PIN=<ms> (default OFF, deterministic mode, looping streams
    // only): freeze on the first decoded sample whose start time is >= <ms>
    // instead of whatever GetCurrentBuffer holds at the first Update().
    MpegDetPinCB*        pin_ = nullptr;
};

}  // namespace D3d9Render
}  // namespace mashed_re
