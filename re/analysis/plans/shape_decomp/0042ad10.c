
/* [C1 2026-05-20] HUD-glyph slot initializer. Takes (param_1 unused, param_2 glyph-id, param_3 x,
   param_4 y, param_5… */

void __fastcall
FUN_0042ad10(undefined4 param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  int *in_EAX;
  
  (&DAT_00898ac0)[*in_EAX * 0xd] = param_2;
  (&DAT_00898acc)[*in_EAX * 0x34] = 0xff;
  (&DAT_00898acd)[*in_EAX * 0x34] = 0xff;
  (&DAT_00898ace)[*in_EAX * 0x34] = 0xff;
  (&DAT_00898acf)[*in_EAX * 0x34] = 0xff;
  (&DAT_00898ad4)[*in_EAX * 0xd] = 0x3f19999a;
  (&DAT_00898ad8)[*in_EAX * 0xd] = (float)param_3;
  (&DAT_00898adc)[*in_EAX * 0xd] = (float)param_4;
  (&DAT_00898ae4)[*in_EAX * 0xd] = param_5;
  return;
}

