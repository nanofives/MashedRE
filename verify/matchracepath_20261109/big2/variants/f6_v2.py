TAG = [("""      c.x = w->n.x * 1000.0f;
      c.y = w->n.y * 1000.0f;
      c.z = w->n.z * 1000.0f;
      F_0046f6c0(0x9b0) = c.x + F_0046f6c0(0x9b0);
      F_0046f6c0(0x9b4) = c.y + F_0046f6c0(0x9b4);
      F_0046f6c0(0x9b8) = c.z + F_0046f6c0(0x9b8);""", """      u.x = w->n.x * 1000.0f;
      u.y = w->n.y * 1000.0f;
      u.z = w->n.z * 1000.0f;
      F_0046f6c0(0x9b0) = u.x + F_0046f6c0(0x9b0);
      F_0046f6c0(0x9b4) = u.y + F_0046f6c0(0x9b4);
      F_0046f6c0(0x9b8) = u.z + F_0046f6c0(0x9b8);""")]
CROSS = [("""      f = F_0046f6c0(0x9e4) * 0.00001f;
      c.x = (u.z * w->n.y - u.y * w->n.z) * f;
      c.y = (u.x * w->n.z - u.z * w->n.x) * f;
      c.z = (u.y * w->n.x - u.x * w->n.y) * f;
      F_0046f6c0(0x144) = c.x + F_0046f6c0(0x144);
      F_0046f6c0(0x148) = c.y + F_0046f6c0(0x148);
      F_0046f6c0(0x14c) = c.z + F_0046f6c0(0x14c);""", """      axis.x = u.z * w->n.y - u.y * w->n.z;
      axis.y = u.x * w->n.z - u.z * w->n.x;
      axis.z = u.y * w->n.x - u.x * w->n.y;
      f = F_0046f6c0(0x9e4) * 0.00001f;
      axis.x *= f;
      axis.y *= f;
      axis.z *= f;
      F_0046f6c0(0x144) = axis.x + F_0046f6c0(0x144);
      F_0046f6c0(0x148) = axis.y + F_0046f6c0(0x148);
      F_0046f6c0(0x14c) = axis.z + F_0046f6c0(0x14c);""")]
CROSSc = [(CROSS[0][0], CROSS[0][1].replace("axis.", "c."))]
ANG1 = [("ang = (float)(FUN_004a3384(f) * 57.29578f);", "ang = (float)FUN_004a3384(f) * 57.29578f;")]
FIN = [("""    a = 0;
    S.z = 0.0f;
    S.y = 0.0f;
    sx = 0.0f;
    if (W_0046f6c0(0)->state != 1) {
      a = 1;""", """    count = 0;
    S.z = 0.0f;
    S.y = 0.0f;
    sx = 0.0f;
    if (W_0046f6c0(0)->state != 1) {
      count = 1;"""),
  ("""      sx += F_0046f6c0(0xa04);
      a++;""", """      sx += F_0046f6c0(0xa04);
      count++;"""),
  ("""      sx += F_0046f6c0(0xa10);
      a++;""", """      sx += F_0046f6c0(0xa10);
      count++;"""),
  ("""      sx += F_0046f6c0(0xa1c);
      a++;""", """      sx += F_0046f6c0(0xa1c);
      count++;"""),
  ("""    inv = 1.0f / a;
    S.x = F_0046f6c0(0x958) - inv * sx;""", """    inv = 1.0f / count;
    S.x = F_0046f6c0(0x958) - inv * sx;""")]
NORM = [("""    S.x = F_0046f6c0(0x958) - inv * sx;
    S.y = F_0046f6c0(0x95c) - inv * S.y;
    S.z = F_0046f6c0(0x960) - inv * S.z;
    FUN_004c39b0(&S, &S);
    S.x *= -20.0f;""", """    sx = inv * sx;
    S.y = inv * S.y;
    S.z = inv * S.z;
    S.x = F_0046f6c0(0x958) - sx;
    S.y = F_0046f6c0(0x95c) - S.y;
    S.z = F_0046f6c0(0x960) - S.z;
    FUN_004c39b0(&S, &S);
    S.x *= -20.0f;""")]
CSW = [("""    c.x = S.y * DAT_006146fc.z - S.z * DAT_006146fc.y;
    c.y = S.z * DAT_006146fc.x - S.x * DAT_006146fc.z;
    c.z = S.x * DAT_006146fc.y - S.y * DAT_006146fc.x;""", """    c.x = DAT_006146fc.z * S.y - DAT_006146fc.y * S.z;
    c.y = DAT_006146fc.x * S.z - DAT_006146fc.z * S.x;
    c.z = DAT_006146fc.y * S.x - DAT_006146fc.x * S.y;""")]
PE = [("""    S.y = 0.0f;
    S.z *= -20.0f;
    F_0046f6c0(0x9b0) = S.x + F_0046f6c0(0x9b0);
    F_0046f6c0(0x9b4) = S.y + F_0046f6c0(0x9b4);
    F_0046f6c0(0x9b8) = S.z + F_0046f6c0(0x9b8);""", """    S.y = 0.0f;
    S.z *= -20.0f;
    F_0046f6c0(0x9b0) += S.x;
    F_0046f6c0(0x9b4) += S.y;
    F_0046f6c0(0x9b8) += S.z;""")]
DOT = [("f = N.x * F_0046f6c0(0x9c8) + N.y * F_0046f6c0(0x9cc) + N.z * F_0046f6c0(0x9d0);", "f = N.z * F_0046f6c0(0x9d0) + (N.x * F_0046f6c0(0x9c8) + N.y * F_0046f6c0(0x9cc));")]
DOT2 = [("f = N.x * F_0046f6c0(0x9c8) + N.y * F_0046f6c0(0x9cc) + N.z * F_0046f6c0(0x9d0);", "f = N.x * F_0046f6c0(0x9c8) + (N.y * F_0046f6c0(0x9cc) + N.z * F_0046f6c0(0x9d0));")]
V = [("tag", TAG), ("cross", CROSS), ("crossc", CROSSc), ("tagcross", TAG+CROSS), ("ang1", ANG1), ("fin", FIN), ("norm", NORM), ("finnorm", FIN+NORM), ("csw", CSW), ("pe", PE), ("dot", DOT), ("dot2", DOT2)]
