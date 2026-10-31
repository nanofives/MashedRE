/* 0x004074a0 (167 B). Best 100.0%. Smoke array 0x0063a490 (10 x 0x20) indexed by int (signed jl), not walked by pointer (jb). */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float m[16]; } Mat;
struct Frame { char pad[0x40]; V3 pos; };
struct Clump { int type; struct Frame *frame; };
struct Atomic { char pad[0x48]; void (*render)(struct Atomic *); };
struct Rec {
    char name[0x10];            /* 0x00 */
    struct Clump *clump;        /* 0x10 */
    int f14;                    /* 0x14 */
    int f18;                    /* 0x18 */
    struct Atomic *atoms[10];   /* 0x1c */
    int key;                    /* 0x44 */
    int f48;                    /* 0x48 */
    int f4c;                    /* 0x4c */
    int f50, f54, f58;          /* 0x50 */
    float f5c;                  /* 0x5c */
    float f60;                  /* 0x60 */
    int f64, f68, f6c;          /* 0x64 */
    float f70, f74, f78;        /* 0x70 */
    V3 p7c;                     /* 0x7c */
    V3 p88;                     /* 0x88 */
    float f94, f98, f9c, fa0;   /* 0x94 */
    int fa4, fa8, fac, fb0;     /* 0xa4 */
    int idx;                    /* 0xb4 */
    int fb8, fbc, fc0, fc4;     /* 0xb8 */
    int fc8;                    /* 0xc8 */
    int fcc;                    /* 0xcc */
    int fd0, fd4, fd8;          /* 0xd0 */
    float fdc, fe0, fe4;        /* 0xdc */
    int fe8;                    /* 0xe8 */
};
struct Smoke { int obj; int f4, f8, fc; int fx; int f14, f18, f1c; };
extern struct Rec DAT_00639d80[];
extern int DAT_0063a5d0;
extern int DAT_0063a5d4;
extern int DAT_0063a5d8;
extern int DAT_0063a478[];
extern struct Smoke DAT_0063a490[10];
void FUN_004e6e00(int o);
void FUN_0047f290(int h);
void FUN_00489c60(int h);

// MATCH: 0x004074a0 FUN_004074a0
void FUN_004074a0(void)
{
    int i;
    for (i = 0; i < DAT_0063a5d0; i++) {
        FUN_004e6e00((int)DAT_00639d80[i].clump);
        if (DAT_00639d80[i].key > -1)
            FUN_0047f290(DAT_00639d80[i].key);
        DAT_0063a478[i] = 0;
        DAT_00639d80[i].clump = 0;
        DAT_00639d80[i].f14 = 0;
        DAT_00639d80[i].fcc = 0;
        DAT_00639d80[i].key = -1;
    }
    DAT_0063a5d0 = 0;
    DAT_0063a5d4 = 0;
    DAT_0063a5d8 = 0;
    for (i = 0; i < 10; i++) {
        if (DAT_0063a490[i].obj != 0)
            FUN_004e6e00(DAT_0063a490[i].obj);
        DAT_0063a490[i].obj = 0;
        if (DAT_0063a490[i].fx != 0)
            FUN_00489c60(DAT_0063a490[i].fx);
        DAT_0063a490[i].fx = 0;
        DAT_0063a490[i].fc = 0;
    }
}
