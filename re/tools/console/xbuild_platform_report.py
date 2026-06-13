#!/usr/bin/env python3
r"""xbuild_platform_report.py — two signals about the PC/Xbox boundary.

  signal="platform-api"  — PC function references a platform-API token
                  (Direct3D/D3D9, DirectInput, DirectShow, Win32 window/wave/
                  registry, LoadLibrary). Platform-by-content, match-independent.
                  This is the CLEAN signal: a token hit that is also unmatched
                  is almost certainly PC-only platform glue.

  signal="recall-gap"  — PC function is CALLED by matched code yet has no Xbox
                  twin. HONEST CAVEAT: with ~27% matcher recall this is
                  dominated by small getters that hash ambiguously (no strings,
                  generic mnemonics), NOT by platform divergence. Shipped as a
                  matcher-coverage diagnostic (which small functions we miss,
                  by subsystem), explicitly NOT as platform attribution.

Output: re/console/match/platform_candidates.csv + honest summary to stdout.
"""
import csv
import re
from collections import Counter, defaultdict

ROOT = "."
PC_FEAT = f"{ROOT}/re/console/pc_features.csv"
MATCH = f"{ROOT}/re/console/match/xbuild_match_v2.csv"
HOOKS = f"{ROOT}/hooks.csv"
OUT = f"{ROOT}/re/console/match/platform_candidates.csv"

# platform-API tokens that do not exist on the Xbox/PS2 builds
TOKENS = re.compile(
    r"d3d9|direct3d|idirect3d|createdevice|d3dx|dinput|directinput|"
    r"directsound|dsound|waveout|wavein|mmsystem|midiout|"
    r"createwindow|wndproc|registerclass|getmessage|peekmessage|"
    r"wsad|winsock|regopenkey|regqueryvalue|regsetvalue|"
    r"directshow|igraphbuilder|imediacontrol|quartz|"
    r"changedisplaysettings|enumdisplay|wglcreate|gdi32|"
    r"\.dll|\\windows\\|getprocaddress|loadlibrary",
    re.I)


def load_pc_features():
    funcs = {}
    with open(PC_FEAT, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.DictReader(f):
            entry = int(row["entry"], 16)
            funcs[entry] = {
                "size": int(row["size"]),
                "strings": [s for s in row["strings"].split("|") if s],
                "callees": [int(c, 16) for c in row["callees"].split(";") if c],
            }
    return funcs


def load_matched_pc():
    matched = set()
    with open(MATCH, newline="") as f:
        for row in csv.DictReader(f):
            matched.add(int(row["pc_va"], 16))
    return matched


def load_hooks():
    info = {}
    with open(HOOKS, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#") or row[0] == "rva":
                continue
            try:
                va = int(row[0], 16)
            except ValueError:
                continue
            if va < 0x400000:
                va += 0x400000
            info[va] = (row[1], row[2], row[3])
    return info


def main():
    pc = load_pc_features()
    matched = load_matched_pc()
    hooks = load_hooks()

    # invert callees -> callers
    callers = defaultdict(set)
    for caller, f in pc.items():
        for callee in f["callees"]:
            callers[callee].add(caller)

    rows = []
    for va, f in pc.items():
        name, sub, conf = hooks.get(va, ("", "", ""))
        if sub.startswith("third-party"):
            continue  # library code is shared, not platform glue
        token_hits = sorted({t.lower() for s in f["strings"]
                             for t in TOKENS.findall(s)})
        is_matched = va in matched
        matched_callers = sum(1 for c in callers.get(va, ()) if c in matched)
        total_callers = len(callers.get(va, ()))
        # orphan signal only meaningful for unmatched functions with callers
        orphan = (not is_matched) and matched_callers >= 1
        content = bool(token_hits)
        if not (content or (orphan and total_callers >= 2)):
            continue
        signal = "platform-api" if content else "recall-gap"
        rows.append({
            "pc_va": f"0x{va:08x}", "signal": signal, "matched": int(is_matched),
            "matched_callers": matched_callers, "total_callers": total_callers,
            "tokens": ";".join(token_hits), "name": name,
            "subsystem": sub, "confidence": conf,
        })

    rows.sort(key=lambda r: (r["signal"] != "platform-api",
                             -r["matched_callers"], r["pc_va"]))
    with open(OUT, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=[
            "pc_va", "signal", "matched", "matched_callers", "total_callers",
            "tokens", "name", "subsystem", "confidence"])
        w.writeheader()
        w.writerows(rows)

    api = [r for r in rows if r["signal"] == "platform-api"]
    api_unmatched = [r for r in api if not r["matched"]]
    gap = [r for r in rows if r["signal"] == "recall-gap"]
    print("=== CLEAN SIGNAL: platform-API functions (unmatched = PC-only glue) ===")
    for r in api:
        crt = "(CRT)" if "crt" in r["name"].lower() or r["tokens"] == ".dll" else ""
        print(f"  {r['pc_va']} m={r['matched']} [{r['subsystem']:8}] "
              f"{r['name'][:30]:30} <{r['tokens']}> {crt}")
    print(f"  -> {len(api_unmatched)} unmatched platform-API functions; the "
          f"DirectShow/D3DX/D3D9 ones are the real PC-only render/IO glue.\n")
    print(f"=== DIAGNOSTIC: matcher recall-gap orphans: {len(gap)} ===")
    print("  (small getters the matcher missed — NOT platform attribution)")
    sub = Counter(r["subsystem"] for r in gap if r["subsystem"])
    for s, n in sub.most_common(12):
        print(f"  {s:30} {n}")
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
