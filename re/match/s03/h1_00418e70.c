// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418e70 FUN_00418e70
/* h1 MATCH-S03 r2: for each of the 64 entries with timer DAT_0063c508[j] <= 0.75f, emits one
   sprite into the block at 0x0063c1c8: position DAT_0063c208[j], colour DAT_005f3300 with alpha
   (1 - t*4/3) * DAT_005f3300.a (__ftol), size t*4/3*pi/2, then
   advances the timer by DAT_007f100c. Then flushes via 0x00476df0. cdecl, no register args. */
typedef struct RwV3d_ { float x, y, z; } RwV3d_;
typedef struct RwRGBA_ { unsigned char r, g, b, a; } RwRGBA_;
#ifndef MATCH_PORT
extern float   DAT_0063c508[64];
extern RwV3d_  DAT_0063c208[64];
extern char    DAT_0063c1c8[];
extern RwRGBA_ DAT_005f3300;
extern float   DAT_007f100c;
void FUN_004769a0(RwV3d_ *pos);
void FUN_004769f0(RwRGBA_ *c);
void FUN_00476a30(float size);
void FUN_00476d00(void *blk);
void FUN_00476df0(void *blk);
#endif

void FUN_00418e70(void)
{
    int j;
    for (j = 0; j < 64; j++) {
        if (DAT_0063c508[j] <= 0.75f) {
            RwRGBA_ c;
            float f;
            f = DAT_0063c508[j] * 1.3333334f;
            c = DAT_005f3300;
            c.a = (unsigned char)((1.0f - f) * DAT_005f3300.a);
            FUN_004769a0(&DAT_0063c208[j]);
            FUN_004769f0(&c);
            FUN_00476a30(f * 1.5707964f);
            FUN_00476d00(DAT_0063c1c8);
            DAT_0063c508[j] += DAT_007f100c;
        }
    }
    FUN_00476df0(DAT_0063c1c8);
}
