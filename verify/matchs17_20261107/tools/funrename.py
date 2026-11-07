"""Rename Ghidra/FidDB names in re/match/s17/<va8>.c to FUN_<va8> (the function itself and every callee listed
in decomp.json), so relocheck can resolve them. Whole-word replace, body otherwise untouched.
    py -3.12 funrename.py <va8> [...]"""
import json, pathlib, re, sys
D = pathlib.Path("D:/MashedWT/MATCH-S17_data")
dec = {f["entry"]: f for f in json.load(open(D / "decomp.json"))["functions"]}
for va8 in sys.argv[1:]:
    p = pathlib.Path(f"re/match/s17/{va8}.c"); t = p.read_text(encoding="latin-1")
    f = dec[f"0x{va8}"]
    ren = {f["name"]: f"FUN_{va8}"}
    for c in f["callees"]:
        n, a = c.rsplit("@", 1)
        if not n.startswith("FUN_"): ren[n] = f"FUN_{int(a,16):08x}"
    head, sep, body = t.partition("/* ---- end preamble ---- */")
    for n, r in sorted(ren.items(), key=lambda x: -len(x[0])):
        body = re.sub(rf"(?<![\w]){re.escape(n)}(?![\w])", r, body)
    body = body.replace(f"// MATCH: 0x{va8} FUN_{va8}", f"// MATCH: 0x{va8} FUN_{va8}")
    p.write_text(head + sep + body, encoding="latin-1"); print(va8, ren)
