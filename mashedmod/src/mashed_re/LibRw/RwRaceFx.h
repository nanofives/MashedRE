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

// [part B] the HUD replay input for the consumed trace row (TrackRenderer CamReplay).
void RaceFx_ReplayHudRow(int ba8c, float dtb);

}  // namespace LibRw
}  // namespace mashed_re
