"""Second witness for the part code: the original's car record slot of each atomic
(FUN_00420420 stores atomic at rec + FUN_004b5190(atomic)*4, read live by camera_probe.py
--matfx) against the DFF geometry whose rpUSERDATA "0.tv_part_id" equals that slot.
Compares the geometry flags (low 16 bits) and material texture names.

    py -3.12 partid_check.py <trace.csv.matfx.json> <userdata dump of the DFF> <rwmat dump of the DFF>
"""
import json
import re
import sys

snap = [s for s in json.load(open(sys.argv[1])) if "s" in s][-1]
car = snap["s"]["cars"][0]
geo_flags = {}
for line in open(sys.argv[2]):
    m = re.match(r"geo\[(\d+)\] flags=0x([0-9a-f]+) .*'0\.tv_part_id', 1, \[(-?\d+)\]", line)
    if m:
        geo_flags[int(m.group(3))] = int(m.group(2), 16)
ok = bad = missing = 0
for part in car["parts"]:
    p = part["p"]
    if p not in geo_flags:
        missing += 1
        print(f"slot {p:3d}: no DFF geometry with tv_part_id {p}")
        continue
    a, b = part["gflags"] & 0xffff, geo_flags[p] & 0xffff
    if a == b:
        ok += 1
    else:
        bad += 1
        print(f"slot {p:3d}: live gflags {a:#06x} vs DFF {b:#06x}")
print(f"slots {len(car['parts'])}: flag match {ok}, mismatch {bad}, no geometry {missing}; "
      f"DFF part ids {len(geo_flags)}")
