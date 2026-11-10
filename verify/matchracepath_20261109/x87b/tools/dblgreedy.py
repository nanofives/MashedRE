"""dblgreedy.py <src> <rva8> [rounds]: greedy search over x87 precision/temp rewrites of single statements in the
MATCH function of src. Transforms per statement `L op= B;` / `L = A op B;` (top-level binary + - *):
  D: L = (float)(A op (double)(B))      (double intermediate, as closed 0x00415e20/0x00475210/0x00476320)
  E: L = (float)((double)(A) op B)
  N: for `L = A - B`: L = -(B) + A
Accept the best variant that lowers the differing-byte count; repeat. Prints one line per round."""
import re, subprocess, sys, pathlib
from concurrent.futures import ThreadPoolExecutor
src, rva = sys.argv[1], sys.argv[2]
rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 4
root = pathlib.Path.cwd()
out = pathlib.Path(__file__).parent / "dg"; out.mkdir(exist_ok=True)
text = pathlib.Path(src).read_text()
m = re.search(r"// MATCH: 0x%s.*?\n" % rva, text, re.I)
start = m.end()
nxt = re.search(r"// MATCH: 0x", text[start:])
end = start + nxt.start() if nxt else len(text)

def split_top(rhs):
    d = 0; pos = None
    for i, c in enumerate(rhs):
        if c in "([": d += 1
        elif c in ")]": d -= 1
        elif d == 0 and c in "+-*" and i > 0:
            j = i - 1
            while j >= 0 and rhs[j] == " ": j -= 1
            if j >= 0 and rhs[j] not in "+-*/(=<>&|!?:,":
                if c in "+-" or pos is None or rhs[pos] not in "+-":
                    if c in "+-": pos = i
                    elif pos is None: pos = i
    if pos is None: return None
    return rhs[:pos].strip(), rhs[pos], rhs[pos + 1:].strip()

ST = re.compile(r"^(\s*)([A-Za-z_][\w\.\[\]\->\*\(\) ]*?)\s*(=|\+=|-=|\*=)\s*([^;=]+);\s*$")
def variants(body):
    lines = body.split("\n"); res = []
    for k, l in enumerate(lines):
        mm = ST.match(l)
        if not mm or "(float)(" in l or "for (" in l or "tB_" in l or "tA_" in l: continue
        ind, L, op, R = mm.groups()
        if op != "=":
            A, o, B = L, op[0], R
        else:
            s = split_top(R)
            if not s: continue
            A, o, B = s
        cands = {"D": f"{ind}{L} = (float)({A} {o} (double)({B}));", "E": f"{ind}{L} = (float)((double)({A}) {o} {B});"}
        if o == "-": cands["N"] = f"{ind}{L} = -({B}) + {A};"
        cands["T"] = f"{ind}{{ float tB_ = {B}; {L} = {A} {o} tB_; }}"
        cands["U"] = f"{ind}{{ float tA_ = {A}; {L} = tA_ {o} {B}; }}"
        for t, nl in cands.items():
            L2 = list(lines); L2[k] = nl; res.append((f"L{k}{t}", "\n".join(L2), nl.strip()))
    return res

def score(t, name):
    p = out / f"{rva}_{name}.c"; p.write_text(t)
    r = subprocess.run([sys.executable, "re/tools/match/match.py", str(p), "--only", rva, "--bytes"], capture_output=True, text=True, cwd=root)
    mm = re.search(r"(\d+) differing unmasked bytes", r.stdout)
    if "BYTE-IDENTICAL" in r.stdout: return 0, p
    return (int(mm.group(1)) if mm else 9999), p

body = text[start:end]
best, _ = score(text, "base"); print(f"base {best}", flush=True)
for rnd in range(rounds):
    vs = variants(body)
    with ThreadPoolExecutor(6) as ex:
        sc = list(ex.map(lambda v: (score(text[:start] + v[1] + text[end:], f"r{rnd}_{v[0]}"), v), vs))
    sc.sort(key=lambda x: x[0][0])
    if not sc or sc[0][0][0] >= best:
        print(f"round {rnd}: no improvement over {best} ({len(vs)} variants)", flush=True); break
    (b, p), v = sc[0]
    best = b; body = v[1]; print(f"round {rnd}: {b} bytes via {v[0]}: {v[2]}  -> {p}", flush=True)
    if b == 0: break
