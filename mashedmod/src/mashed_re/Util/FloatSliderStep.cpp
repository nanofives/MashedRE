// Mashed RE - clamped float slider/fader step (one global, one direction code).
// Original: 0x0045db50  FUN_0045db50  util  C2 -> C3
//
// void FUN_0045db50(int dir /*param_1, [esp+4]*/):
// Steps the slider value at DAT_0068fc8c by a direction code, then clamps it to [0,1]:
//   dir 0 -> DAT_0068fc8c -= step1 (0x005cc9c0)   then clamp
//   dir 1 -> DAT_0068fc8c += step1 (0x005cc9c0)   then clamp
//   dir 2 -> DAT_0068fc8c  = 0.0                  (jumps straight to the zero-store)
//   dir 3 -> DAT_0068fc8c -= step2 (0x005cd0ec)   then clamp  (larger step)
//   dir >3 -> no step, but the current value is still re-clamped to [0,1]
// Clamp: if value > 1.0f (0x005cc320) set 1.0f; else if value < 0.0f (0x005d757c) set 0.0f.
// The original dispatches through an embedded jump table at 0x0045dbcc
//   ([0]=0x45db60 dec1, [1]=0x45db6e inc, [2]=0x45dbbf zero-store, [3]=0x45db7c dec2);
// this reimpl replaces it with an equivalent eax==0/1/2/(else 3) branch ladder, exactly as
// SubStripUV.cpp replaced its image jump table. case 2 targets the zero-store directly
// (0x45dbbf), matching the original — it does not run the clamp compares.
//
// GLOBALS: reads/writes DAT_0068fc8c (the slider, the sole mutable state); step1/step2 and
// the 0.0/1.0 clamp bounds are all .rdata constants, valid in both the .asi and the exe.
// PURE LEAF: no callees. The x87 fld/fsub/fadd/fstp and the fcomp+fnstsw+test clamp compares
// are transcribed verbatim so the stored f32 and the branch taken are bit-identical (a plain-C
// `>`/`<` clamp could diverge on the FCOM status-word edge cases the original tests literally).
// __cdecl single int arg: caller cleans -> plain RET.
#include "../Core/HookSystem.h"

// MATCH-S10: the body of 0x0045db50 (FloatSliderStep) moved to Match/MatchS10_0045db50.cpp,
// which compiles the byte-matched source re/match/s10/0045db50.c.
