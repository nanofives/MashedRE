/* a7 0x004260e0 (525 B): best 100.0% exact (regs 100, ops 100).
   Tried: g2s raw 10.3%; strcpy/strcat intrinsics with literal "c:" 62.1% (a literal source makes strcpy a
   fixed word+byte move, the original runs a byte loop: the source is the const array at 0x005cd4ac, not a
   literal) -> 75.5%; if(!dos)/else and sep initialised to "/" 98.1%; goto-to-tail block 98.1% (the compiler
   re-lays a goto'd tail block inline, so source order of blocks does not set layout); const char*, uchar
   param, statement moves 74-87%. Final: `sep = dos ? "\\" : "/";` then `if (dos) { strcpy; strcat(buf, sep); }`
   -> 100%: the ternary's "/" arm is what the compiler puts after the ret, and constant propagation turns
   strcat(buf, sep) inside if(dos) into the fixed 2-byte move of "\\" @0x005cc4dc.
   strcat(buf, "") is the byte store from 0x005cc4de. Strings: "c:" @0x005cd4ac, "ToastArt" @0x005cd4a0,
   "tracks" @0x005cd498, "/" @0x005cc4d8, "\\" @0x005cc4dc. Buffer @0x008991e0.
   Residual: none. */
// FLAGS: /O2 /GS /Ob1
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
#pragma intrinsic(strcpy, strcat)
extern char DAT_008991e0[];
extern const char DAT_005cd4ac[];
// MATCH: 0x004260e0 _FUN_004260e0
char *FUN_004260e0(int dos, char *a, char *b, char *c)
{
    char *sep;
    DAT_008991e0[0] = 0;
    sep = dos ? "\\" : "/";
    if (dos) {
        strcpy(DAT_008991e0, DAT_005cd4ac);
        strcat(DAT_008991e0, sep);
    }
    strcat(DAT_008991e0, "ToastArt");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, "");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, "tracks");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, a);
    if (*b)
        strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, b);
    if (*c)
        strcat(DAT_008991e0, c);
    return DAT_008991e0;
}
