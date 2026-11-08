"""C4R-AC80-EXTENT regression: run c4_reclass.gate_match on every `// MATCH:` row of every re/match source (hooks.csv
state ignored, so rows already at C4 are gated too) and write one TSV line per (rva, source): verdict + detail.
Run once on the gate before the extent override and once after; diff the two TSVs.

    py -3.12 verify/c4rac80extent_20261107/regress_gate.py <out.tsv> [--jobs N]
"""
import pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import c4_reclass  # noqa: E402
import match  # noqa: E402


def main():
    out = pathlib.Path(sys.argv[1])
    jobs = int(sys.argv[sys.argv.index("--jobs") + 1]) if "--jobs" in sys.argv else 8
    srcs = sorted(p for p in (ROOT / "re" / "match").rglob("*.c*") if p.suffix in (".c", ".cpp"))
    pairs = []
    for p in srcs:
        rel = p.relative_to(ROOT).as_posix()
        for a, _ in match.parse_annotations(p.read_text(errors="replace")):
            pairs.append((f"{a:08x}", rel))
    sources = sorted({s for _, s in pairs})
    evals = c4_reclass.evaluate(sources, jobs)
    rows = []
    for k, (rva, src) in enumerate(pairs, 1):
        ok, detail, _ = c4_reclass.gate_match(rva, src, evals.get(src, {}))
        v = "PASS" if ok else "REFUSED"
        rows.append(f"0x{rva}\t{src}\t{v}\t{detail}")
        print(f"[{k}/{len(pairs)}] 0x{rva} {v} {detail}", flush=True)
    out.write_text("rva\tsource\tverdict\tdetail\n" + "\n".join(rows) + "\n", encoding="utf-8")
    print(f"{len(pairs)} rows from {len(sources)} sources, "
          f"{sum(r.split(chr(9))[2] == 'PASS' for r in rows)} PASS -> {out}")


if __name__ == "__main__":
    main()
