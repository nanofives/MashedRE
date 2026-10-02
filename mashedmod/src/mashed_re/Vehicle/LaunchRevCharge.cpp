// Mashed RE — the start-line LAUNCH REV-CHARGE pair.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Two functions, one mechanic:
//   0x0046d7f0  BoundedTableSignClamp46d7f0   per-state-tick CHARGE on veh+0xbf4
//   0x0046d780  LaunchRevRelease46d780        the RELEASE: veh+0xbf8 = 1 or 2
//
// WHY THIS FILE EXISTS (U-9174, resolved 2026-10-01, D2 attempt 15).
// `0x0046d7a2` is the instruction that writes the vehicle record's `+0xbf8 = 2` at
// the green light, and it was unlocated for three attempts because it uses a FOLDED
// base: MSVC turns `&veh[i].+0xbf8` into `i*0xd04 + 0x00882198`, so the instruction
// encodes no `0xbf8` at all and every `+0xbf8` displacement scan missed it. The
// stride witness is `imul eax,eax,0xd04` at `0x0046d78e`; the array base `0x008815a0`
// is independently recorded in every .msd capture's provenance (`base_va`).
// Confirmed on the RUNNING original on two boots, entry hooks only:
// verify/d2_writer_20261001/RESULT_STEP1.md.
//
// THE MECHANIC. During the pre-race state the per-car integer charge at `+0xbf4`
// accumulates while the accel byte is held above 160, saturating at 3000. At the
// green light the release converts it to a state at `+0xbf8`: `2` when the charge
// exceeds 1000 (an OVER-REV BOG — A6a then zeroes the drive force for charge/200
// frames), `1` otherwise (a launch boost). A6a `FUN_00467650` consumes it at
// `0x00467d2e` / `0x00467def`.
//
// THE CALLERS, in the original:
//   0x00410441  FUN_0046d7f0(car, 50)   per active car, every pre-race state-tick
//   0x0041049b  FUN_0046d780(car)       per active car, when the 1.86 s timer elapses
//   both inside FUN_004103a0, which is NOT ported; the standalone's equivalent
//   wiring is TrackRenderer's countdown (see LaunchRev_PreRaceTick / _Release below).
// The delta 50 is a HARD CONSTANT in the image: `push 0x32` at `0x0042c980` and
// `0x00492d83`, with the caller looping `(frameMs-1)/50 + 1` times (the
// `mul 0x51eb851f` / `shr 4` divide-by-50 at `0x0042c973`) — one tick per frame at
// 60 fps. Measured live: `+50` per tick, 0 -> 3000 in 59 ticks.
//
// DUAL-TARGET BINDING. This TU is in BOTH exe_sources.rsp and asi_sources.rsp, so
// there is ONE body per RVA and the .asi's Frida evidence covers the exe's code.
// Under /DMASHED_STANDALONE (exe only) every MASHED absolute is resolved to a NAMED
// standalone symbol, the pattern Save\GameSaveBuffer.cpp established:
//   record array   0x008815a0  -> Vehicle::g_vehicleArrayBase (= g_records)
//   accel byte     0x007f103c  -> Vehicle::g_launchAccelByte[car], set by the caller
//   threshold      0x005cea3c  -> the literal 160.0f (bits 0x43200000)
//   score add      0x00422b50  -> ABSENT. The standalone has no 0x008995bc per-car
//                                 counter block; the write is declared absent, not
//                                 emulated. It feeds no physics: nothing reads that
//                                 counter in any path A6a touches.
// The .asi keeps every original absolute, so the diff-original reference is
// byte-identical there.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace mashed_re {
namespace Vehicle {

#ifdef MASHED_STANDALONE
extern int* g_vehicleArrayBase;              // DAT_008815a0 (16 x 0xd04 records)
// Standalone binding for the original's cooked accel byte at 0x007f103c + ctrl*0x4c.
// The caller (TrackRenderer's countdown) writes the car's current accel byte here
// immediately before the charge tick, which is what FUN_004103a0's tick reads from
// the control block in the original.
unsigned char g_launchAccelByte[16] = { 0 };
#endif

} // namespace Vehicle
} // namespace mashed_re

