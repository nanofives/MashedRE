// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cb190 (h1, h5): 100% (regs 100%). Written in the shape of RW 3.6 _rwD3D9SetStreams (vertex-stream
   cache LastVertexStreamUsed = DAT_007d40c0, IDirect3DDevice9::SetStreamSource at +0x190), which moved 37.8% to 98.3%:
   useOffsets == FALSE loop first, a `const vb = streams[n].vertexBuffer` local, if/else nesting as in RW.
   Round 2 (h5): copying the parameter into a local (`const Rec16 *streams = param_1;`) moved the spill of
   (cache + 4 - streams) from param_2's stack slot to param_1's, as in the original: 98.3% -> 100%.
   For records 0 and 1 of the 16-byte array at param_1 (src) against the cache
   DAT_007d40c0: src.a == 0 -> when cache.a != 0, zero cache a/b/c and call the D3D device (DAT_007d4110) vtable
   +0x190 (__stdcall) with (device, i, 0, 0, 0); otherwise, when (a, b, c) differ, store them and call it with
   (device, i, a, b, c). With param_2 == 0, b is taken as 0. FLAGS /G5 (RW core). */
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
typedef struct Rec16 { int a; int b; int c; int d; } Rec16;
typedef int (__stdcall *DevFn4)(void *self, unsigned int i, int a, int b, int c);
MGLOBAL(int **, DAT_007d4110, 0x007d4110);
MGLOBALA(Rec16, DAT_007d40c0, 4, 0x007d40c0);

// MATCH: 0x004cb190 FUN_004cb190
void FUN_004cb190(const Rec16 *param_1, int param_2)
{
  const Rec16 *streams = param_1;
  unsigned int n;
  if (param_2 == 0) {
    for (n = 0; n < 2; n++) {
      const int vb = streams[n].a;
      if (vb != 0) {
        if (DAT_007d40c0[n].a != vb || DAT_007d40c0[n].b != 0 || DAT_007d40c0[n].c != streams[n].c) {
          DAT_007d40c0[n].a = vb;
          DAT_007d40c0[n].b = 0;
          DAT_007d40c0[n].c = streams[n].c;
          ((DevFn4)(*DAT_007d4110)[0x190 / 4])(DAT_007d4110, n, vb, 0, streams[n].c);
        }
      } else {
        if (DAT_007d40c0[n].a != 0) {
          DAT_007d40c0[n].a = 0;
          DAT_007d40c0[n].b = 0;
          DAT_007d40c0[n].c = 0;
          ((DevFn4)(*DAT_007d4110)[0x190 / 4])(DAT_007d4110, n, 0, 0, 0);
        }
      }
    }
  } else {
    for (n = 0; n < 2; n++) {
      const int vb = streams[n].a;
      if (vb != 0) {
        if (DAT_007d40c0[n].a != vb || DAT_007d40c0[n].b != streams[n].b || DAT_007d40c0[n].c != streams[n].c) {
          DAT_007d40c0[n].a = vb;
          DAT_007d40c0[n].b = streams[n].b;
          DAT_007d40c0[n].c = streams[n].c;
          ((DevFn4)(*DAT_007d4110)[0x190 / 4])(DAT_007d4110, n, vb, streams[n].b, streams[n].c);
        }
      } else {
        if (DAT_007d40c0[n].a != 0) {
          DAT_007d40c0[n].a = 0;
          DAT_007d40c0[n].b = 0;
          DAT_007d40c0[n].c = 0;
          ((DevFn4)(*DAT_007d4110)[0x190 / 4])(DAT_007d4110, n, 0, 0, 0);
        }
      }
    }
  }
}
