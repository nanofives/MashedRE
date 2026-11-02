# Reads the phase-2 clock-call log of detorig.py --call-log (PROMOTE-DETORIG-LOADHOLD, 2026-11-01).
#
# calllog.bin = one 64-byte record per main-thread clock call from the warp to the race anchor:
# [N, kind (0 timeGetTime/GetTickCount, 1 QPC), esp[0..13] at the stub's entry]. esp[0] is the clock
# call's return address. Stack dwords inside MASHED.exe's .text are listed as candidate frames: they are
# a stack scan, not an unwound backtrace, so a stale dword can appear. Functions are named from the
# Ghidra facts cache (log/ghidra_funcfacts.jsonl, built by re/tools/xref.py).
#
#   py -3.12 re/frida/detorig_calllog.py RUN_DIR [RUN_DIR ...]     # per-run call-site histogram
#   py -3.12 re/frida/detorig_calllog.py --diff RUN_A RUN_B         # first record where the two sequences part
import argparse, bisect, json, struct, sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
TEXT_LO, TEXT_HI = 0x00401000, 0x005cc000


def funcs():
    facts = None
    for p in (ROOT, *ROOT.parents):
        f = p / "log" / "ghidra_funcfacts.jsonl"
        if f.is_file():
            facts = f; break
    starts, rows = [], []
    if facts:
        for line in facts.read_text().splitlines():
            r = json.loads(line)
            if "entry" not in r: continue
            for lo, hi in r["ranges"]:
                rows.append((int(lo, 16), int(hi, 16), int(r["entry"], 16)))
    rows.sort(); starts = [r[0] for r in rows]
    return rows, starts


ROWS, STARTS = funcs()


def fn_of(va):
    i = bisect.bisect_right(STARTS, va) - 1
    if i >= 0 and ROWS[i][0] <= va <= ROWS[i][1]:
        return ROWS[i][2]
    return None


def site(va):
    f = fn_of(va)
    return f"0x{va:08x}" + (f" (FUN_{f:08x})" if f else "")


def load(d):
    meta = json.loads((Path(d) / "meta.json").read_text())
    rec = meta.get("calllog", {}).get("rec", 64)
    b = (Path(d) / "calllog.bin").read_bytes()
    out = []
    for o in range(0, len(b) - rec + 1, rec):
        w = struct.unpack_from("<16I", b, o)
        frames = [x for x in w[2:] if TEXT_LO <= x < TEXT_HI]
        out.append({"n": w[0], "kind": w[1], "ret": w[2], "frames": frames})
    return meta, out


def key(r):
    # call site = return address plus the first two further code pointers on the stack
    return (r["ret"],) + tuple(r["frames"][1:3])


def summary(d):
    meta, recs = load(d)
    race = meta.get("race") or {}
    print(f"== {Path(d).name}: {len(recs)} calls logged, dn_warp={race.get('dn_warp')}, race anchor N={race.get('n')}")
    c = Counter(key(r) for r in recs)
    for k, v in c.most_common(25):
        print(f"  {v:5d}  " + "  <-  ".join(site(x) for x in k))
    return recs


def diff(a, b):
    _, ra = load(a); _, rb = load(b)
    ka = [key(r) for r in ra]; kb = [key(r) for r in rb]
    i = 0
    while i < min(len(ka), len(kb)) and ka[i] == kb[i]:
        i += 1
    print(f"{Path(a).name} {len(ka)} calls, {Path(b).name} {len(kb)} calls, identical prefix {i}")
    for j in range(max(0, i - 3), min(i + 8, max(len(ka), len(kb)))):
        sa = "  <-  ".join(site(x) for x in ka[j]) if j < len(ka) else "-"
        sb = "  <-  ".join(site(x) for x in kb[j]) if j < len(kb) else "-"
        print(f"  [{j}] {'=' if sa == sb else '!'}  A {sa}\n        B {sb}")
    # run-length view: consecutive identical call sites collapsed
    for name, ks in ((Path(a).name, ka), (Path(b).name, kb)):
        rl = []
        for k in ks:
            if rl and rl[-1][0] == k: rl[-1][1] += 1
            else: rl.append([k, 1])
        print(f"  {name} runs: " + ", ".join(f"{site(k[0])}x{n}" for k, n in rl[:60]))


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("dirs", nargs="*")
    ap.add_argument("--diff", nargs=2)
    a = ap.parse_args()
    if a.diff:
        diff(*a.diff)
    for d in a.dirs:
        summary(d)
