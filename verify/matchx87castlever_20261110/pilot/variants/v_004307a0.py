C = "  if (FUN_00429a70(0) + (float)(FUN_00429a80(0) * 60 + FUN_00429a90(0)) <= limit) {"
E = "FUN_00429a70(0) + (float)(FUN_00429a80(0) * 60 + FUN_00429a90(0))"
V = [
 ("swapcmp", [(C, "  if (limit >= " + E + ") {")]),
 ("commalim", [(C, "  if (" + E + " <= (0, limit)) {")]),
 ("dbl", [(C, "  if ((float)(FUN_00429a70(0) + (double)(float)(FUN_00429a80(0) * 60 + FUN_00429a90(0))) <= limit) {")]),
 ("namedf", [(C, "  { float t = " + E + "; if (t <= limit) {"), ("    return 1;\n  }\n  return 0;", "    return 1;\n  } }\n  return 0;")]),
 ("namedd", [(C, "  { double t = " + E + "; if (t <= limit) {"), ("    return 1;\n  }\n  return 0;", "    return 1;\n  } }\n  return 0;")]),
 ("ret", [(C + "\n    return 1;\n  }\n  return 0;", "  return " + E + " <= limit;")]),
 ("notgt", [(C, "  if (!(" + E + " > limit)) {")]),
 ("dlim", [("  float limit;", "  double limit;")]),
 ("dlimcast", [(C, "  if ((double)(" + E + ") <= (double)limit) {")]),
]
