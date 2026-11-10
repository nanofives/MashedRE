"""MATCH-NEARMISS-2 step 1: build the near-miss pool and classify each row's miss type.

Pool = match_ledger.tsv REFUSED rows with best_pct in [95, 100), whose hooks.csv confidence is not C4,
minus CRT / Lua / qhull (L-95: cl 9466 unavailable). Each row is re-scored live with match.py --show --bytes
(XP DDK toolchain, the file's line-1 FLAGS) and its instruction diff is bucketed mechanically:

  order     the differing instructions are the same multiset on both sides (pure reordering)
  x87       any differing instruction is an x87 op (f*), and the row is not pure order
  regalloc  every replace hunk pairs 1:1 with the same mnemonics and differs only in register names
  stack     every differing pair differs only in an esp/ebp displacement or a sub/add esp immediate
  const     every differing pair has the same mnemonic and differs only in an immediate
  count     instruction counts differ (structural: missing/extra code)
  other     anything else (mixed)

Run from the worktree root:  py -3.12 verify/matchnearmiss2_20261109/build_pool.py
Writes POOL.tsv next to this script.
"""
import collections
import csv
import pathlib
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = pathlib.Path.cwd()
OUT = pathlib.Path(__file__).resolve().parent
EXCL = re.compile(r"msvc-crt|msvc-seh|lua|qhull", re.I)
X87 = re.compile(r"^f[a-z]+")
REGS = r"\b(e?[abcd]x|[abcd][lh]|e?[sd]i|e?[sb]p)\b"


def hooks():
    h = {}
    for r in csv.reader(open(ROOT / "hooks.csv", encoding="utf-8")):
        if r and re.fullmatch(r"[0-9a-fA-F]{8}", r[0]):
            h["0x" + r[0].lower()] = r
    return h


def run(row):
    rva, src = row["rva"], row["source"]
    p = subprocess.run([sys.executable, "re/tools/match/match.py", "--show", "--bytes", "--only", rva, src],
                       capture_output=True, text=True, cwd=ROOT, timeout=300)
    return p.stdout + p.stderr


def parse(out, rva):
    head = re.search(r"_FUN_%s[^:]*: ([0-9.]+)%%.*?(\d+)/(\d+) ins" % rva[2:], out, re.I)
    pct = float(head.group(1)) if head else None
    mine, orig = [], []
    for line in out.splitlines():
        m = re.match(r"   ! (.{44}) \| ?(.*)$", line)
        if m:
            a, b = m.group(1).strip(), m.group(2).strip()
            mine.append(a) if a else None
            orig.append(b) if b else None
    bb = re.search(r"(\d+) differing unmasked bytes", out)
    ident = "BYTE-IDENTICAL" in out
    cnt = re.search(r"(\d+)/(\d+) ins", out)
    return pct, mine, orig, (int(bb.group(1)) if bb else (0 if ident else None)), ident, cnt


def norm_addr(s):
    # match.py zeroes our relocated operands; zero the original's absolute addresses too
    return re.sub(r"0x[0-9a-f]{5,}", "0", s)


def classify(mine, orig, cnt):
    if not mine and not orig:
        return "none"
    a, b = [norm_addr(x) for x in mine], [norm_addr(x) for x in orig]
    if collections.Counter(a) == collections.Counter(b):
        return "order"
    if any(X87.match(x) for x in a + b):
        return "x87"
    if len(a) != len(b):
        return "count"
    kinds = set()
    for x, y in zip(a, b):
        mx, my = x.split(" ")[0], y.split(" ")[0]
        if mx != my:
            kinds.add("other")
            continue
        if re.sub(REGS, "R", x) == re.sub(REGS, "R", y):
            kinds.add("regalloc")
        elif re.sub(r"\[(esp|ebp)[^\]]*\]", "[S]", x) == re.sub(r"\[(esp|ebp)[^\]]*\]", "[S]", y) or \
                (re.match(r"(sub|add) esp", x) and re.match(r"(sub|add) esp", y)):
            kinds.add("stack")
        elif re.sub(r"\b(0x[0-9a-f]+|\d+)\b", "I", x) == re.sub(r"\b(0x[0-9a-f]+|\d+)\b", "I", y):
            kinds.add("const")
        else:
            kinds.add("other")
    return kinds.pop() if len(kinds) == 1 else "+".join(sorted(kinds))


def main():
    H = hooks()
    L = list(csv.DictReader(open(ROOT / "re/analysis/match_ledger.tsv", encoding="utf-8"), delimiter="\t"))
    pool = []
    for r in L:
        if r["verdict"] != "REFUSED":
            continue
        try:
            p = float(r["best_pct"])
        except ValueError:
            continue
        if not (95.0 <= p < 100.0):
            continue
        h = H.get(r["rva"].lower())
        conf = h[3] if h else "(no row)"
        sub = h[2] if h else ""
        if conf == "C4" or EXCL.search(sub):
            continue
        pool.append((r, conf, sub))
    with ThreadPoolExecutor(6) as ex:
        outs = list(ex.map(lambda t: run(t[0]), pool))
    rows = []
    for (r, conf, sub), out in zip(pool, outs):
        pct, mine, orig, bad, ident, cnt = parse(out, r["rva"].lower())
        asm = "y" if re.search(r"__asm|naked", (ROOT / r["source"]).read_text(encoding="utf-8", errors="replace")) else "-"
        typ = "identical" if ident else classify(mine, orig, cnt)
        rows.append([r["rva"], r["source"], conf, sub, r["best_pct"], pct, bad, r["bytes_n"],
                     cnt.group(0) if cnt else "", len(mine), len(orig), asm, typ,
                     " ; ".join(orig[:3])[:160]])
    hdr = ["rva", "source", "conf", "subsystem", "ledger_pct", "live_pct", "bytes_bad", "bytes_n", "ins",
           "ours_diff", "orig_diff", "asm", "miss_type", "orig_sample"]
    with open(OUT / "POOL.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(hdr)
        w.writerows(sorted(rows, key=lambda x: (x[12], -(x[5] or 0))))
    c = collections.Counter(x[12] for x in rows)
    for k, v in c.most_common():
        print(f"{v:4d} {k}")
    print("total", len(rows))


if __name__ == "__main__":
    main()
