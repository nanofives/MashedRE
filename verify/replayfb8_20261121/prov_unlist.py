# REPLAY-FALLBACK-8: take the PROVISIONAL.tsv rows of replay-C4 bodies off the list (run 54 merge bar: "a c4=frida-replay
# body whose source is the replay-verified file -> replay-C4 body"), so ddkobj links the unit's DDK object and the 100%
# neighbour is byte-checked again. Only rows whose (rva, source) equal an argument pair go.
#   py -3.12 verify/replayfb8_20261121/prov_unlist.py 0x00401690=re/match/s01/00401690.c [...]
import sys
from pathlib import Path
P = Path(__file__).resolve().parents[2] / "re" / "match" / "PROVISIONAL.tsv"
want = {tuple(a.lower().split("=", 1)) for a in sys.argv[1:]}
raw = P.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in raw else "\n"
keep, gone = [], []
for ln in raw.split(nl):
    f = ln.split("\t")
    if len(f) > 2 and (f[0].lower(), f[2].lower()) in want:
        gone.append(ln[:120])
    else:
        keep.append(ln)
P.write_bytes(nl.join(keep).encode("utf-8"))
print(f"removed {len(gone)}:", *gone, sep="\n  ")
