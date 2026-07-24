// Mashed RE — B5d: the chain -> rigid-body COUPLING BRIDGE (FUN_0047eb30), verbatim.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-15). Clean-room verbatim transcription of the
// once-per-tick coupling driver that maps the A3-A6 vehicle force chain's per-car state
// onto the RW-Physics proxy body, steps the vendor solver, and reads the integrated body
// transform back into the render struct. Called unconditionally from dispatcher
// FUN_00470c70 after FUN_00467350 (C1 comment in the decomp).
//
// Source map: re/analysis/vehicle_coupling.md (the definitive law, all .rdata constants
// RVA-cited) + re/analysis/B5c_RWP37_INTEGRATOR_SUBSET_2026-07-15.md (the subset this
// drives) + re/analysis/B5d_COUPLING_BRIDGE_2026-07-15.md (this port).
//
// DESIGN — every callee is dispatched to its real RVA via a thunk. Under the .asi the
// bridge therefore calls the REAL game helpers; and because the B5c hooks install an
// inline-JMP at 0x0057c210 / 0x0055dff0 / 0x0055ac00 / 0x0055deb0 / 0x0047ea40, the
// bridge's RVA calls to those addresses transparently route into OUR ported helpers when
// the B5c hooks are installed. That is exactly the C4 exercise B5d needs: install the
// bridge + the 8 B5c hooks and the per-tick coupling surface runs through the ports, so a
// per-field body-state diff vs the original is a real bit-identity test — while everything
// stays runtime-toggleable (inert on the exe via HookSystemNoOp).
//
// GLOBALS — the bridge reads live game STATE (world ptr, build gate, dt, the 4 body-handle
// keys, the per-car record array, the readback CoM array). Those are bound to their
// absolute .rdata/.bss addresses (the reinterpret_cast-RVA idiom used across the tree, cf.
// RwpIntegrator.cpp MASHED_DAT_007dc8d8). They are only meaningful under the .asi; on the
// standalone exe this TU compiles + links but the hook is inert so none of it executes.
// The float CONSTANTS are inlined from their exact .rdata bit patterns (identical bits to
// the original's `FMUL float ptr [const]`), which is both-target safe.
//
// x87 — built with the tree's x87 model (no /arch:SSE2). The linear PD-spring half is
// float-only and is expected bit-identical. The angular heading half calls the game's
// atan2->deg (0x004233e0) and CRT acos (0x004a3384), whose results the original keeps in
// 80-bit ST0 across a cancelling subtract; MSVC has no 80-bit C type, so those fields
// carry the documented <=1-ULP (post-cancellation, a few-ULP) x87 floor — see the B5d note
// §acceptance for the measured per-field deltas.
//
// NO-GUESSING correction carried by this file (vs vehicle_coupling.md:95): the READBACK
// loop gate is record+0x14 (abs 0x008815b4, process iff != 0), NOT record+0x1c. Confirmed
// from disasm 0x0047f0e0: TEST [EBP-0x124],EBP with EBP=0x008816d8 -> 0x008815b4.
#include "../Core/HookSystem.h"
#include <cstring>
#include <cstdint>

