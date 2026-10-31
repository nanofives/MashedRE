// FLAGS: /O2 /GS /Ob1
/* a2 0x00403c60 (207 B). Best: 100%.
   g2s 17.0%: colours as int immediates. Two RGBA structs by value, same shape as 0x00403d30. */
typedef struct { unsigned char r, g, b, a; } RwRGBA;
void FUN_00427ad0(int p0, float p1, float p2, float p3, float p4, RwRGBA p5, float p6);

// MATCH: 0x00403c60 FUN_00403c60
void FUN_00403c60(void)
{
  RwRGBA black;
  RwRGBA yellow;
  yellow.r = 216; yellow.g = 216;
  black.r = 0; black.g = 0; black.b = 0; black.a = 255;
  yellow.b = 0; yellow.a = 200;
  FUN_00427ad0(0x22a, 123.0f, 83.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0x22a, 120.0f, 80.0f, 400.0f, 80.0f, yellow, 0.8f);
  FUN_00427ad0(0x22b, 123.0f, 143.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0x22b, 120.0f, 140.0f, 400.0f, 80.0f, yellow, 0.8f);
}
