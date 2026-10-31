// FLAGS: /O2 /GS /Ob1
typedef struct { int team; int pad[3]; } Slot;      /* stride 0x10 at 0x007f1a18 */
typedef struct { int score; int pad[0x4d]; } Rec;   /* stride 0x138 at 0x00899f7c */
extern Slot DAT_007f1a18[4];
extern Rec DAT_00899f7c[4];
int FUN_0042f500(void);

// MATCH: 0x00423e60 _v_loop
int v_loop(int i)
{
    int j, r;
    if (FUN_0042f500()) {
        r = DAT_00899f7c[i].score;
        for (j = 0; j < 4; j++)
            if (DAT_007f1a18[j].team == DAT_007f1a18[i].team && j != i)
                r += DAT_00899f7c[j].score;
        return r;
    }
    return DAT_00899f7c[i].score;
}

// MATCH: 0x00423e60 _v_loop2
int v_loop2(int i)
{
    int j, r;
    if (FUN_0042f500()) {
        int t = DAT_007f1a18[i].team;
        r = DAT_00899f7c[i].score;
        for (j = 0; j < 4; j++)
            if (DAT_007f1a18[j].team == t && j != i)
                r += DAT_00899f7c[j].score;
    } else
        r = DAT_00899f7c[i].score;
    return r;
}
