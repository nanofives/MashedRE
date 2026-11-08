# MATCH-S18 helper h2 notes
0x004b44f0  26  100.0  100.0  cxx:ok  -  by hand (g2s C2059); r % (hi - lo + 1) + lo; relocheck OK
0x004b4510  51  100.0  100.0  cxx:ok  -  by hand (g2s C2059, not thiscall); float range from (uint)(r & 0x7fffffff); relocheck OK
0x004b4550  247  63.3  63.3  cxx:ok  a  vec3 mean: original unrolls the add loop x4, 9176 x8 (as 0x00401690/0x00499d90); rest lines up
0x004b4650  91  100.0  100.0  cxx:ok  -  lerp as sub/scale/add statement groups on a vec3 struct; relocheck OK
0x004b47c0  30  100.0  100.0  cxx:ok  -  g2s body, param_2 is a pass-through float (not int->float); relocheck OK
0x004b47e0  159  100.0  100.0  cxx:ok  -  keyframe lookup; walking pointer k plus trailing cur/prev pair (single cur++ gave 76.7%); relocheck OK
0x004b4a80  72  100.0  100.0  cxx:ok  -  callback query FUN_00538c80 with cb {p3,&result,p4,p1}; result = 0 written first (else result lands in a dead param slot); relocheck OK
0x004b4ad0  71  100.0  100.0  cxx:ok  -  query record {sphere copy, type 3} -> FUN_004b4a80(p1,&rec,p3,p4); relocheck OK
0x004b4b20  59  100.0  100.0  cxx:ok  -  query record, type 1 before the line copy -> FUN_004b4a80(p1,&rec,p3,1); relocheck OK
0x004b4b60  68  100.0  100.0  cxx:ok  -  query record {sphere copy, type 3} -> FUN_004b4a80(p1,&rec,p3,1); relocheck OK
0x004b4c80  72  100.0  100.0  cxx:ok  -  4a80 form, cb {p3,&result,&param_3,p1}, callback FUN_004b4bb0; relocheck OK
0x004b4cd0  57  100.0  100.0  cxx:ok  -  query record, type 1 before line copy -> FUN_004b4c80; relocheck OK
0x004b4d10  66  100.0  100.0  cxx:ok  -  query record {sphere copy, type 3} -> FUN_004b4c80; relocheck OK
0x004b4e20  70  100.0  100.0  cxx:ok  -  4c80 form on FUN_0053c5d0/FUN_004b4d60, cb {p3,&result,&param_3,0}; relocheck OK
0x004b4e70  57  100.0  100.0  cxx:ok  -  query record, type 1 before line copy -> FUN_004b4e20; relocheck OK
0x004b5af0  661  85.7  85.7  cxx:ok  b  matrix-axes Im3D draw; structure exact, y/z axis products load the axis before the scale in the original (fld order), forms/helpers/flags/permuter no move

## Final (h2)
0x004b44f0  26  100.0  100.0  cxx:ok  -  by hand (g2s C2059); r % (hi - lo + 1) + lo
0x004b4510  51  100.0  100.0  cxx:ok  -  by hand (g2s C2059, not thiscall); float range from (uint)(r & 0x7fffffff)
0x004b4550  247  63.3  63.3  cxx:ok  a  vec3 mean; original unrolls the add loop x4, 9176 x8 (as 0x00401690/0x00499d90)
0x004b4650  91  100.0  100.0  cxx:ok  -  lerp as sub/scale/add statement groups
0x004b47c0  30  100.0  100.0  cxx:ok  -  param_2 is a pass-through float
0x004b47e0  159  100.0  100.0  cxx:ok  -  keyframe lookup, walking pointer k plus trailing cur/prev
0x004b4a80  72  100.0  100.0  cxx:ok  -  callback query, result = 0 written first
0x004b4ad0  71  100.0  100.0  cxx:ok  -  query record {sphere, type 3} -> 4a80(p1,&rec,p3,p4)
0x004b4b20  59  100.0  100.0  cxx:ok  -  query record, type 1 before line copy -> 4a80(...,1)
0x004b4b60  68  100.0  100.0  cxx:ok  -  query record {sphere, type 3} -> 4a80(...,1)
0x004b4c80  72  100.0  100.0  cxx:ok  -  4a80 form, cb {p3,&result,&param_3,p1}
0x004b4cd0  57  100.0  100.0  cxx:ok  -  query record, type 1 before line copy -> 4c80
0x004b4d10  66  100.0  100.0  cxx:ok  -  query record {sphere, type 3} -> 4c80
0x004b4e20  70  100.0  100.0  cxx:ok  -  4c80 form on FUN_0053c5d0, cb {p3,&result,&param_3,0}
0x004b4e70  57  100.0  100.0  cxx:ok  -  query record, type 1 before line copy -> 4e20
0x004b5af0  661  85.7  85.7  cxx:ok  b  matrix-axes Im3D draw; y/z axis product load order (fld axis vs fld scale) residual
