// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f0dc0 (h1): best 79.6% (regs 79.6%, ops 92.6%, 108/108 ins) under /O2 /Ob1 /G5. Raw 42.6%.
   Stream writer: 12-byte header {+0, (u32)u16 +4, +8} through 0x004cc770, then per 12-byte entry at +0x10 an 8-byte
   {count, max(0, 0x004f3cb0(param_4, key))} record and, when *param_2 is 8 or 7 with bit 0x1000000 of +8 clear, the
   u16 data widened to u32 in 256-entry chunks. Every instruction matches; the only residual is block placement:
   the original puts the shared `return 0` exit right after the header write's test (jne over it), ours puts it at
   the end, so every branch displacement differs. An early `if (!write) return 0;` places the exit right but
   shrink-wraps ebx/ebp/edi past it and splits the exit (69.4%). Tried: goto fail at end, fail label before the
   loop, e/n initialised before the header write, while (n--), for-loop form, break; permuter 167 compiles, no gain.
   Class (b). */
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
typedef struct { unsigned short *data; unsigned int count; int key; } Ent_004f0dc0;
typedef struct { int a; unsigned short n; short pad; int c; int pad2; Ent_004f0dc0 e[1]; } Obj_004f0dc0;
typedef struct { int a; unsigned int n; int c; } Hdr_004f0dc0;
typedef struct { int count; int index; } EHdr_004f0dc0;
void *FUN_004cc770(void *stream, const void *buf, unsigned int len);
int FUN_004f3cb0(void *p0, int p1);
// MATCH: 0x004f0dc0 FUN_004f0dc0
void *FUN_004f0dc0(Obj_004f0dc0 *param_1, char *param_2, void *param_3, void *param_4)
{
  Hdr_004f0dc0 hdr;
  EHdr_004f0dc0 eh;
  unsigned int buf[256];
  Ent_004f0dc0 *e;
  unsigned int n;
  unsigned int left;
  unsigned int chunk;
  unsigned int i;
  unsigned short *src;

  hdr.a = param_1->a;
  hdr.n = param_1->n;
  hdr.c = param_1->c;
  if (FUN_004cc770(param_3, &hdr, sizeof(hdr))) {
  e = param_1->e;
  n = param_1->n;
  while (n) {
    n--;
    eh.count = e->count;
    eh.index = FUN_004f3cb0(param_4, e->key);
    if (eh.index < 0) {
      eh.index = 0;
    }
    if (!FUN_004cc770(param_3, &eh, sizeof(eh))) {
      return 0;
    }
    if ((*param_2 == 8 && !(*(unsigned int *)(param_2 + 8) & 0x1000000)) ||
        (*param_2 == 7 && !(*(unsigned int *)(param_2 + 8) & 0x1000000))) {
      src = e->data;
      for (left = e->count; left > 0; left -= chunk) {
        chunk = left;
        if (left >= 0x100) {
          chunk = 0x100;
        }
        for (i = 0; i < chunk; i++) {
          buf[i] = *src++;
        }
        if (!FUN_004cc770(param_3, buf, chunk * sizeof(unsigned int))) {
          return 0;
        }
      }
    }
    e++;
  }
  return param_3;
  }
  return 0;
}
