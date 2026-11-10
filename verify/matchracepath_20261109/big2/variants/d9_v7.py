src = open("re/match/s11/00469df0.c").read()
RET = [("    if (cnt == 0) return 0;\n", "    if (cnt != 0) {\n"),
       ("    if (DAT_007e9de0.count >= 0x100) DAT_007e9de0.count = 0;\n    return p3 == 0;\n}\n", "    if (DAT_007e9de0.count >= 0x100) DAT_007e9de0.count = 0;\n    return p3 == 0;\n    }\n    return 0;\n}\n")]
N3 = [("""    sx = inv * sx;
    sy = inv * sy;
    sz = inv * sz;
    da.x = sx - pb->x;
    da.y = sy - pb->y;
    da.z = sz - pb->z;""", """    n3.x = inv * sx;
    n3.y = inv * sy;
    n3.z = inv * sz;
    da.x = n3.x - pb->x;
    da.y = n3.y - pb->y;
    da.z = n3.z - pb->z;"""),
      ("""    db.x = sx - pa->x;
    db.y = sy - pa->y;
    db.z = sz - pa->z;""", """    db.x = n3.x - pa->x;
    db.y = n3.y - pa->y;
    db.z = n3.z - pa->z;"""),
      ("""    rb.ev[0] = sx;
    rb.ev[1] = sy;
    rb.ev[2] = sz;""", """    rb.ev[0] = n3.x;
    rb.ev[1] = n3.y;
    rb.ev[2] = n3.z;""")]
TA = [("""    ta.x = b->spin.x * DAT_005cea50 + b->vel.x;
    ib.z = 0.0f;
    ib.y = 0.0f;
    ib.x = 0.0f;
    ia.z = 0.0f;
    ia.y = 0.0f;
    ia.x = 0.0f;
    ta.y = b->spin.y * DAT_005cea50 + b->vel.y;
    ta.z = b->spin.z * DAT_005cea50 + b->vel.z;
    tb.x = a->spin.x * DAT_005cea50 + a->vel.x;
    tb.y = a->spin.y * DAT_005cea50 + a->vel.y;
    tb.z = a->spin.z * DAT_005cea50 + a->vel.z;""", """    ta = *vb;
    tb = *va;
    ib.z = 0.0f;
    ib.y = 0.0f;
    ib.x = 0.0f;
    ia.z = 0.0f;
    ia.y = 0.0f;
    ia.x = 0.0f;
    ta.x = b->spin.x * DAT_005cea50 + ta.x;
    ta.y = b->spin.y * DAT_005cea50 + ta.y;
    ta.z = b->spin.z * DAT_005cea50 + ta.z;
    tb.x = a->spin.x * DAT_005cea50 + tb.x;
    tb.y = a->spin.y * DAT_005cea50 + tb.y;
    tb.z = a->spin.z * DAT_005cea50 + tb.z;""")]
V = [("ret", RET), ("n3", N3), ("ta", TA), ("retn3", RET+N3), ("all7", RET+N3+TA)]
