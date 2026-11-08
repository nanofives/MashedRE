// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1870 (h2): 59.4% (regs 67.3%), cxx ok. Hand rewrite of the raw converter file (43.6%, whose
   unaff_EBX/unaff_EBP were Ghidra misreads of stack locals: the function takes (stream, sector) on the stack, no
   register args, so no static callee or stand-in). Shape: RW D3D9 native world-sector stream write: size from
   repEntry->size + 8 + 4 + numElements*8 (+ index bytes, + 0x10 + stride*numVertex per stream), materials to
   indices (FUN_004f3cb0), chunk header (1, size, 0x37002, 10), platform DAT_006189fc, size, res data, element
   count, elements, index/vertex buffers via Lock/Unlock (vtbl 0x2c/0x30), materials back (FUN_004f3bd0),
   returns stream. Residual (b): the original gives each of the six early write failures its own epilogue
   returning the call's eax (no xor); here they merge into one xor eax,eax exit (~40 ins). Tried `return r`
   with one or per-call temporaries (53.8%), int vs pointer callee returns (no change), flag sweep (best /O2 /Ob1
   /G5; /Oy-, /O1 worse), permuter 300 s (only a semantics-breaking reorder scored higher, rejected). Also a
   register swap (sector esi / size ebx in the original). */
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
MGLOBAL(int, DAT_006189fc, 0x006189fc);

typedef struct ResEntry_1870 ResEntry_1870;
struct ResEntry_1870 {
  ResEntry_1870 *next;
  ResEntry_1870 *prev;
  unsigned int size;
  void *owner;
  ResEntry_1870 **ownerRef;
  void (*destroyNotify)(ResEntry_1870 *entry);
};
typedef struct D3DBufVtbl_1870 {
  void *slots[11];
  long (__stdcall *Lock)(void *self, unsigned int offset, unsigned int size, void **data, unsigned int flags);
  long (__stdcall *Unlock)(void *self);
} D3DBufVtbl_1870;
typedef struct D3DBuf_1870 { D3DBufVtbl_1870 *vtbl; } D3DBuf_1870;
typedef struct D3DDeclVtbl_1870 {
  void *slots[4];
  long (__stdcall *GetDeclaration)(void *self, void *elements, unsigned int *numElements);
} D3DDeclVtbl_1870;
typedef struct D3DDecl_1870 { D3DDeclVtbl_1870 *vtbl; } D3DDecl_1870;
typedef struct VertexStream_1870 {
  D3DBuf_1870 *vertexBuffer;
  unsigned int offset;
  unsigned int stride;
  unsigned short geometryFlags;
  unsigned char managed;
  unsigned char dynamicLock;
} VertexStream_1870;
typedef struct InstanceData_1870 {
  unsigned int numIndex;
  unsigned int minVert;
  int material;
  int vertexAlpha;
  void *vertexShader;
  unsigned int baseIndex;
  unsigned int numVertices;
  unsigned int startIndex;
  unsigned int numPrimitives;
} InstanceData_1870;
typedef struct ResHeader_1870 {
  unsigned int serverIndex;
  unsigned int numMeshes;
  D3DBuf_1870 *indexBuffer;
  unsigned int primType;
  VertexStream_1870 vertexStream[2];
  int useOffsets;
  D3DDecl_1870 *vertexDeclaration;
  unsigned int totalNumIndex;
  unsigned int totalNumVertex;
} ResHeader_1870;
typedef struct Sector_1870 {
  unsigned char pad0[0x34];
  ResEntry_1870 *repEntry;
} Sector_1870;
typedef struct World_1870 {
  unsigned char pad0[8];
  unsigned int flags;
  unsigned char pad0c[4];
  unsigned char matList[0x10];
} World_1870;

void *FUN_004cbe80(void *stream, const void *buffer, unsigned int length);
void *FUN_004cc580(void *stream, int type, unsigned int length, unsigned int version, unsigned int buildNum);
World_1870 *FUN_004e55d0(Sector_1870 *sector);
int FUN_004f3bd0(void *matList, int index);
int FUN_004f3cb0(void *matList, int material);

// MATCH: 0x004f1870 FUN_004f1870
void *FUN_004f1870(void *stream, Sector_1870 *sector)
{
  World_1870 *world;
  ResHeader_1870 *header;
  VertexStream_1870 *vs;
  InstanceData_1870 *inst;
  unsigned int size;
  unsigned int numElements;
  unsigned char elements[160];
  unsigned int i;
  int n;

  world = FUN_004e55d0(sector);
  if (world->flags & 0x1000000) {
    header = (ResHeader_1870 *)(sector->repEntry + 1);
    size = sector->repEntry->size + 8;
    header->vertexDeclaration->vtbl->GetDeclaration(header->vertexDeclaration, elements, &numElements);
    size += 4 + numElements * 8;
    if (header->indexBuffer != 0) {
      size += header->totalNumIndex * 2;
    }
    vs = header->vertexStream;
    for (i = 0; i < 2; i++, vs++) {
      size += 0x10;
      if (vs->vertexBuffer != 0) {
        size += header->totalNumVertex * vs->stride;
      }
    }
    n = header->numMeshes;
    if (n != 0) {
      inst = (InstanceData_1870 *)(header + 1);
      do {
        inst->material = FUN_004f3cb0(world->matList, inst->material);
        inst++;
      } while (--n);
    }
    if (!FUN_004cc580(stream, 1, size, 0x37002, 10)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &DAT_006189fc, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &sector->repEntry->size, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, sector->repEntry + 1, sector->repEntry->size)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &numElements, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, elements, numElements * 8)) {
      return 0;
    }
    if (header->indexBuffer != 0) {
      void *locked;
      void *written;
      locked = 0;
      written = 0;
      header->indexBuffer->vtbl->Lock(header->indexBuffer, 0, 0, &locked, 0);
      if (locked != 0) {
        written = FUN_004cbe80(stream, locked, header->totalNumIndex * 2);
      }
      header->indexBuffer->vtbl->Unlock(header->indexBuffer);
      if (written == 0) {
        return 0;
      }
    }
    vs = header->vertexStream;
    for (i = 0; i < 2; i++, vs++) {
      void *locked;
      if (!FUN_004cbe80(stream, vs, 0x10)) {
        return 0;
      }
      if (vs->vertexBuffer != 0 &&
          vs->vertexBuffer->vtbl->Lock(vs->vertexBuffer, vs->offset, vs->stride * header->totalNumVertex,
                                       &locked, 0x800) == 0) {
        void *written;
        written = FUN_004cbe80(stream, locked, vs->stride * header->totalNumVertex);
        vs->vertexBuffer->vtbl->Unlock(vs->vertexBuffer);
        if (written == 0) {
          return 0;
        }
      }
    }
    n = header->numMeshes;
    if (n != 0) {
      inst = (InstanceData_1870 *)(header + 1);
      do {
        inst->material = FUN_004f3bd0(world->matList, inst->material);
        inst++;
      } while (--n);
    }
  }
  return stream;
}
