// Mashed QoL .asi — player-facing runtime patches, env-gated per launch.
// Loaded by the dinput8 proxy (see dinput8_shim.cpp); does NOTHING unless a
// MASHED_QOL feature env var is set, so RE-session boots are unaffected.
//
// Features (each applied at DLL_PROCESS_ATTACH, before any game code runs):
//   MASHED_NO_SAVE=1  — SaveWrite 0x00404f50: skip the gamesave.bin disk write,
//                       keep SaveStatusClear(0) + return 0 (game thinks it saved).
//                       Evidence: mashedmod/src/mashed_re/Save/GameSave.cpp (C3),
//                       disasm 0x00404f50..0x00404f70.
//   MASHED_DECOUPLE=1 — framerate decoupling: retarget the CALL @0x00493480 so the
//                       frame-time source feeds MEASURED time (not a pinned 1-tick
//                       constant) into the game's own native tick quantizer. Game
//                       speed becomes correct at ANY framerate; physics stays on
//                       its native 60 Hz tick. Full evidence block at the hook.
//                       Pair with the d3d9 shim's MASHED_FPS_CAP_RACE for
//                       menu-60/race-fast caps (menus are per-frame-coupled).
//   MASHED_UNLOCK=1   — in-memory replica of scripts/patch_mashed_unlock_restore.py:
//                       redirect @ 0x00404eb4 in Save::DeserializeFromBuffer
//                       (0x00404e80) to a cave @ 0x005caf30 (.text zero padding)
//                       that fills track/cup table 0x007f0a40 (156 int32 = 2) and
//                       car flags 0x007f0e50 (156 bytes = 1) after every restore.
//                       Root cause narrative in that script's header (proven live
//                       2026-06-02). Bonus Features gate (0x430910) NOT covered.
//
// Diagnostics: OutputDebugStringA always; plus append to ..\log\qol_asi.txt
// (relative to MASHED.exe dir) when MASHED_QOL_LOG=1.
//
// Version anchor: patches verify expected pre-bytes before writing; on mismatch
// the feature is skipped and logged, never force-written.

#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

// ─── logging ─────────────────────────────────────────────────────────────────
bool g_fileLog = false;
wchar_t g_logPath[MAX_PATH] = {0};

void LogLine(const char* msg) {
    char buf[512];
    _snprintf_s(buf, _TRUNCATE, "[mashed_qol] %s\n", msg);
    OutputDebugStringA(buf);
    if (g_fileLog && g_logPath[0]) {
        FILE* f = nullptr;
        if (_wfopen_s(&f, g_logPath, L"ab") == 0 && f) {
            fputs(buf, f);
            fclose(f);
        }
    }
}

bool EnvSet(const char* name) {
    char buf[8] = {0};
    DWORD r = GetEnvironmentVariableA(name, buf, sizeof(buf));
    return r > 0 && buf[0] != '\0' && buf[0] != '0';
}

// ─── patch helpers ───────────────────────────────────────────────────────────
bool WriteMem(std::uintptr_t va, const void* src, size_t len) {
    void* p = reinterpret_cast<void*>(va);
    DWORD old = 0;
    if (!VirtualProtect(p, len, PAGE_EXECUTE_READWRITE, &old)) return false;
    std::memcpy(p, src, len);
    VirtualProtect(p, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), p, len);
    return true;
}

bool BytesAre(std::uintptr_t va, const void* expect, size_t len) {
    return std::memcmp(reinterpret_cast<const void*>(va), expect, len) == 0;
}

std::uint32_t Rel32(std::uintptr_t next_insn_va, std::uintptr_t target_va) {
    return static_cast<std::uint32_t>(target_va - next_insn_va);
}

// ─── MASHED_NO_SAVE — stub SaveWrite (0x00404f50) ────────────────────────────
// Original prologue (GameSave.cpp disasm, 0x00404f50..0x00404f5a):
//   68 A0 4F 02 00      PUSH 0x24fa0
//   68 58 33 80 00      PUSH 0x803358
// Replacement (12 bytes, function is 0x24 bytes so it fits):
//   6A 00               PUSH 0
//   E8 rel32            CALL 0x004099e0        ; SaveStatusClear(0)
//   83 C4 04            ADD ESP,4
//   33 C0               XOR EAX,EAX
//   C3                  RET
void ApplyNoSave() {
    constexpr std::uintptr_t kSaveWrite       = 0x00404f50;
    constexpr std::uintptr_t kSaveStatusClear = 0x004099e0;
    static const std::uint8_t pre[10] = {0x68, 0xA0, 0x4F, 0x02, 0x00,
                                         0x68, 0x58, 0x33, 0x80, 0x00};
    if (!BytesAre(kSaveWrite, pre, sizeof(pre))) {
        LogLine("NO_SAVE: pre-bytes at 0x404f50 unexpected — SKIPPED");
        return;
    }
    std::uint8_t stub[12];
    stub[0] = 0x6A; stub[1] = 0x00;                       // push 0
    stub[2] = 0xE8;                                       // call rel32
    const std::uint32_t rel = Rel32(kSaveWrite + 2 + 5, kSaveStatusClear);
    std::memcpy(&stub[3], &rel, 4);
    stub[7] = 0x83; stub[8] = 0xC4; stub[9] = 0x04;       // add esp,4
    stub[10] = 0x33; stub[11] = 0xC0;                     // xor eax,eax
    std::uint8_t full[13];
    std::memcpy(full, stub, 12);
    full[12] = 0xC3;                                      // ret
    if (WriteMem(kSaveWrite, full, sizeof(full)))
        LogLine("NO_SAVE: SaveWrite 0x404f50 stubbed (no gamesave.bin writes)");
    else
        LogLine("NO_SAVE: VirtualProtect failed — SKIPPED");
}

