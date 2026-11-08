"""PORT-RWP-01: absolute-address references to RenderWare-Physics-3.7 RVAs inside the exe's .text.

backfill_port.py's graph only sees rel32 calls onto exe symbols and base-relocated pointers. A call
written as reinterpret_cast<fn>(0x0057c210u)(...) is neither: the literal targets the ORIGINAL's .text,
which the standalone does not map (only 6 RVA thunks exist), so such a call faults if it executes.
This scan lists every dword literal in the exe .text equal to an RWP row's RVA, the exe symbol that
holds it, and whether WinMain reaches that symbol.
  py -3.12 verify/portrwp_20261108/abs_calls.py  -> ABS_CALLS.tsv
"""
import bisect, csv, pathlib, struct, sys
HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import backfill_port as P  # noqa: E402

b = ROOT / "mashedmod" / "build"
ents, names_at, edges, nrel, (t_lo, t_hi) = P.build_graph(b / "mashed_re.exe", b / "mashed_re.map")
prev, _ = P.walk(ents, names_at, edges)
d, ib, secs, _r, _s = P.load_image(b / "mashed_re.exe")
text = next(s for s in secs if s[0] == ".text")
starts = sorted({va for va, *_ in ents})
rwp = {int(r["rva"], 16): r for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8"))
       if "RenderWare-Physics-3.7" in r["subsystem"]}
tb = d[text[3]:text[3] + text[4]]
base = ib + text[1]
hits = {}
for i in range(len(tb) - 4):
    v = struct.unpack_from("<I", tb, i)[0]
    if v in rwp:
        j = bisect.bisect_right(starts, base + i) - 1
        o = starts[j]
        hits.setdefault((v, o), 0)
        hits[(v, o)] += 1
with open(HERE / "ABS_CALLS.tsv", "w", encoding="utf-8", newline="") as f:
    f.write("rwp_rva\trwp_name\tconfidence\tholder_symbol\tholder_reached_by_WinMain\n")
    for (v, o), n in sorted(hits.items()):
        hn = P.base_name(names_at[o][0]) if o in names_at else hex(o)
        f.write(f"{v:08x}\t{rwp[v]['name']}\t{rwp[v]['confidence']}\t{hn}\t{'yes' if o in prev else 'no'}\n")
reached = [(v, o) for (v, o) in hits if o in prev]
print(f"literal refs {len(hits)}, held by WinMain-reached symbols: {len(reached)}")
for v, o in reached:
    print(f"  {v:08x} in {P.base_name(names_at[o][0])}")
