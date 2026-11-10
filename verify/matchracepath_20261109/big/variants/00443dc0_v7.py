src=open('re/match/s08/00443dc0.c').read()
G3=" && a.y > -1.0f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {"
C12="""if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f"""+G3
assert src.count(C12)==2
def g3(ay,by):
    new=C12.replace(G3," && %s > -1.0f && %s > -1.0f && %s < 2.0f && %s < 2.0f) {"%(ay,by,ay,by))
    return [(src,src.replace(C12,new))]
V=[
 ("fidx", g3("((float *)&a)[1]","((float *)&b)[1]")),
 ("xidx", g3("(&a.x)[1]","(&b.x)[1]")),
 ("pa", g3("(&a)->y","(&b)->y")),
 ("chr", g3("*(float *)((char *)&a + 4)","*(float *)((char *)&b + 4)")),
]
