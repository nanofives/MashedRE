import itertools
O = "  v.x *= DAT_005cc9dc;\n  v.y *= DAT_005cc9dc;\n  v.z *= DAT_005cc9dc;\n"
Y = "  v.y = (float)(v.y * (double)DAT_005cc9dc);\n"
X = {
 "o": "  v.x *= DAT_005cc9dc;\n",
 "k": "  v.x = DAT_005cc9dc * v.x;\n",
 "kd": "  v.x = (float)((double)DAT_005cc9dc * v.x);\n",
 "kc": "  v.x = (0, DAT_005cc9dc) * v.x;\n",
 "xc": "  v.x = (0, v.x) * DAT_005cc9dc;\n",
 "ck": "  v.x *= (0, DAT_005cc9dc);\n",
 "dd": "  v.x = (float)((double)v.x * (double)DAT_005cc9dc);\n",
}
Z = {
 "o": "  v.z *= DAT_005cc9dc;\n",
 "zc": "  v.z = (0, v.z) * DAT_005cc9dc;\n",
 "zcd": "  v.z = (float)((0, v.z) * (double)DAT_005cc9dc);\n",
 "kd": "  v.z = (float)((double)DAT_005cc9dc * v.z);\n",
 "k": "  v.z = DAT_005cc9dc * v.z;\n",
}
V = []
for xk, xv in X.items():
    for zk, zv in Z.items():
        V.append((f"x{xk}_z{zk}", [(O, xv + Y + zv)]))
