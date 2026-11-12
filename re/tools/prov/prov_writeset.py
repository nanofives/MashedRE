"""PROV-WRITESET-114: decode the single-step logs of `detorig.py --prov-wlog N` (detorig.js wsInstall) into the dynamic
write-set of each DDK body (L-38 / L-40 / L-54).

    py -3.12 re/tools/prov/prov_writeset.py --bodies verify/provinsitu_20261111/bodies_all.json \
        --run D:/MashedWT/PROV-WRITESET-114_data/ws_a [--run ...] --allowed-ref D:/MashedWT/PROV-INSITU_data/ref_a,D:/MashedWT/PROV-INSITU_data/ref_b \
        --rvas a,b,c --out verify/provinsitu_20261111/WRITESET_TABLE.tsv [--stores-out stores.tsv]

Each run dir holds wlog.bin (10-dword records: eip, eax, ecx, edx, ebx, esp, ebp, esi, edi, base_esp|first), the register
state BEFORE the instruction at eip, and prov_install.json (blob base of each body). The instruction at eip is decoded
from the linked blob (provlink code with the load-time fixups applied, as detorig.js provInstall applies them), the
effective address of every memory write is computed from the logged registers, and the target is classified against the
frame's base_esp (esp at body entry, pointing at the return address):

  frame         base_esp-4MB <= EA < base_esp            the body's own frame (and callee frames below it)
  caller-stack  base_esp <= EA < base_esp+4MB            return slot, arguments, the caller's frame
  image         inside 0x005ea000..0x00915008, outside the masked blocks (the per-tick hash compares it)
  image-masked  inside the image, inside a block two unmodified runs already disagree on (prov_compare allowed_blocks)
  blob          the body's own blob (literal pool, switch table)
  other         anything else: heap, other modules, the exe's code and .rdata

Implicit stack stores (push, call, enter) are the frame by construction and are not listed. A rep movs/stos is the
range [edi, edi + ecx*size). Verdict per body: covered = every observed store is frame or image (inside the compared
and unmasked image), escapes = at least one caller-stack / image-masked / blob / other store, not-run = no call was
stepped. Calls stepped are the first N per body (the stub's counter), reported per body.
"""
from __future__ import annotations
import argparse, collections, json, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
DATA_LO, DATA_HI = 0x005EA000, 0x00915008
STK = 0x400000
REGS = {"eax": 1, "ecx": 2, "edx": 3, "ebx": 4, "esp": 5, "ebp": 6, "esi": 7, "edi": 8}
M32 = 0xFFFFFFFF


def blob_bytes(b, base):
    """The body's blob as detorig.js provInstall leaves it in memory."""
    code = bytearray.fromhex(b["code"])
    for o in b["abs"]:
        struct.pack_into("<I", code, o, (struct.unpack_from("<I", code, o)[0] + base) & M32)
    for o, t in b["ext"].items():
        o = int(o)
        struct.pack_into("<i", code, o, ((t - (base + o + 4)) + 2**31) % 2**32 - 2**31)
    return bytes(code)


def allowed_blocks(refs):
    from prov_compare import verdict
    a, b = (pathlib.Path(x) for x in refs)
    out = set()
    for args in ((b, a, b), (a, b, a)):
        out |= set(int(x, 16) for x in verdict(*args)["allowed_blocks"])
    return out


def classify(ea, n, base_esp, lo, hi, allowed):
    if base_esp - STK <= ea < base_esp:
        return "frame"
    if base_esp <= ea < base_esp + STK:
        return "caller-stack"
    if lo <= ea < hi:
        return "blob"
    if DATA_LO <= ea < DATA_HI:
        # any 1 KiB block the store touches that is masked makes it masked
        for blk in range(ea & ~1023, ((ea + max(n, 1) - 1) & ~1023) + 1, 1024):
            if blk in allowed:
                return "image-masked"
        return "image"
    return "other"


