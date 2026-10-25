// Mashed RE - Frontend/MenuMeasure.h
// The one body of FUN_004282a0 (menu message width), shared by both targets. See MenuMeasure.cpp.
#pragma once
#include <cstdint>

// The body. __cdecl, result left in ST0 (the original's float10 return).
extern "C" __declspec(dllexport) float __cdecl MenuMenusBA(std::uint32_t id, float scale);

// Per-target cells, defined by the host TU of each target (Frontend/MenuMenusB.cpp for the .asi,
// exe_main.cpp for mashed_re.exe). Each callee keeps the original's ABI:
//   g_menuBa_Lookup   cdecl (id), result in EAX                          FUN_00427780
//   g_menuBa_Remap    register ABI: EAX = Lookup's result, EBX = buffer   FUN_004277a0
//   g_menuBa_Measure  cdecl (font, buffer, size), result in ST0           FUN_005554d0
//   g_menuBa_FontCell address of the dword pushed as the font argument    [0x0067d838]
//   g_menuBa_DivCell  address of the float divisor                         [0x0067d830]
extern "C" void*        g_menuBa_Lookup;
extern "C" void*        g_menuBa_Remap;
extern "C" void*        g_menuBa_Measure;
extern "C" const void*  g_menuBa_FontCell;
extern "C" const float* g_menuBa_DivCell;
