// FLAGS: /O2 /GS /Ob1
/* 0x00423040 (556 B). Best 100.0% exact (regs 100, ops 100).
   Tried: Ghidra shape (81.6%: else-first order put the pressed branch first), then each key block
   written as if (!key) { clear } else { repeat logic } -> 100%. Float literals -4.0f / 0.25f are
   the constants at 0x005cca00 / 0x005cc564. Residual: none. */
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1a5c (*(float *)0x007f1a5cu)
#else
extern float DAT_007f1a5c;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1a58 (*(float *)0x007f1a58u)
#else
extern float DAT_007f1a58;
#endif
extern const float DAT_005cca00;   /* -4.0f */
extern const float DAT_005cc564;   /* 0.25f */
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1042 (*(char *)0x007f1042u)
#else
extern char DAT_007f1042;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1076 (*(char *)0x007f1076u)
#else
extern char DAT_007f1076;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1a54 (*(int *)0x007f1a54u)
#else
extern int  DAT_007f1a54;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007f1a64 (*(int *)0x007f1a64u)
#else
extern int  DAT_007f1a64;
#endif
#ifdef MATCH_PORT   /* PORT-TOP-06: the port reads them at their original addresses */
#define DAT_007f1a90 (*(unsigned char *)0x007f1a90u)
#define DAT_007f1044 (*(char *)0x007f1044u)
#define DAT_007f1045 (*(char *)0x007f1045u)
#define DAT_007f1046 (*(char *)0x007f1046u)
#define DAT_007f1047 (*(char *)0x007f1047u)
#define DAT_006440fc (*(int *)0x006440fcu)
#define DAT_00644108 (*(int *)0x00644108u)
#define DAT_0064410c (*(int *)0x0064410cu)
#define DAT_006440f4 (*(int *)0x006440f4u)
#define DAT_006440ec (*(int *)0x006440ecu)
#define DAT_00644104 (*(int *)0x00644104u)
#define DAT_006440f0 (*(int *)0x006440f0u)
#define DAT_006440f8 (*(int *)0x006440f8u)
#else
extern unsigned char DAT_007f1a90;
extern char DAT_007f1044, DAT_007f1045, DAT_007f1046, DAT_007f1047;
extern int DAT_006440fc, DAT_00644108;
extern int DAT_0064410c, DAT_006440f4;
extern int DAT_006440ec, DAT_00644104;
extern int DAT_006440f0, DAT_006440f8;
#endif
void FUN_00417450(int x, int y, unsigned char b);
void FUN_00417530(int x, int y);

// MATCH: 0x00423040 _FUN_00423040
void FUN_00423040(void)
{
    int x = -0x14 - (int)(DAT_007f1a5c * -4.0f);
    int y = -0xc - (int)(DAT_007f1a58 * -4.0f);
    if (DAT_007f1042 && DAT_007f1a54 == 0 && DAT_007f1a64 == 4)
        FUN_00417450(x, y, DAT_007f1a90 & 3);
    if (DAT_007f1076 && DAT_007f1a54 == 0 && DAT_007f1a64 == 4)
        FUN_00417530(x, y);

    if (!DAT_007f1044) { DAT_006440fc = 0; DAT_00644108 = 0; }
    else {
        if (DAT_00644108 == 0) DAT_007f1a5c -= 0.25f;
        if (DAT_006440fc == 0) { DAT_006440fc = 1; DAT_00644108 = 10; }
        else { DAT_006440fc = 2; if (DAT_00644108 == 0) DAT_00644108 = 1; else if (--DAT_00644108 < 0) DAT_00644108 = 0; }
    }

    if (!DAT_007f1045) { DAT_0064410c = 0; DAT_006440f4 = 0; }
    else {
        if (DAT_006440f4 == 0) DAT_007f1a5c += 0.25f;
        if (DAT_0064410c == 0) { DAT_0064410c = 1; DAT_006440f4 = 10; }
        else { DAT_0064410c = 2; if (DAT_006440f4 == 0) DAT_006440f4 = 1; else if (--DAT_006440f4 < 0) DAT_006440f4 = 0; }
    }

    if (!DAT_007f1046) { DAT_006440ec = 0; DAT_00644104 = 0; }
    else {
        if (DAT_00644104 == 0) DAT_007f1a58 += 0.25f;
        if (DAT_006440ec == 0) { DAT_006440ec = 1; DAT_00644104 = 10; }
        else { DAT_006440ec = 2; if (DAT_00644104 == 0) DAT_00644104 = 1; else if (--DAT_00644104 < 0) DAT_00644104 = 0; }
    }

    if (!DAT_007f1047) { DAT_006440f0 = 0; DAT_006440f8 = 0; }
    else {
        if (DAT_006440f8 == 0) DAT_007f1a58 -= 0.25f;
        if (DAT_006440f0 == 0) { DAT_006440f0 = 1; DAT_006440f8 = 10; }
        else { DAT_006440f0 = 2; if (DAT_006440f8 == 0) DAT_006440f8 = 1; else if (--DAT_006440f8 < 0) DAT_006440f8 = 0; }
    }
}
