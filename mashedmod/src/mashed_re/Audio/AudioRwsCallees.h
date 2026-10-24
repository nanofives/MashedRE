// Mashed RE — Audio/AudioRwsCallees.h
// Declarations of the ported callees Audio/AudioRws.cpp calls by name (D4-AUDIO-RWS3 slice 3). Each has ONE body,
// linked into both targets: AudioFmtEntryMatch (Audio/RwsFmt.cpp), AudioPoolFree (Audio/AudioPool.cpp),
// AudioTreeWalkPredicateSearch (Audio/RwsStream.cpp). In a header so the rva-body scan does not read a declaration
// as a second body.
#pragma once
#include <cstdint>

extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioFmtEntryMatch(std::uint32_t* param_1, std::uint32_t* param_2);
extern "C" __declspec(dllexport) void __cdecl AudioPoolFree(std::uint32_t* param_1, std::uint32_t param_2);
extern "C" __declspec(dllexport) std::uint32_t* __cdecl AudioTreeWalkPredicateSearch(
        std::uint32_t* param_1, int* param_2, void* param_3, std::uint32_t param_4, int param_5);
