// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f13d0 (h2): 94.0% (regs 95.4%), relocheck BAD only past the misplaced block (offsets shift), cxx ok. Hand pass from a non-compiling raw file
   (FUN_004ebc30 was an MGLOBAL; now a cdecl void(ResEntry*) callee stored into the destroy-notify slot).
   Shape: RW D3D9 native geometry stream read (RwStreamFindChunk, version 0x35000..0x37002, platform 9, RwResEntry
   of size+0x18, vertex declaration, index buffer Lock/Unlock (vtbl 0x2c/0x30), 2 vertex streams, mesh base index,
   material index -> pointer). What moved it: elements[160] (frame 0xcc), Lock calls duplicated per managed flag
   (cross-jumped push 0x800/0x2800), loop read failure as failed=1;break, init through geometry->repEntry->field
   (stops the hoisted geometry register), i scoped with numVB/failed (stack packing matches).
   Residual (b): the two merged destroy+return blocks (cb840 failure, index-buffer short read) are kept at the
   second site, the original keeps the first (22 ins); tried &&/nested if, goto to a label at the first site
   (86%), goto to a label at the end (86%), if/else forms; plus register picks in the final material loop and the
   RwError lea (ecx vs edx). Permuter 2x300 s: no gain. flagsweep: /G5 /G4 /G3 all 94.0. */
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

typedef struct ResEntry_13d0 ResEntry_13d0;
struct ResEntry_13d0 {
  ResEntry_13d0 *next;
  ResEntry_13d0 *prev;
  unsigned int size;
  void *owner;
  ResEntry_13d0 **ownerRef;
  void (*destroyNotify)(ResEntry_13d0 *entry);
};
typedef struct D3DBufVtbl_13d0 {
  void *slots[11];
  long (__stdcall *Lock)(void *self, unsigned int offset, unsigned int size, void **data, unsigned int flags);
  long (__stdcall *Unlock)(void *self);
} D3DBufVtbl_13d0;
typedef struct D3DBuf_13d0 { D3DBufVtbl_13d0 *vtbl; } D3DBuf_13d0;
typedef struct VertexStream_13d0 {
  D3DBuf_13d0 *vertexBuffer;
  unsigned int offset;
  unsigned int stride;
  unsigned short geometryFlags;
  unsigned char managed;
  unsigned char dynamicLock;
} VertexStream_13d0;
typedef struct InstanceData_13d0 {
  unsigned int numIndex;
  unsigned int minVert;
  int material;
  int vertexAlpha;
  void *vertexShader;
  unsigned int baseIndex;
  unsigned int numVertices;
  unsigned int startIndex;
  unsigned int numPrimitives;
} InstanceData_13d0;
typedef struct ResHeader_13d0 {
  unsigned int serverIndex;
  unsigned int numMeshes;
  D3DBuf_13d0 *indexBuffer;
  unsigned int primType;
  VertexStream_13d0 vertexStream[2];
  int useOffsets;
  void *vertexDeclaration;
  unsigned int totalNumIndex;
  unsigned int totalNumVertex;
} ResHeader_13d0;
typedef struct Geometry_13d0 {
  unsigned char pad0[0x18];
  int numMorphTargets;
  unsigned char pad1c[4];
  unsigned char matList[0x38];
  ResEntry_13d0 *repEntry;
} Geometry_13d0;
typedef struct RwError_13d0 { int pluginID; int errorCode; } RwError_13d0;
typedef void *(*RwMallocFn_13d0)(unsigned int size, unsigned int hint);

int FUN_004cb840(unsigned int numIndices, D3DBuf_13d0 **indexBuffer);
int FUN_004cb8a0(void *elements, void **vertexDeclaration);
unsigned int FUN_004cbd30(void *stream, void *buffer, unsigned int length);
int FUN_004cc5e0(void *stream, int type, unsigned int *lengthOut, unsigned int *versionOut);
int FUN_004d7ff0(int code);
RwError_13d0 *FUN_004d8480(RwError_13d0 *error);
int FUN_004dc5b0(unsigned int stride, unsigned int size, VertexStream_13d0 *stream, unsigned int *offset);
int FUN_004dcaa0(unsigned int size, VertexStream_13d0 *stream);
void FUN_004ebc30(ResEntry_13d0 *entry);
int FUN_004f3bd0(void *matList, int index);

// MATCH: 0x004f13d0 FUN_004f13d0
Geometry_13d0 *FUN_004f13d0(void *stream, Geometry_13d0 *geometry)
{
  unsigned int length;
  unsigned int version;
  int platform;
  unsigned int size;
  unsigned int numElements;
  unsigned char elements[160];
  ResEntry_13d0 **slot;
  ResEntry_13d0 *entry;
  ResHeader_13d0 *header;
  VertexStream_13d0 *vs;
  InstanceData_13d0 *inst;
  int n;

  if (!FUN_004cc5e0(stream, 1, &length, &version)) {
    return 0;
  }
  if (version < 0x35000 || version > 0x37002) {
    RwError_13d0 err;
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
  entry = (ResEntry_13d0 *)(*(RwMallocFn_13d0 *)(DAT_007d3ff8 + 0x108))(size + 0x18, 0x3050d);
  geometry->repEntry = entry;
  geometry->repEntry->next = 0;
  geometry->repEntry->prev = 0;
  geometry->repEntry->owner = geometry;
  geometry->repEntry->size = size;
  geometry->repEntry->ownerRef = &geometry->repEntry;
  geometry->repEntry->destroyNotify = FUN_004ebc30;
  slot = &geometry->repEntry;
  if (FUN_004cbd30(stream, *slot + 1, size) != size) {
    return 0;
  }
  length -= size;
  header = (ResHeader_13d0 *)(*slot + 1);
  header->indexBuffer = 0;
  for (n = 0; n < 2; n++) {
    header->vertexStream[n].vertexBuffer = 0;
  }
  header->vertexDeclaration = 0;
  if (FUN_004cbd30(stream, &numElements, 4) != 4) {
    return 0;
  }
  length -= 4;
  if (FUN_004cbd30(stream, elements, numElements * 8) != numElements * 8) {
    return 0;
  }
  length -= numElements * 8;
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
    unsigned int i;
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
      length -= 0x10;
      if (vs->vertexBuffer != 0) {
        numVB++;
        bytes = header->totalNumVertex * vs->stride;
        if (geometry->numMorphTargets > 1) {
          vs->managed = 0;
          ok = FUN_004dcaa0(bytes, vs);
        } else {
          vs->managed = 1;
          ok = FUN_004dc5b0(vs->stride, bytes, vs, &vs->offset);
        }
        if (ok || !failed) {
          failed = 1;
          if (vs->managed) {
            vs->vertexBuffer->vtbl->Lock(vs->vertexBuffer, vs->offset, bytes, &locked, 0x800);
          } else {
            vs->vertexBuffer->vtbl->Lock(vs->vertexBuffer, vs->offset, bytes, &locked, 0x2800);
          }
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
    inst = (InstanceData_13d0 *)(header + 1);
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
    inst = (InstanceData_13d0 *)(header + 1);
    do {
      inst->material = FUN_004f3bd0(geometry->matList, inst->material);
      inst++;
    } while (--n);
  }
  return geometry;
}
