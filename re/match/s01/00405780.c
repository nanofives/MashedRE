// FLAGS: /O2 /GS /Ob1
/* 0x00405780 (257 B). Best 100.0%. Record init over the 0xec-byte array 0x00639d80 (struct S_EC below). Fix vs g2s 52.5%: pointer to the record, loop index `int i = 0` initialised at the declaration (edi zeroed before the strcpy call), float fields zeroed through the zero register. */
typedef struct S_EC {
    char name[0x10];      /* +0x00 */
    int *obj;             /* +0x10 */
    int f14;              /* +0x14 */
    int f18;              /* +0x18 */
    int *slot[11];        /* +0x1c .. +0x44 */
    int f48;              /* +0x48 */
    int f4c;              /* +0x4c */
    float dir[3];         /* +0x50 */
    float hp;             /* +0x5c */
    float f60;            /* +0x60 */
    int f64;              /* +0x64 */
    float f68;            /* +0x68 */
    float f6c;            /* +0x6c */
    float f70;            /* +0x70 */
    float f74;            /* +0x74 */
    float f78;            /* +0x78 */
    float p7c[3];         /* +0x7c */
    float p88[3];         /* +0x88 */
    float f94;            /* +0x94 */
    float f98;            /* +0x98 */
    float f9c;            /* +0x9c */
    float fa0;            /* +0xa0 */
    int fa4;              /* +0xa4 */
    float fa8;            /* +0xa8 */
    float fac;            /* +0xac */
    int n;                /* +0xb0 */
    int ids[6];           /* +0xb4 */
    int fcc;              /* +0xcc */
    float fd0;            /* +0xd0 */
    int fd4;              /* +0xd4 */
    float fd8;            /* +0xd8 */
    int fdc;              /* +0xdc */
    int fe0;              /* +0xe0 */
    int fe4;              /* +0xe4 */
    float fe8;            /* +0xe8 */
} S_EC;
extern S_EC DAT_00639d80[];
extern int DAT_0063a5d0;
char *FUN_004a2b60(char *d, const char *s);
// MATCH: 0x00405780 FUN_00405780
int FUN_00405780(const char *name, int *ids, int n)
{
    S_EC *s = &DAT_00639d80[DAT_0063a5d0];
    int i = 0;
    FUN_004a2b60(s->name, name);
    s->fd0 = 0;
    s->hp = 100.0f;
    s->f14 = 0;
    for (; i < n; i++)
        s->ids[i] = ids[i];
    s->n = n;
    s->fcc = 0;
    s->fe4 = 0;
    s->fe0 = 0;
    s->fdc = 0;
    s->fd4 = 0;
    s->f48 = 0;
    s->fa4 = 0;
    s->fa8 = 0;
    s->fac = 0;
    s->f6c = 0;
    s->f68 = 0;
    s->fd8 = 1.0f;
    s->fe8 = 0.95f;
    s->f70 = 0.6f;
    s->f74 = 1.4f;
    s->f78 = 0.05f;
    s->f94 = 0.09f;
    s->f98 = 10.0f;
    s->f9c = 0.1f;
    s->fa0 = 0.2f;
    return DAT_0063a5d0++;
}
