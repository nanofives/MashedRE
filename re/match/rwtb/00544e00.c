// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: from g2s (59.8% rw9044) to 100% under rw9044 (ddk 97.9%: fdivr order): plugin block at obj + DAT_007dc66c = {data, f4, i8}. If src block i8 != src+0x18, zero the dst block and return dst. Else copy the three fields one by one, and when data != 0 set +0x50 (short data+4), +0x52 (short data+6), +0x54 (float data+8, through a block-scope float), +0x58 = 1.0f / (0, data+8), +0x5c (block f4); after each set flags +0x4c |= 3 and FUN_004c0e50(obj+4) unless bit 4 is set or obj+4 is 0. */
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

extern int DAT_007dc66c;
extern undefined1 DAT_005cc320;
void FUN_004c0e50(int p0);
extern int _DAT_005cc320;
// MATCH: 0x00544e00 FUN_00544e00

typedef struct { char *data; float f4; int i8; } Ext4e00;
typedef struct { int pad0; int frame; int pad8[4]; int i18; int pad1c[12]; uint flags; short s50; short s52; float f54; float f58; float f5c; } Obj4e00;
#define EXT4E00(o) ((Ext4e00 *)((char *)(o) + DAT_007dc66c))
static __inline void Mark4e00(Obj4e00 *o)
{
  o->flags |= 3;
  if (!(o->flags & 4) && o->frame) FUN_004c0e50(o->frame);
}
Obj4e00 *FUN_00544e00(Obj4e00 *dst, Obj4e00 *src)
{
  Ext4e00 *d = EXT4E00(dst);
  Ext4e00 *s = EXT4E00(src);
  char *p;
  if (s->i8 != src->i18) {
    d->data = 0;
    *(int *)&d->f4 = 0;
    d->i8 = 0;
    return dst;
  }
  d->data = s->data;
  *(int *)&d->f4 = *(int *)&s->f4;
  d->i8 = s->i8;
  if (d->data) {
    dst->s50 = *(short *)(d->data + 4);
    Mark4e00(dst);
    dst->s52 = *(short *)(d->data + 6);
    Mark4e00(dst);
    {
      float r = *(float *)(d->data + 8);
      dst->f54 = r;
    }
    dst->f58 = 1.0f / (0, *(float *)(d->data + 8));
    Mark4e00(dst);
    {
      float r = d->f4;
      dst->f5c = r;
    }
    Mark4e00(dst);
  }
  return dst;
}
