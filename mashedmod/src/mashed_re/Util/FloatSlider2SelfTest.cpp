// Mashed RE - clamped float slider #2 (DAT_0068fcbc) with a video-gated audio tail.
// Original: 0x0045dbe0  FUN_0045dbe0  util  C2 -> C3 (NEEDS-BOOTED-RACE)
//
// void FUN_0045dbe0(int dir):
//   Half 1 (0x0045dbe0..0x0045dc4d): step the slider at DAT_0068fcbc by dir
//     (0 -= step1[0x005cc9c0]; 1 += step1; 2 = 0.0; >2 no step, re-clamp only) then clamp [0,1]
//     (bounds 1.0f @0x005cc320 / 0.0f @0x005d757c). Identical shape to FloatSliderStep (0x0045db50)
//     but on DAT_0068fcbc, and note the 3-case is a dec/je ladder here, not a jump table.
//   Half 2, the TAIL (0x0045dc4d..0x0045dc71): flag = VideoStateFlagGet(0) (0x00493f70, C4 —
//     reads DAT_00771a04); if flag != 0, call FUN_004943f0 (0x004943f0, audio C2) with the float
//     argument DAT_006900d4 * DAT_0068fcbc. FUN_004943f0 is a COM/DirectSound vtable call
//     (derefs the interface at DAT_00771a20, invokes [ecx+0x20], E_NOTIMPL-checked) — its side
//     effect belongs to ITS row, not this one.
//
// ACCEPTANCE (parent-specified 2026-09-01): an in-race A/B self-test in the ai/SteerDispatch shape.
// We witness what THIS function transcribes, not the callee's COM effect: three things compared
// bit-exactly per invocation — (1) the post-call slider DAT_0068fcbc, (2) the branch decision from
// VideoStateFlagGet (taken/not), (3) the float argument computed for FUN_004943f0 as a raw u32.
// CRITICAL INVARIANT: FUN_004943f0 must be invoked EXACTLY ONCE per call (a double COM call is a
// real side effect on live audio). The modded reimpl routes its COM call through a swappable
// fn-ptr that the dispatch points at a capture stub (records the arg, makes NO COM call); only the
// ORIGINAL, run via trampoline, makes the single real COM call. Env-gated MASHED_UTIL_SLIDER2_SELFTEST,
// own log float_slider2_selftest.log. Returns after the original pass, so behaviour is unchanged.
// asi-ONLY (linked in asi_sources.rsp, NOT build.bat): the reimpl calls MASHED VAs by fn-ptr.
//
// FLEET NOTE honoured: every absolute global read below uses ds:[imm] (a bare mov r,[imm] would
// assemble as mov-IMMEDIATE and load the ADDRESS, not the value — cost area/frontend a RED run).
// Byte-verified with dumpbin (see PROMOTION_QUEUE row).
#include "../Core/HookSystem.h"
#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace {

// COM callee, routed so the dispatch can suppress the modded side's call (default = real VA).
void* g_slider2_com  = reinterpret_cast<void*>(0x004943f0);   // FUN_004943f0 (audio COM)
void* g_493f70       = reinterpret_cast<void*>(0x00493f70);   // VideoStateFlagGet (pure read)
void* g_orig_45dbe7  = reinterpret_cast<void*>(0x0045dbe7);   // trampoline re-entry (post stolen bytes)

// Capture state for the modded pass (the COM call is suppressed and its arg recorded here).
volatile std::uint32_t s_modArgBits   = 0;
volatile long          s_modComCalled = 0;

// Capture stub the modded COM call is routed to during the self-test. Reads the float arg the
// reimpl fstp'd onto the stack ([esp+4] after the call's own return-address push), records it,
// and returns WITHOUT any COM interaction. Signature matches the original's __cdecl-ish call
// (caller cleans the pushed dword via `pop ecx`).
__declspec(naked) void CaptureComStub() {
    __asm {
        mov  eax, [esp + 4]
        mov  s_modArgBits, eax
        mov  s_modComCalled, 1
        ret
    }
}

