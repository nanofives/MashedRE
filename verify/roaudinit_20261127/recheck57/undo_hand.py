"""Put REACH-6's two former hand edits back to their pre-edit state (for the tool reproduction check).

1. re-add "Match\\PortTop_004ccef0.cpp" to mashedmod/exe_sources.rsp after "Match\\PortTop_005c6f50.cpp"-ish position
   (the line position it held in 97f4246cf: before the first line that sorts after it, as the tool's own insertion would).
2. rewrite the BINDINGS.tsv 0x004ccef0 row to its pre-edit text (100ba0cc5 = 6b7da6d8e: kind missing, named stop PortTop_AStop_004ccef0).
"""
import pathlib

root = pathlib.Path(__file__).resolve().parents[3]
rsp = root / "mashedmod" / "exe_sources.rsp"
lines = rsp.read_bytes().decode("utf-8").split("\n")
new = '"Match\\PortTop_004ccef0.cpp"'
assert new not in [l.rstrip("\r") for l in lines]
eol = "\r" if lines[0].endswith("\r") else ""
# insert where 97f4246cf had it: right before the line that followed it there
import subprocess
old = subprocess.run(["git", "show", "97f4246cf:mashedmod/exe_sources.rsp"], cwd=root, capture_output=True).stdout.decode().split("\n")
idx = [l.rstrip("\r") for l in old].index(new)
after = old[idx + 1].rstrip("\r")
pos = [l.rstrip("\r") for l in lines].index(after)
lines.insert(pos, new + eol)
rsp.write_bytes("\n".join(lines).encode("utf-8"))
print("rsp: inserted at line", pos + 1, "before", after)

b = root / "verify" / "porttop_20261108" / "BINDINGS.tsv"
bl = b.read_bytes().decode("utf-8").split("\n")
n = 0
for i, l in enumerate(bl):
    if l.startswith("0x004ccef0\t"):
        bl[i] = ("0x004ccef0\tFUN_004ccef0\tmissing\tPortTop_AStop_004ccef0\tPORT-TOP-AUDIOTICK-STOPS: named stop "
                 "(log + exit code = rva, Match/PortTop_AudioStops.cpp). No matched or admitted body\t3\t") + (
                     "\r" if l.endswith("\r") else "")
        n += 1
b.write_bytes("\n".join(bl).encode("utf-8"))
print("BINDINGS rows rewritten:", n)
