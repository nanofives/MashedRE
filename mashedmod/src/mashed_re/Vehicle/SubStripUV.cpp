// Mashed RE — sub-strip quadrant UV coordinate generator (0x00482030).
//
// Zero-callee pure leaf (area-vehicle round 1). Disasm 0x00482030 (verbatim,
// from Mashed_pool4 via DisasmPC.java; see log/area-vehicle/disasm_482030.txt):
//
//   00482030  MOV  EAX,[ESP+8]                 ; param_2 (int)
//   00482034  FILD [ESP+8]
//   00482038  TEST EAX,EAX ; JGE +6
//   0048203c  FADD float ptr [0x005cc94c]      ; unsigned fixup when param_2<0
//   00482042  MOV  ECX,[ESP+0xc]               ; param_3 (uint)
//   00482046  FMUL float ptr [0x005cc35c]
//   0048204c  TEST ECX,ECX
//   0048204e  FILD [ESP+0xc]
//   00482052  JGE +6 ; FADD float ptr [0x005cc94c]   ; unsigned fixup when param_3<0
//   0048205a  SHL  EAX,2
//   0048205d  FDIVP                             ; (param_2f*[5cc35c]) / param_3f
//   0048205f  XOR  EDX,EDX ; DIV ECX            ; EAX = (uint)(param_2<<2) / param_3
//   00482063  TEST EAX,EAX ; MOV [ESP+8],EAX ; FILD [ESP+8]
//   0048206d  JGE +6 ; FADD float ptr [0x005cc94c]
//   00482075  CMP  EAX,3
//   00482078  FSUBP                             ; fVar2 = quotient_of_prev - (float)EAX
//   0048207a  JA   0x004820cc                   ; EAX>3 -> default (writes nothing)
//   0048207c  JMP  [EAX*4 + 0x004820d0]         ; switch(EAX) 0..3  (jump table)
//     case0 00482083: FSTP [out];    [out+4]=0.0
//     case1 00482091: FSTP [out+4];  [out]=1.0
//     case2 0048209f: FLD [0x005cc320]; FSUB st,st1; [out+4]=1.0; FSTP [out]; FSTP st0
//     case3 004820b7: FLD [0x005cc320]; FSUB st,st1; [out]=0.0;   FSTP [out+4]; (fall)
//     default 004820cc: FSTP st0; RET
//
// The indirect JMP through the image jump table at 0x004820d0 is replaced with an
// equivalent local branch ladder (EAX is already range-clamped 0..3 by the JA above).
// The x87 op *sequence* is transcribed verbatim so the two 32-bit floats stored to
// the out pointer are bit-identical to the original (a C reimpl would round
// differently — cf. float_vec3_lerp_out's verbatim-asm note).
//
// Constants (image data, present when injected):
//   0x005cc94c  unsigned-fixup addend (2^32 as float)
//   0x005cc35c  UV multiplier
//   0x005cc320  1.0-complement base for the mirrored quadrants
//
// Ref: re/analysis/skeleton_prep_render/ (C2 2026-06-02 note characterises it as the
//   sub-strip quadrant UV coordinate generator).
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// MATCH-S13: the body of 0x00482030 (SubStripQuadUV) moved to Match/MatchS13_00482030.cpp,
// which compiles the byte-matched source re/match/s13/00482030.c.
