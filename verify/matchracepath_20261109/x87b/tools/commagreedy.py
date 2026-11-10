"""commagreedy.py <src> <rva8> [rounds] [--qword-ok]: greedy search over the comma-operator rvalue lever (MATCH-RACEPATH-BIG,
0x00446520) inside the MATCH function of src. Per assignment statement `L = R;` / `L op= R;`:
  C<k>: wrap the k-th member/index access on the RHS as `(0, X)` (same value and type, an rvalue the compiler loads first)
  A:    `L op= R` -> `L = (0, L) op (R)`
Accepts the best variant that lowers the differing-byte count; repeats. A variant whose listing has more
`qword ptr [0]` operands than the base is rejected (a double literal the original never has).
Prints one line per round."""
import re, subprocess, sys, pathlib
from concurrent.futures import ThreadPoolExecutor
args = [a for a in sys.argv[1:] if not a.startswith("--")]
src, rva = args[0], args[1]
rounds = int(args[2]) if len(args) > 2 else 4
root = pathlib.Path.cwd()
out = pathlib.Path(__file__).parent / "cg"; out.mkdir(exist_ok=True)
text = pathlib.Path(src).read_text()
m = re.search(r"// MATCH: 0x%s.*?\n" % rva, text, re.I)
start = m.end()
nxt = re.search(r"// MATCH: 0x", text[start:])
end = start + nxt.start() if nxt else len(text)

ACC = re.compile(r"(?<![\w\)\]])(?:\*\s*)?[A-Za-z_]\w*(?:\s*(?:->|\.)\s*\w+|\[[^\[\]]*\])+")
ST = re.compile(r"^(\s*)([^;=]*?[\w\]\)])\s*(=|\+=|-=|\*=|/=)\s*([^;]+);\s*(.*)$")

def variants(body):
    lines = body.split("\n"); res = []
    for k, l in enumerate(lines):
        if "(0, " in l and l.count("(0, ") > 6: continue
        mm = ST.match(l)
        if not mm or l.strip().startswith(("for", "if", "while", "//", "/*", "return")): continue
        ind, L, op, R, tail = mm.groups()
        if "==" in op: continue
        for j, a in enumerate(ACC.finditer(R)):
            if R[max(0, a.start() - 4):a.start()] == "(0, ": continue
            nr = R[:a.start()] + "(0, " + a.group(0) + ")" + R[a.end():]
            res.append((f"L{k}C{j}", lines[:k] + [f"{ind}{L} {op} {nr}; {tail}".rstrip()] + lines[k + 1:]))
        if op != "=":
            res.append((f"L{k}A", lines[:k] + [f"{ind}{L} = (0, {L}) {op[0]} ({R}); {tail}".rstrip()] + lines[k + 1:]))
    return [(n, "\n".join(ls)) for n, ls in res]

def score(t, name):
    p = out / f"{rva}_{name}.c"; p.write_text(t)
    r = subprocess.run([sys.executable, "re/tools/match/match.py", str(p), "--only", rva, "--bytes", "--show"],
                       capture_output=True, text=True, cwd=root)
    q = sum(1 for l in r.stdout.splitlines() if "qword ptr [0]" in l.split("|")[0])
    if "BYTE-IDENTICAL" in r.stdout: return 0, q, p
    mm = re.search(r"(\d+) differing unmasked bytes", r.stdout)
    return (int(mm.group(1)) if mm else 9999), q, p

body = text[start:end]
best, q0, _ = score(text, "base"); print(f"base {best} (qword {q0})", flush=True)
for rnd in range(rounds):
    vs = variants(body)
    with ThreadPoolExecutor(8) as ex:
        sc = list(ex.map(lambda v: (score(text[:start] + v[1] + text[end:], f"r{rnd}_{v[0]}"), v), vs))
    sc = [s for s in sc if s[0][1] <= q0]
    sc.sort(key=lambda x: x[0][0])
    if not sc or sc[0][0][0] >= best:
        print(f"round {rnd}: no improvement over {best} ({len(vs)} variants)", flush=True); break
    (b, q, p), v = sc[0]
    best = b; body = v[1]; print(f"round {rnd}: {b} bytes via {v[0]}  -> {p}", flush=True)
    if b == 0: break
