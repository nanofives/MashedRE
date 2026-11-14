DY = "d.y = a.pos.y - b.pos.y;"
PX = "b.pos.x += b.up.x * (s.y * 0.5f);"
PY = "b.pos.y += (s.y * 0.5f) * b.up.y;"
PZ = "b.pos.z += (s.y * 0.5f) * b.up.z;"
VARIANTS = [
 ("base", []),
 ("dy-dbl", [(DY, "d.y = (float)((double)a.pos.y - b.pos.y);")]),
 ("px-dbl", [(PX, "b.pos.x += (float)((double)b.up.x * (s.y * 0.5f));")]),
 ("px-dbl2", [(PX, "b.pos.x = (float)(b.pos.x + (double)b.up.x * (s.y * 0.5f));")]),
 ("px-swap", [(PX, "b.pos.x += (s.y * 0.5f) * b.up.x;")]),
 ("py-swap", [(PY, "b.pos.y += b.up.y * (s.y * 0.5f);")]),
 ("half-dbl", [(PX, "b.pos.x += b.up.x * (float)(s.y * 0.5);")]),
 ("all-dbl05", [(PX, "b.pos.x += (float)(b.up.x * (s.y * 0.5));"), (PY, "b.pos.y += (float)((s.y * 0.5) * b.up.y);"), (PZ, "b.pos.z += (float)((s.y * 0.5) * b.up.z);")]),
 ("dy-swapstmt", [("d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;", "d.y = a.pos.y - b.pos.y;\n    d.x = a.pos.x - b.pos.x;")]),
 ("dz-first", [("d.x = a.pos.x - b.pos.x;\n    d.y = a.pos.y - b.pos.y;\n    d.z = a.pos.z - b.pos.z;", "d.x = a.pos.x - b.pos.x;\n    d.z = a.pos.z - b.pos.z;\n    d.y = a.pos.y - b.pos.y;")]),
 ("dy-neg", [(DY, "d.y = -(b.pos.y - a.pos.y);")]),
 ("dy-plusneg", [(DY, "d.y = a.pos.y + -b.pos.y;")]),
]
