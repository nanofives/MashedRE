/* 0x00407b00 (99 B). Best 100.0%. for(;;) over the table with a group index; first try. */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);

// MATCH: 0x00407b00 FUN_00407b00
int FUN_00407b00(int v)
{
    int *t = FUN_00426090();
    int i = 0;
    int grp = 0;
    for (;;) {
        if (t[i] == -1 || t[i] == -10) {
            i++;
            if (t[i] == -2)
                return grp;
            grp = i;
        } else if (t[i + 1] == 0x100) {
            if (v == t[i] || v == t[i + 2])
                return grp;
            i += 2;
        } else {
            if (v >= t[i] && v <= t[i + 1])
                return grp;
            i++;
        }
    }
}
