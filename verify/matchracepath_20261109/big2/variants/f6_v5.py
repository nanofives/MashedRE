import runpy
P4 = runpy.run_path("verify/matchracepath_20261109/big2/variants/f6_v4.py")
C0 = """    c.x = S.y * DAT_006146fc.z - S.z * DAT_006146fc.y;
    c.y = S.z * DAT_006146fc.x - S.x * DAT_006146fc.z;
    c.z = S.x * DAT_006146fc.y - S.y * DAT_006146fc.x;"""
CC = [(C0, """    c.x = (0, S.y) * DAT_006146fc.z - (0, S.z) * DAT_006146fc.y;
    c.y = (0, S.z) * DAT_006146fc.x - (0, S.x) * DAT_006146fc.z;
    c.z = (0, S.x) * DAT_006146fc.y - (0, S.y) * DAT_006146fc.x;""")]
CC2 = [(C0, """    c.x = S.y * (0, DAT_006146fc.z) - S.z * (0, DAT_006146fc.y);
    c.y = S.z * (0, DAT_006146fc.x) - S.x * (0, DAT_006146fc.z);
    c.z = S.x * (0, DAT_006146fc.y) - S.y * (0, DAT_006146fc.x);""")]
Y0 = "    F_0046f6c0(0x9b4) = S.y + F_0046f6c0(0x9b4);"
Y1 = [(Y0, "    F_0046f6c0(0x9b4) = F_0046f6c0(0x9b4) + S.y;")]
Y2 = [(Y0, "    F_0046f6c0(0x9b4) = (0, F_0046f6c0(0x9b4)) + S.y;")]
Y3 = [(Y0, "    F_0046f6c0(0x9b4) = S.y + (0, F_0046f6c0(0x9b4));")]
V = [("nz", P4["NZ"]), ("cc", CC), ("cc2", CC2), ("y1", Y1), ("y2", Y2), ("y3", Y3)]
