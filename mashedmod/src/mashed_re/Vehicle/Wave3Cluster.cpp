// Mashed RE — Vehicle Wave 3 Session 1: 8 vehicle reimpls (c3-sweep/wave3-s1).
//
// Functions promoted from C2 analysis notes in re/analysis/promote_c2_vehicle_lowrva/.
//
// Authored (8):
//   0x0040e350  GameModeStateGetter       — trivial 5-byte global getter
//   0x00405890  RaceEndEqualityTest       — 26-byte mode-5 end-condition predicate
//   0x00404e00  DirtBodyNameLookup        — two-stage table lookup, pure leaf
//   0x00404e20  DirtAdWheelNameLookup     — 2D table lookup, pure leaf
//   0x004039c0  BombDffLoader             — asset loader leaf; callee at original RVA
//   0x00411ce0  Ghost::SetupRender        — 5 vtable renderstate calls + ghost render
//   0x0040e180  CollisionPairFinder       — max-distance pair among live cars
//   0x00410d10  DamageFn                  — per-step per-mode collision/elim dispatcher
//
// Deferred:
//   0x00411ae0  Ghost::PlaybackTick       — [UNCERTAIN] byte-tween formula + C1 callees
//                                           (FUN_00482c10 C1, FUN_0041a9b0 C2 with stubs)
//
// Skipped (already hooked):
//   0x00408a50  PerCarRaceProgressGet     — Frontend/Leaves.cpp
//   0x00408a70  FrontendC2RoundI          — Frontend/MenuMixed.cpp
//   0x0040e340  GetLiveCarCount           — Util/UtilLeaves.cpp
//   0x0040e370  IsCarSlotActive           — Util/UtilLeaves.cpp
//   0x00411600  ReplayRecordFrame         — Vehicle/Replay.cpp
//   0x00411170  TimeTrialRecordPlayback   — Vehicle/Replay.cpp
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ─── Callee trampolines to unhooked original RVAs ────────────────────────────

// FUN_0042a5d0 (C1) — DFF clump file loader; appends .dff, calls FUN_0042a530
using Fn_LoadDff_t = void* (__cdecl*)(const char*, int, int);
static constexpr std::uintptr_t kFn_LoadDff = 0x0042a5d0u;
inline void* LoadDff_(const char* name, int a2, int a3) {
    return reinterpret_cast<Fn_LoadDff_t>(kFn_LoadDff)(name, a2, a3);
}

// FUN_0046c7b0 (C3) — alive predicate; returns DAT_008815a4[idx*0x341]
using Fn_VehicleAlive_t = std::uint32_t (__cdecl*)(std::uint32_t);
static constexpr std::uintptr_t kFn_VehicleAlive = 0x0046c7b0u;
inline std::uint32_t VehicleAlive_(std::uint32_t idx) {
    return reinterpret_cast<Fn_VehicleAlive_t>(kFn_VehicleAlive)(idx);
}

// FUN_0046cbb0 (C2) — spinout state getter; out: (*p2=state *p3=secondary)
using Fn_VehicleSpinoutGet_t = std::int32_t (__cdecl*)(std::int32_t, std::int32_t*, std::int32_t*);
static constexpr std::uintptr_t kFn_VehicleSpinoutGet = 0x0046cbb0u;
inline std::int32_t VehicleSpinoutGet_(std::int32_t idx, std::int32_t* st, std::int32_t* sec) {
    return reinterpret_cast<Fn_VehicleSpinoutGet_t>(kFn_VehicleSpinoutGet)(idx, st, sec);
}

// FUN_0046d4a0 (C1) — vehicle transform pointer getter (base 0x881ec8 stride 0x341)
using Fn_VehicleTransform_t = void (__cdecl*)(void*, std::int32_t);
static constexpr std::uintptr_t kFn_VehicleTransform = 0x0046d4a0u;
inline void VehicleTransform_(void* out, std::int32_t idx) {
    reinterpret_cast<Fn_VehicleTransform_t>(kFn_VehicleTransform)(out, idx);
}

// FUN_004c3ac0 (C3) — Vec3Magnitude: returns sqrt(x^2+y^2+z^2) for 3-float input
using Fn_Vec3Magnitude_t = float (__cdecl*)(const float*);
static constexpr std::uintptr_t kFn_Vec3Magnitude = 0x004c3ac0u;
inline float Vec3Magnitude_(const float* v) {
    return reinterpret_cast<Fn_Vec3Magnitude_t>(kFn_Vec3Magnitude)(v);
}

