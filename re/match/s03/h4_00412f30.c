// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412f30 FUN_00412f30
/* h4 MATCH-S03 round 2. 0x00412f30 (2762 B): 83.9% (regs 83.8, ops 90.6), 799/801 ins. g2s start 30.3%.
   Shape: four 4-byte colour arrays initialised in the order A B C D, RwMatrix-shaped local (at at +0x20,
   pos at +0x30), loops written `for (k = 4; k != 0; k--)` with off/scale stepped at the end (the original
   counts EBX down), FUN_0046d400 takes the position as a V3 by value, FUN_00467210 returns a record base
   (+0x30 reads), pulse as one ternary expression (83.5 -> 83.9).
   Residuals: (1) the camera-relative vector: the original loads rec.x and spills it to a temp before each
   FUN_00467210 call; cl 9176 keeps &rec.x in EBP and loads after the call (tried array indexing, negated
   form, an explicit temp: 83.5-83.9). (2) scheduling of the three at.x/y/z scale stores between the 8 pushes
   of every FUN_00412cf0 call (pure interleave, same instructions). Not permuted (compile time).
   Class (b) unknown for (1), (a) candidate for (2) (instruction scheduling only). */
typedef struct {
  float x;
  float y;
  float z;
} V3;
typedef struct {
  V3 right;
  int flags;
  V3 up;
  int pad1;
  V3 at;
  int pad2;
  V3 pos;
  int pad3;
} RwMat;
typedef struct {
  int slot;
  int pad[3];
} CarSlotIdx;
typedef struct {
  char pad0[5];
  char vis;
  char pad1;
  char fire;
  char pad2[0x44];
} PwrSlot;
typedef struct {
  char b[0x2c];
} PartRec;
#ifndef MATCH_PORT
extern unsigned int DAT_007f101c;
extern int DAT_008a94a0;
extern int DAT_0063bc58;
extern int DAT_0063bc5c;
extern float DAT_005cc750;
extern float DAT_005cd05c;
extern float DAT_005cd058;
extern float DAT_005cc32c;
extern float DAT_005cc320;
extern float DAT_005f2a4c;
extern float DAT_005cd054;
extern float DAT_005cc33c;
extern float DAT_005cc9a0;
extern float DAT_005cd050;
extern float DAT_005cc99c;
extern int DAT_007f0fd0;
extern int DAT_007f0fd8;
extern int DAT_0063bc44[];
extern PwrSlot DAT_007f1038[];
extern CarSlotIdx DAT_007f1a14[];
extern char DAT_00803324[];
extern PartRec DAT_0089a8a0[];
extern PartRec DAT_0089bea0[];
int FUN_0040e340(void);
int FUN_0040e370(int car);
void FUN_00412cf0(PartRec *dst, RwMat *m, int p1, float scale, float fade, int flag, unsigned char *col, int *count);
void FUN_00412e30(PartRec *dst, RwMat *m, int p1, float scale, float fade, int flag, unsigned char *col, int *count);
void FUN_00413c70(int car, int a, int b);
void FUN_0041f090(int car, int *a, int *b);
void FUN_0041f0d0(int car, int *b);
int FUN_0041f1c0(int car, int id);
void FUN_0041f1e0(int car, int id, RwMat *m);
int FUN_00442c80(int car);
int FUN_00467210(int i);
float FUN_0046d400(V3 pos, char *p);
int FUN_0046d4a0(int *p0, int p1);
float FUN_004c3ac0(float *v);
#endif
void FUN_00412f30(int p1)
{
  unsigned char colA[4];
  unsigned char colB[4];
  unsigned char colC[4];
  unsigned char colD[4];
  int n;
  int i;
  int k;
  CarSlotIdx *p;
  int rec;
  int a;
  int b;
  int c;
  float pulse;
  float ground;
  float off;
  float scale;
  float v[3];
  RwMat m;

  colA[0] = 0xff;
  colA[1] = 0xff;
  colA[2] = 0xff;
  colA[3] = 0xff;
  colB[0] = 0xff;
  colB[1] = 0;
  colB[2] = 0;
  colB[3] = 0xff;
  colC[0] = 0xff;
  colC[1] = 0;
  colC[2] = 0;
  colC[3] = 0xff;
  colD[0] = 0;
  colD[1] = 0;
  colD[2] = 0xff;
  colD[3] = 0xff;
  n = FUN_0040e340();
  pulse = (float)(int)(DAT_007f101c & 0x1f);
  DAT_008a94a0 = 0;
  DAT_0063bc58 = 0;
  DAT_0063bc5c = 0;
  pulse = (pulse > DAT_005cc750 ? DAT_005cd05c - pulse : pulse) * DAT_005cd058;
  p = DAT_007f1a14;
  for (i = 0; i < n; i++, p++) {
    FUN_0046d4a0(&rec, i);
    v[0] = *(float *)(rec + 0x30) - *(float *)(FUN_00467210(0) + 0x30);
    v[1] = *(float *)(rec + 0x34) - *(float *)(FUN_00467210(0) + 0x34);
    v[2] = *(float *)(rec + 0x38) - *(float *)(FUN_00467210(0) + 0x38);
    DAT_005f2a4c = FUN_004c3ac0(v) * DAT_005cc32c - DAT_005cc320;
    if (DAT_005f2a4c > DAT_005cd054) {
      DAT_005f2a4c = -0.05f;
    }
    else if (DAT_005f2a4c < DAT_005cc33c) {
      DAT_005f2a4c = -1.0f;
    }
    ground = FUN_0046d400(*(V3 *)(rec + 0x30), &DAT_00803324);
    if (p->slot >= 0) {
      FUN_0041f090(i, 0, &a);
      FUN_0041f0d0(i, &b);
      if (DAT_007f1038[p->slot].vis != 0 && a == 0 && b == 0) {
        if (FUN_0041f1c0(i, 0x39) != 0) {
          FUN_0041f1e0(i, 0x39, &m);
          m.at.x = m.at.x * DAT_005cc33c;
          m.at.y = m.at.y * DAT_005cc33c;
          m.at.z = m.at.z * DAT_005cc33c;
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colB, &DAT_0063bc58);
        }
        if (FUN_0041f1c0(i, 0x3a) != 0) {
          FUN_0041f1e0(i, 0x3a, &m);
          m.at.x = m.at.x * DAT_005cc33c;
          m.at.y = m.at.y * DAT_005cc33c;
          m.at.z = m.at.z * DAT_005cc33c;
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colB, &DAT_0063bc58);
        }
        off = 0.0f;
        scale = 1.0f;
        for (k = 4; k != 0; k--) {
          if (FUN_0041f1c0(i, 0x39) != 0) {
            FUN_0041f1e0(i, 0x39, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            m.at.x = m.at.x * DAT_005cc33c;
            m.at.y = m.at.y * DAT_005cc33c;
            m.at.z = m.at.z * DAT_005cc33c;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colC, &DAT_0063bc5c);
          }
          if (FUN_0041f1c0(i, 0x3a) != 0) {
            FUN_0041f1e0(i, 0x3a, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            m.at.x = m.at.x * DAT_005cc33c;
            m.at.y = m.at.y * DAT_005cc33c;
            m.at.z = m.at.z * DAT_005cc33c;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colC, &DAT_0063bc5c);
          }
          off -= DAT_005cc9a0;
          scale -= DAT_005cd050;
        }
      }
    }
    if (DAT_007f0fd0 == 2) {
      if (i != DAT_007f0fd8 && FUN_0040e370(i) != 0) {
        if (FUN_0041f1c0(i, 0x2a) != 0) {
          FUN_0041f1e0(i, 0x2a, &m);
          m.pos.y = m.pos.y + DAT_005cc99c;
          FUN_00412e30(&DAT_0089bea0[DAT_0063bc58], &m, p1, pulse, -0.2f, 1, colD, &DAT_0063bc58);
        }
      }
    }
    else {
      FUN_00413c70(i, 0, (FUN_00442c80(i) != 0 && DAT_007f0fd0 != 7) ? 1 : 0);
    }
    if (DAT_0063bc44[i] != 0) {
      FUN_0041f090(i, 0, &c);
      if (c == 0) {
        if (FUN_0041f1c0(i, 0x19) != 0) {
          FUN_0041f1e0(i, 0x19, &m);
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colA, &DAT_0063bc58);
        }
        if (FUN_0041f1c0(i, 0x1a) != 0) {
          FUN_0041f1e0(i, 0x1a, &m);
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colA, &DAT_0063bc58);
        }
        if (FUN_0041f1c0(i, 0x1c) != 0) {
          FUN_0041f1e0(i, 0x1c, &m);
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colA, &DAT_0063bc58);
        }
        if (FUN_0041f1c0(i, 0x1b) != 0) {
          FUN_0041f1e0(i, 0x1b, &m);
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 1.0f, DAT_005f2a4c, 1, colA, &DAT_0063bc58);
        }
        off = 0.0f;
        scale = 1.0f;
        for (k = 4; k != 0; k--) {
          if (FUN_0041f1c0(i, 0x19) != 0) {
            FUN_0041f1e0(i, 0x19, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colA, &DAT_0063bc5c);
          }
          if (FUN_0041f1c0(i, 0x1a) != 0) {
            FUN_0041f1e0(i, 0x1a, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colA, &DAT_0063bc5c);
          }
          if (FUN_0041f1c0(i, 0x1c) != 0) {
            FUN_0041f1e0(i, 0x1c, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colA, &DAT_0063bc5c);
          }
          if (FUN_0041f1c0(i, 0x1b) != 0) {
            FUN_0041f1e0(i, 0x1b, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colA, &DAT_0063bc5c);
          }
          off -= DAT_005cc9a0;
          scale -= DAT_005cd050;
        }
      }
      FUN_0041f090(i, &a, 0);
      if (a == 0) {
        if (FUN_0041f1c0(i, 0x37) != 0) {
          FUN_0041f1e0(i, 0x37, &m);
          m.at.x = m.at.x * DAT_005cc33c;
          m.at.y = m.at.y * DAT_005cc33c;
          m.at.z = m.at.z * DAT_005cc33c;
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 0.25f, DAT_005f2a4c, 1, colC, &DAT_0063bc58);
        }
        if (FUN_0041f1c0(i, 0x38) != 0) {
          FUN_0041f1e0(i, 0x38, &m);
          m.at.x = m.at.x * DAT_005cc33c;
          m.at.y = m.at.y * DAT_005cc33c;
          m.at.z = m.at.z * DAT_005cc33c;
          FUN_00412cf0(&DAT_0089bea0[DAT_0063bc58], &m, p1, 0.25f, DAT_005f2a4c, 1, colC, &DAT_0063bc58);
        }
        off = 0.0f;
        scale = 1.0f;
        for (k = 8; k != 0; k--) {
          if (FUN_0041f1c0(i, 0x37) != 0) {
            FUN_0041f1e0(i, 0x37, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            m.at.x = m.at.x * DAT_005cc33c;
            m.at.y = m.at.y * DAT_005cc33c;
            m.at.z = m.at.z * DAT_005cc33c;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colC, &DAT_0063bc5c);
          }
          if (FUN_0041f1c0(i, 0x38) != 0) {
            FUN_0041f1e0(i, 0x38, &m);
            m.pos.y = (ground - (m.pos.y - ground)) + off;
            m.at.x = m.at.x * DAT_005cc33c;
            m.at.y = m.at.y * DAT_005cc33c;
            m.at.z = m.at.z * DAT_005cc33c;
            FUN_00412cf0(&DAT_0089a8a0[DAT_0063bc5c], &m, p1, scale, DAT_005f2a4c, 0, colC, &DAT_0063bc5c);
          }
          off -= DAT_005cc9a0;
          scale -= DAT_005cd050;
        }
      }
    }
  }
}
