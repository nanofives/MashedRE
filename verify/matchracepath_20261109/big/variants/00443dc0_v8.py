src=open('re/match/s08/00443dc0.c').read()
G3=" && a.y > -1.0f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {"
C12="""if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f"""+G3
V=[("nog3",[(src,src.replace(C12,C12.replace(G3,") {")))]),
   ("g3_other",[(src,src.replace(C12,C12.replace(G3," && a.y > -1.5f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {")))])]
