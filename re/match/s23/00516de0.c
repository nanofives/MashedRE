// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00516de0: libpng 1.0.12 png.c png_free_data (PNG_FREE_ME_SUPPORTED, floating-point sCAL so no
   scal_s frees). png_info offsets from the decomp (free_me +0xb8, valid +8). The PLTE free calls png_free
   (0x00520930) directly, not png_zfree.
   Best 98.4% (regs 98.7%). Residual (a): partial shrink-wrap. Ours pushes ebx/edi, tests png_ptr, then pushes
   ebp/esi (and pops in a different order); the original pushes all four before the first test. Same
   family as 0x00519820 / 0x0051bb40. Every body row is equal. */
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
#define IP(T, off) (*(T *)((char *)info_ptr + (off)))
void FUN_00520930(void *png_ptr, void *ptr);
typedef struct { int compression; char *key; char *text; int text_length; } png_text;
typedef struct { char *name; int depth; void *entries; int nentries; } png_sPLT_t;
typedef struct { unsigned char name[5]; unsigned char pad[3]; unsigned char *data; unsigned int size;
   unsigned char location; } png_unknown_chunk;
// MATCH: 0x00516de0 FUN_00516de0
void FUN_00516de0(void *png_ptr, void *info_ptr, unsigned int mask, int num)
{
   if (png_ptr == 0 || info_ptr == 0)
      return;

if ((mask & 0x4000) & IP(unsigned int, 0xb8))
{
   if (num != -1)
   {
     if (IP(png_text *, 0x38) && IP(png_text *, 0x38)[num].key)
     {
         FUN_00520930(png_ptr, IP(png_text *, 0x38)[num].key);
         IP(png_text *, 0x38)[num].key = 0;
     }
   }
   else
   {
       int i;
       for (i = 0; i < IP(int, 0x30); i++)
           FUN_00516de0(png_ptr, info_ptr, 0x4000, i);
       FUN_00520930(png_ptr, IP(png_text *, 0x38));
       IP(png_text *, 0x38) = 0;
       IP(int, 0x30)=0;
   }
}

if ((mask & 0x2000) & IP(unsigned int, 0xb8))
{
    FUN_00520930(png_ptr, IP(void *, 0x4c));
    IP(unsigned int, 8) &= ~0x0010;
    IP(void *, 0x4c) = 0;
}

if ((mask & 0x0100) & IP(unsigned int, 0xb8))
{
    IP(unsigned int, 8) &= ~0x4000;
}

if ((mask & 0x0080) & IP(unsigned int, 0xb8))
{
    FUN_00520930(png_ptr, IP(void *, 0xa0));
    FUN_00520930(png_ptr, IP(void *, 0xac));
    IP(void *, 0xa0) = 0;
    IP(void *, 0xac) = 0;
    if (IP(void **, 0xb0) != 0)
    {
        int i;
        for (i = 0; i < (int)IP(unsigned char, 0xb5); i++)
        {
          FUN_00520930(png_ptr, IP(void **, 0xb0)[i]);
          IP(void **, 0xb0)[i]=0;
        }
        FUN_00520930(png_ptr, IP(void **, 0xb0));
        IP(void **, 0xb0) = 0;
    }
    IP(unsigned int, 8) &= ~0x0400;
}

if ((mask & 0x0010) & IP(unsigned int, 0xb8))
{
    FUN_00520930(png_ptr, IP(void *, 0xc4));
    FUN_00520930(png_ptr, IP(void *, 0xc8));
    IP(void *, 0xc4) = 0;
    IP(void *, 0xc8) = 0;
    IP(unsigned int, 8) &= ~0x1000;
}

if ((mask & 0x0020) & IP(unsigned int, 0xb8))
{
   if (num != -1)
   {
      if(IP(png_sPLT_t *, 0xd4))
      {
          FUN_00520930(png_ptr, IP(png_sPLT_t *, 0xd4)[num].name);
          FUN_00520930(png_ptr, IP(png_sPLT_t *, 0xd4)[num].entries);
          IP(png_sPLT_t *, 0xd4)[num].name = 0;
          IP(png_sPLT_t *, 0xd4)[num].entries = 0;
      }
   }
   else
   {
       if(IP(int, 0xd8))
       {
         int i;
         for (i = 0; i < (int)IP(int, 0xd8); i++)
            FUN_00516de0(png_ptr, info_ptr, 0x0020, i);

         FUN_00520930(png_ptr, IP(png_sPLT_t *, 0xd4));
         IP(png_sPLT_t *, 0xd4) = 0;
         IP(int, 0xd8) = 0;
       }
       IP(unsigned int, 8) &= ~0x2000;
   }
}

if ((mask & 0x0200) & IP(unsigned int, 0xb8))
{
   if (num != -1)
   {
       if(IP(png_unknown_chunk *, 0xbc))
       {
          FUN_00520930(png_ptr, IP(png_unknown_chunk *, 0xbc)[num].data);
          IP(png_unknown_chunk *, 0xbc)[num].data = 0;
       }
   }
   else
   {
       int i;

       if(IP(int, 0xc0))
       {
         for (i = 0; i < (int)IP(int, 0xc0); i++)
            FUN_00516de0(png_ptr, info_ptr, 0x0200, i);

         FUN_00520930(png_ptr, IP(png_unknown_chunk *, 0xbc));
         IP(png_unknown_chunk *, 0xbc) = 0;
         IP(int, 0xc0) = 0;
       }
   }
}

if ((mask & 0x0008)  & IP(unsigned int, 0xb8))
{
    FUN_00520930(png_ptr, IP(void *, 0x7c));
    IP(void *, 0x7c) = 0;
    IP(unsigned int, 8) &= ~0x0040;
}

if ((mask & 0x1000) & IP(unsigned int, 0xb8))
{
    FUN_00520930(png_ptr, IP(void *, 0x10));
    IP(void *, 0x10) = 0;
    IP(unsigned int, 8) &= ~0x0008;
    IP(unsigned short, 0x14) = 0;
}

if ((mask & 0x0040) & IP(unsigned int, 0xb8))
{
    if(IP(void **, 0xf8))
    {
       int row;
       for (row = 0; row < (int)IP(unsigned int, 4); row++)
       {
          FUN_00520930(png_ptr, IP(void **, 0xf8)[row]);
          IP(void **, 0xf8)[row]=0;
       }
       FUN_00520930(png_ptr, IP(void **, 0xf8));
       IP(void **, 0xf8)=0;
    }
    IP(unsigned int, 8) &= ~0x8000;
}

   if(num == -1)
     IP(unsigned int, 0xb8) &= ~mask;
   else
     IP(unsigned int, 0xb8) &= ~(mask & ~0x4220);
}
