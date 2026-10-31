/* Main-session probe (2026-10-30): a7 left 0x00422470 at 48.6% as (c), callee 0x004223f0 takes EAX/ECX and the
   caller keeps EDX across the call. Here the callee is a `static` function defined in the same file, before the
   caller: cl 13.00.9176 then gives it a register convention of its own (no /GL involved).
   Constants: 4.656612873077393e-10 = the float at 0x005cd314 (1/2^31); 2^32 fix-up at 0x005cc94c is the
   compiler's unsigned->float conversion.
   Result: both functions BYTE-IDENTICAL. Needed: callee static + defined first (gives EAX/ECX and leaves EDX live
   across the call), parameter order (i, seed), t and b computed into locals, and no auto-inlining (/Ob1; under
   /O2 alone the static callee is inlined). a7's 48.6% (c) was source shape (TU layout), not /GL. */
// FLAGS: /O2 /GS /Ob1
double floor(double);
float FUN_00422440(float a, float b, float t);

// MATCH: 0x004223f0 _FUN_004223f0
static float FUN_004223f0(unsigned int i, unsigned int seed)
{
    unsigned int n;
    if (seed)
        i &= seed;
    n = (i << 13) ^ i;
    n = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return (float)n * 4.656612873077393e-10f;
}

// MATCH: 0x00422470 _FUN_00422470
float FUN_00422470(float x, unsigned int seed)
{
    unsigned int i = (int)floor(x);
    float t = x - i; float b = FUN_004223f0(i + 1, seed); return FUN_00422440(FUN_004223f0(i, seed), b, t);
}
