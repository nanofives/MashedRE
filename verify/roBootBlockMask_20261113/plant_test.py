"""RO-BOOT-BLOCK-MASK proof on existing dumps. py -3.12 -I plant_test.py
1. ctl (wss_ctl_g04) vs r1/r2: block 0x00773c00 must not be in the divergence list (old code listed it).
2. copy of ctl with one dword outside the boot words changed in every ckpt (0x00773c10): block must be reported again.
3. copy of ctl with a dword INSIDE the boot words changed (0x00773c44, 0x00773d60): block must stay masked."""
import shutil, struct, sys, json
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
import prov_compare as pc
I = "D:/MashedWT/PROV-INSITU_data/"; W = "D:/MashedWT/PROV-WRITESET-SCENARIO_data/"
OUT = Path("D:/MashedWT/RO-BOOT-BLOCK-MASK_data")

def run(d):
    r = pc.verdict(d, I + "r1", I + "r2", [(I + "r1", I + "r2")])
    blocks = [b[0] for b in (r["first"] or {}).get("blocks", [])]
    return "0x773c00" in blocks, blocks, [n for n in r["notes"] if "boot-variant" in n]

def plant(name, vas):
    d = OUT / name
    if d.exists(): shutil.rmtree(d)
    shutil.copytree(W + "wss_ctl_g04", d)
    for f in d.glob("ckpt*.data.bin"):
        b = bytearray(f.read_bytes())
        for va in vas:
            o = va - pc.DATA_LO
            b[o:o + 4] = struct.pack("<I", struct.unpack_from("<I", b, o)[0] ^ 0x5a5a5a5a)
        f.write_bytes(b)
    return d

res = {}
res["ctl_vs_r1r2"] = run(W + "wss_ctl_g04")
res["plant_outside_0x773c10"] = run(plant("plant_out", [0x773c10]))
res["plant_inside_0x773c44_0x773c80_0x773d60"] = run(plant("plant_in", [0x773c44, 0x773c80, 0x773d60]))
for k, (in_list, blocks, notes) in res.items():
    print(f"{k}: 0x773c00 reported={in_list} blocks={blocks}\n   {notes}")
exp = {"ctl_vs_r1r2": False, "plant_outside_0x773c10": True, "plant_inside_0x773c44_0x773c80_0x773d60": False}
ok = all(res[k][0] == v for k, v in exp.items())
print("PLANT TEST", "PASS" if ok else "FAIL")
sys.exit(0 if ok else 1)
