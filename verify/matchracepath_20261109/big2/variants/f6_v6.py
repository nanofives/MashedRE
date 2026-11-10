import runpy
P5 = runpy.run_path("verify/matchracepath_20261109/big2/variants/f6_v5.py")
ARR = [("MGLOBAL(V3_0046f6c0, DAT_006146fc, 0x006146fc);", "MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);"),
       (P5["C0"], """    c.x = S.y * DAT_006146fc[2] - S.z * DAT_006146fc[1];
    c.y = S.z * DAT_006146fc[0] - S.x * DAT_006146fc[2];
    c.z = S.x * DAT_006146fc[1] - S.y * DAT_006146fc[0];""")]
LOC = [(P5["C0"], """    u = DAT_006146fc;
    c.x = S.y * u.z - S.z * u.y;
    c.y = S.z * u.x - S.x * u.z;
    c.z = S.x * u.y - S.y * u.x;""")]
V = [("arr", ARR), ("loc", LOC)]
