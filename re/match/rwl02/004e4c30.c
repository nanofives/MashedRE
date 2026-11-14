// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// MATCH-RWT-S7: 100% byte-identical under rw9044 (L-131), relocheck OK. Levers: the type byte is read as (tf >> 16) & 0xff of a
// uint field (xor ecx,ecx / mov cl,[esp+0x2a] at 0x004e4c9e / 0x004e4ca0; a uchar field read gives a dword load + and and pads the frame by 4),
// size is a plain local that c2 9044 places in the dead stream parameter slot (lea ecx,[esp+0x28] at 0x004e4c37, mov ebx,[esp+0x2c] at 0x004e4c3c), early returns.
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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

void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
typedef struct { undefined4 code; undefined4 data; } Err;
typedef struct { undefined4 v[4]; float angle; uint tf; } Chunk;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined4, DAT_0061862c, 0x0061862c);
float FUN_004c3b30(float p0);
uint FUN_004cbd30(int p0, Chunk *p1, uint p2);
int FUN_004cc5e0(int p0, int p1, int *p2, uint *p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 *FUN_004d8480(Err *p0);
int FUN_004e1b60(undefined4 *p0, int p1, int p2);
int FUN_004e4dd0(int p0);// MATCH: 0x004e4c30 RpLightStreamRead
int RpLightStreamRead(int stream)
{
  int size;
  uint version;
  int light;

  if (!FUN_004cc5e0(stream, 1, &size, &version)) return 0;
  if ((version >= 0x35000) && (version <= 0x37002)) {
    Chunk ck;
    memset(&ck, 0, sizeof(ck));
    if (FUN_004cbd30(stream, &ck, size) != size) return 0;
    light = FUN_004e4dd0((ck.tf >> 16) & 0xff);
    if (!light) return 0;
    *(undefined4 *)(light + 0x14) = ck.v[0];
    *(undefined4 *)(light + 0x18) = ck.v[1];
    *(undefined4 *)(light + 0x1c) = ck.v[2];
    *(undefined4 *)(light + 0x20) = ck.v[3];
    if (version < 0x30300) {
      float a = DAT_005cc320 / (ck.angle * ck.angle + DAT_005cc320);
      *(float *)(light + 0x28) = -FUN_004c3b30(a);
    }
    else {
      *(float *)(light + 0x28) = ck.angle;
    }
    if ((*(float *)(light + 0x18) == *(float *)(light + 0x1c)) &&
        (*(float *)(light + 0x18) == *(float *)(light + 0x20))) {
      *(uchar *)(light + 3) = 1;
    }
    else {
      *(uchar *)(light + 3) = 0;
    }
    *(uchar *)(light + 2) = (uchar)ck.tf;
    if (!FUN_004e1b60(&DAT_0061862c, stream, light)) return 0;
    return light;
  }
  else {
    Err err;
    err.code = 2;
    err.data = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    return 0;
  }
}