// LibRw/RwRaceFx.h — the original's in-race car shadow and headlight beam on the librw path
// (lane rfx, 2026-10-16, verify/rfx_20261016/PREREG_rfx.md).
//
// WHAT THE ORIGINAL DOES. Per car, FUN_00420050 @ 0x00420050 builds one RpPTank particle into the
// "car_shadow" batch (FUN_0041fcc0 @ 0x0041fcc0) and, with the headlight flag set, one into the
// "beam" batch (FUN_0041faf0 @ 0x0041faf0); FUN_0041ebb0 @ 0x0041ebb0 then flushes both and draws
// car_shadow, then beam, with ZWRITE off, from FUN_00410b30 after the four car renders
// (0x00410bb9). A matrix particle is the quad pos +- right/2 +- up/2 (RpPTank FUN_00537890).
//
// No rw types cross this header (EXE-ONLY TU, built in isolation by build.bat like RwRaceSubmit).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace LibRw {

// MASHED_CAR_SHADOW=0 / MASHED_CAR_BEAM=0 turn the two passes off (default ON).
bool RaceFx_CarShadowEnabled();
bool RaceFx_BeamEnabled();

// The vehicle descriptor family the shadow tables are indexed by: *(desc+0x80) / 6, the
// descriptor table at 0x005f37a8 (stride 0x84, 78 entries, six per family), from the car piz's
// base name. -1 when the name is not one of the 13 families.
int RaceFx_FamilyOfPiz(const char* piz_path);

// One car for this frame. Matrices are D3DMATRIX memory order (rows right, up, at, pos), the
// layout RaceSubmit_AddInstance takes. `root` = the clump frame's modelling matrix (the shadow's
// source, *(rec+0x26c) -> +4 -> +0x10); `body` = the frame the beam's parts hang off (frame 22 of
// the car DFF, whose LTM the car body draws with).
struct CarFxInput {
    float root[16];
    float body[16];
    float fade;      // rec+0x288
    bool  lights;    // rec+0x294 & 0x40
};
// Per-model data, set at car load: the descriptor family and parts 25 / 28's frame origins.
void RaceFx_SetCarModel(int family, const float part25[3], const float part28[3], bool parts_ok);
// Queue car `slot` (0..3) for this frame; consumed and cleared by RaceFx_DrawCarFx.
void RaceFx_QueueCar(int slot, const CarFxInput& in);

// Draw this frame's queue. Called by RaceSubmit_Render inside the camera update, after the
// instanced models (the cars), with its rw::Camera* (for the per-car frustum test, FUN_004c1b40
// at 0x00420081) as void*.
void RaceFx_DrawCarFx(void* rw_camera);

// ---- the in-race HUD (C08): team panels + start lights under the HUD camera -----------------
// FUN_00492e90 @ 0x00492e90 (states 3/5/6/7) renders FUN_0040dfc0 @ 0x0040dfc0 with the HUD camera
// DAT_006905b4 (a clone of the race camera, FUN_00467110: near 0.1, far 180), set up each frame by
// FUN_0040de30 @ 0x0040de30 (identity frame, view window 0.6 x 0.45, rwPARALLEL), with ZTEST and
// ZWRITE off. MASHED_HUD3D=0 turns the pass off (default ON).
bool RaceFx_HudEnabled();
// The game state the HUD reads, per frame (TrackRenderer fills it from the standalone's state).
struct HudInputs {
    int   state      = 0;      // DAT_0063ba8c
    float timer      = 0.f;    // DAT_0063d588 (start-light clock)
    int   team_play  = 0;      // DAT_0067ea64 (FUN_0042f500)
    int   round_type = 0;      // DAT_0067e9fc (FUN_0042f6a0)
    int   mode_fd0   = 0;      // DAT_007f0fd0
    int   n4d0       = 0;      // DAT_008a94d0 (FUN_0040e340)
    int   slot[4]    = {-1, -1, -1, -1};   // 0x007f1a14 + i*0x10
    int   team[4]    = {-1, -1, -1, -1};   //          ... + 4
    int   colour[4]  = {0, 0, 0, 0};       //          ... + 8
    int   score[4]   = {0, 0, 0, 0};       // DAT_008a94e0[i] (FUN_0040b6d0)
    bool  world_up   = true;   // FUN_00426ba0 (DAT_0066d704)
};
void RaceFx_SetHud(const HudInputs& in);
// Draw the HUD pass. Called by RaceSubmit_Render after the race camera's endUpdate, with the
// backbuffer size for the HUD camera's rasters.
void RaceFx_RenderHud(int width, int height);
// MASHED_HUD_REPLAY=<file>, DIAGNOSTIC ONLY (PREREG_rfx.md amendment 1): the HUD state and
// start-light timer come from the consumed trace row (ba8c, dtb), the mode / slot / score inputs
// from <file> (measured on the original, verify/rfx_20261016/hud_inputs.txt).
void RaceFx_ReplayHudRow(int ba8c, float dtb);

}  // namespace LibRw
}  // namespace mashed_re
