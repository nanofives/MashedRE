"""PORT-TOP-RWAUDIO-TABLE-632BA8: put a re/match/s30 source into the shared MATCH preamble form so the exe (C++, MATCH_PORT)
can include it: the g2s typedef/macro header is replaced by the preamble (re/match/s26 copy) plus the header names the
text still uses; `extern T DAT_<va>;` / `extern T _DAT_<va>;` become MGLOBAL(T, name, 0x<va>) (address = the name's
suffix). The body below `// MATCH:` and every non-g2s declaration (structs, prototypes) is untouched. In place.
    py -3.12 verify/portorigtop_20261113/rwaudio_632ba8/tools/to_s30.py <va8> [...]"""
import pathlib, re, sys

W = pathlib.Path(__file__).resolve().parents[4]
PRE_SRC = (W / "re/match/s26/005ae7e0.c").read_text(encoding="latin-1")
PRE = PRE_SRC[PRE_SRC.index("/* ---- MATCH-S11 preamble"):PRE_SRC.index("/* ---- end preamble ---- */") + len("/* ---- end preamble ---- */")] + "\n"
SKIP_NAMES = {"bool", "wchar_t"}


def convert(va8):
    p = W / "re/match/s30" / f"{va8}.c"
    src = p.read_bytes().decode("latin-1").replace("\r\n", "\n")
    if "MATCH-S11 preamble" in src:
        print(va8, "already converted"); return
    lines = src.split("\n")
    flags = [l for l in lines if l.startswith("// FLAGS")]
    out, extra = [], []
    for ln in lines:
        if ln.startswith("// FLAGS") or ln.startswith("// TOOLCHAIN"):
            continue
        if ln.startswith("typedef ") and ("undefined" in ln or "HWND" in ln or "LPWSTR" in ln or "ULONG_PTR" in ln
                                          or "unkbyte10" in ln or "INT_PTR" in ln or "HRESULT" in ln):
            for part in ln.split(";"):
                part = part.strip()
                if not part: continue
                nm = re.findall(r"\w+", part.replace("()", ""))[-1]
                if nm in SKIP_NAMES or re.search(rf"\b{nm}\s*;", PRE): continue
                extra.append((part + ";", nm))
            continue
        if ln.startswith("#define ") and re.match(r"#define (true|false|CONCAT|SUB|ZEXT|SEXT)", ln):
            nm = ln.split()[1].split("(")[0]
            extra.append(((("#ifndef __cplusplus\n" + ln + "\n#endif") if nm in ("true", "false") else ln), nm))
            continue
        m = re.match(r"extern (.+?)\s*(\*?)\s*(_?DAT_([0-9a-f]{8}))\s*;$", ln)
        if m:
            t, star, name, a = m.group(1).strip(), m.group(2), m.group(3), m.group(4)
            out.append(f"MGLOBAL({t}{(' ' + star) if star else ''}, {name}, 0x{a});")
            continue
        m = re.match(r"extern (.+?)\s+(_?DAT_([0-9a-f]{8}))\[(\w+)\]\s*;$", ln)
        if m:
            out.append(f"MGLOBALA({m.group(1)}, {m.group(2)}, {m.group(4)}, 0x{m.group(3)});")
            continue
        out.append(ln)
    text = "\n".join(out).strip("\n")
    keep = [d for d, nm in extra if re.search(rf"\b{nm}\b", text)]
    hdr = flags + [f"/* PORT-TOP-RWAUDIO-TABLE-632BA8 0x{va8}: g2s.py output (decomp_pc.py --create --port --callees on the 34"
                   f" table-path functions), shaped where",
                   "   verify/portorigtop_20261113/RESULT_PORT-TOP-RWAUDIO-TABLE-632BA8.md says; globals through MGLOBAL. */"]
    p.write_bytes(("\n".join(hdr) + "\n" + PRE + "\n".join(keep) + ("\n" if keep else "") + text + "\n").encode("latin-1"))
    print(va8, "converted")


for v in sys.argv[1:]:
    convert(v.lower().replace("0x", ""))