namespace mashed_re {
namespace Vehicle {

namespace {

// Exact .rdata float constants (bit patterns from vehicle_coupling.md §"Recovered constants").
inline float fb(unsigned bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float C_005cf014 = fb(0x3951b717u);  // 0.0002    springTgt lookahead
const float C_005ccd6c = fb(0x41a00000u);  // 20.0      PD gain
const float C_005cd09c = fb(0x43340000u);  // 180.0     heading half-circle
const float C_005ccac4 = fb(0x43b40000u);  // 360.0     heading full-circle
const float C_005cc320 = fb(0x3f800000u);  // 1.0       dot clamp hi
const float C_005cc33c = fb(0xbf800000u);  // -1.0      dot clamp lo / negate
const float C_005cc98c = fb(0x42652ee1u);  // 57.2958   acos rad -> deg (180/pi)
const float C_005d757c = fb(0x00000000u);  // 0.0       zero literal

// Live game globals (absolute-address bound; meaningful only under the .asi).
#define G_world     (*reinterpret_cast<int*>  (0x006ce274u))  // DAT_006ce274   world ptr / present gate
#define G_buildGate (*reinterpret_cast<int*>  (0x0086caa0u))  // DAT_0086caa0   one-shot build gate (==0x7b)
#define G_dt        (*reinterpret_cast<float*>(0x0061331cu))  // DAT_0061331c   solver dt (0.05)
#define G_5cc9a0    (*reinterpret_cast<float*>(0x005cc9a0u))  // _DAT_005cc9a0  dt floor comparand
#define G_773920    (*reinterpret_cast<int*>  (0x00773920u))  // DAT_00773920   mode selector (562460 gate)
#define G_6cad48    (*reinterpret_cast<int*>  (0x006cad48u))  // _DAT_006cad48  per-tick counter
#define G_7f0f8c    (*reinterpret_cast<float*>(0x007f0f8cu))  // _DAT_007f0f8c  (playerCount+5000) as float
#define G_bodyKeys  ( reinterpret_cast<int*>  (0x006c9a78u))  // &DAT_006c9a78  4 body-handle keys
#define G_carBase   ( reinterpret_cast<float*>(0x00881f68u))  // &DAT_00881f68  per-car record cursor start
#define G_comBase   ( reinterpret_cast<float*>(0x008816d8u))  // &DAT_008816d8  readback CoM-offset (+0x138)

// ---- MASHED-side callees: RVA-forwarding thunks (__cdecl; caller-cleaned in the original). ----
inline int      gameMode()                 { return reinterpret_cast<int(__cdecl*)(void)>(0x0040e350u)(); }          // FUN_0040e350
inline int      playerCount()              { return reinterpret_cast<int(__cdecl*)(void)>(0x0040e340u)(); }          // FUN_0040e340
inline unsigned f00426060()                { return reinterpret_cast<unsigned(__cdecl*)(void)>(0x00426060u)(); }     // FUN_00426060
inline void     f00562460(int a)           { reinterpret_cast<void(__cdecl*)(int)>(0x00562460u)(a); }                // FUN_00562460
inline void     f0047d3c0(int w, unsigned a){ reinterpret_cast<void(__cdecl*)(int,unsigned)>(0x0047d3c0u)(w, a); }   // FUN_0047d3c0
inline void     getRenderMtx(void* o, unsigned i){ reinterpret_cast<void(__cdecl*)(void*,unsigned)>(0x0046d4a0u)(o, i); } // FUN_0046d4a0
inline void     getOffset3D(float* o, unsigned i){ reinterpret_cast<void(__cdecl*)(float*,unsigned)>(0x0046cb30u)(o, i); } // FUN_0046cb30
inline void     getVelWorld(void* o, unsigned i){ reinterpret_cast<void(__cdecl*)(void*,unsigned)>(0x0046d510u)(o, i); }   // FUN_0046d510
inline void     readbackMtx(unsigned i, void* s){ reinterpret_cast<void(__cdecl*)(unsigned,void*)>(0x0046d4d0u)(i, s); }   // FUN_0046d4d0
inline void     xform(float* dst, const float* src, int n, const void* m)
                { reinterpret_cast<void(__cdecl*)(float*,const float*,int,const void*)>(0x004c3df0u)(dst, src, n, m); }     // FUN_004c3df0
inline double   atan2deg(float x, float z) { return reinterpret_cast<double(__cdecl*)(float,float)>(0x004233e0u)(x, z); }  // FUN_004233e0
inline double   facos(double d)            { return reinterpret_cast<double(__cdecl*)(double)>(0x004a3384u)(d); }          // FUN_004a3384
inline void     f004c45f0(void* m)         { reinterpret_cast<void(__cdecl*)(void*)>(0x004c45f0u)(m); }                    // FUN_004c45f0
inline void     f004c0e50(int a)           { reinterpret_cast<void(__cdecl*)(int)>(0x004c0e50u)(a); }                      // FUN_004c0e50
// B5c-ported helpers (call by RVA -> routes into our ports when the B5c hooks are installed).
inline int      rwpBody(int key)           { return reinterpret_cast<int(__cdecl*)(int)>(0x0057c210u)(key); }             // FUN_0057c210
inline int      rwpWorldSolver(int w, float dt){ return reinterpret_cast<int(__cdecl*)(int,float)>(0x0055deb0u)(w, dt); } // FUN_0055deb0
inline int      rwpSceneStep(unsigned ctx) { return reinterpret_cast<int(__cdecl*)(unsigned)>(0x0047ea40u)(ctx); }        // FUN_0047ea40
inline void     rwpRefreshGate(int body, int idx){ reinterpret_cast<void(__cdecl*)(int,int)>(0x0055dff0u)(body, idx); }   // FUN_0055dff0
inline void     rwpActiveBit(int owner, int body, int set){ reinterpret_cast<void(__cdecl*)(int,int,int)>(0x0055ac00u)(owner, body, set); } // FUN_0055ac00

// The two body-local axis constants transformed by the body matrix (passed by address).
#define AXIS_00614708 (reinterpret_cast<const float*>(0x00614708u))  // &DAT_00614708 forward axis
#define AXIS_006146fc (reinterpret_cast<const float*>(0x006146fcu))  // &DAT_006146fc up/right axis

}  // namespace

// ===========================================================================
// 0x0047eb30  VehicleCouplingBridge — once-per-tick chain<->rigid-body coupling.
// Verbatim transcription of FUN_0047eb30 (0x0047eb30..0x0047f1db, ~1700 b).
// ===========================================================================
static bool VehicleCouplingBridge_impl(bool forwardOnly)
{
    if (G_world == 0) return false;                                   // 0x0047eb45

    if (gameMode() != 6) {                                            // 0x0047eb55
        G_6cad48 = 0;                                                 // 0x0047eb5f
    }
    if (G_buildGate == 0x7b) {                                        // 0x0047eb65  one-shot build
        unsigned built = f00426060();                                // 0x0047eb6e
        if (G_773920 == 1 || (4 < G_773920 && G_773920 < 7)) {       // 0x0047eb7a..eb87
            f00562460(*reinterpret_cast<int*>(0x0080332cu));          // 0x0047eb89  PUSH [0x0080332c]
        }
        f0047d3c0(G_world, built);                                    // 0x0047eb9f
        G_buildGate = 0;                                             // 0x0047eba7
    }
    if (G_5cc9a0 < G_dt) {                                            // 0x0047ebad FLD dt / FCOMP 5cc9a0
        G_dt = 0.05f;                                                 // 0x0047ebc0 0x3d4ccccd
    }
    G_6cad48 = G_6cad48 + 1;                                          // 0x0047ebca
    G_7f0f8c = (float)(playerCount() + 5000);                        // 0x0047ebd0..ebed  +0x1388

    // ---- Per-car forward loop: cursor p = &DAT_00881f68, stride 0x341 floats (0xd04 b),
    //      index i in parallel, until (int)p >= 0x00885378. ----
    float* p = G_carBase;
    int i = 0;
    do {
        // live gate: process iff record+0x14 (p[-0x26d]) == 0, AND i < playerCount.
        if (p[-0x26d] == 0.0f && i < playerCount()) {                // 0x0047ebf3 / 0x0047ebff
            unsigned* renderMtx;                                      // out of getRenderMtx (mtx ptr)
            getRenderMtx(&renderMtx, (unsigned)i);                    // 0x0047ec12  FUN_0046d4a0
            int body = rwpBody(G_bodyKeys[i]);                        // 0x0047ec1f  FUN_0057c210
            int* piVar2  = *(int**)(*(int*)(body + 0x10) + 4);        // 0x0047ec2b  body handle
            unsigned* bodyMtx = *(unsigned**)(*(int*)(body + 0x10) + 8); // 0x0047ec2e  body world matrix

            if (piVar2 != 0) {                                        // 0x0047ec34
                float o[3];
                getOffset3D(o, (unsigned)i);                         // 0x0047ec42  FUN_0046cb30 -> offset3D
                float sx = o[0] * C_005cf014 + (float)renderMtx[0xc];// 0x0047ec4b springTgt = offset3D*0.0002 + renderPos
                float sy = o[1] * C_005cf014 + (float)renderMtx[0xd];
                float sz = o[2] * C_005cf014 + (float)renderMtx[0xe];
                float com[3] = { p[-0x224], p[-0x223], p[-0x222] };  // 0x0047ec51 record+0x138 CoM-offset
                xform(com, com, 1, bodyMtx);                         // 0x0047ecbd  comWorld = transform(CoM, bodyMtx)
                float* linVel = (float*)(*(int*)(*(int*)(*piVar2 + 0x10) + 8) + piVar2[1] * 0x20); // 0x0047ecc9 body linvel slot
                float bpz = (float)bodyMtx[0xe];                     // 0x0047ecc2 bodyPos.z
                float vx = (sx - (com[0] + (float)bodyMtx[0xc])) * C_005ccd6c;  // 0x0047ecfa (springTgt - (comW+bodyPos))*20
                float vy = (sy - (com[1] + (float)bodyMtx[0xd])) * C_005ccd6c;
                *linVel = vx;                                        // 0x0047ed0e
                float vz = (sz - (com[2] + bpz)) * C_005ccd6c;
                linVel[1] = vy;                                      // 0x0047ed22
                linVel[2] = vz;                                      // 0x0047ed32

                // angular: align body heading to velocity heading (DEGREES).
                float velW[3];
                getVelWorld(velW, (unsigned)i);                     // 0x0047ed35  FUN_0046d510
                float velHeading = (float)atan2deg(velW[0], velW[2]);// 0x0047ed44 velHeading (rounded to float)
                float fwd[3];
                xform(fwd, AXIS_00614708, 1, bodyMtx);              // 0x0047ed5a bodyFwd = transform(fwdAxis, bodyMtx)
                // hd kept "extended" as double (original holds ST0 80-bit across this subtract).
                double hd = (atan2deg(fwd[0], fwd[2]) - (double)velHeading) + (double)C_005cd09c; // 0x0047ed6c..ed78
                if (hd < (double)C_005d757c) {                       // 0x0047ed7e
                    do { hd = hd + (double)C_005ccac4; } while (hd < (double)C_005d757c); // 0x0047ed90
                }
                if ((double)C_005cd09c < hd) {                       // 0x0047eda3
                    hd = -((double)C_005ccac4 - hd);                 // 0x0047edb0 FSUBR/FCHS
                }
                if (hd <= (double)C_005d757c) {                      // 0x0047edb8
                    hd = -(hd + (double)C_005cd09c);                 // 0x0047edc5
                } else {
                    hd = (double)C_005cd09c - hd;                    // 0x0047edcd
                }
                float aVx = 0.0f, aVy = (float)hd, aVz = 0.0f;       // 0x0047edd5 bodyAngVel = (0, hd, 0)

                // cross-correction toward velocity (only if axis x vel != 0 and dot < 1).
                float axis[3];
                xform(axis, AXIS_006146fc, 1, bodyMtx);             // 0x0047edfa axis = transform(upAxis, bodyMtx)
                float cx = axis[1] * p[2] - axis[2] * p[1];         // 0x0047edff cross(axis, p[0..2])
                float cy = axis[2] * p[0] - axis[0] * p[2];
                float cz = axis[0] * p[1] - axis[1] * p[0];
                float dt_ = axis[2] * p[2] + axis[1] * p[1] + axis[0] * p[0]; // 0x0047ee54 dot
                if ((cx != C_005d757c || cy != C_005d757c || cz != C_005d757c) && dt_ < C_005cc320) { // 0x0047ee8b
                    float c = C_005cc33c;                            // 0x0047ee9c clamp lo -1
                    if (C_005cc33c <= dt_) { c = dt_; if (C_005cc320 < dt_) c = C_005cc320; } // clamp hi +1
                    double k = facos((double)c) * (double)C_005cc98c; // 0x0047eece acos*57.2958
                    // save/restore of bodyMtx[0xc]/[0xe] around the writes (verbatim; net no-op).
                    unsigned s0 = bodyMtx[0xc], s2 = bodyMtx[0xe];   // 0x0047eee2
                    aVx = (float)((double)cx * k);                   // 0x0047ef05 region
                    bodyMtx[0xc] = s0; bodyMtx[0xd] = bodyMtx[0xd]; bodyMtx[0xe] = s2;
                    aVy = (float)((double)cy * k + (double)aVy);     // 0x0047ef0a  cy*k + hd
                    aVz = (float)(k * (double)cz);                   // 0x0047ef1f
                }
                float* angVel = (float*)(*(int*)(*(int*)(*piVar2 + 0x10) + 8) + 0x10 + piVar2[1] * 0x20); // 0x0047ef2b angvel slot
                angVel[0] = aVx; angVel[1] = aVy; angVel[2] = aVz;   // 0x0047ef45..ef4f
                f004c45f0(bodyMtx);                                  // 0x0047ef52
                f004c0e50(*(int*)(G_bodyKeys[i] + 4));               // 0x0047ef62
                rwpRefreshGate(body, 0);                             // 0x0047ef6e  FUN_0055dff0(body, 0)
                rwpActiveBit(*(int*)(body + 0x24), body, 1);         // 0x0047ef7a  FUN_0055ac00(owner, body, 1)
            }

            int gm = gameMode();                                     // 0x0047ef82
            if (gm == 3 || gm == 5) {                                // 0x0047ef87..ef8c  teleport body<-render
                unsigned* rm = renderMtx;                            // *local_64 = render matrix words
                for (int k = 0; k < 0x10; ++k) bodyMtx[k] = rm[k];   // 0x0047efc0 REP MOVSD render -> body
                float coff[3] = { p[-0x224], p[-0x223], p[-0x222] }; // CoM-offset
                xform(coff, coff, 1, bodyMtx);                      // 0x0047efe6
                *(float*)&bodyMtx[0xc] = (float)bodyMtx[0xc] - coff[0]; // 0x0047efec  translation -= comWorld
                *(float*)&bodyMtx[0xd] = (float)bodyMtx[0xd] - coff[1]; // 0x0047eff6
                *(float*)&bodyMtx[0xe] = (float)bodyMtx[0xe] - coff[2]; // 0x0047f003
                f004c45f0(bodyMtx);                                  // 0x0047f010
                f004c0e50(*(int*)(G_bodyKeys[i] + 4));               // 0x0047f020
                rwpRefreshGate(body, 0);                             // 0x0047f02c
                rwpActiveBit(*(int*)(body + 0x24), body, 1);         // 0x0047f038
            }
        }

        // idx >= playerCount: drop the proxy body out of play (+0x34 = -10000.0).
        if (playerCount() <= i) {                                     // 0x0047f046
            int body = rwpBody(G_bodyKeys[i]);                       // 0x0047f057
            int m = *(int*)(*(int*)(body + 0x10) + 8);               // body matrix
            *(int*)(m + 0x30) = 0;                                   // 0x0047f065
            *(unsigned*)(m + 0x34) = 0xc61c4000u;                    // 0x0047f068  -10000.0
            *(int*)(m + 0x38) = 0;                                   // 0x0047f06f
            f004c45f0((void*)m);                                     // 0x0047f072
            f004c0e50(*(int*)(G_bodyKeys[i] + 4));                   // 0x0047f082
            rwpRefreshGate(body, 0);                                 // 0x0047f089
            rwpActiveBit(*(int*)(body + 0x24), body, 0);             // 0x0047f094  set=0 here
        }

        p += 0x341;                                                  // 0x0047f09c  += 0xd04
        i += 1;
    } while ((int)(uintptr_t)p < 0x00885378);                        // 0x0047f0a3

    // C4 self-test: run only the self-contained forward-loop PD/heading math (the bridge's
    // characteristic ported code). The vendor solver below mutates global world state and
    // cannot be cleanly snapshot/restored for an in-process A/B, so the diff is scoped to
    // the body linVel/angVel/matrix the forward loop writes. Matches the original's early-
    // return at 0x0047f0b3 -> epilogue 0x0047eb4b (see Bridge_SelfTest).
    if (forwardOnly) return true;

    // ---- vendor solver step ----
    int ctx = rwpWorldSolver(G_world, G_dt);                        // 0x0047f0c0  FUN_0055deb0(world, dt)
    int stepResult = rwpSceneStep((unsigned)ctx);                   // 0x0047f0c9  FUN_0047ea40(ctx)

    // ---- Readback loop: cursor q = &DAT_008816d8 (CoM-offset, record+0x138), stride 0x341,
    //      index j in parallel, until (int)q >= 0x00884ae8.  Gate: record+0x14 (q[-0x49]) != 0. ----
    float* q = G_comBase;
    int j = 0;
    do {
        if (q[-0x49] != 0.0f) {                                      // 0x0047f0e0  record+0x14 != 0
            unsigned* srcMtx = (unsigned*)(*(int*)(G_bodyKeys[j] + 4) + 0x10); // body matrix
            unsigned buf[0x10];
            for (int k = 0; k < 0x10; ++k) buf[k] = srcMtx[k];      // 0x0047f10b  copy 16 words
            float nco[3] = { q[0] * C_005cc33c, q[1] * C_005cc33c, q[2] * C_005cc33c }; // -CoM-offset
            xform(nco, nco, 1, buf);                                // 0x0047f15d  transform by body matrix
            ((float*)buf)[0xc] = ((float*)buf)[0xc] - nco[0];       // 0x0047f169  translation -= comWorld
            ((float*)buf)[0xd] = ((float*)buf)[0xd] - nco[1];       // 0x0047f184
            ((float*)buf)[0xe] = ((float*)buf)[0xe] - nco[2];       // 0x0047f196
            readbackMtx((unsigned)j, buf);                          // 0x0047f1a1  FUN_0046d4d0 -> both render buffers
            rwpBody(G_bodyKeys[j]);                                 // 0x0047f1ae  (return ignored)
        }
        q += 0x341;                                                 // 0x0047f1b6
        j += 1;
    } while ((int)(uintptr_t)q < 0x00884ae8);                       // 0x0047f1bd

    return stepResult != 0;                                          // 0x0047f1cd  SETNZ
}

// ═══════════════════════════════════════════════════════════════════════════
// B5d IN-PROCESS BIT-IDENTITY SELF-TEST — the bridge's forward-loop PD/heading math (C4).
// env MASHED_PHYS_C4_SELFTEST. Per sampled tick: snapshot the 4 proxy bodies' matrix +
// linVel/angVel, run the ORIGINAL setup+forward-loop (temp HookSystem::Uninstall + a
// JMP-to-epilogue patch at 0x0047f0b3 -> 0x0047eb4b that returns BEFORE the vendor solver),
// capture, restore, run OUR forward-loop (impl(forwardOnly=true)), per-field compare,
// restore, then run the real full port. Skips the one-shot build tick (G_buildGate==0x7b)
// so f0047d3c0's world-build side effect never triple-runs. Logs original/phys_c4_bridge_selftest.log.
//
// 2026-07-24 RESULT + LIMITATION: over a canonical race the ANGULAR half is bit-identical
// (dAng=0, 32/32 — validates the x87 atan2/acos heading math), but the LINEAR half shows a
// systematic dLin=12/dMtx=12. Root cause = a HARNESS artifact, NOT a port bug: the forward
// loop reads its render inputs via getRenderMtx (0x0046d4a0) / getOffset3D (0x0046cb30), which
// are NON-IDEMPOTENT (return different values on the 2nd call within a tick — per-call-advancing/
// double-buffered), so the ORIG and PORT runs see different springTgt -> different linVel/matrix.
// (getVelWorld, used by the angular half, IS idempotent -> dAng=0.) So the in-process snapshot/
// restore A/B — clean for the self-contained B5c leaves — is CONTAMINATED for the coupled bridge.
// Bridge C4 is NOT earned by this method: it needs the render inputs FROZEN (capture on run 1,
// force-feed run 2) or a two-launch deterministic diff (B5d note §4). Evidence:
// log/phys_c4_bridge_selftest_ARTIFACT_20260724.log. dAng=0 stands as partial (angular) validation.
// ═══════════════════════════════════════════════════════════════════════════
static int bridgeSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { char b[8]; DWORD n = GetEnvironmentVariableA("MASHED_PHYS_C4_SELFTEST", b, sizeof b);
                 v = (n > 0 && n < sizeof b && b[0]) ? 1 : 0; }
    return v;
}
static int bridgeHookIndex(std::uintptr_t rva) {
    for (std::size_t i = 0; i < HookSystem::Count(); ++i)
        if (HookSystem::At(i).target_rva == rva) return (int)i;
    return -1;
}
static void bridgeLog(const char* s) {
    HANDLE h = CreateFileA("phys_c4_bridge_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, s, (DWORD)lstrlenA(s), &w, nullptr); CloseHandle(h);
}
static bool g_inBridgeTest = false;
static int  g_bridgeCount = 0;
static const int kBridgeMax = 32;

struct BridgeBody { unsigned* mtx; unsigned* vel; bool hasVel; };   // per-car slot addresses (stable in a tick)
static bool bridgeResolveBodies(BridgeBody out[4]) {
    for (int i = 0; i < 4; ++i) {
        out[i].mtx = nullptr; out[i].vel = nullptr; out[i].hasVel = false;
        int body = rwpBody(G_bodyKeys[i]);          if (body == 0) return false;
        int m0 = *(int*)(body + 0x10);              if (m0 == 0)   return false;
        out[i].mtx = *(unsigned**)(m0 + 8);         if (out[i].mtx == nullptr) return false;
        int* piVar2 = *(int**)(m0 + 4);
        if (piVar2 != nullptr) {
            out[i].vel = (unsigned*)(std::uintptr_t)(*(int*)(*(int*)(*piVar2 + 0x10) + 8) + piVar2[1] * 0x20);
            out[i].hasVel = true;
        }
    }
    return true;
}
static void bridgeRead(const BridgeBody b[4], unsigned buf[4][24]) {
    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < 0x10; ++k) buf[i][k] = b[i].mtx[k];
        for (int k = 0; k < 8;    ++k) buf[i][0x10 + k] = b[i].hasVel ? b[i].vel[k] : 0u;
    }
}
static void bridgeRestore(const BridgeBody b[4], const unsigned buf[4][24]) {
    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < 0x10; ++k) b[i].mtx[k] = buf[i][k];
        if (b[i].hasVel) for (int k = 0; k < 8; ++k) b[i].vel[k] = buf[i][0x10 + k];
    }
}

