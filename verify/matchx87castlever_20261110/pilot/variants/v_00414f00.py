X0 = "v.x = pos[0] - *(float *)(rec + 0x30);"
Z0 = "v.z = pos[2] - *(float *)(rec + 0x38);"
V = [
 ("cxz", [(X0, "v.x = pos[0] - (0, *(float *)(rec + 0x30));"), (Z0, "v.z = pos[2] - (0, *(float *)(rec + 0x38));")]),
 ("cx", [(X0, "v.x = pos[0] - (0, *(float *)(rec + 0x30));")]),
 ("dxz", [(X0, "v.x = (float)(pos[0] - (double)*(float *)(rec + 0x30));"), (Z0, "v.z = (float)(pos[2] - (double)*(float *)(rec + 0x38));")]),
 ("wxz", [(X0, "{ double t = pos[0] - *(float *)(rec + 0x30); v.x = (float)t; }"), (Z0, "{ double t = pos[2] - *(float *)(rec + 0x38); v.z = (float)t; }")]),
]
