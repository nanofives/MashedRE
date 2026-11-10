GT = [("if (best < f) {\n      idx = 1;", "if (f > best) {\n      idx = 1;"),
      ("if (best < f) {\n      idx = 2;", "if (f > best) {\n      idx = 2;"),
      ("if (best < f)\n      idx = 3;", "if (f > best)\n      idx = 3;")]
ASG = [("f = W_0046f6c0(%d)->len;\n    if (f < 0.0f)\n      f = -f;" % k, "if ((f = W_0046f6c0(%d)->len) < 0.0f)\n      f = -f;" % k) for k in (1,2,3)]
TOP = [("  DAT_0088e650 = 0;\n", "  a = 0;\n  b = 0;\n  DAT_0088e650 = 0;\n"), ("  a = 0;\n  b = 0;\n  if (W_0046f6c0(0)->state == 1) {\n    f = W_0046f6c0(0)->len;", "  if (W_0046f6c0(0)->state == 1) {\n    f = W_0046f6c0(0)->len;")]
TOP2 = [("  DAT_0088e650 = 0;\n", "  b = 0;\n  a = 0;\n  DAT_0088e650 = a;\n"), ("  a = 0;\n  b = 0;\n  if (W_0046f6c0(0)->state == 1) {\n    f = W_0046f6c0(0)->len;", "  if (W_0046f6c0(0)->state == 1) {\n    f = W_0046f6c0(0)->len;")]
QR = [("typedef struct Query_0046f6c0 { V3_0046f6c0 p; float r;", "typedef struct Query_0046f6c0 { V3_0046f6c0 p; int r;"), ("q.r = F_0046f6c0(0x4a4);", "q.r = I_0046f6c0(0x4a4);")]
I0 = [("""    if (W_0046f6c0(0)->state == 1) {
      if (W_0046f6c0(1)->state == 1) {
        if (W_0046f6c0(2)->state == 1) {""", """    if (W_0046f6c0(0)->state == 1) {
      i0 = 0;
      if (W_0046f6c0(1)->state == 1) {
        if (W_0046f6c0(2)->state == 1) {"""),
      ("i0 = 0; i1 = 1; i2 = 2;", "i1 = 1; i2 = 2;"), ("i0 = 0; i1 = 1; i2 = 3;", "i1 = 1; i2 = 3;"), ("i0 = 0; i1 = 3; i2 = 2;", "i1 = 3; i2 = 2;")]
V = [("gt", GT), ("asg", ASG), ("gtasg", GT+ASG), ("top", TOP), ("top2", TOP2), ("qr", QR), ("i0", I0), ("all", GT+TOP+QR+I0)]
