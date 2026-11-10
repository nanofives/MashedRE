import re
src=open('re/match/s08/00443dc0.c').read()
i=src.index('// MATCH: 0x00443dc0')
body=src[i:]
def ren(old,new):
    b=re.sub(r'\b%s\b'%old,new,body)
    return [(body,b)]
V=[("best_"+n, ren("best",n)) for n in ["zq","ab","m1","bestx","qq"]]+[("decl_first",[("  V2_00443dc0 a;\n  unsigned char col[4];\n  float best;","  float best;\n  V2_00443dc0 a;\n  unsigned char col[4];")])]
