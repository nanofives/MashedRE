// FLAGS: /O2 /GS /Ob1
/* a2 0x00403660 (54 B). Best: 100%.
   g2s 81.2%: vtable slot +8 called as cdecl (extra add esp). Declared as a __stdcall COM-style slot. */
typedef struct Obj Obj;
typedef struct { void *s0; void *s1; void (__stdcall *Release)(Obj *); } ObjVtbl;
struct Obj { ObjVtbl *vt; };
extern Obj *DAT_00636b70;
extern int DAT_00636b78;
int FUN_004c7650(int p0);

// MATCH: 0x00403660 FUN_00403660
void FUN_00403660(void)
{
  if (DAT_00636b70 != 0) {
    DAT_00636b70->vt->Release(DAT_00636b70);
    DAT_00636b70 = 0;
  }
  if (DAT_00636b78 != 0) {
    FUN_004c7650(DAT_00636b78);
    DAT_00636b78 = 0;
  }
}
