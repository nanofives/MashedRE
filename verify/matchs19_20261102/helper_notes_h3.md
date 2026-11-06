0x004c3730  150  40.4  40.4  cxx:n/a  b  RwV3dTransformPoint C fallback; x87 schedule (in.x kept on stack, y/z spilled via one arg slot) not reproduced; 6 forms + permuter
0x004c3880  133  51.1  51.1  cxx:n/a  b  RwV3dTransformVector C fallback; x kept on x87 via fld m;fmul st(2) in original, 9176 dups x; permuter no gain
0x004c3910  149  83.0  87.2  cxx:n/a  a  V3d normalize w/ RwSplitBits table invsqrt; x87 pop placement around test + scale operand order; permuter no gain
0x004c39b0  271  69.9  88.0  cxx:n/a  b  RwV3dNormalize (sqrt+invsqrt tables, RWERROR); ops all match, in-ptr eax vs ecx register choice; permuter no gain
0x004c3ac0  112  75.0  86.1  cxx:n/a  b  RwV3dLength table sqrt; x87 pops around test + RwGlobals load scheduling; permuter no gain
0x004c3b30  84  100.0  100.0  cxx:ok  -  _rwSqrt: RwSplitBits union (volatile int members) + const volatile table pointer; relocheck OK unchecked=0
0x004c3b90  87  100.0  100.0  cxx:ok  -  _rwInvSqrt: same form as 0x004c3b30 (table +4, ~bits); relocheck OK unchecked=0
