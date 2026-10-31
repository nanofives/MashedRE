/* a7 0x0041c380 (130 B) + caller 0x0041cb80 (53 B): both 100.0% exact (regs 100, ops 100), cl 13.00.9176, no /GL.
   First pass (extern, non-static, stack param): 60.6%, read as (c) LTCG because the object arrives in ESI
   with no prolog. That was wrong. It was source shape (TU layout), the same as the main session's 0x00422470 finding:
   - FUN_0041c380 `static`, defined before its only caller FUN_0041cb80 (0x0041cba0 is the only call site):
     cl 9176 gives it a custom convention, `o` in ESI, the caller's loop pointer. -> 86.7%.
   - DAT_0063cde4 `static` (file-local): the compiler then knows stores through `o` cannot alias it, and
     hoists both of its loads above the o->mask stores (`cmp [0x63cde4],3` and `mov edx,[0x63cde4]`). -> 100%.
     Only 3 references to 0x0063cde4 exist in .text (0x0041c3a6, 0x0041c3e5, 0x0041cb87), all in these two
     functions, which is consistent with a file-static.
     Making DAT_0063ce20 static, or defining (non-static) any of the globals, does not move it: 86.7%.
   - /Ob1 not required here: /O2 /GS alone also gives 100/100 (the callee is not inlined).
   - Statement order of the first stores and 4 spellings of the final compare: no effect once the above holds
     (the `else o->flag = 0` and `& 0x1f` forms are worse).
   Obj stride 0x114 at 0x0063ce20 (4 entries, end 0x0063d270): +0x108 idx, +0x10c mask, +0x110 flag.
   Slot stride 0x10 at 0x007f1a18, field +4 = 0x007f1a1c. Residual: none. */
// FLAGS: /O2 /GS /Ob1
typedef struct { int a; int b; int pad[2]; } Slot;
typedef struct { char pad[0x108]; int idx; unsigned int mask; int flag; } Obj;   /* size 0x114 */
extern Slot DAT_007f1a18[];
static int DAT_0063cde4;
extern int DAT_0063ce18;
extern int DAT_007f0fd0;
extern Obj DAT_0063ce20[4];
int FUN_0042f500(void);
int FUN_0040e340(void);
int FUN_0040b890(void);

// MATCH: 0x0041c380 _FUN_0041c380
static void FUN_0041c380(Obj *o)
{
    o->mask = 0;
    o->flag = 0;
    o->mask = 1 << DAT_007f1a18[o->idx].b;
    if (DAT_0063cde4 > 3) {
        if (!FUN_0042f500() && DAT_007f0fd0 != 1 && DAT_007f0fd0 != 2)
            o->mask |= 0x800;
        else
            o->mask |= 0x400;
    } else
        o->mask |= 0x400;
    if (o->idx < DAT_0063cde4)
        o->flag = 1;
}

// MATCH: 0x0041cb80 _FUN_0041cb80
void FUN_0041cb80(void)
{
    int i;
    DAT_0063cde4 = FUN_0040e340();
    DAT_0063ce18 = FUN_0040b890();
    for (i = 0; i < 4; i++)
        FUN_0041c380(&DAT_0063ce20[i]);
}
