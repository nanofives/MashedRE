# MATCH-S19 helper h2 notes

0x004c1a40  44  100.0%  100.0%  cxx:ok  -  float param_2; relocheck OK
0x004c1a70  150  88.4%  88.4%  cxx:ok  b  switch(case 2/default) found; device near/far copied via fld/fstp in original vs mov pair
0x004c1b10  44  100.0%  100.0%  cxx:ok  -  float param_2; relocheck OK
0x004c1b40  104  76.2%  78.6%  cxx:ok  a  and eax,0x4100 vs test ah,0x41 for same fcom, no loop pad in original
0x004c1bb0  40  87.5%  87.5%  cxx:ok  a  fn-pointer load scheduled after push esi in original
0x004c1be0  35  93.3%  93.3%  cxx:ok  a  +0x60 load scheduled after arg pushes in original
0x004c1c10  102  100.0%  100.0%  cxx:ok  -  relocheck OK
0x004c1c80  64  86.4%  86.4%  cxx:ok  a  x load + fld 1.0 scheduled after push esi in original
0x004c1cf0  54  94.1%  94.1%  cxx:ok  a  push esi before mov edx load in original
0x004c2de0  43  92.9%  92.9%  cxx:?  b  lea of out-local scheduled one push earlier in original
0x004c2e10  43  100.0%  100.0%  cxx:?  -  result local nulled on failure gives the branch; relocheck OK
0x004c2e40  45  94.7%  94.7%  cxx:?  b  lea of out-local scheduled one push earlier in original
0x004c2ea0  45  94.7%  94.7%  cxx:?  b  same as 0x004c2e40
0x004c2ed0  43  100.0%  100.0%  cxx:?  -  same form as 0x004c2e10; relocheck OK
0x004c2f00  45  94.7%  94.7%  cxx:?  b  same as 0x004c2e40
0x004c2f60  79  76.9%  92.3%  cxx:?  b  mov eax,[G]; lea esi,[eax+0x10] prologue not reproduced
0x004c2fb0  130  75.6%  88.9%  cxx:?  b  same lea esi prologue + rotated scratch regs on RwGlobals reloads
0x004c3270  50  100.0%  100.0%  cxx:?  -  relocheck OK
0x004c35f0  122  96.7%  96.7%  cxx:?  b  test eax placement in the 4th default block
0x004c3040  102  91.2%  91.2%  cxx:?  b  struct copy matches; three single-slot scheduling moves
0x004c2c90  191  93.2%  93.2%  cxx:?  b  switch + jump table match; arg loads / error-tail stores scheduled differently
0x004c2a30  334  49.6%  49.6%  cxx:?  b  x87 operand choice (fld mem; fmul st(i) vs fld st(i); fmul mem) throughout
