A = """  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6424) == 0) {
    return 1;
  }"""
C = """  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6434) == 0) {
    return 1;
  }"""
V = [
 ("goto1", [(A, """  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6424) != 0) goto L1;
  return 1;
L1:""")]),
 ("var1", [(A, """  if ((iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6424)) == 0) {
    return iVar1 + 1;
  }""")]),
 ("else_chain", [(A, """  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6424) == 0) {
    return 1;
  } else""")]),
 ("var3", [(C, """  if ((iVar1 = FUN_005adf30((byte *)param_1->w[1],DAT_005e6434)) == 0) {
    return iVar1 + 1;
  }""")]),
 ("not1", [(A, """  if (!FUN_005adf30((byte *)param_1->w[1],DAT_005e6424)) {
    return 1;
  }""")]),
 ("bool1", [(A, """  if (FUN_005adf30((byte *)param_1->w[1],DAT_005e6424) == 0) {
    return (uint)(param_1 != 0) | 1;
  }""")]),
]