// ─── MASHED_UNLOCK — in-memory unlock_restore cave ───────────────────────────
// Byte-for-byte the same cave/redirect as scripts/patch_mashed_unlock_restore.py
// (see its header for the root-cause narrative + evidence citations).
void ApplyUnlock() {
    constexpr std::uintptr_t kCaveVa  = 0x005caf30;
    constexpr std::uintptr_t kRedirVa = 0x00404eb4;
    constexpr std::uintptr_t kRetVa   = 0x00404eba;

    // cave body, sans the trailing JMP rel32 (computed below)
    static const std::uint8_t caveBody[] = {
        0x50, 0x51, 0x52,                         // push eax/ecx/edx
        0xB8, 0x40, 0x0A, 0x7F, 0x00,             // mov eax,0x7f0a40
        0xB9, 0x9C, 0x00, 0x00, 0x00,             // mov ecx,156
        0xBA, 0x02, 0x00, 0x00, 0x00,             // mov edx,2
        0x89, 0x10,                               // L1: mov [eax],edx
        0x83, 0xC0, 0x04,                         //     add eax,4
        0x49,                                     //     dec ecx
        0x75, 0xF8,                               //     jnz L1
        0xB8, 0x50, 0x0E, 0x7F, 0x00,             // mov eax,0x7f0e50
        0xB9, 0x9C, 0x00, 0x00, 0x00,             // mov ecx,156
        0xC6, 0x00, 0x01,                         // L2: mov byte[eax],1
        0x40,                                     //     inc eax
        0x49,                                     //     dec ecx
        0x75, 0xF9,                               //     jnz L2
        0x5A, 0x59, 0x58,                         // pop edx/ecx/eax
        0x8B, 0x3D, 0xA8, 0x94, 0x8A, 0x00,       // mov edi,[0x8a94a8] (displaced)
    };
    std::uint8_t cave[sizeof(caveBody) + 5];
    std::memcpy(cave, caveBody, sizeof(caveBody));
    cave[sizeof(caveBody)] = 0xE9;
    const std::uint32_t caveJmp =
        Rel32(kCaveVa + sizeof(caveBody) + 5, kRetVa);
    std::memcpy(&cave[sizeof(caveBody) + 1], &caveJmp, 4);

    // redirect: MOV EDI,[0x008a94a8] (6B) -> JMP cave + NOP
    static const std::uint8_t redirPre[6] = {0x8B, 0x3D, 0xA8, 0x94, 0x8A, 0x00};
    std::uint8_t redir[6];
    redir[0] = 0xE9;
    const std::uint32_t redirJmp = Rel32(kRedirVa + 5, kCaveVa);
    std::memcpy(&redir[1], &redirJmp, 4);
    redir[5] = 0x90;

    if (BytesAre(kRedirVa, redir, sizeof(redir)) &&
        BytesAre(kCaveVa, cave, sizeof(cave))) {
        LogLine("UNLOCK: already applied (on-disk patch present) — OK");
        return;
    }
    if (!BytesAre(kRedirVa, redirPre, sizeof(redirPre))) {
        LogLine("UNLOCK: pre-bytes at 0x404eb4 unexpected — SKIPPED");
        return;
    }
    // cave padding must be zero (or already our bytes, handled above)
    static const std::uint8_t zeros[sizeof(cave)] = {0};
    if (!BytesAre(kCaveVa, zeros, sizeof(cave))) {
        LogLine("UNLOCK: cave at 0x5caf30 not zero padding — SKIPPED");
        return;
    }
    if (!WriteMem(kCaveVa, cave, sizeof(cave))) {
        LogLine("UNLOCK: cave write failed — SKIPPED");
        return;
    }
    if (!WriteMem(kRedirVa, redir, sizeof(redir))) {
        LogLine("UNLOCK: redirect write failed after cave write — INCONSISTENT");
        return;
    }
    LogLine("UNLOCK: restore-cave installed (tracks/cups=2, cars=1 after every restore)");
}

