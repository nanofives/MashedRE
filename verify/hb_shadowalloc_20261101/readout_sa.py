"""HB-SHADOW-ALLOC-COMPARE read-out: one row per shadow_ab.log folder under a data dir.

  py -3.12 verify/hb_shadowalloc_20261101/readout_sa.py D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_data [--md]

Per row: samples, samples with ndiff>0, heap pages compared (min/max of heap=o), allocations per sample (alloc=n),
mismatches (mm), mask size range, mask overflow, block-dword diffs, mutation kind, residue-pin extra blocks and misses
(from the [al] lines), and the log's SHA-256.
"""
import hashlib, re, sys
from pathlib import Path

SAMPLE = re.compile(r"^\[(\d+)\] fn=\S+ rva=([0-9a-f]{8}) idx=-?\d+ ndiff=(\d+)(.*)$")
AL = re.compile(r"^\[al\] fn=\S+ sample=\d+ nA=(\d+) nB=(\d+) pin=(\d+)/(\d+)")


def row(d):
    log = d / "shadow_ab.log"
    if not log.exists():
        return None
    txt = log.read_text(encoding="utf-8", errors="replace").splitlines()
    n = nd = 0; heap = []; an = set(); mm = 0; mask = []; movf = 0; blk = 0; mut = set(); pt = pm = 0
    for ln in txt:
        m = SAMPLE.match(ln)
        if m:
            n += 1; nd += int(m.group(3)) > 0
            rest = m.group(4)
            h = re.search(r"heap=o:(\d+)", rest)
            if h: heap.append(int(h.group(1)))
            a = re.search(r"alloc=n:(\d+),mm:(\d+),mask:(\d+),movf:(\d+),blk:(\d+)(?:,mut:(\d+))?", rest)
            if a:
                an.add(int(a.group(1))); mm += int(a.group(2)); mask.append(int(a.group(3)))
                movf += int(a.group(4)); blk += int(a.group(5)) > 0; mut.add(a.group(6) or "-")
            continue
        m = AL.match(ln)
        if m:
            pt += int(m.group(3)); pm += int(m.group(4))
    rng = lambda v: (f"{min(v)}-{max(v)}" if v else "-")
    return {"row": d.name, "n": n, "ndiff": nd, "heap_o": rng(heap), "alloc_n": ",".join(map(str, sorted(an))) or "-",
            "mm": mm, "mask": rng(mask), "movf": movf, "blk_samples": blk, "mut": ",".join(sorted(mut)) or "-",
            "pin_extra": pt, "pin_miss": pm, "log_sha256": hashlib.sha256(log.read_bytes()).hexdigest()}


def main():
    base = Path(sys.argv[1]); md = "--md" in sys.argv
    rows = [r for r in (row(d) for d in sorted(base.iterdir()) if d.is_dir()) if r]
    cols = ["row", "n", "ndiff", "heap_o", "alloc_n", "mm", "mask", "movf", "blk_samples", "mut", "pin_extra", "pin_miss", "log_sha256"]
    if md:
        print("| " + " | ".join(cols) + " |"); print("|" + "---|" * len(cols))
        for r in rows: print("| " + " | ".join(str(r[c]) if c != "log_sha256" else f"`{r[c][:16]}`" for c in cols) + " |")
    else:
        print("\t".join(cols))
        for r in rows: print("\t".join(str(r[c]) for c in cols))


if __name__ == "__main__":
    main()
