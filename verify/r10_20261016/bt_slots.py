"""r10: attribute the render-site chains of bt_capture.py to per-track hook-table slots (table 0x005f33f8).

    py -3.12 bt_slots.py

For each captured original draw with a "bt" chain: a chain element V lies in slot S of hook record R when
entry(R,S) <= V < the next higher slot entry of ANY record (the table's functions are contiguous per track, and a
shared stub 0x0045b350 is ignored). Reported per (track, texture): the slot(s) found, e.g. "warzone slot10 @0x00452da0".
Static byte read of original/MASHED.exe.unpatched (main checkout), nothing is executed.
"""
import glob, json, struct, importlib.util
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
spec = importlib.util.spec_from_file_location("ht", ROOT / "verify" / "rprops_20261016" / "dump_hooktable.py")
ht = importlib.util.module_from_spec(spec); spec.loader.exec_module(ht)
spec2 = importlib.util.spec_from_file_location("d3", ROOT / "re" / "tools" / "drawlist3d.py")
d3 = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(d3)
spec3 = importlib.util.spec_from_file_location("bs", HERE / "bt_summary.py")
bs = importlib.util.module_from_spec(spec3); spec3.loader.exec_module(bs)

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
TRACK_REC = {"Egypt": "egypt", "Neustein": "neustein", "Highway": "highway", "Sands": "sands", "SuperG": "superg",
             "Roundabout": "rouabout", "Storm": "storm", "Forest": "forest", "Dump": "dump", "Warzone": "warzone"}


def table():
    d, ib, secs = ht.load(EXE)
    cnt, = struct.unpack_from("<I", d, ht.va2off(ht.COUNT_VA, ib, secs))
    recs = {}
    for i in range(cnt):
        o = ht.va2off(ht.TABLE + i * ht.STRIDE, ib, secs)
        name = d[o:o + 0x10].split(b"\0")[0].decode("latin1")
        recs[name] = struct.unpack_from("<13I", d, o + 0x14)
    return recs


def main():
    recs = table()
    entries = sorted({e for s in recs.values() for e in s if e and e != 0x0045b350})
    def slot_of(rec, v):
        best = None
        for k, e in enumerate(recs[rec]):
            if not e or e == 0x0045b350:
                continue
            nxt = min(min((x for x in entries if x > e), default=0x600000), e + 0x300)
            if e <= v < nxt:
                best = (k, e, nxt)
        return best
    out = defaultdict(lambda: defaultdict(int))
    for f in sorted(glob.glob(str(HERE / "bt" / "[0-9][0-9]_*" / "orig_row_*.draws.json"))):
        p = Path(f); trk = p.parent.name.split("_", 1)[1]
        doc = json.loads(p.read_text())
        for e in d3.cut_frame(doc):
            if "bt" not in e:
                continue
            tex = d3.clean(e.get("tex"))
            ch = bs.clean_chain(e["bt"])
            hit = None
            for x in ch:
                s = slot_of(TRACK_REC[trk], int(x, 16))
                if s:
                    hit = f"{TRACK_REC[trk]} slot{s[0]} [0x{s[1]:08x},0x{s[2]:08x}) via {x}"
                    break
            out[(trk, tex)][hit or "-"] += 1
    for (trk, tex), h in sorted(out.items()):
        if tex in ("Advantage", "AdvantageSmall", "Glass"):
            continue
        print(f"{trk:<11}{tex:<18}" + " ; ".join(f"{k} x{v}" for k, v in sorted(h.items())))


if __name__ == "__main__":
    main()
