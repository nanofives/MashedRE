BLK = "  { float vx = rec->vel.x, vy = rec->vel.y, vz = rec->vel.z;\n  line.end.x = vx + line.start.x;\n  line.end.y = vy + line.start.y;\n  line.end.z = vz + line.start.z; }\n"
OX = "      out.x += hit.v0.x * 0.05f;\n"
G = (OX, "      { float tB_ = hit.v0.x * 0.05f; out.x = out.x + tB_; }\n")
def blk(x, y, z, pre="  {\n", post="  }\n"):
    return (BLK, pre + "  line.end.x = " + x + ";\n  line.end.y = " + y + ";\n  line.end.z = " + z + ";\n" + post)
V = [
 ("plain", [blk("rec->vel.x + line.start.x", "rec->vel.y + line.start.y", "rec->vel.z + line.start.z"), G]),
 ("plainD", [blk("rec->vel.x + line.start.x", "(float)(rec->vel.y + (double)line.start.y)", "(float)(rec->vel.z + (double)line.start.z)"), G]),
 ("plainW", [blk("rec->vel.x + line.start.x", "(float)(rec->vel.y + (double)line.start.y)", "(float)(rec->vel.z + (double)line.start.z)"), G]),
 ("vxD_yz", [(BLK, BLK.replace("line.end.y = vy + line.start.y;", "line.end.y = (float)(vy + (double)line.start.y);").replace("line.end.z = vz + line.start.z;", "line.end.z = (float)(vz + (double)line.start.z);")), G]),
 ("vxW_yz", [(BLK, BLK.replace("line.end.y = vy + line.start.y;", "{ double t = vy + line.start.y; line.end.y = (float)t; }").replace("line.end.z = vz + line.start.z;", "{ double t = vz + line.start.z; line.end.z = (float)t; }")), G]),
 ("vxD_y_W_z", [(BLK, BLK.replace("line.end.y = vy + line.start.y;", "line.end.y = (float)(vy + (double)line.start.y);").replace("line.end.z = vz + line.start.z;", "{ double t = vz + line.start.z; line.end.z = (float)t; }")), G]),
 ("vxD_y_cz", [(BLK, BLK.replace("line.end.y = vy + line.start.y;", "line.end.y = (float)(vy + (double)line.start.y);").replace("line.end.z = vz + line.start.z;", "line.end.z = (0, vz) + line.start.z;")), G]),
 ("vel_ptr", [(BLK, "  { V3 *v = &rec->vel;\n  line.end.x = v->x + line.start.x;\n  line.end.y = (float)(v->y + (double)line.start.y);\n  line.end.z = v->z + line.start.z; }\n"), G]),
 ("dyz_x", [(BLK, BLK.replace("line.end.x = vx + line.start.x;", "line.end.x = (float)(vx + (double)line.start.x);").replace("line.end.y = vy + line.start.y;", "line.end.y = (float)(vy + (double)line.start.y);").replace("line.end.z = vz + line.start.z;", "line.end.z = (float)(vz + (double)line.start.z);")), G]),
]
