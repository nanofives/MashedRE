/* 0x00407b70 (98 B). Best 100.0%. Index form t[i], t[i+2] (the compiler strength-reduces t[i+2] into its own pointer); range test written v >= t[i] && v <= t[i+1]. */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);

// MATCH: 0x00407b70 FUN_00407b70
int FUN_00407b70(int v, int i)
{
    int *t = FUN_00426090();
    while (t[i] != -1) {
        if (t[i] == -10)
            return 0;
        if (t[i + 1] == 0x100) {
            if (v == t[i] || v == t[i + 2])
                return 1;
            i += 2;
        } else {
            if (v >= t[i] && v <= t[i + 1])
                return 1;
            i++;
        }
    }
    return 0;
}
