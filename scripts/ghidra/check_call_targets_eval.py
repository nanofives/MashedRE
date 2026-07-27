# check_call_targets_eval.py — verify every hardcoded call target in the reimpl
# sources is a real FUNCTION ENTRY in MASHED.exe.
#
# Motivation (2026-07-27): MenuMixed.cpp called 0x004a3220 as a "wprintf thunk".
# That address is mid-body of FUN_004a31f3, so the CALL decoded a garbage
# instruction and AV'd at runtime. A wrong address compiles and links fine — the
# only cheap way to catch the class is to check every target against the function
# database.
#
# READ-ONLY: opens no transaction, mutates nothing. Safe against a pool slot.
#
# Run via the MCP `ghidra_eval` tool against an open session:
#   _ns = {}
#   exec(open(r"<repo>/scripts/ghidra/check_call_targets_eval.py").read(), _ns)
#   _ns["run"](sessions, "<session_id>", r"<call_targets.json>", r"<out.json>")
#
# Verdicts:
#   ENTRY     addr is a function entry_point            -> fine
#   MID_BODY  addr is INSIDE a function but not entry   -> BUG (the 0x004a3220 class)
#   NO_FUNC   no function defined at/containing addr    -> suspect (data? undefined?)
import json


def run(sessions, session_id, targets_path, out_path):
    obj = sessions[session_id]
    prog = getattr(obj, "program", obj)
    fm = prog.getFunctionManager()
    af = prog.getAddressFactory()
    space = af.getDefaultAddressSpace()

    with open(targets_path, "r") as f:
        rows = json.load(f)

    # one lookup per distinct address
    by_addr = {}
    for r in rows:
        by_addr.setdefault(r["addr"], []).append(r)

    verdicts = {}
    for a in sorted(by_addr):
        addr = space.getAddress(a)
        f_at = fm.getFunctionAt(addr)
        if f_at is not None:
            verdicts[a] = {"verdict": "ENTRY", "name": f_at.getName()}
            continue
        f_in = fm.getFunctionContaining(addr)
        if f_in is not None:
            ep = f_in.getEntryPoint()
            verdicts[a] = {
                "verdict": "MID_BODY",
                "name": f_in.getName(),
                "entry": "0x%08x" % ep.getOffset(),
                "delta": a - ep.getOffset(),
            }
            continue
        verdicts[a] = {"verdict": "NO_FUNC", "name": None}

    out = []
    for a, v in verdicts.items():
        for r in by_addr[a]:
            out.append({
                "addr": "0x%08x" % a,
                "verdict": v["verdict"],
                "fn": v.get("name"),
                "entry": v.get("entry"),
                "delta": v.get("delta"),
                "kind": r["kind"],
                "file": r["file"],
                "line": r["line"],
                "snippet": r["snippet"],
            })

    rank = {"MID_BODY": 0, "NO_FUNC": 1, "ENTRY": 2}
    out.sort(key=lambda r: (rank[r["verdict"]], r["file"], r["line"]))
    with open(out_path, "w") as f:
        json.dump(out, f, indent=1)

    counts = {}
    for a, v in verdicts.items():
        counts[v["verdict"]] = counts.get(v["verdict"], 0) + 1
    return {
        "distinct_addresses": len(verdicts),
        "sites": len(out),
        "counts": counts,
        "out": out_path,
        "mid_body": [r for r in out if r["verdict"] == "MID_BODY"][:40],
    }