// Verbatim transcription of 0x0045dbe0..0x0045dc71 (the modded reimpl). The two callees are
// re-issued indirectly through globals (MSVC would otherwise encode a wrong relative displacement
// in a relocated DLL): VideoStateFlagGet via g_493f70, and FUN_004943f0 via g_slider2_com
// (suppressible). Every global read is ds:[imm].
__declspec(naked) void Slider2_Reimpl(int /*dir*/) {
    __asm {
        mov    eax, dword ptr [esp + 4]            // dir
        sub    eax, 0
        je     L_dec                                // dir 0 -> decrement
        dec    eax
        je     L_inc                                // dir 1 -> increment
        dec    eax
        jne    L_clamp_high                         // dir > 2 -> re-clamp only
        jmp    L_zero                               // dir 2 -> zero
    L_inc:
        fld    dword ptr ds:[0x0068fcbc]
        fadd   dword ptr ds:[0x005cc9c0]            // += step1
        jmp    L_store
    L_dec:
        fld    dword ptr ds:[0x0068fcbc]
        fsub   dword ptr ds:[0x005cc9c0]            // -= step1
    L_store:
        fstp   dword ptr ds:[0x0068fcbc]
    L_clamp_high:
        fld    dword ptr ds:[0x0068fcbc]
        fcomp  dword ptr ds:[0x005cc320]            // vs 1.0f
        fnstsw ax
        test   ah, 0x41
        jne    L_clamp_low
        mov    dword ptr ds:[0x0068fcbc], 0x3f800000 // clamp to 1.0
        jmp    L_tail
    L_clamp_low:
        fld    dword ptr ds:[0x0068fcbc]
        fcomp  dword ptr ds:[0x005d757c]            // vs 0.0f
        fnstsw ax
        test   ah, 5
        jp     L_tail
    L_zero:
        mov    dword ptr ds:[0x0068fcbc], 0          // clamp/zero to 0.0
    L_tail:
        push   0
        call   dword ptr [g_493f70]                  // VideoStateFlagGet(0)
        add    esp, 4
        test   eax, eax
        je     L_ret                                 // flag == 0 -> no COM call
        fld    dword ptr ds:[0x006900d4]             // scale
        push   ecx
        fmul   dword ptr ds:[0x0068fcbc]             // scale * slider
        fstp   dword ptr [esp]                       // -> stack arg
        call   dword ptr [g_slider2_com]             // FUN_004943f0 (suppressed in self-test)
        pop    ecx
    L_ret:
        ret
    }
}

// Original trampoline: the 5-byte RH_ScopedInstall JMP overwrites MOV EAX,[ESP+4] (4) + the first
// byte of SUB EAX,0 (5). Re-exec both stolen instructions, then jmp to the clean boundary
// 0x0045dbe7 (JE 0x0045dbff). The original body runs FULLY, incl. its single real COM call.
__declspec(naked) void OrigSlider2(int /*dir*/) {
    __asm {
        mov    eax, dword ptr [esp + 4]
        sub    eax, 0
        jmp    dword ptr [g_orig_45dbe7]
    }
}

// Recompute the tail's float argument the way the original does (x87: scale * slider), reading the
// CURRENT DAT_0068fcbc. Called after the original pass, when DAT_0068fcbc holds the original's
// post-clamp slider, so this is bit-identical to the arg the original passed.
__declspec(naked) std::uint32_t Slider2ArgRecompute() {
    __asm {
        push   ecx
        fld    dword ptr ds:[0x006900d4]
        fmul   dword ptr ds:[0x0068fcbc]
        fstp   dword ptr [esp]
        mov    eax, [esp]
        pop    ecx
        ret
    }
}

