// Mashed RE — B5c: link-resolution for the RenderWare-Physics-3.7 load-time build
// chain (QhullBridge / CollisionBodyCreate / PhysicsWorldBuild) + the RW-math/solver
// callees used by the ported integrator subset (RwpIntegrator.cpp).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
// This closes B5b's "wire the build chain into build.bat" residue: it provides a
// definition for every un-ported RWP helper extern-declared in CollisionBuildDeps.h so
// the three build TUs compile+link into BOTH targets.
//
// RESOLUTION STRATEGY (deliberate; NO-GUESSING):
//   * FUNCTIONS -> RVA-forwarding thunks (reinterpret_cast to the real address). Under the
//     .asi (injected into MASHED) these call the REAL RWP helpers, so once the build chain
//     is wired (B5e) it runs faithfully. On the standalone exe they link but are UNCALLED
//     (the load-time build chain is not yet invoked standalone — see the TU headers). This
//     is the same reinterpret_cast-RVA idiom used across the tree (Race/ScoringHooks.cpp …).
//   * FUN_0055dec0 is implemented DIRECTLY — it is the confirmed 3-byte leaf `return *world`
//     (body 0x0055dec0..0x0055dec6). FUN_0047d3c0 calls it with 4 args but cdecl discards
//     the extra 3 (they are pushed-and-ignored) — the "register body into world" label was
//     mis-attributed. Resolves the CollisionBuildDeps.h vs Util/UtilLeaves_ab6.cpp conflict.
//   * DATA GLOBALS -> placeholder storage (the build chain is not run yet on EITHER target;
//     when it is wired live under the .asi, B5e must rebind these to the game's real globals
//     via absolute-address macros, exactly as RwpIntegrator.cpp binds DAT_007dc8d8). The
//     read-only cone/scale CONSTANTS are seeded with their real .rdata values so a standalone
//     dry-run is at least self-consistent; runtime STATE globals are zero (documented).
#include <cstring>

namespace {
inline float f_from_bits(unsigned bits) { float f; std::memcpy(&f, &bits, 4); return f; }
}  // namespace

// Forward one call to the real MASHED function at its RVA.
#define RWP_FWD(rva) (rva##u)

extern "C" {

// ===================== FUNCTION externs -> RVA-forwarding thunks =====================
// (signatures exactly as CollisionBuildDeps.h / QhullBridge.cpp declare them.)

void* FUN_0057c500(void)                    { return reinterpret_cast<void*(__cdecl*)(void)>(0x0057c500u)(); }
int   FUN_0057c300(void* b)                 { return reinterpret_cast<int(__cdecl*)(void*)>(0x0057c300u)(b); }
void  FUN_0057c220(int key, int node)       { reinterpret_cast<void(__cdecl*)(int,int)>(0x0057c220u)(key, node); }
void  FUN_0057c550(void* b)                 { reinterpret_cast<void(__cdecl*)(void*)>(0x0057c550u)(b); }
void  FUN_0055c810(int s, float* com)       { reinterpret_cast<void(__cdecl*)(int,float*)>(0x0055c810u)(s, com); }
void  FUN_0055c4f0(int s, float v)          { reinterpret_cast<void(__cdecl*)(int,float)>(0x0055c4f0u)(s, v); }
void  FUN_0055c4a0(int s, float v)          { reinterpret_cast<void(__cdecl*)(int,float)>(0x0055c4a0u)(s, v); }
void  FUN_0055bab0(int s, float v)          { reinterpret_cast<void(__cdecl*)(int,float)>(0x0055bab0u)(s, v); }
void  FUN_0055b940(void* b, float m, int f) { reinterpret_cast<void(__cdecl*)(void*,float,int)>(0x0055b940u)(b, m, f); }
void  FUN_00482730(int body, void* rgba)    { reinterpret_cast<void(__cdecl*)(int,void*)>(0x00482730u)(body, rgba); }
void* FUN_004c0b30(void)                    { return reinterpret_cast<void*(__cdecl*)(void)>(0x004c0b30u)(); }
void  FUN_004e7e30(int body, void* pl)      { reinterpret_cast<void(__cdecl*)(int,void*)>(0x004e7e30u)(body, pl); }
void  FUN_0047d240(float i,int a,float b,float y,float x)
                                            { reinterpret_cast<void(__cdecl*)(float,int,float,float,float)>(0x0047d240u)(i,a,b,y,x); }
void  FUN_00559c40(void* reg)               { reinterpret_cast<void(__cdecl*)(void*)>(0x00559c40u)(reg); }
int   FUN_00426c00(void)                    { return reinterpret_cast<int(__cdecl*)(void)>(0x00426c00u)(); }
void  FUN_0055ae70(int owner,int node,int g){ reinterpret_cast<void(__cdecl*)(int,int,int)>(0x0055ae70u)(owner, node, g); }
void* FUN_00482140(void* hull, int flag)    { return reinterpret_cast<void*(__cdecl*)(void*,int)>(0x00482140u)(hull, flag); }
void  FUN_00563810(void* hull)              { reinterpret_cast<void(__cdecl*)(void*)>(0x00563810u)(hull); }
void  FUN_00563940(void* slab)              { reinterpret_cast<void(__cdecl*)(void*)>(0x00563940u)(slab); }

// RW-Graphics matrix callees used by RwpIntegrator.cpp's FUN_0055b800 port, and the
// deferred 12-stage RWP world step. RVA-forwarding so the .asi A/B is bit-identical.
unsigned* FUN_004c52f0(unsigned* d, unsigned* s, int op) { return reinterpret_cast<unsigned*(__cdecl*)(unsigned*,unsigned*,int)>(0x004c52f0u)(d, s, op); }
void      FUN_004c51a0(float* m, float* v, int op)       { reinterpret_cast<void(__cdecl*)(float*,float*,int)>(0x004c51a0u)(m, v, op); }
unsigned  FUN_00546b10(float* q, float* m)               { return reinterpret_cast<unsigned(__cdecl*)(float*,float*)>(0x00546b10u)(q, m); }
void      FUN_0047e9c0(int ctx, unsigned a2)             { reinterpret_cast<void(__cdecl*)(int,unsigned)>(0x0047e9c0u)(ctx, a2); }

// 0x0055dec0 — confirmed leaf `return *param_1` (extra cdecl args discarded).
void* FUN_0055dec0(void* world, int /*node*/, int /*minus1*/, int /*two*/) { return *reinterpret_cast<void**>(world); }

}  // extern "C"

