"""Compile C sources with the period compiler (XP DDK cl 13.00.9176 / c2 13.00.9178) into a scratch directory.

  py -3.12 re/tools/match/period_cc.py --out <dir> --flags "/O2 /G6" -I <inc> ... -- <src.c ...>

Headers: tools/vc7ddk/inc/crt (the DDK's CRT headers, extracted from SDKINCS*.CAB). The compiler and headers live under
the main checkout's gitignored tools/ tree.
"""
import argparse
import os
import pathlib
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

MAIN = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
CL = MAIN / "tools" / "vc7ddk" / "bin" / "cl.exe"
CRT_INC = MAIN / "tools" / "vc7ddk" / "inc" / "crt"


def compile_all(srcs, out, flags, incs, defines=(), jobs=8):
    out = pathlib.Path(out)
    out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, INCLUDE=str(CRT_INC), LIB="", CL="", _CL_="")
    env["PATH"] = str(CL.parent) + os.pathsep + env.get("PATH", "")

    def one(src):
        src = pathlib.Path(src)
        obj = out / (src.stem + ".obj")
        cmd = [str(CL), "/nologo", "/c", "/W0", *flags.split(), *[f"/I{i}" for i in incs],
               *[f"/D{d}" for d in defines], f"/Fo{obj}", str(src)]
        p = subprocess.run(cmd, env=env, capture_output=True, text=True)
        if p.returncode:
            return f"FAIL {src.name}: {p.stdout.strip()[-400:]}"
        return None

    with ThreadPoolExecutor(jobs) as ex:
        errs = [e for e in ex.map(one, srcs) if e]
    return errs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--flags", default="/O2")
    ap.add_argument("-I", action="append", default=[])
    ap.add_argument("-D", action="append", default=[])
    ap.add_argument("srcs", nargs="+")
    a = ap.parse_args()
    errs = compile_all(a.srcs, a.out, a.flags, a.I, a.D)
    for e in errs:
        print(e)
    sys.exit(1 if errs else 0)


if __name__ == "__main__":
    main()
