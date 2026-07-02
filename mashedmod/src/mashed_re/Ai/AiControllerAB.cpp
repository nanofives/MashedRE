// Mashed RE — WS-R6: snapshot/restore A/B self-test drivers for the three
// ported-but-C2 AiController.cpp functions (LeaderAB pattern, AiLeaderTimer.cpp):
//
//   0x00417640  AiPostStepPowerupBrake  -> AbAiPost
//   0x00418560  AiVehicleStep           -> AbAiVehStep
//   0x00418860  AiTickLoop              -> AbAiTick
//
// Each driver is installed AT the target RVA (replacing the original), runs the
// PORT on the live pre-call state, captures the write surface, rolls the state
// back, runs the ORIGINAL (via prologue-re-exec trampoline), captures again,
// compares byte-for-byte, and leaves the ORIGINAL's writes committed so game
// behavior stays original. MINE and ORIG both call the ORIGINAL callees by RVA
// (the ports' call-throughs), so given identical restored input state the two
// sides must produce bit-identical write surfaces.
//
// Registration is gated on MASHED_AI_AB=1 (else this file registers nothing),
// and exactly ONE driver is selected per run via MASHED_HOOK_ONLY=<name>.
// Run the three drivers in SEPARATE runs — never together (an installed
// AbAiVehStep would intercept the RVA calls made by AiTickLoop's A/B).
// NOTE: MASHED_HOOK_ONLY=0x00418560/0x00418860/0x00417640 (RVA form) would
// select BOTH the port hook in AiController.cpp and the driver here — always
// select by name for these three RVAs.
//
// Snapshot windows (write surface of the FUN_00418860 call tree; every address
// cited to the plate/decomp that shows the write):
//   W1 0x0089a360..0x0089a8a0  per-vehicle AI records (base 0x0089a4cc stride
//        0x74, 4 vehicles -> ..0x0089a698: line type/spline idx/timers/replay/
//        mode 0x0089a52c; leader timer+rank 0x0089a4c8/0x0089a4c4; powerup flag
//        bank 0x0089a51c/520/524/528; ram latches 0x0089a4dc/a4e0), debug flag
//        0x0089a368 + toggle timer 0x0089a36c (FUN_004177b0), candidate slots
//        0x0089a870..a87c, race-angle array 0x0089a880[4] (FUN_004177b0).
//   W2 0x007f1038..0x007f1298  control-output blocks, 8 slots x 0x4c
//        (FUN_00418560 zero+replay writes; control steps write [0/1/4/5];
//        FUN_00415220 writes [3]; FUN_00417640 writes [4]/[5]).
//   W3 0x008032d4..0x00803324  FUN_00413fe0 sets 0x8032d4+iter*0x14 = 1000.
//   W4 0x007f0ff8              FUN_00418560 mode-5 zeroes DAT_007f0ff8.
//   W5 0x008816f4+v*0xd04 (4 dwords)  FUN_0046dd90 speed field writes
//        (FUN_004177b0 rubber-band; hooks.csv VehField8816f4Set).
//   W6 RNG (FUN_00534870 disasm, this session): ctx = *(u32*)0x007dc578 +
//        *(u32*)0x007d3ff8; writes: ring cell at *(ctx+4), cur_ptr ctx+4,
//        wrap: cur_ptr := *(ctx+0), step_ptr ctx+8 += 4. Snapshot the 4 ctx
//        dwords + the whole ring [*(ctx+0), *(ctx+0xc)) (cap 64 KiB).
//   W7 (AbAiTick only) *(u32*)0x005f2770 + 0x34 + v*4, v=0..3
//        (FUN_0040e480 CarSlotStateSet disasm: mov [edx+ecx*4+0x34], eax).
//
// A GREEN here is synthetic-scenario A/B with the hook installed at the target
// RVA driving natural call sites/arguments in a live race => C3 evidence
// (NOT C4: the committed result is the original's, so the port never steers
// the canonical scenario).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (prologue bytes verified against original\MASHED.exe.unpatched, this session):
//   0x00417640: 83 ec 18 e8 b8 f5 00 00   sub esp,0x18 / call 0x426c00
//   0x00418560: 53 55 8b 6c 24 0c         push ebx / push ebp / mov ebp,[esp+0xc]
//   0x00418860: 83 3d a0 1c 80 00 04      cmp dword [0x801ca0],4

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

