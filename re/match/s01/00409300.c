// FLAGS: /O2 /GS /Ob1
/* a6 0x00409300 (48 B): 100% BYTES-OK. Plain cdecl; FUN_00426090 returns int* list walked to the n-th
   -1/-10 terminator. g2s was 80.0% (wrong loop shape only). */
int *FUN_00426090(void);
// MATCH: 0x00409300 FUN_00409300
int FUN_00409300(int n)
{
    int *p = FUN_00426090();
    for (; n > 0; n--) {
        while (*p != -1 && *p != -10)
            p++;
        p++;
    }
    return *p;
}
