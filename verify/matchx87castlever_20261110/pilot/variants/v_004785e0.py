O0 = "  out[0] = b[2] * ay - b[1] * az;"
V = [
 ("c_b2", [(O0, "  out[0] = (0, b[2]) * ay - b[1] * az;")]),
 ("d_o0", [(O0, "  out[0] = (float)(b[2] * ay - (double)(b[1] * az));")]),
 ("w_o0", [(O0, "  { double t = b[2] * ay - b[1] * az; out[0] = (float)t; }")]),
 ("f_b2", [(O0, "  { float t = b[2]; out[0] = t * ay - b[1] * az; }")]),
 ("ay_first", [(O0, "  out[0] = ay * b[2] - b[1] * az;")]),
 ("b2local", [("  b[2] = p[8] - p[2];", "  { float bz = p[8] - p[2]; b[2] = bz; }")]),
 ("normlocal", [("  float b[3];\n", "  float b[3];\n  int n = norm;\n"), ("  if (norm) {", "  if (n) {")]),
 ("ptrout", [("  out[0] = b[2] * ay - b[1] * az;\n  out[1] = b[0] * az - b[2] * ax;\n  out[2] = b[1] * ax - b[0] * ay;",
              "  { float *o = out; o[0] = b[2] * ay - b[1] * az; o[1] = b[0] * az - b[2] * ax; o[2] = b[1] * ax - b[0] * ay; }")]),
 ("normne0", [("  if (norm) {", "  if (norm != 0) {")]),
]