// Ported functions under test (AiController.cpp).
extern "C" {
void __cdecl AiPostStepPowerupBrake(int param_1, std::uint8_t* param_2);
void __cdecl AiVehicleStep(int param_1);
void __cdecl AiTickLoop(void);
}

namespace {

// ── original-side trampolines (re-exec clobbered prologue, resume past it) ──
void* g_orig_417648 = reinterpret_cast<void*>(0x00417648);
__declspec(naked) void __cdecl OrigPostStep(int, std::uint8_t*) {
    __asm {
        sub  esp, 0x18                  // 0x00417640
        mov  eax, 0x00426c00            // 0x00417643 call FUN_00426c00 (E8 rel32)
        call eax
        jmp  dword ptr [g_orig_417648]  // resume at cmp eax,0x21
    }
}

void* g_orig_418566 = reinterpret_cast<void*>(0x00418566);
__declspec(naked) void __cdecl OrigVehStep(int) {
    __asm {
        push ebx                        // 0x00418560
        push ebp                        // 0x00418561
        mov  ebp, dword ptr [esp + 0xc] // 0x00418562 (param_1)
        jmp  dword ptr [g_orig_418566]  // resume at push edi
    }
}

void* g_orig_418867 = reinterpret_cast<void*>(0x00418867);
__declspec(naked) void __cdecl OrigTickLoop() {
    __asm {
        cmp  dword ptr ds:[0x00801ca0], 4  // 0x00418860 (flags consumed by jl @0x418867)
        jmp  dword ptr [g_orig_418867]
    }
}

// ── logging (raw WriteFile append; LeaderTimer SelfTestLog pattern) ─────────
void AbLog(const char* path, const char* s) {
    HANDLE h = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote;
    WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr);
    CloseHandle(h);
}

// ── snapshot segments ───────────────────────────────────────────────────────
struct Seg { std::uint8_t* p; std::uint32_t n; };
const int kMaxSegs = 16;
Seg  g_segs[kMaxSegs];
int  g_nsegs = 0;
bool g_segOverflow = false;

const std::uint32_t kBufCap = 0x11000;   // fixed 0x7f4 + dyn 0x40 + ring cap 0x10000
std::uint8_t g_save[kBufCap], g_mine[kBufCap], g_orig[kBufCap];

void SegAdd(std::uintptr_t base, std::uint32_t n) {
    if (g_nsegs >= kMaxSegs) { g_segOverflow = true; return; }
    g_segs[g_nsegs].p = reinterpret_cast<std::uint8_t*>(base);
    g_segs[g_nsegs].n = n;
    ++g_nsegs;
}

std::uint32_t g_ringSize = 0;   // for the layout log line

void BuildSegs(bool withCarSlots) {
    g_nsegs = 0;
    SegAdd(0x0089a360u, 0x540);                       // W1 AI records + pre-tick globals
    SegAdd(0x007f1038u, 0x260);                       // W2 ctrl blocks (8 x 0x4c)
    SegAdd(0x008032d4u, 0x50);                        // W3 FUN_00413fe0 table
    SegAdd(0x007f0ff8u, 4);                           // W4 mode-5 zero target
    for (int v = 0; v < 4; ++v)                       // W5 speed fields
        SegAdd(0x008816f4u + static_cast<std::uintptr_t>(v) * 0xd04u, 4);
    // W6 RNG ctx + ring (FUN_00534870)
    std::uint32_t b1 = *reinterpret_cast<std::uint32_t*>(0x007dc578u);
    std::uint32_t b2 = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    g_ringSize = 0;
    if (b1 && b2) {
        std::uintptr_t ctx = static_cast<std::uintptr_t>(b1) + b2;
        SegAdd(ctx, 0x10);                            // start/cur/step/end ptrs
        std::uint32_t start = *reinterpret_cast<std::uint32_t*>(ctx);
        std::uint32_t end   = *reinterpret_cast<std::uint32_t*>(ctx + 0xc);
        if (start && end > start && (end - start) <= 0x10000) {
            g_ringSize = end - start;
            SegAdd(start, g_ringSize);
        }
    }
    if (withCarSlots) {                               // W7 CarSlotStateSet targets
        std::uint32_t t = *reinterpret_cast<std::uint32_t*>(0x005f2770u);
        if (t) SegAdd(static_cast<std::uintptr_t>(t) + 0x34u, 0x10);
    }
}

