/* 0x00407800 (498 B). Best 99.3%, residual (b): one store/load swap, `mov [esi+0xb0],0` (rec+0xc8) is scheduled before the load of rec->idx in the original and after it in ours. Tried: int/float field, temp for the table value, store moved, store through DAT_00639d80[i], int* table base. Permuter 285 compiles no gain. RGBA col must be block-scoped in the smoke loop to share the counter slot (frame 0xb0). */
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
    float fc8;                  /* 0xc8 */
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
struct Big { char pad[0x105b0]; int tex[1]; };
extern struct Smoke DAT_0063a490[10];
extern char DAT_005cc980[];
int FUN_004a2b60(char *buf, const char *fmt, ...);
void FUN_004c5c80(int a);
struct Clump *FUN_004781b0(char *path, int a);
int FUN_004b3f90(struct Clump *c);
int FUN_0047fad0(int a, char *b, int c, float d, float e, float f, int g);
void FUN_0047cdc0(int h, float s);
int FUN_0047d130(int h);
void FUN_004b47e0(Mat *m, int a, int b);
void FUN_0047d180(int h, Mat *m);
void FUN_0047cf20(int h, int a);
int FUN_004b3fc0(struct Clump *c, struct Atomic **out);
void FUN_004b5190(struct Atomic *a, int b, int c);
void FUN_00425bf0(void);
void FUN_004952f0(void);
int FUN_0040bb30(const char *name);
int FUN_00454f80(void);
int FUN_00489940(int a, int tex, int c, int d, RGBA *col, float e, float f);
void FUN_004073b0(void);
void FUN_00406370(void);

// MATCH: 0x00407800 FUN_00407800
void FUN_00407800(struct Big *big)
{
    int i, n;
    struct Atomic *tmp[10];
    char path[64];
    Mat m;
    i = 0;
    FUN_00425bf0();
    for (; i < DAT_0063a5d0; i++) {
        struct Rec *r = &DAT_00639d80[i];
        FUN_004a2b60(path, "%s%s%s.dff", "ToastArt/common/perm/", "Copters/", r->name);
        FUN_004c5c80(0);
        r->clump = FUN_004781b0(path, 0);
        r->f14 = FUN_004b3f90(r->clump);
        r->key = FUN_0047fad0(r->f14, DAT_005cc980, 0, 0.5f, 0.5f, 20.0f, 200000);
        FUN_0047cdc0(r->key, 100.0f);
        r->f18 = FUN_0047d130(r->key);
        r->fc8 = 0.0f;
        r->fcc = big->tex[r->idx];
        FUN_004b47e0(&m, r->fcc, 0);
        FUN_0047d180(r->key, &m);
        FUN_0047cf20(r->key, 1);
        n = FUN_004b3fc0(r->clump, tmp);
        while (n != 0) {
            n--;
            FUN_004b5190(tmp[n], 0, 0);
            r->atoms[n] = tmp[n];
        }
    }
    FUN_004952f0();
    for (i = 0; i < 10; i++) {
        RGBA col;
        int tex = FUN_0040bb30("smoke");
        col.r = 0xa0;
        col.g = 0xa0;
        col.b = 0xa0;
        col.a = 0x40;
        if (DAT_0063a490[i].obj == 0)
            DAT_0063a490[i].obj = FUN_00454f80();
        DAT_0063a490[i].fx = FUN_00489940(8, tex, 0x817, 2, &col, 0.3f, 1.5f);
    }
    FUN_004073b0();
    FUN_00406370();
}
