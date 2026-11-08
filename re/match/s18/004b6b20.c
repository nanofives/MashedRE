// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6b20 (h4): 71.4%, regs 91.4%, ops 97.1% (g2s did not compile). Register args (EAX = record, EBX = file position, ECX = size; Ghidra in_EAX/unaff_EBX): pilot finding 1, FUN_004b6b20 is a static function defined before a stand-in caller (MatchStandIn_004b6b20, not a matched function, the real caller is 0x004b6e39 with ECX = 0x20000). All six parameter orders give the same registers. Residual: after the FUN_004b67e0 read the original moves the byte count into EAX and forms end with lea ecx,[eax+ebx]; 9176 loads it into ECX and adds, which shifts the registers of the DAT_007d3e64/68 stores. Tried: pos+n / n+pos / s->pos forms, end = pos; end += n, store order swaps, int count, caller variants, flagscan (best /O2). The permuter reached 77.1% only by storing DAT_007d3e68 before DAT_007d3e64, which contradicts the original order (it reloads s->hdr after the DAT_007d3e64 store), so the natural order is kept. Class (b). */
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
typedef struct { char pad[0x74]; int base; } H_004b6b20;
typedef struct {
  char pad[0x64];
  unsigned char buf[0x20000];
  H_004b6b20 *hdr;
  int pos;
  int end;
  int cur;
} S_004b6b20;
MGLOBAL(int, DAT_007d3e64, 0x007d3e64);
MGLOBAL(int, DAT_007d3e68, 0x007d3e68);
MGLOBALA(unsigned char, DAT_008ad9c0, 0x20000, 0x008ad9c0);
void FUN_004b67e0(unsigned long p0, void *p1, unsigned int p2, unsigned long *p3);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

// MATCH: 0x004b6b20 FUN_004b6b20
static void FUN_004b6b20(S_004b6b20 *s, int pos, unsigned int size)
{
  unsigned long n;

  FUN_004b67e0(s->hdr->base + pos, s->buf, size, &n);
  s->pos = pos;
  s->end = n + pos;
  DAT_007d3e64 = s->hdr->base + pos;
  DAT_007d3e68 = s->hdr->base + s->end;
  memcpy(DAT_008ad9c0, s->buf, 0x20000);
}

/* stand-in caller (not a matched function): makes 9176 pass the record in EAX, the position in EBX and the size in ECX */
int MatchStandIn_004b6b20(S_004b6b20 *s, int pos, int len)
{
  while (len) {
    FUN_004b6b20(s, pos, 0x20000);
    len -= s->end;
    pos += 0x20000;
  }
  return pos;
}
