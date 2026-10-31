// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041ad60 FUN_0041ad60
/* h1 MATCH-S03: 17-slot twin of the pilot's 0x00419090. Takes EAX = obj, EBX = rec (Ghidra in_EAX /
   unaff_EBX): a static callee defined before its caller, parameter order (rec, obj). caller_0041adxx is
   a stand-in caller (not matched) that keeps rec in EBX across the call. */
typedef struct Rec41ad60 {
    int  slot[0x14];   /* +0x00, 0x50 bytes cleared by FUN_004b6520 */
    char pad50[0x5c - 0x50];
    int  obj;          /* +0x5c */
    int  obj4;         /* +0x60 */
    char pad64[0x74 - 0x64];
} Rec41ad60;
#ifndef MATCH_PORT
extern Rec41ad60 DAT_0063c8d0[4];
void FUN_004b3fc0(int obj, int *out);
int  FUN_004b5190(int a, int b, int c);
void FUN_004b6520(void *p, int n);
int  FUN_0042a5d0(const char *s, int a, int b);
#endif

static void FUN_0041ad60(Rec41ad60 *rec, int obj)
{
    int list[17];
    int i;
    FUN_004b3fc0(obj, list);
    rec->obj = obj;
    rec->obj4 = *(int *)(obj + 4);
    FUN_004b6520(rec->slot, 0x50);
    i = 0;
    do {
        int a = list[i];
        rec->slot[FUN_004b5190(a, 0, 0)] = a;
        i++;
    } while (i < 17);
}

#ifndef MATCH_PORT
/* stand-in caller, not matched, not shipped */
void caller_0041adxx(void)
{
    Rec41ad60 *r;
    for (r = DAT_0063c8d0; r < DAT_0063c8d0 + 4; r++)
        FUN_0041ad60(r, FUN_0042a5d0("x", 0, 0));
}
#endif