inline int Slider2SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_UTIL_SLIDER2_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void Slider2SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("float_slider2_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_slider2_calls = 0, g_slider2_mismatch = 0;
// Coverage counters (2026-09-01, parent booted-race lane). A GREEN count alone cannot tell
// "every compared field was exercised" from "the branch never fired and only the slider half
// was ever compared" — argBad is gated on origFlag!=0. These make the degenerate shape visible
// in the log itself: flagTaken = calls where the COM branch was taken (arg half compared),
// moved = calls where the slider value actually changed, argFold = XOR of the compared float
// bits (0 or constant => a single repeated value), dirMask = bitmask of the dir values seen.
long g_slider2_flagTaken = 0, g_slider2_moved = 0;
std::uint32_t g_slider2_argFold = 0, g_slider2_dirMask = 0;
const long kSlider2MaxCompare = 40000;

// A/B dispatch. Snapshot the slider, run the modded reimpl with the COM call suppressed (capture
// the arg + branch), restore the slider, then run the ORIGINAL (single real COM call). Compare the
// post-call slider, the branch decision, and — when the branch is taken — the float argument, all
// bit-exactly. Return after the original pass (slider left at the original's value: behaviour
// unchanged). When the env var is unset, this is a pure passthrough to the original.
void Slider2Dispatch(int dir) {
    if (Slider2SelfTestEnabled() && g_slider2_calls < kSlider2MaxCompare) {
        const std::uint32_t snap = *reinterpret_cast<volatile std::uint32_t*>(0x0068fcbc);

        // modded pass — COM suppressed
        s_modComCalled = 0; s_modArgBits = 0;
        g_slider2_com = reinterpret_cast<void*>(&CaptureComStub);
        Slider2_Reimpl(dir);
        const std::uint32_t modSlider = *reinterpret_cast<volatile std::uint32_t*>(0x0068fcbc);
        const long          modFlag   = s_modComCalled;   // 1 iff the branch was taken
        const std::uint32_t modArg    = s_modArgBits;
        g_slider2_com = reinterpret_cast<void*>(0x004943f0);

        // restore, then original pass — the single real COM call happens here
        *reinterpret_cast<volatile std::uint32_t*>(0x0068fcbc) = snap;
        OrigSlider2(dir);
        const std::uint32_t origSlider = *reinterpret_cast<volatile std::uint32_t*>(0x0068fcbc);
        const long          origFlag   = (*reinterpret_cast<volatile std::int32_t*>(0x00771a04) != 0) ? 1 : 0;
        const std::uint32_t origArg    = Slider2ArgRecompute();

        ++g_slider2_calls;
        if (origFlag != 0) { ++g_slider2_flagTaken; g_slider2_argFold ^= origArg; }
        if (origSlider != snap) ++g_slider2_moved;
        g_slider2_dirMask |= (dir >= 0 && dir < 32) ? (1u << dir) : 0x80000000u;
        const bool sliderBad = (modSlider != origSlider);
        const bool flagBad   = (modFlag  != origFlag);
        const bool argBad    = (origFlag != 0) && (modArg != origArg);
        if (sliderBad || flagBad || argBad) {
            ++g_slider2_mismatch;
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH slider m=%08X o=%08X | flag m=%ld o=%ld | arg m=%08X o=%08X\r\n",
                      g_slider2_calls, modSlider, origSlider, modFlag, origFlag, modArg, origArg);
            Slider2SelfTestLog(line);
        }
        if ((g_slider2_calls & 0x7f) == 1) {
            char line[224];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld flagTaken=%ld moved=%ld argFold=%08X dirMask=%08X %s\r\n",
                      g_slider2_calls, g_slider2_calls, g_slider2_mismatch,
                      g_slider2_flagTaken, g_slider2_moved, g_slider2_argFold, g_slider2_dirMask,
                      g_slider2_mismatch ? "" : "ALL-GREEN");
            Slider2SelfTestLog(line);
        }
        return;   // original already applied the real step + COM call
    }
    OrigSlider2(dir);
}

// Naked entry installed at 0x0045dbe0 — __cdecl void(int dir): forward dir to the dispatch, RET.
__declspec(naked) void Slider2_Entry() {
    __asm {
        mov    eax, dword ptr [esp + 4]
        push   eax
        call   Slider2Dispatch
        add    esp, 4
        ret
    }
}

}  // namespace

RH_ScopedInstall(Slider2_Entry, 0x0045dbe0);
