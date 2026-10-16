#pragma once
// [d1r1 2026-10-16] Per-draw D3D9 recorder for the in-race draw-list adjudication (D1-residue R1,
// verify/d1r1_20261016/PREREG_d1r1.md I-2). Diagnostic only: everything here is inert unless
// MASHED_DBG_D3DREC is set, so the default build's behaviour is unchanged.
//
// Records every DrawPrimitive / DrawIndexedPrimitive / DrawPrimitiveUP / DrawIndexedPrimitiveUP of
// the current frame with the same fields the original-side twin records
// (re/frida/camera_probe.py --drawrec): primitive type, counts, the stage-0 texture's name, a
// fixed render-state set read back from the device, the world matrix, FVF, vertex-shader-bound and
// the stream-0 stride. librw draws through shaders, so its texture and world matrix come from
// librw itself (rw_drawrec_pre/post hooks in deps/librw/src/d3d/d3d9render.cpp), not from the
// device's fixed-function state.
struct IDirect3DDevice9;

namespace mashed_re {
namespace DrawRec {

bool Enabled();                                   // MASHED_DBG_D3DREC set (cached)
void NameTexture(const void* d3dtex, const char* name);
void Install(IDirect3DDevice9* dev);              // patch the four draw slots once
// Drop the previous frame's records; arm for this frame when the previous frame consumed a
// replay row within 3 of a pending MASHED_CAM_REPLAY_SHOTS entry.
void BeginFrame(int prev_row);
// Write the current frame's records as JSON: {"side":"stand","row":row,"events":[...]}
bool Write(const char* path, int row, int actual_row);
// [rfx 2026-10-16] librw immediate mode streams through a DYNAMIC write-only vertex/index buffer
// that a read-only lock cannot be trusted to read back, so the caller hands over the CPU-side copy
// it just submitted: the next indexed draw takes its triangles and positions (first three floats
// of each `stride`-byte vertex; the vertex colour is found through the bound declaration as for
// every other librw draw) from here. One draw only; inert unless the recorder is armed.
// [rfx2] screen = the vertices are pre-transformed screen positions (librw Im2D, the original's
// XYZRHW), recorded with the "screen" flag the original side's FVF carries.
void SetNextGeometry(const void* verts, unsigned stride, const unsigned short* idx, unsigned nidx,
                     bool screen = false);

}  // namespace DrawRec
}  // namespace mashed_re
