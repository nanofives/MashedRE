# sweep_rega_eval.py — CLASS A stage 2: function bodies + callers for every live hook.
#
# READ-ONLY: opens no transaction, mutates nothing. Safe against a pool slot.
#
# Run via the MCP `ghidra_eval` tool against an open session:
#   _ns = {}
#   exec(open(r"<repo>/scripts/ghidra/sweep_rega_eval.py").read(), _ns)
#   _ns["run"](sessions, "<session_id>", r"<hooks_inventory.json>", r"<out.json>")
#
# For each hooked RVA we need, from the ORIGINAL binary:
#   body_start / body_end   -> so capstone can compute which registers it actually writes
#   callers                 -> a caller that is NOT itself hooked still runs original code,
#                              and is therefore entitled to hold a live value in any
#                              register the original preserves across the CALL.
import json


def run(sessions, session_id, inv_path, out_path):
    prog = sessions[session_id]
    fm = prog.getFunctionManager()
    af = prog.getAddressFactory()
    space = af.getDefaultAddressSpace()

    inv = json.load(open(inv_path))
    hooks = inv["hooks"]
    hooked_rvas = set(h["rva"] for h in hooks)

    out = []
    for h in hooks:
        a = h["rva"]
        addr = space.getAddress(a)
        f = fm.getFunctionAt(addr)
        rec = {"rva": a, "sym": h["sym"], "naked": h["naked"],
               "file": h["file"], "line": h["line"]}
        if f is None:
            rec["status"] = "NO_FUNC"
            out.append(rec)
            continue
        body = f.getBody()
        rec["status"] = "OK"
        rec["name"] = f.getName()
        rec["start"] = int(body.getMinAddress().getOffset())
        rec["end"] = int(body.getMaxAddress().getOffset())
        callers = []
        try:
            for c in f.getCallingFunctions(None):
                ep = int(c.getEntryPoint().getOffset())
                callers.append(ep)
        except Exception:
            pass
        rec["callers"] = callers
        rec["callers_hooked"] = [c for c in callers if c in hooked_rvas]
        rec["callers_unhooked"] = [c for c in callers if c not in hooked_rvas]
        out.append(rec)

    with open(out_path, "w") as fh:
        json.dump(out, fh)
    ok = sum(1 for r in out if r.get("status") == "OK")
    return "%d hooks, %d resolved, %d NO_FUNC" % (len(out), ok, len(out) - ok)
