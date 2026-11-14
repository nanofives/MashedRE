"""Which letters does c2's -G<x> (passed as /d2G<x>) accept, and which change the dn listing?
usage: py -3.12 gscan.py <file.c> <tc> [prefix]   (prefix default 'G'; e.g. 'Q', 'O', 'Z')"""
import subprocess, sys, pathlib, os, hashlib, string
MAIN = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
BIN = {"ddk": MAIN / "tools/vc7ddk/bin", "rw9044": MAIN / "tools/vc6pp/bin"}
src = pathlib.Path(sys.argv[1]).resolve(); tc = sys.argv[2]; pre = sys.argv[3] if len(sys.argv) > 3 else "G"
env = {**os.environ, "PATH": str(BIN[tc]) + ";" + os.environ["PATH"]}
base = ["/O2", "/Ob1", "/G5", "/Oy-"]


def lst(extra):
    out = src.with_suffix(".g.asm")
    if out.exists():
        out.unlink()
    r = subprocess.run([str(BIN[tc] / "cl.exe"), "/nologo", "/c", "/Gy", *base, *extra, f"/Fa{out}",
                        f"/Fo{src.with_suffix('.g.obj')}", str(src)], capture_output=True, text=True, cwd=src.parent, env=env)
    if r.returncode:
        return None, (r.stdout + r.stderr).strip().splitlines()[-1]
    body = [l.split(";")[0].strip() for l in out.read_text(errors="replace").splitlines()]
    body = [l for l in body if l and not l.startswith(("TITLE", ".", "INCLUDELIB", "_TEXT", "_DATA", "PUBLIC", "EXTRN"))]
    txt = "\n".join(body)
    return hashlib.md5(txt.encode()).hexdigest()[:8], ("ebx" in txt, "cmp\tesi, DWORD PTR" in txt or "cmp\tesi, DWORD PTR [eax]" in txt)


h0, info0 = lst([])
print("base", h0, info0)
for ch in string.ascii_letters + string.digits + "-+":
    h, info = lst([f"/d2{pre}{ch}"])
    if h is None:
        if "unknown option" not in info and "unrecognized" not in info:
            print(f"{pre}{ch}: ERR {info}")
        continue
    print(f"{pre}{ch}: {h} {'SAME' if h == h0 else 'DIFF'} ebx={info[0]} fold={info[1]}")
