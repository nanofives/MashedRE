// Mashed RE — WS-B3: car<->car convex-hull contact test + impulse response.
//
// VERBATIM port of FUN_00469df0 (0x00469df0..0x0046b1b6). Anchored to MASHED.exe
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Decompiled this session from Ghidra pool3 (read_only). Map + evidence:
//   re/analysis/WSB2_B3_CONTACT_PORT_MAP.md
//
// Original: bool __thiscall FUN_00469df0(int* param_1, undefined4, int param_3)
//   in_EAX  = EAX  -> vehA
//   param_1 = ECX  -> vehB
//   param_3 = substep pass index              -> pass   (returns pass == 0)
//   param_2 = the j index; UNUSED (it occurs exactly once in the whole
//             decompilation, in the declaration line), so the port drops it.
// Called from FUN_004709a0 only when game-mode in {6,7,10,0xb} and the two car
// centroids are within (radiusA+radiusB)*0.75 (_DAT_005cc950).
//
// CALL SITE AND ARGUMENT ORDER — CORRECTED 2026-10-05. This header used to say
// `in_EAX = vehicle A ("this", register)`. **in_EAX is NOT the `__thiscall` this; ECX
// is**, read from the only call site (`0x00470bcd`, the sole caller FUN_004709a0,
// confirmed by both `decomp_pc.py --callers` and `--datarefs`):
//     0x00470bc5  lea eax, [edi + 0x8815a0]   ; EAX = &records[j]  (edi = j*0xd04)
//     0x00470bcb  mov ecx, esi                ; ECX = &records[i]  (the SELF car)
//     0x00470bcd  call 0x469df0
// So `param_1`/ECX is the SELF car and `in_EAX` is the OTHER car. The parameter MAPPING
// below was always right — `vehA` is `in_EAX`, `vehB` is `param_1` — only the word
// "this" was misplaced. Consequence for anyone wiring this:
//     the call is VehicleCarCarContact(rec(j), rec(i), pass)  -- OTHER car FIRST.
// Full decode of the five gates and the retry semantics:
// re/analysis/CARCAR_CALLSITE_2026-10-05.md.
//
// FIDELITY NOTES:
//  * Vehicle records are explicit int* (DAT_008815a0 + car*0xd04). vF() = float
//    view of a dword. The decompiler renders float stores into int*-typed fields
//    as `field = (int)(floatexpr)`; the fields ARE floats (velocity/angular), so
//    those are ported as float stores via vF() — NOT numeric int truncation.
//  * x87 float10 intermediates -> double (width-preserving; bit-identity vs the
//    original FPU 80-bit path is a recorded residual, [[project-wsa2-rwmath-bitident]]).
//  * The debug-event ring enqueue (type 4, gated by game-mode + contact-count
//    asymmetry) is GAMEPLAY-INERT and omitted; RVA cited at the site.
//  * C-LEVEL: C2 faithful transcription; gate = installed-hook scenario trace.
#include "ContactConstants.h"
#include "ContactDeps.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "../Vehicle/VehicleRecordBase.h"

