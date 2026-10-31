// FLAGS: /O2 /GS /Ob1
/* a5 matching pilot, 0x0047f4c0 (523 B). Best: 100.0% exact (regs 100, ops 100), 157/157 ins.
   Path: typed the bbox as two {x,y,z,int} (1000.0f = 0x447a0000, -1000.0f = 0xc47a0000, pads 0), the
   124-byte local as a struct (no /GS cookie in the original, so not a char array), the 9 bit-sets as one
   SETBIT(m,x,y) macro over a bitmap {int; w at +4; int; bits[] at +0xc}, FUN_0055af40(FUN_0055dec0(r),
   map) as one nested call (map pushed before the inner call), and the 200-dword fill as a for loop
   (rep stosd). First try 94.3%: only shr vs sar, fixed by making the +4 width unsigned.
   Note: the original mixes `or [mem],reg` and `mov edi,[mem]/or/mov [mem],edi` across the 9 identical
   macro expansions. 13.00.9176/9178 reproduces that mix exactly from uniform source. Residual: none. */
typedef struct { int a; unsigned int w; int c; unsigned int bits[1]; } BitMap;
typedef struct { float x, y, z; int pad; } V4;
typedef struct { V4 lo; V4 hi; } BBox;
typedef struct { int d[31]; } Buf;    /* 124 bytes */
extern BitMap *DAT_0086ecc0;
extern int DAT_006c6eb8[200];
void FUN_00562580(Buf *b);
void FUN_0057c4b0(Buf *b, int a, int c);
void FUN_005625e0(Buf *b, int a);
int FUN_0055dc70(Buf *b, int p, BBox *bb);
BitMap *FUN_00562520(int a, int b);
int FUN_0055dec0(int a);
void FUN_0055af40(int a, BitMap *m);

#define SETBIT(m, x, y) ((m)->bits[((y) * (m)->w + (x)) >> 5] |= 1 << (((y) * (m)->w + (x)) & 0x1f))

// MATCH: 0x0047f4c0 _FUN_0047f4c0
int FUN_0047f4c0(int p)
{
    BBox bb;
    Buf buf;
    int r, i;
    bb.lo.x = 1000.0f; bb.lo.y = 1000.0f; bb.lo.z = 1000.0f; bb.lo.pad = 0;
    bb.hi.x = -1000.0f; bb.hi.y = -1000.0f; bb.hi.z = -1000.0f; bb.hi.pad = 0;
    FUN_00562580(&buf);
    FUN_0057c4b0(&buf, 200, 100);
    FUN_005625e0(&buf, 0x20);
    r = FUN_0055dc70(&buf, p, &bb);
    DAT_0086ecc0 = FUN_00562520(10, 10);
    SETBIT(DAT_0086ecc0, 1, 1);
    SETBIT(DAT_0086ecc0, 2, 2);
    SETBIT(DAT_0086ecc0, 3, 3);
    SETBIT(DAT_0086ecc0, 4, 4);
    SETBIT(DAT_0086ecc0, 5, 5);
    SETBIT(DAT_0086ecc0, 6, 6);
    SETBIT(DAT_0086ecc0, 7, 7);
    SETBIT(DAT_0086ecc0, 1, 7);
    SETBIT(DAT_0086ecc0, 8, 8);
    FUN_0055af40(FUN_0055dec0(r), DAT_0086ecc0);
    for (i = 0; i < 200; i++)
        DAT_006c6eb8[i] = 1;
    return r;
}
