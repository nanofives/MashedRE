"""Compile a C file with the DDK or rw9044 cl and print the asm listing of each function.
usage: py -3.12 asm.py <file.c> <tc ddk|rw9044> <flags...>"""
import subprocess, sys, pathlib, re
MAIN = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
BIN = {"ddk": MAIN / "tools/vc7ddk/bin", "rw9044": MAIN / "tools/vc6pp/bin"}
src = pathlib.Path(sys.argv[1]).resolve()
tc = sys.argv[2]
flags = sys.argv[3:]
out = src.with_suffix(f".{tc}.asm")
cmd = [str(BIN[tc] / "cl.exe"), "/nologo", "/c", "/Gy", *flags, f"/Fa{out}", f"/Fo{src.with_suffix('.obj')}", str(src)]
r = subprocess.run(cmd, capture_output=True, text=True, cwd=src.parent, env={**__import__('os').environ, "PATH": str(BIN[tc]) + ";" + __import__('os').environ["PATH"]})
if r.returncode:
    print(r.stdout, r.stderr); sys.exit(1)
show = False
for line in out.read_text(errors="replace").splitlines():
    s = line.strip()
    if re.match(r"^\S+\s+PROC", s):
        show = True; print("----", s.split()[0])
        continue
    if re.match(r"^\S+\s+ENDP", s):
        show = False; continue
    if show and s and not s.startswith(";"):
        print("   ", re.sub(r"\s*;.*$", "", s))
