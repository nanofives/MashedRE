"""RO-CTL-68BC0C regression: verdict of every recorded run against r1/r2 with prov_compare.py at HEAD (old) and in the
working tree (new). py -3.12 -I regress.py OLD_DIR   (OLD_DIR holds `git show HEAD:re/frida/prov_compare.py`)."""
import importlib.util, glob, os, sys
from pathlib import Path
FR = Path(__file__).resolve().parents[2] / "re" / "frida"
sys.path.insert(0, str(FR))


def mod(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


old = mod(os.path.join(sys.argv[1], "prov_compare.py"), "pc_old")
new = mod(FR / "prov_compare.py", "pc_new")
I = "D:/MashedWT/PROV-INSITU_data/"
runs = sorted(glob.glob("D:/MashedWT/PROV-WRITESET-SCENARIO_data/*/ticks.bin") + glob.glob("D:/MashedWT/PROV-FIX-9_data/*/ticks.bin")
              + glob.glob("D:/MashedWT/PROV-DIVERGED-18_data/*/ticks.bin") + glob.glob("D:/MashedWT/RO-CTL-68BC0C_data/w1/ticks.bin"))
same = changed = 0
for t in runs:
    d = os.path.dirname(t)
    try:
        a = old.verdict(d, I + "r1", I + "r2", [(I + "r1", I + "r2")])
        b = new.verdict(d, I + "r1", I + "r2", [(I + "r1", I + "r2")])
    except AssertionError as e:
        print(f"{d}: skipped (record layout differs from r1)")
        continue
    ka = (a["verdict"], a.get("identical_for"), [x[0] for x in (a["first"] or {}).get("blocks", [])])
    kb = (b["verdict"], b.get("identical_for"), [x[0] for x in (b["first"] or {}).get("blocks", [])])
    if ka == kb:
        same += 1
    else:
        changed += 1
    print(f"{'SAME   ' if ka == kb else 'CHANGED'} {d}: old {ka}  new {kb}")
print(f"runs {same + changed}: same {same}, changed {changed}")
