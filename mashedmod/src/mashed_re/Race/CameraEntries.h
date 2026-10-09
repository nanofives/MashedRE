// Mashed RE — D-11073 leg 2: race-camera entry objects. See CameraEntries.cpp.
#pragma once
#include <cstdint>

extern "C" __declspec(dllexport) void __cdecl CamEntryBuild441b30(std::uint8_t* e, const float* row);   // 0x00441b30
extern "C" __declspec(dllexport) void __cdecl CamCarMidpoint441c80(float* out, const std::int32_t* cam); // 0x00441c80
extern "C" __declspec(dllexport) void __cdecl CamEntriesInit442600(void);                               // 0x00442600
extern "C" __declspec(dllexport) void __cdecl CamEntryType0Step445aa0(std::uint8_t* e, std::int32_t* cam); // 0x00445aa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraEntryDispatchAll(std::uint32_t param_1);  // 0x004464c0

#ifdef MASHED_STANDALONE
// Exe bindings for the callees the exe cannot reach at their RVA. Set by TrackRenderer.
struct CamEntryHost {
    int  (*track_id)();                       // FUN_00426c00 -> aib_track_index()
    void (*most_separated)(int* a, int* b);   // FUN_0040e180 -> RaceCamera::MostSeparatedPair
};
void CamEntry_SetHost(const CamEntryHost& h);
#endif
