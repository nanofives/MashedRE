/* a7 0x00422470 (94 B): best 48.6% exact (regs 57.1, ops 74.3).
   Body matches instruction for instruction except around the two calls to FUN_004223f0: floor (0x004a3090),
   _ftol (0x004a2c48), the unsigned-int -> float fix-up (fadd 2^32 @0x005cc94c), x - i, and the
   right-to-left argument order into FUN_00422440(a, b, t) (cosine lerp) all come out identical.
   Residual: (c) FUN_004223f0 takes its two arguments in EAX (seed) and ECX (i), has no stack arguments,
   and the caller keeps i live in EDX across the first call (it relies on the callee not touching EDX).
   That is an interprocedural custom convention (LTCG / /GL); C at the object level cannot express it, so
   ours pushes two stack args per call and needs an extra callee-saved register (push/pop edi).
   Caller 0x0041f7fb pushes (x, 0xff): the stack signature below is confirmed.
   Tried: g2s raw did not compile (C2708 on the register-convention prototype); no alternative spelling
   moves the call sequence, since the difference is the callee's convention. */
// FLAGS: /O2 /GS /Ob1
double floor(double);
float FUN_004223f0(unsigned int seed, unsigned int i);   /* real convention: EAX = seed, ECX = i */
float FUN_00422440(float a, float b, float t);

// MATCH: 0x00422470 _FUN_00422470
float FUN_00422470(float x, unsigned int seed)
{
    unsigned int i = (int)floor(x);
    return FUN_00422440(FUN_004223f0(seed, i), FUN_004223f0(seed, i + 1), x - i);
}
