// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Index into the 0xec-byte records at 0x00639d80 (field +0x44), pointer result + 0x30.
typedef struct { int f00[17]; int h44; int f48[41]; } Rec639d80;   /* 0xec bytes */
extern Rec639d80 DAT_00639d80[];
char *FUN_0047d150(int h);

// MATCH: 0x00407600 FUN_00407600
char *FUN_00407600(int i)
{
  return FUN_0047d150(DAT_00639d80[i].h44) + 0x30;
}
