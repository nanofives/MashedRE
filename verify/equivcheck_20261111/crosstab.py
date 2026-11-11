"""EQUIV-CHECK measurement tables: verdict counts, NOT-PROVEN kinds, and the cross with the top-down path.

    py -3.12 verify/equivcheck_20261111/crosstab.py measure_v3.tsv [measure_v3_relax.tsv] > tables.md
"""
import collections
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
ORDER = HERE / "blocking_c0c3.tsv"
BIND = ROOT / "verify" / "porttop_20261108" / "BINDINGS.tsv"
LEDGER = ROOT / "re" / "analysis" / "match_ledger.tsv"


def load(p):
    rows = {}
    for ln in pathlib.Path(p).read_text().splitlines()[1:]:
        f = ln.split("\t")
        rows[int(f[0], 16)] = dict(zip(["rva", "source", "best_pct", "bytes_bad", "bytes_n", "subsystem", "hooks_conf",
                                        "verdict", "classes", "regs", "callers_all_ignore", "kind", "detail"], f))
    return rows


def vkey(r):
    if r["verdict"] == "EQUIV":
        return "EQUIV" if r["classes"] else "EQUIV (byte-identical today)"
    return r["verdict"]


def table(title, counter, total=None):
    print(f"\n**{title}**\n\n| | rows |\n|---|---|")
    for k, n in counter.most_common():
        print(f"| {k} | {n} |")
    if total is not None:
        print(f"| total | {total} |")


def main():
    m = load(HERE / sys.argv[1])
    relax = load(HERE / sys.argv[2]) if len(sys.argv) > 2 else None
    print(f"# Tables from {sys.argv[1]}" + (f" and {sys.argv[2]}" if relax else ""))
    table("Verdicts, all REFUSED rows with a source", collections.Counter(vkey(r) for r in m.values()), len(m))
    hi = {a: r for a, r in m.items() if r["best_pct"] and float(r["best_pct"]) >= 95}
    table("Verdicts, rows at 95% or more", collections.Counter(vkey(r) for r in hi.values()), len(hi))
    if relax:
        table("Verdicts with --relax-icall (A4': calls through a pointer read no register)",
              collections.Counter(vkey(r) for r in relax.values()), len(relax))
    eq = [r for r in m.values() if r["verdict"] != "NOT-PROVEN"]
    table("Class sets of the proven rows (EQUIV and EQUIV-IF-CALLERS-IGNORE)",
          collections.Counter(f"{r['verdict']} [{r['classes'] or 'none'}]" for r in eq))
    cls = collections.Counter()
    for r in eq:
        for c in (r["classes"].split(",") if r["classes"] else ["(byte-identical)"]):
            cls[c] += 1
    table("Proven rows that use each class (a row counts once per class)", cls)
    cond = [r for r in m.values() if r["verdict"] == "EQUIV-IF-CALLERS-IGNORE"]
    table("EQUIV-IF-CALLERS-IGNORE by register set and caller scan",
          collections.Counter(f"{r['regs']} / callers all ignore: {r['callers_all_ignore']}" for r in cond))
    table("NOT-PROVEN by first unexplained difference",
          collections.Counter(r["kind"] for r in m.values() if r["verdict"] == "NOT-PROVEN"))
    table("NOT-PROVEN by first unexplained difference, rows at 95% or more",
          collections.Counter(r["kind"] for r in hi.values() if r["verdict"] == "NOT-PROVEN"))
    # unsupported detail
    un = collections.Counter(re.sub(r"0x[0-9a-f]+|\+0x[0-9a-f]+", "", r["detail"]).split("(")[0].strip()[:60]
                             for r in m.values() if r["kind"] == "unsupported")
    table("unsupported, by construct", un)

    # top-down path
    blocking = {}
    for ln in ORDER.read_text().splitlines():
        if ln.startswith("#") or not ln.strip():
            continue
        f = ln.split()
        blocking[int(f[0], 16)] = f[1]
    ledger = {}
    for ln in LEDGER.read_text().splitlines()[1:]:
        f = ln.split("\t")
        ledger[int(f[0], 16)] = f
    print(f"\n## Top-down path: the {len(blocking)} C0..C3 game functions in reach "
          f"(blocking_c0c3.tsv: origtop_size.py --order today, class game-c0c3)\n")
    c = collections.Counter()
    released = []
    for a, name in sorted(blocking.items()):
        if a in m:
            k = vkey(m[a])
            c[k] += 1
            if m[a]["verdict"] != "NOT-PROVEN":
                released.append((a, name, m[a]))
        elif a in ledger:
            c[f"ledger {ledger[a][14]}, not measured"] += 1
        else:
            c["no match_ledger row (no source)"] += 1
    print("| | functions |\n|---|---|")
    for k, n in c.most_common():
        print(f"| {k} | {n} |")
    print(f"| total | {len(blocking)} |")
    binds = []
    for ln in BIND.read_text().splitlines()[1:]:
        f = ln.split("\t")
        if f[2] in ("stand-in", "under-stand-in"):
            binds.append((int(f[0], 16), f[1], f[2]))
    print(f"\n## BINDINGS.tsv stand-in and under-stand-in rows ({len(binds)})\n")
    c2 = collections.Counter()
    rel2 = []
    for a, name, kind in binds:
        if a in m:
            k = vkey(m[a])
            c2[(kind, k)] += 1
            if m[a]["verdict"] != "NOT-PROVEN":
                rel2.append((a, name, kind, m[a]))
        elif a in ledger:
            c2[(kind, f"ledger {ledger[a][14]}")] += 1
        else:
            c2[(kind, "no match_ledger row")] += 1
    print("| kind | verdict | rows |\n|---|---|---|")
    for (kind, k), n in sorted(c2.items()):
        print(f"| {kind} | {k} | {n} |")
    print("\n## Released functions on the top-down path\n")
    print("| RVA | name | verdict | classes | regs / callers ignore |\n|---|---|---|---|---|")
    for a, name, r in released:
        print(f"| `0x{a:08x}` | {name} | {r['verdict']} | {r['classes'] or 'byte-identical'} | "
              f"{r['regs']} {r['callers_all_ignore']} |")
    print("\n## Released BINDINGS stand-in rows\n")
    print("| RVA | name | kind | verdict | classes | regs / callers ignore |\n|---|---|---|---|---|---|")
    for a, name, kind, r in rel2:
        print(f"| `0x{a:08x}` | {name} | {kind} | {r['verdict']} | {r['classes'] or 'byte-identical'} | "
              f"{r['regs']} {r['callers_all_ignore']} |")


if __name__ == "__main__":
    main()
