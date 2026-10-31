// FLAGS: /O2 /GS /Ob1
/* 0x00419090 (96 B). Best 100.0% exact (regs 100, ops 100), plain cl 13.00.9176, no /GL.
   The original takes EAX = obj, EBX = rec, and idx on the stack. Its caller at 0x0041a0d8 sets
   EBX = 0x0063bf30 + i*0x54 and EAX = the return of FUN_0042a5d0. That is the register convention
   plain cl gives a `static` function defined before its caller in the same .c file. /Ob1 stops it being
   inlined. Parameter order (rec, obj, idx). caller_0041a0xx is a stand-in for the real caller (not
   matched). Before: 78.6% as an extern function with stack args. In the first pass I put this down to
   /GL. That was wrong: Rich has no LTCG prodid 43/44, and the static-function convention explains it.
   Residual: none. */
typedef struct Rec {
    char pad[0x20];
    int  slot[8];
    int  obj;
    int  obj4;
    int  idx;
    char pad2[0x54 - 0x4c];
} Rec;
extern Rec DAT_0063bf30[8];
void FUN_004b3fc0(int obj, int *out);
int  FUN_004b5190(int a, int b, int c);
void FUN_004b52c0(int a, unsigned int b, int c);
void FUN_004b6520(void *p, int n);
int  FUN_0042a5d0(const char *s, int a, int b);

// MATCH: 0x00419090 _FUN_00419090
static void FUN_00419090(Rec *rec, int obj, int idx)
{
    int list[6];
    int i;
    FUN_004b3fc0(obj, list);
    rec->idx = idx;
    rec->obj = obj;
    rec->obj4 = *(int *)(obj + 4);
    FUN_004b6520(rec->slot, 0x20);
    i = 0;
    do {
        int a = list[i];
        rec->slot[FUN_004b5190(a, 0, 0)] = a;
        FUN_004b52c0(*(int *)(a + 0x18), 0x40, 1);
        i++;
    } while (i < 6);
}

void caller_0041a0xx(void)
{
    int i;
    Rec *r;
    for (i = 0, r = DAT_0063bf30; r < DAT_0063bf30 + 2; r++, i++)
        FUN_00419090(r, FUN_0042a5d0("x", 0, 0), i);
}
