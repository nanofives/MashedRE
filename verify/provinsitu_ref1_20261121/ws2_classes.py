# PROV-INSITU-REF1-WS2: per-store detail of the escape classes. Re-decodes the wlog.bin of the given stepped runs with
# re/tools/prov/prov_writeset.py's own decoder (blob_bytes, classify, allowed_blocks; same classes) and keeps, per body:
#   caller-stack stores by offset from base_esp (esp at body entry, pointing at the return address)
#   other stores by target region: exe image outside .data (0x00400000..0x005ea000), prov stub/blob area of the run,
#   above 0x70000000 (system modules), else the rest (heap / other allocations; no memory map is recorded)
#   image-masked stores by 1 KiB block
#   py -3.12 ws2_classes.py --out ESC_DETAIL.json <run dir> [<run dir> ...]
import argparse, collections, json, pathlib, struct, sys

H = pathlib.Path(__file__).resolve().parent
WT = H.parents[1]
sys.path.insert(0, str(WT / "re" / "tools" / "prov"))
sys.path.insert(0, str(WT / "re" / "frida"))
import prov_writeset as pw  # noqa: E402

R = "D:/MashedWT/RO-REF-NAME_data"


def main():
    import capstone
    from capstone import x86
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--bodies", default="D:/MashedWT/PROV-INSITU-REF1_data/bodies309.json")
    ap.add_argument("runs", nargs="+")
    a = ap.parse_args()
    allowed = pw.allowed_blocks([f"{R}/ref1", f"{R}/ref2"])
    bodies = {b["rva"].lower(): b for b in json.loads(pathlib.Path(a.bodies).read_text())["bodies"] if b["status"] == "OK"}
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    res = {}
    for run in a.runs:
        run = pathlib.Path(run)
        inst = json.loads((run / "prov_install.json").read_text())["installed"]
        spans = []
        stub_lo, stub_hi = 1 << 32, 0
        for x in inst:
            if "base" not in x:
                continue
            base = int(x["base"], 16)
            stub_lo = min(stub_lo, int(x["stub"], 16)); stub_hi = max(stub_hi, base + x["size"])
            b = bodies.get(x["rva"].lower())
            if b is None:
                continue
            spans.append((base - 48, base + x["size"], base, x["rva"].lower(), pw.blob_bytes(b, base)))
        raw = (run / "wlog.bin").read_bytes() if (run / "wlog.bin").exists() else b""
        cache = {}
        for i in range(len(raw) // 40):
            rec = struct.unpack_from("<10I", raw, i * 40)
            eip = rec[0]
            sp = next((s for s in spans if s[0] <= eip < s[1]), None)
            if sp is None:
                continue
            rv = sp[3]
            acc = res.setdefault(rv, {"run": run.name, "cs": collections.Counter(), "other": collections.Counter(),
                                      "masked": collections.Counter(), "blob": collections.Counter(), "cs_sites": collections.Counter()})
            base_esp = rec[9] & ~3
            ins = cache.get((rv, eip))
            if ins is None:
                off = eip - sp[2]
                got = list(md.disasm(sp[4][off:off + 16], eip, 1)) if off >= 0 else []
                ins = got[0] if got else False
                cache[(rv, eip)] = ins
            if not ins:
                continue
            regs = {nm: rec[ix] for nm, ix in pw.REGS.items()}
            rep = any(p in (0xF2, 0xF3) for p in ins.prefix if p)
            for op in ins.operands:
                if op.type != x86.X86_OP_MEM or not (op.access & capstone.CS_AC_WRITE) or op.mem.segment:
                    continue
                ea = op.mem.disp
                if op.mem.base:
                    ea += regs.get(ins.reg_name(op.mem.base), 0)
                if op.mem.index:
                    ea += regs.get(ins.reg_name(op.mem.index), 0) * op.mem.scale
                ea &= pw.M32
                size = op.size or 4
                if rep and ins.mnemonic.startswith(("movs", "stos")):
                    size = max(size * regs["ecx"], 1)
                cl = pw.classify(ea, size, base_esp, sp[0], sp[1], allowed)
                if cl == "caller-stack":
                    acc["cs"][ea - base_esp] += 1
                    acc["cs_sites"][f"{eip - sp[2]:#x} {ins.mnemonic} {ins.op_str}"] += 1
                elif cl == "other":
                    reg = ("exe-code-rdata" if 0x00400000 <= ea < pw.DATA_LO else
                           "prov-area" if stub_lo <= ea < stub_hi else
                           "high>=0x70000000" if ea >= 0x70000000 else "heap/other")
                    acc["other"][reg] += 1
                elif cl == "image-masked":
                    acc["masked"][f"{ea & ~1023:#010x}"] += 1
                elif cl == "blob":
                    acc["blob"][f"{ea - sp[2]:#x}"] += 1
    out = {rv: {"run": v["run"], "caller_stack_offsets": {f"{k:+#x}": c for k, c in sorted(v["cs"].items())},
                "caller_stack_sites": dict(v["cs_sites"].most_common(8)),
                "other_regions": dict(v["other"]), "masked_blocks": dict(v["masked"]), "blob_offsets": dict(v["blob"])}
           for rv, v in res.items()}
    pathlib.Path(a.out).write_text(json.dumps(out, indent=1))
    print(len(out), "bodies")


if __name__ == "__main__":
    main()
