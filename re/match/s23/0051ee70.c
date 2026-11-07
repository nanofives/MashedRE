// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051ee70: libpng 1.0.12 pngwutil.c png_write_pCAL, png_write_chunk_start / _data / _end and
   png_save_int_32 written inline, png_strlen = intrinsic strlen.
   67.4% (regs 81.1, ops 91.6). purpose copied from purpose_arg (49.3 -> 67.4: frees its slot). Residual: the two
   png_strlen sites. The original's intrinsic strlen is repne scasb (string in edi), 9176 emits a byte loop, and that
   moves the edi/ebp/edx allocation of the params_len loop and the following chunk_data blocks. Single-param copy sweep
   over all 8 params done. Class (a), strlen codegen. */
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
extern "C" unsigned int __cdecl strlen(const char *s);
#else
unsigned int __cdecl strlen(const char *s);
#endif
#pragma intrinsic(strlen)
MGLOBALA(char, s_Unrecognized_equation_type_for_p_00621be0, 42, 0x00621be0);
MGLOBALA(unsigned char, png_pCAL, 5, 0x005e3274);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key);
void *FUN_005208c0(int png_ptr, unsigned int size);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051ee70 FUN_0051ee70
void FUN_0051ee70(int png_ptr, char *purpose_arg, int X0, int X1, int type, int nparams, char *units, char **params)
{
   char *purpose = purpose_arg;
   unsigned int purpose_len, units_len, total_len;
   unsigned int *params_len;
   unsigned char buf[10];
   char *new_purpose;
   int i;

   if (type >= 4)
      FUN_00517270(png_ptr, s_Unrecognized_equation_type_for_p_00621be0);

   purpose_len = FUN_0051e7f0(png_ptr, purpose, &new_purpose) + 1;
   units_len = strlen(units) + (nparams == 0 ? 0 : 1);
   total_len = purpose_len + units_len + 10;

   params_len = (unsigned int *)FUN_005208c0(png_ptr, (unsigned int)(nparams
      *sizeof(unsigned int)));

   for (i = 0; i < nparams; i++)
   {
      params_len[i] = strlen(params[i]) + (i == nparams - 1 ? 0 : 1);
      total_len += (unsigned int)params_len[i];
   }

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)total_len;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_pCAL, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_pCAL, 4);
   }
   {
      unsigned char *data = (unsigned char *)new_purpose;
      unsigned int length = purpose_len;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }
   buf[0] = (unsigned char)((X0 >> 24) & 0xff);
   buf[1] = (unsigned char)((X0 >> 16) & 0xff);
   buf[2] = (unsigned char)((X0 >> 8) & 0xff);
   buf[3] = (unsigned char)(X0 & 0xff);
   buf[4 + 0] = (unsigned char)((X1 >> 24) & 0xff);
   buf[4 + 1] = (unsigned char)((X1 >> 16) & 0xff);
   buf[4 + 2] = (unsigned char)((X1 >> 8) & 0xff);
   buf[4 + 3] = (unsigned char)(X1 & 0xff);
   buf[8] = (unsigned char)type;
   buf[9] = (unsigned char)nparams;
   FUN_00516d50(png_ptr, buf, 10);
   FUN_00516bb0(png_ptr, buf, 10);
   {
      unsigned char *data = (unsigned char *)units;
      unsigned int length = (unsigned int)units_len;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }

   FUN_00520930(png_ptr, new_purpose);

   for (i = 0; i < nparams; i++)
   {
      unsigned char *data = (unsigned char *)params[i];
      unsigned int length = (unsigned int)params_len[i];
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }

   FUN_00520930(png_ptr, params_len);
   {
      unsigned char buf[4];
      unsigned int crc = *(unsigned int *)(png_ptr + 0x110);
      buf[0] = (unsigned char)((crc >> 24) & 0xff);
      buf[1] = (unsigned char)((crc >> 16) & 0xff);
      buf[2] = (unsigned char)((crc >> 8) & 0xff);
      buf[3] = (unsigned char)(crc & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
   }
}
