import runpy
P = runpy.run_path("verify/matchracepath_20261109/big2/variants/f6_v2.py")
B = P["TAG"] + P["CROSS"] + P["ANG1"] + P["DOT"]
NORM2 = [(P["NORM"][0][0], """    sx *= inv;
    S.y *= inv;
    S.z *= inv;
    S.x = F_0046f6c0(0x958) - sx;
    S.y = F_0046f6c0(0x95c) - S.y;
    S.z = F_0046f6c0(0x960) - S.z;
    FUN_004c39b0(&S, &S);
    S.x *= -20.0f;""")]
NORM3 = [(P["NORM"][0][0], """    S.x = inv * sx;
    S.y = inv * S.y;
    S.z = inv * S.z;
    S.x = F_0046f6c0(0x958) - S.x;
    S.y = F_0046f6c0(0x95c) - S.y;
    S.z = F_0046f6c0(0x960) - S.z;
    FUN_004c39b0(&S, &S);
    S.x *= -20.0f;""")]
V = [("b", B), ("bnorm", B + P["NORM"]), ("bnorm2", B + NORM2), ("bnorm3", B + NORM3), ("bfin", B + P["FIN"]), ("bfinnorm3", B + P["FIN"] + NORM3), ("bpe", B+P["PE"]), ("bcsw", B+P["CSW"])]
