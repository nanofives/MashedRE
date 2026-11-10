B2 = "v.x * zaxis.x + (0, v.y * zaxis.y) + v.z * zaxis.z"
B3 = "(0, v.x * m->right.x + v.y * m->right.y) + (0, m->right.z) * v.z"
def sp(b): return ("d = (FUN_004a3384(" + b + ") * 57.29578f);", "d = FUN_004a3384(" + b + ");\n  d = d * 57.29578f;")
V = [("d2", [sp(B2)]), ("d3", [sp(B3)]), ("d23", [sp(B2), sp(B3)])]
