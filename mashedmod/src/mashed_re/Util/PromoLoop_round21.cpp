// Mashed RE — promote-round round 21 (out-ptr survey: int_copy_outbuf).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0041f030  TriggerStructRead — ai; fn(i, out*) copies 4 dwords
//   0x0041f260  WorldMatrixCopy   — util; fn(i, out*) copies a 4x4 matrix
//
// Both bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12.
// Diffed via the existing int_copy_outbuf handler (fn(int idx, T* dst)).
//
// Analysis:
//   re/analysis/ai_update_d3/0x0041f030.md
//   re/analysis/random_rng_d2/0x0041f260.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TriggerStructRead  --  0x0041f030   (subsystem: ai)
//
// Original: FUN_0041f030 (0x2e bytes, 0x0041f030..0x0041f05e)
// Bytes (head): 8B 44 24 04 / 8B 4C 24 08 / 69 C0 AC 02 00 00 /
//               05 E0 D9 63 00 / 05 58 02 00 00 / 8B 10 / 89 11 / ...
//   (mov eax,[esp+4]=i; mov ecx,[esp+8]=out; imul eax,eax,0x2ac;
//    add eax,0x0063d9e0; add eax,0x258;  <- base 0x0063dc38 + i*0x2ac
//    then 4x: mov edx,[eax+k]; mov [ecx+k],edx)
// Signature: void FUN_0041f030(int param_1, undefined4* param_2)
//
// Copies 4 consecutive dwords from 0x0063dc38 + i*0x2ac into param_2[0..3]
// (the type-0x15 trigger struct, per the round-3 decomp transcript). Reads
// live trigger state -> scenario:'race'.
//
// Constants (cited from function body at 0x0041f030):
//   0x0063dc38 — trigger struct field base (= 0x0063d9e0 + 0x258)
//   0x2ac      — per-trigger stride
//
// Caller: FUN_00414c30 (ai obstacle-avoidance, C2). Leaf.
//
// Uncertainties (non-blocking):
//   U-1894: semantic of the 4-dword struct (data-semantic).
// ---------------------------------------------------------------------------

// MATCH-S04: the body of 0x0041f030 (TriggerStructRead) moved to Match/MatchS04_0041f030.cpp,
// which compiles the byte-matched source re/match/s04/0041f030.c.

// ---------------------------------------------------------------------------
// WorldMatrixCopy  --  0x0041f260   (subsystem: util)
//
// Original: FUN_0041f260 (42 bytes, 0x0041f260..0x0041f28a)
// Bytes (head): 8B 44 24 04 / 69 C0 AC 02 00 00 / 56 / 05 E0 D9 63 00 /
//               8B 80 6C 02 00 00 / 8B 70 04 / 57 / 8B 7C 24 10 / ...
//   (mov eax,[esp+4]=i; imul eax,eax,0x2ac; add eax,0x0063d9e0;
//    mov eax,[eax+0x26c];  <- p0 = *(0x0063dc4c + i*0x2ac)
//    mov esi,[eax+4];      <- sub = *(p0+4)
//    mov edi,[esp+0x10]=out;  then copy 16 dwords from esi+0x10)
// Signature: void FUN_0041f260(int param_1, undefined4* param_2)
//
// DOUBLE-DEREF: src = *(*(0x0063dc4c + i*0x2ac) + 4) + 0x10; copies the 16
// dwords (4x4 matrix) to param_2. The per-slot object pointers are NULL at
// menu -> scenario:'race' with in-bounds slots.
//
// Constants (cited from function body at 0x0041f260):
//   0x0063dc4c — per-slot object pointer array base (= 0x0063d9e0 + 0x26c)
//   0x2ac      — per-slot stride; +4 sub-object; +0x10 matrix data; 16 dwords
//
// Callers: FUN_00421d20 (util C2), FUN_0040d8f0 (gameplay C2), and others. Leaf.
// ---------------------------------------------------------------------------

// MATCH-S04: the body of 0x0041f260 (WorldMatrixCopy) moved to Match/MatchS04_0041f260.cpp,
// which compiles the byte-matched source re/match/s04/0041f260.c.
