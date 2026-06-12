// Mashed RE — env-gated draw-stream dump (parity harness, standalone side).
//
// MASHED_DBG_DRAWSTREAM=<N | N:M | 1> captures every bridge Im2D draw of
// frontend frames N..M (`1` = frame 200, matching MASHED_DBG_BBDUMP's timing)
// into log/drawstream_re.json using the SAME record schema as the
// original-side capture (re/frida/menu_draw_dump.py -> log/menu_draw_dump.json):
//
//   { "f<frame>": [ {"v": "<hex vert blob>",
//                    "r": ["0x<ret>", ...],
//                    "s": [tex_handle, alpha_on, src_blend, dst_blend]}, ... ] }
//
// `v` is the verbatim RW Im2D vertex blob (count x 0x1c bytes: x,y,z,rhw,
// color,u,v) the C3 draw reimpls wrote at DAT_00898a20 — byte-identical layout
// to what the original's device draw fn reads, so re/tools/drawlist_diff.py
// decodes both sides with one decoder.
// `r` is the module-relative return-address chain on OUR exe (resolve names
// via build/mashed_re.map); it is attribution within this side only — it is
// never compared against the original's RVA chain.
// `s` is the bridge's mirrored render state at draw time (the original-side
// capture has no `s`; those states live inside the RW device there).
//
// Pair with MASHED_GOTO=<screen> for nav-matched captures.

#pragma once

namespace mashed_re {
namespace D3d9Render {

// Call once per frontend frame (frame delimiter). Draws reported between two
// calls belong to the earlier frame. Cheap no-op when the env var is unset.
void DrawStreamDump_OnFrameBegin();

// Call from the bridge draw handler with the raw source vertex blob and the
// mirrored render state. Cheap no-op unless the current frame is in range.
// `bridge_ret` is the bridge handler's _ReturnAddress() (the C3 reimpl call
// site) — recorded first so attribution survives /Oy frame-pointer omission
// defeating the deeper stack walk.
void DrawStreamDump_OnDraw(const void* verts, int count, int tex_handle,
                           bool alpha_on, int src_blend, int dst_blend,
                           void* bridge_ret);

}  // namespace D3d9Render
}  // namespace mashed_re
