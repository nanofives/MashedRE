// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 h5 0x00498c00. See the file note below.
   Video-config dialog file: holds 10 functions in address order: FUN_00498a00 (static,
   Mode* in EBX), 00498c00, 00498d20 (static, HWND in EAX), 00498d60 (static), 00498e40, 00498ea0, 00498f60 (static,
   HWND in EAX + 3 stack args), 00499170 (static, HWND in EAX), 004991f0 (VideoSettingsDlgProc), 00499400. The same
   text is in each of these files (0x00498a00 0x00498c00 0x00498d20 0x00498d60 0x00498e40 0x00498ea0 0x00498f60
   0x00499170 0x004991f0 0x00499400). DAT_007731f8 is file-static (pilot finding 2: 0x00498c00 loads it before the
   store through DAT_00773408). Scores (ddk): 00498a00 96.7% instr, whole obj symbol BYTE-IDENTICAL (Ghidra size
   344 stops before the epilogue); 00498c00 00498d20 00498d60 00498e40 00498f60 00499170 004991f0 100% and
   byte-identical (004991f0 incl. both jump tables, 527 B); 00498ea0 91.1% (b); 00499400 90.1% (b).
   Moves found: 004991f0 needs `default: return 0;` FIRST in the msg switch, `break` + one trailing `return 1`, and
   empty `break` cases 0x3eb/0x3ec/0x3ed (signed `(unsigned short)wParam` switch); 00498f60's check boxes are
   if/else pairs of SendMessageA calls (tail-merged into `push 0 / jmp / push 1`), a ternary gives setcc;
   00498e40 needs `result = i; break;` and the count read in the loop condition; 00498c00 needs the mode count
   in a temp.
   00498ea0 residual (b): lea ebx for the FUN_00498a00 Mode* scheduled after `push 0x77330c`, and base/index swapped
   in 3 field loads. Tried: Mode* temp, i[p] / p+i / (int*) forms, unsigned mode index, VideoCfg struct for
   0x00773208, more file-statics, permuter 126 compiles.
   00499400 residual (b): inner listing loop loads DAT_00773408 before the `char line[64] = ""` fill and copies the
   offset counter through edi. Tried: {0} init, Mode* temp, p+j, outer-scope buffer, file-static DAT_00773408,
   permuter 186 compiles. C++ (VS2022 /TP): ok. */
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
typedef void *HWND; typedef void *HINSTANCE; typedef unsigned int UINT; typedef unsigned int WPARAM;
typedef long LPARAM; typedef long LRESULT; typedef int INT_PTR; typedef const char *LPCSTR;
typedef INT_PTR (__stdcall *DLGPROC)(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int id);
__declspec(dllimport) LRESULT __stdcall SendMessageA(HWND h, UINT m, WPARAM w, LPARAM l);
__declspec(dllimport) int __stdcall EndDialog(HWND h, INT_PTR r);
__declspec(dllimport) HWND __stdcall SetFocus(HWND h);
__declspec(dllimport) INT_PTR __stdcall DialogBoxParamA(HINSTANCE inst, LPCSTR tmpl, HWND parent, DLGPROC proc, LPARAM l);
__declspec(dllimport) long __stdcall SetWindowLongA(HWND h, int idx, long v);
__declspec(dllimport) int __stdcall SetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT f);
typedef struct Mode { int w; int h; int bpp; int flags; int r10; int r14; } Mode;
typedef struct SubSys { char name[80]; } SubSys;
typedef void (*RwStrCpyFn)(char *dst, const char *src);
#ifndef __cplusplus
static int *DAT_007731f8;
#else
MGLOBAL(int *, DAT_007731f8, 0x007731f8);
#endif
MGLOBAL(SubSys *, DAT_007731fc, 0x007731fc);
MGLOBAL(int, DAT_00773200, 0x00773200);
MGLOBAL(int, DAT_00773204, 0x00773204);
MGLOBAL(int, DAT_00773208, 0x00773208);
MGLOBALA(char, DAT_0077320c, 0x100, 0x0077320c);
MGLOBALA(char, DAT_0077330c, 0x80, 0x0077330c);
MGLOBAL(int, DAT_0077338c, 0x0077338c);
MGLOBAL(int, DAT_00773390, 0x00773390);
MGLOBAL(int, DAT_00773394, 0x00773394);
MGLOBAL(int, DAT_00773398, 0x00773398);
MGLOBAL(int, DAT_0077339c, 0x0077339c);
MGLOBAL(int, DAT_007733a0, 0x007733a0);
MGLOBAL(int, DAT_007733a4, 0x007733a4);
MGLOBAL(int, DAT_007733a8, 0x007733a8);
MGLOBAL(int, DAT_007733ac, 0x007733ac);
MGLOBAL(int, DAT_007733b0, 0x007733b0);
MGLOBAL(Mode **, DAT_00773408, 0x00773408);
MGLOBAL(int, DAT_0077340c, 0x0077340c);
MGLOBAL(int, DAT_00773410, 0x00773410);
MGLOBAL(int, DAT_00773414, 0x00773414);
MGLOBALA(int, DAT_00773418, 1, 0x00773418); /* [UNCERTAIN] extent: indexed by mode number */
MGLOBAL(char *, DAT_007719e8, 0x007719e8);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(HINSTANCE, DAT_007e9580, 0x007e9580);
MGLOBAL(HWND, DAT_007e9584, 0x007e9584);
MGLOBAL(int, DAT_00616028, 0x00616028);
MGLOBAL(int, DAT_0061602c, 0x0061602c);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(int, DAT_00636ad0, 0x00636ad0);
MGLOBAL(int, DAT_00636ad4, 0x00636ad4);
MGLOBAL(int, DAT_00636adc, 0x00636adc);
MGLOBAL(int, DAT_00636ae0, 0x00636ae0);
MGLOBAL(int, DAT_005ea098, 0x005ea098);
MGLOBALA(char, s_fmt_005d0170, 16, 0x005d0170);
MGLOBALA(char, s_Number_005d0244, 26, 0x005d0244);
MGLOBALA(char, s_fmt_005cd494, 4, 0x005cd494);
MGLOBALA(char, s_fmt_005d023c, 7, 0x005d023c);
MGLOBALA(char, s_Current_005d0224, 23, 0x005d0224);
MGLOBALA(char, s_Current_005d0210, 18, 0x005d0210);
char *strcat(char *, const char *);
#pragma intrinsic(strcat)
int FUN_004a2b60(char *o, const char *f, ...);
void *FUN_004a45fb(unsigned int n);
int FUN_004c2de0(void);
int FUN_004c2e10(SubSys *info, int i);
int FUN_004c2e40(void);
int FUN_004c2e70(int i);
int FUN_004c2ea0(void);
int FUN_004c2ed0(Mode *m, int i);
int FUN_004c2f00(void);
int FUN_004c2f30(int i);
void FUN_00499740(HWND h, char *text);
void FUN_00496400(const char *fmt, ...);
int FUN_00498950(void);
int FUN_004989b0(void);

