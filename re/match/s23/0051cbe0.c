// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051cbe0: libpng 1.0.12 pngwutil.c png_write_IHDR (no PNG_MNG_FEATURES in this function's filter
   check, PNG_WRITE_INTERLACING_SUPPORTED), png_save_uint_32 and png_write_chunk with its start / data / end written
   inline, deflateInit2 = deflateInit2_(..., "1.1.3", 0x38).
   95.6% (regs 96.4, ops 98.8). Residual: zlib_strategy. The original keeps 0 in ebx (xor ebx,ebx, shared by the zero
   length bytes, the do_filter test and the Z_DEFAULT_STRATEGY store) and branches; 9176 uses xor bl,bl and a setne
   store. Tried L/unsigned/int-cast spellings of the strategy if, png_ptr/width copies, permuter 240 s. Class (b).
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
MGLOBALA(unsigned char, png_IHDR, 5, 0x005e321c);
MGLOBALA(char, s_1_1_3_006214b0, 6, 0x006214b0);
MGLOBALA(char, s_Invalid_interlace_type_specified_006216ec, 33, 0x006216ec);
MGLOBALA(char, s_Invalid_filter_type_specified_00621710, 30, 0x00621710);
MGLOBALA(char, s_Invalid_compression_type_specifi_00621730, 35, 0x00621730);
MGLOBALA(char, s_Invalid_image_color_type_specifi_00621754, 35, 0x00621754);
MGLOBALA(char, s_Invalid_bit_depth_for_RGBA_image_00621778, 33, 0x00621778);
MGLOBALA(char, s_Invalid_bit_depth_for_grayscale__0062179c, 44, 0x0062179c);
MGLOBALA(char, s_Invalid_bit_depth_for_paletted_i_006217c8, 37, 0x006217c8);
MGLOBALA(char, s_Invalid_bit_depth_for_RGB_image_006217f0, 32, 0x006217f0);
MGLOBALA(char, s_Invalid_bit_depth_for_grayscale__00621810, 38, 0x00621810);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517250(int png_ptr, const char *msg);
void FUN_00517270(int png_ptr, const char *msg);
void *FUN_00516cc0(void *png_ptr, unsigned int items, unsigned int size);
void FUN_00516d20(void *png_ptr, void *ptr);
int FUN_00521110(void *strm, int level, int method, int windowBits, int memLevel, int strategy,
   const char *version, int stream_size);