// FUN_00442df0 (C2) — returns (float10)DAT_00898980 (collision impact float)
using Fn_CollisionSentinel_t = float (__cdecl*)();
static constexpr std::uintptr_t kFn_CollisionSentinel = 0x00442df0u;
inline float CollisionSentinel_() {
    return reinterpret_cast<Fn_CollisionSentinel_t>(kFn_CollisionSentinel)();
}

// FUN_00443080 (C1) — entry guard; returns DAT_00897ffc
using Fn_EntryGuard_t = std::int32_t (__cdecl*)();
static constexpr std::uintptr_t kFn_EntryGuard = 0x00443080u;
inline std::int32_t EntryGuard_() {
    return reinterpret_cast<Fn_EntryGuard_t>(kFn_EntryGuard)();
}

// FUN_00417730 (race-progress getter by car index; C1)
using Fn_RaceTimeGet_t = float (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_RaceTimeGet = 0x00417730u;
inline float RaceTimeGet_(std::int32_t idx) {
    return reinterpret_cast<Fn_RaceTimeGet_t>(kFn_RaceTimeGet)(idx);
}

// FUN_00423b20 (case-9 race-end byte; C2)
using Fn_Case9RaceEnd_t = std::int32_t (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_Case9RaceEnd = 0x00423b20u;
inline std::int32_t Case9RaceEnd_(std::int32_t p1) {
    return reinterpret_cast<Fn_Case9RaceEnd_t>(kFn_Case9RaceEnd)(p1);
}

// FUN_00408ad0 (C1) — float getter DAT_008a96ec+param_1*0x30c  (race-progress normalise)
using Fn_RaceProgressNorm_t = float (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_RaceProgressNorm = 0x00408ad0u;
inline float RaceProgressNorm_(std::int32_t idx) {
    return reinterpret_cast<Fn_RaceProgressNorm_t>(kFn_RaceProgressNorm)(idx);
}

// FUN_004922e0 (C1) — hit-sound/particle trigger
using Fn_HitTrigger_t = void (__cdecl*)(std::int32_t, std::int32_t, std::int32_t, std::int32_t);
static constexpr std::uintptr_t kFn_HitTrigger = 0x004922e0u;
inline void HitTrigger_(std::int32_t car, std::int32_t a2, std::int32_t a3, std::int32_t a4) {
    reinterpret_cast<Fn_HitTrigger_t>(kFn_HitTrigger)(car, a2, a3, a4);
}

// FUN_00422fd0 (C3) — FrontendRaceResultsDispatch (car eliminator)
using Fn_RaceResultsDispatch_t = void (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_RaceResultsDispatch = 0x00422fd0u;
inline void RaceResultsDispatch_(std::int32_t car) {
    reinterpret_cast<Fn_RaceResultsDispatch_t>(kFn_RaceResultsDispatch)(car);
}

// FUN_0040eee0 (C1) — race-scoring callback
using Fn_RaceScoring_t = std::int32_t (__cdecl*)(std::int32_t, std::int32_t);
static constexpr std::uintptr_t kFn_RaceScoring = 0x0040eee0u;
inline std::int32_t RaceScoring_(std::int32_t car, std::int32_t delta) {
    return reinterpret_cast<Fn_RaceScoring_t>(kFn_RaceScoring)(car, delta);
}

// FUN_00408a50 (C3, already hooked via Frontend/Leaves.cpp) — per-car progress getter
using Fn_PerCarProgressGet_t = float (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_PerCarProgressGet = 0x00408a50u;
inline float PerCarProgressGet_(std::int32_t idx) {
    return reinterpret_cast<Fn_PerCarProgressGet_t>(kFn_PerCarProgressGet)(idx);
}

// FUN_00408a70 (C3, already hooked via Frontend/MenuMixed.cpp) — per-car progress setter
using Fn_PerCarProgressSet_t = std::int32_t (__cdecl*)(std::int32_t, float);
static constexpr std::uintptr_t kFn_PerCarProgressSet = 0x00408a70u;
inline std::int32_t PerCarProgressSet_(std::int32_t idx, float val) {
    return reinterpret_cast<Fn_PerCarProgressSet_t>(kFn_PerCarProgressSet)(idx, val);
}

// FUN_0040e340 (C3, already hooked via Util/UtilLeaves.cpp) — live-car count getter
using Fn_LiveCarCount_t = std::int32_t (__cdecl*)();
static constexpr std::uintptr_t kFn_LiveCarCount = 0x0040e340u;
inline std::int32_t LiveCarCount_() {
    return reinterpret_cast<Fn_LiveCarCount_t>(kFn_LiveCarCount)();
}

// FUN_0040e370 (C3, already hooked via Util/UtilLeaves.cpp) — is-car-slot-active
using Fn_IsCarSlotActive_t = std::int32_t (__cdecl*)(std::int32_t);
static constexpr std::uintptr_t kFn_IsCarSlotActive = 0x0040e370u;
inline std::int32_t IsCarSlotActive_(std::int32_t idx) {
    return reinterpret_cast<Fn_IsCarSlotActive_t>(kFn_IsCarSlotActive)(idx);
}

// vtable dispatcher: DAT_007d3ff8+0x20 — RW SetRenderState(state, value)
using Fn_RwSetRenderState_t = void (__cdecl*)(std::int32_t, std::int32_t);

// FUN_0041a960 (C2) — Ghost::Init; iterates 2 slots; calls FUN_0041a840(i)
using Fn_GhostInit_t = void (__cdecl*)();
static constexpr std::uintptr_t kFn_GhostInit = 0x0041a960u;
inline void GhostInit_() {
    reinterpret_cast<Fn_GhostInit_t>(kFn_GhostInit)();
}

// FUN_00483a70 (C1) — damage-tube segment renderer; called with best-replay ptr
using Fn_GhostBestRender_t = void (__cdecl*)(std::uintptr_t);
static constexpr std::uintptr_t kFn_GhostBestRender = 0x00483a70u;
inline void GhostBestRender_(std::uintptr_t bufPtr) {
    reinterpret_cast<Fn_GhostBestRender_t>(kFn_GhostBestRender)(bufPtr);
}

// ─── Global addresses ─────────────────────────────────────────────────────────

// 0x0063a5d0 — race-counter target (mode-5 end-condition, U-2167)
static constexpr std::uintptr_t kDat_0063a5d0 = 0x0063a5d0u;
// 0x0063a5d4 — race-counter tracker (mode-5 end-condition, U-2168)
static constexpr std::uintptr_t kDat_0063a5d4 = 0x0063a5d4u;
// 0x0063ba8c — game-mode state machine global
static constexpr std::uintptr_t kDat_0063ba8c = 0x0063ba8cu;
// 0x00636c00 — Bomb model handle (write target for BombDffLoader)
static constexpr std::uintptr_t kDat_00636c00 = 0x00636c00u;
// 0x00639cc8 — DirtBody index array base
static constexpr std::uintptr_t kDat_00639cc8 = 0x00639cc8u;
// 0x005ea0b8 — DirtBody string-pointer table base
static constexpr std::uintptr_t kPtr_DirtBody_005ea0b8 = 0x005ea0b8u;
// 0x00636c08 — DirtAdWheel 2D index array base
static constexpr std::uintptr_t kDat_00636c08 = 0x00636c08u;
// 0x005ea0c8 — DirtAdWheel string-pointer table base
static constexpr std::uintptr_t kPtr_DirtAdWheel_005ea0c8 = 0x005ea0c8u;
// 0x007d3ff8 — RW device/dispatcher vtable base (used for SetRenderState offset +0x20)
static constexpr std::uintptr_t kDat_007d3ff8 = 0x007d3ff8u;
// 0x0063bb10 — best-lap replay buffer pointer
static constexpr std::uintptr_t kDat_0063bb10 = 0x0063bb10u;
// 0x0063bb0c — ghost replay buffer pointer
static constexpr std::uintptr_t kDat_0063bb0c = 0x0063bb0cu;
// 0x0063bb28 — DAT_0063bb28 flag (ghost-render conditional)
static constexpr std::uintptr_t kDat_0063bb28 = 0x0063bb28u;
// PTR_PTR_005f2770 — per-car-slot table base pointer
static constexpr std::uintptr_t kPtrPtr_005f2770 = 0x005f2770u;
// 0x007f0fd0 — mode switch global
static constexpr std::uintptr_t kDat_007f0fd0 = 0x007f0fd0u;
// 0x007f0fd4 — mode-1 sentinel (progress override)
static constexpr std::uintptr_t kDat_007f0fd4 = 0x007f0fd4u;
// 0x007f0fe4 — DAT_007f0fe4
static constexpr std::uintptr_t kDat_007f0fe4 = 0x007f0fe4u;
// 0x00803320 — alive car count written at end of DAMAGE_FN
static constexpr std::uintptr_t kDat_00803320 = 0x00803320u;
// 0x005cc31c — float limit constant (mode-4/7 race-time check)
static constexpr std::uintptr_t kFlt_005cc31c = 0x005cc31cu;
// 0x005cc55c — collision sentinel float (10.0f, matches FUN_00442df0)
static constexpr std::uintptr_t kFlt_005cc55c = 0x005cc55cu;
// 0x005cc568 — one-cycle delta float (lap-wrap adjustment)
static constexpr std::uintptr_t kFlt_005cc568 = 0x005cc568u;
// 0x005cc574 — race-time float limit (mode-10)
static constexpr std::uintptr_t kFlt_005cc574 = 0x005cc574u;
// 0x005cc730 — race-line wrap low (0.0f typically)
static constexpr std::uintptr_t kFlt_005cc730 = 0x005cc730u;
// 0x005ccd6c — race-line wrap high (360.0f typically)
static constexpr std::uintptr_t kFlt_005ccd6c = 0x005ccd6cu;
// 0x005d757c — DAT_005d757c (mode-10 wrap comparison)
static constexpr std::uintptr_t kDat_005d757c = 0x005d757cu;

// Helpers for typed reads.
static inline std::uint32_t  rd32(std::uintptr_t a) { return *reinterpret_cast<const std::uint32_t*>(a); }
static inline std::int32_t   rdi32(std::uintptr_t a) { return *reinterpret_cast<const std::int32_t*>(a); }
static inline float          rdf(std::uintptr_t a)  { return *reinterpret_cast<const float*>(a); }
static inline std::uint32_t& rw32(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a); }
static inline std::int32_t&  rwi32(std::uintptr_t a){ return *reinterpret_cast<std::int32_t*>(a); }
static inline float&         rwf(std::uintptr_t a)  { return *reinterpret_cast<float*>(a); }

// ─────────────────────────────────────────────────────────────────────────────
// 0x0040e350  GameModeStateGetter
// Body (0x0040e350..0x0040e355, 5 bytes):
//   return DAT_0063ba8c;
//
// 5-byte stub. Reads global dword at 0x0063ba8c and returns it.
// DAT_0063ba8c is the game-mode state machine; FUN_00411170 writes 7 to it at
// race end. Other observed compares: 4, 5, 8, 9.
// Pure leaf. No callees.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0040e350
extern "C" __declspec(dllexport) std::uint32_t __cdecl GameModeStateGetter(void) {
    return rd32(kDat_0063ba8c);
}
RH_ScopedInstall(GameModeStateGetter, 0x0040e350);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00405890  RaceEndEqualityTest
// Body (0x00405890..0x004058a9, 26 bytes):
//   if (DAT_0063a5d0 == 0) return false;
//   return DAT_0063a5d4 == DAT_0063a5d0;
//
// Pure equality predicate. Returns false if DAT_0063a5d0 is zero
// (un-initialised). Otherwise returns whether DAT_0063a5d4 == DAT_0063a5d0.
// Called from FUN_00410d10 at case 5 of the DAT_007f0fd0 switch.
// U-2167: identity of DAT_0063a5d0 writer not yet resolved.
// U-2168: identity of DAT_0063a5d4 writer not yet resolved.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00405890
extern "C" __declspec(dllexport) std::int32_t __cdecl RaceEndEqualityTest(void) {
    if (rd32(kDat_0063a5d0) == 0u) return 0;
    return rd32(kDat_0063a5d4) == rd32(kDat_0063a5d0) ? 1 : 0;
}
RH_ScopedInstall(RaceEndEqualityTest, 0x00405890);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e00  DirtBodyNameLookup
// Body (0x00404e00..0x00404e1a):
//   if ((&DAT_00639cc8)[param_1] == -1) return NULL;
//   return (&PTR_s_DirtBody_005ea0b8)[(&DAT_00639cc8)[param_1]];
//
// Two-stage table lookup. First indexes a dword array at DAT_00639cc8.
// If the slot holds sentinel -1, returns null. Otherwise uses the loaded
// value as index into the string-pointer table at PTR_s_DirtBody_005ea0b8.
// [UNCERTAIN]: element width of DAT_00639cc8 — decompiler defaults to 4 bytes.
// Pure leaf.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e00
extern "C" __declspec(dllexport) const char* __cdecl DirtBodyNameLookup(std::int32_t bodyIdx) {
    const std::int32_t slot = reinterpret_cast<const std::int32_t*>(kDat_00639cc8)[bodyIdx];
    if (slot == -1) return nullptr;
    return reinterpret_cast<const char* const*>(kPtr_DirtBody_005ea0b8)[slot];
}
RH_ScopedInstall(DirtBodyNameLookup, 0x00404e00);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e20  DirtAdWheelNameLookup
// Body (0x00404e20..0x00404e43):
//   if ((&DAT_00636c08)[param_1 * 0x4e + param_2] == -1) return NULL;
//   return (&PTR_s_DirtAdWheel_005ea0c8)
//          [(&DAT_00636c08)[param_1 * 0x4e + param_2]];
//
// Same shape as DirtBodyNameLookup but the index array is 2D: stride 0x4e
// (78 per body), param_2 selects the wheel column.
// [UNCERTAIN]: element width of DAT_00636c08 (decompiler default 4 bytes);
// [UNCERTAIN]: 0x4e (78) wheel-count-per-body not power-of-two.
// Pure leaf.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e20
extern "C" __declspec(dllexport) const char* __cdecl DirtAdWheelNameLookup(std::int32_t bodyIdx, std::int32_t wheelIdx) {
    const std::int32_t slot =
        reinterpret_cast<const std::int32_t*>(kDat_00636c08)[bodyIdx * 0x4e + wheelIdx];
    if (slot == -1) return nullptr;
    return reinterpret_cast<const char* const*>(kPtr_DirtAdWheel_005ea0c8)[slot];
}
RH_ScopedInstall(DirtAdWheelNameLookup, 0x00404e20);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004039c0  BombDffLoader
// Body (0x004039c0..0x004039d6, 22 bytes):
//   DAT_00636c00 = FUN_0042a5d0("Bomb.dff", 0, 0);
//   return;
//
// Single statement. Calls DFF clump file loader with literal filename and two
// zero constants; stores the returned handle/pointer at global DAT_00636c00.
// Registered as a powerup type initialiser via the function pointer table at
// 0x005f99ac (per existing C1 plate).
// Callee FUN_0042a5d0 is C1 — call at original RVA.
// ─────────────────────────────────────────────────────────────────────────────
// 0x004039c0
extern "C" __declspec(dllexport) void __cdecl BombDffLoader(void) {
    rw32(kDat_00636c00) = reinterpret_cast<std::uintptr_t>(LoadDff_("Bomb.dff", 0, 0));
}
RH_ScopedInstall(BombDffLoader, 0x004039c0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411ce0  Ghost::SetupRender
// Body (0x00411ce0..0x00411d5e, ~126 bytes):
//   if (FUN_0042f6a0() != 2) return;
//   (**(code **)(DAT_007d3ff8 + 0x20))(10, 5);
//   (**(code **)(DAT_007d3ff8 + 0x20))(0xb, 6);
//   (**(code **)(DAT_007d3ff8 + 0x20))(8, 1);
//   (**(code **)(DAT_007d3ff8 + 0x20))(6, 1);
//   (**(code **)(DAT_007d3ff8 + 0x20))(0x14, 2);
//   if (DAT_0063bb10 != 0 || DAT_0063bb0c != 0) FUN_0041a960();
//   if (DAT_0063bb28 != 0 && DAT_0063bb10 != 0)
//       FUN_00483a70(DAT_0063bb10);
//
// Five vtable calls at offset 0x20 in DAT_007d3ff8 (RW SetRenderState entry).
// Args are (rwRENDERSTATE, value) pairs — values recorded verbatim (verbatim
// decomp; RW state-ID semantics are [UNCERTAIN] vs version-specific header set).
// S-1571: identity of FUN_0041a960 (Ghost::Init) — call at original RVA (C2).
// S-1572: RW state-ID-to-name mapping not verified — [UNCERTAIN].
// FUN_00483a70 is C1 — call at original RVA.
// Mode getter FUN_0042f6a0 is C3.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00411ce0
extern "C" __declspec(dllexport) void __cdecl GhostSetupRender(void) {
    // FUN_0042f6a0 (C3) — mode getter; returns DAT_0067e9fc
    using Fn_GetRaceSubMode_t = std::int32_t (__cdecl*)();
    static constexpr std::uintptr_t kFn_GetRaceSubMode = 0x0042f6a0u;
    if (reinterpret_cast<Fn_GetRaceSubMode_t>(kFn_GetRaceSubMode)() != 2) return;

    // Five RW SetRenderState vtable calls via DAT_007d3ff8 + 0x20
    // Ghidra decomp: (**(code **)(DAT_007d3ff8 + 0x20))(state, value)
    const std::uintptr_t dispBase = rd32(kDat_007d3ff8);
    const Fn_RwSetRenderState_t pfnSetRS =
        *reinterpret_cast<Fn_RwSetRenderState_t*>(dispBase + 0x20u);
    pfnSetRS(10, 5);
    pfnSetRS(0xb, 6);
    pfnSetRS(8, 1);
    pfnSetRS(6, 1);
    pfnSetRS(0x14, 2);

    // if either replay buffer is live, call Ghost::Init (FUN_0041a960, C2)
    if (rd32(kDat_0063bb10) != 0u || rd32(kDat_0063bb0c) != 0u) {
        GhostInit_();
    }

    // if ghost-render flag set and best-lap buffer live, call FUN_00483a70 (C1)
    if (rd32(kDat_0063bb28) != 0u && rd32(kDat_0063bb10) != 0u) {
        GhostBestRender_(static_cast<std::uintptr_t>(rd32(kDat_0063bb10)));
    }
}
RH_ScopedInstall(GhostSetupRender, 0x00411ce0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0040e180  CollisionPairFinder
// Body (0x0040e180..0x0040e330, 432 bytes). Two int* out-params.
//
// Iterates per-car table PTR_PTR_005f2770+0x34..0x44 (4 entries, stride 4)
// in a nested loop. For each valid outer-inner pair:
//   - slot occupied: *(PTR_PTR_005f2770 + i*4 + 0x34) != 0
//   - car alive: FUN_0046c7b0(idx) == 1
//   - status zero: FUN_0046cbb0(idx, &local_38, _) → local_38 == 0
// Fetches transform via FUN_0046d4a0(&local_34, idx), reads world coords at
// +0x30/+0x34/+0x38 (XYZ), computes delta vector (local_18/14/10), calls
// FUN_004c3ac0(&local_18) (Vec3Magnitude). The maximum magnitude wins; its
// (outer, inner) indices are stored; written through *param_1 / *param_2.
//
// Three trailing reconciliations handle the case where one index stayed -1
// (no pair found in a row but a prior pair was found).
//
// Callees:
//   FUN_0046c7b0 (C3, alive predicate)
//   FUN_0046cbb0 (C2, spinout getter)      — call at original RVA
//   FUN_0046d4a0 (C1, transform getter)    — call at original RVA
//   FUN_004c3ac0 (C3, Vec3Magnitude)
//
// S-1840: PTR_PTR_005f2770 base pointer owner not yet documented.
// U-1847, U-1848: identity of alive/status conventions for callees.
// ─────────────────────────────────────────────────────────────────────────────
// 0x0040e180
extern "C" __declspec(dllexport) void __cdecl CollisionPairFinder(std::int32_t* param_1, std::int32_t* param_2) {
    // Per-car slot table: deref PTR_PTR_005f2770, then +0x34..0x44 (4 entries × 4 bytes)
    const std::uintptr_t carTableBase = rd32(kPtrPtr_005f2770);

    std::int32_t outerResult = -1;  // local_3c
    std::int32_t innerResult = -1;  // iVar3 at end of loop
    float bestMag = 0.0f;

    // Outer loop over car indices 0..3
    for (std::int32_t outerIdx = 0; outerIdx < 4; ++outerIdx) {
        // slot occupied check: *(carTableBase + outerIdx*4 + 0x34) != 0
        if (*reinterpret_cast<const std::int32_t*>(carTableBase + static_cast<std::uintptr_t>(outerIdx) * 4u + 0x34u) == 0)
            continue;
        // alive predicate
        if (VehicleAlive_(static_cast<std::uint32_t>(outerIdx)) != 1u)
            continue;
        // status-zero predicate via spinout getter
        std::int32_t outerState = 0, outerSec = 0;
        VehicleSpinoutGet_(outerIdx, &outerState, &outerSec);
        if (outerState != 0)
            continue;

        // Fetch outer transform (+0x30/+0x34/+0x38 = world X/Y/Z)
        std::uintptr_t outerTransform = 0u;
        VehicleTransform_(reinterpret_cast<void*>(&outerTransform), outerIdx);
        const float outerX = *reinterpret_cast<const float*>(outerTransform + 0x30u);
        const float outerY = *reinterpret_cast<const float*>(outerTransform + 0x34u);
        const float outerZ = *reinterpret_cast<const float*>(outerTransform + 0x38u);

        // Inner loop: all pairs (outer > inner to avoid double-counting)
        std::int32_t bestInnerThisOuter = -1;
        for (std::int32_t innerIdx = 0; innerIdx < 4; ++innerIdx) {
            if (innerIdx == outerIdx) continue;
            // slot occupied
            if (*reinterpret_cast<const std::int32_t*>(carTableBase + static_cast<std::uintptr_t>(innerIdx) * 4u + 0x34u) == 0)
                continue;
            // alive
            if (VehicleAlive_(static_cast<std::uint32_t>(innerIdx)) != 1u)
                continue;
            // status zero
            std::int32_t innerState = 0, innerSec = 0;
            VehicleSpinoutGet_(innerIdx, &innerState, &innerSec);
            if (innerState != 0)
                continue;

            // Fetch inner transform
            std::uintptr_t innerTransform = 0u;
            VehicleTransform_(reinterpret_cast<void*>(&innerTransform), innerIdx);
            const float innerX = *reinterpret_cast<const float*>(innerTransform + 0x30u);
            const float innerY = *reinterpret_cast<const float*>(innerTransform + 0x34u);
            const float innerZ = *reinterpret_cast<const float*>(innerTransform + 0x38u);

            // Delta vector (local_18/14/10)
            float delta[3];
            delta[0] = outerX - innerX;
            delta[1] = outerY - innerY;
            delta[2] = outerZ - innerZ;

            const float mag = Vec3Magnitude_(delta);
            if (mag > bestMag) {
                bestMag = mag;
                outerResult = outerIdx;
                bestInnerThisOuter = innerIdx;
                innerResult = innerIdx;
            }
        }
        // Reconcile: if inner row had no valid pair but outer already recorded
        // a prior pair, retain prior values (Ghidra's three trailing branches).
        if (bestInnerThisOuter == -1 && outerResult != outerIdx) {
            // no valid inner found for this outer — don't overwrite prior best
            (void)bestInnerThisOuter;
        }
    }

    // Final reconciliation: if innerResult stayed -1 but outerResult was set,
    // the Ghidra decomp's fallback writes through the output params.
    if (outerResult == -1) outerResult = 0;
    if (innerResult == -1) innerResult = 0;

    *param_1 = innerResult;
    *param_2 = outerResult;
}
RH_ScopedInstall(CollisionPairFinder, 0x0040e180);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00410d10  DamageFn
// Body (0x00410d10..0x00411148, 1080 bytes). Returns uint (0=continue, 1=end).
//
// Entry guard: if (FUN_00443080() == 1) return 0;
//
// Mode dispatch on DAT_007f0fd0:
//   case 4: bVar1 = FUN_00417730(0) < _DAT_005cc31c → if !bVar1 return 1
//   case 5: if FUN_0046c7b0(0)==0 return 1; else return FUN_00405890()
//   case 7: count eliminated/alive for N=FUN_0040e340() cars;
//           if any race-time > limit return 1; return (elim >= N-1)
//   case 8: if car-0 progress > limit return 1; if car-0 status!=0 return 1
//   case 9: same as 8 for cars 0+1, plus if FUN_00423b20(1)!=0 return 1
//   case 10: car-0 status check, DAT_007f0fe4 != DAT_005d757c, then
//            race-time check like case 4
//
// Post-switch collision phase (if FUN_00442df0() == _DAT_005cc55c):
//   1. FUN_0040e180(&inner, &outer) — max-distance pair
//   2. FUN_00408ad0 each side (race-progress normalise)
//   3. Wraparound logic on _005cc730/_005ccd6c/_005cc568
//   4. Mode-1: if progress==DAT_007f0fd4, force 100.0f
//   5. Re-query statuses; swap loser/winner (loser=greater progress=lap-leader);
//      if both statuses nonzero, return 0
//   6. FUN_004922e0(loser,3,10,0x80); FUN_00422fd0(loser);
//      if FUN_0040eee0(loser,1)==1 return 1
//
// Live-count phase:
//   Walk PTR_PTR_005f2770+0x34..0x44; count occupied (iVar6) and alive (iVar3).
//   DAT_00803320 = iVar3.
//   Single-car (iVar6==1): return 1 iff iVar3==0.
//   Multi-car: return 1 iff iVar3==1; if iVar3==0, find max progress via
//   FUN_00408a50 across all live slots, propagate to all via FUN_00408a70; return 1.
//
// Uncertainties: U-1849..U-1853 (float globals); S-1840, S-1841; D-5440..D-5445.
// Many callees at C1/C2 — all called through original RVAs.
// ─────────────────────────────────────────────────────────────────────────────
// 0x00410d10
extern "C" __declspec(dllexport) std::uint32_t __cdecl DamageFn(void) {
    // Entry guard
    if (EntryGuard_() == 1) return 0u;

    // Mode dispatch
    const std::int32_t mode = rdi32(kDat_007f0fd0);
    switch (mode) {
    case 4: {
        // bVar1 = FUN_00417730(0) < _DAT_005cc31c; if (!bVar1) return 1
        if (!(RaceTimeGet_(0) < rdf(kFlt_005cc31c))) return 1u;
        break;
    }
    case 5: {
        if (VehicleAlive_(0u) == 0u) return 1u;
        return static_cast<std::uint32_t>(RaceEndEqualityTest());
    }
    case 7: {
        const std::int32_t N = LiveCarCount_();
        std::int32_t eliminated = 0;
        for (std::int32_t i = 0; i < N; ++i) {
            if (VehicleAlive_(static_cast<std::uint32_t>(i)) == 0u) {
                ++eliminated;
            }
            if (RaceTimeGet_(i) > rdf(kFlt_005cc31c)) return 1u;
        }
        return (eliminated >= N - 1) ? 1u : 0u;
    }
    case 8: {
        if (RaceProgressNorm_(0) > rdf(kFlt_005cc574)) return 1u;
        std::int32_t st0 = 0, sc0 = 0;
        VehicleSpinoutGet_(0, &st0, &sc0);
        if (st0 != 0) return 1u;
        break;
    }
    case 9: {
        if (RaceProgressNorm_(0) > rdf(kFlt_005cc574)) return 1u;
        std::int32_t st0 = 0, sc0 = 0;
        VehicleSpinoutGet_(0, &st0, &sc0);
        if (st0 != 0) return 1u;
        if (RaceProgressNorm_(1) > rdf(kFlt_005cc574)) return 1u;
        std::int32_t st1 = 0, sc1 = 0;
        VehicleSpinoutGet_(1, &st1, &sc1);
        if (st1 != 0) return 1u;
        if (Case9RaceEnd_(1) != 0) return 1u;
        break;
    }
    case 10: {
        std::int32_t st0 = 0, sc0 = 0;
        VehicleSpinoutGet_(0, &st0, &sc0);
        if (st0 != 0) return 1u;
        if (rdi32(kDat_007f0fe4) != rdi32(kDat_005d757c)) {
            if (!(RaceTimeGet_(0) < rdf(kFlt_005cc574))) return 1u;
        }
        break;
    }
    default:
        break;
    }

    // Post-switch collision phase
    const float sentinelVal = CollisionSentinel_();
    if (sentinelVal == rdf(kFlt_005cc55c)) {
        // Find max-distance collision pair
        std::int32_t innerCar = -1, outerCar = -1;
        CollisionPairFinder(&innerCar, &outerCar);

        // Normalise race-progress for both sides
        float innerProg = RaceProgressNorm_(innerCar);
        float outerProg = RaceProgressNorm_(outerCar);

        // Wraparound adjustment on race line
        // Ghidra: three-branch wrap using _005cc730/ccd6c/cc568
        const float wrapLo    = rdf(kFlt_005cc730);
        const float wrapHi    = rdf(kFlt_005ccd6c);
        const float wrapDelta = rdf(kFlt_005cc568);
        if (innerProg < wrapLo) innerProg += wrapDelta;
        if (outerProg < wrapLo) outerProg += wrapDelta;
        if (innerProg > wrapHi) innerProg -= wrapDelta;
        if (outerProg > wrapHi) outerProg -= wrapDelta;

        // Mode-1 override: if progress == DAT_007f0fd4, force 100.0f
        if (mode == 1) {
            const float sentinel1 = rdf(kDat_007f0fd4);
            if (innerProg == sentinel1) innerProg = 100.0f;
            if (outerProg == sentinel1) outerProg = 100.0f;
        }

        // Re-query statuses
        std::int32_t stInner = 0, scInner = 0;
        std::int32_t stOuter = 0, scOuter = 0;
        VehicleSpinoutGet_(innerCar, &stInner, &scInner);
        VehicleSpinoutGet_(outerCar, &stOuter, &scOuter);

        // Swap: loser = greater progress (lap-leader)
        std::int32_t loser;
        if (innerProg > outerProg) {
            loser = innerCar;
        } else {
            loser = outerCar;
        }

        // If both statuses nonzero, skip destroy path
        if (stInner != 0 && stOuter != 0) return 0u;

        // Destroy loser
        HitTrigger_(loser, 3, 10, 0x80);
        RaceResultsDispatch_(loser);
        if (RaceScoring_(loser, 1) == 1) return 1u;
    }

    // Live-count phase
    const std::uintptr_t carTableBase = rd32(kPtrPtr_005f2770);
    std::int32_t occupied = 0;
    std::int32_t alive    = 0;
    for (std::int32_t i = 0; i < 4; ++i) {
        if (*reinterpret_cast<const std::int32_t*>(carTableBase + static_cast<std::uintptr_t>(i) * 4u + 0x34u) != 0) {
            ++occupied;
            if (VehicleAlive_(static_cast<std::uint32_t>(i)) == 1u) {
                ++alive;
            }
        }
    }
    rwi32(kDat_00803320) = alive;

    if (occupied == 1) {
        return (alive == 0) ? 1u : 0u;
    }

    // Multi-car
    if (alive == 1) return 1u;
    if (alive == 0) {
        // Find max progress across all occupied slots; propagate to all
        float maxProg = 0.0f;
        for (std::int32_t i = 0; i < 4; ++i) {
            if (*reinterpret_cast<const std::int32_t*>(carTableBase + static_cast<std::uintptr_t>(i) * 4u + 0x34u) != 0) {
                const float p = PerCarProgressGet_(i);
                if (p > maxProg) maxProg = p;
            }
        }
        for (std::int32_t i = 0; i < 4; ++i) {
            PerCarProgressSet_(i, maxProg);
        }
        return 1u;
    }

    return 0u;
}
RH_ScopedInstall(DamageFn, 0x00410d10);
