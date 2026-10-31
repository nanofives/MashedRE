// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x004041c0 (343 B) 100% (g2s 27.7%).
   - Every _DAT_005cc3xx/5cc5xx/5cc7xx and DAT_005d757c here is an .rdata float LITERAL (2.0, 0.001, 0.625, 0.5,
     1.0, 120.0, 60.0, 3.0, 0.0), not a global. As externs the compiler reorders y+global (95.2% best).
   - `(p.y + 3.0f) + 60.0f` with explicit parentheses keeps two fadds; without them 9176 folds 3+60 (96.4%).
   - text colour {5,0xd8,0,200} assigned before shadow {0,0,0,255}, both RwRGBA by value.
   - y is the second float of an 8-byte local: a lone float gives frame 0xc instead of 0x10.
   - `x > c` forms for the test ah,0x41/jne branches, the `= 0.0f` else-branch last. */
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
extern float DAT_00636b80;
extern float DAT_007f100c;
float FUN_00417730(int p0);
void FUN_00427e00(int p0, float p1, float p2, RwRGBA p3, float p4, int p5);
// MATCH: 0x004041c0 FUN_004041c0
void FUN_004041c0(void)
{
  float t;
  struct { float x, y; } p;
  RwRGBA text;
  RwRGBA shadow;

  p.y = 60.0f;
  text.red = 5; text.green = 0xd8; text.blue = 0; text.alpha = 200;
  shadow.red = 0; shadow.green = 0; shadow.blue = 0; shadow.alpha = 0xff;
  if (FUN_00417730(0) > 2.0f) {
    if (DAT_00636b80 == 0.0f) {
      DAT_00636b80 = 3.0f;
    }
    DAT_00636b80 = DAT_00636b80 - DAT_007f100c;
    if (DAT_00636b80 < 0.0f) {
      DAT_00636b80 = 0.001f;
      return;
    }
    if (DAT_00636b80 > 0.001f) {
      if (DAT_00636b80 * 0.625f < 0.5f) {
        t = 1.0f - DAT_00636b80 * 0.625f;
        p.y = (1.0f - (t + t)) * 120.0f + 60.0f;
      }
      FUN_00427e00(0x228, 323.0f, (p.y + 3.0f) + 60.0f, shadow, 0.9f, 2);
      FUN_00427e00(0x228, 320.0f, p.y + 60.0f, text, 0.9f, 2);
    }
  } else {
    DAT_00636b80 = 0.0f;
  }
}
