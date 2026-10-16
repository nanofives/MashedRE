"""txdscope: cites for the after arm = rskyverify's merged cites (42) minus C30-TXD-SCOPE plus C30-TRAIN-ROOF-LEAD.

    py -3.12 verify\\txdscope_20261016\\make_cites_txdscope.py

C30-TXD-SCOPE (kind EXTRA, `trainroof`, Dump) described the standalone texturing the train roof from CITY.TXD. With the scoped lookup the
standalone no longer draws `trainroof`, so the row cannot occur and the cite is retired (U-9724 resolved). In its place the roof, now
untextured on both sides, pairs with the original's untextured roof draw and shows the train's own one-tick lead (C28 / U-9725):
the same 0.0571 maximum shift as the C28 train row on all three Dump frames; the standalone-side box is identical to the box of the old
EXTRA row (geometry unchanged, only the texture dropped). The cite is tight: kind MOVED, texture `-`, Dump only, and the row detail must
carry exactly the 0.0571 shift, so any other motion of an untextured Dump draw stays RED.
"""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
SRC = HERE.parent / "rskyverify_20261016" / "cites_merged.json"
OUT = HERE / "cites_txdscope.json"

cites = json.loads(SRC.read_text())
kept = [c for c in cites if c.get("id") != "C30-TXD-SCOPE"]
assert len(kept) == len(cites) - 1, "C30-TXD-SCOPE not found exactly once"
kept.append({
    "id": "C30-TRAIN-ROOF-LEAD",
    "kind": ["MOVED"],
    "tex": "^-$",
    "tracks": ["Dump"],
    "detail": "moved by up to 0\\.0571",
    "status": "MAPPED",
    "evidence": "verify/txdscope_20261016/RESULT_txdscope.md section 4; base vs after Dump rows (compare_rows_base_vs_after.txt): "
                "the old EXTRA trainroof box equals the new MOVED box on all three frames",
    "note": "[txdscope] The train roof (Train.dff, 32 triangles, material `trainroof`) is untextured on both sides now: the original binds only "
            "dump.txd (CreateFromDescription 0x00479330, FUN_004c5c80 at 0x004793de) and `trainroof` exists only in CITY.TXD of dump.piz, so "
            "FUN_004c5cb0 returns 0 for it; the standalone looks only in the named dictionary (U-9724 resolved). The roof then pairs with the "
            "original's untextured roof draw and moves by the same 0.0571 in x as the cited train body (C28): the one-tick phase of "
            "SetClumpAnim, U-9725. Replaces C30-TXD-SCOPE.",
})
OUT.write_text(json.dumps(kept, indent=1))
print(f"{len(cites)} cites in, {len(kept)} out -> {OUT}")
