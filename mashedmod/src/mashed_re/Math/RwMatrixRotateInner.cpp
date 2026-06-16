// Mashed RE - RwMatrixRotateInner: Rodrigues rotation-matrix builder + combine.
//
// 0x004c4a50  FUN_004c4a50  WS-A2 follow-up (standalone wiring for RwMatrixRotate)
//
// Verbatim from Ghidra (pool2, read-only, 2026-06-16). Body 0x004c4a50..0x004c4d1c.
// Called by RwMatrixRotate (0x004c4d20) after deg->rad + axis-normalize + sin/cos.
// Shape: float* FUN_004c4a50(float* matrix, const float* axis_n, float one_minus_cos,
//                            float sin_a, int mode).
//
// Builds the Rodrigues 3x3 R(axis, angle) into a local matrix `m` (RwMatrix float[16]
// column layout: rows of {right,up,at,pos} with a 4th pad each; m[3]=flags):
//   m[0]=1-(1-x²)(1-c)            m[1]=z·s+y·x·(1-c)     m[2]=z·x·(1-c)-y·s   m[3]=flags(3)
//   m[4]=y·x·(1-c)-z·s            m[5]=1-(1-y²)(1-c)     m[6]=s·x+y·z·(1-c)   m[7]=PAD(uninit)
//   m[8]=y·s+z·x·(1-c)           m[9]=y·z·(1-c)-s·x     m[10]=1-(1-z²)(1-c)  m[11]=PAD(uninit)
//   m[12]=0  m[13]=0  m[14]=0     m[15]=PAD(uninit)
// where x,y,z=axis_n, c=cos, s=sin, (1-c)=one_minus_cos. The original leaves the pad
// slots m[7]/m[11]/m[15] unwritten (genuinely uninitialized stack) — RenderWare ignores
// matrix pad; we leave them 0 here (don't-care, excluded from diffs).
//
// Combine modes (param_5):
//   0 rwCOMBINEREPLACE  : copy R -> matrix.  PURE — no device dependency.
//   1 rwCOMBINEPRECONCAT: if matrix is flagged identity, copy R; else device matrix-mult
//                         deviceMul(temp, R, matrix); temp[3] = matrix.flags & 3; copy temp.
//   2 rwCOMBINEPOSTCONCAT: same but deviceMul(temp, matrix, R).
//   else                : error 0x80000003 "Invalid_combination_type" via FUN_004d7ff0/
//                         FUN_004d8480; return NULL.
// The identity-flag word and the matrix-mult method live in the RW device table at
// DAT_007d4028 + DAT_007d3ff8 (+4 = identity mask word, +8 = mult method). Modes 1/2 are
// thus device-state dependent (live in the dev .asi; in the standalone they require RW
// device init — WS-E). Mode 0 (a fresh rotation build) is fully self-contained.
//
// BIT-IDENTITY: the Rodrigues 3x3 build is an inline __asm verbatim transcription of the
// original FPU stream (0x004c4a60..0x004c4b95). A plain C++ float port is NOT bit-identical
// here: with this function's FPU register pressure MSVC spills a sub-expression
// ((1-y²)·omc for the R11 diagonal) to an f32 temp, rounding it to 24-bit, where the
// original keeps it at x87 register width — a 1-ULP divergence on symmetric axes (proven by
// diff idx=3). The asm uses only RELATIVE operands (axis pointer, omc/sin locals, a local
// 1.0f, and the local matrix), so it is bit-identical AND standalone-safe (no MASHED RVAs).
// Six intermediates (t84/t8c/t90/t98/ta4/ta0) are f32-stored exactly as the original does.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

static constexpr std::uintptr_t kDevTableBase  = 0x007d4028u;  // DAT_007d4028
static constexpr std::uintptr_t kRwGlobalsBase = 0x007d3ff8u;  // DAT_007d3ff8
static constexpr std::uint32_t  kIdentityMask  = 0x20000u;     // rwMATRIXINTERNALIDENTITY

typedef int  (__cdecl *RwErrPassFn)(int, const char*);
typedef void (__cdecl *RwErrRecordFn)(int*);
static constexpr std::uintptr_t kFUN_004d7ff0 = 0x004d7ff0u;
static constexpr std::uintptr_t kFUN_004d8480 = 0x004d8480u;
static constexpr std::uintptr_t kInvalidComboStr = 0x0061811cu;  // "Invalid_combination_type"

typedef void (__cdecl *RwMatrixMulFn)(float* out, const float* a, const float* b);

static inline void copy16(float* dst, const float* src) { std::memcpy(dst, src, 16 * sizeof(float)); }

