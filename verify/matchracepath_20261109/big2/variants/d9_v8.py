import re
src = open("re/match/s11/00469df0.c").read()
M = {"005d757c": "0.0f", "005cc320": "1.0f", "005cc32c": "0.5f", "005cc33c": "-1.0f", "005cea50": "600.0f",
     "005cc568": "100.0f", "005ce194": "200.0f", "005cea4c": "2e-07f", "005cd50c": "-0.5f", "005ce268": "0.0005f", "005cc328": "0.01f"}
def lit(keys):
    t = src
    for k in keys:
        t = t.replace(f"MGLOBAL(float, DAT_{k}, 0x{k});\n", "")
        t = re.sub(rf"\bDAT_{k}\b", M[k], t)
    return t
V = [("lit_all", [(src, lit(M))])]
for k in M:
    V.append((f"lit_{k}", [(src, lit([k]))]))
V = [("lit_most", [(src, lit([k for k in M if k not in ("005cc568", "005ce194")]))])]
