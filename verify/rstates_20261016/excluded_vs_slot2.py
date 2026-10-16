#!/usr/bin/env python3
"""rstates F6 check: each track's Clump_Exclude_From_World set (COURSE.LUA) against the clumps its
after-world render slot draws (slot2_order.json). Clump indices are Clump_Filename's first argument.

    py -3.12 excluded_vs_slot2.py
"""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import piz_extract  # noqa: E402

TR = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART\TRACKS")
PIZ = {"training": "training", "superg": "SuperG", "arctic": "Arctic", "rouabout": "rouabout",
       "sands": "sands", "warzone": "Warzone", "forest": "Forest", "egypt": "Egypt", "storm": "Storm",
       "dump": "dump", "highway": "Highway", "neustein": "Neustein", "city": "City"}


def course_lua(piz):
    data, _v, _a, entries, _m = piz_extract.read_archive(TR / f"{piz}.piz")
    for (n, o, ln, _i) in entries:
        name = n.decode("latin-1") if isinstance(n, bytes) else n
        if name.split("\\")[-1].upper() == "COURSE.LUA":
            return data[o:o + ln].decode("latin-1")
    return ""


def strip_comments(txt):
    return "\n".join(l.split("--")[0] for l in txt.splitlines())


def main():
    s2 = json.loads((HERE / "slot2_order.json").read_text())
    for track, piz in PIZ.items():
        lua = strip_comments(course_lua(piz))
        files = {int(m.group(1)): m.group(2) for m in
                 re.finditer(r'Clump_Filename\s*\(\s*(\d+)\s*,\s*"([^"]+)"', lua)}
        excl = sorted({int(m.group(1)) for m in re.finditer(r'Clump_Exclude_From_World\s*\(\s*(\d+)', lua)})
        lst = s2[track]["clumps"]
        print(f"{track:9s} excluded={excl} slot2={lst} other={s2[track]['other']}")
        print(f"          excluded-not-in-slot2={[ (i, files.get(i)) for i in excl if i not in lst]}")
        print(f"          slot2-not-excluded={[ (i, files.get(i)) for i in lst if i not in excl]}")


if __name__ == "__main__":
    main()
