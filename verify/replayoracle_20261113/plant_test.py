"""RO-CTL-68BC0C proof on existing dumps. py -3.12 -I plant_test.py
1. ctl (wss_ctl_g04) vs r1/r2: block 0x0068bc00 must not be in the divergence list (before this change it was the only one).
2. copies of ctl with one dword outside the pad words changed in every ckpt (0x0068bc10 0 -> 0x5a5a5a5a, 0x0068bc04 1.0f
   xored): block must be reported again.
3. copy of ctl with the two pad words changed (0x0068bc0c, 0x0068bc1c): block must stay masked.
4. copy of r1 used as the run against r1/r2 with its pad words zeroed (the ctl state): block masked, no divergence.
The ticks.bin of a copy is ctl's own, so the block hash still differs from r1 at every tick (0x0068bc0c is 0 vs 4) and the
checkpoint check is what decides."""
import shutil, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
import prov_compare as pc
I = "D:/MashedWT/PROV-INSITU_data/"; W = "D:/MashedWT/PROV-WRITESET-SCENARIO_data/"
OUT = Path("D:/MashedWT/RO-CTL-68BC0C_data/plant")
OUT.mkdir(parents=True, exist_ok=True)


def run(d):
    r = pc.verdict(d, I + "r1", I + "r2", [(I + "r1", I + "r2")])
    blocks = [b[0] for b in (r["first"] or {}).get("blocks", [])]
    return "0x68bc00" in blocks, r["verdict"], blocks, [n for n in r["notes"] if "variant" in n]


def plant(name, src, edits):
    d = OUT / name
    if d.exists():
        shutil.rmtree(d)
    shutil.copytree(src, d)
    for f in d.glob("ckpt*.data.bin"):
        b = bytearray(f.read_bytes())
        for va, fn in edits:
            o = va - pc.DATA_LO
            b[o:o + 4] = struct.pack("<I", fn(struct.unpack_from("<I", b, o)[0]))
        f.write_bytes(b)
    return d


x = lambda v: v ^ 0x5a5a5a5a
res = {}
res["ctl_vs_r1r2"] = run(W + "wss_ctl_g04")
res["plant_outside_0x68bc10"] = run(plant("out_bc10", W + "wss_ctl_g04", [(0x0068bc10, x)]))
res["plant_outside_0x68bc04"] = run(plant("out_bc04", W + "wss_ctl_g04", [(0x0068bc04, x)]))
res["plant_inside_0x68bc0c_0x68bc1c"] = run(plant("in_pads", W + "wss_ctl_g04", [(0x0068bc0c, x), (0x0068bc1c, x)]))
res["r1_pads_zeroed_vs_r1r2"] = run(plant("r1_zero", I + "r1", [(0x0068bc0c, lambda v: 0), (0x0068bc1c, lambda v: 0)]))
for k, (in_list, verdict, blocks, notes) in res.items():
    print(f"{k}: verdict={verdict} 0x68bc00 reported={in_list} blocks={blocks}\n   {notes}")
exp = {"ctl_vs_r1r2": False, "plant_outside_0x68bc10": True, "plant_outside_0x68bc04": True,
       "plant_inside_0x68bc0c_0x68bc1c": False, "r1_pads_zeroed_vs_r1r2": False}
ok = all(res[k][0] == v for k, v in exp.items())
print("PLANT TEST", "PASS" if ok else "FAIL")
sys.exit(0 if ok else 1)
