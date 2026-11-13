// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (53.8% rw9044) to 100%: user-data plugin stream read (offset param_4 in param_3): read count (FUN_004cc790, 4 bytes), malloc count*16 hint 0x3011f, then per array an inline reader: name length, name bytes (FUN_004cbd30), format, numElements, then switch: 1 and 2 malloc n*4 and read n*4, 3 malloc n*4 and per string read length then bytes (length < 1 stores 0), other formats fail. The running stream becomes 0 on a failure, the function returns param_1. FUN_004cc791_site2 is the case-2 read: in the original that call also targets 0x004cc790 (verified in the match.py listing), but c2 merges two identical call sites, so the case-2 call needs its own extern name; relocheck reports it unchecked. */
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef int code();
typedef unsigned char bool; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT; typedef void *HWND;
typedef void *HINSTANCE; typedef void *HMODULE; typedef void *HANDLE; typedef int BOOL; typedef unsigned int UINT;
typedef unsigned long DWORD; typedef char *LPSTR; typedef const char *LPCSTR; typedef void *LPVOID; typedef unsigned short WORD;
typedef unsigned char BYTE; typedef long LONG; typedef void *HDC; typedef void *HMENU; typedef void *HICON;
typedef unsigned short wchar_t; typedef wchar_t *LPWSTR; typedef const wchar_t *LPCWSTR; typedef int INT_PTR;
typedef unsigned int UINT_PTR; typedef long LONG_PTR; typedef unsigned long ULONG_PTR; typedef long HRESULT;
typedef long double unkbyte10; typedef unsigned int uintptr_t; typedef unsigned int size_t;
#define true 1
#define false 0
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
#define SUB41(a,n) ((uchar)((uint)(a)>>((n)*8)))
#define SUB42(a,n) ((ushort)((uint)(a)>>((n)*8)))
#define SUB81(a,n) ((uchar)((ulonglong)(a)>>((n)*8)))
#define SUB84(a,n) ((uint)((ulonglong)(a)>>((n)*8)))
#define ZEXT14(a) ((uint)(uchar)(a))
#define ZEXT24(a) ((uint)(ushort)(a))
#define ZEXT48(a) ((ulonglong)(uint)(a))
#define SEXT14(a) ((int)(signed char)(a))
#define SEXT24(a) ((int)(short)(a))
#define SEXT48(a) ((longlong)(int)(a))

extern int DAT_007d3ff8;
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
// MATCH: 0x005444a0 FUN_005444a0

typedef struct { char *name; int format; int numElements; void *data; } UDArr44;
typedef struct { int numArrays; UDArr44 *arrays; } UDExt44;
typedef void *(*mallocfn44)(int, int);
#define UDMalloc44(sz) (((mallocfn44)(*(void **)(DAT_007d3ff8 + 0x108)))((sz), 0x3011f))
int FUN_004cc791_site2(int, void *, int);
static __inline int UDArrRead44(int stream, UDArr44 *a)
{
  int len;
  int j;
  if (!FUN_004cc790(stream, &len, 4)) return 0;
  if (len > 0) {
    a->name = (char *)UDMalloc44(len);
    if (!a->name || !FUN_004cbd30((undefined4 *)stream, (undefined4 *)a->name, len)) return 0;
  }
  if (!FUN_004cc790(stream, &a->format, 4)) return 0;
  if (!FUN_004cc790(stream, &a->numElements, 4)) return 0;
  switch (a->format) {
  case 1:
    a->data = UDMalloc44(a->numElements << 2);
    if (!a->data) return 0;
    if (!FUN_004cc790(stream, a->data, a->numElements << 2)) return 0;
    break;
  case 2:
    a->data = UDMalloc44(a->numElements << 2);
    if (!a->data) return 0;
    if (!FUN_004cc791_site2(stream, a->data, a->numElements << 2)) return 0;
    break;
  case 3: {
    char **s;
    s = (char **)UDMalloc44(a->numElements << 2);
    a->data = s;
    if (!s) return 0;
    for (j = 0; j < a->numElements; j++) {
      if (!FUN_004cc790(stream, &len, 4)) return 0;
      if (len > 0) {
        s[j] = (char *)UDMalloc44(len);
        if (!s[j] || !FUN_004cbd30((undefined4 *)stream, (undefined4 *)s[j], len)) return 0;
      }
      else {
        s[j] = 0;
      }
    }
    break;
  }
  default:
    return 0;
  }
  return stream;
}
undefined4 FUN_005444a0(undefined4 param_1,undefined4 param_2,int param_3,int param_4)
{
  UDExt44 *ext = (UDExt44 *)(param_3 + param_4);
  int stream = param_1;
  int i;
  int n;
  if (ext && FUN_004cc790(param_1, &n, 4)) {
    ext->numArrays = n;
    ext->arrays = (UDArr44 *)UDMalloc44(n << 4);
    for (i = 0; i < ext->numArrays; i++) {
      stream = UDArrRead44(stream, &ext->arrays[i]);
    }
  }
  return param_1;
}
