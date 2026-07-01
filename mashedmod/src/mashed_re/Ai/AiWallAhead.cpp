// Mashed RE — WS-R6-D: AI wall-ahead trajectory check (FUN_00415d00) verbatim port.
//
// Target FUN_00415d00 (0x00415d00..0x00415e19, 281 bytes), __cdecl 1 arg:
//   int WallAhead(int vehIdx)
// Extrapolates the vehicle 2x its world velocity forward, then ray-marches (step 0.25
// in length units) from the vehicle position along that direction, sampling the track
// tile grid via FUN_00443d10. Returns 1 the moment a wall tile (surface byte 0 or 3)
// lies on the forward path, else 0. PURE: reads pos/velocity getters + the static tile
// grid, no side effects. Used by FUN_00416250 to SUPPRESS the mode-2 ram (don't chase a
// target through a wall). Draining it de-risks the orchestrator port
// (see R6_FUN_00416250_SCOPE_20260701.md).
//
// AUTHORED FROM RAW ASM (Mashed_pool13 RO, 2026-07-01). Structure mirrors AiLineOfSight
// (0x00416060): the direction is built as `(2*vel + pos) - pos` (kept in one x87 ST reg,
// rounded to f32 once at the store — the `-pos` is NOT a no-op at f32), its length comes
// from FUN_004c3bf0 (Vec2Length, float10-in-ST0 → declared float so the SSE2 .asi pops
// ST0; see feedback_x87_st0_float10_return_fnptr), and the per-step sample
// `((invlen*dir)*t)+pos` is computed in `double` to emulate x87 single-rounding (the
// march t steps by exact 0.25 so plain float is bit-exact for it). The tile lookup
// FUN_00443d10 is FORWARDED to the live original (not re-inlined): it uses the OPPOSITE
// X/Z-row grid convention from LosWallAt (arg1=row, arg2=col) and returns the surface
// byte (0xff out of range) — forwarding sidesteps replicating that convention. All
// callees are __cdecl caller-cleans (proven by the caller's batched ADD ESP,0x14 over
// the 5 pushed dwords: 0046d4a0(2) + 0046d510(2) + 004c3bf0(1)). Anchored to MASHED.exe
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float kZero = Cf(0x00000000);  // DAT_005d757c  0.0
const float kOne  = Cf(0x3f800000);  // _DAT_005cc320 1.0
const float k0_25 = Cf(0x3e800000);  // _DAT_005cc564 ray-march step

// ── live callee forwards (all present; forwarded so mine and orig call the identical
// originals; with MASHED_HOOK_ONLY=0x00415d00 no other hook installs) ──
// FUN_0046d4a0 (PtrCompute881ec8, C3): writes the vehicle-record ptr to *out.
// FUN_0046d510 (VehicleVelocityWorldGet, C3): writes the world-velocity float3 to out.
// FUN_004c3bf0 (Vec2Length, C4): RETURNS float10 IN ST0 → declared float (pop ST0).
// FUN_00443d10 (tile-type getter, C2): __cdecl(x,z) → surface byte (0xff out of range).
typedef void (__cdecl* fn_rec_t)  (void** out, int idx);
typedef void (__cdecl* fn_vel_t)  (float* out3, int idx);
typedef float(__cdecl* fn_len_t)  (float*);
typedef unsigned char (__cdecl* fn_tile_t)(float, float);
inline void  VehRecPtr(void** o, int i) { reinterpret_cast<fn_rec_t>(0x0046d4a0)(o, i); }
inline void  VehVelWorld(float* o, int i){ reinterpret_cast<fn_vel_t>(0x0046d510)(o, i); }
inline float Vec2Len(float* v)          { return reinterpret_cast<fn_len_t>(0x004c3bf0)(v); }
inline unsigned char TileType(float x, float z) { return reinterpret_cast<fn_tile_t>(0x00443d10)(x, z); }

