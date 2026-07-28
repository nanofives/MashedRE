# sweep_rega_rank.py — CLASS A stage 4: rank confirmed sites by our port's declared return.
#
# Stage 3 confirms an unhooked caller holds a live value in a register the original never
# writes. Whether that is a DEFECT depends on the register:
#
#   ECX / EDX  ->  a __cdecl C function never returns in these, so if the caller reads one
#                  after the CALL it is relying on preservation. Compiled C is free to
#                  clobber them. REAL RISK.
#
#   EAX        ->  if the original never writes EAX, the caller reading EAX after the call
#                  is relying on PASS-THROUGH (the "implicit EAX return" sub-class). That is
#                  only broken if OUR port returns void — a value-returning port puts
#                  something in EAX, and a pass-through port must be declared to return the
#                  incoming value. So: void return => RISK, non-void => needs eyeballing.
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
HERE = os.path.dirname(os.path.abspath(__file__))


def decl_return(path, sym):
    try:
        text = open(os.path.join(ROOT, path), encoding="utf-8", errors="replace").read()
    except Exception:
        return "?"
    m = re.search(r"([A-Za-z_][\w:<>,\s\*\.]*?)\s+__cdecl\s+" + re.escape(sym) + r"\s*\(", text)
    if not m:
        m = re.search(r"([A-Za-z_][\w:<>,\s\*\.]*?)\s+" + re.escape(sym) + r"\s*\(", text)
    if not m:
        return "?"
    t = m.group(1)
    t = re.sub(r'extern\s+"C"|__declspec\s*\([^)]*\)|static|inline', " ", t).strip()
    return re.sub(r"\s+", " ", t) or "?"


def main():
    data = json.load(open(os.path.join(HERE, "classa_findings.json")))
    conf = data["confirmed"]
    rows, seen = [], set()
    for c in conf:
        key = (c["rva"], c["reg"], c["caller"])
        if key in seen:
            continue
        seen.add(key)
        c["ret"] = decl_return(c["file"], c["sym"])
        rows.append(c)

    def rank(c):
        if c["reg"] in ("ecx", "edx"):
            return 0
        return 1 if c["ret"] == "void" else 2

    rows.sort(key=lambda c: (rank(c), c["rva"]))
    labels = {0: "RISK  (cdecl never returns in this reg -> caller relies on preservation)",
              1: "RISK  (implicit EAX pass-through, our port returns void)",
              2: "check (EAX, our port returns a value - verify it is the pass-through value)"}
    cur = None
    for c in rows:
        r = rank(c)
        if r != cur:
            cur = r
            print("\n== %s ==" % labels[r])
        print("  0x%08x %-28s %-4s ret=%-10s caller 0x%08x call@0x%08x  uses: %s"
              % (c["rva"], c["sym"], c["reg"].upper(), c["ret"], c["caller"],
                 c["call_site"], c["use"]))
        print("        %s" % c["file"])
    print("\ntotals: %d sites  (%d high-risk, %d void-EAX, %d to eyeball)"
          % (len(rows), sum(1 for c in rows if rank(c) == 0),
             sum(1 for c in rows if rank(c) == 1),
             sum(1 for c in rows if rank(c) == 2)))


if __name__ == "__main__":
    main()
