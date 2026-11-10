SAVE = [("    save = m->pos;", "    u = m->pos;"), ("    m->pos = save;", "    m->pos = u;")]
NCP = [("""    if (W_0046f6c0(0)->state != 0) {
      N = W_0046f6c0(0)->n;
      a = 1;""", """    if (W_0046f6c0(0)->state != 0) {
      N.x = W_0046f6c0(0)->n.x;
      N.y = W_0046f6c0(0)->n.y;
      N.z = W_0046f6c0(0)->n.z;
      a = 1;""")]
NZ = [("""      N.z *= f;
      FUN_004c39b0(&N, &N);""", """      N.z = f * N.z;
      FUN_004c39b0(&N, &N);""")]
V = [("save", SAVE), ("ncp", NCP), ("nz", NZ), ("all4", SAVE+NCP+NZ)]