// MATCH: 0x00498a00 FUN_00498a00
static void FUN_00498a00(Mode *m, char *out)
{
  char buf[128] = "";
  if (m->flags & 1)
    strcat(buf, "[Fullscreen]");
  else
    strcat(buf, "[Window]");
  if (m->flags & 2)
    strcat(buf, "[Interlaced]");
  if (m->flags & 4)
    strcat(buf, "[Flicker Free Interlaced]");
  if (m->flags & 0x100)
    strcat(buf, "[Fullscreen antialiasing mode 0]");
  if (m->flags & 0x200)
    strcat(buf, "[Fullscreen antialiasing mode 1]");
  FUN_004a2b60(out, s_fmt_005d0170, m->w, m->h, m->bpp, buf);
}

// MATCH: 0x00498c00 FUN_00498c00
void FUN_00498c00(void)
{
  int i, j, n;
  DAT_0077340c = FUN_004c2e40();
  DAT_00773200 = FUN_004c2f00();
  DAT_00773410 = FUN_004c2de0();
  DAT_007731fc = (SubSys *)FUN_004a45fb(DAT_00773410 * sizeof(SubSys));
  DAT_007731f8 = (int *)FUN_004a45fb(DAT_00773410 * sizeof(int));
  DAT_00773408 = (Mode **)FUN_004a45fb(DAT_00773410 * sizeof(Mode *));
  for (i = 0; i < DAT_00773410; i++) {
    FUN_004c2e70(i);
    FUN_004c2e10(&DAT_007731fc[i], i);
    n = FUN_004c2ea0();
    DAT_007731f8[i] = n;
    DAT_00773408[i] = (Mode *)FUN_004a45fb(n * sizeof(Mode));
    for (j = 0; j < DAT_007731f8[i]; j++)
      FUN_004c2ed0(&DAT_00773408[i][j], j);
  }
  FUN_004c2e70(DAT_0077340c);
  FUN_004c2f30(DAT_00773200);
}

// MATCH: 0x00498d20 FUN_00498d20
static void FUN_00498d20(HWND hDlg)
{
  HWND c = GetDlgItem(hDlg, 0x3e9);
  LRESULT sel = SendMessageA(c, 0x147, 0, 0);
  DAT_00773200 = SendMessageA(c, 0x150, sel, 0);
}

