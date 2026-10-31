/* 0x00407cd0 (157 B). Best 100.0%. Index form, while (t[i] != -10); first try. */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);

// MATCH: 0x00407cd0 FUN_00407cd0
int FUN_00407cd0(int v, int start)
{
    int *t = FUN_00426090();
    int i = start;
    while (t[i] != -10) {
        if (t[i] == -1)
            return 0;
        if (t[i + 1] == 0x100) {
            if (v == t[i])
                return t[i + 2];
            if (v == t[i + 2])
                return v + 1;
            i += 2;
        } else {
            if (v >= t[i] && v < t[i + 1])
                return v + 1;
            if (v == t[i + 1]) {
                if (t[i + 2] == -10)
                    return v;
                if (t[i + 2] == -1)
                    return t[start];
                return t[i + 3];
            }
            i++;
        }
    }
    return v;
}