namespace mashed_re {
namespace Collision {

// [d3bB2 2026-10-17] 1 when the last call found a contact (iVar15 != 0), i.e. reached the type-4 ring
// push at its tail (0x0046ae0f.. / 0x0046afc4..). The caller pushes the exe-side event
// (Ai_ContactEventPush, MASHED_AI_CONTACT_EVENT) from it, because the exe's record +0x0 does not hold the
// car index the original's push reads ([edi] / [esi] at 0x0046ae1b / 0x0046ae1d).
int g_ccLastContact = 0;

// [u9769 2026-10-19] MASHED_CC_TRACE=<path> (diagnostic, default OFF): one row per call in the format of
// scenario_launch.py --carcar-probe (tick 0x007f0ff4, self = vehB, other = vehA, pass, both records at entry and at
// return). Slot index = (rec - slot-0 record) / 0xd04. Capped at 64 rows. The dump runs in non-inlined helpers on
// static buffers so the knob-off frame of VehicleCarCarContact stays as it was: its spin tail reads a stub-filled
// stack local (Rw_MatrixDerive), so a change to that frame changes the default build's output.
static std::FILE* s_ccTr = nullptr;
static int s_ccN = 0, s_ccOn = -1;
static std::string s_ccSi, s_ccOi;
static __declspec(noinline) void CcHex(std::string& o, const int* r) {
    static const char* d = "0123456789abcdef";
    const unsigned char* b = reinterpret_cast<const unsigned char*>(r);
    o.clear();
    for (int i = 0; i < 0xd04; ++i) { o += d[b[i] >> 4]; o += d[b[i] & 15]; }
}
static __declspec(noinline) int CcTraceOn() {
    if (s_ccOn < 0) {
        const char* e = std::getenv("MASHED_CC_TRACE");
        s_ccTr = e ? std::fopen(e, "w") : nullptr;
        if (s_ccTr) std::fprintf(s_ccTr, "sdframe\ttick\tself\tother\tpass\tself_in\tother_in\tret\tself_out\tother_out\n");
        s_ccOn = s_ccTr ? 1 : 0;
    }
    return (s_ccOn == 1 && s_ccN < 64) ? 1 : 0;
}
static __declspec(noinline) void CcTraceIn(const int* vehA, const int* vehB) { CcHex(s_ccSi, vehB); CcHex(s_ccOi, vehA); }
static __declspec(noinline) void CcTraceOut(const int* vehA, const int* vehB, int pass, int ret) {
    static std::string so, oo; CcHex(so, vehB); CcHex(oo, vehA);
    const char* r0 = mashed_re::Vehicle::VehRecord(0);
    std::fprintf(s_ccTr, "%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%s\t%s\n", s_ccN, *reinterpret_cast<const int*>(0x007f0ff4u),
                 static_cast<int>((reinterpret_cast<const char*>(vehB) - r0) / 0xd04),
                 static_cast<int>((reinterpret_cast<const char*>(vehA) - r0) / 0xd04), pass,
                 s_ccSi.c_str(), s_ccOi.c_str(), ret, so.c_str(), oo.c_str());
    if (++s_ccN == 64) std::fflush(s_ccTr);
}

// 0x00469df0
bool VehicleCarCarContact(int* vehA, int* vehB, int pass)
{
    if (CcTraceOn()) CcTraceIn(vehA, vehB);
    g_ccLastContact = 0;
    float* slotA = vFP(vehA, vehA[0x26a] * 0x10 + 0x256);   // local_a8 (vehA active slot)
    float* pfVar2 = vFP(vehB, vehB[0x26a] * 0x10 + 0x256);  // vehB active slot
    int iVar15 = 0;
    int local_b0[2] = { 0, 0 };
    float local_11c = kZero, local_120 = kZero, local_124 = kZero;

    // --- Phase 1: two-pass SAT convex-hull vertex test --------------------
    for (unsigned int uVar13 = 0; uVar13 < 2; ++uVar13) {
        int* src = (uVar13 != 0) ? vehB : vehA;
        float local_d8 = vF(src, 0x28a), fVar3 = vF(src, 0x272), fVar4 = vF(src, 0x273);
        float local_e0 = vF(src, 0x28b), fVar5 = vF(src, 0x274), local_cc = vF(src, 0x28c);
        float local_f0 = vF(src, 0x28d), local_ec = vF(src, 0x28e), local_e4 = vF(src, 0x28f);
        float local_dc = vF(src, 0x293), local_d0 = vF(src, 0x294), local_d4 = vF(src, 0x295);
        float local_e8 = vF(src, 0x290), local_f4 = vF(src, 0x291), fVar6 = vF(src, 0x292);

        float local_13c = (local_e4 - local_cc) * fVar4 - (local_ec - local_e0) * fVar5;
        float local_138 = (local_f0 - local_d8) * fVar5 - (local_e4 - local_cc) * fVar3;
        float local_134 = (local_ec - local_e0) * fVar3 - (local_f0 - local_d8) * fVar4;
        float local_130 = (local_d4 - local_e4) * fVar4 - (local_d0 - local_ec) * fVar5;
        float local_12c = (local_dc - local_f0) * fVar5 - (local_d4 - local_e4) * fVar3;
        float local_128 = (local_d0 - local_ec) * fVar3 - (local_dc - local_f0) * fVar4;
        float local_118 = (fVar6 - local_d4) * fVar4 - (local_f4 - local_d0) * fVar5;
        float local_114 = (local_e8 - local_dc) * fVar5 - (fVar6 - local_d4) * fVar3;
        float local_110 = (local_f4 - local_d0) * fVar3 - (local_e8 - local_dc) * fVar4;
        float local_148 = local_e0 - local_f4;
        float local_140 = local_cc - fVar6;
        float fVar9 = local_140 * fVar4 - local_148 * fVar5;
        float fv5  = (local_d8 - local_e8) * fVar5 - local_140 * fVar3;
        float fv3  = local_148 * fVar3 - (local_d8 - local_e8) * fVar4;

        int* other = (uVar13 != 0) ? vehA : vehB;
        // Four hull vertices of `other`: indices 0x28a, 0x28d, 0x290, 0x293.
        // (verbatim quirk: only vertex 1 and 2 increment local_b0[uVar13].)
        for (int vi = 0; vi < 4; ++vi) {
            int base = 0x28a + vi * 3;
            float vx = vF(other, base), vy = vF(other, base + 1), vz = vF(other, base + 2);
            if (((vx - local_d8) * local_13c + (vy - local_e0) * local_138 + local_134 * (vz - local_cc) < kZero) &&
                ((vx - local_f0) * local_130 + (vy - local_ec) * local_12c + local_128 * (vz - local_e4) < kZero) &&
                ((vx - local_dc) * local_118 + (vy - local_d0) * local_114 + local_110 * (vz - local_d4) < kZero) &&
                ((vx - local_e8) * fVar9 + (vy - local_f4) * fv5 + fv3 * (vz - fVar6) < kZero)) {
                local_124 += vx;
                iVar15 += 1;
                local_120 += vy;
                if (vi == 0 || vi == 1) local_b0[uVar13] += 1;
                local_11c += vz;
            }
        }
    }

    if (iVar15 == 0) { if (s_ccOn == 1 && s_ccN < 64) CcTraceOut(vehA, vehB, pass, 0); return false; }
    g_ccLastContact = 1;

    float inv = (float)iVar15;
    if (iVar15 < 0) inv = inv + kCC_UIntFixup;
    inv = kOne / inv;
    local_124 = inv * local_124;   // contact centroid
    local_120 = inv * local_120;
    local_11c = inv * local_11c;

    // vAB = centroid - vehB slot (normalized); vAB0 = unnormalized copy.
    float vAB[3]  = { local_124 - pfVar2[0], local_120 - pfVar2[1], local_11c - pfVar2[2] };
    float vAB0[3] = { vAB[0], vAB[1], vAB[2] };
    Vec3Normalize(vAB, vAB);
    // vB = centroid - vehA slot (normalized); vB0 = unnormalized copy.
    float vB[3]  = { local_124 - slotA[0], local_120 - slotA[1], local_11c - slotA[2] };
    float vB0[3] = { vB[0], vB[1], vB[2] };
    Vec3Normalize(vB, vB);

    // --- Phase 2: impulse (coefficient-of-restitution) --------------------
    float velB[3] = { vF(vehB, 0x51) * kCC_AngToLin + vF(vehB, 0x26c),
                      vF(vehB, 0x52) * kCC_AngToLin + vF(vehB, 0x26d),
                      vF(vehB, 0x53) * kCC_AngToLin + vF(vehB, 0x26e) };
    float velA[3] = { vF(vehA, 0x51) * kCC_AngToLin + vF(vehA, 0x26c),
                      vF(vehA, 0x52) * kCC_AngToLin + vF(vehA, 0x26d),
                      vF(vehA, 0x53) * kCC_AngToLin + vF(vehA, 0x26e) };
    float impB[3] = { 0.0f, 0.0f, 0.0f };   // {local_10c, local_108, local_104} -> vehB
    float impA[3] = { 0.0f, 0.0f, 0.0f };   // {local_100, local_fc,  local_f8 } -> vehA
    float local_114 = 0.0f, local_110 = 0.0f;

    float magB = Vec3Mag(velB);
    float magA = Vec3Mag(velA);
    float dot  = velA[2] * velB[2] + velA[0] * velB[0] + velA[1] * velB[1];

    if (magB != kZero) {
        double f = (double)dot * ((double)kOne / (double)magB);
        if (magA != kZero) f = f / (double)magA;
        f = -(((double)kOne / (double)magB) * (double)magA * f);
        if (f <= (double)kZero) { if (f < (double)kCC_ImpulseLoClamp) f = (double)kCC_ImpulseLoClamp; }
        else f = (double)kZero;
        f = f + (double)kOne;
        local_114 = (float)(f * (double)velB[1]);
        local_110 = (float)(f * (double)velB[2]);
        impA[0] = (float)((double)velB[0] * f);
        impA[1] = (float)(f * (double)velB[1]);
        impA[2] = (float)(f * (double)velB[2]);
        impB[0] = (float)(-((double)velB[0] * f));
        impB[1] = -local_114;
        impB[2] = -local_110;
    }
    if (magA != kZero) {
        double f = (double)dot * ((double)kOne / (double)magA);
        if (magB != kZero) f = f / (double)magB;
        f = -(((double)kOne / (double)magA) * (double)magB * f);
        if (f <= (double)kZero) { if (f < (double)kCC_ImpulseLoClamp) f = (double)kCC_ImpulseLoClamp; }
        else f = (double)kZero;
        f = f + (double)kOne;
        local_114 = (float)(f * (double)velA[1]);
        local_110 = (float)(f * (double)velA[2]);
        impB[0] = (float)((double)impB[0] + (double)velA[0] * f);
        impB[1] = impB[1] + local_114;
        impB[2] = impB[2] + local_110;
        impA[0] = (float)((double)impA[0] - (double)velA[0] * f);
        impA[1] = impA[1] - local_114;
        impA[2] = impA[2] - local_110;
    }

    vF(vehB, 0x26c) = impB[0] * kCC_ImpulseHalf + vF(vehB, 0x26c);
    vF(vehB, 0x26d) = impB[1] * kCC_ImpulseHalf + vF(vehB, 0x26d);
    vF(vehB, 0x26e) = impB[2] * kCC_ImpulseHalf + vF(vehB, 0x26e);
    vF(vehA, 0x26c) = impA[0] * kCC_ImpulseHalf + vF(vehA, 0x26c);
    vF(vehA, 0x26d) = impA[1] * kCC_ImpulseHalf + vF(vehA, 0x26d);
    vF(vehA, 0x26e) = impA[2] * kCC_ImpulseHalf + vF(vehA, 0x26e);

    float local_148 = Vec3Mag(impA) + Vec3Mag(impB);   // |impA| + |impB|

    float fVar3 = (g_playerCount < 3) ? kCC_Mag_LT3players : 0.0f;
    if (g_playerCount == 3 || g_playerCount == 4) fVar3 = kCC_Mag_34players;
    float local_164 = local_148 * kCC_ImpactInScale;
    if (kOne < local_148 * kCC_ImpactInScale) local_164 = kOne;
    local_164 = local_164 * fVar3;
    if (local_164 < kCC_Mag_34players) local_164 = 200.0f;   // floor

    // tvB = normalize(vehB.vel) - vAB ; tvA = normalize(vehA.vel) - vB
    float tvB[3], tvA[3];
    Vec3Normalize(tvB, vFP(vehB, 0x26c));
    Vec3Normalize(tvA, vFP(vehA, 0x26c));
    tvB[0] -= vAB[0]; tvB[1] -= vAB[1]; tvB[2] -= vAB[2];
    tvA[0] -= vB[0];  tvA[1] -= vB[1];  tvA[2] -= vB[2];
    Vec3Normalize(tvB, tvB);
    Vec3Normalize(tvA, tvA);
    float fScale = local_164 * kCC_ImpulseScale;
    vF(vehB, 0x26c) = vF(vehB, 0x26c) - tvB[0] * fScale;
    vF(vehB, 0x26d) = vF(vehB, 0x26d) - tvB[1] * fScale;
    vF(vehB, 0x26e) = vF(vehB, 0x26e) - tvB[2] * fScale;
    vF(vehA, 0x26c) = vF(vehA, 0x26c) - tvA[0] * fScale;
    vF(vehA, 0x26d) = vF(vehA, 0x26d) - tvA[1] * fScale;
    vF(vehA, 0x26e) = vF(vehA, 0x26e) - fScale * tvA[2];

    vF(vehB, 0x26c) = vF(vehB, 0x26c) - vAB[0] * local_164;
    vF(vehB, 0x26d) = vF(vehB, 0x26d) - vAB[1] * local_164;
    vF(vehB, 0x26e) = vF(vehB, 0x26e) - vAB[2] * local_164;
    vF(vehA, 0x26c) = vF(vehA, 0x26c) - vB[0] * local_164;
    vF(vehA, 0x26d) = vF(vehA, 0x26d) - vB[1] * local_164;
    vF(vehA, 0x26e) = vF(vehA, 0x26e) - vB[2] * local_164;

    if (vehB[699] == 0) vehB[699] = kCC_BounceTimer;
    if (vehA[699] == 0) vehA[699] = kCC_BounceTimer;

    unsigned char local_40[64];
    Rw_MatrixDerive(local_40, vehB + vehB[0x26b] * 0x10 + 0x24a);
    Rw_TransformPoints(vAB, vAB0, 1, local_40);   // vAB = transform(vAB0)
    Rw_MatrixDerive(local_40, vehA + vehA[0x26b] * 0x10 + 0x24a);
    Rw_TransformPoints(vB, vB0, 1, local_40);     // vB  = transform(vB0)

    // --- Phase 3/4: spin (angular) impulse --------------------------------
    // orig: fVar3 = -local_164 when (vAB.x>=0 && vAB.z>0) || (vAB.x<0 && vAB.z<0)
    float fVar3b = local_164;
    if ((kZero <= vAB[0]) ? (kZero < vAB[2]) : (vAB[2] < kZero)) fVar3b = -local_164;
    if (vehB[4] == 0) {
        vF(vehB, 0x52) = fVar3b * kCC_SpinScale + vF(vehB, 0x52);
    } else {
        fVar3b = fVar3b * kCC_SpinStopScale;
        vF(vehB, 0x26f) = 0.0f;
        vF(vehB, 0x271) = 0.0f;
        vF(vehB, 0x270) = fVar3b;
    }
    float local_164b = local_164;
    if ((kZero <= vB[0]) ? (kZero < vB[2]) : (vB[2] < kZero)) local_164b = -local_164;
    if (vehA[4] == 0) {
        vF(vehA, 0x52) = local_164b * kCC_SpinScale + vF(vehA, 0x52);
    } else {
        local_164b = local_164b * kCC_SpinStopScale;
        vF(vehA, 0x26f) = 0.0f;
        vF(vehA, 0x271) = 0.0f;
        vF(vehA, 0x270) = local_164b;
    }

    // [0x0046ae0f..0x0046b18b] type-4 event ring push (head 0x007e9de0, type 0x007e9de4, frame stamp
    // 0x007ea1e4, car field 0x007ea5e4, pair 0x007ea9e4 / 0x007ea9e8, flag 0x007ea9ec, vectors 0x007ea9f0..
    // and 0x007ec9e4..). CORRECTED 2026-10-17 (d3bB2): it is NOT dev-viz only and NOT gated by the count
    // asymmetry. Both arms push type 4 whenever FUN_0040e350() is not 1..5 (0x0046ae97..0x0046aec3 and
    // 0x0046b04c..0x0046b078); local_b0[0] != local_b0[1] (0x0046adf9..0x0046ae09) only orders the pair and
    // sets the flag. FUN_00422ba0 drains it after the physics frame and its type-4 arm sets the AI override
    // timer through FUN_00417370 (watched on the original: o_ov1, 0x0041739b). The ring itself is not
    // written here; the exe-side event is pushed by the caller from g_ccLastContact (MASHED_AI_CONTACT_EVENT,
    // Ai/AiStandalone.cpp Ai_ContactEventPush / Ai_ContactEventDrain).
    (void)local_b0;   // consumed only by the ring payload (flag 0x007ea9ec), which no exe reader takes

    if (s_ccOn == 1 && s_ccN < 64) CcTraceOut(vehA, vehB, pass, pass == 0 ? 1 : 0);
    return pass == 0;
}

// [u9769 2026-10-19] MASHED_CC_REPLAY=<carcar.tsv> (scenario_launch.py --carcar-probe), MASHED_CC_REPLAY_OUT=<path>.
// Diagnostic only, runs once at track load: each row's original entry records (self = ECX, other = EAX at
// 0x00470bcd) go through VehicleCarCarContact(other, self, pass) in scratch copies, and every dword that differs
// from the original's leave records is written out. No game state is touched.
void CarCarContact_MaybeReplay() {
    static bool s_done = false;
    if (s_done) return;
    s_done = true;
    const char* in = std::getenv("MASHED_CC_REPLAY");
    const char* outp = std::getenv("MASHED_CC_REPLAY_OUT");
    if (!in || !outp) return;
    std::FILE* f = std::fopen(in, "rb");
    if (!f) return;
    std::string all;
    { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
    std::fclose(f);
    std::FILE* o = std::fopen(outp, "w");
    if (!o) return;
    std::fprintf(o, "row\tsdframe\tself\tother\tpass\tret_o\tret_p\tndiff\tdiffs(car:+off:orig:port)\n");
    auto unhex = [](const std::string& h, std::vector<int>& v) {
        v.assign(0xd04 / 4, 0);
        unsigned char* b = reinterpret_cast<unsigned char*>(v.data());
        for (std::size_t i = 0; i + 1 < h.size() && i / 2 < 0xd04u; i += 2)
            b[i / 2] = static_cast<unsigned char>(std::strtoul(h.substr(i, 2).c_str(), nullptr, 16));
    };
    std::size_t p = all.find('\n') + 1;
    int row = 0;
    while (p < all.size()) {
        std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
        std::string ln = all.substr(p, e - p); if (!ln.empty() && ln.back() == '\r') ln.pop_back();
        p = e + 1;
        std::vector<std::string> c; std::size_t q = 0;
        for (;;) { std::size_t t = ln.find('\t', q); c.push_back(ln.substr(q, t == std::string::npos ? std::string::npos : t - q));
                   if (t == std::string::npos) break; q = t + 1; }
        if (c.size() < 10) continue;
        std::vector<int> si, oi, so, oo;
        unhex(c[5], si); unhex(c[6], oi); unhex(c[8], so); unhex(c[9], oo);
        const int pass = std::atoi(c[4].c_str());
        const int retO = std::atoi(c[7].c_str());
        const int retP = VehicleCarCarContact(oi.data(), si.data(), pass) ? 1 : 0;
        int nd = 0; std::string d;
        for (int k = 0; k < 0xd04 / 4; ++k) {
            for (int w = 0; w < 2; ++w) {
                const int a = w ? oo[k] : so[k], b = w ? oi[k] : si[k];
                if (a == b) continue;
                ++nd;
                if (nd <= 40) {
                    float fa, fb; std::memcpy(&fa, &a, 4); std::memcpy(&fb, &b, 4);
                    char t[96]; std::snprintf(t, sizeof t, "%s%s:+0x%x:%.9g:%.9g", d.empty() ? "" : " ", w ? "other" : "self", k * 4, fa, fb);
                    d += t;
                }
            }
        }
        std::fprintf(o, "%d\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%s\n", row, c[0].c_str(), c[2].c_str(), c[3].c_str(), pass, retO, retP, nd, d.c_str());
        ++row;
    }
    std::fclose(o);
}

}  // namespace Collision
}  // namespace mashed_re
