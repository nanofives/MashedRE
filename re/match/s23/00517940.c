// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00517940: libpng 1.0.12 pngread.c png_read_row (PNG_READ_INTERLACING_SUPPORTED, no MNG
   features, png_pass_dsp_mask at 0x005e3350 and png_pass_mask at 0x005e3334 as globals, png_IDAT at
   0x005e3224; png_memcmp(chunk_name, png_IDAT, 4) is the inlined dword compare). png_ptr copied from
   png_ptr_arg so chunk_length can sit in the arg slot.
   100% (h7 round 2; was 99.4). What moved it: cases 2 and 4 written with the constant on the left,
   `if (4 != (row_number & 0x07))` / `if (2 != (row_number & 3))`: the original's 32-bit `and ecx, 7` before
   `cmp cl, 4` comes from that operand order (the official `(x & 7) != 4` gives `and cl, 7`). */
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
MGLOBALA(int, png_pass_mask, 7, 0x005e3334);
MGLOBALA(int, png_pass_dsp_mask, 7, 0x005e3350);
MGLOBALA(unsigned char, png_IDAT, 5, 0x005e3224);
MGLOBALA(char, s_Extra_compressed_data_00621528, 22, 0x00621528);
MGLOBALA(char, s_Decompression_error_00621540, 20, 0x00621540);
MGLOBALA(char, s_Not_enough_image_data_00621554, 22, 0x00621554);
MGLOBALA(char, s_Invalid_attempt_to_read_row_data_0062156c, 33, 0x0062156c);
#ifdef __cplusplus
extern "C"
#endif
int __cdecl memcmp(const void *a, const void *b, unsigned int n);
#pragma intrinsic(memcmp)
#define PP(T, off) (*(T *)((char *)png_ptr + (off)))
typedef void (*png_read_status_ptr)(void *, unsigned int, int);
void FUN_00528ba0(void *png_ptr);
void FUN_00527ee0(void *png_ptr, unsigned char *row, int mask);
void FUN_00528810(void *png_ptr);
void FUN_00517250(void *png_ptr, const char *msg);
unsigned int FUN_00523170(void *png_ptr, unsigned int skip);
void FUN_0051c9f0(void *png_ptr, unsigned char *data, unsigned int length);
unsigned int FUN_00523110(unsigned char *buf);
void FUN_00516d30(void *png_ptr);
void FUN_00523140(void *png_ptr, unsigned char *buf, unsigned int length);
int FUN_00522ce0(void *strm, int flush);
void FUN_00528630(void *png_ptr, void *row_info, unsigned char *row, unsigned char *prev_row, int filter);
void FUN_00520960(void *png_ptr, void *s1, void *s2, unsigned int size);
void FUN_00519270(void *png_ptr);
void FUN_00528200(void *png_ptr);
// MATCH: 0x00517940 FUN_00517940
void FUN_00517940(void *png_ptr_arg, unsigned char *row, unsigned char *dsp_row)
{
   void *png_ptr = png_ptr_arg;
   int ret;
   if (!(PP(unsigned int, 0x6c) & 0x0040))
      FUN_00528ba0(png_ptr);

   /* if interlaced and we do not need a new row, combine row and return */
   if (PP(unsigned char, 0x123) && (PP(unsigned int, 0x70) & 0x0002))
   {
      switch (PP(unsigned char, 0x124))
      {
         case 0:
            if (PP(unsigned int, 0xe4) & 0x07)
            {
               if (dsp_row != 0)
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 1:
            if ((PP(unsigned int, 0xe4) & 0x07) || PP(unsigned int, 0xc8) < 5)
            {
               if (dsp_row != 0)
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 2:
            if (4 != (PP(unsigned int, 0xe4) & 0x07))
            {
               if (dsp_row != 0 && (PP(unsigned int, 0xe4) & 4))
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 3:
            if ((PP(unsigned int, 0xe4) & 3) || PP(unsigned int, 0xc8) < 3)
            {
               if (dsp_row != 0)
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 4:
            if (2 != (PP(unsigned int, 0xe4) & 3))
            {
               if (dsp_row != 0 && (PP(unsigned int, 0xe4) & 2))
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 5:
            if ((PP(unsigned int, 0xe4) & 1) || PP(unsigned int, 0xc8) < 2)
            {
               if (dsp_row != 0)
                  FUN_00527ee0(png_ptr, dsp_row,
                     png_pass_dsp_mask[PP(unsigned char, 0x124)]);
               FUN_00528810(png_ptr);
               return;
            }
            break;
         case 6:
            if (!(PP(unsigned int, 0xe4) & 1))
            {
               FUN_00528810(png_ptr);
               return;
            }
            break;
      }
   }

   if (!(PP(unsigned int, 0x68) & 0x04))
      FUN_00517250(png_ptr, s_Invalid_attempt_to_read_row_data_0062156c);

   PP(unsigned char *, 0x80) = PP(unsigned char *, 0xec);
   PP(unsigned int, 0x84) = (unsigned int)PP(unsigned int, 0xdc);
   do
   {
      if (!(PP(unsigned int, 0x78)))
      {
         while (!PP(unsigned int, 0x10c))
         {
            unsigned char chunk_length[4];

            FUN_00523170(png_ptr, 0);

            FUN_0051c9f0(png_ptr, chunk_length, 4);
            PP(unsigned int, 0x10c) = FUN_00523110(chunk_length);

            FUN_00516d30(png_ptr);
            FUN_00523140(png_ptr, &PP(unsigned char, 0x11c), 4);
            if (memcmp(&PP(unsigned char, 0x11c), png_IDAT, 4))
               FUN_00517250(png_ptr, s_Not_enough_image_data_00621554);
         }
         PP(unsigned int, 0x78) = (unsigned int)PP(unsigned int, 0xb0);
         PP(unsigned char *, 0x74) = PP(unsigned char *, 0xac);
         if (PP(unsigned int, 0xb0) > PP(unsigned int, 0x10c))
            PP(unsigned int, 0x78) = (unsigned int)PP(unsigned int, 0x10c);
         FUN_00523140(png_ptr, PP(unsigned char *, 0xac),
            (unsigned int)PP(unsigned int, 0x78));
         PP(unsigned int, 0x10c) -= PP(unsigned int, 0x78);
      }
      ret = FUN_00522ce0((char *)png_ptr + 0x74, 1);
      if (ret == 1)
      {
         if (PP(unsigned int, 0x84) || PP(unsigned int, 0x78) ||
            PP(unsigned int, 0x10c))
            FUN_00517250(png_ptr, s_Extra_compressed_data_00621528);
         PP(unsigned int, 0x68) |= 0x08;
         PP(unsigned int, 0x6c) |= 0x20;
         break;
      }
      if (ret != 0)
         FUN_00517250(png_ptr, PP(char *, 0x8c) ? PP(char *, 0x8c) :
                   s_Decompression_error_00621540);

   } while (PP(unsigned int, 0x84));

   PP(unsigned char, 0x108) = PP(unsigned char, 0x126);
   PP(unsigned int, 0x100) = PP(unsigned int, 0xe0);
   PP(unsigned char, 0x10a) = PP(unsigned char, 0x12a);
   PP(unsigned char, 0x109) = PP(unsigned char, 0x127);
   PP(unsigned char, 0x10b) = PP(unsigned char, 0x129);
   PP(unsigned int, 0x104) = ((PP(unsigned int, 0x100) *
      (unsigned int)PP(unsigned char, 0x10b) + 7) >> 3);

   if(PP(unsigned char *, 0xec)[0])
   FUN_00528630(png_ptr, &PP(unsigned char, 0x100),
      PP(unsigned char *, 0xec) + 1, PP(unsigned char *, 0xe8) + 1,
      (int)(PP(unsigned char *, 0xec)[0]));

   FUN_00520960(png_ptr, PP(unsigned char *, 0xe8), PP(unsigned char *, 0xec),
      PP(unsigned int, 0xd8) + 1);

   if (PP(unsigned int, 0x70))
      FUN_00519270(png_ptr);

   /* blow up interlaced rows to full size */
   if (PP(unsigned char, 0x123) &&
      (PP(unsigned int, 0x70) & 0x0002))
   {
      if (PP(unsigned char, 0x124) < 6)
         FUN_00528200(png_ptr);

      if (dsp_row != 0)
         FUN_00527ee0(png_ptr, dsp_row,
            png_pass_dsp_mask[PP(unsigned char, 0x124)]);
      if (row != 0)
         FUN_00527ee0(png_ptr, row,
            png_pass_mask[PP(unsigned char, 0x124)]);
   }
   else
   {
      if (row != 0)
         FUN_00527ee0(png_ptr, row, 0xff);
      if (dsp_row != 0)
         FUN_00527ee0(png_ptr, dsp_row, 0xff);
   }
   FUN_00528810(png_ptr);

   if (PP(png_read_status_ptr, 0x198) != 0)
      (*(PP(png_read_status_ptr, 0x198)))(png_ptr, PP(unsigned int, 0xe4), PP(unsigned char, 0x124));
}
