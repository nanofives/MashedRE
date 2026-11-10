O = "  v.x *= DAT_005cc9dc;\n  v.y *= DAT_005cc9dc;\n  v.z *= DAT_005cc9dc;\n"
Y = "  v.y = (float)(v.y * (double)DAT_005cc9dc);\n"
X = {"o": "  v.x *= DAT_005cc9dc;\n", "g": "  v.x = (float)(v.x * (double)DAT_005cc9dc);\n"}
Z = {
 "a": "  v.z = (float)((0, v.z) * (0, (double)DAT_005cc9dc));\n",
 "b": "  v.z = v.z * (0, DAT_005cc9dc);\n",
 "c": "  v.z = (float)((double)(0, v.z) * DAT_005cc9dc);\n",
 "e": "  v.z = (float)(v.z * (double)(0, DAT_005cc9dc));\n",
 "f": "  { float t = v.z; v.z = t * DAT_005cc9dc; }\n",
 "h": "  v.z = (float)((double)DAT_005cc9dc * (0, v.z));\n",
}
V = [(f"x{xk}_z{zk}", [(O, xv + Y + zv)]) for xk, xv in X.items() for zk, zv in Z.items()]
V += [("yxz", [(O, "  v.y = (float)(v.y * (double)DAT_005cc9dc);\n  v.x *= DAT_005cc9dc;\n  v.z *= DAT_005cc9dc;\n")])]
V += [("zfirst", [(O, "  v.z *= DAT_005cc9dc;\n  v.x *= DAT_005cc9dc;\n" + Y)])]
