// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005173f0: libpng 1.0.12 pngread.c png_create_read_struct_2 (PNG_USER_MEM_SUPPORTED,
   PNG_NO_STDIO: no sprintf/png_warning blocks). png_libpng_ver at 0x005e3200, jmpbuf at +0, zstream +0x74,
   zbuf +0xac, zbuf_size +0xb0. setjmp = _setjmp (compiles to _setjmp3(buf, 0)). Ghidra size 437 ends at
   the switch table.
   Best 83.8% (regs 83.8%, ops 95.5%). Residual (a): the original gives the first `return NULL` its own
   epilogue copy (pop x3 / mov esp,ebp / pop ebp / ret, returning eax = 0), ours jumps into the setjmp
   path epilogue (cross-jumping). Every later row is equal; the score loss is jump displacements only.
   Tried return png_ptr, split assignment, permuter 115 compiles. Same family as 0x00516c40. */
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
MGLOBALA(char, png_libpng_ver, 18, 0x005e3200);
MGLOBALA(char, s_Application_must_be_recompiled__v_00620e90, 68, 0x00620e90);
MGLOBALA(char, s_Incompatible_libpng_version_in_a_00620ed4, 55, 0x00620ed4);
MGLOBALA(char, s_Unknown_zlib_error_00621474, 19, 0x00621474);
MGLOBALA(char, s_zlib_version_error_00621488, 19, 0x00621488);
MGLOBALA(char, s_zlib_memory_error_0062149c, 18, 0x0062149c);
MGLOBALA(char, s_1_1_3_006214b0, 6, 0x006214b0);
#ifdef __cplusplus
extern "C"
#endif
int __cdecl _setjmp(int *buf);
void *FUN_005207c0(int type, void *malloc_fn, void *mem_ptr);
void FUN_00520930(int png_ptr, void *ptr);
void FUN_00520850(void *ptr);
void FUN_005209d0(int png_ptr, void *mem_ptr, void *malloc_fn, void *free_fn);
void FUN_005173d0(int png_ptr, void *error_ptr, void *error_fn, void *warn_fn);
void FUN_00517250(int png_ptr, const char *msg);
void *FUN_005208c0(int png_ptr, unsigned int size);
void *FUN_00516cc0(void *png_ptr, unsigned int items, unsigned int size);
void FUN_00516d20(void *png_ptr, void *ptr);
int FUN_00522cc0(void *strm, const char *version, int stream_size);
void FUN_0051ca10(int png_ptr, void *io_ptr, void *read_data_fn);
// MATCH: 0x005173f0 FUN_005173f0
int FUN_005173f0(const char *user_png_ver, void *error_ptr, void *error_fn, void *warn_fn, void *mem_ptr,
   void *malloc_fn, void *free_fn)
{
   int png_ptr;
   int i;

   if ((png_ptr = (int)FUN_005207c0(1, malloc_fn, mem_ptr)) == 0)
   {
      return 0;
   }

   if (_setjmp((int *)png_ptr))
   {
      FUN_00520930(png_ptr, *(void **)(png_ptr + 0xac));
      *(void **)(png_ptr + 0xac)=0;
      FUN_00520850((void *)png_ptr);
      return 0;
   }

   FUN_005209d0(png_ptr, mem_ptr, malloc_fn, free_fn);

   FUN_005173d0(png_ptr, error_ptr, error_fn, warn_fn);

   i=0;
   do
   {
     if(user_png_ver[i] != png_libpng_ver[i])
        *(unsigned int *)(png_ptr + 0x6c) |= 0x20000;
   } while (png_libpng_ver[i++]);

   if (*(unsigned int *)(png_ptr + 0x6c) & 0x20000)
   {
     if (user_png_ver == 0 || user_png_ver[0] != png_libpng_ver[0] ||
         (user_png_ver[0] == '1' && user_png_ver[2] != png_libpng_ver[2]) ||
         (user_png_ver[0] == '0' && user_png_ver[2] < '9'))
     {
        FUN_00517250(png_ptr,
           s_Incompatible_libpng_version_in_a_00620ed4);
     }

     if (user_png_ver[0] == '1' && user_png_ver[2] == '0' &&
         (user_png_ver[4] <  '2' || user_png_ver[4] == '6') &&
         user_png_ver[5] == '\0')
     {
        FUN_00517250(png_ptr,
        s_Application_must_be_recompiled__v_00620e90);
     }
   }

   *(unsigned int *)(png_ptr + 0xb0) = 0x2000;
   *(void **)(png_ptr + 0xac) = FUN_005208c0(png_ptr,
     (unsigned int)*(unsigned int *)(png_ptr + 0xb0));
   *(void **)(png_ptr + 0x94) = (void *)FUN_00516cc0;
   *(void **)(png_ptr + 0x98) = (void *)FUN_00516d20;
   *(void **)(png_ptr + 0x9c) = (void *)png_ptr;

   switch (FUN_00522cc0((void *)(png_ptr + 0x74), s_1_1_3_006214b0, 0x38))
   {
     case 0: break;
     case -4:
     case -2: FUN_00517250(png_ptr, s_zlib_memory_error_0062149c); break;
     case -6: FUN_00517250(png_ptr, s_zlib_version_error_00621488); break;
     default: FUN_00517250(png_ptr, s_Unknown_zlib_error_00621474);
   }

   *(void **)(png_ptr + 0x80) = *(void **)(png_ptr + 0xac);
   *(unsigned int *)(png_ptr + 0x84) = (unsigned int)*(unsigned int *)(png_ptr + 0xb0);

   FUN_0051ca10(png_ptr, 0, 0);

   return (png_ptr);
}
