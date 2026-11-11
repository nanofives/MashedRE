import sys,pathlib,os,subprocess,tempfile
sys.path.insert(0,r"C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RT2D-FONT-2/re/tools/match")
import match
SG=pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\SimGolf\tools")
VC=SG/"vc6"/"vc98"; SP5=SG/"vc6sp5"
match.TOOLCHAINS.update({
 "vc6":{"bin":VC/"Bin","extra":[],"path":[],"label":"vc6 8168"},
 "sp5":{"bin":SP5/"VC98"/"Bin","extra":[],"path":[SP5/"Common"/"MSDev98"/"Bin"],"label":"vc6sp5"},
 "sp5c2":{"bin":VC/"Bin","extra":[f"/B2{SP5/'VC98'/'Bin'/'C2.DLL'}"],"path":[SP5/"Common"/"MSDev98"/"Bin"],"label":"rtm+sp5c2"},
})
_orig=match.compile_obj
def compile_obj(src, flags, lang="c", toolchain=match.DEFAULT_TOOLCHAIN, errors_ok=False):
    tc=match.TOOLCHAINS[toolchain]
    if "path" not in tc: return _orig(src,flags,lang,toolchain,errors_ok)
    tmp=pathlib.Path(tempfile.mkdtemp(prefix="m6_")); out=tmp/(src.stem+".obj")
    env={"PATH":";".join(str(d) for d in [tc["bin"],*tc["path"]]),"INCLUDE":"","LIB":"","SystemRoot":r"C:\Windows","TEMP":str(tmp),"TMP":str(tmp)}
    cmd=[str(tc["bin"]/"cl.exe"),"/nologo","/c","/Gy",*tc["extra"],*flags.split(),f"/Fo{out}",f"/Fd{tmp}\\",f"/Tc{src}" if lang=="c" else f"/Tp{src}"]
    r=subprocess.run(cmd,capture_output=True,text=True,env=env)
    if r.returncode!=0 or not out.exists():
        if errors_ok: return None, r.stdout+r.stderr
        raise SystemExit("compile failed:\n"+r.stdout+r.stderr)
    return (out,r.stdout) if errors_ok else out
match.compile_obj=compile_obj
if __name__=="__main__":
    sys.argv=["match.py"]+sys.argv[1:]
    match.main()
