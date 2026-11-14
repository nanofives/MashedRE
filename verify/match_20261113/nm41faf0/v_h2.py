DX = "d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;\n    d.z = a.pos.z - b.pos.z;"
CALL = "FUN_004c4d20(&b, &axis, 90.0f, 1);"
PX = "b.pos.x += b.up.x * (s.y * 0.5f);"
VARIANTS = [
 ("d-ptr-a", [(DX, "{ V3 *ap = &a.pos; d.x = ap->x - b.pos.x; d.y = ap->y - b.pos.y; d.z = ap->z - b.pos.z; }")]),
 ("d-ptr-b", [(DX, "{ V3 *bp = &b.pos; d.x = a.pos.x - bp->x; d.y = a.pos.y - bp->y; d.z = a.pos.z - bp->z; }")]),
 ("d-ptr-d", [(DX, "{ V3 *dp = &d; dp->x = a.pos.x - b.pos.x; dp->y = a.pos.y - b.pos.y; dp->z = a.pos.z - b.pos.z; }")]),
 ("d-dbl-all", [(DX, "d.x = (float)((double)a.pos.x - b.pos.x);\n    d.y = (float)((double)a.pos.y - b.pos.y);\n    d.z = (float)((double)a.pos.z - b.pos.z);")]),
 ("d-dblz", [(DX, "d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;\n    d.z = (float)((double)a.pos.z - b.pos.z);")]),
 ("d-dblx", [(DX, "d.x = (float)((double)a.pos.x - b.pos.x);\n    d.y = a.pos.y - b.pos.y;\n    d.z = a.pos.z - b.pos.z;")]),
 ("call-ptr", [(CALL, "{ V3 *ax = &axis; FUN_004c4d20(&b, ax, 90.0f, 1); }")]),
 ("call-int", [(CALL, "FUN_004c4d20(&b, &axis, 90.0f, 1 );")]),
 ("px-h", [(PX, "b.pos.x += b.up.x * (s.y * 0.5f);"), ]),
 ("p-dbl-x", [(PX, "b.pos.x = (float)((double)b.pos.x + b.up.x * (s.y * 0.5f));")]),
 ("p-dbl-x2", [(PX, "b.pos.x = (float)(b.up.x * (s.y * 0.5f) + (double)b.pos.x);")]),
 ("p-dbl-x3", [(PX, "b.pos.x += (float)((double)(s.y * 0.5f) * b.up.x);")]),
 ("p-dbl-x4", [(PX, "b.pos.x += (float)((double)s.y * 0.5f * b.up.x);")]),
 ("p-dbl-x5", [(PX, "b.pos.x += (float)(b.up.x * ((double)s.y * 0.5f));")]),
]
