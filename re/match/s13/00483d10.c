// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00483d10 (h12): 100% (regs 100%), first hand pass. Finds the 0x10b replay chunk with id param_2 in a
   memory block (13-step scan, i = 100 marks found), reads the 0x19c header twice through a memory stream
   (0x004cc230 open / 0x004cbd30 read / 0x004cc160 close), allocates via 0x00482930(len, res, 0), reads the frame
   array and links it into a ring via +0x1c. 'res' is uninitialised when the first open fails (the original reuses
   len's slot for it: kept as is). Format string 0x005cf260 is a 36-byte MGLOBALA. */
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
typedef struct RwMem483d10 { int *start; int length; } RwMem483d10;
typedef struct ReplayHdr483d10 {
  int type;          /* +0x000: 0x10b */
  int f4;
  int f8;
  float len;         /* +0x00c */
  int res;           /* +0x010 */
  int data;          /* +0x014 */
  int first;         /* +0x018 */
  int cur;           /* +0x01c */
  int last;          /* +0x020 */
  int pad[0x5b - 9];
  int count;         /* +0x16c */
  int tail[0x67 - 0x5c];
} ReplayHdr483d10;
typedef void (*Free483d10)(void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, s_loaded_replay_of_length__f__res__d_005cf260, 36, 0x005cf260);
int FUN_004a2cbd(const char *fmt, ...);
ReplayHdr483d10 *FUN_00482930(float len, int res, int p3);
int FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004cc160(int stream, RwMem483d10 *mem);
int FUN_004cc230(int type, int mode, RwMem483d10 *mem);
// MATCH: 0x00483d10 FUN_00483d10
ReplayHdr483d10 *FUN_00483d10(RwMem483d10 *param_1, int param_2)
{
  RwMem483d10 mem;
  float len;
  ReplayHdr483d10 hdr;
  int *p;
  int i;
  int s;
  int res;
  ReplayHdr483d10 *rep;
  int data;
  int n;

  rep = 0;
  mem = *param_1;
  for (i = 0; i < 13; i++) {
    p = mem.start;
    if (p[2] == param_2 && p[0] == 0x10b) {
      i = 100;
    } else {
      mem.start = (int *)((char *)p + p[1]);
      mem.length -= p[1];
    }
  }
  if (i < 100) {
    return 0;
  }
  s = FUN_004cc230(3, 1, &mem);
  if (s != 0) {
    FUN_004cbd30(s, &hdr, 0x19c);
    FUN_004a2cbd(s_loaded_replay_of_length__f__res__d_005cf260, (double)hdr.len, hdr.res);
    len = hdr.len;
    res = hdr.res;
    FUN_004cc160(s, &mem);
  }
  s = FUN_004cc230(3, 1, &mem);
  if (s != 0) {
    rep = FUN_00482930(len, res, 0);
    data = rep->data;
    FUN_004cbd30(s, rep, 0x19c);
    if (rep->type != 0x10b) {
      ((Free483d10)*(int *)(DAT_007d3ff8 + 0x10c))(rep);
      FUN_004cc160(s, &mem);
      return 0;
    }
    rep->data = data;
    rep->first = data;
    rep->cur = data;
    FUN_004cbd30(s, (void *)data, (rep->count + 2) * 0x24);
    rep->first = rep->data;
    rep->cur = rep->data;
    *(int *)(rep->data + 0x1c) = rep->data + 0x24;
    for (n = 0; n < rep->count; n++) {
      *(int *)(rep->cur + 0x1c) = rep->cur + 0x24;
      rep->cur = *(int *)(rep->cur + 0x1c);
    }
    rep->last = rep->cur;
    *(int *)(rep->cur + 0x1c) = rep->first;
    rep->cur = rep->first;
    FUN_004cc160(s, &mem);
  }
  return rep;
}