// MATCH: 0x00498d60 FUN_00498d60
static void FUN_00498d60(HWND wnd)
{
  char text[64];
  int i;
  for (i = 0; i < DAT_007731f8[DAT_0077340c]; i++) {
    Mode *m = &DAT_00773408[DAT_0077340c][i];
    if (!((m->flags == 0) & 1) && m->w >= 0x280 && m->h >= 0x1e0) {
      WPARAM idx;
      FUN_00498a00(m, text);
      idx = SendMessageA(wnd, 0x143, 0, (LPARAM)text);
      SendMessageA(wnd, 0x151, idx, i);
      DAT_00773418[i] = idx;
    }
  }
}

// MATCH: 0x00498e40 FUN_00498e40
int FUN_00498e40(void)
{
  int result = 0;
  int i;
  for (i = 0; i < DAT_007731f8[DAT_0077340c]; i++) {
    Mode *m = &DAT_00773408[DAT_0077340c][i];
    if (!((m->flags == 0) & 1) && m->w >= 0x280 && m->w >= 0x1e0) {
      result = i;
      break;
    }
  }
  return result;
}

// MATCH: 0x00498ea0 FUN_00498ea0
void FUN_00498ea0(void)
{
  DAT_0077338c = DAT_0077340c;
  DAT_00773208 = 5;
  DAT_00773390 = DAT_00773200;
  (*(RwStrCpyFn *)(DAT_007d3ff8 + 0xcc))(DAT_0077320c, DAT_007731fc[DAT_0077340c].name);
  FUN_00498a00(&DAT_00773408[DAT_0077340c][DAT_00773200], DAT_0077330c);
  DAT_00773394 = DAT_00773408[DAT_0077340c][DAT_00773200].w;
  DAT_00773398 = DAT_00773408[DAT_0077340c][DAT_00773200].h;
  DAT_0077339c = DAT_00773408[DAT_0077340c][DAT_00773200].bpp;
  DAT_007733a0 = DAT_00773408[DAT_0077340c][DAT_00773200].flags & 1;
}

// MATCH: 0x00498f60 FUN_00498f60
static void FUN_00498f60(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HWND sub, mode;
  int i;
  FUN_00499740(hDlg, DAT_007719e8 + 0x7e4);
  FUN_00499740(GetDlgItem(hDlg, 1), DAT_007719e8 + 0x7d0);
  FUN_00499740(GetDlgItem(hDlg, 2), DAT_007719e8 + 0x7da);
  FUN_00499740(GetDlgItem(hDlg, 0x41a), DAT_007719e8 + 0x8c0);
  FUN_00499740(GetDlgItem(hDlg, 0x417), DAT_007719e8 + 0x8ca);
  FUN_00499740(GetDlgItem(hDlg, 0x418), DAT_007719e8 + 0x8d4);
  FUN_00499740(GetDlgItem(hDlg, 0x419), DAT_007719e8 + 0x8de);
  FUN_00499740(GetDlgItem(hDlg, 0x41b), DAT_007719e8 + 0x8e8);
  sub = GetDlgItem(hDlg, 1000);
  mode = GetDlgItem(hDlg, 0x3e9);
  if (DAT_007733a8)
    SendMessageA(GetDlgItem(hDlg, 0x417), 0xf1, 0, 0);
  else
    SendMessageA(GetDlgItem(hDlg, 0x417), 0xf1, 1, 0);
  if (DAT_007733a4)
    SendMessageA(GetDlgItem(hDlg, 0x418), 0xf1, 0, 0);
  else
    SendMessageA(GetDlgItem(hDlg, 0x418), 0xf1, 1, 0);
  if (DAT_007733ac)
    SendMessageA(GetDlgItem(hDlg, 0x419), 0xf1, 1, 0);
  else
    SendMessageA(GetDlgItem(hDlg, 0x419), 0xf1, 0, 0);
  if (DAT_007733b0)
    SendMessageA(GetDlgItem(hDlg, 0x41b), 0xf1, 0, 0);
  else
    SendMessageA(GetDlgItem(hDlg, 0x41b), 0xf1, 1, 0);
  for (i = 0; i < DAT_00773410; i++) {
    SendMessageA(sub, 0x143, 0, (LPARAM)DAT_007731fc[i].name);
    SendMessageA(sub, 0x151, i, i);
  }
  SendMessageA(sub, 0x14e, DAT_0077340c, 0);
  FUN_00498d60(mode);
  SendMessageA(mode, 0x14e, DAT_00773418[DAT_00773200], 0);
  SetFocus(mode);
}

