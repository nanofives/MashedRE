// FLAGS: /O2 /GS /Ob1
// h11: 86.5% (regs 96.5%, ops 97.6%). Hand from decomp (raw did not compile). DAT_006ce818 is file-static (pilot finding 2: its load is scheduled before the float stores through the returned pointer).
/* MATCH-S13 0x00481e00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { float x; float y; float z; } V3;
#ifndef __cplusplus
static unsigned int DAT_006ce818;
#else
MGLOBAL(unsigned int, DAT_006ce818, 0x006ce818);
#endif
MGLOBALA(V3, DAT_006ce278, 120, 0x006ce278);
MGLOBAL(double, DAT_005cf240, 0x005cf240);
MGLOBAL(float, DAT_005cf23c, 0x005cf23c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ce2f4, 0x005ce2f4);
MGLOBAL(float, DAT_005cd2c0, 0x005cd2c0);
MGLOBAL(float, DAT_005cf238, 0x005cf238);
MGLOBAL(float, DAT_005cd0ec, 0x005cd0ec);
double sin(double);
double cos(double);
double sqrt(double);
#pragma intrinsic(sin, cos, sqrt)
float *FUN_0055c000(int p0, int p1, V3 *p2, float *p3);
int FUN_0057ca30(unsigned int p0, float *p1, int p2, int p3);
// MATCH: 0x00481e00 FUN_00481e00
int FUN_00481e00(int param_1, float param_2, int param_3)
{
  float step;
  float s;
  float c;
  float ratio;
  float limit;
  float phase;
  float elev;
  float az;
  float as;
  float scale;
  float out_local[360];
  float *out;
  float *v;
  unsigned int i;
  short kind;
  V3 *dir;

  if (DAT_006ce818 == 0) {
    step = DAT_005cf23c / (float)sqrt(DAT_005cf240);
    phase = DAT_005d757c;
    for (elev = step; elev < DAT_005ce2f4; elev += step + step) {
      s = (float)sin(elev);
      c = (float)cos(elev);
      as = s >= DAT_005d757c ? s : -s;
      ratio = step / as;
      az = phase;
      limit = DAT_005cd2c0 - ratio;
      for (; az < limit; az += ratio + ratio) {
        if (DAT_006ce818 >= 0x78) break;
        DAT_006ce278[DAT_006ce818].x = (float)cos(az) * s;
        DAT_006ce278[DAT_006ce818].y = (float)sin(az) * s;
        DAT_006ce278[DAT_006ce818].z = c;
        DAT_006ce818++;
      }
      phase = step - phase;
    }
  }
  scale = (*(float *)(param_1 + 0x4c)) * (param_2);
  kind = **(short **)(param_1 + 0x5c);
  if (kind == 1) {
    param_2 *= DAT_005cf238;
  }
  if (**(short **)(param_1 + 0x5c) == 4 && scale < DAT_005cd0ec) {
    scale = 0.005f;
  }
  out = out_local;
  dir = DAT_006ce278;
  for (i = 0; i < DAT_006ce818; i++) {
    v = FUN_0055c000(param_1, param_3, dir, out);
    if (v == 0) {
      return 0;
    }
    v[0] = v[0] * param_2;
    v[1] = v[1] * param_2;
    v[2] = v[2] * param_2;
    v[0] = scale * dir->x + v[0];
    v[1] = (dir->y) * (scale) + v[1];
    dir++;
    v[2] = (dir[-1].z) * (scale) + v[2];
    out = v + 3;
  }
  return FUN_0057ca30(DAT_006ce818, out_local, 0, 0);
}
