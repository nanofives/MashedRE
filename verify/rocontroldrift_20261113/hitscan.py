"""RO-CONTROL-DRIFT: do any of the c4=frida-replay rows store into the drift addresses?

Targets: the compare block 0x00773c00..0x00774000 (the block the control wss_ctl_g04 differs in from r1 / r2 on every
tick) and the dword 0x006147d8 (MASK:os, the raw word the PROV-WRITESET-SCENARIO RESULT named).

Two legs per row:
  static   the DDK body's absolute store list (bodies_all.json writes.abs_in / abs_out, capstone over the linked code)
  dynamic  every single-step write log (wlog.bin) of PROV-WRITESET-114 and PROV-WRITESET-SCENARIO, decoded exactly as
           re/tools/prov/prov_writeset.py does (same blob relink, same effective-address rule, rep movs/stos = range),
           and every store whose [ea, ea+size) overlaps a target is counted per body.

    py -3.12 verify/rocontroldrift_20261113/hitscan.py --out verify/rocontroldrift_20261113/HITSCAN.tsv
"""
import argparse, collections, csv, json, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools" / "prov"))
from prov_writeset import blob_bytes, REGS, M32  # noqa: E402

TARGETS = [("block_773c00", 0x00773C00, 0x00774000), ("dword_6147d8", 0x006147D8, 0x006147DC)]
RUN_DIRS = ["D:/MashedWT/PROV-WRITESET-114_data", "D:/MashedWT/PROV-WRITESET-SCENARIO_data"]


def hit(ea, size):
    return [nm for nm, lo, hi in TARGETS if ea < hi and ea + max(size, 1) > lo]


def main():
    import capstone
    from capstone import x86
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--runs-out", default="")
    a = ap.parse_args()
    rows = [r for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8")) if "c4=frida-replay" in r["notes"]]
    want = {"0x" + r["rva"].lower().removeprefix("0x"): r for r in rows}
    bodies = {b["rva"].lower(): b for b in json.loads((ROOT / "verify/provinsitu_20261111/bodies_all.json").read_text())["bodies"]}
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    dyn = collections.defaultdict(collections.Counter)   # rva -> counters
    dyn_hits = collections.defaultdict(collections.Counter)  # rva -> "target:ea" -> n
    runlog = []
    for d in RUN_DIRS:
        for run in sorted(pathlib.Path(d).iterdir()):
            wl = run / "wlog.bin"
            if not run.is_dir() or not wl.exists() or not (run / "prov_install.json").exists():
                continue
            inst = json.loads((run / "prov_install.json").read_text())["installed"]
            spans = []
            for x in inst:
                if "base" not in x or x["rva"].lower() not in bodies:
                    continue
                base = int(x["base"], 16)
                spans.append((base - 48, base + x["size"], base, x["rva"].lower(), blob_bytes(bodies[x["rva"].lower()], base)))
            spans.sort()
            raw = wl.read_bytes()
            n = len(raw) // 40
            cache = {}
            nst = nh = 0
            stepped = collections.Counter()
            for i in range(n):
                rec = struct.unpack_from("<10I", raw, i * 40)
                eip = rec[0]
                sp = next((s for s in spans if s[0] <= eip < s[1]), None)
                if sp is None:
                    continue
                rv = sp[3]
                if rec[9] & 1:
                    stepped[rv] += 1
                key = (rv, eip)
                ops = cache.get(key)
                if ops is None:
                    off = eip - sp[2]
                    got = list(md.disasm(sp[4][off:off + 16], eip, 1)) if off >= 0 else []
                    ops = []
                    if got:
                        ins = got[0]
                        rep = any(p in (0xF2, 0xF3) for p in ins.prefix if p) and ins.mnemonic.startswith(("movs", "stos"))
                        for op in ins.operands:
                            if op.type == x86.X86_OP_MEM and (op.access & capstone.CS_AC_WRITE) and not op.mem.segment:
                                ops.append((op.mem.disp, ins.reg_name(op.mem.base) if op.mem.base else None,
                                            ins.reg_name(op.mem.index) if op.mem.index else None, op.mem.scale, op.size or 4, rep))
                    cache[key] = ops
                for disp, br, ir, sc, size, rep in ops:
                    ea = disp + (rec[REGS[br]] if br in REGS else 0) + (rec[REGS[ir]] * sc if ir in REGS else 0)
                    ea &= M32
                    if rep:
                        size = max(size * rec[REGS["ecx"]], 1)
                    nst += 1
                    dyn[rv]["stores"] += 1
                    for t in hit(ea, size):
                        nh += 1
                        dyn_hits[rv][f"{t}:{ea:#010x}"] += 1
            for rv, c in stepped.items():
                dyn[rv]["calls_stepped"] += c
                dyn[rv]["runs"] += 1
            runlog.append([str(run).replace("\\", "/"), n, nst, nh])
            print(f"{run.name}: records {n} stores {nst} target hits {nh}", flush=True)
    out = []
    for rv, r in sorted(want.items()):
        b = bodies.get(rv)
        st = "no-ddk-body"
        sh = []
        nidx = 0
        if b and b.get("status") == "OK":
            addrs = [x.split("@")[0] for x in b["writes"].get("abs_in", []) + b["writes"].get("abs_out", [])]
            # "0x008a9570[ebp]" = absolute base + register index (range not static): the dynamic leg decides it;
            # flagged here only when the base itself lies in a target
            sh = sorted({f"{t}:{x}" for x in addrs for t in hit(int(x.split("[")[0], 16), 4)})
            nidx = sum("[" in x for x in addrs)
            st = f"abs_stores {len(addrs)} indexed_abs {nidx} indirect {len(b['writes'].get('indirect', []))}"
        elif b:
            st = f"body-status {b.get('status')}"
        dc = dyn.get(rv, {})
        dh = dyn_hits.get(rv, {})
        ok = b and b.get("status") == "OK"
        open_static = ok and (nidx or b["writes"].get("indirect"))
        if sh or dh:
            verdict = "HIT"
        elif dc.get("calls_stepped"):
            verdict = "clear-dynamic"          # stepped calls observed, no store into a target
        elif ok and not open_static:
            verdict = "clear-static"           # every store has a static absolute address, none in a target
        elif ok:
            verdict = "clear-static-abs-only"  # absolute stores clear; register-based stores not observed
        else:
            verdict = "no-evidence"
        out.append([rv, r["name"], r["confidence"], st, ";".join(sh) or "-", dc.get("runs", 0), dc.get("calls_stepped", 0),
                    dc.get("stores", 0), ";".join(f"{k}x{v}" for k, v in sorted(dh.items())) or "-", verdict])
    hdr = ["rva", "name", "conf", "static_leg", "static_hits", "wlog_runs", "calls_stepped", "stores_decoded", "dynamic_hits", "verdict"]
    pathlib.Path(a.out).write_text("\t".join(hdr) + "\n" + "\n".join("\t".join(str(x) for x in o) for o in out) + "\n")
    if a.runs_out:
        pathlib.Path(a.runs_out).write_text("run\trecords\tstores\ttarget_hits\n" + "\n".join("\t".join(str(x) for x in r) for r in runlog) + "\n")
    print(collections.Counter(o[-1] for o in out))


if __name__ == "__main__":
    main()
