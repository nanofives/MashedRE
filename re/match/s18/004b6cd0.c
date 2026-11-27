// FLAGS: /O2 /GS /Ob1
/* RACE-PIZ-DEVICE 0x004b6cd0: piz: device method slot +0x30 (read): DAT_008ad868 += 1; 0 when n = 0; n clamped to ent->len; copies from the record window +0x64 (+0x20068..+0x2006c, counters DAT_008ad878 / 87c), else from the shared window DAT_008ad9c0 (DAT_007d3e64..68, counters DAT_008ad880 / 884), else refills through 0x004b6b20 in 0x20000 steps from the 0x800-aligned position (0x004b6b90, counter DAT_008ad888). The fast paths return the unclamped n (EDX, 0x004b6d88 / 0x004b6dfd), the refill path the bytes copied (0x004b6ebb). */
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
typedef struct { char name[0x74]; unsigned int base; unsigned int len; int pad7c; } PizEnt_004b6cd0;
typedef struct {
  int f0;
  int f4;
  int f8;
  char pad0c[0x64 - 0xc];
  unsigned char buf[0x20000];
  PizEnt_004b6cd0 *ent;
  unsigned int wstart;
  unsigned int wend;
  unsigned int cur;
  int f20074;
} PizFile_004b6cd0;
MGLOBAL(int, DAT_008ad868, 0x008ad868);
MGLOBAL(int, DAT_008ad878, 0x008ad878);
MGLOBAL(unsigned int, DAT_008ad87c, 0x008ad87c);
MGLOBAL(int, DAT_008ad880, 0x008ad880);
MGLOBAL(unsigned int, DAT_008ad884, 0x008ad884);
MGLOBAL(int, DAT_008ad888, 0x008ad888);
MGLOBAL(unsigned int, DAT_007d3e64, 0x007d3e64);
MGLOBAL(unsigned int, DAT_007d3e68, 0x007d3e68);
MGLOBALA(unsigned char, DAT_008ad9c0, 0x20000, 0x008ad9c0);
void FUN_004b67e0(unsigned long p0, void *p1, unsigned int p2, unsigned long *p3);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

/* 0x004b6b00 (re/match/s18/004b6b00.c, C4): static register-arg helper, ECX = destination, EAX = value */
static void FUN_004b6b00(int *param_1, int param_2)
{
  *param_1 = param_2;
}

/* 0x004b6b20 (re/match/s18/004b6b20.c, C2 71.4%): static register-arg helper, EAX = record, EBX = position, ECX = size */
static void FUN_004b6b20(PizFile_004b6cd0 *s, int pos, unsigned int size)
{
  unsigned long n;

  FUN_004b67e0(s->ent->base + pos, s->buf, size, &n);
  s->wstart = pos;
  s->wend = n + pos;
  DAT_007d3e64 = s->ent->base + pos;
  DAT_007d3e68 = s->ent->base + s->wend;
  memcpy(DAT_008ad9c0, s->buf, 0x20000);
}

/* 0x004b6b90 (re/match/s18/004b6b90.c, C4): static register-arg helper, EAX = dividend, ECX = divisor */
static int FUN_004b6b90(int param_1, int param_2)
{
  return param_1 / param_2 * param_2;
}

// MATCH: 0x004b6cd0 FUN_004b6cd0
unsigned int FUN_004b6cd0(PizFile_004b6cd0 *param_1, unsigned char *param_2, unsigned int param_3)
{
  unsigned int n;
  unsigned int cur;
  unsigned int end;
  unsigned int base;
  int off;
  int pos;
  unsigned int chunk;
  unsigned int total;
  unsigned char *src;

  DAT_008ad868++;
  if (param_3 == 0) {
    return 0;
  }
  n = param_3;
  cur = param_1->cur;
  total = 0;
  if (param_3 > param_1->ent->len) {
    n = param_1->ent->len;
  }
  end = cur + n;
  if (cur >= param_1->wstart && end <= param_1->wend) {
    memcpy(param_2, param_1->buf + (cur - param_1->wstart), n);
    param_1->cur += n;
    FUN_004b6b00((int *)&param_1->f8, param_1->cur);
    DAT_008ad878++;
    DAT_008ad87c += n;
    return param_3;
  }
  base = param_1->ent->base;
  if (base + cur >= DAT_007d3e64 && base + end <= DAT_007d3e68) {
    memcpy(param_2, DAT_008ad9c0 + (base - DAT_007d3e64) + cur, n);
    param_1->cur += n;
    FUN_004b6b00((int *)&param_1->f8, param_1->cur);
    DAT_008ad880++;
    DAT_008ad884 += n;
    return param_3;
  }
  DAT_008ad888++;
  pos = FUN_004b6b90(cur, 0x800);
  off = cur - pos;
  src = param_1->buf + off;
  while (n != 0) {
    FUN_004b6b20(param_1, pos, 0x20000);
    if (end > param_1->wend) {
      chunk = 0x20000 - off;
    }
    else {
      chunk = n;
    }
    memcpy(param_2, src, chunk);
    param_2 += chunk;
    total += chunk;
    n -= chunk;
    off = 0;
    src = param_1->buf;
    pos = FUN_004b6b90(cur + total, 0x800);
  }
  param_1->cur += total;
  FUN_004b6b00((int *)&param_1->f8, param_1->cur);
  return total;
}