std::uint32_t Capture(std::uint8_t* buf) {
    std::uint32_t off = 0;
    for (int i = 0; i < g_nsegs; ++i) {
        if (off + g_segs[i].n > kBufCap) { g_segOverflow = true; break; }
        std::memcpy(buf + off, g_segs[i].p, g_segs[i].n);
        off += g_segs[i].n;
    }
    return off;
}

void Restore(const std::uint8_t* buf) {
    std::uint32_t off = 0;
    for (int i = 0; i < g_nsegs; ++i) {
        if (off + g_segs[i].n > kBufCap) break;
        std::memcpy(g_segs[i].p, buf + off, g_segs[i].n);
        off += g_segs[i].n;
    }
}

// First differing byte between mine/orig captures; fills seg addr + values.
// Returns -1 on full match.
long FirstDiff(std::uint32_t len, std::uintptr_t* addr, int* segIdx,
               std::uint8_t* vm, std::uint8_t* vo) {
    for (std::uint32_t i = 0; i < len; ++i) {
        if (g_mine[i] != g_orig[i]) {
            std::uint32_t off = i;
            for (int s = 0; s < g_nsegs; ++s) {
                if (off < g_segs[s].n) {
                    *addr = reinterpret_cast<std::uintptr_t>(g_segs[s].p) + off;
                    *segIdx = s;
                    break;
                }
                off -= g_segs[s].n;
            }
            *vm = g_mine[i]; *vo = g_orig[i];
            return static_cast<long>(i);
        }
    }
    return -1;
}

// Bitmask of segments containing >=1 differing byte (mine vs orig captures).
// Discriminates RNG-window contamination (the ctx/ring segments differ too)
// from a genuine port divergence (they don't): FUN_00534870's ring is shared
// with the audio thread (rand-pitch SFX, e.g. FUN_00464670), so a foreign
// draw landing inside the A/B window skews one side's rolls.
std::uint32_t SegDiffMask(std::uint32_t len) {
    std::uint32_t mask = 0, off = 0;
    for (int s = 0; s < g_nsegs && off < len; ++s) {
        std::uint32_t n = g_segs[s].n;
        if (off + n > len) n = len - off;
        if (std::memcmp(g_mine + off, g_orig + off, n) != 0) mask |= (1u << s);
        off += g_segs[s].n;
    }
    return mask;
}

const long kMaxMismLog = 400;

void LayoutLogOnce(const char* path, long* onceFlag) {
    if (*onceFlag) return;
    *onceFlag = 1;
    char line[512];
    wsprintfA(line, "layout: nsegs=%d ring=%u overflow=%d segs=", g_nsegs,
              g_ringSize, g_segOverflow ? 1 : 0);
    AbLog(path, line);
    for (int i = 0; i < g_nsegs; ++i) {
        wsprintfA(line, "[%d]0x%08x+0x%x ", i,
                  (unsigned)reinterpret_cast<std::uintptr_t>(g_segs[i].p), g_segs[i].n);
        AbLog(path, line);
    }
    AbLog(path, "\r\n");
}

// ── 0x00417640 AbAiPost ─────────────────────────────────────────────────────
// Write surface of FUN_00417640 = param_2[4], param_2[5] ONLY (plate
// re/analysis/ai_update/0x00417640.md; port AiController.cpp). Gate coverage
// counted via the same pure getter the function calls (FUN_00426c00==0x21).
//
// Forced pass: gate 1 (DAT_00644158 == 0x21) never holds in a solo scenario
// race (U-0421; one note reads the global as game mode, 0x21 = network), so
// after the natural A/B each call runs a second paired A/B with DAT_00644158
// temporarily seeded to 0x21 and a SCRATCH ctrl buffer. The callee tree past
// gate 1 is getter-only (0x00452eb0/0x0089a52c/0x00452ea0/0x00452160/
// 0x0046d6d0/0x0046d570/0x0046d4a0/0x004c3ac0), so with writes redirected to
// scratch the forced pass perturbs nothing; the global is restored before
// returning. AV guard: the deep branch derefs *(u32*)0x00684dac + 0x30
// (FUN_00452160), so the forced pass is skipped while gates 2-4 would pass
// with a NULL 0x00684dac seed.
const char* kPostLog = "ai_ab_poststep.log";
long g_postCalls = 0, g_postMism = 0, g_postGate1 = 0, g_postWrote = 0;
long g_postForced = 0, g_postForcedMism = 0, g_postForcedWrote = 0, g_postForcedSkip = 0;
long g_postF2 = 0, g_postF2Mism = 0, g_postF2Brake = 0, g_postF2Coast = 0, g_postF2Skip = 0;