// ─── MASHED_DECOUPLE — measured frame time -> native tick quantizer ──────────
// Evidence (Ghidra pool12 session 2026-08-01, anchored binary):
//   FUN_00493390 (frame-time source, called ONLY from 0x00493480 which is itself
//   the CALL instruction `E8 0B FF FF FF`) measures real elapsed time into
//   _DAT_007f0ffc but then PINS the tick output:
//     0x004933d5: MOV [0x007f1000], 0x32   ; one 60Hz tick per rendered frame
//     0x0049341e: DAT_007f1004 = 1/60f
//   FUN_00493480 (the quantizer) then snaps DAT_007f1000 to 50/100/150/200-unit
//   bands, runs a sub-frame accumulator (DAT_007719d4), and emits
//   DAT_007f1000 = N*0x32 ticks + DAT_007f1004 = N*50 * (1/3000f @0x005cc948).
//   Consumers run the game update PER TICK, not per frame:
//     0x00492d6a / 0x00492da8 (FUN_00492d30 cases 3/6) and 0x0042c960:
//       N = (DAT_007f1000-1)/0x32+1;  do { FUN_004111c0(0x32); } while(--N);
//     0x0040fdde (FUN_0040fc00): explicit multi-tick catch-up when 0x3c < ticks.
//   i.e. the engine natively supports 0..4 ticks per rendered frame; the PC
//   build just feeds the quantizer a constant. This hook feeds it the MEASURED
//   frame time instead (units of 1/3000 s; 50 = one 60 Hz tick), which is the
//   entire framerate decoupling: at 165 fps most frames carry 0 ticks
//   (render-only), at 60 fps behavior is bit-identical (snap band 47..53 -> 50).
//   DAT_007f0ff0 (+0x32/frame @0x00493453; cosmetic scroll/effect clocks, e.g.
//   0x00401343) is corrected to advance by measured units too.
//   NOTE: _DAT_007f0ffc itself is unusable at >60fps — the pacing accumulator
//   DAT_00771984 (elapsed-50000 feedback @0x004933c2..) poisons it — so we
//   sample the raw timer FUN_004950b0 (3,000,000 units/s) ourselves.
namespace decouple {

using TimerFn = int(__cdecl*)();
constexpr std::uintptr_t kTimerFn      = 0x004950b0;  // raw timer, 3e6 units/s
constexpr std::uintptr_t kOrigFn       = 0x00493390;  // frame-time source
constexpr std::uintptr_t kCallSite     = 0x00493480;  // E8 0B FF FF FF (sole caller)
constexpr std::uintptr_t kTickUnits    = 0x007f1000;  // units in, N*0x32 ticks out
constexpr std::uintptr_t kEffectsClock = 0x007f0ff0;  // int, +0x32/frame stock
constexpr std::uint32_t  kUnitsCap     = 200;         // stock clamp = 200000 raw (4 ticks)

std::uint32_t s_prevTimer = 0;
std::uint32_t s_rem       = 0;   // sub-unit remainder carry (truncation costs ~3.5% at 165fps otherwise)

void __cdecl FrameTimeSourceFix() {
    reinterpret_cast<void(__cdecl*)()>(kOrigFn)();
    const std::uint32_t now = static_cast<std::uint32_t>(
        reinterpret_cast<TimerFn>(kTimerFn)());
    std::uint32_t d = now - s_prevTimer;
    s_prevTimer = now;
    if (d > kUnitsCap * 1000u) { d = kUnitsCap * 1000u; s_rem = 0; }  // first call / hitches
    d += s_rem;
    std::uint32_t units = d / 1000u;              // 1/3000 s units; 50 = 1/60 s
    s_rem = d % 1000u;
    *reinterpret_cast<volatile std::uint32_t*>(kTickUnits) = units;
    *reinterpret_cast<volatile std::int32_t*>(kEffectsClock) +=
        static_cast<std::int32_t>(units) - 0x32;
}

void Apply() {
    static const std::uint8_t pre[5] = {0xE8, 0x0B, 0xFF, 0xFF, 0xFF}; // CALL 0x493390
    if (!BytesAre(kCallSite, pre, sizeof(pre))) {
        LogLine("DECOUPLE: pre-bytes at 0x493480 unexpected — SKIPPED");
        return;
    }
    std::uint8_t patch[5];
    patch[0] = 0xE8;
    const std::uint32_t rel = Rel32(kCallSite + 5,
        reinterpret_cast<std::uintptr_t>(&FrameTimeSourceFix));
    std::memcpy(&patch[1], &rel, 4);
    if (WriteMem(kCallSite, patch, sizeof(patch)))
        LogLine("DECOUPLE: tick source = measured frame time (native quantizer live)");
    else
        LogLine("DECOUPLE: write failed — SKIPPED");
}

} // namespace decouple

