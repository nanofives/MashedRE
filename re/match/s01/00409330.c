// FLAGS: /O2 /GS /Ob1
/* a6 0x00409330 (120 B): 100% BYTES-OK. Same list walk as 0x00409300, then a range/0x100 lookup.
   The one residual at g2s (cmp operand order) needed `v >= *p` instead of `*p <= v`. */
int *FUN_00426090(void);
// MATCH: 0x00409330 FUN_00409330
int FUN_00409330(int v, int n)
{
    int *p = FUN_00426090();
    int *q;
    for (; n > 0; n--) {
        while (*p != -1 && *p != -10)
            p++;
        p++;
    }
    q = p;
    while (*p != -1 && *p != -10) {
        if (p[1] == 0x100) {
            if (v == *p)
                return p[2];
        } else if (v >= *p && v < p[1]) {
            return v + 1;
        }
        p++;
    }
    return *q;
}
