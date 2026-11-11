"""EQUIV-ADMIT-1: write re/match/ADMITTED.tsv, the L-126 admission record, from this lane's evidence.

    py -3.12 verify/equivadmit_20261111/gen_admitted.py

Inputs (all in verify/equivadmit_20261111 unless named):
  admit33_in.tsv      the 33 rows of verify/equivcheck_20261111/measure_v4.tsv released with no per-function decision
                      (RESULT_EQUIV-CHECK.md section 7): rules, local callees, caller-scan registers
  equiv33.log         equiv.py re-run at the tool commit below, one verdict line per row
  dyn33_default.tsv   Unicorn cross-check, 16 uniform-random runs (equiv_dyn.py, this lane's harness)
  dyn33_seeded.tsv    the same with --seeded (equiv_seeds.py builders where a seed exists, sparse pages elsewhere)
A row is written only when the re-run verdict equals the measured one, no cross-check run differs, and the better of
the two cross-checks has 8 or more conclusive runs (L-126 / RESULT_EQUIV-CHECK section 8). Otherwise the script stops.
"""
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
D = ROOT / "verify" / "equivadmit_20261111"
OUT = ROOT / "re" / "match" / "ADMITTED.tsv"

# the source the port binds when it differs from the measured one (both proven, see the RESULT)
PORT_SOURCE = {0x00475A00: "re/match/s12/00475a00.c"}
# where the body runs on the MASHED_ORIG_TOP path today (BINDINGS.tsv); every other row has no compiled caller yet
BINDING = {
    0x00475A00: "bound: Match/PortTop_00475a00.cpp Orig_00475a00, called by the matched 0x0040bbb0 (BINDINGS near-match, "
                "PORT-OT-RENDER-2 under L-115; admitted here under L-126)",
    0x00555360: "not bound here: row NEARMATCH-555360 (4503d7205) binds it in 0x00552b60, held behind L-129",
}


def git(*a):
    return subprocess.run(["git", *a], cwd=ROOT, capture_output=True, text=True, check=True).stdout.strip()


def tsv(p):
    lines = p.read_text(encoding="utf-8").splitlines()
    head = lines[0].split("\t")
    return [dict(zip(head, ln.split("\t"))) for ln in lines[1:] if ln.strip()]


def main():
    tool = git("log", "-1", "--format=%h", "--", "re/tools/match/equiv.py")
    rows = {int(r["rva"], 16): r for r in tsv(D / "admit33_in.tsv")}
    rerun = {}
    for ln in (D / "equiv33.log").read_text(encoding="utf-8").splitlines():
        m = re.search(r":: (0x[0-9a-f]{8})\t(\S+)\t\[([^\]]*)\](.*)$", ln.strip())
        if m:
            rerun[int(m.group(1), 16)] = (m.group(2), m.group(3), m.group(4).strip())
    dd = {int(r["rva"], 16): r for r in tsv(D / "dyn33_default.tsv")}
    ds = {int(r["rva"], 16): r for r in tsv(D / "dyn33_seeded.tsv")}
    head = ["rva", "name", "source", "flags", "source_commit", "tool_commit", "verdict", "classes", "rules",
            "caller_scan", "local_callees", "xcheck_default", "xcheck_seeded", "port_binding", "evidence"]
    out = ["\t".join(head)]
    bad = []
    for rva in sorted(rows):
        r = rows[rva]
        v, cls, tail = rerun.get(rva, ("?", "", ""))
        want = r["verdict"] + (f"({r['regs'].upper()})" if r["regs"] else "")
        if v != want or cls != r["classes"]:
            bad.append(f"0x{rva:08x}: re-run {v} [{cls}] != measured {want} [{r['classes']}]")
        src = PORT_SOURCE.get(rva, r["source"])
        text = (ROOT / src).read_text(encoding="utf-8", errors="replace")
        fm = re.search(r"^// FLAGS:\s*(.+)$", text, re.M)
        flags = fm.group(1).strip() if fm else "(match.py default)"
        nm = re.search(r"// MATCH: 0x%08x (\S+)" % rva, text)
        name = nm.group(1) if nm else f"FUN_{rva:08x}"
        tallies = []
        for t in (dd.get(rva), ds.get(rva)):
            if t is None:
                tallies.append(None)
                continue
            tallies.append((int(t["same"]), int(t["inconclusive"]), int(t["differs"])))
        if any(t and t[2] for t in tallies):
            bad.append(f"0x{rva:08x}: cross-check differs {tallies}")
        if max((t[0] for t in tallies if t), default=0) < 8:
            bad.append(f"0x{rva:08x}: fewer than 8 conclusive runs {tallies}")
        scan = tail.replace("callers ", "callers=").strip() if r["regs"] else "n/a (plain EQUIV)"
        fmt = lambda t: "same=%d inc=%d diff=%d" % t if t else "-"
        out.append("\t".join([
            f"0x{rva:08x}", name, src, flags, git("log", "-1", "--format=%h", "--", src), tool,
            want, r["classes"], r["rules"] or "-", scan, r["local_callees"] or "-",
            fmt(tallies[0]), fmt(tallies[1]), BINDING.get(rva, "no compiled caller on the path yet (bind when the caller lands)"),
            "verify/equivadmit_20261111/RESULT_EQUIV-ADMIT-1.md"]))
    if bad:
        print("\n".join(bad))
        return 1
    OUT.write_text("# L-126 admission record (EQUIV-ADMIT-1, 2026-11-11). A row here may run on the port axis with no "
                   "per-function decision. Never C4 by this status (L-126, L-128). Columns: verify/equivadmit_20261111/"
                   "RESULT_EQUIV-ADMIT-1.md section 3.\n" + "\n".join(out) + "\n", encoding="utf-8")
    print(f"{len(out) - 1} rows -> {OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
