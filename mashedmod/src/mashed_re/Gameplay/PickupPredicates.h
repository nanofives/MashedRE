// Mashed RE — the shared pickup-lane predicates (Gameplay/PickupPredicates.cpp, both targets).
// Declared for the exe's naked callers in Gameplay/PickupTick_naked.inc and PickupCollect_naked.inc.
#pragma once

#include <cstdint>

extern "C" __declspec(dllexport) std::uint32_t __cdecl AnyActive4576b0(void);         // 0x004576b0
extern "C" __declspec(dllexport) int __cdecl PredNot45c330(int idx);                   // 0x0045c330