def main():
    import capstone
    from capstone import x86
    ap = argparse.ArgumentParser()
    ap.add_argument("--bodies", required=True)
    ap.add_argument("--run", action="append", required=True)
    ap.add_argument("--allowed-ref", required=True)
    ap.add_argument("--rvas", default="")
    ap.add_argument("--out", required=True)
    ap.add_argument("--stores-out", default="")
    a = ap.parse_args()
    allowed = allowed_blocks(a.allowed_ref.split(","))
    bodies = {b["rva"].lower(): b for b in json.loads(pathlib.Path(a.bodies).read_text())["bodies"] if b["status"] == "OK"}
    want = [x.strip().lower() for x in a.rvas.split(",") if x.strip()] or None
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    res = {}      # rva -> accumulators
    steps_total = 0
    for run in a.run:
        run = pathlib.Path(run)
        inst = json.loads((run / "prov_install.json").read_text())["installed"]
        meta = json.loads((run / "meta.json").read_text()) if (run / "meta.json").exists() else {}
        spans = []
        for x in inst:
            if "base" not in x:
                continue
            base = int(x["base"], 16)
            b = bodies[x["rva"].lower()]
            spans.append((base - 48, base + x["size"], base, x["rva"].lower(), blob_bytes(b, base)))
        spans.sort()
        wl = run / "wlog.bin"
        raw = wl.read_bytes() if wl.exists() else b""
        n = len(raw) // 40
        cache = {}
        for r in spans:
            res.setdefault(r[3], collections.defaultdict(lambda: collections.Counter()))
            res[r[3]]["_run"][run.name] = 0
        for i in range(n):
            rec = struct.unpack_from("<10I", raw, i * 40)
            eip = rec[0]
            sp = next((s for s in spans if s[0] <= eip < s[1]), None)
            if sp is None:
                continue
            rv = sp[3]
            acc = res[rv]
            steps_total += 1
            acc["n"]["steps"] += 1
            if rec[9] & 1:
                acc["n"]["frames"] += 1
            base_esp = rec[9] & ~3
            key = (rv, eip)
            ins = cache.get(key)
            if ins is None:
                off = eip - sp[2]
                if off < 0:
                    ins = False
                else:
                    got = list(md.disasm(sp[4][off:off + 16], eip, 1))
                    ins = got[0] if got else False
                cache[key] = ins
            if not ins:
                acc["n"]["undecoded"] += 1
                continue
            regs = {nm: rec[ix] for nm, ix in REGS.items()}
            rep = any(p in (0xF2, 0xF3) for p in ins.prefix if p)
            for op in ins.operands:
                if op.type != x86.X86_OP_MEM or not (op.access & capstone.CS_AC_WRITE):
                    continue
                if op.mem.segment:
                    acc["n"]["segment-store"] += 1
                    continue
                ea = op.mem.disp
                if op.mem.base:
                    ea += regs.get(ins.reg_name(op.mem.base), 0)
                if op.mem.index:
                    ea += regs.get(ins.reg_name(op.mem.index), 0) * op.mem.scale
                ea &= M32
                size = op.size or 4
                if rep and ins.mnemonic.startswith(("movs", "stos")):
                    size *= regs["ecx"]
                    size = max(size, 1)
                cl = classify(ea, size, base_esp, sp[0], sp[1], allowed)
                acc["n"]["stores"] += 1
                acc["class"][cl] += 1
                acc["site"][(eip - sp[2] - 0, cl)] += 1
                if cl in ("image", "image-masked"):
                    acc["img"][f"{ea:#010x}"] += 1
                elif cl in ("other", "caller-stack", "blob"):
                    acc["oth"][f"{cl}:{ea:#010x}" if cl == "other" else f"{cl}:{ea - base_esp:+#x}"] += 1
    rows = []
    stores = []
    for rv in (want or sorted(res)):
        acc = res.get(rv)
        b = bodies.get(rv)
        st = b["writes"] if b else {}
        static = f"frame {st.get('frame', '-')} abs_in {len(st.get('abs_in', []))} abs_out {len(st.get('abs_out', []))} indirect {len(st.get('indirect', []))}" if st else "-"
        if not acc or not acc["n"]["frames"]:
            rows.append([rv, 0, 0, 0, "", "", "", "", "", "", "not-run", static, "no call stepped"])
            continue
        c = acc["class"]
        bad = c["caller-stack"] + c["image-masked"] + c["blob"] + c["other"]
        verdict = "escapes" if bad else "covered"
        outside = ", ".join(f"{k}={v}" for k, v in sorted(c.items()) if k in ("caller-stack", "image-masked", "blob", "other"))
        samp = ";".join(f"{k}x{v}" for k, v in sorted(acc["img"].items())[:12]) + ((";+%d more" % (len(acc["img"]) - 12)) if len(acc["img"]) > 12 else "")
        osamp = ";".join(f"{k}x{v}" for k, v in acc["oth"].most_common(6))
        rows.append([rv, acc["n"]["frames"], acc["n"]["steps"], acc["n"]["stores"], c["frame"], c["image"], c["image-masked"],
                     c["caller-stack"], c["blob"], c["other"], verdict, static,
                     f"undecoded {acc['n']['undecoded']}; image targets: {samp or '-'}; outside targets: {osamp or '-'}"])
        for (off, cl), v in sorted(acc["site"].items()):
            stores.append([rv, f"{off:#x}", cl, v])
    hdr = ["rva", "calls_stepped", "steps", "stores", "frame", "image", "image_masked", "caller_stack", "blob", "other", "verdict", "static_leg", "detail"]
    out = pathlib.Path(a.out)
    out.write_text("\t".join(hdr) + "\n" + "\n".join("\t".join(str(x) for x in r) for r in rows) + "\n")
    if a.stores_out:
        pathlib.Path(a.stores_out).write_text("rva\tsite_off\tclass\tcount\n" + "\n".join("\t".join(str(x) for x in r) for r in stores) + "\n")
    print(collections.Counter(r[10] for r in rows), "steps", steps_total)


if __name__ == "__main__":
    main()
