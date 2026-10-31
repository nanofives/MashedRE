// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417cf0 FUN_00417cf0
/* h2 0x00417cf0 (Ghidra 161 B, real 171 B: Ghidra's size drops the `add esp,8 / mov eax,1 / pop ecx / ret` tail).
   Best: 100% BYTE-IDENTICAL (--bytes 171 B, relocations masked). Start (g2s) 48.5%.
   Steps: Ghidra's extraout_ECX at the FUN_0046d4a0 call is `car` (param 4) kept in ECX across both
   FUN_00417730 calls (0x00417cf1 load, 0x00417d73 push). That needs the compiler to know FUN_00417730
   (0x00417730, 12 B leaf, cdecl, no ECX write) leaves ECX alone: defining it in the same file BEFORE the
   caller, with EXTERNAL linkage, does it (stack args kept, push ecx reused). `static` instead turns it into
   a register-arg custom convention (mov eax,ecx / xor eax,eax, 74.5%). So this is intra-TU callee register
   knowledge with no /GL. Dead zone written as `if (d > hi || d < lo) { ...; return 1; } return 0;`
   (the `<= && >=` / early return 0 form gives jp / test ah,1). The stand-in callee definition of
   FUN_00417730 = VehicleRaceAngleGet in the port: when #included the port must supply it in the same TU
   ahead of this body (or drop the copy and accept the ECX reload). Class: none (exact). */
#ifndef MATCH_PORT
extern float DAT_0089a880[];
extern float DAT_005cc320;
extern float DAT_005cc33c;
extern float DAT_005cd0ec;
extern float DAT_005cd0fc;
int FUN_0046d4a0(int *out, int car);
#endif
float FUN_00417730(int car)
{
  return DAT_0089a880[car];
}

int FUN_00417cf0(int param_1, int param_2, float *out, int car)
{
  int rec;
  float d;

  d = FUN_00417730(car) - FUN_00417730(0);
  while (d > DAT_005cc320) d -= DAT_005cc320;
  while (d < DAT_005cc33c) d += DAT_005cc320;
  if (d > DAT_005cd0ec || d < DAT_005cd0fc) {
    FUN_0046d4a0(&rec, car);
    out[0] = *(float *)(rec + 0x30);
    out[1] = *(float *)(rec + 0x38);
    return 1;
  }
  return 0;
}
