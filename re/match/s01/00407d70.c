/* 0x00407d70 (60 B). Best 100.0%. Fix: index form t[i+1] over the FUN_00426090 table (pointer form 72.7%, permuter could not move it). */
// FLAGS: /O2 /GS /Ob1
int *FUN_00426090(void);

// MATCH: 0x00407d70 FUN_00407d70
int FUN_00407d70(int i)
{
    int *t = FUN_00426090();
    int n = 0;
    while (t[i + 1] != -10 && t[i + 1] != -1) {
        if (t[i + 1] == 0x100) {
            n++;
            i += 2;
        } else {
            n += t[i + 1] - t[i];
            i++;
        }
    }
    return n + 1;
}
