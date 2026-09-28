// Powerup/PowerupContact.h — the power-up CONTACT chain (D3 criterion (c)).
//
// Ports the four functions every power-up placement in MASHED runs through.
// Every offset and branch below was disassembled from original/MASHED.exe.unpatched
// (SHA-256 BDCAE093...3C0E, the pinned anchor) with re/tools/disasm_va.py, or read
// from a read-only Ghidra pool clone with re/tools/decomp_pc.py.
//
//   0x004b4cd0  SegmentQuery   — copies 6 dwords (a 2-point segment) to a local,
//                                writes tag 1, tails FUN_004b4c80, which runs the
//                                BSP walk FUN_00538c80 with collector FUN_004b4bb0
//                                and RETURNS THE NUMBER OF INTERSECTIONS.
//   0x004b4650  Vec3Lerp       — out = a + t*(b - a), component-wise.
//   0x004b5080  BasisFromTri   — orthonormal RwMatrix from the hit triangle.
//   0x0045c110  SurfaceGate    — reads the hit triangle's RpMaterial RwRGBA and
//                                returns 1 for two marker colours (refuse).
//
// PORT BOUNDARY, stated plainly:
//   - The DECISION STRUCTURE (which call, in what order, with which branch, and
//     what state change each outcome produces) is verbatim.
//   - The BSP walk itself (FUN_00538c80 over COLLI*.BSP world sectors) is NOT
//     ported. SegmentQuery's stand-in walks the standalone's collision triangle
//     soup linearly, through the TriSource hook the host installs. It reproduces
//     the original's RESULT rule (count every intersection, keep the nearest by
//     t) over a possibly different candidate set.
//   - The material channel the two gates key on is not carried by the standalone's
//     collision triangles, so SurfaceGate reads TriSource's matKey, which the host
//     currently reports as 0 (= "allow") for every triangle. That matches the
//     MEASURED original reference for OIL (10 drops, 10 allowed) and is recorded,
//     not assumed — see re/analysis/D3_CONTACT_PORT_2026-09-28.md.
//   This is C2-grade. Do NOT mark C4.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Powerup {
namespace Contact {

// The 0x40-byte result buffer FUN_004b4cd0 fills. Field offsets are MEASURED
// twice over: from the collector FUN_004b4bb0 (0x004b4bd4..0x004b4c5f, which
// writes puVar2[0..0xf]) and from the two readers — OIL FUN_00457800
// (0x004578dd..0x00457978) and P_MINE FUN_00457c10 (locals local_80..local_40).
struct WorldHit {
    float        normal[3];   // +0x00  puVar2[0..2]   <- descriptor[0..2]
    std::int32_t triIndex;    // +0x0c  puVar2[3]      <- descriptor[6]
    float        vert[9];     // +0x10  puVar2[4..12]  <- *descriptor[7..9] (3 verts)
    const void*  geom;        // +0x34  puVar2[0xd]    <- the walked object
    float        t;           // +0x38  puVar2[0xe]    <- the segment parameter
    const void*  owner;       // +0x3c  puVar2[0xf]    <- the walker's user data
    // NOT part of the original buffer. Stand-in channel for the RpMaterial the
    // two gates reach through geom/triIndex:
    //   OIL    0x004578dd..0x00457905, P_MINE FUN_00457c10 — both compute
    //   mat = (*(geom+0x3c))->[0x10][ tri[triIndex].matIdx + geom->u16@0x80 ]
    //   and pass it to FUN_0045c110, which reads *(uint*)(mat+4) (the RwRGBA).
    std::uint32_t matKey;
};

// The collision-triangle source the SegmentQuery stand-in walks (the
// FUN_00538c80 BSP walk's stand-in). Returns 1 and fills v9 (3 vertices,
// 9 floats) + matKey for index i, 0 to stop.
typedef int (*TriSourceFn)(void* ctx, int i, float v9[9], std::uint32_t* matKey);
void SetTriSource(TriSourceFn fn, void* ctx, int count);
int  HaveTriSource();

// Offline injectors for re/tools/pu_replay, which has no collision world: it
// replays the ORIGINAL's MEASURED per-call query verdicts instead of computing
// them. When installed these REPLACE the two leaves; the decision structure
// under test is unchanged. Not installed in the shipping exe.
typedef int (*QueryInjectFn)(void* ctx, const float seg[6], WorldHit* out,
                             std::uint32_t retAddr);
typedef int (*GateInjectFn)(void* ctx, const WorldHit* hit, std::uint32_t retAddr);
// The dispatcher's armed sweep, per slot (its two leaves are attributed by slot
// through arg2 = slot_base + 0x80, not by activation window).
typedef int (*SweepInjectFn)(void* ctx, int slot, int which);   // which: 0=query 1=confirm
void SetInjectors(QueryInjectFn q, GateInjectFn g, void* ctx);
void SetSweepInjector(SweepInjectFn s, void* ctx);

// ---- the four ported functions --------------------------------------------
// 0x004b4cd0 (-> FUN_004b4c80 -> FUN_00538c80 / FUN_004b4bb0). seg = {A, B}.
// Returns the intersection COUNT; `out` holds the nearest (smallest t).
//
// `rva`/`name` select which ORIGINAL wrapper this call stands in for, because the
// two that take a 6-float segment differ only in their collector and both are
// recorded separately by scenario_launch.py:
//   0x004b4cd0 -> FUN_004b4c80 -> collector FUN_004b4bb0: keeps the NEAREST hit,
//                 walks everything, returns the count.
//   0x004b4b20 -> FUN_004b4a80 -> collector FUN_004b49b0: fills an ARRAY of
//                 0x40-byte records up to a capacity (SHOTGUN passes 1, `PUSH 1`
//                 at 0x004b4b31), so it keeps the FIRST hit and stops when full.
// The stand-in walk is the same for both; the difference is noted, not modelled.
int  SegmentQuery(const float seg[6], WorldHit* out, std::uint32_t retAddr,
                  std::uint32_t rva = 0x004b4cd0u,
                  const char* name = "query_4b4cd0");
// 0x004b4650. Verbatim: out[i] = a[i] + t*(b[i]-a[i]).
void Vec3Lerp(float out[3], const float a[3], const float b[3], float t,
              std::uint32_t retAddr);
// 0x004b5080. m = 16-float RwMatrix; v9 = the hit triangle; pos may be null.
void BasisFromTri(float m[16], const float v9[9], const float* pos,
                  std::uint32_t retAddr);
// 0x0045c110. Nonzero = the surface REFUSES the drop.
int  SurfaceGate(const WorldHit* hit, std::uint32_t retAddr);

// ---- the dispatcher's armed sweep (FUN_0045bba0 0x0045bcb2..0x0045bd11) ----
// 0x004b4b60 @0x0045bcd3 — a 4-dword (tag 3) query, arg2 = slot_base + 0x80,
// whose first 3 dwords the dispatcher refreshes from EBX every pass
// (0x0045bcb2..0x0045bcca). Returns the intersection count.
int  SweepQuery(int slot, const float pos[3], WorldHit* out, std::uint32_t retAddr);
// 0x0045c350 @0x0045bce5 — (&result, slot_base+0x80). ZERO means the sweep
// CONDEMNS the power-up: 0x0045bcef `JNE 0x0045bd14` keeps it, fall-through runs
// FUN_0045bac0 at 0x0045bcf7 (the capture's deact_ra 0x45bcfc).
int  SweepConfirm(int slot, const WorldHit* hit, std::uint32_t retAddr);

// ---- MASHED_PU_CONTACTDUMP ------------------------------------------------
// Writes <path> in the exact column shape of scenario_launch.py's
// `<out>.pucontact.csv` (frame,call,rva,name,ret_addr,a1,a2,a3,ret) so that
// re/tools/pu_contact_report.py reads an ORIGINAL capture and a standalone /
// replay run with the same code path and the same (name, ret_addr) keys.
void SetCallCounter(std::uint32_t frame, std::uint32_t call);
void CloseDump();

}  // namespace Contact
}  // namespace Powerup
}  // namespace mashed_re
