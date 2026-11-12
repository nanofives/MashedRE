// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d1640 (h1): 92.4% (regs 92.4%, ops 99.6%). Raw converter 36.9% (game flags). Moved by: texture
   probe as a __forceinline helper with the adapter format read into a local first (61.1% -> 75.8%), the requested-
   format test as an if/else chain 0x500, 0x600, 0x300, 0x400 instead of a switch (-> 91.5%), the 16-bit
   early return in type 5 and DAT_00911fdc tested as `& 0x80` (jns, -> 92.4%). Residual: that early return is
   `mov eax, esi; or ah, 2` in ours, `or esi, 0x200; mov eax, esi` in the original (one instruction; the jump
   offsets after it shift by 3 bytes, which is most of the remaining 7.6%). Tried: `format |= 0x200; return
   format;` (same). Class (b). Raster format check by raster type (param_1) for the requested format bits
   (param_2 & 0xff00). Types 0/4: D3D9 object (DAT_007d4108) vtable +0x28 texture-format probes (usage 0, resource 3)
   on 0x29 (P8), 0x15, 0x16, 0x1a, 0x32 with the fallbacks 8888 -> 4444/1555, 888 -> 565, 4444 -> 1555,
   LUM8 -> `| 0x16` (as in the binary at 0x004d19aa), PAL4 -> PAL8, and the default pick (P8 when supported with
   0x500/0x600 by the sign of DAT_00911fdc, else 0x500 / 0x300 / 0x100); DAT_00911f94 > 16 gates the 32-bit picks.
   Type 5: render-target probes (usage 1, resource 3, +0x28 then +0x30 against DAT_00912108), 16/32-bit remap by
   the depth byte at DAT_005d8bf4[(fmt >> 8 & 0xf) * 8] and DAT_00911f9c. Type 2: the 16-bit format word of
   DAT_00911b00[DAT_009120e8]. Type 1: 0x700 / 0x900 by DAT_00911f9c. Requested formats on types 1/2 and unknown
   types raise RWERROR 0x8000000d. The jump table at 0x004d1d08 follows the Ghidra size. FLAGS /G5 (RW core). */
/* RWC-DRIVER (2026-11-11): 92.4% -> 99.8%. The 16-bit early return is now `format += 0x200; break;` (same value:
   req == 0 there), which keeps the original's in-place `... esi, 0x200; pop edi; mov eax, esi` tail and all jump
   offsets. Residual: that one opcode (`add esi` vs the original's `or esi`). `format |= 0x200; break;` and every
   |= spelling tried (8 forms) get cross-jumped into the 0x100 path's `or esi, 0x200` tail (547 ins, 91.7%);
   permuter 1008 compiles from that form: 0 gains. Class (b). */
