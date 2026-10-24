// Mashed RE — Audio/AudioFmtKey.h
// 0x005adf30 AudioFmtKeyCompare, the one body shared by both targets (Audio/AudioFmtKey.cpp).
#pragma once
#include <cstdint>

extern "C" __declspec(dllexport) int __cdecl AudioFmtKeyCompare(const std::uint8_t* param_1, const std::uint8_t* param_2);
