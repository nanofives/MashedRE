// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417750 FUN_00417750
/* 0x00417750 (91 B). 100.0% byte-identical (--bytes). Converter was 72.0% (__fastcall, an arg on
   FUN_00414220 that the caller 0x0040ddb0 does not pass). Last gap: `DAT_0089a420[FUN_00413fa0()]++`
   compiles to lea/mov/inc/mov; storing the index in a named int first gives the original's single
   `inc dword ptr [eax*4+0x89a420]`. Class: none left. */
#ifndef MATCH_PORT
extern float DAT_005d757c;
extern float DAT_0089a37c;
extern float DAT_0089a380;
extern int   DAT_0089a378;
extern int   DAT_0089a420[];
int  FUN_00413fa0(void);
void FUN_00414220(void);
int  FUN_004241b0(void);
#endif

void FUN_00417750(void)
{
  float n;
  int i;

  FUN_00414220();
  n = (float)FUN_004241b0();
  if (n != DAT_005d757c)
    DAT_0089a37c += DAT_0089a380 / n;
  DAT_0089a378++;
  DAT_0089a380 = 0.0f;
  i = FUN_00413fa0();
  DAT_0089a420[i]++;
}
