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

#include <cstddef>
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
    bool  brake;     // [rfx2] *(0x007f103c + slot*0x4c + 1), the controller's brake byte
};
// [rfx2] the light marker parts FUN_00412f30 @ 0x00412f30 reads (FUN_0041f1c0 / FUN_0041f1e0): 0x19,
// 0x1a, 0x1c, 0x1b (headlights), 0x37, 0x38 (tail), 0x39, 0x3a (brake), in that slot order; their
// frame origins in the body frame and whether the model has each one.
void RaceFx_SetCarLightParts(const float pos[8][3], const bool have[8]);
// MASHED_HEADLIGHT=0 turns the Headlight sprites (FUN_00413a00 / FUN_00413a40) off (default ON).
bool RaceFx_HeadlightEnabled();
// Per-model data, set at car load: the descriptor family and parts 25 / 28's frame origins.
void RaceFx_SetCarModel(int family, const float part25[3], const float part28[3], bool parts_ok);
// Queue car `slot` (0..3) for this frame; consumed and cleared by RaceFx_DrawCarFx.
void RaceFx_QueueCar(int slot, const CarFxInput& in);

// Draw this frame's queue. Called by RaceSubmit_Render inside the camera update, after the
// instanced models (the cars), with its rw::Camera* (for the per-car frustum test, FUN_004c1b40
// at 0x00420081) as void*.
void RaceFx_DrawCarFx(void* rw_camera);

// ---- [rfx2] the dynamic-object shadows (C04 "RWObjShad") ------------------------------------
// FUN_00481a30 @ 0x00481a30 (tick, from FUN_0040ce00) walks the DAT_006c6eb0 RWP objects and adds
// one RpPTank matrix particle per shadowed visible object to DAT_00703068 (FUN_00487d00); the
// shadow matrix is FUN_00487280 @ 0x00487280's (a ray along the course light's at-vector into the
// collision world); FUN_00487d70 @ 0x00487d70 draws the batch with cull off and ZWRITE off, first in
// FUN_0040bde0 (after the copters, FUN_00410b30). MASHED_OBJ_SHADOW=0 turns it off (default ON).
bool RaceFx_ObjShadowEnabled();
// One object at track load, in the original's creation order (FUN_00480340 @ 0x00480340: the
// RWP_Object descriptors by index, then each one's MTS records). ltm = the frame LTM (D3DMATRIX
// memory order: rows right, up, at, pos), bbox = DAT_006c9a88's six floats (sup.xyz, inf.xyz),
// sphere = RpAtomicGetWorldBoundingSphere (centre xyz, radius).
struct ObjShadowIn {
    float ltm[16];
    float bbox[6];
    float sphere[4];
    bool  casts;     // false: the slot exists (index parity) but its shape is not ported
};
// The collision world the shadow ray reads (FUN_00538c80 on DAT_0080332c): xyz per vertex,
// three vertex indices per triangle (RW order v0, v1, v2), and each triangle's material colour
// as the RwRGBA dword (r | g<<8 | b<<16 | a<<24) FUN_0045c110 @ 0x0045c110 tests.
struct ObjShadowSoup {
    const float*         verts = nullptr;
    const std::uint32_t* tris = nullptr;
    const std::uint32_t* tri_rgba = nullptr;
    std::size_t          ntri = 0;
};
// Compute every object's shadow once (the objects are static in the standalone): `light` = the
// course directional light's frame LTM (DAT_0065743c -> +4 -> +0x50), 16 floats.
void RaceFx_SetObjShadows(const ObjShadowIn* objs, int n, const float light[16], const ObjShadowSoup& soup);
// This tick's DAT_007f101c (the parity of the alternate-frame frustum refresh, 0x00481a80).
void RaceFx_SetObjShadowFrame(std::uint32_t f101c);
// Gate and draw this frame's object shadows (and, from rfx2 on, the other FUN_0040bde0 passes).
// Called by RaceSubmit_Render inside the camera update, after every instance (the copters).
void RaceFx_DrawWorldFx(void* rw_camera);

// ---- [rfx2] lensflare-fs8 (C09): the course light list's two RpPTank batches ----------------
// FUN_00487e70 @ 0x00487e70 adds a light to the 128-entry list at 0x0086ae20 (stride 0x38: type,
// position, element count, enabled, k, tables); FUN_00488e70 @ 0x00488e70 fills the small batch
// DAT_007030a8 (one camera-facing particle per element) and FUN_004883b0 @ 0x004883b0 the big one
// DAT_007030ac (16 particles per light, mirrored in y), ONCE (DAT_007030b4) after the list changes.
// MASHED_LENSFLARE=0 turns both off (default ON).
bool RaceFx_LensEnabled();
struct LensLight { int type; float pos[3]; float k; };
// The list in creation order (TrackRenderer: City's 0x0044b100, Arctic's 0x00448940); `bob` = the
// index of the light whose k FUN_00448ef0 rewrites each tick (-1 none).
// after_sky: the big batch goes right after the first sky copy (Arctic's FUN_004492b0), else at the
// RaceSubmit_AddLensMarker queue point (City's slot 2).
void RaceFx_SetLensLights(const LensLight* l, int n, int bob, bool after_sky);
bool RaceFx_LensAfterSky();
void RaceFx_SetLensLightK(int index, float k);      // FUN_00488390 @ 0x00488390
void RaceFx_LensRequestBuild();                     // the tick whose FUN_00488e70 builds (b4 == 0)
// SkySecondaryDispatch @ 0x00489240 (the big batch): mode 0 = Arctic's FUN_004492b0 (after the sky,
// Z test off), mode 1 = City's slot 2 FUN_0044b8f0 (ZWRITE off, before its clumps).
void RaceFx_DrawLensBig(void* rw_camera, int mode);
// Called by RaceSubmit_Render at the start of the camera update: builds the batches on the
// requested frame with this frame's camera (FUN_00488e70's param, FUN_00467210(0)).
void RaceFx_BeginFrame(void* rw_camera);

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
    int   minus[4]   = {0, 0, 0, 0};       // DAT_008a9500[i] (FUN_0040b420), the last score delta
    int   alive[4]   = {1, 1, 1, 1};       // FUN_0046c7b0(i) (vehicle record +0x4)
    bool  world_up   = true;   // FUN_00426ba0 (DAT_0066d704)
};
// [rfx2] MASHED_HUD_NONTEAM=1 draws the non-team panels FUN_0041b630 (default OFF: no original
// capture with Team Play off exists to adjudicate them, U-9704).
bool RaceFx_HudNonTeamEnabled();
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