/* RWC-DRIVER-2 (2026-11-11): 100% under TOOLCHAIN rw9044 (L-131) with the plain `format |= 0x200; break;`. The
   VC6 + 9044 compiler does not cross-jump that block into the 0x100 tail (the DDK does, 91.7%). */
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
typedef struct IUnk { void **v; } IUnk;
typedef int (__stdcall *CheckFmtFn)(IUnk *self, int adapter, int type, int adapterFmt, unsigned int usage, int rtype, int fmt);
typedef int (__stdcall *CheckDSFn)(IUnk *self, int adapter, int type, int adapterFmt, int rtFmt, int dsFmt);
typedef struct FmtInfo { unsigned char alpha; unsigned char depth; unsigned short rwFormat; } FmtInfo;
MGLOBAL(IUnk *, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
MGLOBAL(int, DAT_00911f9c, 0x00911f9c);
MGLOBAL(signed char, DAT_00911fdc, 0x00911fdc);
MGLOBAL(int, DAT_00912108, 0x00912108);
MGLOBAL(int, DAT_009120e8, 0x009120e8);
MGLOBALA(FmtInfo, DAT_00911b00, 0x80, 0x00911b00);
MGLOBALA(unsigned char, DAT_005d8bf4, 0x80, 0x005d8bf4);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

static __forceinline int CheckTexFormat(int fmt)
{
  int adapterFmt = DAT_00911f90;
  return ((CheckFmtFn)DAT_007d4108->v[0x28 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, 0, 3, fmt) >= 0;
}

static __forceinline int CheckCamTexFormat(int fmt)
{
  int adapterFmt = DAT_00911f90;
  if (((CheckFmtFn)DAT_007d4108->v[0x28 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, 1, 3, fmt) >= 0 &&
      ((CheckDSFn)DAT_007d4108->v[0x30 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, fmt, DAT_00912108) >= 0) {
    return 1;
  }
  return 0;
}

// MATCH: 0x004d1640 FUN_004d1640
unsigned int FUN_004d1640(int param_1, unsigned int param_2)
{
  unsigned int format = param_2 & 0xff00;
  switch (param_1) {
  case 0:
  case 4:
    if (!(format & 0xf00)) {
      if (format & 0x2000) {
        if (CheckTexFormat(0x29)) {
          if ((DAT_00911fdc & 0x80)) {
            format |= 0x500;
          } else {
            format |= 0x600;
          }
        } else {
          format &= ~0x2000;
        }
        if (format & 0x2000) {
          break;
        }
      }
      if (CheckTexFormat(0x15)) {
        format |= 0x500;
      } else if (CheckTexFormat(0x1a)) {
        format |= 0x300;
      } else {
        format |= 0x100;
      }
    } else {
      if (format & 0x4000) {
        format = (format & ~0x4000) | 0x2000;
      }
      if (format & 0x2000) {
        if (CheckTexFormat(0x29)) {
          if ((format & 0xf00) != 0x500 || (DAT_00911fdc & 0x80)) {
            break;
          }
        }
        format &= ~0x2000;
        if ((format & 0xf00) == 0x600) {
          if (DAT_00911f94 > 16 && CheckTexFormat(0x16)) {
            format |= 0x600;
          } else {
            format = (format & ~0x400) | 0x200;
          }
        } else if (DAT_00911f94 > 16) {
          if (CheckTexFormat(0x15)) {
            format |= 0x500;
          } else {
            format &= ~0x500;
            if (CheckTexFormat(0x1a)) {
              format |= 0x300;
            } else {
              format |= 0x100;
            }
          }
        } else {
          format &= ~0x500;
          if (CheckTexFormat(0x1a)) {
            format |= 0x300;
          } else {
            format |= 0x100;
          }
        }
      } else {
        unsigned int req = format & 0xf00;
        if (req == 0x500) {
          if (!CheckTexFormat(0x15)) {
            format &= ~0x500;
            if (CheckTexFormat(0x1a)) {
              format |= 0x300;
            } else {
              format |= 0x100;
            }
          }
        } else if (req == 0x600) {
          if (!CheckTexFormat(0x16)) {
            format = (format & ~0x400) | 0x200;
          }
        } else if (req == 0x300) {
          if (!CheckTexFormat(0x1a)) {
            format = (format & ~0x200) | 0x100;
          }
        } else if (req == 0x400) {
          if (!CheckTexFormat(0x32)) {
            format &= ~0x400;
            if (CheckTexFormat(0x16)) {
              format |= 0x16;
            } else {
              format |= 0x200;
            }
          }
        }
      }
    }
    break;
  case 5:
    {
      unsigned int req = format & 0xf00;
      if (req == 0) {
        if (DAT_00911f9c == 16) {
          format |= 0x200;
          break;
        }
        if (CheckCamTexFormat(0x16)) {
          format |= 0x600;
        } else {
          format |= 0x200;
        }
      } else {
        unsigned int depth = DAT_005d8bf4[((format >> 8) & 0xf) * 8];
        if (DAT_00911f9c == 16) {
          if (depth == 32) {
            if (req == 0x500) {
              format = (format & ~0x400) | 0x300;
            } else if (req == 0x600) {
              format = (format & ~0x400) | 0x200;
            } else {
              format = 0x200;
            }
          }
          if ((format & 0xf00) == 0x300 && !CheckCamTexFormat(0x1a)) {
            format = (format & ~0x200) | 0x100;
          }
          if ((format & 0xf00) == 0x100 && !CheckCamTexFormat(0x19)) {
            format = (format & ~0x100) | 0x200;
          }
        } else {
          if (depth == 16) {
            if (req == 0x300) {
              format = (format & ~0x200) | 0x500;
            } else if (req == 0x100) {
              format |= 0x500;
            } else if (req == 0x200) {
              format |= 0x600;
            } else {
              format = 0x600;
            }
          }
          if ((format & 0xf00) == 0x500 && !CheckCamTexFormat(0x15)) {
            format = (format & ~0x100) | 0x600;
          }
        }
      }
    }
    break;
  case 2:
    if (!(format & 0xf00)) {
      format = DAT_00911b00[DAT_009120e8].rwFormat;
    } else {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000000d);
      FUN_004d8480(&err);
    }
    break;
  case 1:
    if (!(format & 0xf00)) {
      if (DAT_00911f9c == 16) {
        format = 0x700;
      } else if (DAT_00911f9c == 32) {
        format = 0x900;
      }
    } else {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000000d);
      FUN_004d8480(&err);
    }
    break;
  default:
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000000d);
      FUN_004d8480(&err);
    }
    break;
  }
  return format;
}
