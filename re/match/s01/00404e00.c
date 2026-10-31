// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Material-slot tables built by 0x00404830: an index per body (0x00639cc8, 0x28 entries) and per
// car x wheel (0x00636c08, 0x4e per car), -1 = none; the index selects a name pointer (0x005ea0b8 / 0x005ea0c8).
#ifndef MASHED_RE_PORT
extern int DAT_00639cc8[0x28];
extern int DAT_00636c08[0xc30];
extern char *PTR_s_DirtBody_005ea0b8[];
extern char *PTR_s_DirtAdWheel_005ea0c8[];
#endif

// 0x00404e00
// MATCH: 0x00404e00 FUN_00404e00
char *FUN_00404e00(int body)
{
  if (DAT_00639cc8[body] == -1) {
    return 0;
  }
  return PTR_s_DirtBody_005ea0b8[DAT_00639cc8[body]];
}

// 0x00404e20
// MATCH: 0x00404e20 FUN_00404e20
char *FUN_00404e20(int car, int wheel)
{
  if (DAT_00636c08[car * 0x4e + wheel] == -1) {
    return 0;
  }
  return PTR_s_DirtAdWheel_005ea0c8[DAT_00636c08[car * 0x4e + wheel]];
}
