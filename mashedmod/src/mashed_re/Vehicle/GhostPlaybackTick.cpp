// Mashed RE - Ghost::PlaybackTick.
// Original: 0x00411ae0  FUN_00411ae0  vehicle  C2 -> C3 candidate (booted, replay-driven)
//
// void FUN_00411ae0(int param_1, uint param_2):
//   PRE-GATE HALF (0x00411ae0..0x00411b65), runs in EVERY mode:
//     if (DAT_0063bb24 != 0)                      // override flag (U-new1: purpose unestablished)
//         param_2 = DAT_007f107a + DAT_0063bb1c
//                 + ((DAT_007f1039/10 - DAT_007f1038/10) - DAT_007f1079);
//     DAT_0063bb1c = param_2;                     // the playback cursor
//     if (DAT_0063bb10 != 0 && *(u32*)(DAT_0063bb10+0x174) < param_2)
//         DAT_0063bb1c = *(u32*)(DAT_0063bb10+0x174);   // clamp to the best lap's length
//   MODE-2 HALF (0x00411b74..0x00411cc6), Time Trial only: build two identity 4x4 matrices on the
//     stack, fill each one's flag word with (uninitialised stack | 0x20003), then
//     if (DAT_0063bb0c) ReadFrame(ghost,    &m_ghost, DAT_0063bb1c);   // 0x00482c10
//     if (DAT_0063bb10) ReadFrame(best_lap, &m_best,  DAT_0063bb1c);
//     FUN_0041a9b0(&ghost_desc);   // S-1569 apply ghost transform
//     FUN_0041ad00(&best_desc);    // S-1570 apply best-lap transform
//
// The two `| 0x20003` reads at [ESP+0x60] and [ESP+0x20] are of UNINITIALISED stack in the
// original (Ghidra shows local_34/local_74 OR'd before any store). That is transcribed as-is
// rather than "fixed": the whole point of a verbatim port is to carry the original's behaviour,
// and normalising those to 0 would silently change the matrix flag words.
//
// EVIDENCE / ACCEPTANCE (2026-09-02, parent booted lane). The witnessing observable is the cursor
// DAT_0063bb1c, established by measurement rather than taken from the notes (the six existing
// notes propose no observable for this row). Under the replayed human scenario 003-race-drive the
// original writes it on 48/48 calls:
//     args [0x32, 0x1612, ...] -> DAT_0063bb1c = 0x1612
//     args [0x32, 0x1644, ...] -> DAT_0063bb1c = 0x1644
// i.e. cursor := param_2, stepping by param_1 per tick.
//
// COVERAGE WARNING, stated because it decides what a GREEN here is worth: the auto-driver
// (scenario_launch.py) passes param_2 == 0 on every call, so the write stores the value the cursor
// already held and is INVISIBLE to a delta test - that is what made this row look inert on first
// measurement. A run of this self-test is only meaningful when p2Distinct > 1. The counters below
// report that, plus whether the mode-2 body and each of its three branches were reached, so a
// green can be read for what it actually covered instead of assumed to be total.
//
// asi-ONLY (asi_sources.rsp, NOT build.bat): the reimpl calls MASHED VAs by fn-ptr.
// Every absolute global read uses ds:[imm] - a bare `mov r,[imm]` assembles as mov-IMMEDIATE and
// loads the ADDRESS, not the value (fleet note; cost area/frontend a RED run).
#include "../Core/HookSystem.h"
#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace {

// Callees, routed through globals so the dispatch can suppress the MODDED pass's side effects.
// FUN_0041a9b0 / FUN_0041ad00 apply transforms to the ghost and best-lap vehicles - running them
// twice per tick is a real, visible side effect on live entities, not harness noise. Same
// single-side-effect invariant the slider2 self-test holds for its COM call.
void* g_pbt_42f6a0 = reinterpret_cast<void*>(0x0042f6a0);   // mode getter (pure read, safe twice)
void* g_pbt_482c10 = reinterpret_cast<void*>(0x00482c10);   // Replay::ReadFrame
void* g_pbt_41a9b0 = reinterpret_cast<void*>(0x0041a9b0);   // S-1569 apply ghost transform
void* g_pbt_41ad00 = reinterpret_cast<void*>(0x0041ad00);   // S-1570 apply best-lap transform
void* g_orig_411ae5 = reinterpret_cast<void*>(0x00411ae5);  // trampoline re-entry (post stolen bytes)

