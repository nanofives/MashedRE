B = """void FUN_004dcbb0(uint p)
{
  uint a, b, c, d;
  a = g[0]; b = g[1]; c = g[2]; d = g[3];
  f(p);
  g[4] = a; g[5] = b; g[6] = c; g[7] = d;
  EXTRA
}
"""
SYN = {
    "s1": B.replace("EXTRA", ""),
    "s2_dmore": B.replace("EXTRA", "f(d); f(d); g[8] = d;"),
    "s3_amore": B.replace("EXTRA", "f(a); f(a); g[8] = a;"),
    "s4_bmore": B.replace("EXTRA", "f(b); f(b); g[8] = b;"),
    "s5_order": B.replace("a = g[0]; b = g[1]; c = g[2]; d = g[3];", "d = g[3]; c = g[2]; b = g[1]; a = g[0];").replace("EXTRA", ""),
    "s6_dlong": B.replace("EXTRA", "f(0); g[8] = d;"),
    "s7_alonger": B.replace("EXTRA", "f(0); f(1); g[8] = a; f(2); g[9]=a;"),
}
