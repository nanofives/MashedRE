// FLAGS: /O2 /GS /Ob1
/* a6 0x00408a70 (96 B): 100% BYTES-OK. 0x008a9640 is element 0 +0x20 of the 0x30c-stride record array
   based at 0x008a9620 (see 0x00408610); here typed from 0x008a9640. The store of `pos` (+0xa8) must come
   BEFORE the +0xb4 store in source (permuter swap) to get `mov edx,[esp+0xc]` hoisted. FUN_004a2cbd = printf
   (cdecl), FUN_004a2c48 = __ftol emitted by the (int) cast. */
typedef struct {
    int a;          /* +0x000 */
    int b;          /* +0x004 */
    char pad08[0xa0];
    float pos;      /* +0x0a8 */
    char pad0ac[4];
    int c;          /* +0x0b0 */
    int d;          /* +0x0b4 */
    char pad0b8[0x30c - 0xb8];
} Rec30c;
extern Rec30c DAT_008a9640[];
int FUN_004a2cbd(const char *fmt, ...);
// MATCH: 0x00408a70 FUN_00408a70
int FUN_00408a70(int i, float pos)
{
    int v;
    if (i >= 4)
        return 0;
    FUN_004a2cbd("put precisepos %d,%f\n", i, (double)pos);
    v = (int)pos;
    DAT_008a9640[i].a = v;
    DAT_008a9640[i].b = v;
    DAT_008a9640[i].c = v;
    DAT_008a9640[i].pos = pos;
    DAT_008a9640[i].d = v;
    return 1;
}
