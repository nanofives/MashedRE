// PORT-TOP-04A (L-96 top-down lane): the pieces of the original's DirectInput gate / exit chain that have no matched
// source, for the MASHED_ORIG_TOP path. EXE ONLY. The matched bodies are in Match/PortTop_<rva>.cpp
// (verify/porttop_20261108/gen_top04a_tus.py). Table: verify/porttop_20261108/BINDINGS.tsv.
//
// The exe maps the original data range zero-filled, and nothing in the exe's hand path reads the globals this chain
// writes (the hand input keeps its own state in exe_main.cpp: g_dinput / g_kbd / g_input_slot_type). The chain makes
// its own IDirectInput8 object and a non-exclusive foreground keyboard device (cooperative level 6), as the original
// does; neither is acquired here, and the exit chain releases both.

#ifdef MASHED_STANDALONE

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace {

void Log(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

struct Seed { unsigned va; unsigned n; const char* bytes; };
const Seed kSeed[] = {
#include "PortTop_DInputSeed.inc"
};

// Original DIDATAFORMAT header at 0x005d0974 (re-read from the anchored binary by gen_dinput_seed.py):
// dwSize 0x18, dwObjSize 0x10, dwFlags 2, dwDataSize 0x100, dwNumObjs 0x100.
const DWORD kDfKbdHdr[5] = { 0x18u, 0x10u, 0x2u, 0x100u, 0x100u };

}  // namespace

extern "C" {

// Knob only, before WinMain 0x00492370 reaches 0x00493540: the IID (0x005d0a8c) and keyboard GUID (0x005d09ec) bytes
// from the original image, and at 0x005d0974 a copy of the exe's own c_dfDIKeyboard (dinput8.lib). The original's
// DIDATAFORMAT points its object array at 0x0049b5d0, inside the original code range, which the exe does not map as
// original data, so its bytes cannot be written back as they are. c_dfDIKeyboard is the same format (header checked
// below) with pointers valid in the exe.
int PortStateImage_AppliedC(void);   // Compat/PortStateImage.cpp (IMG-2-LOADER)
void PortTop_SeedDInput(void) {
    unsigned done = 0;
    const int image = PortStateImage_AppliedC();   // the two byte entries are in the image (witnessed there)
    for (const Seed& s : kSeed) {
        if (image) { ++done; continue; }
        __try {
            std::memcpy(reinterpret_cast<void*>(static_cast<UINT_PTR>(s.va)), s.bytes, s.n);
            ++done;
        } __except (1) {
            Log("dinput seed 0x%08X (%u bytes) faulted", s.va, s.n);
        }
    }
    const DWORD* h = reinterpret_cast<const DWORD*>(&c_dfDIKeyboard);
    const bool same = std::memcmp(h, kDfKbdHdr, sizeof(kDfKbdHdr)) == 0;
    __try {
        std::memcpy(reinterpret_cast<void*>(static_cast<UINT_PTR>(0x005d0974u)), &c_dfDIKeyboard, sizeof(DIDATAFORMAT));
        ++done;
    } __except (1) {
        Log("dinput seed 0x005D0974 (c_dfDIKeyboard) faulted");
    }
    Log("dinput seed: %u/3 written; c_dfDIKeyboard header %s the original's (0x18 0x10 2 0x100 0x100)", done,
        same ? "equals" : "DIFFERS FROM");
}

// rva 0x0049b300 = the DirectInput8Create import thunk (hooks.csv C2). The real dinput8 export.
long __stdcall PortTop_DirectInput8Create_0049b300(HMODULE hinst, unsigned long ver, void* riid, void* out, void* outer) {
    return DirectInput8Create(hinst, ver, *static_cast<const IID*>(riid), static_cast<LPVOID*>(out),
                              static_cast<LPUNKNOWN>(outer));
}

// rva 0x004a42c5 = CRT vsprintf (third-party-library[msvc-crt-main]); 0x004987b0 passes the address of its first
// vararg, which is the va_list on x86.
int PortTop_Crt_004a42c5(char* out, const char* fmt, char* ap) {
    return std::vsprintf(out, fmt, reinterpret_cast<va_list>(ap));
}

// rva 0x00495520 getter + matched callees used by the port bodies below (C linkage, defined in their TUs).
int* Get771e78(void);                         // Match/MatchS16_00495520.cpp
int  JoypadEnumerate_00496040(void);          // Match/PortTop_00496040.cpp
int  KeyboardDeviceCreate_00496320(void* hwnd);  // Match/PortTop_00496320.cpp
void KeyboardRelease_00496370(void);          // Match/PortTop_00496370.cpp

// rva 0x004960a0, no matched source. Port body from the decomp (decomp_pc.py, 2026-11-08): calls 0x00495520 (result
// unused), stores the window at 0x00772fb0, runs 0x00496040; on success writes 0x10 to 0x00771e80 and 0x3eb33333 to
// 0x00772fa8, returns 1; else 0.
int PortTop_Port_004960a0(void* hwnd) {
    (void)Get771e78();
    *reinterpret_cast<void* volatile*>(0x00772fb0u) = hwnd;
    if (JoypadEnumerate_00496040() == 0) return 0;
    *reinterpret_cast<volatile unsigned*>(0x00771e80u) = 0x10u;
    *reinterpret_cast<volatile unsigned*>(0x00772fa8u) = 0x3eb33333u;
    return 1;
}

// rva 0x004963b0, no matched source. Port body from the decomp: 0x00772fb4 = 0x00495520(), returns
// rva 0x00496320(hwnd) != 0.
int PortTop_Port_004963b0(void* hwnd) {
    *reinterpret_cast<int* volatile*>(0x00772fb4u) = Get771e78();
    return KeyboardDeviceCreate_00496320(hwnd) != 0;
}

// rva 0x004963d0 = thunk to 0x00496370 (hooks.csv thunk_FUN_00496370, no matched source).
void PortTop_Port_004963d0(void) {
    KeyboardRelease_00496370();
}

// rva 0x00495ee0: the EnumDevices callback 0x00496040 installs (per-joypad CreateDevice / SetDataFormat / properties
// into the 0x448-byte records at 0x00771e88, count 0x00772fac). No matched source, and its joystick data format lives
// in the original code range like the keyboard one. Missing: creates no device and returns DIENUM_CONTINUE, so
// 0x00772fac stays 0. The hand path enumerates controllers itself (exe_main.cpp InitDirectInput).
int __stdcall PortTop_Missing_00495ee0(void* inst, void* ctx) {
    (void)inst; (void)ctx;
    return DIENUM_CONTINUE;
}

// rva 0x00498510: no longer a stub here. Provisional body from re/match/s16/00498510.c in Match/PortTop_00498510.cpp
// (BIND-CRT-498510).

}  // extern "C"

#endif  // MASHED_STANDALONE