// ─── MASHED_INTERP — 165Hz camera render interpolation ───────────────────────
// With MASHED_DECOUPLE, most rendered frames at >60fps carry 0 physics ticks, so
// the camera pose is frozen between ticks → 60Hz-stepped motion. This wraps the
// main-loop render call and, on every rendered frame, writes an interpolated
// camera pose (lerp of the last two tick poses by the sub-tick fraction) into the
// camera controller struct, rebuilds the RW frame, renders, then restores the
// true pose so the next tick's director is unperturbed.
//
// Evidence (Ghidra pool12 2026-08-01 + re/analysis/race_camera/race_camera.md):
//   Main loop FUN_00492290 calls the render fn at 0x004922b8:
//     004922b8: E8 D3 0B 00 00   CALL 0x00492e90   (render + buffer flip)
//   Camera controller struct DAT_00897fe0 holds the RENDERED pose:
//     +0x34 elevation°, +0x38 azimuth°, +0x3c roll°, +0x40..0x48 position xyz.
//   FUN_00441760(camStruct) (cdecl) rebuilds the camera's RW frame
//     (*(*(cam+0x84)+4), matrix +0x10) from exactly those fields — it is THE
//     commit-pose-to-frame function, called each tick by the director FUN_00446520
//     via FUN_0040d470 (which runs in the per-frame race tick BEFORE this render).
//   Sub-tick fraction: DAT_007719d4 is the quantizer's leftover accumulator in
//     [0,50) units where 50 = one 60Hz tick (FUN_00493480) → alpha = acc/50.
//   At 60fps a tick lands every frame and acc≈0 → alpha≈0 → renders the true
//   pose = bit-identical to stock. Only active in race phases (DAT_00771968∈{3,6}).
namespace interp {

constexpr std::uintptr_t kCallSite  = 0x004922b8;  // CALL FUN_00492e90
constexpr std::uintptr_t kRenderFn  = 0x00492e90;  // render + flip
constexpr std::uintptr_t kCamApply  = 0x00441760;  // void __cdecl(camStruct*)
constexpr std::uintptr_t kCamStruct = 0x00897fe0;
constexpr std::uintptr_t kAccum     = 0x007719d4;  // sub-tick accumulator [0,50)
constexpr std::uintptr_t kPhase     = 0x00771968;  // session-phase enum

using RenderFn   = void(__cdecl*)();
using CamApplyFn = void(__cdecl*)(std::uintptr_t);

// ── car RwFrame hierarchy (BFS 2026-08-01, car_frame_bfs2.py) ────────────────
// renderable = *(DAT_0063da18 + i*0x2ac); a car atom frame = *(renderable+0x4);
// ROOT = *(frame+0xa0) (RW 3.x RwFrame: child +0x98, next +0x9c, root +0xa0).
// The root frame holds the car world matrix at modelling +0x10 AND LTM +0x50.
// A single child-frame LTM write was a no-op (render recomputes children from the
// root), so we walk the root's whole subtree and interpolate every frame's LTM
// (+0x50 ONLY — writing modelling wedges the car: it is the fixed local
// part-offset). Verified via BBDUMP: cars render clean and intact at 165fps, no
// freeze/tear (verify/qol_asi_20260801/bb_interp165_f3000.png). The car body's
// LTM is read by the render (a +80 cartest visibly moved it); some parts recompute
// and micro-step, but per-frame interp deltas are tiny so it's imperceptible.
constexpr std::uintptr_t kCarRendBase = 0x0063da18;
constexpr std::uintptr_t kCarStride   = 0x000002ac;
constexpr std::uintptr_t kCarCount    = 0x008a94d0;
constexpr int kMaxCars   = 16;
constexpr int kMaxFrames = 48;   // per-car frame-subtree cap

inline std::uintptr_t RdPtr(std::uintptr_t a) {
    return *reinterpret_cast<volatile std::uintptr_t*>(a);
}
inline std::uintptr_t CarRootFrame(int i) {
    const std::uintptr_t rend = RdPtr(kCarRendBase + (std::uintptr_t)i * kCarStride);
    if (rend < 0x10000) return 0;
    const std::uintptr_t fa = RdPtr(rend + 0x4);
    if (fa < 0x10000) return 0;
    const std::uintptr_t root = RdPtr(fa + 0xa0);
    return (root >= 0x10000) ? root : fa;
}
// Collect root + all descendants into out[]. NB: we must NOT follow the root's
// own `next` (that is root's SIBLING — another car / world object, outside this
// subtree). So seed the stack with root->child only; thereafter follow both
// child (+0x98) and next (+0x9c), since a child's next-siblings are root's other
// children (in-subtree).
int CollectSubtree(std::uintptr_t root, std::uintptr_t* out, int cap) {
    int n = 0;
    if (root < 0x10000) return 0;
    out[n++] = root;
    std::uintptr_t stack[kMaxFrames];
    int sp = 0;
    const std::uintptr_t rc = RdPtr(root + 0x98);   // root->child
    if (rc >= 0x10000) stack[sp++] = rc;
    while (sp > 0 && n < cap) {
        std::uintptr_t f = stack[--sp];
        bool dup = false;
        for (int k = 0; k < n; ++k) if (out[k] == f) { dup = true; break; }
        if (dup) continue;
        out[n++] = f;
        const std::uintptr_t child = RdPtr(f + 0x98);
        const std::uintptr_t next  = RdPtr(f + 0x9c);
        if (child >= 0x10000 && sp < kMaxFrames) stack[sp++] = child;
        if (next  >= 0x10000 && sp < kMaxFrames) stack[sp++] = next;
    }
    return n;
}

struct Pose { float elev, azim, roll, px, py, pz; };
bool s_have = false;
Pose s_prev, s_curr;

inline float rdF(std::uintptr_t a) { return *reinterpret_cast<volatile float*>(a); }
inline void  wrF(std::uintptr_t a, float v) { *reinterpret_cast<volatile float*>(a) = v; }

Pose ReadPose() {
    Pose p;
    p.elev = rdF(kCamStruct + 0x34);
    p.azim = rdF(kCamStruct + 0x38);
    p.roll = rdF(kCamStruct + 0x3c);
    p.px   = rdF(kCamStruct + 0x40);
    p.py   = rdF(kCamStruct + 0x44);
    p.pz   = rdF(kCamStruct + 0x48);
    return p;
}
void WritePose(const Pose& p) {
    wrF(kCamStruct + 0x34, p.elev);
    wrF(kCamStruct + 0x38, p.azim);
    wrF(kCamStruct + 0x3c, p.roll);
    wrF(kCamStruct + 0x40, p.px);
    wrF(kCamStruct + 0x44, p.py);
    wrF(kCamStruct + 0x48, p.pz);
}
inline float LerpAngle(float a, float b, float t) {
    float d = b - a;
    while (d >  180.0f) d -= 360.0f;
    while (d < -180.0f) d += 360.0f;
    return a + d * t;
}
inline float LerpF(float a, float b, float t) { return a + (b - a) * t; }
inline bool SamePose(const Pose& a, const Pose& b) {
    return a.elev == b.elev && a.azim == b.azim && a.roll == b.roll &&
           a.px == b.px && a.py == b.py && a.pz == b.pz;
}

// ── per-frame matrix (modelling +0x10 and LTM +0x50; each: right/up/at/pos) ───
struct Mat { float m[12]; };  // 3 rows (r,u,a) + pos, packed 0..11
Mat ReadFrameLTM(std::uintptr_t f) {
    Mat o;
    for (int k = 0; k < 12; ++k) {
        const int row = k / 3, col = k % 3;
        o.m[k] = rdF(f + 0x50 + row*0x10 + col*4);   // LTM
    }
    return o;
}
void WriteFrame(std::uintptr_t f, const Mat& o) {
    // LTM (+0x50) ONLY — never touch modelling (+0x10): child frames' modelling
    // is their fixed local part-offset, which physics + the tick's LTM recompute
    // depend on. Overwriting it wedges the car. If the render reads the cached
    // LTM, this interpolates; if it recomputes LTM from modelling, this is a
    // harmless no-op (and physics is unperturbed).
    for (int k = 0; k < 12; ++k) {
        const int row = k / 3, col = k % 3;
        wrF(f + 0x50 + row*0x10 + col*4, o.m[k]);    // LTM
    }
}
inline bool SameMat(const Mat& a, const Mat& b) {
    for (int k = 0; k < 12; ++k) if (a.m[k] != b.m[k]) return false;
    return true;
}
inline Mat LerpMat(const Mat& a, const Mat& b, float t) {
    Mat o;
    for (int k = 0; k < 12; ++k) o.m[k] = a.m[k] + (b.m[k] - a.m[k]) * t;
    return o;
}
// per-frame prev/curr snapshots keyed by frame address (stable within a race)
struct FrameSnap { std::uintptr_t f; Mat prev, curr; bool have; };
FrameSnap s_fs[kMaxCars * kMaxFrames];
int s_fsN = 0;
FrameSnap* FindSnap(std::uintptr_t f) {
    for (int k = 0; k < s_fsN; ++k) if (s_fs[k].f == f) return &s_fs[k];
    if (s_fsN < kMaxCars * kMaxFrames) { FrameSnap* p = &s_fs[s_fsN++]; p->f = f; p->have = false; return p; }
    return nullptr;
}

void __cdecl Wrapper() {
    const std::uint32_t phase =
        *reinterpret_cast<volatile std::uint32_t*>(kPhase);
    const std::uintptr_t frameHolder =
        *reinterpret_cast<volatile std::uintptr_t*>(kCamStruct + 0x84);
    // Only interpolate a live in-race camera with a valid RW-frame chain; else
    // pass straight through (menus, loading, phase transitions).
    if ((phase != 3 && phase != 6) || frameHolder == 0) {
        s_have = false;
        s_fsN = 0;
        reinterpret_cast<RenderFn>(kRenderFn)();
        return;
    }

    std::uint32_t acc = *reinterpret_cast<volatile std::uint32_t*>(kAccum);
    float alpha = (float)acc / 50.0f;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    // ── camera ──
    const Pose truePose = ReadPose();
    if (!s_have) {
        s_curr = truePose; s_prev = truePose; s_have = true;
    } else if (!SamePose(truePose, s_curr)) {
        // A tick advanced the camera. Roll snapshots — but a far jump
        // (respawn / scene cut / director snap) is snapped, not interpolated.
        const float dx = truePose.px - s_curr.px;
        const float dy = truePose.py - s_curr.py;
        const float dz = truePose.pz - s_curr.pz;
        s_prev = (dx*dx + dy*dy + dz*dz > 100.0f * 100.0f) ? truePose : s_curr;
        s_curr = truePose;
    }
    Pose mid;
    mid.elev = LerpAngle(s_prev.elev, s_curr.elev, alpha);
    mid.azim = LerpAngle(s_prev.azim, s_curr.azim, alpha);
    mid.roll = LerpAngle(s_prev.roll, s_curr.roll, alpha);
    mid.px   = LerpF(s_prev.px, s_curr.px, alpha);
    mid.py   = LerpF(s_prev.py, s_curr.py, alpha);
    mid.pz   = LerpF(s_prev.pz, s_curr.pz, alpha);
    WritePose(mid);
    reinterpret_cast<CamApplyFn>(kCamApply)(kCamStruct);

    // ── cars: collect each car's frame subtree, snapshot true LTMs, write
    //    interpolated (or +80 lift for cartest); restore after render ──
    int carCount = *reinterpret_cast<volatile std::int32_t*>(kCarCount);
    if (carCount < 0) carCount = 0;
    if (carCount > kMaxCars) carCount = kMaxCars;
    std::uintptr_t frames[kMaxCars * kMaxFrames];
    Mat frTrue[kMaxCars * kMaxFrames];
    int frN = 0;
    for (int i = 0; i < carCount; ++i) {
        const std::uintptr_t root = CarRootFrame(i);
        if (!root) continue;
        std::uintptr_t sub[kMaxFrames];
        const int m = CollectSubtree(root, sub, kMaxFrames);
        for (int j = 0; j < m && frN < kMaxCars * kMaxFrames; ++j) {
            const std::uintptr_t f = sub[j];
            const Mat trueMat = ReadFrameLTM(f);
            frames[frN] = f;
            frTrue[frN] = trueMat;
            ++frN;
            FrameSnap* fs = FindSnap(f);
            if (!fs) continue;
            if (!fs->have) { fs->curr = trueMat; fs->prev = trueMat; fs->have = true; }
            else if (!SameMat(trueMat, fs->curr)) {
                const float dx = trueMat.m[9]  - fs->curr.m[9];
                const float dy = trueMat.m[10] - fs->curr.m[10];
                const float dz = trueMat.m[11] - fs->curr.m[11];
                fs->prev = (dx*dx + dy*dy + dz*dz > 100.0f * 100.0f) ? trueMat : fs->curr;
                fs->curr = trueMat;
            }
            WriteFrame(f, LerpMat(fs->prev, fs->curr, alpha));
        }
    }

    reinterpret_cast<RenderFn>(kRenderFn)();                // render + flip

    // ── restore true state (physics/next tick unperturbed) ──
    for (int k = 0; k < frN; ++k) WriteFrame(frames[k], frTrue[k]);
    WritePose(s_curr);
    reinterpret_cast<CamApplyFn>(kCamApply)(kCamStruct);    // restore true frame
}

void Apply() {
    static const std::uint8_t pre[5] = {0xE8, 0xD3, 0x0B, 0x00, 0x00}; // CALL 0x492e90
    if (!BytesAre(kCallSite, pre, sizeof(pre))) {
        LogLine("INTERP: call site 0x4922b8 bytes unexpected — SKIPPED");
        return;
    }
    std::uint8_t patch[5];
    patch[0] = 0xE8;
    const std::uint32_t rel =
        Rel32(kCallSite + 5, reinterpret_cast<std::uintptr_t>(&Wrapper));
    std::memcpy(&patch[1], &rel, 4);
    if (WriteMem(kCallSite, patch, sizeof(patch)))
        LogLine("INTERP: camera + car render interpolation live (race phases only)");
    else
        LogLine("INTERP: write failed — SKIPPED");
}

} // namespace interp

