"""DDKOBJ-REGCONV arm 5 confirmation: boot the bisect's culprit alone (knob OFF and ON) and the remaining 37 units with
the knob ON, through bisect_units.probe (same build, boot and FAIL/OK rule as the bisect). Run under test-slot with
DDKOBJ_REGCONV=promote, BISECT_ROOT, BISECT_DATA and BISECT_LABEL set as in run_arms.sh.

    py -3.12 verify/ddkobj_20261112/confirm_probes.py <regconv38.json> <out_tsv> <culprit_rva>
"""
import json
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / "re" / "tools" / "ddkobj"))
import bisect_units as b  # noqa: E402

rep = json.loads(pathlib.Path(sys.argv[1]).read_text())
culprit = sys.argv[3]
allu = [b.first_rva(u) for u in rep["units"] if u["status"] == "ok"]
rest = [u for u in allu if u != culprit]
with open(sys.argv[2], "a") as log:
    b.probe([culprit], "solo_off", "off", log)
    b.probe([culprit], "solo_on", "on", log)
    b.probe(rest, "rest37_on", "on", log)
