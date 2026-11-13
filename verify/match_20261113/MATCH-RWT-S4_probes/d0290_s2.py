exec(open('verify/match_20261113/MATCH-RWT-S4_probes/d0290_s1.py', encoding='utf-8').read().split('VARIANTS =')[0])
B = [(C0_OLD, C0_NEW), (CUBE_OLD, CUBE_A)]
R_OLD = """          rect.left = param_2->nOffsetX;
          rect.top = param_2->nOffsetY;
          rect.right = rect.left + param_2->width;
          rect.bottom = rect.top + param_2->height;
          pt.x = rect.left;
          pt.y = rect.top;"""
R1 = """          rect.left = param_2->nOffsetX;
          rect.right = rect.left + param_2->width;
          rect.top = param_2->nOffsetY;
          rect.bottom = rect.top + param_2->height;
          pt.x = rect.left;
          pt.y = rect.top;"""
R2 = """          long x = param_2->nOffsetX;
          long y = param_2->nOffsetY;
          rect.left = x;
          rect.top = y;
          rect.right = x + param_2->width;
          rect.bottom = y + param_2->height;
          pt.x = x;
          pt.y = y;"""
R3 = """          rect.left = param_2->nOffsetX;
          rect.top = param_2->nOffsetY;
          rect.right = param_2->nOffsetX + param_2->width;
          rect.bottom = param_2->nOffsetY + param_2->height;
          pt.x = param_2->nOffsetX;
          pt.y = param_2->nOffsetY;"""
R4 = """          pt.x = param_2->nOffsetX;
          pt.y = param_2->nOffsetY;
          rect.left = pt.x;
          rect.top = pt.y;
          rect.right = pt.x + param_2->width;
          rect.bottom = pt.y + param_2->height;"""
R5 = """          rect.left = param_2->nOffsetX;
          rect.top = param_2->nOffsetY;
          rect.right = rect.left + param_2->width;
          rect.bottom = rect.top + param_2->height;
          pt.x = param_2->nOffsetX;
          pt.y = param_2->nOffsetY;"""
VARIANTS = {
    'r0': B,
    'r1': B + [(R_OLD, R1)],
    'r2': B + [(R_OLD, R2)],
    'r3': B + [(R_OLD, R3)],
    'r4': B + [(R_OLD, R4)],
    'r5': B + [(R_OLD, R5)],
}
