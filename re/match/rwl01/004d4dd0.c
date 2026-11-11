// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* RWC-MATCH-MOD40A 0x004d4dd0: 100% byte-identical under rw9044 (was 63.2% ddk raw g2s / h3). Rewritten with typed structs: list {state, count, entries[0x28], +0x20 mem, +0x24 cap}, entry obj vtable block at obj+4. Levers: err {1, code} as a struct, size > cap compare, Vt* local from &arr[i].obj->vt, global operand order 7d3ff8 + 911ae0 + 0x38. ddk scores lower (see RESULT). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
typedef struct Obj4dd0 Obj4dd0;
typedef struct Vt4dd0 { int a; int b; void (*f8)(Obj4dd0 *); int c; void (*f10)(void *); } Vt4dd0;
struct Obj4dd0 { int hdr; Vt4dd0 vt; char pad[0x3c - 0x18]; int refs; };
typedef struct Ent4dd0 { Obj4dd0 *obj; int a; int b; int c; char pad[0x28 - 0x10]; } Ent4dd0;
typedef struct Lst4dd0 { int state; uint count; Ent4dd0 *arr; int pad[5]; void *mem; uint cap; } Lst4dd0;
typedef struct Err4dd0 { int a; int b; } Err4dd0;
int FUN_004d4910(Lst4dd0 * p0, uint p1);
int FUN_004d4a20(Lst4dd0 * p0, Lst4dd0 * p1);
int FUN_004d7ff0(int p0, uint p1);
void FUN_004d8480(Err4dd0 * p0);
// MATCH: 0x004d4dd0 FUN_004d4dd0
Lst4dd0 * FUN_004d4dd0(Lst4dd0 *s)
{
  uint size;
  uint i;
  Err4dd0 err;

  if (s->state == 0) {
    size = *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38) * 0xb4;
    if (s->arr != 0) {
      if (size > s->cap) {
        if (!FUN_004d4910(s, size)) return 0;
      }
      if (!FUN_004d4a20(s, s)) return 0;
    }
    else {
      Ent4dd0 *p = (Ent4dd0 *)(*(void *(**)(uint, int))(DAT_007d3ff8 + 0x108))(size, 0x1030409);
      s->mem = p;
      if (p == 0) {
        err.a = 1;
        err.b = FUN_004d7ff0(0x80000013, size);
        FUN_004d8480(&err);
        return 0;
      }
      s->cap = size;
      s->arr = p;
    }
    s->state = 1;
    if (s->arr != 0) {
      for (i = 0; i < s->count; i++) {
        Vt4dd0 *vt = &s->arr[i].obj->vt;
        if (vt->f10) vt->f10(&s->arr[i]);
        s->arr[i].obj->refs--;
        if (s->arr[i].obj->refs == 0 && vt->f8) vt->f8(s->arr[i].obj);
        s->arr[i].c = 0;
      }
    }
  }
  return s;
}
