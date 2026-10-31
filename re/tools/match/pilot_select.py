"""Deterministic stratified sample of MASHED.exe game functions for the matching pilot (2026-10-30).
Strata: small < 64 B, medium 64-400 B, large 401-2000 B (cap: one function per session).
Groups (hooks.csv subsystem): physics = vehicle/physics/smplfzx, ai, render = render/particle/sky/camera,
frontend = frontend/hud, save, audio, race = gameplay/track/powerups/world-objects. Seeded random within a stratum,
round-robin over groups. Only 0x401000-0x49c000 (the game objects; above it hooks.csv
labels RenderWare, RW audio and CRT functions with game subsystems). Thunks (func_bounds thunk=1) are excluded.
    py -3.12 re/tools/match/pilot_select.py > verify/matchpilot_20261030/sample.tsv"""
import csv, pathlib, random, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match
GROUPS = {"physics": {"vehicle", "physics", "smplfzx"}, "ai": {"ai"}, "render": {"render", "particle", "sky", "camera"},
          "frontend": {"frontend", "hud"}, "save": {"save"}, "audio": {"audio"},
          "race": {"gameplay", "track", "powerups", "world-objects"}}
FORCED = [0x0046dbe0, 0x00423e60, 0x00424920]
QUOTA = {"small": 15, "medium": 20, "large": 15}
thunk = {int(r["rva"], 16) for r in csv.DictReader(open(match.ROOT / "re/console/cache/func_bounds.csv")) if r["thunk"] == "1"}
rows = []
for r in csv.reader(open(match.ROOT / "hooks.csv", newline="")):
    try: va = int(r[0], 16)
    except (ValueError, IndexError): continue
    g = next((k for k, v in GROUPS.items() if r[2] in v), None)
    n = match.ghidra_size(va)
    if g is None or n is None or va in thunk or not (0x401000 <= va < 0x49c000): continue  # game objects only
    s = "small" if n < 64 else "medium" if n <= 400 else "large" if n <= 2000 else None
    if s: rows.append((va, n, r[2], g, s, r[1], r[3]))
rnd = random.Random(20261030)
pick = [x for x in rows if x[0] in FORCED]
for s, q in QUOTA.items():
    have = [x for x in pick if x[4] == s]
    pools = {g: rnd.sample([x for x in rows if x[3] == g and x[4] == s and x not in pick], k=len([x for x in rows if x[3] == g and x[4] == s and x not in pick])) for g in GROUPS}
    order = list(GROUPS); i = 0
    while len(have) < q:
        g = order[i % len(order)]; i += 1
        if pools[g]: have.append(pools[g].pop()); pick.append(have[-1])
print("rva\tsize\tsubsystem\tgroup\tstratum\tname\tconfidence")
for x in sorted(pick, key=lambda x: ("small medium large".split().index(x[4]), x[0])):
    print(f"0x{x[0]:08x}\t{x[1]}\t{x[2]}\t{x[3]}\t{x[4]}\t{x[5]}\t{x[6]}")
