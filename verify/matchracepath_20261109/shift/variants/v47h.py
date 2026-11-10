E = "  DAT_008816e0 = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n"
L = "  DAT_00883dec = DAT_008816e0;\n"
D = "  int *cam;\n"
V = [
 ("t_last", [(D, D + "  float t;\n"), (E, "  DAT_008816e0 = t = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n"), (L, "  DAT_00883dec = t;\n")]),
 ("t_all", [(D, D + "  float t;\n"), (E, "  t = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n  DAT_008816e0 = t;\n"), (L, "  DAT_00883dec = t;\n"), ("  DAT_008823e4 = DAT_008816e0;\n", "  DAT_008823e4 = t;\n"), ("  DAT_008830e8 = DAT_008816e0;\n", "  DAT_008830e8 = t;\n")]),
 ("t_ch", [(D, D + "  float t;\n"), (E, "  DAT_008816e0 = t = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n"), (L, "  DAT_00883dec = t;\n"), ("  DAT_008823e4 = DAT_008816e0;\n", "  DAT_008823e4 = t;\n"), ("  DAT_008830e8 = DAT_008816e0;\n", "  DAT_008830e8 = t;\n")]),
 ("v_last", [(E, "  DAT_008816e0 = v.z = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n"), (L, "  DAT_00883dec = v.z;\n")]),
 ("t_first", [(D, D + "  float t;\n"), (E, "  t = DAT_008816e0 = (-DAT_00881650 - DAT_00881638) * DAT_005cc32c;\n"), (L, "  DAT_00883dec = t;\n")]),
]
