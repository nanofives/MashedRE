// Mashed RE — WS-H2: camera-cluster verbatim hooks (.asi-only).
//
// FUN_0040e180 MostSeparatedPair — the 3D most-separated active+alive+!dying car
// pair finder. Callee of BOTH the race camera director (0x00446520) and the
// elimination rule (0x00410d10), so a C4 here strengthens the whole cluster.
//
// Verbatim transcription of the Mashed_pool13 decomp (0x0040e180..0x0040e330).
// All car state is read via the original getters (forwarded to their RVAs):
//   FUN_0046c7b0(i)  alive  = *(int*)(0x008815a4 + i*0xd04)
//   FUN_0046cbb0(i,&dead,&sec) dead = *(int*)(0x00881f90 + i*0xd04)
//   FUN_0046d4a0(&ptr,i) car-struct ptr; pos read at +0x30/+0x34/+0x38
//   FUN_004c3ac0(&v)  Vec3Magnitude (RW fast-sqrt LUT) — forwarded → bit-exact
//   PTR_PTR_005f2770  slot-active table: *(int*)(*(int*)0x5f2770 + off)
//
// NOTE the output assignment matches the ORIGINAL, which is the OPPOSITE of the
// standalone RaceCamera::MostSeparatedPair: here *param_1 = best INNER index
// (iVar3), *param_2 = best OUTER index (iVar5=local_3c). Transcribed from the
// decomp, not the standalone.
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
inline std::int32_t SlotActive(int off) {
    return *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::int32_t*>(0x005f2770) + off);
}
// forwarders (cdecl) — route to the live originals (these are NOT hooked)
inline int  Alive(int i)        { return reinterpret_cast<int(__cdecl*)(unsigned)>(0x0046c7b0)(i); }
inline int  Dead(int i, int* a, int* b) {
    return reinterpret_cast<int(__cdecl*)(unsigned, int*, int*)>(0x0046cbb0)(i, a, b);
}
inline int  CarPtr(int* out, int i) {
    return reinterpret_cast<int(__cdecl*)(int*, unsigned)>(0x0046d4a0)(out, i);
}
inline float Vec3Mag(const float* v) {
    return reinterpret_cast<float(__cdecl*)(const float*)>(0x004c3ac0)(v);  // RW fast-sqrt LUT
}
}  // namespace

// 0x0040e180  void FUN_0040e180(int* param_1, int* param_2)
extern "C" __declspec(dllexport) void __cdecl MostSepPair_0040e180(int* param_1, int* param_2) {
    int iVar1, iVar3, iVar4, iVar5, iVar6;
    int local_38, local_28;     // FUN_0046cbb0 out (dead, secondary)
    int local_34;               // FUN_0046d4a0 out (car-struct ptr)
    float local_30;             // running best magnitude
    int local_3c, local_2c, iVar2;
    float local_24, local_20, local_1c;   // posA
    float d[3];                           // delta vector (MUST be contiguous for the LUT)
    float local_4;

    iVar5 = -1; iVar3 = -1; local_30 = 0.0f; local_3c = -1; iVar2 = 0; local_2c = 0x34;
    do {
        iVar4 = local_2c;
        if (((SlotActive(local_2c) != 0) && (iVar1 = Alive(iVar2), iVar1 == 1)) &&
            (Dead(iVar2, &local_38, &local_28), local_38 == 0)) {
            CarPtr(&local_34, iVar2);
            local_24 = *reinterpret_cast<float*>(local_34 + 0x30);
            local_20 = *reinterpret_cast<float*>(local_34 + 0x34);
            local_1c = *reinterpret_cast<float*>(local_34 + 0x38);
            iVar1 = 0; iVar6 = 0x34;
            do {
                if (((SlotActive(iVar6) != 0) && (iVar5 = Alive(iVar1), iVar5 == 1)) &&
                    (Dead(iVar1, &local_38, &local_28), local_38 == 0)) {
                    CarPtr(&local_34, iVar1);
                    local_4 = *reinterpret_cast<float*>(local_34 + 0x38);
                    d[0] = local_24 - *reinterpret_cast<float*>(local_34 + 0x30);
                    d[1] = local_20 - *reinterpret_cast<float*>(local_34 + 0x34);
                    d[2] = local_1c - local_4;
                    float mag = Vec3Mag(d);
                    if (local_30 <= mag) { local_30 = mag; iVar3 = iVar1; local_3c = iVar2; }
                }
                iVar6 = iVar6 + 4; iVar1 = iVar1 + 1; iVar4 = local_2c; iVar5 = local_3c;
            } while (iVar6 < 0x44);
        }
        local_2c = iVar4 + 4; iVar2 = iVar2 + 1;
    } while (local_2c < 0x44);
    if (iVar3 == -1) iVar3 = iVar5;
    if (iVar5 == -1) iVar5 = iVar3;
    if (iVar3 == -1) iVar3 = 0;
    if (iVar5 != -1) { *param_1 = iVar3; *param_2 = iVar5; return; }
    *param_1 = iVar3; *param_2 = 0;
}

RH_ScopedInstall(MostSepPair_0040e180, 0x0040e180);
