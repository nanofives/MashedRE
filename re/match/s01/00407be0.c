/* 0x00407be0 (225 B). Best 67.6%, residual (b): instruction stream matches up to block layout. The original places the shared -10 return after the first scan and the v-1 return before the t[i] return. Tried if/else polarity of both i==start tests; permuter 204 compiles no gain. */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);

// MATCH: 0x00407be0 FUN_00407be0
int FUN_00407be0(int v, int start)
{
    int *t = FUN_00426090();
    int i = start;
    while (t[i] != -10) {
        if (t[i] == -1)
            return 0;
        if (t[i + 1] == 0x100) {
            if (v == t[i]) {
                if (i == start) {
                    while (t[i] != -1) {
                        if (t[i] == -10)
                            return -10;
                        i++;
                    }
                    return t[i - 1];
                }
                return v - 1;
            }
            if (v == t[i + 2])
                return t[i];
            i += 2;
        } else {
            if (v > t[i] && v <= t[i + 1])
                return v - 1;
            if (v == t[i]) {
                if (i == start) {
                    while (t[i] != -1) {
                        if (t[i] == -10)
                            return -10;
                        i++;
                    }
                    return t[i - 1];
                }
                return t[i - 2];
            }
            i++;
        }
    }
    return v;
}
