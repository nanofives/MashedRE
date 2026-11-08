"""C++-check fixes for raw-converter 100% rows (casts, callee arity, variadic code typedef under C++). DDK score unchanged."""
import pathlib
R = pathlib.Path('re/match/rwl06')
CODE = ("typedef int code();", "#ifdef __cplusplus\ntypedef int code(...);\n#else\ntypedef int code();\n#endif")
fixes = {
 '0057c1b0': [("FUN_0055c380(param_1,&DAT_005e5e50);", "FUN_0055c380((undefined4 *)param_1,(undefined4)&DAT_005e5e50);")],
 '0057c220': [("void FUN_0055dff0(int p0);", "void FUN_0055dff0(int p0, int p1);")],
 '0057c300': [CODE, ("*puVar1 = &DAT_0062403c;", "*puVar1 = (undefined4)&DAT_0062403c;"),
              ("FUN_0055ded0(puVar1,1,0);", "FUN_0055ded0((int)puVar1,1,0);")],
 '0057c370': [CODE],
 '0057c500': [CODE, ("FUN_0055b860(iVar1,uVar2);", "FUN_0055b860((undefined4 *)iVar1,uVar2);")],
 '0057c550': [CODE, ("void FUN_0055b930(void);", "void FUN_0055b930(int p0);"),
              ("void FUN_00564190(void);", "void FUN_00564190(undefined4 p0);")],
}
for v, subs in fixes.items():
    p = R / f'{v}.c'
    t = p.read_bytes().decode('latin-1')
    for a, b in subs:
        assert a in t, (v, a)
        t = t.replace(a, b, 1)
    p.write_bytes(t.encode('latin-1'))
print('ok')