// Modded-pass capture state.
volatile long          s_readFrameCalls = 0;
volatile std::uint32_t s_readFrameTime  = 0;   // the DAT_0063bb1c passed to the last ReadFrame
volatile long          s_applyGhost     = 0;
volatile long          s_applyBest      = 0;

// __cdecl(replay, outMatrix, time) - record and return, touching nothing.
__declspec(naked) void CaptureReadFrame() {
    __asm {
        mov  eax, [esp + 12]           // time
        mov  s_readFrameTime, eax
        inc  s_readFrameCalls
        ret
    }
}
// __cdecl(desc) x2 - record only; no transform applied to any live vehicle.
__declspec(naked) void CaptureApplyGhost() { __asm { inc s_applyGhost
                                                     ret } }
__declspec(naked) void CaptureApplyBest()  { __asm { inc s_applyBest
                                                     ret } }

// Verbatim transcription of 0x00411ae0..0x00411cd0. Instruction order is load-bearing beyond the
// usual: the JZ at 0x00411c7d consumes the flags set by the TEST EAX,EAX at 0x00411b92, and every
// instruction between them is a LEA or MOV precisely because neither touches flags.
__declspec(naked) void PlaybackTick_Reimpl(int /*p1*/, unsigned /*p2*/) {
    __asm {
        mov    eax, dword ptr ds:[0x0063bb24]        // 00411ae0  override flag
        sub    esp, 0x90                             // 00411ae5
        test   eax, eax                              // 00411aeb
        push   esi                                   // 00411aed
        jnz    L_override                            // 00411aee
        mov    eax, dword ptr [esp + 0x9c]           // 00411af0  param_2
        jmp    L_store_cursor                        // 00411af7
    L_override:                                      // 00411af9
        movzx  ecx, byte ptr ds:[0x007f1039]
        mov    eax, 0x66666667                       // magic reciprocal for /10
        imul   ecx
        movzx  ecx, byte ptr ds:[0x007f1038]
        sar    edx, 2
        mov    eax, edx
        shr    eax, 0x1f
        add    edx, eax
        mov    esi, edx
        mov    eax, 0x66666667
        imul   ecx
        movzx  eax, byte ptr ds:[0x007f107a]
        sar    edx, 2
        mov    ecx, edx
        shr    ecx, 0x1f
        add    edx, ecx
        mov    ecx, dword ptr ds:[0x0063bb1c]
        sub    esi, edx
        movzx  edx, byte ptr ds:[0x007f1079]
        sub    esi, edx
        add    eax, ecx
        add    eax, esi
    L_store_cursor:                                  // 00411b47
        mov    ecx, dword ptr ds:[0x0063bb10]
        test   ecx, ecx
        mov    dword ptr ds:[0x0063bb1c], eax
        jz     L_mode
        mov    ecx, dword ptr [ecx + 0x174]
        cmp    eax, ecx
        jbe    L_mode
        mov    dword ptr ds:[0x0063bb1c], ecx        // clamp
    L_mode:                                          // 00411b66
        call   dword ptr [g_pbt_42f6a0]
        sub    eax, 2
        jnz    L_ret                                 // mode != 2 -> done
        mov    esi, dword ptr [esp + 0x60]           // 00411b74 UNINITIALISED, verbatim
        lea    edx, [esp + 0x14]
        mov    dword ptr [esp + 8], edx
        mov    edx, dword ptr [esp + 0x20]           // UNINITIALISED, verbatim
        mov    eax, 0x20003
        or     esi, eax
        or     edx, eax
        mov    eax, dword ptr ds:[0x0063bb0c]        // ghost ptr
        test   eax, eax                              // 00411b92 flags consumed at 00411c7d
        lea    ecx, [esp + 0x54]
        mov    dword ptr [esp + 4], ecx
        mov    dword ptr [esp + 0xc], 1
        mov    dword ptr [esp + 0x10], 0
        mov    dword ptr [esp + 0x7c], 0x3f800000
        mov    dword ptr [esp + 0x68], 0x3f800000
        mov    dword ptr [esp + 0x54], 0x3f800000
        mov    dword ptr [esp + 0x64], 0
        mov    dword ptr [esp + 0x5c], 0
        mov    dword ptr [esp + 0x58], 0
        mov    dword ptr [esp + 0x78], 0
        mov    dword ptr [esp + 0x74], 0
        mov    dword ptr [esp + 0x6c], 0
        mov    dword ptr [esp + 0x8c], 0
        mov    dword ptr [esp + 0x88], 0
        mov    dword ptr [esp + 0x84], 0
        mov    dword ptr [esp + 0x60], esi
        mov    dword ptr [esp + 0x3c], 0x3f800000
        mov    dword ptr [esp + 0x28], 0x3f800000
        mov    dword ptr [esp + 0x14], 0x3f800000
        mov    dword ptr [esp + 0x24], 0
        mov    dword ptr [esp + 0x1c], 0
        mov    dword ptr [esp + 0x18], 0
        mov    dword ptr [esp + 0x38], 0
        mov    dword ptr [esp + 0x34], 0
        mov    dword ptr [esp + 0x2c], 0
        mov    dword ptr [esp + 0x4c], 0
        mov    dword ptr [esp + 0x48], 0
        mov    dword ptr [esp + 0x44], 0
        mov    dword ptr [esp + 0x20], edx
        jz     L_bestlap                             // 00411c7d (ghost ptr == 0)
        mov    ecx, dword ptr ds:[0x0063bb1c]
        push   ecx
        lea    edx, [esp + 0x18]
        push   edx
        push   eax
        call   dword ptr [g_pbt_482c10]
        add    esp, 0xc
    L_bestlap:                                       // 00411c94
        mov    eax, dword ptr ds:[0x0063bb10]
        test   eax, eax
        jz     L_apply
        mov    ecx, dword ptr ds:[0x0063bb1c]
        push   ecx
        lea    edx, [esp + 0x58]
        push   edx
        push   eax
        call   dword ptr [g_pbt_482c10]
        add    esp, 0xc
    L_apply:                                         // 00411cb2
        lea    eax, [esp + 0xc]
        push   eax
        call   dword ptr [g_pbt_41a9b0]
        lea    ecx, [esp + 8]
        push   ecx
        call   dword ptr [g_pbt_41ad00]
        add    esp, 8
    L_ret:                                           // 00411cc9
        pop    esi
        add    esp, 0x90
        ret
    }
}

