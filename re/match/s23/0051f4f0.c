// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051f4f0: libpng 1.0.12 pngwutil.c png_write_finish_row, png_write_IDAT (both calls) written inline
   in the 0x0051d0f0 form, png_memset = intrinsic memset.
   97.6% (regs 98.4, ops 99.2). Official body with both png_write_IDAT calls inline (0x0051d0f0 form; the first one's crc
   buffer block-scoped, 78.9 -> 97.6). Residual: the original pushes ebx/edi in the prologue, ours after the early
   row_number return (shrink-wrap), same pattern as 0x0051cb40. Tried if-block instead of return, flag variants, png_ptr
   copy, permuter 240 s. Class (b). */
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
#ifdef __cplusplus
extern "C" void *__cdecl memset(void *d, int c, unsigned int n);
#else
void *__cdecl memset(void *d, int c, unsigned int n);
#endif
#pragma intrinsic(memset)
MGLOBALA(int, DAT_005e32c4, 7, 0x005e32c4);
MGLOBALA(int, DAT_005e32e0, 7, 0x005e32e0);
MGLOBALA(int, DAT_005e32fc, 7, 0x005e32fc);
MGLOBALA(int, DAT_005e3318, 7, 0x005e3318);
MGLOBALA(unsigned char, png_IDAT, 5, 0x005e3224);
MGLOBALA(char, s_zlib_error_00620f0c, 11, 0x00620f0c);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517250(int png_ptr, const char *msg);
int FUN_00521410(void *strm);
int FUN_00521520(void *strm, int flush);

// MATCH: 0x0051f4f0 FUN_0051f4f0
void FUN_0051f4f0(int png_ptr)
{
   int ret;

   (*(unsigned int *)(png_ptr + 0xe4))++;

   if (*(unsigned int *)(png_ptr + 0xe4) < *(unsigned int *)(png_ptr + 0xd0))
      return;

   if (*(unsigned char *)(png_ptr + 0x123))
   {
      *(unsigned int *)(png_ptr + 0xe4) = 0;
      if (*(unsigned int *)(png_ptr + 0x70) & 2)
      {
         (*(unsigned char *)(png_ptr + 0x124))++;
      }
      else
      {
         do
         {
            (*(unsigned char *)(png_ptr + 0x124))++;
            if (*(unsigned char *)(png_ptr + 0x124) >= 7)
               break;
            *(unsigned int *)(png_ptr + 0xd4) = (*(unsigned int *)(png_ptr + 0xc8) +
               DAT_005e32e0[*(unsigned char *)(png_ptr + 0x124)] - 1 -
               DAT_005e32c4[*(unsigned char *)(png_ptr + 0x124)]) /
               DAT_005e32e0[*(unsigned char *)(png_ptr + 0x124)];
            *(unsigned int *)(png_ptr + 0xd0) = (*(unsigned int *)(png_ptr + 0xcc) +
               DAT_005e3318[*(unsigned char *)(png_ptr + 0x124)] - 1 -
               DAT_005e32fc[*(unsigned char *)(png_ptr + 0x124)]) /
               DAT_005e3318[*(unsigned char *)(png_ptr + 0x124)];
            if (*(unsigned int *)(png_ptr + 0x70) & 2)
               break;
         } while (*(unsigned int *)(png_ptr + 0xd4) == 0 || *(unsigned int *)(png_ptr + 0xd0) == 0);
      }

      if (*(unsigned char *)(png_ptr + 0x124) < 7)
      {
         if (*(unsigned char * *)(png_ptr + 0xe8) != 0)
            memset(*(unsigned char * *)(png_ptr + 0xe8), 0,
               (unsigned int) ((((unsigned int)*(unsigned char *)(png_ptr + 0x12b) *
               (unsigned int)*(unsigned char *)(png_ptr + 0x128) *
               *(unsigned int *)(png_ptr + 0xc8) + 7) >> 3) + 1));
         return;
      }
   }

   do
   {
      ret = FUN_00521520((void *)(png_ptr + 0x74), 4);
      if (ret == 0)
      {
         if (!(*(unsigned int *)(png_ptr + 0x84)))
         {
            {
               unsigned char *data = *(unsigned char * *)(png_ptr + 0xac);
               unsigned int length = *(unsigned int *)(png_ptr + 0xb0);
               unsigned char buf[4];
               unsigned int crc;
               buf[0] = (unsigned char)((length >> 24) & 0xff);
               buf[1] = (unsigned char)((length >> 16) & 0xff);
               buf[2] = (unsigned char)((length >> 8) & 0xff);
               buf[3] = (unsigned char)(length & 0xff);
               FUN_00516bb0(png_ptr, buf, 4);
               FUN_00516bb0(png_ptr, png_IDAT, 4);
               FUN_00516d30(png_ptr);
               FUN_00516d50(png_ptr, png_IDAT, 4);
               if (data != 0 && length > 0)
               {
                  FUN_00516d50(png_ptr, data, length);
                  FUN_00516bb0(png_ptr, data, length);
               }
               {
               unsigned char buf[4];
               crc = *(unsigned int *)(png_ptr + 0x110);
               buf[0] = (unsigned char)((crc >> 24) & 0xff);
               buf[1] = (unsigned char)((crc >> 16) & 0xff);
               buf[2] = (unsigned char)((crc >> 8) & 0xff);
               buf[3] = (unsigned char)(crc & 0xff);
               FUN_00516bb0(png_ptr, buf, 4);
               }
               *(unsigned int *)(png_ptr + 0x68) |= 4;
            }
            *(unsigned char * *)(png_ptr + 0x80) = *(unsigned char * *)(png_ptr + 0xac);
            *(unsigned int *)(png_ptr + 0x84) = *(unsigned int *)(png_ptr + 0xb0);
         }
      }
      else if (ret != 1)
      {
         if (*(char * *)(png_ptr + 0x8c) != 0)
            FUN_00517250(png_ptr, *(char * *)(png_ptr + 0x8c));
         else
            FUN_00517250(png_ptr, s_zlib_error_00620f0c);
      }
   } while (ret != 1);

   if (*(unsigned int *)(png_ptr + 0x84) < *(unsigned int *)(png_ptr + 0xb0))
   {
      {
         unsigned char *data = *(unsigned char * *)(png_ptr + 0xac);
         unsigned int length = *(unsigned int *)(png_ptr + 0xb0) - *(unsigned int *)(png_ptr + 0x84);
         unsigned char buf[4];
         unsigned int crc;
         buf[0] = (unsigned char)((length >> 24) & 0xff);
         buf[1] = (unsigned char)((length >> 16) & 0xff);
         buf[2] = (unsigned char)((length >> 8) & 0xff);
         buf[3] = (unsigned char)(length & 0xff);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_IDAT, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_IDAT, 4);
         if (data != 0 && length > 0)
         {
            FUN_00516d50(png_ptr, data, length);
            FUN_00516bb0(png_ptr, data, length);
         }
         crc = *(unsigned int *)(png_ptr + 0x110);
         buf[0] = (unsigned char)((crc >> 24) & 0xff);
         buf[1] = (unsigned char)((crc >> 16) & 0xff);
         buf[2] = (unsigned char)((crc >> 8) & 0xff);
         buf[3] = (unsigned char)(crc & 0xff);
         FUN_00516bb0(png_ptr, buf, 4);
         *(unsigned int *)(png_ptr + 0x68) |= 4;
      }
   }

   FUN_00521410((void *)(png_ptr + 0x74));
}
