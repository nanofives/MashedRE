// FLAGS: /O2 /GS /Ob1
/* a5 matching pilot, 0x00422b50 (75 B). Best: 100.0% exact (regs 100, ops 100).
   Tried: raw Ghidra shape (in-place param_2 -= ...): 35.5%, the compiler tail-duplicates the
   store and keeps n on the stack. A separate local `r` initialised from n (and recomputed
   from n in the branch) makes n live in ESI across the call: exact.
   Residual: none. 0x43960000 = 300.0f pushed as the second arg of FUN_00472650 (cdecl, float
   return, converted by _ftol 0x004a2c48). Stride 0x138 = Rec[] at 0x008995bc. */
typedef struct { int v; int pad[0x4d]; } Rec;   /* stride 0x138 at 0x008995bc */
extern Rec DAT_008995bc[];
int FUN_0040e470(int i);
float FUN_00472650(int a, float b);

// MATCH: 0x00422b50 _FUN_00422b50
void FUN_00422b50(int i, int n)
{
    int r = n;
    if (FUN_0040e470(i) != 1) {
        r = n - (int)FUN_00472650(0, 300.0f);
        if (r < 0)
            r = 0;
    }
    DAT_008995bc[i].v += r;
}