// 0x004c4a50
extern "C" __declspec(dllexport)
float* __cdecl RwMatrixRotateInner(float* matrix, const float* axis_n,
                                   float one_minus_cos, float sin_a, int mode)
{
    const float kOne = 1.0f;                  // local 1.0f (standalone-safe; == DAT_005cc320)
    float omc = one_minus_cos, s = sin_a;
    const float* axisPtr = axis_n;
    float t84, t8c, t90, t98, ta4, ta0;       // the six f32-stored intermediates (as in orig)
    float m[16] = {0};

    // Rodrigues 3x3 build — verbatim x87 transcription of 0x004c4a60..0x004c4b95. Operands
    // are all relative (axisPtr, omc/s/kOne locals, the m local) -> bit-identical + portable.
    __asm {
        mov   eax, axisPtr
        lea   edx, m
        fld   dword ptr [eax]              // x
        fld   st(0)
        fmulp st(1), st(0)                 // x*x
        fsubr dword ptr kOne               // 1 - x²
        fld   dword ptr [eax+4]            // y
        fld   st(0)
        fmulp st(1), st(0)                 // y*y
        fsubr dword ptr kOne               // 1 - y²     (st1 = 1-x²)
        fld   dword ptr [eax+8]            // z
        fld   st(0)
        fmul  st(0), st(1)                 // z*z
        fsubr dword ptr kOne               // 1 - z²
        fstp  dword ptr t84                // t84 = 1-z²
        fstp  st(0)                        // pop z
        fxch  st(1)                        // st0=1-x², st1=1-y²
        fmul  dword ptr omc                // (1-x²)·omc
        fstp  dword ptr t8c                // t8c
        fmul  dword ptr omc                // (1-y²)·omc  (kept extended -> R11)
        fld   dword ptr t84                // 1-z²
        fmul  dword ptr omc                // (1-z²)·omc
        fld   dword ptr [eax+4]            // y
        fmul  dword ptr [eax+8]            // y·z
        fld   dword ptr [eax+8]            // z
        fmul  dword ptr [eax]              // z·x
        fld   dword ptr [eax+4]            // y
        fmul  dword ptr [eax]              // y·x
        fstp  dword ptr t90                // t90 = y·x
        fxch  st(1)                        // st0=y·z, st1=z·x
        fmul  dword ptr omc                // y·z·omc
        fstp  dword ptr t98                // t98
        fmul  dword ptr omc                // z·x·omc (fVar3, kept extended)
        fld   dword ptr t90                // y·x
        fmul  dword ptr omc                // y·x·omc
        fld   dword ptr s                  // sin
        fmul  dword ptr [eax]              // sin·x
        fstp  dword ptr ta4                // ta4
        fld   dword ptr [eax+4]            // y
        fmul  dword ptr s                  // y·sin
        fstp  dword ptr ta0                // ta0
        fld   dword ptr [eax+8]            // z
        fmul  dword ptr s                  // z·sin
        fld   dword ptr kOne               // 1.0
        fsub  dword ptr t8c                // 1 - t8c = R00
        fstp  dword ptr [edx]              // m[0]
        fld   st(0)                        // dup z·sin
        fadd  st(0), st(2)                 // z·sin + y·x·omc = R01
        fstp  dword ptr [edx+4]            // m[1]
        fld   st(2)                        // z·x·omc
        fsub  dword ptr ta0                // z·x·omc - ta0 = R02
        fstp  dword ptr [edx+8]            // m[2]
        fxch  st(1)                        // st0=y·x·omc, st1=z·sin
        fsub  st(0), st(1)                 // y·x·omc - z·sin = R10
        fstp  dword ptr [edx+16]           // m[4]
        fstp  st(0)                        // pop z·sin
        fld   dword ptr kOne               // 1.0
        fsub  st(0), st(3)                 // 1 - (1-y²)·omc = R11
        fstp  dword ptr [edx+20]           // m[5]
        fld   dword ptr ta4                // sin·x
        fadd  dword ptr t98                // sin·x + y·z·omc = R12
        fstp  dword ptr [edx+24]           // m[6]
        fld   dword ptr ta0                // y·sin
        fadd  st(0), st(1)                 // y·sin + z·x·omc = R20
        fstp  dword ptr [edx+32]           // m[8]
        fstp  st(0)                        // pop z·x·omc
        fld   dword ptr t98                // y·z·omc
        fsub  dword ptr ta4                // y·z·omc - sin·x = R21
        fstp  dword ptr [edx+36]           // m[9]
        fld   dword ptr kOne               // 1.0
        fsub  st(0), st(1)                 // 1 - (1-z²)·omc = R22
        fstp  dword ptr [edx+40]           // m[10]
        fstp  st(0)                        // pop (1-z²)·omc
        fstp  st(0)                        // pop (1-y²)·omc
    }

    { std::uint32_t f = 0x3u; std::memcpy(&m[3], &f, 4); }   // flags (0x00000003)
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; // translation row
    // m[7]/m[11]/m[15] stay 0 (RwMatrix pad — original leaves them uninitialized; diff-excluded)

    if (mode == 0) {                          // rwCOMBINEREPLACE
        copy16(matrix, m);
        return matrix;
    }

    if (mode == 1 || mode == 2) {
        const std::uint32_t devBase  = *reinterpret_cast<const std::uint32_t*>(kDevTableBase)
                                     + *reinterpret_cast<const std::uint32_t*>(kRwGlobalsBase);
        const std::uint32_t identWord = *reinterpret_cast<const std::uint32_t*>(devBase + 4);

        std::uint32_t mflags;
        std::memcpy(&mflags, &matrix[3], 4);

        if ((mflags & identWord & kIdentityMask) != 0u) {
            // input matrix is flagged identity -> combine is just the rotation
            copy16(matrix, m);
            return matrix;
        }

        float temp[16];
        RwMatrixMulFn mul = *reinterpret_cast<RwMatrixMulFn*>(devBase + 8);
        if (mode == 1) mul(temp, m, matrix);   // preconcat:  R · matrix
        else           mul(temp, matrix, m);   // postconcat: matrix · R
        const std::uint32_t tf = mflags & 3u;
        std::memcpy(&temp[3], &tf, 4);
        copy16(matrix, temp);
        return matrix;
    }

    // Invalid combine type -> error 0x80000003, return NULL (matches original).
    int local[2];
    local[0] = 1;
    local[1] = reinterpret_cast<RwErrPassFn>(kFUN_004d7ff0)(
                   0x80000003, reinterpret_cast<const char*>(kInvalidComboStr));  // STUB S-3748
    reinterpret_cast<RwErrRecordFn>(kFUN_004d8480)(local);                        // STUB S-3749
    return nullptr;
}

RH_ScopedInstall(RwMatrixRotateInner, 0x004c4a50);