// MATCH: 0x0051cbe0 FUN_0051cbe0
void FUN_0051cbe0(int png_ptr, unsigned int width, unsigned int height,
   int bit_depth, int color_type, int compression_type, int filter_type,
   int interlace_type)
{
   unsigned char buf[13];

   switch (color_type)
   {
      case 0:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8:
            case 16: *(unsigned char *)(png_ptr + 0x12a) = 1; break;
            default: FUN_00517250(png_ptr, s_Invalid_bit_depth_for_grayscale__00621810);
         }
         break;
      case 2:
         if (bit_depth != 8 && bit_depth != 16)
            FUN_00517250(png_ptr, s_Invalid_bit_depth_for_RGB_image_006217f0);
         *(unsigned char *)(png_ptr + 0x12a) = 3;
         break;
      case 3:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8: *(unsigned char *)(png_ptr + 0x12a) = 1; break;
            default: FUN_00517250(png_ptr, s_Invalid_bit_depth_for_paletted_i_006217c8);
         }
         break;
      case 4:
         if (bit_depth != 8 && bit_depth != 16)
            FUN_00517250(png_ptr, s_Invalid_bit_depth_for_grayscale__0062179c);
         *(unsigned char *)(png_ptr + 0x12a) = 2;
         break;
      case 6:
         if (bit_depth != 8 && bit_depth != 16)
            FUN_00517250(png_ptr, s_Invalid_bit_depth_for_RGBA_image_00621778);
         *(unsigned char *)(png_ptr + 0x12a) = 4;
         break;
      default:
         FUN_00517250(png_ptr, s_Invalid_image_color_type_specifi_00621754);
   }

   if (compression_type != 0)
   {
      FUN_00517270(png_ptr, s_Invalid_compression_type_specifi_00621730);
      compression_type = 0;
   }

   if (filter_type != 0)
   {
      FUN_00517270(png_ptr, s_Invalid_filter_type_specified_00621710);
      filter_type = 0;
   }

   if (interlace_type != 0 &&
      interlace_type != 1)
   {
      FUN_00517270(png_ptr, s_Invalid_interlace_type_specified_006216ec);
      interlace_type = 1;
   }

   *(unsigned char *)(png_ptr + 0x127) = (unsigned char)bit_depth;
   *(unsigned char *)(png_ptr + 0x126) = (unsigned char)color_type;
   *(unsigned char *)(png_ptr + 0x123) = (unsigned char)interlace_type;
   *(unsigned int *)(png_ptr + 0xc8) = width;
   *(unsigned int *)(png_ptr + 0xcc) = height;

   *(unsigned char *)(png_ptr + 0x129) = (unsigned char)(bit_depth * *(unsigned char *)(png_ptr + 0x12a));
   *(unsigned int *)(png_ptr + 0xd8) = ((width * (unsigned int)*(unsigned char *)(png_ptr + 0x129) + 7) >> 3);
   *(unsigned int *)(png_ptr + 0xd4) = *(unsigned int *)(png_ptr + 0xc8);
   *(unsigned char *)(png_ptr + 0x128) = *(unsigned char *)(png_ptr + 0x127);
   *(unsigned char *)(png_ptr + 0x12b) = *(unsigned char *)(png_ptr + 0x12a);

   buf[0] = (unsigned char)((width >> 24) & 0xff);
   buf[1] = (unsigned char)((width >> 16) & 0xff);
   buf[2] = (unsigned char)((width >> 8) & 0xff);
   buf[3] = (unsigned char)(width & 0xff);
   buf[4 + 0] = (unsigned char)((height >> 24) & 0xff);
   buf[4 + 1] = (unsigned char)((height >> 16) & 0xff);
   buf[4 + 2] = (unsigned char)((height >> 8) & 0xff);
   buf[4 + 3] = (unsigned char)(height & 0xff);
   buf[8] = (unsigned char)bit_depth;
   buf[9] = (unsigned char)color_type;
   buf[10] = (unsigned char)compression_type;
   buf[11] = (unsigned char)filter_type;
   buf[12] = (unsigned char)interlace_type;

   {
      unsigned char buf[4];
      unsigned int length = 13;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_IHDR, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_IHDR, 4);
   }
   FUN_00516d50(png_ptr, buf, 13);
   FUN_00516bb0(png_ptr, buf, 13);
   {
      unsigned char buf[4];
      unsigned int crc = *(unsigned int *)(png_ptr + 0x110);
      buf[0] = (unsigned char)((crc >> 24) & 0xff);
      buf[1] = (unsigned char)((crc >> 16) & 0xff);
      buf[2] = (unsigned char)((crc >> 8) & 0xff);
      buf[3] = (unsigned char)(crc & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
   }

   *(void **)(png_ptr + 0x94) = (void *)FUN_00516cc0;
   *(void **)(png_ptr + 0x98) = (void *)FUN_00516d20;
   *(void **)(png_ptr + 0x9c) = (void *)png_ptr;
   if (!(*(unsigned char *)(png_ptr + 0x125)))
   {
      if (*(unsigned char *)(png_ptr + 0x126) == 3 ||
         *(unsigned char *)(png_ptr + 0x127) < 8)
         *(unsigned char *)(png_ptr + 0x125) = 0x08;
      else
         *(unsigned char *)(png_ptr + 0x125) = 0xf8;
   }
   if (!(*(unsigned int *)(png_ptr + 0x6c) & 1))
   {
      if (*(unsigned char *)(png_ptr + 0x125) != 0x08)
         *(int *)(png_ptr + 0xc4) = 1;
      else
         *(int *)(png_ptr + 0xc4) = 0;
   }
   if (!(*(unsigned int *)(png_ptr + 0x6c) & 2))
      *(int *)(png_ptr + 0xb4) = -1;
   if (!(*(unsigned int *)(png_ptr + 0x6c) & 4))
      *(int *)(png_ptr + 0xc0) = 8;
   if (!(*(unsigned int *)(png_ptr + 0x6c) & 8))
      *(int *)(png_ptr + 0xbc) = 15;
   if (!(*(unsigned int *)(png_ptr + 0x6c) & 0x10))
      *(int *)(png_ptr + 0xb8) = 8;
   FUN_00521110((void *)(png_ptr + 0x74), *(int *)(png_ptr + 0xb4),
      *(int *)(png_ptr + 0xb8), *(int *)(png_ptr + 0xbc),
      *(int *)(png_ptr + 0xc0), *(int *)(png_ptr + 0xc4), s_1_1_3_006214b0, 0x38);
   *(unsigned char **)(png_ptr + 0x80) = *(unsigned char **)(png_ptr + 0xac);
   *(unsigned int *)(png_ptr + 0x84) = *(unsigned int *)(png_ptr + 0xb0);

   *(unsigned int *)(png_ptr + 0x68) = 1;
}
