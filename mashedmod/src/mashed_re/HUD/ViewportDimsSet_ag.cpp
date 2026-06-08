// Mashed RE - c3_batch_ag harness-ext: viewport dims/ratio setter.
// One conditional callee (FUN_004c0e50, C2) gated on *(this+4)!=0; the diff seeds a
// zeroed scratch struct so the gate is 0 and the callee path is skipped.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x004c1c80  FUN_004c1c80  (63 bytes)
// Stores viewport dims param_2[0/1] at this+0x68/0x6c, then reciprocal-scale ratios
// at this+0x70/0x74 = DAT_005cc320 / (float)dim. If *(this+4)!=0, marks the camera
// viewport dirty via FUN_004c0e50.
// Decomp (pool13 2026-06-08, matches re/analysis/hud_ingame_d2/0x004c1c80.md):
//   fVar1 = _DAT_005cc320;
//   *(this+0x68) = *param_2;  *(this+0x6c) = param_2[1];
//   *(float*)(this+0x70) = fVar1 / *(float*)(this+0x68);
//   *(float*)(this+0x74) = _DAT_005cc320 / *(float*)(this+0x6c);
//   if (*(this+4) != 0) FUN_004c0e50(*(this+4));
//   return this;
// NB: the dim words stored at +0x68/+0x6c are re-read as float for the ratios.
extern "C" __declspec(dllexport) int __cdecl ViewportDimsSet(int param_1, uint32_t* param_2) {
    const float fVar1 = *reinterpret_cast<float*>(0x005cc320u);
    char* const p = reinterpret_cast<char*>(param_1);
    *reinterpret_cast<uint32_t*>(p + 0x68) = param_2[0];
    *reinterpret_cast<uint32_t*>(p + 0x6c) = param_2[1];
    *reinterpret_cast<float*>(p + 0x70) = fVar1 / *reinterpret_cast<float*>(p + 0x68);
    *reinterpret_cast<float*>(p + 0x74) =
        *reinterpret_cast<float*>(0x005cc320u) / *reinterpret_cast<float*>(p + 0x6c);
    if (*reinterpret_cast<int*>(p + 4) != 0) {
        reinterpret_cast<void(__cdecl*)(int)>(0x004c0e50u)(*reinterpret_cast<int*>(p + 4));
    }
    return param_1;
}

RH_ScopedInstall(ViewportDimsSet, 0x004c1c80);  // c3_batch_ag harness-ext 2026-06-08
