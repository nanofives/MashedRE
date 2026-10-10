#!/usr/bin/env python3
"""Mode 7 M4: summarize a scenario_launch.py --c30-probe capture (<out>.c30.tsv) or an exe trace in the same format.
Per car: calls, returns 0/1/2; registry cc at entry; car-slot position vs the car's record +0x958 at the call
(bitwise equal / not); nested call counts; refdist and held values on return-2 calls.
Usage: py -3.12 re/tools/c30_report.py <c30.tsv> [...]"""
import collections, struct, sys
f32 = lambda h: struct.unpack("<f", bytes.fromhex(h))[0]
for path in sys.argv[1:]:
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
    hdr, rows = rows[0], rows[1:]
    ix = {k: i for i, k in enumerate(hdr)}
    ret = collections.Counter(); cc = collections.Counter(); lag = collections.Counter(); sub = collections.Counter()
    r2 = []; frames = []
    for r in rows:
        v = int(r[ix["v"]]); rt = int(r[ix["ret"]]); ret[(v, rt)] += 1; cc[int(r[ix["cc"]])] += 1
        frames.append(int(r[ix["frame"]]))
        cars = r[ix["cars958"]].split(";")
        for s in (r[ix["slots"]].split(";") if r[ix["slots"]] else []):
            b = bytes.fromhex(s); ty = struct.unpack_from("<I", b, 0x10)[0]
            if 0xff00 <= ty < 0xff04:
                lag["equal" if b[0:12].hex() == cars[ty - 0xff00] else "differ"] += 1
        for x in (r[ix["sub"]].split("|") if r[ix["sub"]] else []): sub[x.split(":")[0]] += 1
        if rt == 2: r2.append((int(r[ix["frame"]]), v, r[ix["refdist"]], r[ix["held"]], r[ix["ba8c"]], r[ix["cand_out"]]))
    print(f"== {path}: {len(rows)} calls, frames {min(frames)}..{max(frames)}")
    print("  returns (v, ret):", dict(sorted(ret.items())))
    print("  cc at entry:", dict(cc.most_common(5)))
    print("  car slot pos vs record +0x958 at the call:", dict(lag))
    print("  nested:", dict(sub), " 41f030:48a630 nonzero rows:", sum(1 for r in rows if r[ix["c41f030_c48a630"]] != "0:0"))
    print(f"  return-2 calls: {len(r2)}; first/last frame {r2[0][0] if r2 else '-'} / {r2[-1][0] if r2 else '-'}")
    print("   refdist/held/ba8c on ret 2:", collections.Counter((x[2], x[3], x[4]) for x in r2).most_common(4))
