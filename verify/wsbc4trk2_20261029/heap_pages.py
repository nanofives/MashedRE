"""D4-WSB-C4-SOLV-TRK2 (copy of verify/wsbc4trk1_20261028/heap_pages.py from orch/D4-WSB-C4-SOLV-TRK1, data path, label, runner names changed; that file: (copy of verify/wsbsolvws_20261027/heap_pages.py, data path changed): per-row page counts from leg H shadow logs (the PREREG read-out fields).

  py -3.12 verify/wsbc4trk2_20261029/heap_pages.py <rva> [<rva> ...]   (reads D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data/<rva>_heap/shadow_ab.log)

Per row: samples, divergent samples, pages o/n min..max, heap o/n min..max, heap pages compared (sum of max(o,n) per
sample), image pages compared (pages - heap), max stack/noise/ovf/ret, NOISY / UNTRACKED / OTHER-EXC lines for the row,
the PATCHBYTE line, and the PREREG read-out. Writes verify/wsbc4trk2_20261029/heap_pages.tsv.
"""
import re, sys
from pathlib import Path

DATA = Path("D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data")
OUT = Path(__file__).resolve().parent / ("heap_pages%s.tsv" % ("" if __import__("os").environ.get("SHADOW_TAG", "heap") == "heap" else "_" + __import__("os").environ["SHADOW_TAG"]))
SMP = re.compile(r"^\[(\d+)\] fn=\S+ rva=(\w+) .*?ndiff=(\d+) (\w+)(.*)$")
KV = re.compile(r"(\w+)=o:(\d+),n:(\d+)")
SC = re.compile(r"\b(stack|noise|ovf|ret)=(\d+)")
COLS = ["rva", "readout", "n", "ndiff", "pages_o", "pages_n", "heap_o", "heap_n", "heap_pages_total",
        "image_pages_total", "stack_max", "noise_max", "ovf_max", "ret_max", "flag_lines", "patchbyte"]


def rng(v):
    return f"{min(v)}..{max(v)}" if v else ""


def one(rva, tag):
    log = DATA / f"{rva}{tag}" / "shadow_ab.log"
    r = {c: "" for c in COLS}; r["rva"] = "0x" + rva + tag
    if not log.exists():
        r["readout"] = "NO_LOG"; return r
    po, pn, ho, hn, nd = [], [], [], [], 0
    sc = {"stack": 0, "noise": 0, "ovf": 0, "ret": 0}
    flags, pb, htot, itot = 0, "", 0, 0
    for ln in log.read_text(encoding="utf-8", errors="replace").splitlines():
        if f"FUN_{rva} " in ln and re.search(r"NOISY|UNTRACKED|OTHER-EXC", ln):
            flags += 1
        if f"fn=FUN_{rva} PATCHBYTE" in ln:
            pb = ln.split("PATCHBYTE", 1)[1].strip()
        m = SMP.match(ln)
        if not m or m.group(2).lower() != rva:
            continue
        if int(m.group(3)) > 0 or m.group(4) != "OK":
            nd += 1
        kv = {k: (int(a), int(b)) for k, a, b in KV.findall(m.group(5))}
        p = kv.get("pages", (0, 0)); h = kv.get("heap", (0, 0))
        po.append(p[0]); pn.append(p[1]); ho.append(h[0]); hn.append(h[1])
        htot += max(h); itot += max(p) - max(h)
        for k, v in SC.findall(m.group(5)):
            sc[k] = max(sc[k], int(v))
    n = len(po)
    real = "A/B-IS-REAL" in pb and "installed=e9" in pb
    if n == 0:
        ro = "NO-SAMPLES"
    elif nd:
        ro = "DIVERGENT"
    elif flags or not real:
        ro = "NOT-CLEAN(flags/proof)"
    elif htot > 0:
        ro = "HEAP-CLEAN"
    elif itot > 0:
        ro = "CLEAN-IMAGE-ONLY"
    else:
        ro = "CLEAN-NO-PAGES"
    r.update(readout=ro, n=n, ndiff=nd, pages_o=rng(po), pages_n=rng(pn), heap_o=rng(ho), heap_n=rng(hn),
             heap_pages_total=htot, image_pages_total=itot, stack_max=sc["stack"], noise_max=sc["noise"],
             ovf_max=sc["ovf"], ret_max=sc["ret"], flag_lines=flags, patchbyte=pb)
    return r


def main():
    tag = "_" + __import__("os").environ.get("SHADOW_TAG", "heap")
    rows = [one(a.lower().replace("0x", "").zfill(8), tag) for a in sys.argv[1:]]
    OUT.write_text("\t".join(COLS) + "\n" + "".join("\t".join(str(r[c]) for c in COLS) + "\n" for r in rows),
                   encoding="utf-8")
    for i, r in enumerate(rows, 1):
        print(f"[{i}/{len(rows)}] {r['rva']} {r['readout']} n={r['n']} ndiff={r['ndiff']} pages={r['pages_o']}/{r['pages_n']} "
              f"heap={r['heap_o']}/{r['heap_n']} heap_total={r['heap_pages_total']} image_total={r['image_pages_total']}")


if __name__ == "__main__":
    main()
