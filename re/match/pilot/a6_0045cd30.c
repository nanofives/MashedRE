// FLAGS: /O2 /GS /Ob1
/* 0x0045cd30 (563 B). match.py (obj): 51.4% (regs 62.6, ops 82.7). /GL /Ob1 + /LTCG link with
   FUN_0045c820 compiled /GL: 61.4% instruction match (normalized).
   Tried: Ghidra shape 29.1% -> explicit nested switches 33.0 -> switch(n) for 13/25/35 37.4 -> loop
   with `start` as the stepping variable and i only counting (the original has a down-counter) 46.4
   -> k = start - (start >= n ? n : 0) 51.4. Inner case order (3 orders), p before/after
   FUN_0045c720 and a temp for the FUN_0045c820 result: no gain.
   Residuals (revised after the no-/GL evidence, Rich has no prodid 43/44): the original keeps
   p = &DAT_0088f680[idx] in EDX across the call to FUN_0045c820 (0x0045cf4e, mov edx,[edx] at
   0x0045cf53). In plain cl only a same-TU static callee can leave EDX live like that (see
   m_00422470.c). A quick try (static FUN_0045c820 defined first, /Ob1, static or extern self) stays at
   51.4%, because this source never puts p in a register. So it is (b), not (c).
   (b) under a scratch /GL build the head registers matched (n=EBX, slot=EDI, start=ESI). Still open: the frame
   (0x14 vs 4 bytes of locals), the loop's induction set (j, j-n, offset, n*0x2c spilled) and where
   the shared case blocks are placed.
   The default paths read the param_1 home slot ([esp+0x28]) as slot/start. They are uninitialised
   locals in the source, so they stay uninitialised here (our compile does the same thing). */
typedef struct Rec {          /* *DAT_0088f6a0[i] */
    int f0;
    int a;                    /* +0x04 index into DAT_007f1a1c */
    int b;                    /* +0x08 index into DAT_007f1a1c */
    int pad0c[3];
    int div;                  /* +0x18 */
    int base;                 /* +0x1c */
    int total;                /* +0x20 */
} Rec;
typedef struct Ent { int f0; int f4; int pad08[2]; int cnt; int f14; int pad18[3]; int f24; int f28; } Ent; /* 0x2c */
typedef struct Kind { int v; int pad[3]; } Kind;   /* stride 0x10 */
extern Kind DAT_007f1a1c[];
extern Ent *DAT_0088f680[];
extern Rec *DAT_0088f6a0[];
void FUN_0045c720(int idx, int slot);
int  FUN_0045c820(int idx);
int  FUN_004b44f0(int lo, int hi);

// MATCH: 0x0045cd30 _FUN_0045cd30
void FUN_0045cd30(int idx)
{
    Rec *r = DAT_0088f6a0[idx];
    Ent **p;
    int n, slot, start, i, k, found;

    if (r->div > 0) {
        n = r->total / r->div;
        switch (DAT_007f1a1c[r->a].v) {
        case 0:
            switch (DAT_007f1a1c[r->b].v) {
            case 0: case 3: case 4: return;
            case 1: slot = r->base; break;
            case 2: slot = r->base + n; break;
            case 5: slot = r->base + n * 2; break;
            }
            break;
        case 1:
            switch (DAT_007f1a1c[r->b].v) {
            case 1: case 3: case 4: return;
            case 0: slot = r->base; break;
            case 2: slot = r->base + n; break;
            case 5: slot = r->base + n * 2; break;
            }
            break;
        case 2:
            switch (DAT_007f1a1c[r->b].v) {
            case 2: case 3: case 4: return;
            case 0: slot = r->base; break;
            case 1: slot = r->base + n; break;
            case 5: slot = r->base + n * 2; break;
            }
            break;
        case 3: case 4:
            return;
        case 5:
            switch (DAT_007f1a1c[r->b].v) {
            case 3: case 4: case 5: return;
            case 0: slot = r->base; break;
            case 1: slot = r->base + n; break;
            case 2: slot = r->base + n * 2; break;
            }
            break;
        }
    } else {
        n = r->total;
        slot = r->base;
    }

    switch (n) {
    case 0xd:
        start = FUN_004b44f0(0, 6) + FUN_004b44f0(0, 6) + FUN_004b44f0(0, 1);
        break;
    case 0x19:
        start = FUN_004b44f0(0, 0xc) + FUN_004b44f0(0, 0xc) + FUN_004b44f0(0, 1);
        break;
    case 0x23:
        start = FUN_004b44f0(0, 0x11) + FUN_004b44f0(0, 0x11) + FUN_004b44f0(0, 1);
        break;
    }

    if (slot == -1)
        return;
    FUN_0045c720(idx, slot);
    p = &DAT_0088f680[idx];
    if (*p == 0)
        return;
    found = -1;
    if (n > 0) {
        for (i = 0; i < n; i++, start++) {
            k = start - (start >= n ? n : 0);
            if ((*p)[k].cnt == 0) {
                if (found == -1) { found = k; (*p)[k].cnt = 1; }
            } else {
                (*p)[k].cnt++;
                if ((*p)[k].cnt >= n) (*p)[k].cnt = 0;
            }
        }
        if (found != -1) {
            *p = *p + found;
            (*p)->f14 = DAT_0088f6a0[idx]->b;
            (*p)->f4 = DAT_0088f6a0[idx]->a;
        }
    }
    (*p)->f28 = DAT_0088f6a0[idx]->f0;
    (*p)->f24 = FUN_0045c820(idx);
}