// ============================ DATA-GLOBAL placeholders =============================
// NOTE (B5e): these back the load-time build chain purely for LINK. The chain is not run
// on either target yet. When wired live under the .asi it must read the game's real
// globals — rebind these to absolute-address macros then (cf. RwpIntegrator.cpp
// MASHED_DAT_007dc8d8). Seeded read-only constants are the real .rdata values.
extern "C" {
// --- CollisionBodyCreate.cpp cone table + constants ---
int   DAT_006ce818 = 0;                     // 0x006ce818 cone sample count (lazy-init guard)
float DAT_006ce278[360] = {0};              // 0x006ce278 cone dir table (<=120 * vec3)
float _DAT_005cf23c = 1.78f;                // 0x005cf23c 0x3fe3d70a cone step numerator
// 0x005cf240 is an 8-byte DOUBLE = 120.0 (0x405e000000000000), NOT a float — see
// CollisionBodyCreate.cpp / B5c note §5. It is inlined there as a literal (no runtime
// writer: reference_to = 1 READ, 0 WRITE), so no extern is needed here.
double _DAT_005cf240_dbl = 120.0;           // 0x005cf240 (documented; not referenced by name)
float _DAT_005cf238 = 0.0f;                 // 0x005cf238 real float denormal 0x047a0001 (placeholder 0; type-1 scale)
float _DAT_005ce2f4 = 3.14159265358979f;    // 0x005ce2f4 0x40490fdb pi (latitude max)
float _DAT_005cd2c0 = 6.28318530717959f;    // 0x005cd2c0 0x40c90fdb 2*pi (longitude span)
float DAT_005d757c  = 0.0f;                 // 0x005d757c 0.0
float _DAT_005cd0ec = 0.005f;               // 0x005cd0ec 0x3ba3d70a type-4 radius floor
// --- PhysicsWorldBuild.cpp body keys + bbox/scale/CoM ---
int   DAT_006c9a78[4] = {0,0,0,0};          // 0x006c9a78 the 4 body-handle keys
float _DAT_00881630 = 0.0f, _DAT_0088163c = 0.0f, _DAT_00881634 = 0.0f;   // car bbox (runtime STATE)
float _DAT_00881638 = 0.0f, _DAT_00881650 = 0.0f, _DAT_00881664 = 0.0f;
float _DAT_005cc32c = 0.5f;                 // 0x005cc32c bbox scale
float _DAT_005cc9dc = 0.95f;                // 0x005cc9dc bbox shrink
float _DAT_005cc94c = 0.0f;                 // 0x005cc94c int->float sign-fix addend (i>=0 so unused)
float _DAT_005cd074 = 0.0f;                 // 0x005cd074 per-body X spacing [UNCERTAIN value] (placeholder 0)
float DAT_008816d8 = 0.0f, DAT_008816dc = 0.0f, DAT_008816e0 = 0.0f;      // CoM (written by chain)
float DAT_008823dc = 0.0f, _DAT_008823e0 = 0.0f, _DAT_008823e4 = 0.0f;
float _DAT_008830e0 = 0.0f, _DAT_008830e4 = 0.0f, _DAT_008830e8 = 0.0f;
float _DAT_00883de4 = 0.0f, _DAT_00883de8 = 0.0f, _DAT_00883dec = 0.0f;
}  // extern "C"

// Keep f_from_bits referenced (documents how B5e should seed the denormal/unknown consts).
namespace mashed_re { namespace Collision {
float Rwp_DenormalConeScale() { return f_from_bits(0x047a0001u); }  // real _DAT_005cf238
}}  // namespace
