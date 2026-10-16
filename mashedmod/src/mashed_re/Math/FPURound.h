// Mashed RE — C++ entry to the verbatim __ftol body (0x004a2c48, Math/FPURound.cpp).
//
// The original's __ftol takes its operand in ST0 and returns EDX:EAX, which no C declaration expresses.
// FtolVerbatim(v) loads v onto the x87 stack, runs the ported instruction stream FPURound_4a2c48 and returns the low 32 bits
// (EAX), which is what the original's callers keep (they truncate to int at the call site). The body truncates toward zero
// through FISTP + the residual correction, exactly as the original does; there is no second implementation of it.
#pragma once

int __cdecl FtolVerbatim(float v);