// ─── MASHED_JUMPFIX — preserve forward velocity through ramp takeoff ──────────
// Root cause (re/analysis/QOL_PATCH_PLAN_2026-08.md Item 4): on a contact frame
// FUN_0046EF70 (wheel contact spring/damper resolver) overwrites the car linear
// velocity +0x9B0/9B4/9B8 with the summed contact-spring force along the contact
// face normal, then clamps by a last-contact factor. A ramp-LIP triangle's
// near-vertical normal, accepted as a stale contact on the takeoff frame
// (classifier flag-reset path unknown, U-3629), redirects/kills FORWARD velocity
// → the car drops straight down. Intermittent.
//
// Fix (velocity-level, one tick delayed, CLEAN ABI — avoids FUN_0046EF70's
// implicit-EDI convention): retarget the sole call to the once-per-tick coupling
// bridge FUN_0047eb30 (0x0047eb30, void(), call @0x00470e15). FUN_0046EF70 runs
// LATER in the tick than the bridge, so at the START of the next tick's bridge
// the killed velocity is visible; we restore it there BEFORE the bridge drives
// the physics body, so forward momentum carries. We only act while a car is
// LEAVING the ground (grounded +0x9E0 <= 1.5) AND its horizontal speed collapsed
// >60% vs the previous tick from a real speed — a signature that cannot occur in
// normal airborne flight (no ground friction airborne) or grounded driving
// (grounded==4), nor a wall crash (stays grounded). Vertical velocity (+0x9B4)
// is left untouched so the upward launch and gravity are unaffected.
// VERIFIED 2026-08-02 (re/frida/capture_jump_bug.py, player-in-the-loop): the
// violent ramp-lip dead jump reproduced twice in ~1900 ticks with the fix OFF
// (recentMax~0.18, grounded 4->0, tilted lip normal -0.061,0.991,-0.12) and did
// NOT reproduce in ~10000 ticks with the fix ON. Evidence:
// verify/qol_asi_20260801/jump_capture_CONFIRMED.txt. Launcher enables it by
// default (opt out with -NoJumpFix). Gated by MASHED_JUMPFIX.
namespace jumpfix {

constexpr std::uintptr_t kCallSite  = 0x00470e15;  // CALL FUN_0047eb30
constexpr std::uintptr_t kBridgeFn  = 0x0047eb30;  // void()
constexpr std::uintptr_t kRecBase   = 0x008815a0;
constexpr std::uintptr_t kRecStride = 0x00000d04;
constexpr std::uintptr_t kCount     = 0x008a94d0;
constexpr int   kMaxCars    = 16;
constexpr float kGroundMax  = 1.5f;   // <=1 wheel touching = leaving the ground
constexpr float kMinSpeed   = 5.0f;   // must have carried real forward speed
constexpr float kCollapse   = 0.40f;  // cur < 40% of prev horiz = a kill

using BridgeFn = void(__cdecl*)();

bool  s_have[kMaxCars] = {};
float s_vx[kMaxCars], s_vz[kMaxCars];

inline float rF(std::uintptr_t a) { return *reinterpret_cast<volatile float*>(a); }
inline void  wF(std::uintptr_t a, float v) { *reinterpret_cast<volatile float*>(a) = v; }

void FixTakeoffKills() {
    int n = *reinterpret_cast<volatile std::int32_t*>(kCount);
    if (n < 0) n = 0; if (n > kMaxCars) n = kMaxCars;
    for (int i = 0; i < n; ++i) {
        const std::uintptr_t rec = kRecBase + (std::uintptr_t)i * kRecStride;
        const float g  = rF(rec + 0x9e0);
        const float vx = rF(rec + 0x9b0);
        const float vz = rF(rec + 0x9b8);
        if (s_have[i]) {
            const float prevH = (s_vx[i]*s_vx[i] + s_vz[i]*s_vz[i]);
            const float curH  = (vx*vx + vz*vz);
            if (g <= kGroundMax && prevH > kMinSpeed*kMinSpeed &&
                curH < prevH * (kCollapse*kCollapse)) {
                // takeoff velocity-kill → restore last tick's horizontal velocity.
                // `continue` leaves s_vx/s_vz as the good value (frozen while
                // airborne = correct projectile motion; gravity still acts on +0x9B4).
                wF(rec + 0x9b0, s_vx[i]);
                wF(rec + 0x9b8, s_vz[i]);
                continue;
            }
        }
        s_have[i] = true;
        s_vx[i] = vx; s_vz[i] = vz;
    }
}

void __cdecl Wrapper() {
    FixTakeoffKills();                              // correct last tick's kill first
    reinterpret_cast<BridgeFn>(kBridgeFn)();        // then run the real bridge
}

void Apply() {
    static const std::uint8_t pre[5] = {0xE8, 0x16, 0xDD, 0x00, 0x00}; // CALL 0x47eb30
    if (!BytesAre(kCallSite, pre, sizeof(pre))) {
        LogLine("JUMPFIX: call site 0x470e15 bytes unexpected — SKIPPED");
        return;
    }
    std::uint8_t patch[5];
    patch[0] = 0xE8;
    const std::uint32_t rel =
        Rel32(kCallSite + 5, reinterpret_cast<std::uintptr_t>(&Wrapper));
    std::memcpy(&patch[1], &rel, 4);
    if (WriteMem(kCallSite, patch, sizeof(patch)))
        LogLine("JUMPFIX: takeoff velocity-preserve live (verified 2026-08-02)");
    else
        LogLine("JUMPFIX: write failed — SKIPPED");
}

} // namespace jumpfix

