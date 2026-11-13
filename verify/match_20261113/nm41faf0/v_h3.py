DXo = "d.x = a.pos.x - b.pos.x;"
DYo = "d.y = a.pos.y - b.pos.y;"
VARIANTS = [
 ("dy-dbl", [(DYo, "d.y = (float)((double)a.pos.y - b.pos.y);")]),
 ("dx-dbl", [(DXo, "d.x = (float)((double)a.pos.x - b.pos.x);")]),
 ("dxy-dbl", [(DXo, "d.x = (float)((double)a.pos.x - b.pos.x);"), (DYo, "d.y = (float)((double)a.pos.y - b.pos.y);")]),
 ("dy-dbl-b", [(DYo, "d.y = (float)(a.pos.y - (double)b.pos.y);")]),
 ("dx-dbl-b", [(DXo, "d.x = (float)(a.pos.x - (double)b.pos.x);")]),
 ("dz-dbl-b", [("d.z = (float)((double)a.pos.z - b.pos.z);", "d.z = (float)(a.pos.z - (double)b.pos.z);")]),
 ("dy-neg", [(DYo, "d.y = -(b.pos.y - a.pos.y);")]),
 ("dy-negdbl", [(DYo, "d.y = (float)-((double)b.pos.y - a.pos.y);")]),
 ("dx-neg", [(DXo, "d.x = -(b.pos.x - a.pos.x);")]),
 ("dxz-swap", [("d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;\n    d.z = (float)((double)a.pos.z - b.pos.z);", "d.y = a.pos.y - b.pos.y;\n    d.x = a.pos.x - b.pos.x;\n    d.z = (float)((double)a.pos.z - b.pos.z);")]),
 ("dz-first", [("d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;\n    d.z = (float)((double)a.pos.z - b.pos.z);", "d.x = a.pos.x - b.pos.x;\n    d.z = (float)((double)a.pos.z - b.pos.z);\n    d.y = a.pos.y - b.pos.y;")]),
 ("dy-plusneg", [(DYo, "d.y = a.pos.y + -b.pos.y;")]),
 ("dy-dbl-both", [(DYo, "d.y = (float)((double)a.pos.y - (double)b.pos.y);")]),
 ("dy-ld", [(DYo, "d.y = (float)((long double)a.pos.y - b.pos.y);")]),
]
