# PROV-INSITU-REF1: first fault of every CRASH repeat run (crash.jsonl from --crash-catch): eip, access, the first
# stack slot that points into a --prov blob (rva+off), clock call n. Prints one TSV line per run.
#   py -3.12 crash_sites.py <state.jsonl> [...]
import json, sys
from pathlib import Path
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
for sf in sys.argv[1:]:
    for ln in Path(sf).read_text().splitlines():
        s = json.loads(ln)
        if s["verdict"] != "CRASH":
            continue
        cf = D / s["run"] / "crash.jsonl"
        out = [s["rva"], s["run"]]
        if not cf.exists():
            print("\t".join(out + ["no crash.jsonl"])); continue
        f = next((json.loads(x) for x in cf.read_text().splitlines() if '"fault"' in x), None)
        if not f:
            print("\t".join(out + ["no fault record (stall or exit only)"])); continue
        blob = next((x[1] for x in f.get("stack", []) if isinstance(x, list) and "rva" in x[1]), None)
        e = f.get('eip_in') or {}; inblob = f"{e['rva']}+{e['off']}" if 'rva' in e else ''
        mem = f.get("memory") or {}
        out += [f"{f.get('type')} {mem.get('op', '')} {mem.get('address', '')} at eip {f['ctx']['eip']}",
                f"eip in blob {inblob}" if inblob else "",
                f"stack ret into {blob['rva']}+{blob['off']}" if blob else "no blob return on the stack", f"n={f.get('n')}"]
        print("\t".join(out))
