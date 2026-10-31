// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413bc0 FUN_00413bc0
/* h2 0x00413bc0 (173 B). Best: 100% BYTE-IDENTICAL (--bytes, relocations masked), first try. Start (g2s) 57.6%.
   Ghidra's unaff_ESI index + ECX out pointer = a `static` callee defined before its caller (/Ob1, address
   not taken), parameter order (out, idx) -> ECX = out, ESI = idx. StandInCaller_00413cb0 below is a
   stand-in caller (not the original 0x00413cb0, not matched): it only exists so the static callee gets
   that convention. Plain if/else with the {0,0,1,1} branch first, idx/4 written as signed division.
   Constants: 0x3e828f5c = 0.255f, 0x3f7eb852 = 0.995f, DAT_005cd060 cell size. Class: none (exact). */
#ifndef MATCH_PORT
extern float DAT_005cd060;
void UseUv(float *uv, int idx);
#endif
static void FUN_00413bc0(float *out, int idx)
{
  int c;

  if (idx < 0 || idx >= 5) {
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 1.0f;
    out[3] = 1.0f;
  } else {
    c = idx & 3;
    out[0] = (float)c * DAT_005cd060;
    out[1] = (float)(idx / 4) * DAT_005cd060;
    out[2] = (float)(c + 1) * DAT_005cd060;
    out[3] = (float)((idx + 4) / 4) * DAT_005cd060;
    if (idx == 3) {
      out[0] = 0.255f;
      out[1] = 0.255f;
      out[2] = 0.995f;
      out[3] = 0.995f;
    }
  }
}

#ifndef MATCH_PORT
/* stand-in caller (not matched): gives the static callee its ESI/ECX register convention */
void StandInCaller_00413cb0(int idx)
{
  float uv[4];
  FUN_00413bc0(uv, idx);
  UseUv(uv, idx);
}
#endif
