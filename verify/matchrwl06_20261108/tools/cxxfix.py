import pathlib, re
R = pathlib.Path('re/match/rwl06')
fixes = {
 '00556d20': [("DAT_00912a00 = thunk_FUN_004cc820(0x78,DAT_00623f60,0x10,DAT_00623f64,&DAT_007dc7e8,0x401a2);",
               "DAT_00912a00 = (undefined4)thunk_FUN_004cc820(0x78,DAT_00623f60,0x10,DAT_00623f64,(uint *)&DAT_007dc7e8,0x401a2);")],
 '00558c50': [("*(int *)(param_1 + 0x10) = iVar1;", "*(int *)(param_1 + 0x10) = (int)iVar1;")],
 '00558c80': [("FUN_005c4d50(*(undefined4 *)(param_1 + 0x10));", "FUN_005c4d50((int *)*(undefined4 *)(param_1 + 0x10));")],
 '00559ba0': [("void FUN_00564190(void);", "void FUN_00564190(undefined4 p0);")],
 '0055ba90': [("void FUN_00564190(void);", "void FUN_00564190(undefined4 p0);")],
 '0055fdd0': [("void FUN_00564190(void);", "void FUN_00564190(undefined4 p0);")],
 '0055b030': [("FUN_0055b080(iVar1,param_1,param_2,param_3);", "FUN_0055b080((int *)iVar1,param_1,param_2,param_3);")],
 '0055b060': [("FUN_00559be0(*param_1,param_1);", "FUN_00559be0(*param_1,(undefined4)param_1);")],
 '0055c540': [("FUN_0055c380(param_1,&DAT_005e4f50);", "FUN_0055c380((undefined4 *)param_1,(undefined4)&DAT_005e4f50);")],
 '0055dc70': [("FUN_0055dca0(uVar1,param_1,param_2,param_3);", "FUN_0055dca0((undefined4 *)uVar1,param_1,param_2,param_3);")],
 '0055e2d0': [("FUN_0055e330(uVar1,param_1,param_2,param_3,param_4);", "FUN_0055e330((undefined4 *)uVar1,param_1,(int *)param_2,(undefined4 *)param_3,(undefined4 *)param_4);")],
 '0055e300': [("FUN_0055e190(param_1[0x13]);", "FUN_0055e190((int *)param_1[0x13]);"),
              ("FUN_00559c20(*param_1,param_1);", "FUN_00559c20(*param_1,(undefined4)param_1);")],
 '0055e330': [("param_1[0x13] = param_3;", "param_1[0x13] = (undefined4)param_3;")],
 '0055e440': [("FUN_0055c380(param_1,&DAT_005e51c8);", "FUN_0055c380((undefined4 *)param_1,(undefined4)&DAT_005e51c8);")],
 '00562fd0': [("FUN_0055c380(param_1,&DAT_005e5338);", "FUN_0055c380((undefined4 *)param_1,(undefined4)&DAT_005e5338);")],
}
for v, subs in fixes.items():
    p = R / f'{v}.c'
    t = p.read_bytes().decode('latin-1')
    for a, b in subs:
        assert a in t, (v, a)
        t = t.replace(a, b)
    p.write_bytes(t.encode('latin-1'))
print('ok')
