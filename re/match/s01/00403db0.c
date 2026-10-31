// FLAGS: /O2 /GS /Ob1
/* a2 0x00403db0 (278 B). Best: 100%.
   g2s 18.8%: colours as int immediates. Two RGBA structs by value (as 0x00403d30); white set before
   black picks al=0xff / cl=0 (black first: 94.2%). */
typedef struct { unsigned char r, g, b, a; } RwRGBA;
void FUN_00427ad0(int p0, float p1, float p2, float p3, float p4, RwRGBA p5, float p6);

// MATCH: 0x00403db0 FUN_00403db0
void FUN_00403db0(void)
{
  RwRGBA black;
  RwRGBA white;
  white.r = 255; white.g = 255; white.b = 255; white.a = 200;
  black.r = 0; black.g = 0; black.b = 0; black.a = 255;
  FUN_00427ad0(0xef, 123.0f, 83.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0xef, 120.0f, 80.0f, 400.0f, 80.0f, white, 0.8f);
  FUN_00427ad0(0x22d, 123.0f, 113.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0x22d, 120.0f, 110.0f, 400.0f, 80.0f, white, 0.8f);
  FUN_00427ad0(0x22e, 123.0f, 143.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0x22e, 120.0f, 140.0f, 400.0f, 80.0f, white, 0.8f);
}
