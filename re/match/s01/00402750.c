// FLAGS: /O2 /GS /Ob1
/* a1 0x00402750 (722 B; the switch tables at 0x00402a24/0x00402a2c follow it). Best: 100.0% BYTES-OK.
   Start: g2s did not compile (Ghidra stack confusion). Rewritten from the disassembly: two RGBA locals for
   FUN_00558240, a 24-byte video-mode struct for FUN_004c2ed0, block-scoped RGBA + V2 for FUN_00484170,
   SetRenderState through the device vtable slot 0xe4 (__stdcall COM call), thunk addresses called directly
   (0x004b6540, 0x004b6560, 0x00418980, 0x00471eb0), FUN_00431b40 takes float 0.3f, varargs log FUN_004963e0.
   The switch cases that set flag=1 come first in source (block order follows source order). */
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float x, y; } V2;
typedef struct { int width, height, depth, flags, refRate, format; } VideoMode;
typedef struct IDevVtbl IDevVtbl;
typedef struct IDev { IDevVtbl *vt; } IDev;
struct IDevVtbl { void *pad[57]; long (__stdcall *SetRenderState)(IDev *d, int s, int v); };
extern int DAT_00773920;
extern IDev *DAT_00771a0c;
extern int DAT_007f09e4;
extern int DAT_007f0a30;
extern int DAT_008aa318;
extern int DAT_008aa314;
extern int DAT_00636ac8;
int FUN_00429290(void);
void FUN_004025f0(void);
void FUN_004026d0(int p0);
void FUN_00402f50(void);
int FUN_00403640(int p0);
void FUN_00404830(void);
int FUN_0040bb30(const char *p0);
int FUN_0040bb50(const char *p0);
void FUN_0040bbb0(void);
int FUN_004113b0(void);
void FUN_00412890(void);
void FUN_00418980(void);
void FUN_0041a1e0(void);
void FUN_0041b450(void);
void FUN_0041bec0(void);
void FUN_0041c100(void);
void FUN_0041cb10(void);
void FUN_0041d6e0(void);
void FUN_0041d8b0(void);
void FUN_0041db90(void);
void FUN_0041def0(void);
void FUN_0041eaa0(void);
void FUN_00420d00(void);
void FUN_00425bc0(void);
int FUN_004274d0(void);
int FUN_004274e0(void);
void FUN_004275d0(int p0);
void FUN_00427ca0(void);
void FUN_00428390(int p0);
void FUN_004283a0(void);
void FUN_00431b40(float p0);
int FUN_0045b350(void);
void FUN_0045bae0(void);
void FUN_004669b0(void);
int FUN_004671a0(int p0);
int FUN_00471eb0(void);
int FUN_004723d0(void);
void FUN_0047ba00(int p0, int p1);
int FUN_00484170(int cam, RGBA *c, V2 *v);
int FUN_004841d0(void);
void FUN_004881d0(int p0);
char *FUN_00494c80(int p0);
void FUN_00495280(const char *p0);
void FUN_004952f0(void);
void FUN_00495350(void);
void FUN_004963e0(const char *fmt, ...);
void FUN_00496e40(void);
int FUN_00498bc0(void);
int FUN_00498bd0(void);
int FUN_00499ce0(void);
int FUN_004b3d80(const char *p0, int p1, int p2);
void FUN_004b6540(int p0);
void FUN_004b6560(int (*fn)(void), int p1);
int FUN_004c2ed0(VideoMode *m, int idx);
short FUN_004c2f00(void);
int FUN_004caea0(void);
int FUN_004d8560(void);
int FUN_00558240(RGBA *a, RGBA *b);
// MATCH: 0x00402750 FUN_00402750
int FUN_00402750(void)
{
  RGBA c2;
  RGBA c1;
  VideoMode vm;
  int flag;
  c2.r = 0x40; c2.g = 0xff; c2.b = 0x40; c2.a = 0xff;
  c1.r = 8; c1.g = 8; c1.b = 8; c1.a = 0x80;
  flag = 0;
  if (FUN_004d8560() == 0) return 0;
  FUN_004963e0("Setting up PIZ file system\n");
  switch (DAT_00773920) {
  case 2: case 3: case 4: case 7:
    flag = 1;
    break;
  case 1: case 5: case 6:
    flag = 0;
    break;
  }
  FUN_004b6540(flag);
  FUN_004b6560(FUN_00429290, 0);
  FUN_004963e0("\tOK\n");
  FUN_004963e0("Calling PCVS_Init\n");
  FUN_00496e40();
  DAT_00771a0c = (IDev *)FUN_004caea0();
  DAT_00771a0c->vt->SetRenderState(DAT_00771a0c, 0x19, 5);
  DAT_00771a0c->vt->SetRenderState(DAT_00771a0c, 0x18, 0);
  FUN_004c2ed0(&vm, FUN_004c2f00());
  FUN_00498bc0();
  FUN_00498bd0();
  FUN_004963e0("Calling HardwareShowIntroVideo\n");
  FUN_00495350();
  FUN_00494c80(0);
  FUN_00495280("d:\toastart\common\font36.piz");
  FUN_004283a0();
  FUN_00427ca0();
  FUN_004275d0(FUN_004671a0(0));
  FUN_004274d0();
  FUN_004274e0();
  FUN_004952f0();
  FUN_004113b0();
  DAT_007f09e4 = FUN_00558240(&c2, &c1);
  if (FUN_004841d0() != 0) {
    RGBA w;
    V2 v;
    v.x = 12.0f; v.y = 15.0f;
    w.r = 0xff; w.g = 0xff; w.b = 0xff; w.a = 0xff;
    DAT_007f0a30 = FUN_00484170(FUN_004671a0(0), &w, &v);
  }
  FUN_004669b0();
  FUN_0040bbb0();
  FUN_004723d0();
  FUN_0045b350();
  DAT_008aa318 = FUN_0040bb50("SemiC");
  DAT_008aa314 = FUN_0040bb50("SemiC2");
  FUN_004881d0(FUN_0040bb30("lensflare-fs8"));
  FUN_004963e0("Calling BackBuffRasterOpen\n");
  FUN_00403640(FUN_004671a0(0));
  FUN_0047ba00(FUN_004671a0(0), 0);
  if (FUN_00499ce0() == 0) return 0;
  FUN_00495280("d:\toastart\common\powerups\powerups.piz");
  FUN_0045bae0();
  FUN_00418980();
  FUN_004952f0();
  FUN_00495280("d:\toastart\common\Panel\Panel.piz");
  FUN_0041b450();
  FUN_0041bec0();
  FUN_0041cb10();
  FUN_0041d6e0();
  FUN_0041def0();
  FUN_0041d8b0();
  FUN_0041c100();
  FUN_0041db90();
  FUN_0041a1e0();
  FUN_004952f0();
  FUN_0041eaa0();
  FUN_00420d00();
  FUN_00412890();
  FUN_00471eb0();
  FUN_00404830();
  FUN_00495280("d:\ToastArt\common\perm.piz");
  FUN_00425bc0();
  FUN_004025f0();
  FUN_004952f0();
  DAT_00636ac8 = FUN_004b3d80("toastart\pc\pc.txd", 0, 0);
  FUN_00431b40(0.3f);
  FUN_00402f50();
  FUN_00428390(3);
  FUN_004026d0(4);
  return 1;
}
