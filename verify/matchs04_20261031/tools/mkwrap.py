"""Write the port wrapper TU for a matched source: mashedmod/src/mashed_re/Match/MatchS04_<va8>.cpp
    py -3.12 mkwrap.py <va8> <PortName> <install:0|1> [--bind FUN_x=PortY ...] [--old <old TU>]
The wrapper names the matched function with the port's symbol (#define), compiles the matched source as C++ inside
extern "C", exports the symbol (the .asi's Frida export name) and carries the RH_ScopedInstall the old body had."""
import argparse, pathlib

ap = argparse.ArgumentParser()
ap.add_argument("va8"); ap.add_argument("name"); ap.add_argument("install", type=int)
ap.add_argument("--bind", nargs="*", default=[]); ap.add_argument("--old", default="")
a = ap.parse_args()
va8 = a.va8.lower().replace("0x", "")
W = pathlib.Path("mashedmod/src/mashed_re/Match") / f"MatchS04_{va8}.cpp"
W.parent.mkdir(parents=True, exist_ok=True)
lines = [
    f"// MATCH-S04: port body of 0x{va8} = the byte-matched source re/match/s04/{va8}.c",
    f"// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).",
    f"// The same source text is this RVA's only body in the targets that compile this TU"
    + (f" (moved out of {a.old})." if a.old else "."),
    "// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).",
    '#include "../Core/HookSystem.h"' if a.install else "",
    "",
    "#define MATCH_PORT 1",
    f"#define FUN_{va8} {a.name}",
]
for b in a.bind:
    k, v = b.split("=")
    lines.append(f"#define {k} {v}")
lines += [
    f'#pragma comment(linker, "/EXPORT:{a.name}=_{a.name}")',
    'extern "C" {',
    f'#include "../../../../re/match/s04/{va8}.c"',
    "}",
]
if a.install:
    lines.append(f"RH_ScopedInstall({a.name}, 0x{va8});")
W.write_text("\n".join(l for l in lines if l is not None) + "\n")
print(W)
