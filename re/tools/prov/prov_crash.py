"""PROV-CRASH-12: read a detorig.py --crash-catch record (crash.jsonl) against the linked --prov blobs.

usage: py -3.12 re/tools/prov/prov_crash.py <run dir> [--bodies verify/provinsitu_20261111/bodies_all.json] [--ctx 12]

For the first 'fault' record (or the 'stall' samples): the faulting EIP and where it lies (a blob offset of a --prov
body, or module+offset), the memory operation and address, the registers, a disassembly of the blob around the fault
offset (rel32 targets from the link table named FUN_<va>), and the stack dwords that point into a blob or a module
(return-address candidates, listed raw: a stack dword pointing at code is not proof of a live frame).
"""
import argparse
import collections
import json
from pathlib import Path

import capstone

ap = argparse.ArgumentParser()
ap.add_argument("run")
ap.add_argument("--bodies", default="verify/provinsitu_20261111/bodies_all.json")
ap.add_argument("--ctx", type=int, default=12, help="instructions shown before the fault")
a = ap.parse_args()

run = Path(a.run)
recs = [json.loads(x) for x in (run / "crash.jsonl").read_text().splitlines() if x.strip()] if (run / "crash.jsonl").exists() else []
inst = json.loads((run / "prov_install.json").read_text())["installed"] if (run / "prov_install.json").exists() else []
bodies = {b["rva"].lower(): b for b in json.loads(Path(a.bodies).read_text())["bodies"]}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)


def blob_dis(rva, base, off_fault, before):
    b = bodies[rva.lower()]
    code = bytes.fromhex(b["code"])
    ext = {int(k): v for k, v in b["ext"].items()}
    rows = []
    for ins in md.disasm(code, base):
        o = ins.address - base
        txt = f"{ins.mnemonic} {ins.op_str}"
        for k in range(o, o + ins.size):
            if k in ext:
                txt += f"    ; -> FUN_{ext[k]:08x}"
        rows.append((o, txt, ins.bytes.hex()))
    i = next((k for k, r in enumerate(rows) if r[0] == off_fault), None)
    if i is None:
        i = next((k for k, r in enumerate(rows) if r[0] > off_fault), len(rows)) - 1
        print(f"  [fault offset 0x{off_fault:x} is not an instruction start in a linear sweep]")
    for o, t, h in rows[max(0, i - before): i + 4]:
        print(f"  {'>>' if o == off_fault else '  '} +0x{o:04x}  {h:<20} {t}")


faults = [r for r in recs if r.get("what") == "fault"]
stalls = [r for r in recs if r.get("what") == "stall"]
print(f"run {run.name}: {len(faults)} fault records, {len(stalls)} stall samples, {len(recs)} records")
if faults:
    f = faults[0]
    print(f"FAULT type={f['type']} address={f['address']} memory={f['memory']} n={f['n']} tid={f['tid']}")
    print(f"  eip={f['eip']} in={f['eip_in']}")
    print("  regs", " ".join(f"{k}={v}" for k, v in f["ctx"].items()))
    print("  code[eip-16..+16]", f["code"])
    w = f["eip_in"] or {}
    if "rva" in w and "off" in w:
        ib = next(x for x in inst if x["rva"] == w["rva"])
        blob_dis(w["rva"], int(ib["base"], 16), int(w["off"], 16), a.ctx)
    print("  stack dwords pointing into code (index from esp, value, where):")
    for i, s in enumerate(f["stack"]):
        if isinstance(s, list):
            print(f"    [esp+0x{i * 4:02x}] {s[0]} {s[1]}")
for k, f in enumerate(faults[1:], 1):
    print(f"FAULT#{k} type={f['type']} address={f['address']} memory={f['memory']} eip={f['eip']} in={f['eip_in']}")
if stalls:
    c = collections.Counter((s["eip"], json.dumps(s["eip_in"])) for s in stalls)
    print("STALL eip histogram (main thread):")
    for (e, w), n in c.most_common(12):
        print(f"  {n:3d}  {e}  {w}")
    s0 = stalls[0]
    print("  regs[0]", " ".join(f"{k}={v}" for k, v in s0["ctx"].items()))
    print("  stack[0] dwords pointing into code:")
    for i, s in enumerate(s0["stack"]):
        if isinstance(s, list):
            print(f"    [esp+0x{i * 4:02x}] {s[0]} {s[1]}")
    w = s0["eip_in"] or {}
    if "rva" in w and "off" in w:
        ib = next(x for x in inst if x["rva"] == w["rva"])
        blob_dis(w["rva"], int(ib["base"], 16), int(w["off"], 16), a.ctx)
