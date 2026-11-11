"""Sensitivity of the seeded cross-check (EQUIV-ADMIT-1): can a seeded run see a difference in these bodies?

For each RVA, mutate the ORIGINAL body inside the Unicorn picture only (never on disk): one displacement + 4 or one
immediate + 1, at up to 8 instructions, and run the seeded check (16 runs) of our body against the mutant. A mutant
the seeded runs report as DIFFERS shows the seeds reach and observe that instruction. A mutant reported SAME is
either unreached by the seeds or unobservable (for example a write later overwritten); it is listed, not hidden.

    py -3.12 verify/equivadmit_20261111/seeded_sensitivity.py [--out verify/equivadmit_20261111/seeded_sensitivity.tsv]
"""
import argparse
import pathlib
import random
import sys

import capstone

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re/tools/match"))
import equiv_dyn as d  # noqa: E402

ROWS = [("re/match/s14/00485b30.c", 0x00485B30), ("re/match/s15/004938e0.c", 0x004938E0),
        ("re/match/s18/004b61c0.c", 0x004B61C0), ("re/match/s25/005a8890.c", 0x005A8890),
        ("re/match/s25/005aaac0.c", 0x005AAAC0), ("re/match/s25/005aab00.c", 0x005AAB00),
        ("re/match/s25/005ab410.c", 0x005AB410), ("re/match/s26/005ad320.c", 0x005AD320),
        ("re/match/s26/005ae920.c", 0x005AE920)]
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True


def mutants(img, lo, hi):
    code = bytes(img.bytes(lo, hi - lo))
    out = []
    for ins in MD.disasm(code, lo):
        off = ins.disp_offset if getattr(ins, "disp_offset", 0) else 0
        if off and ins.disp_size in (1, 4):
            out.append((ins.address, ins.address + off, ins.disp_size, 4, f"{ins.mnemonic} {ins.op_str} disp+4"))
        ioff = ins.imm_offset if getattr(ins, "imm_offset", 0) else 0
        if ioff and ins.imm_size in (1, 4) and ins.mnemonic not in ("call", "jmp") and not ins.mnemonic.startswith("j") \
                and ins.mnemonic not in ("ret", "push"):
            out.append((ins.address, ins.address + ioff, ins.imm_size, 1, f"{ins.mnemonic} {ins.op_str} imm+1"))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="")
    ap.add_argument("--per", type=int, default=8)
    a = ap.parse_args()
    d.SEEDED = True
    rows = ["rva\tins_addr\tmutation\tsame\tinconclusive\tdiffers\tseen"]
    tot = det = 0
    cov = ["rva	same	inconclusive	differs	ins_hit	ins_body"]
    for src, rva in ROWS:
        layout, bA, bB = d.bodies_for(str(ROOT / src), rva)
        img = d.equiv.image()
        # instruction coverage of the original body over the 16 seeded runs (the unmutated check)
        hit = set()
        orig_build = d.Layout.build

        def build_cov(self, uc, lo=bB.lo, hi=bB.hi):
            orig_build(self, uc)
            uc.hook_add(d.UC_HOOK_CODE, lambda uc_, addr, size, _: hit.add(addr), None, lo, hi - 1)

        d.Layout.build = build_cov
        try:
            tally0, _ = d.check(layout, bA, bB, 16, seed=1, rva=rva)
        finally:
            d.Layout.build = orig_build
        n_ins = sum(1 for _ in MD.disasm(bytes(img.bytes(bB.lo, bB.hi - bB.lo)), bB.lo))
        print(f"0x{rva:08x} check same={tally0['same']} inc={tally0['inconclusive']} diff={tally0['differs']} "
              f"coverage {len(hit)}/{n_ins} instructions of the original body", flush=True)
        cov.append(f"0x{rva:08x}	{tally0['same']}	{tally0['inconclusive']}	{tally0['differs']}	{len(hit)}	{n_ins}")
        ms = mutants(img, bB.lo, bB.hi)
        random.Random(rva).shuffle(ms)
        for at, field, size, delta, what in ms[:a.per]:
            orig_build = d.Layout.build

            def build(self, uc, field=field, size=size, delta=delta):
                orig_build(self, uc)
                v = int.from_bytes(bytes(uc.mem_read(field, size)), "little")
                uc.mem_write(field, ((v + delta) & ((1 << (8 * size)) - 1)).to_bytes(size, "little"))

            d.Layout.build = build
            try:
                tally, first = d.check(layout, bA, bB, 16, seed=1, rva=rva)
            finally:
                d.Layout.build = orig_build
            seen = tally["differs"] > 0
            tot += 1
            det += seen
            print(f"[{tot}] 0x{rva:08x} @0x{at:08x} {what}: same={tally['same']} inc={tally['inconclusive']} "
                  f"diff={tally['differs']} {'SEEN' if seen else 'not seen'}", flush=True)
            rows.append(f"0x{rva:08x}\t0x{at:08x}\t{what}\t{tally['same']}\t{tally['inconclusive']}\t{tally['differs']}\t{int(seen)}")
    print(f"mutants seen by the seeded runs: {det}/{tot}")
    if a.out:
        pathlib.Path(a.out).write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
