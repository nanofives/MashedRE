// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1b00 (h2): 91.9% (regs 93.4%), cxx ok. Hand pass from a non-compiling raw file (FUN_004ebc30 was an
   MGLOBAL; now a cdecl void(ResEntry*) callee). World-sector twin of 0x004f13d0 (same TU shape): world =
   FUN_004e55d0(sector) first, repEntry at sector+0x34, matList at world+0x10, no length -= 0x10 per stream, Lock
   flags 0x800 only. What moved it: the 13d0 recipe, plus the vertex-element byte count as a second expression
   `i = (int)numElements << 3` (the original computes numElements*8 twice and keeps one in i's stack slot 0x1c;
   that also restores the stream in esi), IB locked/nread zeroed before Lock and bytes computed after it.
   Residual (b): stack slot order of version/platform/numElements (0x3c/0x38/0x30 vs 0x38/0x30/0x3c), which no
   declaration order (5040 permutations), type or name change moved and block scoping made worse; the merged
   destroy block kept at the second site (same as 0x004f13d0); register picks in the material loop / RwError.
   Permuter 300 s: 77.2 -> 82.9 before the hand steps. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(unsigned char, DAT_00912074, 0x00912074);

typedef struct ResEntry_1b00 ResEntry_1b00;
struct ResEntry_1b00 {
  ResEntry_1b00 *next;
  ResEntry_1b00 *prev;
  unsigned int size;
  void *owner;
  ResEntry_1b00 **ownerRef;
  void (*destroyNotify)(ResEntry_1b00 *entry);
};
typedef struct D3DBufVtbl_1b00 {
  void *slots[11];
  long (__stdcall *Lock)(void *self, unsigned int offset, unsigned int size, void **data, unsigned int flags);
  long (__stdcall *Unlock)(void *self);
} D3DBufVtbl_1b00;
typedef struct D3DBuf_1b00 { D3DBufVtbl_1b00 *vtbl; } D3DBuf_1b00;
typedef struct VertexStream_1b00 {
  D3DBuf_1b00 *vertexBuffer;
  unsigned int offset;
  unsigned int stride;
  unsigned short geometryFlags;
  unsigned char managed;
  unsigned char dynamicLock;
} VertexStream_1b00;
typedef struct InstanceData_1b00 {
  unsigned int numIndex;
  unsigned int minVert;
  int material;
  int vertexAlpha;
  void *vertexShader;
  unsigned int baseIndex;
  unsigned int numVertices;
  unsigned int startIndex;
  unsigned int numPrimitives;
} InstanceData_1b00;
typedef struct ResHeader_1b00 {
  unsigned int serverIndex;
  unsigned int numMeshes;
  D3DBuf_1b00 *indexBuffer;
  unsigned int primType;
  VertexStream_1b00 vertexStream[2];
  int useOffsets;
  void *vertexDeclaration;
  unsigned int totalNumIndex;
  unsigned int totalNumVertex;
} ResHeader_1b00;
typedef struct Sector_1b00 {
  unsigned char pad0[0x34];
  ResEntry_1b00 *repEntry;
} Sector_1b00;
typedef struct World_1b00 {
  unsigned char pad0[0x10];
  unsigned char matList[0x10];
} World_1b00;
typedef struct RwError_1b00 { int pluginID; int errorCode; } RwError_1b00;
typedef void *(*RwMallocFn_1b00)(unsigned int size, unsigned int hint);

int FUN_004cb840(unsigned int numIndices, D3DBuf_1b00 **indexBuffer);
int FUN_004cb8a0(void *elements, void **vertexDeclaration);
unsigned int FUN_004cbd30(void *stream, void *buffer, unsigned int length);
int FUN_004cc5e0(void *stream, int type, unsigned int *lengthOut, unsigned int *versionOut);
int FUN_004d7ff0(int code);
RwError_1b00 *FUN_004d8480(RwError_1b00 *error);
int FUN_004dc5b0(unsigned int stride, unsigned int size, VertexStream_1b00 *stream, unsigned int *offset);
World_1b00 *FUN_004e55d0(Sector_1b00 *sector);
void FUN_004ebc30(ResEntry_1b00 *entry);
int FUN_004f3bd0(void *matList, int index);

