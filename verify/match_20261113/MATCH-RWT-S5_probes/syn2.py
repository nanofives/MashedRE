B = """void FUN_004dcbb0(uint p)
{
  uint a, b, c, d, i;
  a = g[0]; b = g[1]; c = g[2]; d = g[3];
  f(p);
  i = 0;
  do { g[8 + i] = d; i++; } while (i < p);
  OUT
  g[4] = a; g[5] = b; g[6] = c; g[7] = d;
}
"""
# d: def + 1 loop ref + final store. b: def + N outside refs + final store. who gets esi?
SYN = {}
for n in range(0, 9):
    SYN[f"n{n}"] = B.replace("OUT", " ".join(["f(b);"] * n))
