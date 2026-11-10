OLD = """  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, DAT_0068f640[i].obj->freq);
"""
def blk(s): return [(OLD, s)]
V = [
 ("chain_f", blk("""  {
  float f;
  DAT_0068f640[i].obj->freq = f = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, f);
  }
""")),
 ("chain_d", blk("""  {
  double f;
  DAT_0068f640[i].obj->freq = (float)(f = 22050.0 - sin(DAT_0069059c) * 500.0);
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, (float)f);
  }
""")),
 ("ld", blk("""  {
  long double f = 22050.0 - sin(DAT_0069059c) * 500.0;
  DAT_0068f640[i].obj->freq = f;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, f);
  }
""")),
 ("startmac", blk("""  START(i, 22050.0 - sin(DAT_0069059c) * 500.0)
""")),
 ("startmac_f", blk("""  START(i, (float)(22050.0 - sin(DAT_0069059c) * 500.0))
""")),
 ("tmp_s", blk("""  {
  double s = sin(DAT_0069059c);
  DAT_0068f640[i].obj->freq = 22050.0 - s * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0 - s * 500.0);
  }
""")),
 ("tmp_s2", blk("""  {
  float s = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    0;
  DAT_0068f640[i].obj->freq = s;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, s);
  }
""")),
]
