/* 0x00407db0 (111 B). Best 100.0%. for (; t[i+1] != -10; ) { if (t[i+1] == -1) break; ...} keeps all pushes in the prologue (the && form shrink-wrapped ebp/esi, 85.1%). */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);
// MATCH: 0x00407db0 FUN_00407db0
int FUN_00407db0(int i, int v)
{
    int *t = FUN_00426090();
    int first = t[1];
    int res = v;
    int n = 0;
    for (; t[i + 1] != -10; ) {
        if (t[i + 1] == -1) break;
        if (t[i + 1] == 0x100) {
            n++;
            i += 2;
        } else {
            if (v >= t[i] && v <= t[i + 1] && t[i] > first)
                res = res + n - t[i];
            n += t[i + 1] - t[i];
            i++;
        }
    }
    return res;
}
