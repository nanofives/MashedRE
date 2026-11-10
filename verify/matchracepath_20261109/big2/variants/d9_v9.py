src = open("re/match/s11/00469df0.c").read()
import runpy
P7 = runpy.run_path("verify/matchracepath_20261109/big2/variants/d9_v7.py")
TA = [(P7["TA"][0][0].replace("DAT_005cea50", "600.0f"), P7["TA"][0][1].replace("DAT_005cea50", "600.0f"))]
O = TA[0][0]
TA2 = [(O, """    ta.x = vb->x;
    ta.y = vb->y;
    ta.z = vb->z;
    tb.x = va->x;
    tb.y = va->y;
    tb.z = va->z;
    ib.z = 0.0f;
    ib.y = 0.0f;
    ib.x = 0.0f;
    ia.z = 0.0f;
    ia.y = 0.0f;
    ia.x = 0.0f;
    ta.x = b->spin.x * 600.0f + ta.x;
    ta.y = b->spin.y * 600.0f + ta.y;
    ta.z = b->spin.z * 600.0f + ta.z;
    tb.x = a->spin.x * 600.0f + tb.x;
    tb.y = a->spin.y * 600.0f + tb.y;
    tb.z = a->spin.z * 600.0f + tb.z;""")]
TA3 = [(O, O.replace("+ b->vel.", "+ vb->").replace("+ a->vel.", "+ va->"))]
V = [("ta", TA), ("ta2", TA2), ("ta3", TA3)]
