"""Count functions in the Mashed Ghidra project via pyghidra (Python 3).

Usage:
    py -3.12 re/tools/count_functions.py
"""
import os
import sys
from pathlib import Path

GHIDRA_INSTALL = Path("C:/Users/maria/Desktop/Proyectos/TD5RE/ghidra_12.0.3_PUBLIC")
PROJECT_LOC = Path("C:/Users/maria/Desktop/Proyectos/Mashed")
PROJECT_NAME = "Mashed"
PROGRAM_NAME = "MASHED.exe"

os.environ["GHIDRA_INSTALL_DIR"] = str(GHIDRA_INSTALL)

import pyghidra  # noqa: E402

pyghidra.start()

from ghidra.base.project import GhidraProject  # noqa: E402

project = GhidraProject.openProject(str(PROJECT_LOC), PROJECT_NAME, True)
try:
    program = project.openProgram("/", PROGRAM_NAME, True)
    try:
        fm = program.getFunctionManager()
        funcs = list(fm.getFunctions(True))
        print(f"FUNCTION_COUNT={len(funcs)}")
        # If --dump <path>, write full inventory: rva,name,size
        if "--dump" in sys.argv:
            out = Path(sys.argv[sys.argv.index("--dump") + 1])
            with out.open("w", encoding="utf-8") as f_out:
                f_out.write("rva,name,body_size\n")
                for fn in funcs:
                    rva = str(fn.getEntryPoint())
                    name = fn.getName()
                    size = fn.getBody().getNumAddresses() if fn.getBody() else 0
                    f_out.write(f"{rva},{name},{size}\n")
            print(f"DUMPED {len(funcs)} rows to {out}")
        else:
            for i, f in enumerate(funcs[:20]):
                print(f"FN[{i}] {f.getName()} @ {f.getEntryPoint()}")
    finally:
        project.close(program)
finally:
    project.close()
