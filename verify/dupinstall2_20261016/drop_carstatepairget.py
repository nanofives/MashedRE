"""46cbb0 consolidation, code side: delete CarStatePairGet (Util/PromoLoop_round25.cpp) and its registry key.
Survivor is VehicleCarStateRead (Vehicle/VehicleState.cpp), not touched. Re-runnable only on a clean tree."""
import pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
p = ROOT / "mashedmod/src/mashed_re/Util/PromoLoop_round25.cpp"
t = p.read_bytes().decode("utf-8")
L = t.split("\r\n")
assert L[18].startswith("// ----") and L[19].startswith("// CarStatePairGet"), (L[18], L[19])
assert L[62].startswith("RH_ScopedInstall(CarStatePairGet"), L[62]
new = [
    "// ---------------------------------------------------------------------------",
    "// FUN_0046cbb0 (CarStatePairGet, per-car state + secondary getter) was ported here first.",
    "// Its one-body-per-RVA copy is VehicleCarStateRead in Vehicle/VehicleState.cpp (also compiled",
    "// into mashed_re.exe, so exe_file == file). That is the body that installed (link order) when",
    "// the C4 in-race racediff ran (re/analysis/phys_c4_evidence/c4_racediff_result_2026-08-15.json,",
    "// installed by RVA), so the C4 evidence covers it. The duplicate that sat here (CarStatePairGet,",
    "// RH_ScopedInstall 0x0046cbb0) was removed 2026-10-16 (dupinstall2).",
    "// Analysis: re/analysis/vehicle_damage_d2/0x0046cbb0.md",
    "// ---------------------------------------------------------------------------",
]
L[18:63] = new
t = "\r\n".join(L)
old = "//   0x0046cbb0  CarStatePairGet — vehicle; fn(i, out_state*, out_secondary*)"
assert old in t
t = t.replace(old, "//   (0x0046cbb0 CarStatePairGet removed 2026-10-16, see below; the body is VehicleCarStateRead)")
p.write_bytes(t.encode("utf-8"))
print("round25 edited")

r = ROOT / "re/frida/hooks_registry.py"
s = r.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in s else "\n"
R = s.split(nl)
i = [k for k, l in enumerate(R) if l.startswith("    'car_state_pair_get': {")]
assert len(i) == 1
i = i[0]
j = i
while not R[j].startswith("    },"):
    j += 1
c = i - 1
while R[c].lstrip().startswith("#"):
    c -= 1
print("registry lines", c + 2, "..", j + 1)
R[c + 1:j + 2] = []
r.write_bytes(nl.join(R).encode("utf-8"))
print("registry pruned")
