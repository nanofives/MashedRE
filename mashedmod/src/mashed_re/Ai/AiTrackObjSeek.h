// Mashed RE — AI course-0x21 track-object seek FUN_00414f00 (Ai/AiTrackObjSeek.cpp, seek414f00 2026-10-16).
#pragma once

#include <cstdint>

// 0x00414f00: the shared body (both targets; RH_ScopedInstall in the .asi). param_1 / param_2 are not read.
extern "C" __declspec(dllexport) int __cdecl AiTrackObjSeek(void* param_1, void* param_2, std::uint32_t* param_3, int param_4);

#ifdef MASHED_STANDALONE
namespace Ai {
// MASHED_NO_SEEK414F00 unset (default ON).
bool Seek414f00_Enabled();
// The exe's mode-10 call: runs the body for vehicle v, writes cand on a 1, counts calls for MASHED_SEEK_TRACE.
int AiTrackObjSeek_Exe(int v, float cand[2]);
}  // namespace Ai
#endif
