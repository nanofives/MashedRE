"""Read-out table for an arm S run (D4-WSB-PORT-BROAD-2 / NARROW-TAIL): one markdown row per RVA from the committed
shadow_summary.tsv, plus heap pages o/n per sample counted from the row's own sample lines in the raw shadow_ab.log
(lines `[k] fn=<name> rva=<rva> ...`; nested lines of other functions are excluded), the PATCHBYTE proof line and
the raw log SHA-256.

  py -3.12 verify/d4wsb_portbroad2_20261101/readout.py <summary.tsv> <data_dir>
"""
import collections, hashlib, re, sys
from pathlib import Path


def main():
    summ, data = Path(sys.argv[1]), Path(sys.argv[2])
    rows = [ln.split("\t") for ln in summ.read_text(encoding="utf-8").splitlines()]
    hdr, rows = rows[0], rows[1:]
    ix = {k: i for i, k in enumerate(hdr)}
    print("| row | verdict | n | ndiff | uninstalled | heap pages o/n per sample | log SHA-256 |")
    print("|---|---|---|---|---|---|---|")
    for r in rows:
        rva = r[ix["rva"]].replace("0x", "")
        log = data / rva / "shadow_ab.log"
        heap, unin, sha = collections.Counter(), "", ""
        if log.exists():
            b = log.read_bytes(); sha = hashlib.sha256(b).hexdigest()
            for ln in b.decode("utf-8", "replace").splitlines():
                if re.match(r"^\[\d+\] fn=\S+ rva=" + rva + " ", ln):
                    m = re.search(r"heap=o:(\d+),n:(\d+)", ln)
                    heap[f"{m.group(1)}/{m.group(2)}" if m else "-"] += 1
                m = re.search(r"PATCHBYTE installed=(\w+) uninstalled=(\w+)", ln)
                if m and rva in ln.lower() or (m and not unin):
                    unin = m.group(2) if m else unin
        hp = ", ".join(f"{k} x{v}" for k, v in sorted(heap.items(), key=lambda kv: -kv[1])) or "-"
        print(f"| `0x{rva}` | {r[ix['verdict']]} | {r[ix['n']]} | {r[ix['ndiff']]} | `{unin}` | {hp} | `{sha[:16]}` |")


if __name__ == "__main__":
    sys.exit(main())
