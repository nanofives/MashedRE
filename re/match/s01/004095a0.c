// FLAGS: /O2 /GS /Ob1
/* a6 0x004095a0 (211 B Ghidra): BYTES-OK, 98.6% only because Ghidra's boundary stops before the final
   `jmp __security_check_cookie` at 0x00409671 (class c, tool limit). char path[32] = "..." and
   char name[32] = "" initialisers, strcpy/strcat intrinsics. FUN_004a2b60 = sprintf (cdecl). */
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
#pragma intrinsic(strcpy, strcat)
extern char DAT_0063b7f0[];
int FUN_00426c00(void);
int FUN_004a2b60(char *buf, const char *fmt, ...);
// MATCH: 0x004095a0 FUN_004095a0
char *FUN_004095a0(int mode)
{
    char path[32] = "toastart/common/led/";
    char name[32] = "";
    FUN_004a2b60(name, "LE%d.LED", FUN_00426c00());
    if (mode == 1) {
        strcpy(DAT_0063b7f0, name);
    } else {
        strcpy(DAT_0063b7f0, path);
        strcat(DAT_0063b7f0, name);
    }
    return DAT_0063b7f0;
}
