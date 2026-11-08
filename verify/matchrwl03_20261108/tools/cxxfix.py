import sys, pathlib
R = pathlib.Path('re/match/rwl03')
F = {
 '004f4340': [("iVar1 = FUN_004d4170();","iVar1 = (int)FUN_004d4170();"),("iVar2 = FUN_004d4dd0(iVar1);","iVar2 = (int)FUN_004d4dd0((int *)iVar1);"),
   ("undefined * * FUN_004eb9d0(void);","undefined * * FUN_004eb9d0(int p0);"),
   ("uVar3 = FUN_004eb9d0(0);","uVar3 = (undefined4)FUN_004eb9d0(0);"),("uVar3 = FUN_004d4f90(iVar2,0,uVar3);","uVar3 = (undefined4)FUN_004d4f90((uint *)iVar2,0,(int)uVar3);"),
   ("iVar2 = FUN_004d4380(uVar3);","iVar2 = (int)FUN_004d4380((int *)uVar3);")],
 '004f43f0': [("iVar1 = FUN_004d4170();","iVar1 = (int)FUN_004d4170();"),("iVar2 = FUN_004d4dd0(iVar1);","iVar2 = (int)FUN_004d4dd0((int *)iVar1);"),
   ("undefined * * FUN_004eb3c0(void);","undefined * * FUN_004eb3c0(int p0);"),
   ("uVar3 = FUN_004eb3c0(0);","uVar3 = (undefined4)FUN_004eb3c0(0);"),("uVar3 = FUN_004d4f90(iVar2,0,uVar3);","uVar3 = (undefined4)FUN_004d4f90((uint *)iVar2,0,(int)uVar3);"),
   ("iVar2 = FUN_004d4380(uVar3);","iVar2 = (int)FUN_004d4380((int *)uVar3);")],
}
for k, reps in F.items():
    p = R / f'{k}.c'; t = p.read_bytes().decode('latin-1')
    for a, b in reps:
        if a not in t: print('MISSING', k, a)
        t = t.replace(a, b)
    p.write_bytes(t.encode('latin-1'))
