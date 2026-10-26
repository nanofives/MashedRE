# Mass C3->C4 navigate testing — orchestrates canonical_c4_navigate.run() over many
# candidates with two robustness features mass testing needs:
#
#  1. OFF-run CHUNKING: Interceptor-counting N functions in one boot risks the hot-path
#     destabilization (CLAUDE.md: >1000/s aggregate drifts MASHED in ~6s). So the exercise
#     pass counts candidates in chunks of --chunk per boot (default 6) and merges counts.
#  2. ON-run CHUNKED INSTALL + CRASH BISECTION: candidates are installed in groups of --on-chunk
#     per boot (default 16; 0 = all in one boot). If a group crashes, only THAT group is bisected
#     to isolate which RVA(s) crash -> those get survived=False, the rest survived=True. One bad
#     hook costs a bisect of its own group, not of the whole batch. A piz-collision crash
#     (U-9677, MASHED.exe+0x1509b4 / +0xe66db) is re-run once, never bisected.
#     [UNCERTAIN] the default of 16 is untuned (re/HARNESS_BACKLOG.md); ROI is reasoned, not measured.
#
# --force-bad 0xRVA = harness self-test only: every ON run that installs that RVA overwrites its
#   JMP with `mov eax,[0]` once the nav completes, so the next call faults for real. Proves the
#   chunk + bisect path end to end. A run with it is NOT evidence for any hook.
# MASHED_ROOT = a private root containing original\ (canonical_c4_navigate.py honours it).
# --out-tag X writes log/c4_nav_batch_resultX.{json,md} instead of the shared default name.
#
# Candidate sources (pick one): --rvas 0x..,0x..  |  --manifest <file> (lines: 0xRVA[,nav])
#   |  --from-hookscsv <subsystem> [--status impl] (pulls hooks.csv rows; navigate self-filters
#   via the exercised>0 gate -- fns that don't fire on the chosen screen just report HOLD).
#
# All candidates in one invocation share ONE --nav path (one target screen). Run again with a
# different --nav for a different screen. Navigation = FUN_00497310 return override
# (4=confirm,11=up,12=down), fully in-process. REPORT-ONLY: never auto-promotes; promotion is
# the re-classify skill's call (leaf-gating). See re/analysis/frontend_input_nav_trace/TRACE.md.
#
# Usage:
#   py -3.12 re/frida/c4_navigate_batch.py --from-hookscsv frontend --status impl \
#       --nav 4,4 --chunk 6 [--on-chunk 16] [--seconds 22] [--shot-dir verify/c4nav]
#   py -3.12 re/frida/c4_navigate_batch.py --rvas 0x0042e3a0,0x0042d5a0 --nav 4,4
import csv, json, os, sys, time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import canonical_c4_navigate as H   # reuse run(), shoot(), AGENT

ROOT = Path(__file__).resolve().parent.parent.parent
LOG  = ROOT / "log"
HOOKS = ROOT / "hooks.csv"


def load_from_hookscsv(subsystem, status_filter):
    rows = []
    with open(HOOKS, newline="", encoding="utf-8") as f:
        r = csv.reader(f); next(r, None)
        for row in r:
            if len(row) < 6: continue
            rva, name, subsys, conf, status, file = row[0], row[1], row[2], row[3], row[4], row[5]
            if subsys != subsystem: continue
            if conf != "C3": continue
            if status_filter and status != status_filter: continue
            notes = row[8] if len(row) > 8 else ""
            rva = rva.strip()
            if not rva.lower().startswith("0x"): rva = "0x" + rva
            rows.append({"rva": rva.lower(), "name": name, "file": file, "notes": notes})
    return rows


def load_manifest(path):
    rows = []
    for line in Path(path).read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"): continue
        parts = [p.strip() for p in line.split(",")]
        rva = parts[0]
        if not rva.lower().startswith("0x"): rva = "0x" + rva
        rows.append({"rva": rva.lower(), "name": parts[1] if len(parts) > 1 else rva,
                     "file": "", "notes": ""})
    return rows


def chunked(seq, n):
    for i in range(0, len(seq), n):
        yield seq[i:i+n]


FORCE_BAD = None              # --force-bad 0xRVA: harness self-test (fault injection), see main()
PIZ_COLLISION = (0x005509b4, 0x004e66db)   # MASHED.exe+0x1509b4 / +0xe66db (U-9677): re-run, not a finding


def run_on(group, nav, settle, dwell, seconds, shot=None, shotat=None):
    """ON run of `group` installed together. A group holding FORCE_BAD gets that RVA's JMP poisoned
    after the nav, so it faults for real. A piz-collision crash (U-9677) is re-run once."""
    poison = FORCE_BAD if FORCE_BAD in group else None
    for attempt in (0, 1):
        res = H.run(",".join(group), group, None, nav, settle, dwell, seconds,
                    shot=shot, shotat=shotat, poison=poison)
        c = res["crash"]
        try: eip = int(c["eip"], 16) if c else None
        except Exception: eip = None
        if eip not in PIZ_COLLISION or attempt: return res
        print(f"      piz-collision crash eip={c['eip']} (U-9677) -> re-run")
    return res


