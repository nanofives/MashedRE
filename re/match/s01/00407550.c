/* 0x00407550 (35 B). Best 100.0%. static, key arg arrives in ESI (cl 9176 picked ESI for the single static arg). Pointer walk inside `if (0 < n)` puts the base load after the jle. CAVEAT: `caller` is a stand-in for the real caller 0x00407670 (no Ghidra size, not scoreable). */
// FLAGS: /O2 /GS /Ob1
struct Rec { char pad0[0x44]; int key; char pad48[0xec - 0x48]; };
extern struct Rec DAT_00639d80[];
extern int DAT_0063a5d0;
extern int DAT_0063a5d8;

// MATCH: 0x00407550 FUN_00407550
static struct Rec *FUN_00407550(int key)
{
    struct Rec *p;
    int i = 0;
    if (0 < DAT_0063a5d0) {
        p = DAT_00639d80;
        do {
            if (p->key == key)
                return p;
            i++;
            p++;
        } while (i < DAT_0063a5d0);
    }
    return 0;
}

struct Ev { char pad[0x10]; int type; int a; int key; };
int caller(struct Ev *e)
{
    struct Rec *r;
    if (e->type == 0x1c) {
        r = FUN_00407550(e->key);
        if (r != 0 && r == &DAT_00639d80[DAT_0063a5d8])
            return 1;
    }
    return 0;
}
