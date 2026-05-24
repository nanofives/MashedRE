// Mashed RE - RwV3d column-major transform primitives.
//
// 0x004c3730  FUN_004c3730  promote_c2_render_d3d9-20260513  C2 -> C3
// Transform a 3D point by a column-major 4x4 RW matrix (includes translation).
//
// 0x004c3880  FUN_004c3880  promote_c2_render_d3d9-20260513  C2 -> C3
// Transform a 3D direction vector by a column-major 4x4 RW matrix (no translation).
//
// Matrix layout (float[16], column-major):
//   col0: mat[0..2]=right.xyz  mat[3]=flags
//   col1: mat[4..6]=up.xyz     mat[7]=pad
//   col2: mat[8..10]=at.xyz    mat[11]=pad
//   col3: mat[12..14]=pos.xyz  mat[15]=pad
//
// Decompilation verbatim from promote_c2_render_d3d9 session. Both functions
// have zero callees and zero direct callers (stored as function pointers by
// FUN_004c35f0).
#include "../Core/HookSystem.h"

// 0x004c3730
// out = mat * in  (full affine point transform including translation)
extern "C" __declspec(dllexport)
void __cdecl RwV3dTransformPoint(float* out, const float* in, const float* mat)
{
    const float x = in[0];
    const float y = in[1];
    const float z = in[2];

    out[0] = mat[8]*z + mat[4]*y + x*mat[0] + mat[12];
    out[1] = mat[13] + mat[9]*z  + mat[5]*y + mat[1]*x;
    out[2] = mat[14] + mat[10]*z + mat[6]*y + mat[2]*x;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RwV3dTransformPoint, 0x004c3730);

// 0x004c3880
// out = mat * in  (linear direction transform; translation omitted)
extern "C" __declspec(dllexport)
void __cdecl RwV3dTransformVector(float* out, const float* in, const float* mat)
{
    const float x = in[0];
    const float y = in[1];
    const float z = in[2];

    out[0] = mat[8]*z  + mat[4]*y + x*mat[0];
    out[1] = mat[9]*z  + mat[5]*y + mat[1]*x;
    out[2] = mat[10]*z + mat[6]*y + mat[2]*x;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RwV3dTransformVector, 0x004c3880);