def bisect_bad(rvas, nav, settle, dwell, seconds, depth=0):
    """Return set of RVAs that crash the ON run (installed together). Bisects on crash."""
    if not rvas: return set()
    ind = "  " * (depth + 2)
    print(f"{ind}ON-test {len(rvas)}: {','.join(rvas)}")
    res = run_on(rvas, nav, settle, dwell, seconds)
    if res["crash"] is None and res["alive"]:
        return set()                      # whole group survives -> none bad
    if len(rvas) == 1:
        c = res["crash"]
        print(f"{ind}  -> {rvas[0]} CRASHES alone (eip={c['eip'] if c else '?'})")
        return set(rvas)                  # this one is the culprit
    mid = len(rvas) // 2
    bad = bisect_bad(rvas[:mid], nav, settle, dwell, seconds, depth+1)
    bad |= bisect_bad(rvas[mid:], nav, settle, dwell, seconds, depth+1)
    if not bad:
        # neither half crashes alone but the union did -> interaction crash; flag all as suspect
        print(f"{ind}  -> interaction crash (halves survive separately); flagging group suspect")
        return set(rvas)
    return bad


def main():
    a = sys.argv
    nav     = [int(x) for x in a[a.index("--nav")+1].split(",")] if "--nav" in a else [4, 4]
    chunk   = int(a[a.index("--chunk")+1])   if "--chunk"   in a else 6
    settle  = int(a[a.index("--settle")+1])  if "--settle"  in a else 7000
    dwell   = int(a[a.index("--dwell")+1])   if "--dwell"   in a else 1500
    seconds = int(a[a.index("--seconds")+1]) if "--seconds" in a else 22
    shotdir = a[a.index("--shot-dir")+1]     if "--shot-dir" in a else None
    on_chunk = int(a[a.index("--on-chunk")+1]) if "--on-chunk" in a else 16   # 0 = all at once
    outtag  = a[a.index("--out-tag")+1]      if "--out-tag" in a else ""
    global FORCE_BAD
    if "--force-bad" in a:
        FORCE_BAD = a[a.index("--force-bad")+1].lower()
        if not FORCE_BAD.startswith("0x"): FORCE_BAD = "0x" + FORCE_BAD
        print(f"!!! --force-bad {FORCE_BAD}: HARNESS SELF-TEST, its JMP is poisoned in every ON run "
              f"that installs it. Verdicts from this run are not evidence for any hook.")

    if "--rvas" in a:
        cands = [{"rva": (x if x.lower().startswith("0x") else "0x"+x).lower(), "name": x,
                  "file": "", "notes": ""} for x in a[a.index("--rvas")+1].split(",")]
    elif "--manifest" in a:
        cands = load_manifest(a[a.index("--manifest")+1])
    elif "--from-hookscsv" in a:
        sub = a[a.index("--from-hookscsv")+1]
        status = a[a.index("--status")+1] if "--status" in a else None
        cands = load_from_hookscsv(sub, status)
    else:
        sys.exit("need --rvas | --manifest <f> | --from-hookscsv <subsystem> [--status impl]")

    rvas = [c["rva"] for c in cands]
    by_rva = {c["rva"]: c for c in cands}
    print(f"=== mass navigate C4: {len(rvas)} candidates, nav={nav}, chunk={chunk}, on_chunk={on_chunk}, seconds={seconds} ===")
    for c in cands: print(f"    {c['rva']}  {c['name']}")
    LOG.mkdir(parents=True, exist_ok=True)

    # ---- PHASE 1: exercise (OFF), chunked ----
    print(f"\n=== PHASE 1: exercise on navigated scenario (OFF, chunks of {chunk}) ===")
    counts = {}
    for ci, group in enumerate(chunked(rvas, chunk)):
        print(f"  chunk {ci}: counting {group}")
        off = H.run("__none__", [], group, nav, settle, dwell, seconds)
        if off["crash"]:
            print(f"    chunk CRASHED (likely hot-path Interceptor) eip={off['crash']['eip']}; "
                  f"counts unreliable for this chunk -> retry singly")
            for one in group:                          # isolate: count each alone
                o1 = H.run("__none__", [], [one], nav, settle, dwell, seconds)
                counts[one] = -1 if o1["crash"] else int(o1["counts"].get(one, 0))
        else:
            for k, v in off["counts"].items():
                try: counts[k.lower()] = int(v)
                except Exception: counts[k.lower()] = 0
        print(f"    navTaps={off['navc']} counts={off['counts']}")

    # ---- PHASE 2: install in groups of --on-chunk + survive (ON), bisect within a crashing group ----
    groups = list(chunked(rvas, on_chunk)) if on_chunk > 0 else [rvas]
    print(f"\n=== PHASE 2: install + navigate + survive (ON, {len(groups)} group(s) of "
          f"{on_chunk if on_chunk > 0 else len(rvas)}, bisect within a crashing group) ===")
    shot = None
    if shotdir:
        Path(ROOT / shotdir).mkdir(parents=True, exist_ok=True)
        shot = f"{shotdir}/c4nav_ON_g0.png"
    bad = set(); jmp = {}; group_log = []
    for gi, group in enumerate(groups):
        print(f"  group {gi}: installing {group}")
        res = run_on(group, nav, settle, dwell, seconds,
                     shot=(f"{shotdir}/c4nav_ON_g{gi}.png" if shotdir else None), shotat=(seconds*0.6))
        if res["crash"] is None and res["alive"]:
            print(f"    group {gi} installed + survived (navTaps={res['navc']})")
            jmp.update(res["jmp"])
            group_log.append({"group": gi, "rvas": group, "crashed": False, "bad": []})
            continue
        c = res["crash"]
        print(f"    group {gi} crashed (eip={c['eip'] if c else '?'}) -> bisecting within the group")
        gbad = bisect_bad(group, nav, settle, dwell, seconds)
        bad |= gbad
        # re-read JMP bytes from a survivor install (install the group's good set) for installed= flag
        good = [r for r in group if r not in gbad]
        if good:
            jmp.update(run_on(good, nav, settle, dwell, seconds).get("jmp", {}))
        for b in gbad: jmp.setdefault(b, None)
        group_log.append({"group": gi, "rvas": group, "crashed": True,
                          "eip": c["eip"] if c else None, "bad": sorted(gbad)})

    # ---- verdicts ----
    # window over which post-nav counts accumulated (rough), for hot-path detection
    window_s = max(0.5, seconds - (settle + len(nav)*dwell)/1000.0 - 0.8)
    HOT_RATE = 800   # calls/s; Interceptor above this destabilizes (CLAUDE.md) -> evidence suspect
    print(f"\n=== VERDICTS (exercised AND installed AND survived) -- REPORT ONLY ===")
    out = {}; n_ready = 0; rows_md = []
    for r in rvas:
        calls = counts.get(r, counts.get(r.lower(), 0))
        installed = (jmp.get(r) == 0xE9)
        survived = (r not in bad)   # whole batch survived -> bad empty -> all True; else not bad
        rate = (calls / window_s) if isinstance(calls, int) and calls > 0 else 0
        hot = rate > HOT_RATE
        exercised = isinstance(calls, int) and calls > 0
        ready = exercised and installed and survived and not hot
        if ready: n_ready += 1
        why = []
        if calls == -1: why.append("hot-crash-isolated")
        elif not exercised: why.append("not-exercised")
        if hot: why.append(f"HOT~{int(rate)}/s(behavioral-lane)")
        if not installed: why.append("no-JMP")
        if not survived: why.append("crashes")
        tag = "C4-trio-OK" if ready else ("HOT" if hot else "HOLD")
        out[r] = {"name": by_rva[r]["name"], "calls": calls, "rate_per_s": round(rate, 1),
                  "hot": hot, "installed": installed, "survived": survived,
                  "C4_trio": ready, "notes": by_rva[r].get("notes", "")}
        print(f"  {r} {by_rva[r]['name']:<26} calls={str(calls):<6} ~{int(rate):>5}/s "
              f"inst={installed} surv={survived} -> {tag} {','.join(why)}")
        rows_md.append(f"| {r} | {by_rva[r]['name']} | {calls} | {int(rate)}/s | {installed} | "
                       f"{survived} | {'**'+tag+'**' if ready else tag} | {','.join(why)} |")

    out["_meta"] = {"nav": nav, "chunk": chunk, "on_chunk": on_chunk, "force_bad": FORCE_BAD,
                    "n": len(rvas), "n_trio_ok": n_ready, "bad": sorted(bad), "groups": group_log,
                    "shot": shot, "window_s": round(window_s, 1)}
    res_path = LOG / f"c4_nav_batch_result{outtag}.json"
    res_path.write_text(json.dumps(out, indent=2), encoding="utf-8")
    md = (f"# C4 navigate batch — nav={nav}, n={len(rvas)}, {n_ready} trio-OK\n\n"
          f"| RVA | name | calls(post-nav) | rate | installed | survived | verdict | why |\n"
          f"|---|---|---|---|---|---|---|---|\n" + "\n".join(rows_md) + "\n")
    md_path = LOG / f"c4_nav_batch_result{outtag}.md"
    md_path.write_text(md, encoding="utf-8")
    print(f"\n  {n_ready}/{len(rvas)} pass exercised+installed+survived+not-hot on nav={nav}")
    print(f"  HOT fns need the behavioral-survival lane (no Interceptor), not this count.")
    print(f"  REPORT ONLY -- promote via re-classify (leaf-gated). -> {res_path}  +  {md_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
