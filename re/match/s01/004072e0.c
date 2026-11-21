/* 0x004072e0 (198 B). Best 100.0%. Record pointer walk, inner j = 0 assigned before the render-state calls (it is the zero pushed to RwRenderStateSet), j > 0 && j <= 2. */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char r, g, b, a; } RGBA;
struct Atomic { char pad[0x48]; void (*render)(struct Atomic *); };
struct Rec {
    char name[0x10];            /* 0x00 */
    int c10, c14, c18;          /* 0x10 */
    struct Atomic *atoms[10];   /* 0x1c */
    int key;                    /* 0x44 */
    char pad48[0x7c - 0x48];
    V3 p7c;                     /* 0x7c */
    V3 p88;                     /* 0x88 */
    char pad94[0xec - 0x94];
};
struct RwGlobals { char pad[0x20]; int (*renderStateSet)(int, int); };
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_00639d80 (*(struct Rec (*)[1])0x00639d80u)
#else
extern struct Rec DAT_00639d80[];
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_0063a5d0 (*(int *)0x0063a5d0u)
#else
extern int DAT_0063a5d0;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_0063a5d8 (*(int *)0x0063a5d8u)
#else
extern int DAT_0063a5d8;
#endif
#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */
#define DAT_007d3ff8 (*(struct RwGlobals * *)0x007d3ff8u)
#else
extern struct RwGlobals *DAT_007d3ff8;
#endif
void FUN_00406130(void);
void FUN_004b59c0(V3 *a, V3 *b, RGBA *c);

// MATCH: 0x004072e0 FUN_004072e0
void FUN_004072e0(void)
{
    int i, j;
    RGBA col;
    struct Rec *r;
    i = 0;
    FUN_00406130();
    if (0 < DAT_0063a5d0) {
        r = DAT_00639d80;
        do {
            col.r = 0xc2;
            col.g = 8;
            col.b = 4;
            col.a = 0xff;
            if (i == DAT_0063a5d8) {
                j = 0;
                DAT_007d3ff8->renderStateSet(1, 0);
                DAT_007d3ff8->renderStateSet(10, 5);
                DAT_007d3ff8->renderStateSet(11, 2);
                FUN_004b59c0(&r->p7c, &r->p88, &col);
                DAT_007d3ff8->renderStateSet(10, 5);
                DAT_007d3ff8->renderStateSet(11, 6);
                for (; j < 10; j++) {
                    if (j > 0 && j <= 2)
                        r->atoms[j]->render(r->atoms[j]);
                }
            }
            i++;
            r++;
        } while (i < DAT_0063a5d0);
    }
}