namespace {

// Record array base, per target. 0x008815a0 + 0xbf4 == 0x00882194 and
// 0x008815a0 + 0xbf8 == 0x00882198, so the two addressings agree in the .asi.
inline char* LrcVehBase() {
#ifdef MASHED_STANDALONE
    return reinterpret_cast<char*>(mashed_re::Vehicle::g_vehicleArrayBase);
#else
    return reinterpret_cast<char*>(0x008815a0u);
#endif
}

inline int* LrcCharge(unsigned idx) {     // veh[idx] + 0xbf4
    return reinterpret_cast<int*>(LrcVehBase() + idx * 0xd04u + 0xbf4u);
}
inline int* LrcState(unsigned idx) {      // veh[idx] + 0xbf8
    return reinterpret_cast<int*>(LrcVehBase() + idx * 0xd04u + 0xbf8u);
}

} // namespace

// ---------------------------------------------------------------------------
// BoundedTableSignClamp46d7f0  --  0x0046d7f0   (subsystem: ai)
//
// Original: FUN_0046d7f0 (143 bytes, 0x0046d7f0..0x0046d87f). PURE LEAF (no calls).
// Listing (original\MASHED.exe.unpatched, re-verified 2026-10-01):
//   0x0046d7f0  8b 4c 24 04              mov   ecx,[esp+4]            ; idx
//   0x0046d7f4  83 f9 10                 cmp   ecx,0x10
//   0x0046d7f7  7c 03                    jl    0x46d7fc               ; SIGNED bound
//   0x0046d7f9  33 c0                    xor   eax,eax
//   0x0046d7fb  c3                       ret                          ; -> 0
//   0x0046d7fe  c1 e0 04                 shl   eax,4                  ; idx*16
//   0x0046d801  8b 90 14 1a 7f 00        mov   edx,[eax+0x7f1a14]     ; ctrl index
//   0x0046d807  6b d2 4c                 imul  edx,edx,0x4c           ; ctrl block stride
//   0x0046d80a  0f b6 82 3c 10 7f 00     movzx eax,byte [edx+0x7f103c]; accel byte
//   0x0046d81a  db 44 24 08              fild  dword [esp+8]          ; (float)byte
//   0x0046d81e  d8 1d 3c ea 5c 00        fcomp dword [0x5cea3c]       ; 160.0
//   0x0046d826  f6 c4 41                 test  ah,0x41                ; C0|C3
//   0x0046d829  75 1d                    jne   0x46d848               ; skip when <= 160
//   0x0046d82d  69 c0 04 0d 00 00        imul  eax,eax,0xd04
//   0x0046d834  8b b8 94 21 88 00        mov   edi,[eax+0x882194]
//   0x0046d840  8d 34 12                 lea   esi,[edx+edx]          ; val*2
//   0x0046d843  03 fe                    add   edi,esi
//   0x0046d845  89 38                    mov   [eax],edi              ; charge += val*2
//   0x0046d848  69 c9 04 0d 00 00        imul  ecx,ecx,0xd04          ; shared tail
//   0x0046d84e  8b b1 94 21 88 00        mov   esi,[ecx+0x882194]
//   0x0046d85a  2b f2                    sub   esi,edx                ; charge -= val
//   0x0046d85e  81 f9 b8 0b 00 00        cmp   ecx,0xbb8              ; 3000
//   0x0046d864  89 30                    mov   [eax],esi
//   0x0046d869  c7 00 b8 0b 00 00        mov   dword [eax],0xbb8      ; high clamp
//   0x0046d86f  83 38 00                 cmp   dword [eax],0
//   0x0046d874  c7 00 00 00 00 00        mov   dword [eax],0          ; low clamp
//   0x0046d87a  b8 01 00 00 00           mov   eax,1
//
// NET: charge += val when the accel byte exceeds 160, else charge -= val; the
// two-step form (`+= val*2` then the shared `-= val`) is kept because that is what
// the original executes. Integer output; only the sign is float-selected, so
// bit-identity is exact.
//
// MOVED here 2026-10-01 from Util\PromoLoop_sessionB.cpp (one RVA, one body): the
// standalone needs this function and that TU is asi-only. Semantics unchanged
// except for the two-step accumulate, which is net-identical.
// ---------------------------------------------------------------------------

