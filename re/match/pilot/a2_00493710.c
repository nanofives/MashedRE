// a2 0x00493710 (430 B): best 100.0% (regs 100, ops 100). g2s start 68.3%.
// Fixes: (1) 0x005c9d00 is `xor eax,eax; ret`, shared by three source calls (identical-code folding), so
//   Ghidra's arity is wrong: the 0x200000 push belongs to 0x004c32b0 (3 args, add esp,0xc at 0x00493748),
//   i.e. FUN_004c32b0(mem, StubC(), 0x200000). This alone fixed the early `add esp,0xc` flush.
//   (2) failure tail via goto so the fail block sits between the RwEngineStart test and the success path.
//   (3) RwVideoMode-shaped local at frame +0x14 (0x18 B) explains the 0x2c frame. 0x004a2cbd = printf.
// FLAGS: /O2 /GS /Ob1
typedef struct { int width, height, depth, flags, refRate, format; } VideoMode;   /* frame +0x14, 0x18 B */
typedef struct { int a[4]; } Buf16;                                               /* frame +0x04 */
extern int DAT_007719d8;
extern int DAT_007719dc;
void FUN_004936f0(void);
void FUN_00493700(void);
int printf(const char *fmt, ...);                 /* 0x004a2cbd */
void FUN_0045b350(void);
int FUN_00493600(void);
int FUN_00493640(void);
int FUN_004951f0(void);
int FUN_004951e0(void);                           /* jmp 0x00495780 */
void FUN_00495270(void *p);
void FUN_004963e0(const char *s);
int FUN_004c2ed0(VideoMode *m, int i);
short FUN_004c2f00(void);
int FUN_004c2fb0(void);
int FUN_004c3040(void);
int FUN_004c30b0(void *p);
int FUN_004c3270(void);
int FUN_004c32b0(int memFuncs, int initFlags, int resArenaSize);   /* 3 args: add esp,0xc at 0x00493748 */
void FUN_004c5c80(int a);
void FUN_004c9eb0(unsigned int a);
void FUN_004c9f50(int a);
void FUN_004c9f60(int a);
void FUN_004cbc60(void *f);
int FUN_004cbc70(void);
void FUN_004cbc80(void *f);
int FUN_004cbc90(void);
int FUN_00550350(int a);
void FUN_00550390(void);
int FUN_005584c0(void);
/* 0x005c9d00 is `xor eax,eax; ret` shared by three call sites (identical-code folding) */
int StubA(void *p);
int StubB(void *p);
int StubC(void);

// MATCH: 0x00493710 _FUN_00493710
int FUN_00493710(void)
{
    int open;
    Buf16 buf;
    VideoMode mode;
    int mem;
    int mi;

    StubA(&buf);
    mem = StubB(&buf);
    FUN_004963e0("Calling RwEngineInit\n");
    if (!FUN_004c32b0(mem, StubC(), 0x200000))
        return 0;
    FUN_004963e0("Calling RtFSManagerOpen\n");
    if (!FUN_00550350(-1))
        return 0;
    FUN_004963e0("Calling HardwareInstallFileSystem\n");
    FUN_004951e0();
    FUN_00495270(&open);
    FUN_004963e0("Calling RenderwareAttachPlugins\n");
    FUN_00493640();
    FUN_004963e0("Calling RwEngineOpen\n");
    if (!FUN_004c30b0(&open)) {
        FUN_00550390();
        FUN_004c3270();
        return 0;
    }
    FUN_004963e0("Calling HardwareSetVideoMode\n");
    if (!FUN_004951f0())
        goto fail;
    FUN_004c9f50(1);
    FUN_004c9eb0(0x3c);
    FUN_004c9f60(1);
    FUN_004963e0("Calling RwEngineStart\n");
    if (!FUN_004c2fb0()) {
fail:
        FUN_00550390();
        FUN_004c3040();
        FUN_004c3270();
        return 0;
    }
    FUN_0045b350();
    FUN_0045b350();
    mi = FUN_004c2f00();
    FUN_004c2ed0(&mode, mi);
    printf("\tUsing video mode %u : %ux%u\n", mi, mode.width, mode.height);
    printf("At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n",
           0x400000, mode.width * mode.height * 12, 0x400000 - mode.width * mode.height * 10);
    FUN_00493600();
    FUN_005584c0();
    FUN_004c5c80(0);
    DAT_007719d8 = FUN_004cbc90();
    FUN_004cbc80(FUN_004936f0);
    DAT_007719dc = FUN_004cbc70();
    FUN_004cbc60(FUN_00493700);
    return 1;
}