// Original trampoline. The 5-byte RH_ScopedInstall JMP overwrites exactly one instruction here -
// MOV EAX,[0x0063bb24] is `a1 24 bb 63 00`, 5 bytes - so re-exec that single stolen instruction
// and jump to the clean boundary 0x00411ae5 (SUB ESP,0x90). No partial instruction is split.
__declspec(naked) void OrigPlaybackTick(int /*p1*/, unsigned /*p2*/) {
    __asm {
        mov    eax, dword ptr ds:[0x0063bb24]
        jmp    dword ptr [g_orig_411ae5]
    }
}

inline int PbtSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_VEHICLE_PBT_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void PbtSelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ghost_playback_tick_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

long g_pbt_calls = 0, g_pbt_mismatch = 0;
// Coverage counters, armed BEFORE the first run rather than after a green
// ([[arm-coverage-counters-before-first-run]]). moved = calls where the cursor actually changed
// value; p2Fold = XOR of the param_2 inputs, so a constant input folds toward 0 and cannot be
// mistaken for an exercised one; overrideTaken / clampTaken / modeBody / ghostBranch / bestBranch
// say which arms were reached at all.
long g_pbt_moved = 0, g_pbt_overrideTaken = 0, g_pbt_clampTaken = 0;
long g_pbt_modeBody = 0, g_pbt_ghostBranch = 0, g_pbt_bestBranch = 0;
std::uint32_t g_pbt_p2Fold = 0;
const long kPbtMaxCompare = 40000;

