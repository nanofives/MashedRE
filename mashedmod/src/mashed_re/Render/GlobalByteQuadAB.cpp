// Mashed RE — snapshot/restore A/B driver for 0x00490f50 / 0x00490f80.
//
// The cheapest possible end-to-end proof of the observation lane (pattern from
// Ai/AiControllerAB.cpp, which took 5 functions C2->C3 over ~79k paired calls).
//
// Per natural call, on the live pre-call state:
//   snapshot the write surface -> run PORT -> capture -> RESTORE -> run
//   ORIGINAL (prologue-re-exec trampoline) -> capture -> memcmp -> leave the
//   ORIGINAL's writes committed so the game stays original.
//
// Write surface: 4 bytes, derived mechanically by scripts/write_surface.py —
// "1 functions, 4 stores, by kind {'abs': 4}", nothing unresolved, no indirect
// dispatch. That is why this target was chosen: the snapshot set is provably
// COMPLETE, which is the property the whole method rests on. A driver whose
// snapshot set is incomplete produces a confident GREEN over unrestored state,
// which is worse than no verification at all.
//
// PROLOGUE / TRAMPOLINE. The inline JMP is 5 bytes and clobbers 0x00490f50..54,
// i.e. all of `mov al,[esp+4]` (4 B) AND the first byte of `mov cl,[esp+8]`.
// So the trampoline re-executes BOTH instructions and resumes at 0x00490f58.
// Verified against original\MASHED.exe.unpatched:
//   00490f50  8a 44 24 04     mov al,  byte ptr [esp+4]
//   00490f54  8a 4c 24 08     mov cl,  byte ptr [esp+8]
//   00490f58  8a 54 24 10     mov dl,  byte ptr [esp+0x10]   <- resume here
//
// Registration is env-gated (MASHED_BQ_AB=1) and REFUSES without
// MASHED_HOOK_ONLY, because these drivers sit at the same RVAs as the port
// hooks in GlobalByteQuad.cpp and would otherwise collide. Driver names AbBq50 /
// AbBq80 deliberately share no substring with the port names ByteQuadSet50 /
// ByteQuadSet80 — MASHED_HOOK_ONLY matches as a SUBSTRING BOTH WAYS, and a
// driver name containing a port name silently installs both and lets the port
// clobber the driver (AiControllerAB.cpp learned this as AbAiPreTick ⊃
// AiPreTick).
//
// A GREEN here is A/B with the hook installed at the target RVA, driven by
// natural call sites and arguments in a live race => C3 evidence. NOT C4: the
// committed result is the ORIGINAL's, so the port never steers the scenario.
#include "../Core/HookSystem.h"
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" void __cdecl ByteQuadSet50(int, int, int, int);
extern "C" void __cdecl ByteQuadSet80(int, int, int, int);

namespace {

void* g_orig_490f58 = reinterpret_cast<void*>(0x00490f58);
void* g_orig_490f88 = reinterpret_cast<void*>(0x00490f88);

__declspec(naked) void __cdecl Orig490f50(int, int, int, int) {
    __asm {
        mov al, byte ptr [esp + 4]      // 0x00490f50
        mov cl, byte ptr [esp + 8]      // 0x00490f54
        jmp dword ptr [g_orig_490f58]   // resume at mov dl,[esp+0x10]
    }
}
__declspec(naked) void __cdecl Orig490f80(int, int, int, int) {
    __asm {
        mov al, byte ptr [esp + 4]      // 0x00490f80
        mov cl, byte ptr [esp + 8]      // 0x00490f84
        jmp dword ptr [g_orig_490f88]
    }
}

void AbLog(const char* s) {
    HANDLE h = CreateFileA("bq_ab.log", FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, s, (DWORD)std::strlen(s), &w, nullptr);
    CloseHandle(h);
}

struct Stats { unsigned pairs, mism; };
Stats g_s50{0, 0}, g_s80{0, 0};

// The whole method in one function. `base` is the 4-byte write surface.
void RunPair(std::uintptr_t base, Stats& st, const char* tag,
             void (__cdecl* mine)(int, int, int, int),
             void (__cdecl* orig)(int, int, int, int),
             int a, int b, int c, int d) {
    volatile std::uint8_t* p = reinterpret_cast<volatile std::uint8_t*>(base);
    std::uint8_t snap[4], vmine[4], vorig[4];
    for (int i = 0; i < 4; ++i) snap[i] = p[i];

    mine(a, b, c, d);
    for (int i = 0; i < 4; ++i) vmine[i] = p[i];

    for (int i = 0; i < 4; ++i) p[i] = snap[i];      // RESTORE

    orig(a, b, c, d);
    for (int i = 0; i < 4; ++i) vorig[i] = p[i];     // ORIGINAL's writes stay

    ++st.pairs;
    if (std::memcmp(vmine, vorig, 4) != 0) {
        ++st.mism;
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s MISMATCH pair=%u args=%02x,%02x,%02x,%02x "
                      "mine=%02x%02x%02x%02x orig=%02x%02x%02x%02x\r\n",
                      tag, st.pairs, a & 0xff, b & 0xff, c & 0xff, d & 0xff,
                      vmine[0], vmine[1], vmine[2], vmine[3],
                      vorig[0], vorig[1], vorig[2], vorig[3]);
        AbLog(buf);
    } else if ((st.pairs % 500) == 0) {
        char buf[128];
        std::snprintf(buf, sizeof buf, "%s ok pairs=%u mism=%u\r\n",
                      tag, st.pairs, st.mism);
        AbLog(buf);
    }
}

void __cdecl AbBq50(int a, int b, int c, int d) {
    RunPair(0x00616030u, g_s50, "bq50", &ByteQuadSet50, &Orig490f50, a, b, c, d);
}
void __cdecl AbBq80(int a, int b, int c, int d) {
    RunPair(0x00616034u, g_s80, "bq80", &ByteQuadSet80, &Orig490f80, a, b, c, d);
}

struct AbBqReg {
    AbBqReg() {
        const char* s = std::getenv("MASHED_BQ_AB");
        if (!s || !s[0]) return;
        const char* only = std::getenv("MASHED_HOOK_ONLY");
        if (!only || !only[0]) {
            AbLog("MASHED_BQ_AB requires MASHED_HOOK_ONLY=<AbBq50|AbBq80> - "
                  "refusing to register (would collide with the port hooks at "
                  "the same RVAs)\r\n");
            return;
        }
        HookSystem::Register(0x00490f50u, reinterpret_cast<void*>(&AbBq50), "AbBq50");
        HookSystem::Register(0x00490f80u, reinterpret_cast<void*>(&AbBq80), "AbBq80");
    }
} g_ab_bq_reg;

} // namespace
