"""Synthetic register-allocation probes: compile small bodies and print our side of the listing only."""
import pathlib, re, subprocess, sys, concurrent.futures

HERE = pathlib.Path(__file__).resolve().parent
MATCH = HERE.parents[2] / "re" / "tools" / "match" / "match.py"
OUT = HERE / "out"
HDR = "// FLAGS: /O2 /Ob1 /G5\ntypedef unsigned int uint;\nextern uint g[16];\nextern void f(uint);\n// MATCH: 0x004dcbb0 FUN_004dcbb0\n"


def ours(name, body):
    p = OUT / f"syn_{name}.c"
    p.write_text(HDR + body)
    o = subprocess.run(["py", "-3.12", str(MATCH), str(p), "--show"], capture_output=True, text=True).stdout
    lines = []
    for ln in o.splitlines()[1:]:
        m = re.match(r"\s+[=!]\s(.*?)\s*\|", ln)
        if m and m.group(1).strip():
            lines.append(m.group(1).strip())
    return lines


if __name__ == "__main__":
    ns = {"__file__": sys.argv[1]}
    exec(pathlib.Path(sys.argv[1]).read_text(), ns)
    with concurrent.futures.ThreadPoolExecutor(8) as ex:
        futs = {k: ex.submit(ours, k, v) for k, v in ns["SYN"].items()}
        for k, f in futs.items():
            print("==", k)
            print("   " + " ; ".join(f.result()))
