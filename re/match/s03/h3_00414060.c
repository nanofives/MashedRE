// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414060 FUN_00414060
/* 0x00414060 (186 B). 100.0% byte-identical (--bytes). Converter was 46.2%: __fastcall and an arg on
   FUN_00413fa0 were wrong (the caller pushes nothing, push ecx is the float local). Last gap: four -1.0f
   stores as float constants each emit mov [m], imm32. The original loads 0xbf800000 into eax once, which
   cl does for int stores of the same constant, so DAT_0089a870..87c are declared int and get the bit
   pattern (port: they hold -1.0f as float). Class: none left. */
#ifndef MATCH_PORT
extern float DAT_0089a37c;
extern float DAT_005cc32c;
extern float DAT_005cd088;
extern float DAT_005d757c;
extern float DAT_005cc55c;
extern float DAT_0089a360;
extern int   DAT_0089a364;
extern int   DAT_0089a870;
extern int   DAT_0089a874;
extern int   DAT_0089a878;
extern int   DAT_0089a87c;
extern int   DAT_0089a384[];
int FUN_00413fa0(void);
int FUN_0042f6a0(void);
int FUN_0042fe80(void);
int FUN_00431d80(void);
#endif

void FUN_00414060(void)
{
  float f;

  f = (float)DAT_0089a384[FUN_00413fa0()] + DAT_0089a37c * DAT_005cc32c;
  if (FUN_0042f6a0() == 6)
    f = (float)FUN_00431d80() * DAT_005cd088;
  if (FUN_0042f6a0() == 10)
    f = (float)FUN_0042fe80() * DAT_005cd088;
  if (f < DAT_005d757c)
    f = DAT_005d757c;
  else if (f > DAT_005cc55c)
    f = DAT_005cc55c;
  DAT_0089a360 = f;
  DAT_0089a870 = (int)0xbf800000;
  DAT_0089a874 = (int)0xbf800000;
  DAT_0089a878 = (int)0xbf800000;
  DAT_0089a364 = -1;
  DAT_0089a87c = (int)0xbf800000;
}
