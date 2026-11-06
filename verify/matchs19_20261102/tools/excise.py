"""Remove the old port body of an RVA from a TU (the scanner's anchor), replacing it with a pointer comment.
    py -3.12 excise.py <TU rel to mashedmod/src/mashed_re> <va8> [<va8> ...]
Removes: the comment block directly above the definition, the definition (brace-matched) and a following
RH_ScopedInstall(..., 0x<va8>) line. Prints what it removed. Keeps the file's newline style."""
import pathlib, re, sys

sys.path.insert(0, "scripts")
import rva_body_scan as S

rel, vas = sys.argv[1], [v.lower().replace("0x", "") for v in sys.argv[2:]]
p = S.SRC / rel
for va8 in vas:
    bodies, _ = S.scan_tu(rel)
    rva = int(va8, 16)
    if rva not in bodies:
        raise SystemExit(f"{rel}: no body for 0x{va8}")
    name, dline = bodies[rva]
    raw = p.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.splitlines()
    d = dline - 1
    s = d
    while s - 1 >= 0 and lines[s - 1].strip().startswith("//"):
        s -= 1
    depth, e, seen = 0, d, False
    while e < len(lines):
        code = re.sub(r"//.*", "", lines[e])
        depth += code.count("{") - code.count("}")
        if "{" in code:
            seen = True
        if seen and depth == 0:
            break
        e += 1
    k = e + 1
    while k < len(lines) and not lines[k].strip():
        k += 1
    pat = r"RH_ScopedInstall\s*\(\s*[\w:]+\s*,\s*0x0*" + va8.lstrip("0") + r"\s*\)"
    if k < len(lines) and re.search(pat, lines[k], re.I) and not lines[k].lstrip().startswith("//"):
        e = k
    print(f"--- {rel} 0x{va8} {name}: removing lines {s + 1}-{e + 1}")
    for ln in lines[s:e + 1]:
        print("   - " + ln)
    lines[s:e + 1] = [f"// MATCH-S19: the body of 0x{va8} ({name}) moved to Match/MatchS19_{va8}.cpp,",
                      f"// which compiles the byte-matched source re/match/s19/{va8}.c."]
    p.write_bytes((nl.join(lines) + (nl if raw.endswith("\n") else "")).encode("utf-8"))