// A/B dispatch. Modded pass first with all three side-effecting callees suppressed, then restore
// the cursor and run the ORIGINAL (which performs the single real set of side effects), then
// compare the post-call cursor bit-exactly. Returns after the original pass, so behaviour with the
// env var unset - or set - is the stock behaviour.
void PlaybackTickDispatch(int p1, unsigned p2) {
    if (PbtSelfTestEnabled() && g_pbt_calls < kPbtMaxCompare) {
        const std::uint32_t snap = *reinterpret_cast<volatile std::uint32_t*>(0x0063bb1c);
        const std::uint32_t ovr  = *reinterpret_cast<volatile std::uint32_t*>(0x0063bb24);
        const std::uint32_t best = *reinterpret_cast<volatile std::uint32_t*>(0x0063bb10);
        const std::uint32_t ghost= *reinterpret_cast<volatile std::uint32_t*>(0x0063bb0c);

        s_readFrameCalls = 0; s_readFrameTime = 0; s_applyGhost = 0; s_applyBest = 0;
        g_pbt_482c10 = reinterpret_cast<void*>(&CaptureReadFrame);
        g_pbt_41a9b0 = reinterpret_cast<void*>(&CaptureApplyGhost);
        g_pbt_41ad00 = reinterpret_cast<void*>(&CaptureApplyBest);
        PlaybackTick_Reimpl(p1, p2);
        const std::uint32_t modCursor = *reinterpret_cast<volatile std::uint32_t*>(0x0063bb1c);
        const long modApply = s_applyGhost + s_applyBest;
        const long modRead  = s_readFrameCalls;
        g_pbt_482c10 = reinterpret_cast<void*>(0x00482c10);
        g_pbt_41a9b0 = reinterpret_cast<void*>(0x0041a9b0);
        g_pbt_41ad00 = reinterpret_cast<void*>(0x0041ad00);

        *reinterpret_cast<volatile std::uint32_t*>(0x0063bb1c) = snap;
        OrigPlaybackTick(p1, p2);
        const std::uint32_t origCursor = *reinterpret_cast<volatile std::uint32_t*>(0x0063bb1c);

        ++g_pbt_calls;
        if (modCursor != snap) ++g_pbt_moved;
        if (ovr != 0) ++g_pbt_overrideTaken;
        if (best != 0 && modCursor != p2) ++g_pbt_clampTaken;
        if (modApply) { ++g_pbt_modeBody; }
        if (ghost != 0 && modRead) ++g_pbt_ghostBranch;
        if (best  != 0 && modRead) ++g_pbt_bestBranch;
        g_pbt_p2Fold ^= p2;

        if (modCursor != origCursor) {
            ++g_pbt_mismatch;
            char line[160];
            wsprintfA(line, "[%ld] MISMATCH cursor m=%08X o=%08X | p1=%08X p2=%08X\r\n",
                      g_pbt_calls, modCursor, origCursor, (unsigned)p1, p2);
            PbtSelfTestLog(line);
        }
        if ((g_pbt_calls & 0x7f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld moved=%ld p2Fold=%08X ovr=%ld clamp=%ld "
                            "modeBody=%ld ghost=%ld best=%ld %s\r\n",
                      g_pbt_calls, g_pbt_calls, g_pbt_mismatch, g_pbt_moved, g_pbt_p2Fold,
                      g_pbt_overrideTaken, g_pbt_clampTaken, g_pbt_modeBody,
                      g_pbt_ghostBranch, g_pbt_bestBranch,
                      g_pbt_mismatch ? "" : "ALL-GREEN");
            PbtSelfTestLog(line);
        }
        return;   // the original already applied the real cursor write and side effects
    }
    OrigPlaybackTick(p1, p2);
}

// Naked entry installed at 0x00411ae0 - __cdecl void(int p1, uint p2). Pushes p2 then p1 so the
// dispatch sees them in the original order; the second `[esp+8]` reads p1 because the first push
// has already moved ESP down by 4.
__declspec(naked) void GhostPlaybackTick_Entry() {
    __asm {
        mov    eax, dword ptr [esp + 8]     // p2
        push   eax
        mov    eax, dword ptr [esp + 8]     // p1 (esp shifted by the push above)
        push   eax
        call   PlaybackTickDispatch
        add    esp, 8
        ret
    }
}

}  // namespace

RH_ScopedInstall(GhostPlaybackTick_Entry, 0x00411ae0);
