"""dualcopy4 allowlist transaction (adapted from dualcopy3): drop every key the lint no longer finds (re-anchored non-bodies, consolidated pairs) and add the
keys that were narrowed (a key's TU list shrank, so its text changed). Dry run by default, --apply writes.  py -3.12 allowlist_edit.py [--apply]"""
import pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import lint_rva_bodies as L  # noqa: E402
P = L.ALLOWLIST
found = {L.key(c, r, f) for c, r, f, _ in L.findings()}
raw = P.read_bytes().decode("utf-8"); nl = "\r\n" if "\r\n" in raw else "\n"
out, dropped = [], []
for ln in raw.split(nl):
    k = ln.split("#", 1)[0].strip()
    if k and k not in found:
        dropped.append(k); continue
    out.append(ln)
have = {ln.split("#", 1)[0].strip() for ln in out}
add = [k for k in sorted(found) if k not in have]
NOTE = {
    "DUP-IN-TARGET:004c39b0:Math/RwLutTables.cpp,Math/RwV3dNormalize.cpp":
        "audit=DIFFERS-BEHAVIOUR; narrowed 2026-10-16 (dualcopy4): Race/RaceCamera.cpp:Vec3Norm (division by an exact sqrt) is now a call into Math::LutNormalize "
        "(d1r2 replay: pose position median 1.105e-4 -> 7.97e-5), so it is no longer a body. What remains is the pair that waits for a USER DECISION: Math/RwLutTables.cpp:LutNormalize "
        "= the REAL-LUT body (tables fitted 4096/4096 vs the live original, verify/d3_modes37_20261009/o_rwlut.*.bin; used by Race/PathNodes.cpp and now the race camera) and the global "
        "Math/RwV3dNormalize.cpp:RwV3dNormalize, which keeps its exact-sqrt fallback in the exe because it feeds D2 physics (flipping it reopens D2 numbers; memory exe-rw-sqrt-is-exact-not-lut).",
}
while out and out[-1] == "": out.pop()
if add:
    out += ["", "# --- dualcopy4 (2026-10-16): keys narrowed after a body was replaced by a call into the shared one"]
    out += [f"{k}   # {NOTE.get(k, 'narrowed (dualcopy4)')}" for k in add]
out.append("")
print("dropped", len(dropped)); [print("  -", d) for d in dropped]
print("added", len(add)); [print("  +", a) for a in add]
if "--apply" in sys.argv:
    P.write_bytes(nl.join(out).encode("utf-8")); print("written")