// ─── MASHED_RES — retarget the screen-dimension getters ──────────────────────
// The d3d9 shim reads the same env var and sizes the backbuffer; the two getters
// MUST return the identical size or the camera frameBuffer raster fails against
// the device and boot AVs (root cause narrative: patch_mashed_fix_camera_res.py
// / re/analysis/BOOT_CRASH_ROOTCAUSE_2026-06-13.md).
//   FUN_00498bc0 (width)  @0x00498bc0: 6 bytes, on-disk-patched form B8 imm32 C3
//   FUN_00498bd0 (height) @0x00498bd0: same; pristine form A1 <glob> C3 also accepted
void ApplyRes() {
    char v[32] = {};
    if (GetEnvironmentVariableA("MASHED_RES", v, sizeof(v)) == 0) return;
    std::uint32_t w = 0, h = 0; const char* p = v;
    for (; *p >= '0' && *p <= '9'; ++p) w = w * 10 + (std::uint32_t)(*p - '0');
    if (*p == 'x' || *p == 'X')
        for (++p; *p >= '0' && *p <= '9'; ++p) h = h * 10 + (std::uint32_t)(*p - '0');
    if (w < 320 || h < 240 || w > 7680 || h > 4320) {
        LogLine("RES: MASHED_RES unparsable/out of range — SKIPPED");
        return;
    }
    struct Site { std::uintptr_t va; std::uint8_t pristine1; std::uint32_t val; };
    const Site sites[2] = {
        {0x00498bc0u, 0x28, w},   // pristine: A1 28 60 61 00 C3
        {0x00498bd0u, 0x2C, h},   // pristine: A1 2C 60 61 00 C3
    };
    for (const Site& s : sites) {
        const std::uint8_t* cur = reinterpret_cast<const std::uint8_t*>(s.va);
        const bool patchedForm  = (cur[0] == 0xB8 && cur[5] == 0xC3);
        const bool pristineForm = (cur[0] == 0xA1 && cur[1] == s.pristine1 &&
                                   cur[2] == 0x60 && cur[3] == 0x61 &&
                                   cur[4] == 0x00 && cur[5] == 0xC3);
        if (!patchedForm && !pristineForm) {
            LogLine("RES: getter bytes unexpected — SKIPPED (res mismatch AV risk: unset MASHED_RES)");
            return;
        }
        std::uint8_t stub[6];
        stub[0] = 0xB8;
        std::memcpy(&stub[1], &s.val, 4);
        stub[5] = 0xC3;
        if (!WriteMem(s.va, stub, sizeof(stub))) {
            LogLine("RES: getter write failed — SKIPPED");
            return;
        }
    }
    LogLine("RES: screen-dim getters retargeted to MASHED_RES");
}

