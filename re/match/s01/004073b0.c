/* 0x004073b0 (237 B). Best 100.0%. First try with the record struct; float fields zeroed through the zero register. */
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
void FUN_00406370(void);
void FUN_004b47e0(Mat *m, int a, int b);
void FUN_004c1480(struct Frame *f, Mat *m, int b);
void FUN_0047d180(int h, Mat *m);

// MATCH: 0x004073b0 FUN_004073b0
void FUN_004073b0(void)
{
    int i;
    Mat m;
    struct Rec *r;
    i = 0;
    FUN_00406370();
    if (0 < DAT_0063a5d0) {
        r = DAT_00639d80;
        do {
            r->f48 = 0;
            r->fd0 = 0;
            r->f5c = 100.0f;
            r->fe4 = 0;
            r->fe0 = 0;
            r->fdc = 0;
            r->fd4 = 0;
            r->f64 = 0;
            r->f4c = 0;
            r->fa8 = 0;
            r->fa4 = 0;
            r->f6c = 0;
            r->f68 = 0;
            r->f70 = 0.6f;
            r->f74 = 1.4f;
            r->f78 = 0.05f;
            r->f94 = 0.09f;
            r->f98 = 10.0f;
            r->f9c = 0.1f;
            r->fa0 = 0.2f;
            if (r->fcc != 0) {
                FUN_004b47e0(&m, r->fcc, 0);
                FUN_004c1480(r->clump->frame, &m, 0);
                FUN_0047d180(r->key, &m);
            }
            i++;
            r++;
        } while (i < DAT_0063a5d0);
    }
    DAT_0063a5d4 = 0;
    DAT_0063a5d8 = 0;
}