// 0x0046d7f0
extern "C" __declspec(dllexport) int __cdecl BoundedTableSignClamp46d7f0(unsigned idx, int val)
{
    // signed bound (jl vs 0x10) cited at 0x0046d7f4.
    if (idx >= 0x10u) return 0;

#ifdef MASHED_STANDALONE
    const unsigned char b = mashed_re::Vehicle::g_launchAccelByte[idx];
    const float         C = 160.0f;                       // _DAT_005cea3c = 0x43200000
#else
    const int           t1 = *reinterpret_cast<const int*>(0x007f1a14u + idx * 16u);
    const unsigned char b  = *reinterpret_cast<const unsigned char*>(
        0x007f103cu + static_cast<unsigned>(t1) * 0x4cu);
    const float         C  = *reinterpret_cast<const float*>(0x005cea3cu);
#endif

    int* const slot = LrcCharge(idx);
    if (static_cast<float>(b) > C) *slot = *slot + val * 2;   // 0x0046d845
    *slot = *slot - val;                                      // 0x0046d864
    if (*slot > 0xbb8) *slot = 0xbb8;                         // 0x0046d869
    if (*slot < 0)     *slot = 0;                             // 0x0046d874
    return 1;
}

RH_ScopedInstall(BoundedTableSignClamp46d7f0, 0x0046d7f0);

// ---------------------------------------------------------------------------
// LaunchRevRelease46d780  --  0x0046d780   (subsystem: ai)
//
// Original: FUN_0046d780 (109 bytes, 0x0046d780..0x0046d7ec).
// Listing (original\MASHED.exe.unpatched, verified 2026-10-01):
//   0x0046d780  8b 54 24 04              mov   edx,[esp+4]            ; car index
//   0x0046d784  83 fa 10                 cmp   edx,0x10
//   0x0046d787  7c 03                    jl    0x46d78c               ; SIGNED bound
//   0x0046d789  33 c0                    xor   eax,eax
//   0x0046d78b  c3                       ret                          ; -> 0
//   0x0046d78e  69 c0 04 0d 00 00        imul  eax,eax,0xd04
//   0x0046d794  8b 88 94 21 88 00        mov   ecx,[eax+0x882194]     ; charge
//   0x0046d79a  81 f9 e8 03 00 00        cmp   ecx,0x3e8              ; 1000
//   0x0046d7a0  7e 21                    jle   0x46d7c3
//   0x0046d7a2  c7 80 98 21 88 00 02 ..  mov   dword [eax+0x882198],2 ; <== THE WRITER
//   0x0046d7ac  b8 e8 03 00 00           mov   eax,0x3e8
//   0x0046d7b1  2b c1                    sub   eax,ecx                ; 1000 - charge (<0)
//   0x0046d7b5  e8 96 53 fb ff           call  0x422b50
//   0x0046d7bd  b8 01 00 00 00           mov   eax,1
//   0x0046d7c2  c3                       ret
//   0x0046d7c3  85 c9                    test  ecx,ecx
//   0x0046d7c5  7e 20                    jle   0x46d7e7               ; charge <= 0: nothing
//   0x0046d7c7  81 c1 e8 03 00 00        add   ecx,0x3e8              ; charge + 1000
//   0x0046d7cf  c7 80 98 21 88 00 01 ..  mov   dword [eax+0x882198],1
//   0x0046d7d9  89 88 94 21 88 00        mov   [eax+0x882194],ecx     ; charge += 1000
//   0x0046d7df  e8 6c 53 fb ff           call  0x422b50
//   0x0046d7e7  b8 01 00 00 00           mov   eax,1
//   0x0046d7ec  c3                       ret
//
// NOTE the asymmetry, which is the whole mechanic: on the OVER-REV arm the charge
// is NOT modified, so A6a's `== 2` arm counts the full saturated 3000 down at
// 200/frame = 15 frames of zero drive force. On the boost arm the charge is raised
// by 1000 first.
//
// Caller: FUN_004103a0 at 0x0041049b (per active car, once per race).
// Callee: FUN_00422b50 (C3, HUD\ScenarioWriters_sa2s2.cpp) adds the signed amount to
//   the per-car counter at 0x008995bc + car*0x138. ABSENT in the standalone — see
//   the DUAL-TARGET BINDING note at the top of this file.
// ---------------------------------------------------------------------------

#ifndef MASHED_STANDALONE
typedef void(__cdecl* LrcScoreAddFn)(int, int);
#endif

