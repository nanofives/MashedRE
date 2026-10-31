// FLAGS: /O2 /GS /Ob1
/* a2 0x00402b70 (960 B). Best: 99.4%.
   Path normaliser (dirs split on \ or /, 8.3 checks, PS2/XBOX/PLATFORM -> PC, upper-case, join).
   g2s did not compile. Written from the disassembly with intrinsic strcpy/strcat/strlen/strcmp
   (strcmp against a literal becomes repe cmpsb), `char path[50] = ""` and `char tmp[128] = ""`
   (byte + rep stosd init), strchr 0x004a2fd0, _strupr 0x005c192c, sprintf 0x004a2b60, error printf
   0x004987b0. No `if (ndirs > 1)` around the PC loop (it doubles the test, 95.7), join loop with a
   pointer stepping by 9 (index form 91.6).
   Residual class (b): one `jle` of the join loop's guard sits after `lea ebp,[esp+0x60]` in the original,
   before it in ours (then the epilogue's `jmp` slot shifts). Tried: for-comma init, if-guard (91.6),
   count-down, while(i--), fresh pointer var, row pointer, permuter (no gain). */
typedef unsigned int size_t;
char *strcpy(char *d, const char *s);
char *strcat(char *d, const char *s);
size_t strlen(const char *s);
int strcmp(const char *a, const char *b);
#pragma intrinsic(strcpy, strcat, strlen, strcmp)
char *strchr(const char *s, int c);   /* 0x004a2fd0 */
char *_strupr(char *s);               /* 0x005c192c */
int sprintf(char *d, const char *fmt, ...); /* 0x004a2b60 */
void FUN_004987b0(const char *fmt, ...);

// MATCH: 0x00402b70 FUN_00402b70
char *FUN_00402b70(char *out, char *in)
{
  int ndirs;
  const char *sep;
  char ext[8];
  char name[12];
  char path[50] = "";
  char dirs[6][9];
  int i;
  char *p;

  ndirs = 0;
  strcpy(path, in);
  for (i = 0; i < 6; i++) {
    if ((p = strchr(path, '\\')) != 0 || (p = strchr(path, '/')) != 0) {
      *p = 0;
      sprintf(dirs[i], "%s", path);
      strcpy(path, p + 1);
      ndirs++;
      if (strlen(dirs[i]) > 8) {
        FUN_004987b0("Directory names must have no more than %d letters\n", 8);
        FUN_004987b0("[%s] has %d\n", dirs[i], strlen(dirs[i]));
        return 0;
      }
    }
  }
  p = strchr(path, '.');
  *p = 0;
  sprintf(name, "%s", path);
  if (strlen(name) > 8) {
    FUN_004987b0("File names must have no more than %d letters\n", 8);
    return 0;
  }
  sprintf(ext, "%s", p + 1);
  if (strchr(in, ':') != 0) {
    sprintf(dirs[0], "%s", "");
    sep = "\\";
  } else {
    sep = "/";
  }
  for (i = 1; i < ndirs; i++) {
    _strupr(dirs[i]);
    if (strcmp(dirs[i], "PS2") == 0 || strcmp(dirs[i], "XBOX") == 0 || strcmp(dirs[i], "PLATFORM") == 0)
      sprintf(dirs[i], "%s", "PC");
  }
  sprintf(out, "");
  p = dirs[0];
  for (i = 0; i < ndirs; i++) {
    strcat(out, p);
    strcat(out, sep);
    p += 9;
  }
  _strupr(name);
  _strupr(ext);
  strcat(out, name);
  strcat(out, ".");
  strcat(out, ext);
  if (out[0] == '\\') {
    char tmp[128] = "";
    strcpy(tmp, out + 1);
    strcpy(out, tmp);
  }
  if (strlen(out) > 0x31) {
    FUN_004987b0("FileName is too long (%dchars max)\n", 0x31);
    return 0;
  }
  return out;
}
