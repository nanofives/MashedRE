// FLAGS: /O2 /GS /Ob1
/* a2 0x00403d30 (124 B). Best: 100%.
   g2s 9.7%: colour passed as an int immediate. The original builds two 4-byte RGBA structs on the
   stack and passes them by value. Store order needs white rgb, then black, then white.a. */
typedef struct { unsigned char r, g, b, a; } RwRGBA;
void FUN_00427ad0(int p0, float p1, float p2, float p3, float p4, RwRGBA p5, float p6);

// MATCH: 0x00403d30 FUN_00403d30
void FUN_00403d30(void)
{
  RwRGBA black;
  RwRGBA white;
  white.r = 255; white.g = 255; white.b = 255;
  black.r = 0; black.g = 0; black.b = 0; black.a = 255;
  white.a = 200;
  FUN_00427ad0(0x22f, 123.0f, 83.0f, 400.0f, 80.0f, black, 0.8f);
  FUN_00427ad0(0x22f, 120.0f, 80.0f, 400.0f, 80.0f, white, 0.8f);
}
