# SOAK-P0: generate the scenarios that carry input rows. py -3.12 gen_scen.py
# Input rows are [dN (main-thread clock calls after the race anchor), control, value] (detorig.py INPUT shape, ~11 calls per tick).
import json, pathlib

d = pathlib.Path(__file__).resolve().parent / "scen"
base = {"track": 0, "mode": 10, "rule": 0, "car": 0, "team": 0, "cars": 4}


def pulses(start_n, count, period_n=660, width_n=110, ctl=4):
    rows = []
    for i in range(count):
        t = start_n + i * period_n
        rows += [[t, ctl, 0xff], [t + width_n, ctl, 0]]
    return rows


# c: all-AI slots plus 1 Hz pulses of control 4 on player 0 from dN 8000 (about tick 727 after the anchor), 150 pulses
(d / "c_ai4_pulse4.json").write_text(json.dumps({**base, "slots": [2, 2, 2, 2], "input": pulses(8000, 150)}) + "\n")
# e: human slot 0 (state 1) plus the same pulses: the condition measured on 2026-10-12 (c4_detect_e_scenario.py) in a warp
(d / "e_human_pulse4.json").write_text(json.dumps({**base, "slots": [1, 2, 2, 2], "input": pulses(8000, 150)}) + "\n")