// ===========================================================================
// WallAhead — greenfield reimpl of FUN_00415d00. Returns 1 (wall ahead) / 0 (clear).
// ===========================================================================
int WallAhead(int param_1) {
    void* rec = nullptr;
    VehRecPtr(&rec, param_1);
    std::uintptr_t r = reinterpret_cast<std::uintptr_t>(rec);
    float posX = *reinterpret_cast<float*>(r + 0x30);
    float posZ = *reinterpret_cast<float*>(r + 0x38);

    float vel[3];                                    // [0]=X [1]=Y [2]=Z (world), then doubled
    VehVelWorld(vel, param_1);
    float vX2 = vel[0] + vel[0];                     // 2*velX (exact power-of-2)
    float vZ2 = vel[2] + vel[2];                     // 2*velZ

    float dir[2];                                    // {dirX, dirZ} — contiguous (Vec2Length reads both)
    dir[0] = static_cast<float>((static_cast<double>(vX2) + posX) - posX);   // (2velX+posX)-posX @ f32
    dir[1] = static_cast<float>((static_cast<double>(vZ2) + posZ) - posZ);   // (2velZ+posZ)-posZ @ f32
    float len = Vec2Len(dir);
    if (kZero < len) {
        float invlen = static_cast<float>(kOne / static_cast<double>(len));  // FLD 1.0 / FDIV len -> f32
        float t = kZero;
        do {
            float sx = static_cast<float>((static_cast<double>(invlen) * dir[0]) * t + posX);
            float sz = static_cast<float>((static_cast<double>(invlen) * dir[1]) * t + posZ);
            unsigned char tile = TileType(sx, sz);
            if (tile == 0 || tile == 3) return 1;    // wall on the forward path
            t = t + k0_25;
        } while (t < len);
    }
    return 0;
}

// ── Orig trampoline: the 5-byte E9-rel32 hook clobbers 0x00415d00..0x00415d04 — SUB
// ESP,0x34 (3B) + PUSH ESI (1B) + the first byte of MOV ESI,[ESP+0x3c] (which begins at
// 0x00415d04). Re-exec all three then jump to the first INTACT instruction 0x00415d08
// (LEA EAX,[ESP+0x10]). The SUB/PUSH reproduce the exact frame the body expects; after
// SUB(0x34)+PUSH ESI, [ESP+0x3c] = the forwarded vehIdx (E+4). ──
void* g_orig_415d08 = reinterpret_cast<void*>(0x00415d08);
__declspec(naked) int OrigWallAhead(int) {
    __asm {
        sub  esp, 0x34
        push esi
        mov  esi, dword ptr [esp + 0x3c]
        jmp  dword ptr [g_orig_415d08]
    }
}

// ── in-process A/B self-test (env MASHED_AI_WALLAHEAD_SELFTEST) ──────────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_WALLAHEAD_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_wallahead_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_mismatch = 0;
const long kMaxCompare = 40000;

// Pure fn -> the A/B compares the 0/1 return. Return ORIG's value so game behavior is
// unchanged during measurement; SAFE PASSTHROUGH when not self-testing.
int WallAheadDispatch(int vehIdx) {
    if (SelfTestEnabled() && g_calls < kMaxCompare) {
        int o = OrigWallAhead(vehIdx);
        int m = WallAhead(vehIdx);
        ++g_calls;
        if (o != m) {
            ++g_mismatch;
            char line[128];
            wsprintfA(line, "[%ld] MISMATCH o=%d m=%d  veh=%d\r\n", g_calls, o, m, vehIdx);
            SelfTestLog(line);
        }
        if ((g_calls & 0x7f) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_calls, g_calls, g_mismatch,
                      g_mismatch ? "" : "ALL-GREEN");
            SelfTestLog(line);
        }
        return o;
    }
    return OrigWallAhead(vehIdx);
}

// ── naked entry installed at 0x00415d00 (forwards the 1 __cdecl stack arg) ──
__declspec(naked) void AiWallAhead_Entry() {
    __asm {
        // [esp]=ret [esp+4]=vehIdx
        push dword ptr [esp + 4]    // vehIdx
        call WallAheadDispatch
        add  esp, 4                 // cdecl clean our 1 forwarded arg
        ret                         // original is caller-cleans; EAX = 0/1 result preserved
    }
}

// COVERAGE NOTE (WS-R6-D): FUN_00415d00 is only reached on the orchestrator's mode-2
// path (ram-from-behind LATCHED and its LOS clear), which the higher-priority mode-1
// (ahead-targeting) preempts in any pack race — so a natural race never calls it
// (measured: 0 calls over 25/40 s). It was verified via a TEMPORARY driver hooked at the
// orchestrator entry FUN_00416250 that drove this A/B for every AI vehicle every frame
// (WallAhead is a pure function of vehIdx, so the extra calls are inert): 3585 calls, 0
// mismatch, ALL-GREEN. That driver was removed after verification to keep 0x00416250 free
// for the orchestrator port; re-add it (hook 0x00416250: pushad; drive WallAheadDispatch
// with param_2 at [esp+0x28]; popad; re-exec PUSH EBP/MOV EBP,ESP/AND ESP,-8; jmp
// 0x00416256) if this leaf needs re-measuring before the orchestrator lands.

}  // namespace

RH_ScopedInstall(AiWallAhead_Entry, 0x00415d00);
