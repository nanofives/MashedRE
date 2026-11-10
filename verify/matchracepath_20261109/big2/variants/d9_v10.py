import runpy
P9 = runpy.run_path("verify/matchracepath_20261109/big2/variants/d9_v9.py")
PR = [("void *FUN_004c39b0(V3_00469df0 *out, const V3_00469df0 *in);", "float FUN_004c39b0(V3_00469df0 *out, const V3_00469df0 *in);")]
V = [("pr", PR), ("prta", PR + P9["TA"]), ("prta2", PR + P9["TA2"])]
