src=open('re/match/s08/00443dc0.c').read()
C12="""if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f && a.y > -1.0f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {"""
assert src.count(C12)==2
HDR="// MATCH: 0x00443dc0 FUN_00443dc0\n"
def allrep(new, helper=""):
    t=src.replace(C12,new).replace(HDR, helper+HDR)
    return [(src,t)]
H1="static __inline int InR_(float a, float b) { return a > -1.0f && b > -1.0f && a < 2.0f && b < 2.0f; }\n"
H2="#define INR_(a, b) ((a) > -1.0f && (b) > -1.0f && (a) < 2.0f && (b) < 2.0f)\n"
V=[
 ("inl3", allrep("if (InR_(a.x, b.x) && InR_(a.y, b.y) && InR_(a.y, b.y)) {", H1)),
 ("inl_nest", allrep("if (InR_(a.x, b.x) && InR_(a.y, b.y)) if (InR_(a.y, b.y)) {", H1)),
 ("mac_nest", allrep("if (INR_(a.x, b.x) && INR_(a.y, b.y)) if (INR_(a.y, b.y)) {", H2)),
 ("mac_nest2", allrep("if (INR_(a.x, b.x)) if (INR_(a.y, b.y)) if (INR_(a.y, b.y)) {", H2)),
 ("inl_nest2", allrep("if (InR_(a.x, b.x)) if (InR_(a.y, b.y)) if (InR_(a.y, b.y)) {", H1)),
]
