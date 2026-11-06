// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004944c0 (h1): best 59.2% (regs 54.4%) in the required C form, class (c) C++ EH.
   Sibling of 0x00494c80 (same graph setup, movie path table 0x006147c4[param_1], renderer constructor
   FUN_00493c00, a 4th QueryInterface into 0x00771a20, IMediaControl::Run (+0x1c) on 0x00771a34, then
   0x00771a00 = 0, 0x006147d8 = 1). Unlike 0x00494c80 its two smart-pointer locals have inline destructors
   inside a C++ EH frame (handler 0x005ca30b, try state bytes 1 / 2 at [ebp-4], state 2 around operator new),
   which C under the DDK cannot emit. The body uses the 0x00494c80 forms (stdcall A2W helper, float / struct
   call-site prototype for the __thiscall constructor) with explicit Release. Residual: the EH frame and the
   state stores. Not tried as C++ (budget). */
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
typedef unsigned short wchar;
typedef int (__stdcall *QIFn)(int *self, void *iid, void *out);
typedef int (__stdcall *AddRefFn)(int *self);
typedef int (__stdcall *AddFilterFn)(int *self, int *filter, const wchar *name);
typedef int (__stdcall *AddSourceFilterFn)(int *self, const wchar *file, const wchar *name, int **out);
typedef int (__stdcall *FindPinFn)(int *self, const wchar *id, int **out);
typedef int (__stdcall *RenderFn)(int *self, int *pin);
typedef uint (__stdcall *AcpFn)(void);
MGLOBAL(int, DAT_00771a00, 0x00771a00);
MGLOBAL(int, DAT_00771a1c, 0x00771a1c);
MGLOBAL(int *, DAT_00771a20, 0x00771a20);
MGLOBAL(int *, DAT_00771a24, 0x00771a24);
MGLOBAL(int *, DAT_00771a28, 0x00771a28);
MGLOBAL(int *, DAT_00771a2c, 0x00771a2c);
MGLOBAL(int *, DAT_00771a30, 0x00771a30);
MGLOBAL(int *, DAT_00771a34, 0x00771a34);
MGLOBAL(int, DAT_006147d8, 0x006147d8);
MGLOBALA(char *, PTR_s_toastart_pc_movies_frontend_mpg_006147c4, 5, 0x006147c4);
MGLOBALA(uint, DAT_005cfad4, 4, 0x005cfad4);
MGLOBALA(char, s_Could_not_run_the_DirectShow_gra_005cfb18, 45, 0x005cfb18);
typedef int (__stdcall *RunFn)(int *self);
MGLOBAL(AcpFn, PTR_FUN_006147dc, 0x006147dc);
MGLOBALA(uint, DAT_005cfac4, 4, 0x005cfac4);
MGLOBALA(uint, DAT_005d0abc, 4, 0x005d0abc);
MGLOBALA(uint, DAT_005d0bbc, 4, 0x005d0bbc);
MGLOBALA(uint, DAT_005d0bcc, 4, 0x005d0bcc);
MGLOBALA(uint, DAT_005d0c4c, 4, 0x005d0c4c);
MGLOBALA(wchar, u_TEXTURERENDERER_005cfc70, 16, 0x005cfc70);
MGLOBALA(wchar, u_SOURCE_005cfc2c, 7, 0x005cfc2c);
MGLOBALA(wchar, u_Output_005cfb9c, 7, 0x005cfb9c);
MGLOBALA(char, s_Could_not_add_renderer_filter_to_005cfc3c, 49, 0x005cfc3c);
MGLOBALA(char, s_Could_not_add_source_filter_to_g_005cfbe0, 73, 0x005cfbe0);
MGLOBALA(char, s_Could_not_add_source_filter_to_g_005cfbac, 47, 0x005cfbac);
MGLOBALA(char, s_Could_not_find_output_pin___hr_0_005cfb78, 36, 0x005cfb78);
MGLOBALA(char, s_Could_not_render_source_output_p_005cfb48, 45, 0x005cfb48);
MGLOBALA(char, s_Could_not_create_texture_render_005cfae4, 51, 0x005cfae4);
__declspec(dllimport) int __stdcall CoCreateInstance(void *rclsid, void *pUnkOuter, uint dwClsContext, void *riid, void *ppv);
__declspec(dllimport) char * __stdcall lstrcpyA(char *lpString1, const char *lpString2);
__declspec(dllimport) int __stdcall lstrlenA(const char *lpString);
#ifdef __cplusplus
extern "C"
#endif
void * __cdecl _alloca(unsigned int size);
#pragma intrinsic(_alloca)
wchar * __cdecl FUN_004a3bdf(wchar *dst, const wchar *src, unsigned int n); /* CRT wcsncpy */
void * __cdecl FUN_004a3c1c(unsigned int size); /* operator new */
void FUN_00493f00(const char *fmt, ...);
wchar * __stdcall FUN_00493bc0(wchar *dst, const char *src, int n, uint acp);
typedef struct { int v; } ArgI4;
typedef struct { int *p; } ArgP4;
/* thiscall callee: ECX = this, the stack args passed as float / 4-byte struct so __fastcall leaves EDX alone */
int *__fastcall FUN_00493c00(void *self, float unk, ArgP4 phr);
#define S16_REL_4C0(p) if ((p) != 0) (*(AddRefFn *)(*(p) + 8))(p)
// MATCH: 0x004944c0 FUN_004944c0
int FUN_004944c0(int param_1)
{
  int hr;
  int *src;
  int *pin;
  char file[260];
  wchar wfile[260];
  void *mem;
  int *obj;
  int *filt;
  int r;
  ArgP4 b;
  hr = 0;
  src = 0;
  pin = 0;
  DAT_00771a1c = param_1;
  if (CoCreateInstance(DAT_005d0c4c, 0, 3, DAT_005d0abc, &DAT_00771a30) >= 0) {
    mem = FUN_004a3c1c(0x170);
    if (mem != 0) {
      b.p = &hr;
      obj = FUN_00493c00(mem, 0.0f, b);
    } else {
      obj = 0;
    }
    if (hr >= 0 && obj != 0) {
      filt = obj + 3;
      if (filt != 0) {
        (*(AddRefFn *)(*filt + 4))(filt);
      }
      if (DAT_00771a24 != 0) {
        (*(AddRefFn *)(*DAT_00771a24 + 8))(DAT_00771a24);
      }
      DAT_00771a24 = filt;
      hr = (*(AddFilterFn *)(*DAT_00771a30 + 0xc))(DAT_00771a30, filt, u_TEXTURERENDERER_005cfc70);
      if (hr < 0) {
        FUN_00493f00(s_Could_not_add_renderer_filter_to_005cfc3c, hr);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      lstrcpyA(file, PTR_s_toastart_pc_movies_frontend_mpg_006147c4[param_1]);
      file[259] = 0;
      wfile[259] = 0;
      {
        uint acp = PTR_FUN_006147dc();
        int n = lstrlenA(file) + 1;
        wchar *w = FUN_00493bc0((wchar *)_alloca(n * 2), file, n, acp);
        FUN_004a3bdf(wfile, w, 0x104);
      }
      hr = (*(AddSourceFilterFn *)(*DAT_00771a30 + 0x38))(DAT_00771a30, wfile, u_SOURCE_005cfc2c, &src);
      if (hr == 0x80040216) {
        FUN_00493f00(s_Could_not_add_source_filter_to_g_005cfbe0, file);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      if (hr < 0) {
        FUN_00493f00(s_Could_not_add_source_filter_to_g_005cfbac, hr);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      hr = (*(FindPinFn *)(*src + 0x2c))(src, u_Output_005cfb9c, &pin);
      if (hr < 0) {
        FUN_00493f00(s_Could_not_find_output_pin___hr_0_005cfb78, hr);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      hr = (*(RenderFn *)(*DAT_00771a30 + 0x30))(DAT_00771a30, pin);
      if (hr < 0) {
        FUN_00493f00(s_Could_not_render_source_output_p_005cfb48, hr);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      (*(QIFn *)*DAT_00771a30)(DAT_00771a30, DAT_005d0bcc, &DAT_00771a34);
      (*(QIFn *)*DAT_00771a30)(DAT_00771a30, DAT_005d0bbc, &DAT_00771a2c);
      (*(QIFn *)*DAT_00771a30)(DAT_00771a30, DAT_005cfac4, &DAT_00771a28);
      hr = (*(QIFn *)*DAT_00771a30)(DAT_00771a30, DAT_005cfad4, &DAT_00771a20);
      hr = (*(RunFn *)(*DAT_00771a34 + 0x1c))(DAT_00771a34);
      if (hr < 0) {
        FUN_00493f00(s_Could_not_run_the_DirectShow_gra_005cfb18, hr);
        r = hr; S16_REL_4C0(pin); S16_REL_4C0(src); return r;
      }
      DAT_00771a00 = 0;
      DAT_006147d8 = 1;
      S16_REL_4C0(pin); S16_REL_4C0(src);
      return 0;
    }
    FUN_00493f00(s_Could_not_create_texture_render_005cfae4, hr);
  }
  S16_REL_4C0(pin);
  S16_REL_4C0(src);
  return 0x80004005;
}
#undef S16_REL_4C0
