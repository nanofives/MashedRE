"""MATCH-RWT-S5 probe driver: compile body variants of 0x004dcbb0 and print scores.
usage: py -3.12 drv.py <variants.py> [--tc ddk|rw9044|both] [--show NAME]
<variants.py> defines VARIANTS = {name: body_text}; optional DECLS (extra text before the function)."""
import concurrent.futures, pathlib, re, subprocess, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
SRC = ROOT / "re" / "match" / "rwl01" / "004dcbb0.c"
MATCH = ROOT / "re" / "tools" / "match" / "match.py"
OUT = HERE / "out"
OUT.mkdir(exist_ok=True)

head = SRC.read_text().split("// MATCH: 0x004dcbb0")[0]


def run(name, text, tc, show=False):
    p = OUT / f"{name}.c"
    p.write_text(text)
    cmd = ["py", "-3.12", str(MATCH), str(p), "--toolchain", tc]
    if show:
        cmd.append("--show")
    r = subprocess.run(cmd, capture_output=True, text=True)
    o = r.stdout + r.stderr
    if show:
        return o
    m = re.search(r"(\d+\.\d)% \(regs (\d+\.\d)%, ops (\d+\.\d)%\)\s+(\d+)/(\d+)", o)
    return m.groups() if m else ("ERR", o.strip()[-300:])


def main():
    vf = pathlib.Path(sys.argv[1])
    ns = {"__file__": str(vf.resolve())}
    exec(vf.read_text(), ns)
    decls = ns.get("DECLS", "")
    tcs = ["ddk"]
    if "--tc" in sys.argv:
        t = sys.argv[sys.argv.index("--tc") + 1]
        tcs = ["ddk", "rw9044"] if t == "both" else [t]
    show = sys.argv[sys.argv.index("--show") + 1] if "--show" in sys.argv else None
    items = {}
    for name, body in ns["VARIANTS"].items():
        if show and name != show:
            continue
        items[name] = head + decls + "// MATCH: 0x004dcbb0 FUN_004dcbb0\n" + body
    if show:
        print(run(show, items[show], tcs[0], True))
        return
    with concurrent.futures.ThreadPoolExecutor(8) as ex:
        futs = {(n, tc): ex.submit(run, f"{vf.stem}_{n}_{tc}", t, tc) for n, t in items.items() for tc in tcs}
        for (n, tc), f in futs.items():
            print(f"{n:28s} {tc:7s} {f.result()}")


if __name__ == "__main__":
    main()
