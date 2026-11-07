"""Convert a raw g2s output (D:/MashedWT/MATCH-S20_data/g2s/<va8>.c) into the re/match/s20 form:
FLAGS line /O2 /GS /Ob1, the shared preamble, globals through MGLOBAL (address from decomp.json), body untouched.
    py -3.12 to_s20.py <va8> [<va8> ...]      (run from the MATCH-S20 worktree)"""
import ast, json, pathlib, re, sys

D = pathlib.Path("D:/MashedWT/MATCH-S20_data")
PRE = (D / "tools" / "preamble.h").read_text()
OUT = pathlib.Path("re/match/s20")
dec = {f["entry"]: f for f in json.load(open(D / "decomp.json"))["functions"]}


def convert(va8):
    # g2s writes the decomp's CRLF text in text mode: CR CR LF on disk. Normalize to LF.
    src = (D / "g2s" / f"{va8}.c").read_bytes().decode("latin-1").replace("\r\r\n", "\n").replace("\r\n", "\n")
    f = dec[f"0x{va8}"]
    gl = {g["name"]: g for g in (f["globals"] if isinstance(f["globals"], list) else ast.literal_eval(f["globals"]))}
    lines = src.splitlines()
    out, used_macros = [], []
    body_started = False
    for ln in lines:
        if ln.startswith("// FLAGS") or ln.startswith("// TOOLCHAIN"):
            continue
        if ln.startswith("typedef "):
            for part in ln.split(";"):
                part = part.strip()
                if not part: continue
                nm = re.findall(r"\w+", part.replace("()", ""))[-1]
                if not re.search(rf"{nm}\s*;", PRE) and nm != "bool":
                    used_macros.append((None, part + ";", None, nm))
            continue
        if not body_started and ln.startswith("#define "):
            name = ln.split()[1].split("(")[0]
            if name in ("true", "false"):
                used_macros.append(("#ifndef __cplusplus", ln, "#endif", name))
            else:
                used_macros.append((None, ln, None, name))
            continue
        m = re.match(r"extern (.+?)\s*(\*?)\s*(\w+)\s*;$", ln)
        if m and not body_started:
            t, star, name = m.group(1).strip(), m.group(2), m.group(3)
            if name in gl:
                out.append(f"MGLOBAL({t}{(' ' + star) if star else ''}, {name}, {gl[name]['addr']});")
                continue
            a = re.search(r"([0-9a-f]{8})$", name)
            if a:
                out.append(f"MGLOBAL({t}{(' ' + star) if star else ''}, {name}, 0x{a.group(1)});")
                continue
            raise SystemExit(f"{va8}: global {name} has no address")
        if ln.startswith("// MATCH:"):
            body_started = True
        out.append(ln)
    # keep only macros the text uses
    text = "\n".join(out)
    keep = []
    for pre, ln, post, name in used_macros:
        if re.search(rf"\b{name}\b", text):
            keep += [x for x in (pre, ln, post) if x]
    fl = re.match(r"// FLAGS: (.*)", src).group(1).strip()
    hdr = [f"// FLAGS: {fl}",
           f"/* MATCH-S20 0x{va8}: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body",
           f"   unchanged; globals moved to MGLOBAL. Score: see verify/matchs20_20261107/results.tsv. */"]
    (OUT / f"{va8}.c").write_bytes(("\n".join(hdr) + "\n" + PRE + "\n".join(keep) + ("\n" if keep else "") + text.strip() + "\n").encode("latin-1"))


for v in sys.argv[1:]:
    convert(v.lower().replace("0x", ""))
