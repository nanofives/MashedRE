// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413bc0 FUN_00413bc0
// MATCH: 0x00413cb0 FUN_00413cb0
/* h2 0x00413cb0 (612 B) + its static callee 0x00413bc0 (173 B). */
typedef struct { float x, y, z; } V3;
typedef struct { float a; int b; int c; } Ent12;
#ifndef MATCH_PORT
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
extern float DAT_005cd060;
extern Ent12 DAT_0063bc60[4][5];
extern float DAT_007f100c;
extern int DAT_007f101c;
extern char DAT_0063bd50[];
int FUN_004671a0(int i);
char *FUN_00467210(int i);
int FUN_0046d4a0(int *out, int car);
float FUN_004c3ac0(V3 *v);
double FUN_004a2d30(double x, double y);
void FUN_004769a0(V3 *p);
void FUN_004769d0(float *s);
void FUN_004769f0(unsigned char *rgba);
void FUN_00476a30(float r);
void FUN_00476a40(float *uv);
void FUN_00476d00(char *p);
void FUN_00476df0(char *p);
#endif
static void FUN_00413bc0(float *out, int idx)
{
  int c;

  if (idx < 0 || idx >= 5) {
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 1.0f;
    out[3] = 1.0f;
  } else {
    c = idx & 3;
    out[0] = (float)c * DAT_005cd060;
    out[1] = (float)(idx / 4) * DAT_005cd060;
    out[2] = (float)(c + 1) * DAT_005cd060;
    out[3] = (float)((idx + 4) / 4) * DAT_005cd060;
    if (idx == 3) {
      out[0] = 0.255f;
      out[1] = 0.255f;
      out[2] = 0.995f;
      out[3] = 0.995f;
    }
  }
}

void FUN_00413cb0(void)
{
  unsigned char rgba[4];
  float rot;
  float sn;
  int rec;
  float len;
  float cs;
  float scale[2];
  V3 pos;
  V3 d;
  float uv[4];
  float *zoom;
  V3 *cp;
  float f;
  int car;
  int i;
  int j;
  Ent12 *e;
  float a;
  float pw;
  float w;

  car = 0;
  rgba[0] = 0xff;
  rgba[1] = 0xff;
  rgba[2] = 0xff;
  rgba[3] = 0x80;
  scale[0] = 0.5f;
  scale[1] = 0.5f;
  zoom = (float *)(FUN_004671a0(0) + 0x68);
  for (i = 0; i < 0x14; i += 5) {
    for (j = 0; j < 5; j++) {
      e = &DAT_0063bc60[0][j + i];
      if (e->a > 0.0f) {
        f = DAT_007f100c + DAT_007f100c;
        e->a = e->c != 0 ? f + e->a : e->a - f;
        if (e->a < 0.0f) {
          e->a = 0.0f;
        } else {
          if (e->a > 1.0f) e->a = 1.0f;
          rgba[3] = (unsigned char)(e->a * 128.0f);
          FUN_0046d4a0(&rec, car);
          pos = *(V3 *)(rec + 0x30);
          pos.y = pos.y + 0.3f;
          cp = (V3 *)(FUN_00467210(0) + 0x30);
          d.x = pos.x - cp->x;
          d.y = cp->y;
          d.y = pos.y - d.y;
          d.z = cp->z;
          d.z = pos.z - d.z;
          len = FUN_004c3ac0(&d);
          FUN_00413bc0(uv, j);
          a = DAT_007f101c * 0.06f;
          sn = (float)sin(a);
          cs = (float)cos(a);
          rot = sn * 0.62831855f - 0.78539819f;
          pw = (float)FUN_004a2d30(len, 0.5);
          if (j == 3) {
            f = (float)rgba[3];
            f = f + f;
            if (f > 255.0f) f = 255.0f;
            rgba[3] = (unsigned char)f;
            rot += 0.78539997f;
            pw = pw * 1.25f;
          }
          w = cs * sn * 0.3f;
          scale[0] = pw * *zoom * 0.25f + w;
          scale[1] = pw * *zoom * 0.25f + w;
          FUN_004769a0(&pos);
          FUN_004769f0(rgba);
          FUN_004769d0(scale);
          FUN_00476a30(rot);
          FUN_00476a40(uv);
          FUN_00476d00(DAT_0063bd50);
        }
      }
    }
    car++;
  }
  FUN_00476df0(DAT_0063bd50);
}