void __cdecl AbAiPost(int v, std::uint8_t* ctrl) {
    std::uint8_t s4 = ctrl[4], s5 = ctrl[5];

    AiPostStepPowerupBrake(v, ctrl);                    // MINE
    std::uint8_t m4 = ctrl[4], m5 = ctrl[5];

    ctrl[4] = s4; ctrl[5] = s5;                         // roll back
    OrigPostStep(v, ctrl);                              // ORIG (commits)
    std::uint8_t o4 = ctrl[4], o5 = ctrl[5];

    ++g_postCalls;
    if (reinterpret_cast<int (__cdecl*)(void)>(0x00426c00)() == 0x21) ++g_postGate1;
    if (o4 != s4 || o5 != s5) ++g_postWrote;
    if (m4 != o4 || m5 != o5) {
        if (++g_postMism <= kMaxMismLog) {
            char line[192];
            wsprintfA(line, "[%ld] MISMATCH v=%d pre=(%02x,%02x) mine=(%02x,%02x) orig=(%02x,%02x)\r\n",
                      g_postCalls, v, s4, s5, m4, m5, o4, o5);
            AbLog(kPostLog, line);
        }
    }

    // ── forced-gate1 pass (scratch ctrl; global seeded + restored) ──
    {
        bool deepReachable =
            !(reinterpret_cast<float (__cdecl*)(void)>(0x00452eb0)() >
              *reinterpret_cast<float*>(0x005cc72cu)) &&                      // gate 2
            *reinterpret_cast<int*>(0x0089a52cu + static_cast<std::uintptr_t>(v) * 0x74u) != 10 && // gate 3
            reinterpret_cast<int (__cdecl*)(int)>(0x00452ea0)(v) != 0;        // gate 4
        bool avUnsafe = deepReachable && *reinterpret_cast<std::uint32_t*>(0x00684dacu) == 0;
        if (avUnsafe) {
            ++g_postForcedSkip;
        } else {
            std::uint32_t* gate = reinterpret_cast<std::uint32_t*>(0x00644158u);
            std::uint32_t saveGate = *gate;
            std::uint8_t fm[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
            std::uint8_t fo[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
            *gate = 0x21;
            AiPostStepPowerupBrake(v, fm);              // MINE  (scratch)
            OrigPostStep(v, fo);                        // ORIG  (scratch)
            *gate = saveGate;
            ++g_postForced;
            if (fo[4] != 0xAA || fo[5] != 0xAA) ++g_postForcedWrote;
            if (fm[4] != fo[4] || fm[5] != fo[5]) {
                if (++g_postForcedMism <= kMaxMismLog) {
                    char line[192];
                    wsprintfA(line, "[%ld] FORCED-MISMATCH v=%d mine=(%02x,%02x) orig=(%02x,%02x)\r\n",
                              g_postCalls, v, fm[4], fm[5], fo[4], fo[5]);
                    AbLog(kPostLog, line);
                }
            }
        }
    }

    // ── forced2: full-depth pass. Seeds every gate + the target pointer so
    // the write branch executes (disasm this session: FUN_00452ea0 =
    // *(u32*)(0x88ff50+v*4); FUN_00452eb0 = *(f32*)0x684de0; FUN_00452160 =
    // *(u32*)0x684dac + 0x30). Alternates the two branch thresholds
    // (_DAT_005cc568 / _DAT_005ccad0, u32 bit patterns +/-FLT_MAX) so both
    // the full-brake and the coast write paths run. All 6 seeded globals are
    // saved/restored around the pair; writes land in scratch ctrl buffers.
    // Skipped when the live mode (0x0089a52c+v*0x74) == 10 (gate 3 is left
    // unseeded; skip counter tracks it).
    {
        std::uint32_t* mode = reinterpret_cast<std::uint32_t*>(
            0x0089a52cu + static_cast<std::uintptr_t>(v) * 0x74u);
        if (*mode == 10) {
            ++g_postF2Skip;
        } else {
            std::uint32_t* gGate = reinterpret_cast<std::uint32_t*>(0x00644158u);
            std::uint32_t* gHeld = reinterpret_cast<std::uint32_t*>(0x00684de0u);
            std::uint32_t* gPred = reinterpret_cast<std::uint32_t*>(
                0x0088ff50u + static_cast<std::uintptr_t>(v) * 4u);
            std::uint32_t* gTgt  = reinterpret_cast<std::uint32_t*>(0x00684dacu);
            std::uint32_t* gTh1  = reinterpret_cast<std::uint32_t*>(0x005cc568u);
            std::uint32_t* gTh2  = reinterpret_cast<std::uint32_t*>(0x005ccad0u);
            std::uint32_t sGate = *gGate, sHeld = *gHeld, sPred = *gPred,
                          sTgt = *gTgt, sTh1 = *gTh1, sTh2 = *gTh2;
            static std::uint8_t tgtStruct[0x40];
            *reinterpret_cast<float*>(tgtStruct + 0x30) = 123.0f;
            *reinterpret_cast<float*>(tgtStruct + 0x34) = 45.0f;
            *reinterpret_cast<float*>(tgtStruct + 0x38) = -67.0f;
            bool brakeSide = (g_postF2 & 1) == 0;
            // _DAT_005cc568/_DAT_005ccad0 are .rdata (read-only page; the
            // unprotected write AV'd on the first attempt) — both live in the
            // same 4 KiB page; toggle it writable around the seeded pair.
            DWORD oldProt;
            if (!VirtualProtect(gTh1, 0x8, PAGE_READWRITE, &oldProt)) {
                ++g_postF2Skip;
            } else {
                *gGate = 0x21;
                *gHeld = *reinterpret_cast<std::uint32_t*>(0x005cc72cu);  // == thresh -> gate 2 passes
                *gPred = 1;
                *gTgt  = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(tgtStruct));
                *gTh1  = brakeSide ? 0xFF7FFFFFu : 0x7F7FFFFFu;           // -FLT_MAX / +FLT_MAX
                *gTh2  = 0x7F7FFFFFu;                                     // +FLT_MAX
                std::uint8_t fm[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
                std::uint8_t fo[8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
                AiPostStepPowerupBrake(v, fm);              // MINE  (scratch)
                OrigPostStep(v, fo);                        // ORIG  (scratch)
                *gGate = sGate; *gHeld = sHeld; *gPred = sPred;
                *gTgt = sTgt; *gTh1 = sTh1; *gTh2 = sTh2;
                DWORD tmpProt;
                VirtualProtect(gTh1, 0x8, oldProt, &tmpProt);
                ++g_postF2;
                if (fo[5] == 0xff) ++g_postF2Brake;
                if (fo[4] == 0 && fo[5] == 0) ++g_postF2Coast;
                if (fm[4] != fo[4] || fm[5] != fo[5]) {
                    if (++g_postF2Mism <= kMaxMismLog) {
                        char line[192];
                        wsprintfA(line, "[%ld] F2-MISMATCH v=%d side=%d mine=(%02x,%02x) orig=(%02x,%02x)\r\n",
                                  g_postCalls, v, brakeSide ? 1 : 0, fm[4], fm[5], fo[4], fo[5]);
                        AbLog(kPostLog, line);
                    }
                }
            }
        }
    }

    if ((g_postCalls & 0xff) == 1) {
        char line[288];
        wsprintfA(line, "[%ld] calls=%ld mism=%ld gate1pass=%ld wrote=%ld forced=%ld fmism=%ld fwrote=%ld fskip=%ld f2=%ld f2mism=%ld f2brake=%ld f2coast=%ld f2skip=%ld %s\r\n",
                  g_postCalls, g_postCalls, g_postMism, g_postGate1, g_postWrote,
                  g_postForced, g_postForcedMism, g_postForcedWrote, g_postForcedSkip,
                  g_postF2, g_postF2Mism, g_postF2Brake, g_postF2Coast, g_postF2Skip,
                  (g_postMism || g_postForcedMism || g_postF2Mism) ? "" : "ALL-GREEN");
        AbLog(kPostLog, line);
    }
}

// ── 0x00418560 AbAiVehStep ──────────────────────────────────────────────────
// On a first-attempt diff the pair is RETRIED once from the same restored
// pre-call state: a genuine transcription bug reproduces deterministically
// (identical inputs incl. restored RNG ring); a cross-thread RNG race inside
// the A/B window does not. rawmism counts first attempts, mism counts
// confirmed (post-retry) divergences — GREEN requires mism==0.
const char* kVehLog = "ai_ab_vehstep.log";
long g_vehCalls = 0, g_vehMism = 0, g_vehRawMism = 0, g_vehLayoutOnce = 0;
// Branch-coverage counters (selector peeks via the same pure getters/globals
// the function reads): sub-mode-5 countdown branch, dispatch selector
// DAT_007f0fd0 (4/9 -> FUN_00416a30, 8 -> FUN_00417da0, else FUN_00416250),
// input-override replay branch (0x0089a4fc+v*0x74 != 0), debug spline
// override (DAT_007f1a50 == 1).
long g_vehSub5 = 0, g_vehFd49 = 0, g_vehFd8 = 0, g_vehFdElse = 0,
     g_vehOvr = 0, g_vehDbg = 0;

void __cdecl AbAiVehStep(int v) {
    if (v < 0 || v > 3) { OrigVehStep(v); return; }     // out-of-model: original only
    BuildSegs(false);
    LayoutLogOnce(kVehLog, &g_vehLayoutOnce);
    std::uint32_t len = Capture(g_save);

    ++g_vehCalls;
    {
        int sub = reinterpret_cast<int (__cdecl*)(void)>(0x0040e350)();
        if (sub == 5) ++g_vehSub5;
        else {
            std::uint32_t fd0 = *reinterpret_cast<std::uint32_t*>(0x007f0fd0u);
            if (fd0 == 4 || fd0 == 9) ++g_vehFd49;
            else if (fd0 == 8) ++g_vehFd8;
            else ++g_vehFdElse;
            if (*reinterpret_cast<std::uint32_t*>(
                    0x0089a4fcu + static_cast<std::uintptr_t>(v) * 0x74u) != 0) ++g_vehOvr;
            if (*reinterpret_cast<std::uint32_t*>(0x007f1a50u) == 1) ++g_vehDbg;
        }
    }
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (attempt) Restore(g_save);                   // retry from same pre-state
        AiVehicleStep(v);                               // MINE
        Capture(g_mine);
        Restore(g_save);
        OrigVehStep(v);                                 // ORIG (commits)
        Capture(g_orig);

        std::uintptr_t addr = 0; int seg = -1; std::uint8_t vm = 0, vo = 0;
        long d = FirstDiff(len, &addr, &seg, &vm, &vo);
        if (d < 0) break;                               // match (attempt 0: clean; 1: transient)
        std::uint32_t mask = SegDiffMask(len);
        long raw = ++g_vehRawMism;
        long conf = attempt ? ++g_vehMism : g_vehMism;
        if (raw <= kMaxMismLog) {
            char line[256];
            wsprintfA(line, "[%ld] %s v=%d seg=%d addr=0x%08x mine=%02x orig=%02x bufoff=%ld segmask=0x%x\r\n",
                      g_vehCalls, attempt ? "MISMATCH-CONFIRMED" : "mism-raw(retrying)",
                      v, seg, (unsigned)addr, vm, vo, d, mask);
            AbLog(kVehLog, line);
        }
        (void)conf;
        if (attempt) break;
    }

    if ((g_vehCalls & 0xff) == 1) {
        char line[224];
        wsprintfA(line, "[%ld] calls=%ld mism=%ld rawmism=%ld cov[sub5=%ld fd49=%ld fd8=%ld fdelse=%ld ovr=%ld dbg=%ld] %s\r\n",
                  g_vehCalls, g_vehCalls, g_vehMism, g_vehRawMism,
                  g_vehSub5, g_vehFd49, g_vehFd8, g_vehFdElse, g_vehOvr, g_vehDbg,
                  g_vehMism ? "" : "ALL-GREEN");
        AbLog(kVehLog, line);
    }
}

// ── 0x00418860 AbAiTick ─────────────────────────────────────────────────────
// Same retry-once discipline as AbAiVehStep (see comment there).
const char* kTickLog = "ai_ab_tick.log";
long g_tickCalls = 0, g_tickMism = 0, g_tickRawMism = 0, g_tickLayoutOnce = 0;
// Coverage: guard pass (DAT_00801ca0 > 3), alive-marking block condition
// (round in {3,4,5,10} or fd0 in {4,8,9,10}), mode-7 extra-step condition
// (round in {3,4,5} and fd0 == 7).
long g_tickGuard = 0, g_tickMark = 0, g_tickExtra = 0;

void __cdecl AbAiTick(void) {
    BuildSegs(true);
    LayoutLogOnce(kTickLog, &g_tickLayoutOnce);
    std::uint32_t len = Capture(g_save);

    ++g_tickCalls;
    {
        if (*reinterpret_cast<std::int32_t*>(0x00801ca0u) > 3) {
            ++g_tickGuard;
            int r = reinterpret_cast<int (__cdecl*)(void)>(0x0042f6a0)();
            std::uint32_t fd0 = *reinterpret_cast<std::uint32_t*>(0x007f0fd0u);
            if (r == 3 || r == 4 || r == 5 || r == 10 ||
                fd0 == 4 || fd0 == 9 || fd0 == 8 || fd0 == 10) ++g_tickMark;
            if ((r == 3 || r == 4 || r == 5) && fd0 == 7) ++g_tickExtra;
        }
    }
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (attempt) Restore(g_save);                   // retry from same pre-state
        AiTickLoop();                                   // MINE
        Capture(g_mine);
        Restore(g_save);
        OrigTickLoop();                                 // ORIG (commits)
        Capture(g_orig);

        std::uintptr_t addr = 0; int seg = -1; std::uint8_t vm = 0, vo = 0;
        long d = FirstDiff(len, &addr, &seg, &vm, &vo);
        if (d < 0) break;
        std::uint32_t mask = SegDiffMask(len);
        long raw = ++g_tickRawMism;
        if (attempt) ++g_tickMism;
        if (raw <= kMaxMismLog) {
            char line[256];
            wsprintfA(line, "[%ld] %s seg=%d addr=0x%08x mine=%02x orig=%02x bufoff=%ld segmask=0x%x\r\n",
                      g_tickCalls, attempt ? "MISMATCH-CONFIRMED" : "mism-raw(retrying)",
                      seg, (unsigned)addr, vm, vo, d, mask);
            AbLog(kTickLog, line);
        }
        if (attempt) break;
    }

    if ((g_tickCalls & 0x3f) == 1) {
        char line[192];
        wsprintfA(line, "[%ld] calls=%ld mism=%ld rawmism=%ld cov[guard=%ld mark=%ld extra=%ld] %s\r\n",
                  g_tickCalls, g_tickCalls, g_tickMism, g_tickRawMism,
                  g_tickGuard, g_tickMark, g_tickExtra,
                  g_tickMism ? "" : "ALL-GREEN");
        AbLog(kTickLog, line);
    }
}

// ── env-gated registration (NOT RH_ScopedInstall: these must never register
//    in a normal full-install run — they collide with the port hooks at the
//    same RVAs). MASHED_AI_AB=1 registers all three; MASHED_HOOK_ONLY=<name>
//    then installs exactly one. ──
struct AbAiReg {
    AbAiReg() {
        const char* s = std::getenv("MASHED_AI_AB");
        if (!s || !s[0]) return;
        HookSystem::Register(0x00417640u, reinterpret_cast<void*>(&AbAiPost),    "AbAiPost");
        HookSystem::Register(0x00418560u, reinterpret_cast<void*>(&AbAiVehStep), "AbAiVehStep");
        HookSystem::Register(0x00418860u, reinterpret_cast<void*>(&AbAiTick),    "AbAiTick");
    }
};
AbAiReg s_abAiReg;

} // namespace
