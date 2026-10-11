# Per-function globals with R/W direction (decomp regex) + exe-side mention/writer status.
import json, os, re, sys, collections
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\scripts")
import rva_body_scan as S
R = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SRC = os.path.join(R, "mashedmod", "src", "mashed_re")
SP = os.path.dirname(os.path.abspath(__file__))
fn = json.load(open(os.path.join(SP, "port.json")))["functions"]
exe_tus = S.load_targets()["exe"]
texts = {}
for rel in exe_tus:
    p = os.path.join(SRC, rel)
    try: texts[rel] = open(p, encoding="utf-8", errors="replace").read()
    except OSError: pass
def strip_comments(t):
    t = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    return re.sub(r"//[^\n]*", "", t)
code = {k: strip_comments(v) for k, v in texts.items()}
def exe_status(addr):
    a = addr & 0xffffffff
    pats = ["0x%08x" % a, "0x%08X" % a, "0x%x" % a, "0x%X" % a]
    w = r = 0; files = set()
    for rel, t in code.items():
        low = t.lower()
        k = "0x%08x" % a; k2 = "0x%x" % a
        for kk in {k, k2}:
            i = low.find(kk)
            while i >= 0:
                seg = low[i:i + 80]
                files.add(rel)
                if re.match(r"0x[0-9a-f]+u?\)?\s*\]?\)?\s*(\[[^\]]*\]\s*)?(=[^=]|\+=|-=|\|=|&=)", seg) or re.match(r"0x[0-9a-f]+u?\)\s*=[^=]", seg):
                    w += 1
                else:
                    r += 1
                i = low.find(kk, i + 1)
    if w: return "exe-writes", files
    if files: return "exe-mention-only", files
    return "no-exe-ref", files
out = {}
for f in fn:
    ent = f.get("entry") or f.get("requested")
    dec = f.get("decomp") or ""
    gl = []
    for g in f.get("globals") or []:
        name = g.get("name", ""); addr = int(str(g.get("addr")), 16) if isinstance(g.get("addr"), str) else g.get("addr")
        wr = bool(re.search(r"\b%s\b(\[[^\]]*\])?\s*(=[^=]|\+=|-=|\|=|&=|\+\+|--)" % re.escape(name), dec)) or \
             bool(re.search(r"\*\([^)]*&%s[^)]*\)\s*=[^=]" % re.escape(name), dec))
        rd = name in dec
        st, files = exe_status(addr)
        gl.append(dict(name=name, addr="0x%08x" % addr, block=g.get("block"), writable=g.get("writable"),
                       dir=("W" if wr else "") + ("R" if rd else ""), exe=st, files=sorted(files)[:3]))
    out[ent] = dict(name=f.get("name"), size=f.get("size"), sig=f.get("signature"), callers=f.get("callers"), globals=gl,
                    callees=[(c.get("name"), c.get("entry")) if isinstance(c, dict) else c for c in (f.get("callee_protos") or [])])
json.dump(out, open(os.path.join(SP, "inputs.json"), "w"), indent=1)
for ent, d in out.items():
    ins = [g for g in d["globals"] if g["writable"]]
    ro = [g for g in d["globals"] if not g["writable"]]
    c = collections.Counter(g["exe"] for g in ins)
    nw = [g["addr"] + ":" + g["dir"] for g in ins if g["exe"] != "exe-writes"]
    print("%s %-26s sz=%s rw-globals=%d %s rdata=%d | not-exe-written: %s" % (ent, (d["name"] or "")[:26], d["size"], len(ins), dict(c), len(ro), " ".join(nw)[:200]))
