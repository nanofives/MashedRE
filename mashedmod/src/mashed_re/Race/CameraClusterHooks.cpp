// Mashed RE — WS-H2: camera-cluster hooks (.asi-only).
//
// FUN_0040e180 MostSeparatedPair — the 3D most-separated active+alive+!dying car
// pair finder. Callee of BOTH the race camera director (0x00446520) and the
// elimination rule (0x00410d10), so a C4 here strengthens the whole cluster.
//
// dcmix (2026-10-22): the body is Race/MostSepPair.cpp RaceCamera::MostSeparatedPair, shared with the exe. This
// export reads the car state through the original getters and hands it over as RaceCamCar[4]:
//   PTR_PTR_005f2770  slot-active table: *(int*)(*(int*)0x5f2770 + 0x34 + 4i)       (0x0040e1b5)
//   FUN_0046c7b0(i)   alive  = *(int*)(0x008815a4 + i*0xd04), tested == 1
//   FUN_0046cbb0(i,&dead,&sec) dead = *(int*)(0x00881f90 + i*0xd04), tested == 0
//   FUN_0046d4a0(&ptr,i) car-struct ptr; pos read at +0x30/+0x34/+0x38
// The original calls the getters again inside its inner loop; they only read (and FUN_0046cbb0 writes its two
// out-params), so reading them once per car gives the same values. pos is read only for a car that passes all three
// tests, as the original does, and through the pointer FUN_0046d4a0 returns, unchecked, as the original does.
// Output order is the original's: *param_1 = best INNER index, *param_2 = best OUTER index.
//
// With MASHED_LEAF_SELFTEST=1 every call also runs the original twice through a trampoline (noo) and compares both
// outputs with the shared body (ndiff), logging to leaf_selftest.log in the CWD (the ForceIntegratorLeaves.cpp
// pattern). The game gets the shared body's pair either way.
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include "RaceCamera.h"
#include <windows.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

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

void FillCars(mashed_re::Race::RaceCamCar cars[4]) {
    std::memset(cars, 0, 4 * sizeof(cars[0]));
    for (int i = 0; i < 4; ++i) {
        cars[i].active = SlotActive(0x34 + 4 * i) != 0;
        if (!cars[i].active) continue;
        cars[i].alive = Alive(i) == 1;
        if (!cars[i].alive) continue;
        int dead = 0, sec = 0;
        Dead(i, &dead, &sec);
        cars[i].dead_flag = dead != 0;
        if (cars[i].dead_flag) continue;
        int p = 0;
        CarPtr(&p, i);
        cars[i].pos[0] = *reinterpret_cast<float*>(p + 0x30);
        cars[i].pos[1] = *reinterpret_cast<float*>(p + 0x34);
        cars[i].pos[2] = *reinterpret_cast<float*>(p + 0x38);
    }
}

// Trampoline into the original: the 5-byte E9 hook covers 0x0040e180..0x0040e184 = SUB ESP,0x3C (3) + PUSH EBX +
// PUSH EBP, so re-run those and resume at 0x0040e185.
void* g_mostSepResume = reinterpret_cast<void*>(0x0040e185);
__declspec(naked) void __cdecl MostSepOrig(int*, int*) {
    __asm {
        sub  esp, 0x3c
        push ebx
        push ebp
        jmp  dword ptr [g_mostSepResume]
    }
}

int SelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
long SelfTestCap() {
    static long v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST_CAP"); v = (s && s[0]) ? std::atol(s) : 3000; }
    return v;
}
void Log(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_noo = 0, g_ndiff = 0, g_pairs = 0;

__declspec(noinline) void Shared(int* a, int* b) {
    mashed_re::Race::RaceCamCar cars[4];
    FillCars(cars);
    mashed_re::Race::RaceCamera::MostSeparatedPair(cars, a, b);
}
}  // namespace

// Install entry at 0x0040e180, void FUN_0040e180(int* param_1, int* param_2): fills the cars, runs the shared body.
extern "C" __declspec(dllexport) void __cdecl MostSepPair_0040e180(int* param_1, int* param_2) {
    if (!SelfTestOn()) { Shared(param_1, param_2); return; }
    int oa = -7, ob = -7, oa2 = -7, ob2 = -7, ma = -7, mb = -7;
    MostSepOrig(&oa, &ob);
    MostSepOrig(&oa2, &ob2);
    Shared(&ma, &mb);
    ++g_calls;
    const int noo = (oa != oa2 || ob != ob2), nd = (ma != oa || mb != ob);
    g_noo += noo; g_ndiff += nd; g_pairs += (oa != ob);
    if (g_calls <= SelfTestCap() || noo || nd) {
        char line[160];
        wsprintfA(line, "[%ld] fn=40e180 o=%d,%d o2=%d,%d m=%d,%d noo=%d ndiff=%d\r\n", g_calls, oa, ob, oa2, ob2, ma, mb,
                  noo, nd);
        Log(line);
    }
    if (g_calls % 1000 == 0) {
        char line[160];
        wsprintfA(line, "SUM fn=40e180 calls=%ld noo=%ld ndiff=%ld distinct=%ld\r\n", g_calls, g_noo, g_ndiff, g_pairs);
        Log(line);
    }
    *param_1 = ma; *param_2 = mb;
}

RH_ScopedInstall(MostSepPair_0040e180, 0x0040e180);