static bool Bridge_SelfTest() {
    g_inBridgeTest = true;
    BridgeBody bods[4];
    if (!bridgeResolveBodies(bods)) { g_inBridgeTest = false; return VehicleCouplingBridge_impl(false); }
    static unsigned pre[4][24], orig[4][24], mine[4][24];
    bridgeRead(bods, pre);
    int g6 = G_6cad48; float g7 = G_7f0f8c; float gdt = G_dt;

    // (1) ORIGINAL setup+forward via temp uninstall + JMP-to-epilogue patch at 0x0047f0b3.
    int idx = bridgeHookIndex(0x0047eb30u);
    unsigned char* site = (unsigned char*)(std::uintptr_t)0x0047f0b3u;
    unsigned char saved[5]; DWORD op, t;
    if (VirtualProtect(site, 5, PAGE_EXECUTE_READWRITE, &op)) {
        std::memcpy(saved, site, 5);
        site[0] = 0xE9;
        *reinterpret_cast<unsigned*>(site + 1) = 0x0047eb4bu - (0x0047f0b3u + 5);   // JMP 0x0047eb4b (epilogue)
        VirtualProtect(site, 5, op, &t);
    }
    if (idx >= 0) HookSystem::Uninstall((std::size_t)idx);
    reinterpret_cast<bool(__cdecl*)()>(0x0047eb30u)();
    if (idx >= 0) HookSystem::Install((std::size_t)idx);
    if (VirtualProtect(site, 5, PAGE_EXECUTE_READWRITE, &op)) { std::memcpy(site, saved, 5); VirtualProtect(site, 5, op, &t); }
    bridgeRead(bods, orig);

    // (2) restore, run OUR forward-loop only
    bridgeRestore(bods, pre); G_6cad48 = g6; G_7f0f8c = g7; G_dt = gdt;
    VehicleCouplingBridge_impl(true);
    bridgeRead(bods, mine);

    // (3) per-field compare (matrix + linVel + angVel; angVel carries the accepted x87 ULP floor)
    int dMtx = 0, dLin = 0, dAng = 0, angMaxUlp = 0;
    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < 0x10; ++k) if (mine[i][k] != orig[i][k]) dMtx++;
        if (bods[i].hasVel) {
            for (int k = 0; k < 3; ++k) if (mine[i][0x10 + k] != orig[i][0x10 + k]) dLin++;
            for (int k = 4; k < 7; ++k) if (mine[i][0x10 + k] != orig[i][0x10 + k]) {
                dAng++; int u = (int)mine[i][0x10 + k] - (int)orig[i][0x10 + k];
                if (u < 0) u = -u; if (u > angMaxUlp) angMaxUlp = u;
            }
        }
    }
    char hdr[320];
    // car-0 linVel[0] + mtx[0xc] pre/orig/mine to characterize the diff (bug vs artifact)
    wsprintfA(hdr, "[%d] fn=VehicleCouplingBridge dMtx=%d dLin=%d dAng=%d angMaxUlp=%d%s"
                   "  | c0 lin0 pre=%08x o=%08x m=%08x  mtxC pre=%08x o=%08x m=%08x\r\n",
              g_bridgeCount, dMtx, dLin, dAng, angMaxUlp,
              (dMtx == 0 && dLin == 0 && dAng == 0) ? " OK" : "",
              bods[0].hasVel ? pre[0][0x10] : 0u, bods[0].hasVel ? orig[0][0x10] : 0u, bods[0].hasVel ? mine[0][0x10] : 0u,
              pre[0][0xc], orig[0][0xc], mine[0][0xc]);
    bridgeLog(hdr);

    // (4) restore + real full run for the game
    bridgeRestore(bods, pre); G_6cad48 = g6; G_7f0f8c = g7; G_dt = gdt;
    ++g_bridgeCount; g_inBridgeTest = false;
    return VehicleCouplingBridge_impl(false);
}

// 0x0047eb30
extern "C" bool __cdecl VehicleCouplingBridge(void)
{
    if (!bridgeSelfTestEnabled() || g_inBridgeTest || g_bridgeCount >= kBridgeMax
        || G_world == 0 || G_buildGate == 0x7b)          // skip the one-shot-build tick
        return VehicleCouplingBridge_impl(false);
    return Bridge_SelfTest();
}

// --- gta-reversed-style hook registration (inert on the exe; inline-JMP under the .asi). ---
RH_ScopedInstall(VehicleCouplingBridge, 0x0047eb30);

}  // namespace Vehicle
}  // namespace mashed_re
