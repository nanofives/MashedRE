// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051e7f0: libpng 1.0.12 pngwutil.c png_check_keyword (PNG_NO_STDIO form of the bad-character
   warning), png_strlen = intrinsic strlen.
   54.4% (regs 78.8, ops 90.6). Residual: png_strlen(key). The original's intrinsic strlen is repne scasb (key in edi),
   9176 emits a byte loop, and that moves the whole register allocation (key/new_key/key_len in ebx/ebp/edi vs ours
   edi/esi/ebx). The control flow and every block line up. Permuter 200 s: 56.6% only by moving calls past the
   stores they precede (semantics change, rejected). Class (a), strlen codegen.
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
MGLOBALA(char, s_invalid_character_in_keyword_00621b60, 29, 0x00621b60);
MGLOBALA(char, s_trailing_spaces_removed_from_key_00621b38, 37, 0x00621b38);
MGLOBALA(char, s_leading_spaces_removed_from_keyw_00621b14, 36, 0x00621b14);
MGLOBALA(char, s_extra_interior_spaces_removed_fr_00621ae8, 43, 0x00621ae8);
MGLOBALA(char, s_Zero_length_keyword_00621ad4, 20, 0x00621ad4);
MGLOBALA(char, s_keyword_length_must_be_1___79_ch_00621aa8, 41, 0x00621aa8);
MGLOBALA(char, s_zero_length_keyword_00621a94, 20, 0x00621a94);
void FUN_00517270(int png_ptr, const char *msg);
void *FUN_005208c0(int png_ptr, unsigned int size);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051e7f0 FUN_0051e7f0
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key)
{
   unsigned int key_len;
   char *kp, *dp;
   int kflag;
   int kwarn = 0;

   *new_key = 0;

   if (key == 0 || (key_len = strlen(key)) == 0)
   {
      FUN_00517270(png_ptr, s_zero_length_keyword_00621a94);
      return ((unsigned int)0);
   }

   *new_key = (char *)FUN_005208c0(png_ptr, (unsigned int)(key_len + 2));

   for (kp = key, dp = *new_key; *kp != '\0'; kp++, dp++)
   {
      if (*kp < 0x20 || (*kp > 0x7E && (unsigned char)*kp < 0xA1))
      {
         FUN_00517270(png_ptr, s_invalid_character_in_keyword_00621b60);
         *dp = ' ';
      }
      else
      {
         *dp = *kp;
      }
   }
   *dp = '\0';

   kp = *new_key + key_len - 1;
   if (*kp == ' ')
   {
      FUN_00517270(png_ptr, s_trailing_spaces_removed_from_key_00621b38);

      while (*kp == ' ')
      {
        *(kp--) = '\0';
        key_len--;
      }
   }

   kp = *new_key;
   if (*kp == ' ')
   {
      FUN_00517270(png_ptr, s_leading_spaces_removed_from_keyw_00621b14);

      while (*kp == ' ')
      {
        kp++;
        key_len--;
      }
   }

   for (kflag = 0, dp = *new_key; *kp != '\0'; kp++)
   {
      if (*kp == ' ' && kflag == 0)
      {
         *(dp++) = *kp;
         kflag = 1;
      }
      else if (*kp == ' ')
      {
         key_len--;
         kwarn = 1;
      }
      else
      {
         *(dp++) = *kp;
         kflag = 0;
      }
   }
   *dp = '\0';
   if (kwarn)
      FUN_00517270(png_ptr, s_extra_interior_spaces_removed_fr_00621ae8);

   if (key_len == 0)
   {
      FUN_00520930(png_ptr, *new_key);
      *new_key = 0;
      FUN_00517270(png_ptr, s_Zero_length_keyword_00621ad4);
   }

   if (key_len > 79)
   {
      FUN_00517270(png_ptr, s_keyword_length_must_be_1___79_ch_00621aa8);
      new_key[79] = '\0';
      key_len = 79;
   }

   return (key_len);
}