// MATCH: 0x00499170 FUN_00499170
static void FUN_00499170(HWND hDlg)
{
  HWND a = GetDlgItem(hDlg, 1000);
  HWND b = GetDlgItem(hDlg, 0x3e9);
  WPARAM sel = SendMessageA(a, 0x147, 0, 0);
  if (sel != (WPARAM)DAT_0077340c) {
    DAT_0077340c = SendMessageA(a, 0x150, sel, 0);
    FUN_004c2e70(DAT_0077340c);
    SendMessageA(b, 0x14b, 0, 0);
    FUN_00498d60(b);
    DAT_00773200 = FUN_004c2f00();
    SendMessageA(b, 0x14e, DAT_00773200, 0);
  }
}

// MATCH: 0x004991f0 FUN_004991f0
INT_PTR __stdcall FUN_004991f0(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  default:
    return 0;
  case 0x110:
    FUN_00498f60(hDlg, 0x110, wParam, lParam);
    break;
  case 0x111:
    switch ((unsigned short)wParam) {
    case 1:
      if ((unsigned short)(wParam >> 16) == 0) {
        FUN_00498d20(hDlg);
        EndDialog(hDlg, 1);
        return 1;
      }
      break;
    case 2:
      if ((unsigned short)(wParam >> 16) == 0) {
        EndDialog(hDlg, 0);
        return 1;
      }
      break;
    case 1000:
      FUN_00499170(hDlg);
      return 1;
    case 0x3e9:
      if ((unsigned short)(wParam >> 16) == 1) {
        FUN_00498d20(hDlg);
        return 1;
      }
      break;
    case 0x3eb:
      break;
    case 0x3ec:
      break;
    case 0x3ed:
      break;
    case 0x417:
      DAT_007733a8 = SendMessageA(GetDlgItem(hDlg, 0x417), 0xf0, 0, 0) == 0;
      return 1;
    case 0x418:
      DAT_007733a4 = SendMessageA(GetDlgItem(hDlg, 0x418), 0xf0, 0, 0) == 0;
      return 1;
    case 0x419:
      DAT_007733ac = SendMessageA(GetDlgItem(hDlg, 0x419), 0xf0, 0, 0);
      return 1;
    case 0x41b:
      DAT_007733b0 = SendMessageA(GetDlgItem(hDlg, 0x41b), 0xf0, 0, 0) == 0;
      return 1;
    default:
      return 0;
    }
    break;
  }
  return 1;
}

// MATCH: 0x00499400 FUN_00499400
int FUN_00499400(int showDialog)
{
  int i, j;
  Mode cur;
  FUN_00498c00();
  FUN_00496400(s_Number_005d0244, DAT_00773410);
  for (i = 0; i < DAT_00773410; i++) {
    FUN_00496400(s_fmt_005cd494, DAT_007731fc[i].name);
    for (j = 0; j < DAT_007731f8[i]; j++) {
      char line[64] = "";
      FUN_00498a00(&DAT_00773408[i][j], line);
      FUN_00496400(s_fmt_005d023c, j, line);
    }
  }
  if (showDialog) {
    if (FUN_00498950()) {
      DAT_0077340c = DAT_0077338c;
      DAT_00773200 = DAT_00773390;
    } else {
      DAT_007733a8 = 0;
      DAT_007733ac = 1;
      DAT_007733b0 = 0;
      DAT_007733a4 = 0;
    }
    if (!DialogBoxParamA(DAT_007e9580, (LPCSTR)0x65, DAT_007e9584, FUN_004991f0, 0))
      return 0;
    FUN_00498ea0();
    FUN_004989b0();
  } else if (FUN_00498950()) {
    DAT_0077340c = DAT_0077338c;
    DAT_00773200 = DAT_00773390;
  } else {
    DAT_0077340c = 0;
    DAT_00773200 = FUN_00498e40();
    FUN_00498ea0();
    FUN_004989b0();
  }
  FUN_004c2e70(DAT_0077340c);
  FUN_004c2f30(DAT_00773200);
  FUN_00496400(s_Current_005d0224, DAT_007731fc[DAT_0077340c].name);
  FUN_00496400(s_Current_005d0210, DAT_00773200);
  FUN_004c2ed0(&cur, DAT_00773200);
  if (cur.flags & 1) {
    DAT_00616028 = cur.w;
    DAT_00773204 = 1;
    DAT_0061602c = cur.h;
    DAT_00773414 = cur.bpp;
    SetWindowLongA(DAT_007e9584, -16, (long)0x80000000);
    SetWindowPos(DAT_007e9584, 0, 0, 0, 0, 0, 0x27);
  }
  DAT_00636ad4 = DAT_007733a4;
  DAT_00636ad0 = DAT_007733a4;
  DAT_00636acc = DAT_007733a8;
  DAT_00636ae0 = DAT_007733b0;
  DAT_00636adc = DAT_007733b0;
  DAT_005ea098 = DAT_007733ac;
  return 1;
}
