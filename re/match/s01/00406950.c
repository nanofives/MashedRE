// FLAGS: /O2 /GS /Ob1
/* 0x00406950 (115 B). Best 100.0%. Index form DAT_0063a220[i][j] over the E12 timer table, `if (!(timer > 0.0f))` gives the je form (the `<=` form gives jp). */
typedef struct { float timer; float reset; int h; } E12;
extern E12 DAT_0063a220[][10];
extern int DAT_0063a478[];
extern float DAT_007f100c;
extern const float DAT_005d757c;
int FUN_00458f20(int h, int p);
int FUN_0045c330(int p);
// MATCH: 0x00406950 FUN_00406950
void FUN_00406950(int i)
{
    int j;
    for (j = 0; j < DAT_0063a478[i]; j++) {
        if (FUN_0045c330(0) == 1) {
            DAT_0063a220[i][j].timer = DAT_0063a220[i][j].reset;
        } else {
            DAT_0063a220[i][j].timer -= DAT_007f100c;
            if (!(DAT_0063a220[i][j].timer > DAT_005d757c)) {
                DAT_0063a220[i][j].timer = DAT_0063a220[i][j].reset;
                FUN_00458f20(DAT_0063a220[i][j].h, 1);
            }
        }
    }
}
