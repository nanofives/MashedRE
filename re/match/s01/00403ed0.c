// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x00403ed0 (206 B) 100% (g2s 17.0%). Two RwRGBA-shaped 4-byte colours passed BY VALUE
   (byte stores then a dword load+push). Assigning the white/alpha-200 colour BEFORE the black/alpha-255 one is
   what gives the original's mov al,0xff / xor cl,cl register pick (black-first: 92.5%). Callee cdecl. */
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
void FUN_00427ad0(int p0, float p1, float p2, float p3, float p4, RwRGBA p5, float p6);
// MATCH: 0x00403ed0 FUN_00403ed0
void FUN_00403ed0(void)
{
  RwRGBA shadow;
  RwRGBA text;
  text.red = 0xff; text.green = 0xff; text.blue = 0xff; text.alpha = 200; shadow.red = 0; shadow.green = 0; shadow.blue = 0; shadow.alpha = 0xff;
  FUN_00427ad0(0xf3, 123.0f, 83.0f, 400.0f, 80.0f, shadow, 0.8f);
  FUN_00427ad0(0xf3, 120.0f, 80.0f, 400.0f, 80.0f, text, 0.8f);
  FUN_00427ad0(0x230, 123.0f, 113.0f, 400.0f, 80.0f, shadow, 0.8f);
  FUN_00427ad0(0x230, 120.0f, 110.0f, 400.0f, 80.0f, text, 0.8f);
}
