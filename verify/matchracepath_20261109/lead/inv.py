import re, subprocess, pathlib, csv, sys
ROOT = pathlib.Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration")
rp = subprocess.run(["git", "-C", str(ROOT), "show", "orch/PORT-ORIGTOP-SIZE:verify/portorigtop_20261109/race_point.tsv"],
                    capture_output=True, text=True).stdout
rows = []
for line in rp.splitlines()[1:]:
    ph, cls, cnt, lst = (line.split("\t") + [""])[:4]
    if ph not in ("race_load", "race_run"): continue
    if cls not in ("game-c0c3", "fam:rwphysics", "fam:rwcore", "fam:crt", "fam:lua", "fam:qhull"): continue
    for m in re.finditer(r"0x([0-9a-f]{8})\[([^,]*),(C\d),(\d+)B\]", lst):
        if m.group(3) == "C4": continue
        rows.append((ph, cls, m.group(1), m.group(2), m.group(3), int(m.group(4))))
# sources
src = {}
for p in (ROOT / "re/match").rglob("*.c"):
    try: t = p.read_text(errors="replace")
    except Exception: continue
    for m in re.finditer(r"// MATCH: 0x([0-9a-f]{8})", t):
        src.setdefault(m.group(1), []).append(str(p.relative_to(ROOT)).replace("\\", "/"))
# best score from results tsvs
best = {}
for p in (ROOT / "verify").rglob("results*.tsv"):
    try:
        with open(p, newline="", errors="replace") as f:
            r = csv.DictReader(f, delimiter="\t")
            for d in r:
                rva = (d.get("rva") or "").lower().replace("0x", "")
                if not rva: continue
                for k in ("after_pct", "pct", "score", "best_pct", "acc", "insn_pct"):
                    v = d.get(k)
                    if v:
                        try: v = float(v.rstrip("%"))
                        except ValueError: continue
                        if v > best.get(rva, (-1,))[0]: best[rva] = (v, str(p.relative_to(ROOT)).replace("\\", "/"))
                        break
    except Exception as e:
        pass
# hooks subsystem
sub = {}
with open(ROOT / "hooks.csv", newline="", errors="replace") as f:
    for d in csv.DictReader(f):
        sub[d["rva"].lower()] = (d["subsystem"], d["confidence"])
seen = set()
out = ["phase\tclass\trva\tname\tconf\tbytes\tsubsystem\thooks_conf\tbest_pct\tbest_from\tsources"]
for ph, cls, rva, nm, c, b in rows:
    if rva in seen: continue
    seen.add(rva)
    s, hc = sub.get(rva, ("?", "?"))
    bp = best.get(rva, ("", ""))
    out.append(f"{ph}\t{cls}\t0x{rva}\t{nm}\t{c}\t{b}\t{s}\t{hc}\t{bp[0]}\t{bp[1]}\t{','.join(src.get(rva, []))}")
pathlib.Path(sys.argv[1]).write_text("\n".join(out) + "\n")
print(len(out) - 1, "rows")
