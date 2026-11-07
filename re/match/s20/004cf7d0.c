// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cf7d0 (h1): 84.2% (regs 86.7%). Raw converter 50.8%. The original keeps the 4-byte
   {numTextures, deviceId} header in the stream parameter's own stack slot (stream lives in edi); the file
   reproduces it by copying param_1 to `stream` and reading into &param_1 (59.5% with a separate local, 77.7% with
   the slot). Residual: the RwGlobals + 0x10 device pointer is computed at entry here (original: `mov edx, [g];
   lea esi, [edx + 0x10]` just before the header read). Computing it there instead (77.7%) gives a whole-body
   eax/ecx/edx rotation and `mov esi, [g]; add esi, 0x10`. Tried: three positions for the device pointer, game
   flags / /G6 / no /G (lower), permuter 462 compiles (its best variant wrote through an unset pointer, discarded).
   Class (b). Written in the shape of RW 3.6 RwTexDictionaryStreamRead:
   FUN_004cc5e0 (find chunk 1), version window 0x35000..0x37002 (RWERROR 0x80000004), FUN_004cbd30 reads the
   4-byte {numTextures, deviceId} header, FUN_004c2c90(RwGlobals + 0x10, 0x16, &id, 0, 0) device-id check,
   FUN_004c5890 creates the dictionary, then numTextures times: find chunk 0x15, version check, RwGlobals+0xb0
   native texture read, FUN_004e1b60(&DAT_00618138, stream, texture) plugin chunks, FUN_004c5bc0 add. Ends with
   FUN_004e1b60(&DAT_00618150, stream, dict). Failure paths destroy every texture (FUN_004c59c0 with callback
   FUN_004cf9e0) and the dictionary (FUN_004c5930). FLAGS /G5 (RW core). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct BinTexDict { unsigned short numTextures; unsigned short deviceId; } BinTexDict;
typedef struct PluginList { int v[6]; } PluginList;
typedef int (*NativeReadFn)(void *stream, void **tex, unsigned int size);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(PluginList, DAT_00618138, 0x00618138);
MGLOBAL(PluginList, DAT_00618150, 0x00618150);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
int FUN_004cc5e0(void *stream, int type, unsigned int *size, unsigned int *version);
unsigned int FUN_004cbd30(void *stream, void *buf, unsigned int len);
int FUN_004c2c90(void *device, int req, void *out, void *in, int n);
void *FUN_004c5890(void);
void *FUN_004c59c0(void *dict, void *(*cb)(void *, void *), void *data);
int FUN_004c5930(void *dict);
void *FUN_004c5bc0(void *dict, void *tex);
int FUN_004e1b60(PluginList *list, void *stream, void *obj);
void *FUN_004cf9e0(void *tex, void *data);

// MATCH: 0x004cf7d0 FUN_004cf7d0
void *FUN_004cf7d0(void *param_1)
{
  unsigned int size;
  unsigned int version;
  void *dict;
  void *tex;
  void *stream;
  void *device;
  BinTexDict *bin;
  unsigned int deviceId;

  stream = param_1;
  device = (void *)(DAT_007d3ff8 + 0x10);
  bin = (BinTexDict *)&param_1;
  if (!FUN_004cc5e0(stream, 1, &size, &version)) {
    return 0;
  }
  if (!(version >= 0x35000 && version <= 0x37002)) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    return 0;
  }
  {
    param_1 = 0;
    if (FUN_004cbd30(stream, &param_1, size) != size) {
      return 0;
    }
    deviceId = 0;
    FUN_004c2c90(device, 0x16, &deviceId, 0, 0);
    deviceId &= 0xff;
    if ((unsigned short)deviceId != 0 && bin->deviceId != 0 && (unsigned short)deviceId != bin->deviceId) {
      return 0;
    }
  }
  dict = FUN_004c5890();
  if (dict == 0) {
    return 0;
  }
  while (bin->numTextures--) {
    if (!FUN_004cc5e0(stream, 0x15, &size, &version)) {
      FUN_004c59c0(dict, FUN_004cf9e0, 0);
      FUN_004c5930(dict);
      return 0;
    }
    if (!(version >= 0x35000 && version <= 0x37002)) {
      RwError err;
      FUN_004c59c0(dict, FUN_004cf9e0, 0);
      FUN_004c5930(dict);
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&err);
      return 0;
    }
    if (!(*(NativeReadFn *)(DAT_007d3ff8 + 0xb0))(stream, &tex, size) || tex == 0 ||
        !FUN_004e1b60(&DAT_00618138, stream, tex)) {
      FUN_004c59c0(dict, FUN_004cf9e0, 0);
      FUN_004c5930(dict);
      return 0;
    }
    FUN_004c5bc0(dict, tex);
  }
  if (!FUN_004e1b60(&DAT_00618150, stream, dict)) {
    FUN_004c59c0(dict, FUN_004cf9e0, 0);
    FUN_004c5930(dict);
    return 0;
  }
  return dict;
}
