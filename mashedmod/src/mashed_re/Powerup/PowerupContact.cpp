// Powerup/PowerupContact.cpp — see PowerupContact.h for the port boundary.
//
// Every constant and branch cited here was read from original/MASHED.exe.unpatched
// (the pinned anchor) with re/tools/disasm_va.py, or decompiled from a read-only
// Ghidra pool clone with re/tools/decomp_pc.py. NO-GUESSING: nothing below is
// inferred from behaviour.
#include "PowerupContact.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Powerup {
namespace Contact {

namespace {

TriSourceFn   s_tri     = nullptr;
void*         s_triCtx  = nullptr;
int           s_triN    = 0;
QueryInjectFn s_qInject = nullptr;
GateInjectFn  s_gInject = nullptr;
void*         s_injCtx  = nullptr;
SweepInjectFn s_sInject = nullptr;
void*         s_swpCtx  = nullptr;

// The original slot array, only so the port's .pucontact.csv carries the same
// arg2 the capture does: 0x0088fbe0 + slot*0xb4 + 0x80. pu_contact_report.py's
// slot_of() decodes exactly that (SLOT_BASE / SLOT_STRIDE / SWEEP_ARG2_DELTA),
// so a standalone dump slot-attributes through the identical rule.
const std::uint32_t kSlotBase   = 0x0088fbe0u;
const std::uint32_t kSlotStride = 0xb4u;
const std::uint32_t kSweepDelta = 0x80u;

std::uint32_t s_frame = 0, s_call = 0;
std::FILE*    s_dump = nullptr;
bool          s_dumpInit = false;

std::FILE* DumpFile() {
    if (!s_dumpInit) {
        s_dumpInit = true;
        const char* p = std::getenv("MASHED_PU_CONTACTDUMP");
        if (p && *p) {
            s_dump = std::fopen(p, "w");
            // Exact header of scenario_launch.py's `<out>.pucontact.csv` so that
            // re/tools/pu_contact_report.py reads both sides identically.
            if (s_dump) std::fprintf(s_dump, "frame,call,rva,name,ret_addr,a1,a2,a3,ret\n");
        }
    }
    return s_dump;
}

void Log(std::uint32_t rva, const char* name, std::uint32_t ra,
         std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, int ret) {
    std::FILE* f = DumpFile();
    if (!f) return;
    std::fprintf(f, "%u,%u,0x%x,%s,0x%x,0x%x,0x%x,0x%x,%d\n",
                 s_frame, s_call, rva, name, ra, a1, a2, a3, ret);
}

// FUN_004c39b0 (RwV3dNormalize). The original scales by an inverse-sqrt read
// from the RW LUT at DAT_007d3ff8/DAT_007d3ffc and, when the squared length is
// exactly 0, uses scale 0 — so the output is the zero vector, not a NaN
// (0x004c39d5 `fVar2 = 0.0` / the `param_2 != 0` guard). Reproduced with the
// CPU sqrt: the standalone has no live RW LUT on this path, and the three
// reported counts this module is measured on do not read the result.
void Normalize3(float* dst, const float* src) {
    const float l2 = src[0]*src[0] + src[1]*src[1] + src[2]*src[2];
    const float s  = (l2 != 0.0f) ? 1.0f / std::sqrt(l2) : 0.0f;
    dst[0] = src[0] * s; dst[1] = src[1] * s; dst[2] = src[2] * s;
}

// Segment/triangle intersection, the stand-in for one FUN_00538c80 candidate.
// Returns 1 and the parameter t in [0,1] along A->B when the segment crosses
// the triangle. Möller-Trumbore; the original's BSP leaf test is not ported.
int SegTri(const float* A, const float* B, const float* v, float* tOut) {
    const float d[3] = { B[0]-A[0], B[1]-A[1], B[2]-A[2] };
    const float e1[3] = { v[3]-v[0], v[4]-v[1], v[5]-v[2] };
    const float e2[3] = { v[6]-v[0], v[7]-v[1], v[8]-v[2] };
    const float p[3] = { d[1]*e2[2]-d[2]*e2[1], d[2]*e2[0]-d[0]*e2[2], d[0]*e2[1]-d[1]*e2[0] };
    const float det = e1[0]*p[0] + e1[1]*p[1] + e1[2]*p[2];
    if (det > -1e-12f && det < 1e-12f) return 0;
    const float inv = 1.0f / det;
    const float s[3] = { A[0]-v[0], A[1]-v[1], A[2]-v[2] };
    const float u = (s[0]*p[0] + s[1]*p[1] + s[2]*p[2]) * inv;
    if (u < 0.0f || u > 1.0f) return 0;
    const float q[3] = { s[1]*e1[2]-s[2]*e1[1], s[2]*e1[0]-s[0]*e1[2], s[0]*e1[1]-s[1]*e1[0] };
    const float w = (d[0]*q[0] + d[1]*q[1] + d[2]*q[2]) * inv;
    if (w < 0.0f || u + w > 1.0f) return 0;
    const float t = (e2[0]*q[0] + e2[1]*q[1] + e2[2]*q[2]) * inv;
    if (t < 0.0f || t > 1.0f) return 0;
    *tOut = t;
    return 1;
}

}  // namespace

void SetTriSource(TriSourceFn fn, void* ctx, int count) {
    s_tri = fn; s_triCtx = ctx; s_triN = count;
}
int  HaveTriSource() { return (s_tri && s_triN > 0) ? 1 : 0; }

void SetInjectors(QueryInjectFn q, GateInjectFn g, void* ctx) {
    s_qInject = q; s_gInject = g; s_injCtx = ctx;
}
void SetSweepInjector(SweepInjectFn s, void* ctx) { s_sInject = s; s_swpCtx = ctx; }

void SetCallCounter(std::uint32_t frame, std::uint32_t call) { s_frame = frame; s_call = call; }

void CloseDump() { if (s_dump) { std::fclose(s_dump); s_dump = nullptr; } s_dumpInit = true; }

// 0x004b4cd0 — `local_4 = 1; copy 6 dwords param_2 -> local_1c; FUN_004b4c80(param_1,
// local_1c, param_3)`. FUN_004b4c80 (0x004b4c80) sets `local_14 = 0` and runs
// FUN_00538c80(world, seg, FUN_004b4bb0, &local_10), then `return local_14`.
// The collector FUN_004b4bb0 (0x004b4bb0):
//   - on the first hit (`*piVar1 == 0`) it stores t; afterwards it SKIPS the write
//     when `bestT <= t` (LAB_004b4c68) — i.e. the buffer keeps the NEAREST hit,
//   - it increments the count on EVERY candidate hit (`*piVar1 = *piVar1 + 1`),
//   - it writes puVar2[0..0xf]: normal, tri index, 3 vertices, object, t, userdata.
// So the return value is the intersection COUNT, and a nonzero return is what both
// drop gates test (OIL `TEST EAX,EAX` 0x004578d4, P_MINE 0x00457ca8).
int SegmentQuery(const float seg[6], WorldHit* out, std::uint32_t retAddr,
                 std::uint32_t rva, const char* name) {
    std::memset(out, 0, sizeof(*out));
    out->normal[1] = 1.0f;
    int n = 0;
    if (s_qInject) {
        n = s_qInject(s_injCtx, seg, out, retAddr);
    } else if (s_tri && s_triN > 0) {
        float bestT = 0.0f;
        for (int i = 0; i < s_triN; ++i) {
            float v[9]; std::uint32_t mk = 0;
            if (!s_tri(s_triCtx, i, v, &mk)) break;
            float t = 0.0f;
            if (!SegTri(seg, seg + 3, v, &t)) continue;
            if (n == 0 || t < bestT) {          // FUN_004b4bb0 nearest-keeps rule
                bestT = t;
                out->t = t;
                out->triIndex = i;
                out->matKey = mk;
                std::memcpy(out->vert, v, sizeof(float) * 9);
                // The original copies the intersection descriptor's normal
                // (param_3[0..2]); the standalone has no descriptor, so this is
                // the triangle's plane normal oriented against the segment (the
                // orientation the two readers rely on: both add normal*k to lift
                // the decal off the surface — OIL 0x00457936 *0.04,
                // P_MINE 0x00457d10-ish *0.005).
                const float e1[3] = { v[3]-v[0], v[4]-v[1], v[5]-v[2] };
                const float e2[3] = { v[6]-v[0], v[7]-v[1], v[8]-v[2] };
                float nr[3] = { e1[1]*e2[2]-e1[2]*e2[1],
                                e1[2]*e2[0]-e1[0]*e2[2],
                                e1[0]*e2[1]-e1[1]*e2[0] };
                const float dir[3] = { seg[3]-seg[0], seg[4]-seg[1], seg[5]-seg[2] };
                if (nr[0]*dir[0] + nr[1]*dir[1] + nr[2]*dir[2] > 0.0f) {
                    nr[0] = -nr[0]; nr[1] = -nr[1]; nr[2] = -nr[2];
                }
                Normalize3(out->normal, nr);
            }
            ++n;
        }
    }
    Log(rva, name, retAddr, 0, 0, 0, n);
    return n;
}

// 0x004b4650 — VERBATIM. `*param_1 = *param_3 - *param_2` … then
// `*param_1 = param_4 * fVar1 + *param_2` per component (0x004b4650..0x004b46aa).
// Already C3 as `Lerp4b4650` (Util/PromoLoop_sessionB.cpp:1429, GREEN r130); this
// is the same expression in the powerup TU so the contact chain does not depend on
// a hook being installed.
void Vec3Lerp(float out[3], const float a[3], const float b[3], float t,
              std::uint32_t retAddr) {
    const float d0 = b[0] - a[0], d1 = b[1] - a[1], d2 = b[2] - a[2];
    out[0] = t * d0 + a[0];
    out[1] = t * d1 + a[1];
    out[2] = t * d2 + a[2];
    Log(0x004b4650, "query_4b4650", retAddr, 0, 0, 0, 1);
}

// 0x004b5080 — VERBATIM structure (decomp_pc 0x004b5080, 260 bytes):
//   A = v[0..2] - v[3..5]                       (local_24/20/1c)
//   B = v[6..8] - v[3..5]                       (local_18/14/10)
//   FUN_004c39b0(m,     &A)        -> m[0..2]  = normalize(A)        (right)
//   C = cross(A, B)                             (local_c/8/4, the three FMULs at
//                                                0x004b50f6..0x004b5126)
//   FUN_004c39b0(m + 8, &C)        -> m[8..10] = normalize(C)        (at)
//   m[4] = m[9]*m[2]  - m[10]*m[1]
//   m[5] = m[10]*m[0] - m[2]*m[8]
//   m[6] = m[8]*m[1]  - m[9]*m[0]                                    (up = at x right)
//   m[12] = m[13] = m[14] = 0
//   FUN_004c45f0(m)                -> *(uint*)(m+0xc) &= 0xfffdfffc  (flags word)
//   if (param_3) FUN_004c51a0(m, param_3, 2)  -> combine 2 = ADD into m[12..14]
//                                                (0x004c5288..0x004c52a4)
void BasisFromTri(float m[16], const float v9[9], const float* pos,
                  std::uint32_t retAddr) {
    const float A[3] = { v9[0]-v9[3], v9[1]-v9[4], v9[2]-v9[5] };
    const float B[3] = { v9[6]-v9[3], v9[7]-v9[4], v9[8]-v9[5] };
    Normalize3(m, A);
    const float C[3] = { B[2]*A[1] - B[1]*A[2],
                         B[0]*A[2] - B[2]*A[0],
                         B[1]*A[0] - B[0]*A[1] };
    Normalize3(m + 8, C);
    m[4] = m[9]*m[2]  - m[10]*m[1];
    m[5] = m[10]*m[0] - m[2]*m[8];
    m[6] = m[8]*m[1]  - m[9]*m[0];
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f;
    std::uint32_t flags;
    std::memcpy(&flags, &m[3], 4);
    flags &= 0xfffdfffcu;                              // FUN_004c45f0
    std::memcpy(&m[3], &flags, 4);
    if (pos) {                                         // FUN_004c51a0(m, pos, 2)
        m[12] += pos[0]; m[13] += pos[1]; m[14] += pos[2];
        std::memcpy(&flags, &m[3], 4);
        flags &= 0xfffdffffu;
        std::memcpy(&m[3], &flags, 4);
    }
    Log(0x004b5080, "query_4b5080", retAddr, 0, 0, 0, 1);
}

// 0x0045c110 — `uVar2 = *(uint *)(param_1 + 4)` (the hit triangle's RpMaterial
// RwRGBA at +4); `if ((uVar2 == 0xff010101) || (uVar2 == 0xffff0080)) return 1`
// (0x0045c116..0x0045c129). Every other arm of the compare tree either calls
// FUN_00472550 for a scripted-gate colour or falls through to `return 0`.
// STAND-IN: the standalone's collision triangles carry no material colour, so
// matKey is whatever TriSource reports (0 today) and only the two literal
// refusals are reproduced. MEASURED against the original, this is the behaviour
// on the two power-up sites: verify/d3_contact_20260928/g2 records
// `surface_gate 0x45c110 from 0x457cf9  calls=3  ret!=0=0` and c3 records 10
// allowed OIL drops.
int SurfaceGate(const WorldHit* hit, std::uint32_t retAddr) {
    int r;
    if (s_gInject) {
        r = s_gInject(s_injCtx, hit, retAddr);
    } else {
        const std::uint32_t k = hit->matKey;
        r = (k == 0xff010101u || k == 0xffff0080u) ? 1 : 0;
    }
    Log(0x0045c110, "surface_gate", retAddr, 0, 0, 0, r);
    return r;
}

// 0x004b4b60 at the dispatcher's only call site 0x0045bcd3. FUN_004b4b60 copies
// FOUR dwords from arg2 (0x004b4b66..0x004b4b7a), writes tag 3 (`local_4 = 3`,
// vs the segment query's tag 1) and tails FUN_004b4a80(world, &copy, arg3, 1),
// which runs FUN_00538c80 with collector FUN_004b49b0 and returns the count.
// Tag 3 + 4 dwords = a sphere: centre = slot+0x80..0x88 (the dispatcher refreshes
// it from EBX every pass, 0x0045bcb2..0x0045bcca) + slot+0x8c.
//
// NOT PORTED, and it cannot be faked: slot+0x8c (the radius) has no writer in the
// ported lifecycle, and the confirm leaf below keys on the same material channel
// the standalone's collision triangles do not carry. With no injector installed
// this returns 0, i.e. the branch is inert — which is what the standalone did
// before this function existed. MEASURED original rate: 1 hit in 3795 slot passes
// (c2 1/1016, c3 0/1348, verify/d3_contact_20260928/g2 0/1431).
int SweepQuery(int slot, const float pos[3], WorldHit* out, std::uint32_t retAddr) {
    std::memset(out, 0, sizeof(*out));
    out->normal[1] = 1.0f;
    int n = 0;
    if (s_sInject) n = s_sInject(s_swpCtx, slot, 0);
    (void)pos;
    Log(0x004b4b60, "sweep_query", retAddr, 0,
        kSlotBase + static_cast<std::uint32_t>(slot) * kSlotStride + kSweepDelta, 0, n);
    return n;
}

// 0x0045c350 at 0x0045bce5. hooks.csv records its body as the same material walk
// the drop gates use (`*(*(*(+0x3c)+0x10)+idx*4)` then FUN_0045c110 early-exit,
// then a FUN_00426c00 track dispatch), returning 0/1. Zero condemns the power-up.
int SweepConfirm(int slot, const WorldHit* hit, std::uint32_t retAddr) {
    int r = 1;                       // no material channel -> "keep", the safe arm
    if (s_sInject) r = s_sInject(s_swpCtx, slot, 1);
    (void)hit;
    Log(0x0045c350, "sweep_confirm", retAddr, 0,
        kSlotBase + static_cast<std::uint32_t>(slot) * kSlotStride + kSweepDelta, 0, r);
    return r;
}

// 0x0045c350 again, but at MORTAR's call site 0x004537b6 (RA 0x004537bb) rather
// than the dispatcher's. Same leaf, DIFFERENT attribution: the dispatcher's sweep
// is keyed by slot through arg2, while a mortar in flight outlives its slot, so
// this one has to be keyed by call site like the drop gates are.
//
// Polarity, from FUN_00453730's own branch: `CALL 0x45c350` @0x004537b6,
// `TEST EAX,EAX`, and a NON-zero falls through to `return 0` -- i.e. non-zero
// REFUSES the detonation and the projectile keeps flying. Measured on
// verify/d3_contact_20260928b/m2.msd: of the 2 hits at 0x453789, one gate
// returned 0 (detonated) and one returned 1 (refused).
int ConfirmGateAt(const WorldHit* hit, std::uint32_t retAddr) {
    int r = 0;                       // no material channel -> "allow", matching
    if (s_gInject) r = s_gInject(s_injCtx, hit, retAddr);
    Log(0x0045c350, "sweep_confirm", retAddr, 0, 0, 0, r);
    return r;
}

}  // namespace Contact
}  // namespace Powerup
}  // namespace mashed_re