// 0x0046d780
extern "C" __declspec(dllexport) int __cdecl LaunchRevRelease46d780(int car)
{
    // signed bound (jl vs 0x10) cited at 0x0046d784.
    if (car >= 0x10) return 0;

    const unsigned idx    = static_cast<unsigned>(car);
    int* const     charge = LrcCharge(idx);
    const int      c      = *charge;                 // 0x0046d794

    if (c > 1000) {                                  // 0x0046d79a
        *LrcState(idx) = 2;                          // 0x0046d7a2  <== THE WRITER
#ifndef MASHED_STANDALONE
        reinterpret_cast<LrcScoreAddFn>(0x00422b50u)(car, 1000 - c);   // 0x0046d7b5
#endif
        return 1;
    }
    if (c > 0) {                                     // 0x0046d7c3
        *LrcState(idx) = 1;                          // 0x0046d7cf
        *charge        = c + 1000;                   // 0x0046d7d9
#ifndef MASHED_STANDALONE
        reinterpret_cast<LrcScoreAddFn>(0x00422b50u)(car, c + 1000);   // 0x0046d7df
#endif
    }
    return 1;
}

RH_ScopedInstall(LaunchRevRelease46d780, 0x0046d780);

// ===========================================================================
// STANDALONE WIRING — the equivalent of FUN_004103a0's two loops.
//
// NOT an RVA port: FUN_004103a0 is a 360-byte pre-race tick that also drives lap
// bookkeeping, the event queue and the DAT_0063ba8c transition, none of which the
// standalone has (TrackRenderer.cpp:3919 records that absence). These two helpers
// reproduce ONLY its two launch loops, at the two original call sites' arity and
// constants, from the standalone's own countdown.
//
// SCOPE, stated rather than assumed: slot 0 (the player) only. AI slots keep the
// separate U-D3-DRIVE-FORCE seed in VehiclePhysicsRun.cpp (slot != 0), so the two
// never write the same record and no AI behaviour moves in this change. Putting the
// AI slots on the real law needs their pre-race accel byte, which the standalone
// does not produce during the countdown — that is D3 work, recorded as such.
// ===========================================================================

namespace mashed_re {
namespace Vehicle {

#ifdef MASHED_STANDALONE

// Default ON; MASHED_NO_LAUNCH_REV=1 is the A/B control arm (the shape
// MASHED_NO_START_BOOST already uses in VehiclePhysicsRun.cpp).
static bool LaunchRevEnabled() {
    static const bool s_off = (std::getenv("MASHED_NO_LAUNCH_REV") != nullptr);
    return !s_off;
}

// One pre-race frame. `dt` is the frame time in seconds; `accel01` is the car's
// accelerator in [0,1] as TrackRenderer already cooks it for io.input[4].
// Tick count and delta are the original's: `push 0x32` (50) per tick,
// `(frameMs-1)/50 + 1` ticks per frame (0x0042c970..0x0042c98b).
void LaunchRev_PreRaceTick(float dt, float accel01) {
    if (!LaunchRevEnabled()) return;
    const int frameMs = static_cast<int>(dt * 1000.0f);
    if (frameMs <= 0) return;
    int ticks = (frameMs - 1) / 50 + 1;
    if (ticks > 16) ticks = 16;            // guard a stalled frame; 60 fps gives 1
    const float a = accel01 > 0.f ? accel01 : 0.f;
    g_launchAccelByte[0] = static_cast<unsigned char>(a * 255.f);
    for (int t = 0; t < ticks; ++t) BoundedTableSignClamp46d7f0(0u, 50);
}

// The green light. FUN_004103a0 calls the release once per active car at
// 0x0041049b, immediately before DAT_0063ba8c = 6.
void LaunchRev_Release() {
    if (!LaunchRevEnabled()) return;
    const int c0 = *LrcCharge(0), s0 = *LrcState(0);
    LaunchRevRelease46d780(0);
    // DIRECT in-capture witness of the event itself, not a proxy for it
    // (memory check-the-capture-carries-the-event-itself). Goes into the SAME
    // motion_diag.log the D2 reducers read; they all require `ftot=[` and `velH=`
    // on a line, so this one is skipped by every parser.
    if (std::getenv("MASHED_MOTION_DIAG")) {
        if (std::FILE* lf = std::fopen("motion_diag.log", "a")) {
            std::fprintf(lf, "launchrev release car=0 bf4 %d -> %d  bf8 %d -> %d\n",
                         c0, *LrcCharge(0), s0, *LrcState(0));
            std::fclose(lf);
        }
    }
}

#endif // MASHED_STANDALONE

} // namespace Vehicle
} // namespace mashed_re
