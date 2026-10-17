// Mashed RE — D-11073 leg 2: race-camera entry objects. See CameraEntries.cpp.
#pragma once
#include <cstdint>

extern "C" __declspec(dllexport) void __cdecl CamEntryBuild441b30(std::uint8_t* e, const float* row);   // 0x00441b30
extern "C" __declspec(dllexport) void __cdecl CamCarMidpoint441c80(float* out, const std::int32_t* cam); // 0x00441c80
extern "C" __declspec(dllexport) void __cdecl CamEntriesInit442600(void);                               // 0x00442600
extern "C" __declspec(dllexport) void __cdecl CamEntryType0Step445aa0(std::uint8_t* e, std::int32_t* cam); // 0x00445aa0
extern "C" __declspec(dllexport) void __cdecl CamEntryType1Step441d40(std::uint8_t* e, std::int32_t* cam); // 0x00441d40 [rprops2 U-9699]
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraEntryDispatchAll(std::uint32_t param_1);  // 0x004464c0

// [rprops2 U-9699] 0x00441d40 with the aim target and the progress passed in: target = the midpoint of
// the two car positions pa / pb (0x00441ce2..0x00441d2c), prog = what FUN_00408a50(cam[8]) returns.
void CamEntryType1StepWith(std::uint8_t* e, const float* pa, const float* pb, double prog);

namespace mashed_re { namespace CamEntry {
// [rprops2] Row k of the table for a Course_Id as the 12 floats CamEntryBuild441b30 takes, null when out of range.
const float* Row(int course_id, int k);
// [rprops] Row k (0-based, the same k CamEntriesInit442600 feeds CamEntryBuild441b30) of the camera-entry
// table for a Course_Id (the switch at 0x0044261b; any id without a table takes the default arm
// 0x005f82a8). xyz = row[0..2] (entry + 0x3c..+0x44, what 0x00442410 returns), yaw = row[4] (entry + 0xb0,
// what 0x00442420 returns BEFORE the type-1 step 0x00441d40 rewrites it). False when k is out of range.
bool TableRow(int course_id, int k, float xyz[3], float* yaw);
} }

#ifdef MASHED_STANDALONE
// Exe bindings for the callees the exe cannot reach at their RVA. Set by TrackRenderer.
struct CamEntryHost {
    int  (*track_id)();                       // FUN_00426c00 -> aib_track_index()
    void (*most_separated)(int* a, int* b);   // FUN_0040e180 -> RaceCamera::MostSeparatedPair
    double (*path_progress)(int idx);         // [rprops2] FUN_00408a50 (type-1 weight only); null -> 0.0
};
void CamEntry_SetHost(const CamEntryHost& h);
#endif
