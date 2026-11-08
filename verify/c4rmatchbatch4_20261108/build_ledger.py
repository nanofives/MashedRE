"""C4R-MATCHBATCH: recompile every reconstructed source and gate each (rva, source) pair with re/tools/c4_reclass.py.

Reads every `// MATCH: 0x<rva>` annotation in re/match/**/*.c and verify/matchpilot_20261030/g2s_raw/*.c, recompiles
each source once (c4_reclass.eval_source = match.compare --bytes + relocheck.check), and gates every pair with
c4_reclass.gate_match (independent of hooks.csv, so rows already C4 are gated too). Writes:
  pairs.tsv                       one row per (rva, source)
  re/analysis/match_ledger.tsv    one row per rva: the best source (PASS first, then a source recorded at 100%, then
                                  the highest insn score)
  batch.tsv                       rva match <best source> for every rva whose best source passes now or that a slice
                                  results.tsv recorded at 100% (the input of c4_reclass.py batch)
Usage: py -3.12 verify/c4rmatchbatch_20261107/build_ledger.py [--jobs 8]
"""
import argparse, csv, pathlib, re, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import c4_reclass as c4  # noqa: E402

ANN = re.compile(r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]+)", re.I)


def recorded100():
    """{rva: set(source)} from every verify/match*/results.tsv row recorded at 100 (make-tsv rule), bare names resolved."""
    out = {}
    for res in sorted(ROOT.glob("verify/match*/results.tsv")):
        for r in c4.load_tsv(res):
            pct = next((r[c] for c in ("best%", "pct", "exact_pct") if r.get(c)), "")
            src = next((r[c] for c in ("source", "file") if r.get(c)), "").replace("\\", "/")
            try:
                ok = float(pct) >= 100.0
            except ValueError:
                ok = False
            if not (ok and r.get("rva")):
                continue
            rva = c4.norm_rva(r["rva"])
            if src == "g2s_raw":
                src = f"verify/matchpilot_20261030/g2s_raw/{rva}.c"
            elif "/" not in src and src:
                src = f"re/match/pilot/{src}"
            out.setdefault(rva, set()).add(src)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=8)
    a = ap.parse_args()
    srcs = sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "re" / "match").rglob("*.c"))
    srcs += sorted(p.relative_to(ROOT).as_posix() for p in (ROOT / "verify/matchpilot_20261030/g2s_raw").glob("*.c"))
    ann = {}
    for s in srcs:
        for h in ANN.findall((ROOT / s).read_text(errors="replace")):
            ann.setdefault(s, []).append(c4.norm_rva(h))
    srcs = [s for s in srcs if s in ann]
    rec = recorded100()
    print(f"{len(srcs)} sources with MATCH annotations, {sum(len(v) for v in ann.values())} annotations, "
          f"{len(rec)} rvas recorded at 100%", flush=True)
    evals = {}
    for k, (s, ev) in enumerate(zip(srcs, c4.evaluate_iter(srcs, a.jobs) if hasattr(c4, "evaluate_iter") else
                                    _eval_iter(srcs, a.jobs)), 1):
        evals[s] = ev
        print(f"[{k}/{len(srcs)}] {s} {'ERROR ' + ev['_error'] if ev.get('_error') else 'OK'}", flush=True)

    raw = c4.read_raw(c4.HOOKS)
    lines, col, index, _ = c4.load_hooks(raw)

    def hk(rva):
        i = index.get(rva)
        if i is None:
            return "", ""
        r = c4.parse_line(lines[i], col)
        return r[col["confidence"]], r[col["subsystem"]]

    pairs = []
    for s in srcs:
        ev = evals[s]
        text = (ROOT / s).read_text(errors="replace")
        for rva in dict.fromkeys(ann[s]):
            ok, detail, _ = c4.gate_match(rva, s, ev)
            r = ev.get(rva, {})
            ft = c4.function_text(text, rva)
            asmflag = "-"
            if ft:
                v = c4.asm_verdict(*ft)
                asmflag = "naked" if v and v.startswith("naked") else ("asm-only" if v else
                          ("asm-in-c" if re.search(r"\b_{1,2}asm\b", ft[1]) else "-"))
            rc = r.get("reloc")
            pairs.append({
                "rva": rva, "source": s, "verdict": "PASS" if ok else "REFUSED", "detail": detail,
                "insn_pct": f"{100 * r['acc']:.1f}" if "acc" in r else "",
                "bytes_bad": str(r.get("bytes_bad", "")), "bytes_n": str(r.get("bytes_n", "")),
                "relocheck": rc[0] if rc else "", "reloc_unchecked": str(len(rc[2])) if rc else "",
                "compiler": c4.COMPILER if ev.get("_toolchain") == "ddk" else str(ev.get("_toolchain", "")),
                "flags": r.get("flags") or ev.get("_flags", ""), "asm": asmflag,
                "recorded100": "y" if s in rec.get(rva, ()) else ("rva" if rva in rec else "n"),
            })
    keys = list(pairs[0])
    with open(HERE / "pairs.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, keys, delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(pairs)

    def rank(p):
        return (p["verdict"] == "PASS", p["recorded100"] == "y", float(p["insn_pct"] or -1), p["source"].startswith("re/"))

    best = {}
    for p in pairs:
        if p["rva"] not in best or rank(p) > rank(best[p["rva"]]):
            best[p["rva"]] = p
    nsrc = {}
    for p in pairs:
        nsrc[p["rva"]] = nsrc.get(p["rva"], 0) + 1
    led = ["rva", "source", "n_sources", "best_pct", "bytes_bad", "bytes_n", "relocheck", "reloc_unchecked", "compiler",
           "flags", "asm", "recorded100", "hooks_conf", "subsystem", "verdict", "detail"]
    rows = []
    for rva in sorted(best):
        p = best[rva]
        conf, sub = hk(rva)
        rows.append({"rva": f"0x{rva}", "source": p["source"], "n_sources": nsrc[rva], "best_pct": p["insn_pct"],
                     "bytes_bad": p["bytes_bad"], "bytes_n": p["bytes_n"], "relocheck": p["relocheck"],
                     "reloc_unchecked": p["reloc_unchecked"], "compiler": p["compiler"], "flags": p["flags"],
                     "asm": p["asm"], "recorded100": "y" if rva in rec else "n", "hooks_conf": conf or "(no row)",
                     "subsystem": sub, "verdict": p["verdict"], "detail": p["detail"]})
    with open(ROOT / "re" / "analysis" / "match_ledger.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, led, delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    missing = sorted(set(rec) - set(best))
    with open(HERE / "batch.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("rva\troute\tevidence\n")
        nb = 0
        for rva in sorted(best):
            if best[rva]["verdict"] == "PASS" or rva in rec:
                f.write(f"0x{rva}\tmatch\t{best[rva]['source']}\n")
                nb += 1
        for rva in missing:   # recorded at 100% but no source carries its MATCH annotation: the gate refuses it
            f.write(f"0x{rva}\tmatch\t{sorted(rec[rva])[0]}\n")
            nb += 1
    npass = sum(1 for p in best.values() if p["verdict"] == "PASS")
    print(f"ledger: {len(best)} rvas, {npass} pass; batch.tsv {nb} rows; {len(missing)} recorded-100 rvas without an "
          f"annotated source", flush=True)


def _eval_iter(srcs, jobs):
    from concurrent.futures import ProcessPoolExecutor
    with ProcessPoolExecutor(max_workers=jobs) as ex:
        yield from ex.map(c4.eval_source, [str(ROOT / s) for s in srcs])


if __name__ == "__main__":
    main()
