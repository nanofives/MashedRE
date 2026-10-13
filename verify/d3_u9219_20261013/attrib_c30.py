"""Report-only attribution (PREREG_U9219.md section 3, extended): on consumer calls where car v has refdist 0
(FUN_00414c30's mode-7 precondition FUN_00442cc0(v) == 0), tabulate what else the c30 trace carries: the held word
(read at 0x004144b3), the nested FUN_00414300 call count, the registry slot types within reach, ba8c, and the return.
usage: py -3.12 attrib_c30.py <c30.tsv> [<c30.tsv> ...]"""
import collections, math, struct, sys
f32 = lambda b, o: struct.unpack_from("<f", b, o)[0]
for path in sys.argv[1:]:
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
    ix = {k: i for i, k in enumerate(rows[0])}
    print("==", path)
    for v in (1, 2, 3):
        held = collections.Counter(); nsub = collections.Counter(); ret = collections.Counter(); near = collections.Counter()
        n = 0
        for r in rows[1:]:
            if int(r[ix["v"]]) != v or int(r[ix["refdist"]], 16) != 0: continue
            if r[ix["ba8c"]] not in ("6",): continue
            n += 1
            held[r[ix["held"]]] += 1; ret[r[ix["ret"]]] += 1
            subs = [x for x in r[ix["sub"]].split("|") if x.startswith("300:")] if r[ix["sub"]] else []
            nsub[min(len(subs), 5)] += 1
            car = bytes.fromhex(r[ix["cars958"]].split(";")[v]); cx, cz = f32(car, 0), f32(car, 8)
            k8 = 0
            for s in (r[ix["slots"]].split(";") if r[ix["slots"]] else []):
                b = bytes.fromhex(s); ty = struct.unpack_from("<I", b, 0x10)[0]
                if ty >= 0xff00: continue
                if math.hypot(f32(b, 0) - cx, f32(b, 8) - cz) < 8.0: k8 += 1
            near[min(k8, 4)] += 1
        print(f"  v{v}: refdist0 calls {n}; ret {dict(ret)}; held {dict(held.most_common(4))}; "
              f"nested 300 calls {dict(sorted(nsub.items()))}; non-car slots within XZ 8 {dict(sorted(near.items()))}")