// MATCH: 0x004f1b00 FUN_004f1b00
Sector_1b00 *FUN_004f1b00(void *stream, Sector_1b00 *sector)
{
  World_1b00 *world;
  unsigned int length;
  unsigned int version;
  int platform;
  unsigned int size;
  unsigned int numElements;
  unsigned char elements[160];
  ResEntry_1b00 **slot;
  ResEntry_1b00 *entry;
  ResHeader_1b00 *header;
  VertexStream_1b00 *vs;
  InstanceData_1b00 *inst;
  int n;
  unsigned int i;

  world = FUN_004e55d0(sector);
  if (!FUN_004cc5e0(stream, 1, &length, &version)) {
    return 0;
  }
  if (version < 0x35000 || version > 0x37002) {
    RwError_1b00 err;
    err.pluginID = 2;
    err.errorCode = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    return 0;
  }
  if (FUN_004cbd30(stream, &platform, 4) != 4) {
    return 0;
  }
  length -= 4;
  if (platform != 9) {
    return 0;
  }
  if (FUN_004cbd30(stream, &size, 4) != 4) {
    return 0;
  }
  length -= 4;
  entry = (ResEntry_1b00 *)(*(RwMallocFn_1b00 *)(DAT_007d3ff8 + 0x108))(size + 0x18, 0x3050d);
  sector->repEntry = entry;
  sector->repEntry->next = 0;
  sector->repEntry->prev = 0;
  sector->repEntry->owner = sector;
  sector->repEntry->size = size;
  sector->repEntry->ownerRef = &sector->repEntry;
  sector->repEntry->destroyNotify = FUN_004ebc30;
  slot = &sector->repEntry;
  if (FUN_004cbd30(stream, *slot + 1, size) != size) {
    return 0;
  }
  length -= size;
  header = (ResHeader_1b00 *)(*slot + 1);
  header->indexBuffer = 0;
  for (n = 0; n < 2; n++) {
    header->vertexStream[n].vertexBuffer = 0;
  }
  header->vertexDeclaration = 0;
  if (FUN_004cbd30(stream, &numElements, 4) != 4) {
    return 0;
  }
  length -= 4;
  i = (int)numElements << 3;
  if (FUN_004cbd30(stream, elements, numElements * 8) != i) {
    return 0;
  }
  length -= i;
  FUN_004cb8a0(elements, &header->vertexDeclaration);
  if (header->totalNumIndex != 0 && !FUN_004cb840(header->totalNumIndex, &header->indexBuffer)) {
    FUN_004ebc30(*slot);
    return 0;
  }
  if (header->indexBuffer != 0) {
    void *locked;
    unsigned int nread;
    unsigned int bytes;
    locked = 0;
    nread = 0;
    header->indexBuffer->vtbl->Lock(header->indexBuffer, 0, 0, &locked, 0);
    bytes = header->totalNumIndex * 2;
    if (locked != 0) {
      nread = FUN_004cbd30(stream, locked, bytes);
      header->indexBuffer->vtbl->Unlock(header->indexBuffer);
    }
    if (nread != bytes) {
      FUN_004ebc30(*slot);
      return 0;
    }
    length -= nread;
  }
  {
    unsigned int numVB;
    int failed;
    numVB = 0;
    i = 0;
    failed = 1;
    vs = header->vertexStream;
    for (; i < 2; i++, vs++) {
      unsigned int bytes;
      int ok;
      void *locked;
      unsigned int nread;
      if (FUN_004cbd30(stream, vs, 0x10) != 0x10) {
        failed = 1;
        break;
      }
      if (vs->vertexBuffer != 0) {
        numVB++;
        bytes = header->totalNumVertex * vs->stride;
        vs->managed = 1;
        ok = FUN_004dc5b0(vs->stride, bytes, vs, &vs->offset);
        if (ok || !failed) {
          failed = 1;
          vs->vertexBuffer->vtbl->Lock(vs->vertexBuffer, vs->offset, bytes, &locked, 0x800);
          nread = FUN_004cbd30(stream, locked, bytes);
          vs->vertexBuffer->vtbl->Unlock(vs->vertexBuffer);
          if (nread == bytes) {
            failed = 0;
            length -= nread;
          }
        }
      }
    }
    if (failed) {
      FUN_004ebc30(*slot);
      return 0;
    }
    inst = (InstanceData_1b00 *)(header + 1);
    if (numVB > 1) {
      if (DAT_00912074 & 1) {
        header->useOffsets = 1;
      }
      n = header->numMeshes;
      do {
        inst->baseIndex = inst->minVert;
        inst++;
      } while (--n);
    } else {
      header->useOffsets = 0;
      n = header->numMeshes;
      do {
        inst->baseIndex = inst->minVert + header->vertexStream[0].offset / header->vertexStream[0].stride;
        inst++;
      } while (--n);
    }
  }
  n = header->numMeshes;
  if (n != 0) {
    inst = (InstanceData_1b00 *)(header + 1);
    do {
      inst->material = FUN_004f3bd0(world->matList, inst->material);
      inst++;
    } while (--n);
  }
  return sector;
}
