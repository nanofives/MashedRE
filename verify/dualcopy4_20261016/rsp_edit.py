"""rsp_edit.py <remove|add> <asi|exe|both> <Dir\\File.cpp> [--after <Dir\\Other.cpp>]
Edits mashedmod/{asi,exe}_sources.rsp as bytes (CRLF kept, no trailing-newline change)."""
import pathlib, sys

root = pathlib.Path(__file__).resolve().parent.parent.parent
op, which, tu = sys.argv[1], sys.argv[2], sys.argv[3]
after = sys.argv[sys.argv.index("--after") + 1] if "--after" in sys.argv else None
targets = ["asi", "exe"] if which == "both" else [which]
needle = f'"{tu}"'.encode()
for n in targets:
    p = root / "mashedmod" / f"{n}_sources.rsp"
    b = p.read_bytes()
    nl = b"\r\n" if b"\r\n" in b else b"\n"
    ls = b.split(nl)
    if op == "remove":
        keep = [l for l in ls if l.strip() != needle]
        print(n, "removed", len(ls) - len(keep))
        ls = keep
    else:
        if any(l.strip() == needle for l in ls):
            print(n, "already present")
            continue
        at = len(ls) - 1 if ls and ls[-1] == b"" else len(ls)
        if after:
            idx = [i for i, l in enumerate(ls) if l.strip() == f'"{after}"'.encode()]
            assert idx, f"{after} not in {n}"
            at = idx[0] + 1
        ls.insert(at, needle)
        print(n, "added at", at)
    p.write_bytes(nl.join(ls))
