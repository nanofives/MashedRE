"""dualcopy3 allowlist transaction: drop every key the lint no longer finds (re-anchored non-bodies, consolidated pairs), narrow
0x004cbd30 to its real pair. Dry run by default, --apply writes.  py -3.12 allowlist_edit.py [--apply]"""
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
NOTE = {"CROSS-TARGET:004cbd30:D3d9Render/RwWorldStream.cpp,Render/TextureLoader_q6.cpp":
        "audit=DIFFERS-BEHAVIOUR; narrowed 2026-10-16 (dualcopy3): Audio/MixedC3Sweep.cpp:Orig_StreamRead was a trampoline INTO the original, not a body. "
        "Real pair: exe-only RwWorldStream.cpp:Rwl_StreamRead ports only the memory stream (type 3) and emits err 5 on a short read; asi-only "
        "TextureLoader_q6.cpp:RwStreamRead dispatches all four stream types. Merge = shared core for the memory path + per-target file/callback thunks. Render file, out of this lane."}
while out and out[-1] == "": out.pop()
if add:
    out += ["", "# --- ADDED 2026-10-16 (dualcopy3, ROADMAP D4 item 4): keys narrowed after re-anchoring call-throughs"]
    out += [f"{k}   # {NOTE.get(k, 'narrowed (dualcopy3)')}" for k in add]
out.append("")
print("dropped", len(dropped)); [print("  -", d) for d in dropped]
print("added", len(add)); [print("  +", a) for a in add]
if "--apply" in sys.argv:
    P.write_bytes(nl.join(out).encode("utf-8")); print("written")
