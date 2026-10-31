/* a7 0x0042b310 (548 B): best 100.0% exact (regs 100, ops 100).
   Tried: g2s raw 57.1%; natural C with rolled loops (the 8-pad loops are fully unrolled by the compiler,
   the 4-slot loops strength-reduced to a pointer compare against 0x007f1a54) 95.6% with if/else-if on
   kind 0x13 / 0x1e; switch(kind) + nested switch(sub) 87.9%; switch + single `s==0||s==1||s==2` 98.9%;
   switch(kind) with case 0x13 testing sub as (0||1) then (2) as two ifs -> 100% (the original duplicates
   the `return 1` tail for the second if).
   Residual: none. Pad stride 0x4c at 0x007f1044 (cur) and 0x007f1504 (= +0x4c0, second array), slot stride 0x10
   at 0x007f1a14 (4 entries, end 0x007f1a54), mode stride 0x40 at 0x0067ed3c (+0 kind, +4 sub @0x0067ed40). */
// FLAGS: /O2 /GS /Ob1
typedef struct { char down; char pad[0x4b]; } Pad;          /* stride 0x4c */
typedef struct { int id; int pad[3]; } Slot;                 /* stride 0x10 at 0x007f1a14 */
typedef struct { int kind; int sub; int pad[14]; } Mode;     /* stride 0x40 at 0x0067ed3c */
extern Pad DAT_007f1044[8];
extern Pad DAT_007f1504[8];
extern Slot DAT_007f1a14[4];
extern int DAT_007f1a0c;
extern int DAT_0067e9f8;
extern Mode DAT_0067ed3c[];
extern int DAT_0067f1b8;
int FUN_0040e470(int i);

// MATCH: 0x0042b310 _FUN_0042b310
int FUN_0042b310(void)
{
    int r = 0, found = 0, i;
    if (DAT_007f1a0c == 0x1000) {
        for (i = 0; i < 8; i++)
            if (DAT_007f1044[i].down)
                found = 1;
    } else {
        for (i = 0; i < 4; i++)
            if (DAT_007f1a14[i].id != -1 && FUN_0040e470(i) != 2 && DAT_007f1044[DAT_007f1a14[i].id].down)
                found = 1;
    }
    if (found) {
        switch (DAT_0067ed3c[DAT_0067e9f8].kind) {
        case 0x13:
            if (DAT_0067ed3c[DAT_0067e9f8].sub == 0 || DAT_0067ed3c[DAT_0067e9f8].sub == 1)
                return 1;
            if (DAT_0067ed3c[DAT_0067e9f8].sub == 2)
                return 1;
            break;
        case 0x1e:
            return 1;
        }
    } else
        DAT_0067f1b8 = 0;
    if (DAT_007f1a0c == 0x1000) {
        for (i = 0; i < 8; i++)
            if (DAT_007f1044[i].down && !DAT_007f1504[i].down)
                r = 1;
    } else {
        for (i = 0; i < 4; i++)
            if (DAT_007f1a14[i].id != -1 && FUN_0040e470(i) != 2 && DAT_007f1044[DAT_007f1a14[i].id].down
                && !DAT_007f1504[DAT_007f1a14[i].id].down)
                r = 1;
    }
    return r;
}
