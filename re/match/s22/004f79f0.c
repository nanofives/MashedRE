// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f79f0 (h6): best 27.2% (regs 54.6%, ops 66.7%) under /O2 /Ob1 /G5. Not a register-arg function:
   six cdecl stack args (a, b, pos V3[], uv V2[], D3D9 resource-entry header, c), Ghidra's in_stack/unaff_retaddr
   were [esp+0x64..0x78]. Body: RwMalloc (RwGlobals+0x108, hint 0x1050d) a V3 per vertex, inline memset, lock the
   index buffer (vtbl +0x2c), per instance (0x24-byte records at header+0x40, startIndex +0x1c, minVert +4) walk
   trilist (primType 4) or tristrip (5, odd triangles flipped, degenerates skipped), accumulate -f per component when
   |det| > 0x005dae94, unlock (+0x30), scale each vector by FUN_004c3b90(len^2), call FUN_004f57e0, RwFree (+0x10c).
   Raw converter output (register-arg guess) scored 14.0%. Tried: tristrip i2 base added after the branch, fabs
   intrinsic (worse, 20%), ternary abs, struct locals for the edge/uv deltas, det operand order. Residual: the
   original spills e1x/t1v/e2x/t2v/numerator/det to stack slots in source order with a 0x50 frame and walks the
   index pointers idx+1/idx+2 separately, re-reading the header arg from its stack home; ours keeps them on the x87
   stack (0x20 frame) and the loop/register assignment diverges from the first block on. Class (b): source shape not
   found within budget. */
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
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
typedef struct V3 { float x; float y; float z; } V3;
typedef struct V2 { float u; float v; } V2;
typedef struct IB IB;
typedef struct IBVtbl {
  void *slot[11];
  long (__stdcall *Lock)(IB *self, unsigned int offset, unsigned int size, void **data, unsigned long flags);
  long (__stdcall *Unlock)(IB *self);
} IBVtbl;
struct IB { IBVtbl *vtbl; };
typedef struct InstData {
  int numIndex;
  int minVert;
  void *material;
  int vertexAlpha;
  void *vertexShader;
  int baseIndex;
  int numVertices;
  int startIndex;
  int numPrimitives;
} InstData;
typedef struct ResHeader {
  int serialNumber;
  int numMeshes;
  IB *indexBuffer;
  int primType;
  char pad10[0x28];
  int totalNumIndex;
  int totalNumVertex;
  InstData inst[1];
} ResHeader;
typedef struct MemFuncs {
  char pad[0x108];
  void *(*rwmalloc)(unsigned int size, unsigned int hint);
  void (*rwfree)(void *p);
} MemFuncs;
MGLOBAL(MemFuncs *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005dae94, 0x005dae94);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float FUN_004c3b90(float x);
int FUN_004f57e0(void *a, void *b, V3 *tangents, int numVerts, void *c);
// MATCH: 0x004f79f0 FUN_004f79f0
int FUN_004f79f0(void *a, void *b, const V3 *pos, const V2 *uv, ResHeader *hdr, void *c)
{
  int numVerts;
  V3 *tan;
  unsigned short *indices;
  InstData *inst;
  int numMeshes;
  int result;
  int i;

  numVerts = hdr->totalNumVertex;
  tan = (V3 *)DAT_007d3ff8->rwmalloc(numVerts * sizeof(V3), 0x1050d);
  memset(tan, 0, numVerts * sizeof(V3));
  hdr->indexBuffer->vtbl->Lock(hdr->indexBuffer, 0, hdr->totalNumIndex * 2, (void **)&indices, 0);
  numMeshes = hdr->numMeshes;
  inst = hdr->inst;
  do {
    unsigned short *idx = indices + inst->startIndex;
    int n = inst->numIndex;
    int j;
    if (hdr->primType == 5) {
      n -= 2;
    }
    for (j = 0; j < n; ) {
      int i0, i1, i2;
      float e1x, e2x, t1u, t1v, t2u, t2v, det, f;
      if (hdr->primType == 4) {
        i0 = idx[j] + inst->minVert;
        i1 = idx[j + 1] + inst->minVert;
        i2 = idx[j + 2] + inst->minVert;
        j += 3;
      } else {
        unsigned short r2;
        if (j & 1) {
          i1 = idx[j + 1] + inst->minVert;
          i0 = idx[j + 2] + inst->minVert;
          r2 = idx[j];
        } else {
          i0 = idx[j] + inst->minVert;
          i1 = idx[j + 1] + inst->minVert;
          r2 = idx[j + 2];
        }
        i2 = r2 + inst->minVert;
        j++;
        if (i0 == i1 || i0 == i2 || i1 == i2) {
          continue;
        }
      }
      e1x = pos[i1].x - pos[i0].x;
      t1u = uv[i1].u - uv[i0].u;
      t1v = uv[i1].v - uv[i0].v;
      e2x = pos[i2].x - pos[i0].x;
      t2u = uv[i2].u - uv[i0].u;
      t2v = uv[i2].v - uv[i0].v;
      det = t1u * t2v - t2u * t1v;
      f = e2x * t1v - t2v * e1x;
      if ((det < 0.0f ? -det : det) > DAT_005dae94) {
        f = (DAT_005cc320 / det) * f;
        tan[i0].x -= f;
        tan[i1].x -= f;
        tan[i2].x -= f;
      }
      e1x = pos[i1].y - pos[i0].y;
      e2x = pos[i2].y - pos[i0].y;
      f = e2x * t1v - t2v * e1x;
      if ((det < 0.0f ? -det : det) > DAT_005dae94) {
        f = (DAT_005cc320 / det) * f;
        tan[i0].y -= f;
        tan[i1].y -= f;
        tan[i2].y -= f;
      }
      e1x = pos[i1].z - pos[i0].z;
      e2x = pos[i2].z - pos[i0].z;
      f = e2x * t1v - t2v * e1x;
      if ((det < 0.0f ? -det : det) > DAT_005dae94) {
        f = (DAT_005cc320 / det) * f;
        tan[i0].z -= f;
        tan[i1].z -= f;
        tan[i2].z -= f;
      }
    }
    inst++;
  } while (--numMeshes);
  hdr->indexBuffer->vtbl->Unlock(hdr->indexBuffer);
  for (i = 0; i < numVerts; i++) {
    float inv = FUN_004c3b90(tan[i].x * tan[i].x + tan[i].y * tan[i].y + tan[i].z * tan[i].z);
    tan[i].x *= inv;
    tan[i].y *= inv;
    tan[i].z *= inv;
  }
  result = FUN_004f57e0(a, b, tan, numVerts, c);
  DAT_007d3ff8->rwfree(tan);
  return result;
}