void InitLogPath() {
    g_fileLog = EnvSet("MASHED_QOL_LOG");
    if (!g_fileLog) return;
    wchar_t exeDir[MAX_PATH];
    DWORD got = GetModuleFileNameW(nullptr, exeDir, MAX_PATH);
    if (got == 0 || got >= MAX_PATH) { g_fileLog = false; return; }
    for (DWORD i = got; i > 0; --i) {
        if (exeDir[i-1] == L'\\' || exeDir[i-1] == L'/') { exeDir[i] = 0; break; }
    }
    _snwprintf_s(g_logPath, _TRUNCATE, L"%s..\\log\\qol_asi.txt", exeDir);
}

void ApplyAll() {
    InitLogPath();
    const bool noSave   = EnvSet("MASHED_NO_SAVE");
    const bool unlock   = EnvSet("MASHED_UNLOCK");
    const bool decouple = EnvSet("MASHED_DECOUPLE");
    const bool interpol = EnvSet("MASHED_INTERP");
    const bool jumpfx   = EnvSet("MASHED_JUMPFIX");
    char resBuf[4] = {};
    const bool res = GetEnvironmentVariableA("MASHED_RES", resBuf, sizeof(resBuf)) > 0;
    if (!noSave && !unlock && !decouple && !res && !interpol && !jumpfx) return;  // inert boot
    LogLine("attach: applying QoL patches");
    if (noSave)   ApplyNoSave();
    if (unlock)   ApplyUnlock();
    if (decouple) decouple::Apply();
    if (interpol) interp::Apply();
    if (jumpfx)   jumpfix::Apply();
    if (res)      ApplyRes();
}

} // namespace

BOOL WINAPI DllMain(HINSTANCE hThis, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hThis);
        ApplyAll();
    }
    return TRUE;
}
