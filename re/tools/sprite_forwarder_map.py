"""Map every call site of the four sprite-dictionary forwarders in MASHED.exe to
the string it resolves, off the anchored binary.

MASHED has FOUR named-sprite dictionaries, not one. `FUN_0040bbb0` opens
`d:\\toastart\\common\\sfx.piz` and fills four contiguous list heads, each with
its own 20-byte forwarder onto the shared search routine `FUN_004c5c00`:

  0x0040bb30 -> DAT_0063b8f8   FX.TXD           (world / effects)
  0x0040bb50 -> DAT_0063b8fc   BADGES.TXD       (23 tex, 16x16 UI glyphs)
  0x0040bb70 -> DAT_0063b900   TrackImages.txd
  0x0040bb90 -> DAT_0063b904   Interface.txd    (30 tex, 32x32 UI art)

A fifth head at 0x0068b9ac sits outside the array (FUN_00458630, powerups).

WHY THIS EXISTS: answering "where does this sprite come from?" by searching
whichever TXD is already open produced a committed, wrong conclusion once
(Finding 36 -> re/analysis/chalsel_icon_dictionary_20260905.md). Several names
exist in more than one dictionary at different sizes -- `Star` is 16x16 in
BADGES and 32x32 in INTERFACE, and `lock`/`Lock` differ only in case, which
`FUN_004c5c00` folds. The forwarder RVA at the call site is the only reliable
discriminator. Run this before sourcing any new named texture in the port.

For each `call <forwarder>` we walk BACKWARD over a window of decoded
instructions and collect every `push imm32` whose immediate lands in .rdata and
reads as a plausible C string. A ternary (two pushes joined by a jmp) yields two
candidates, which is the correct answer, not an error; the LAST push listed is
normally the argument actually in flight. Sites that pass the key in a register
print as unresolved rather than being guessed at.

Not covered: the three arg-rewriting tail-jmp gates that supply names on behalf
of a caller -- 0x0042ee00 (-> bb50: lock/dot/check), 0x004391b0 (-> bb90:
Lock/Star/tick) and 0x0042fab0 (-> bb90: the 10 NFL* car colours). Their callers
show up here as calls to the GATE, not to a forwarder, so check those separately.

Usage:  py -3.12 re/tools/sprite_forwarder_map.py
"""
import struct, re, sys
from pathlib import Path
import capstone

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe.unpatched"
FWD = {0x0040bb30: "bb30/FX",
       0x0040bb50: "bb50/BADGES",
       0x0040bb70: "bb70/TRACKIMAGES",
       0x0040bb90: "bb90/INTERFACE"}

d = EXE.read_bytes()
pe = struct.unpack_from("<I", d, 0x3C)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
opt = struct.unpack_from("<H", d, pe + 20)[0]
base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
sec0 = pe + 24 + opt
secs = []
for i in range(nsec):
    s = sec0 + i * 40
    nm = d[s:s + 8].rstrip(b"\0").decode()
    vsz, va, rsz, ro = struct.unpack_from("<IIII", d, s + 8)
    secs.append((nm, va, vsz, ro, rsz))
TXT = [s for s in secs if s[0] == ".text"][0]
_, tva, tvs, tro, trs = TXT


def rva2off(va):
    rva = va - base
    for nm, v, vs, ro, rs in secs:
        if v <= rva < v + max(vs, rs):
            return ro + (rva - v), nm
    return None, None


def cstr(va, maxlen=40):
    off, sec = rva2off(va)
    if off is None or sec not in (".rdata", ".data"):
        return None
    raw = d[off:off + maxlen]
    end = raw.find(b"\0")
    if end <= 0 or end > 32:
        return None
    s = raw[:end]
    if not all(32 <= c < 127 for c in s):
        return None
    return s.decode()


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
text = d[tro:tro + trs]
tbase = base + tva

# 1. find every direct call to a forwarder
sites = []
for m in re.finditer(rb"\xe8", text):
    off = m.start()
    if off + 5 > len(text):
        continue
    rel = struct.unpack_from("<i", text, off + 1)[0]
    va = tbase + off
    tgt = va + 5 + rel
    if tgt in FWD:
        sites.append((va, tgt))

print(f"# {len(sites)} direct call sites across the four forwarders\n")

# 2. for each, decode backward window and collect push imm32 -> string
WIN = 64
rows = []
for va, tgt in sorted(sites):
    start = va - WIN
    soff = start - tbase
    if soff < 0:
        continue
    names = []
    # decode from several alignments; keep the decode that ends exactly at `va`
    for skip in range(0, 24):
        insns = list(md.disasm(text[soff + skip: va - tbase], start + skip))
        if insns and insns[-1].address + insns[-1].size == va:
            for ins in insns:
                if ins.mnemonic == "push" and ins.op_str.startswith("0x"):
                    s = cstr(int(ins.op_str, 16))
                    if s:
                        names.append((ins.address, s))
            break
    rows.append((va, FWD[tgt], names))

for va, fwd, names in rows:
    if names:
        txt = ", ".join(f"{n!r}@{hex(a)}" for a, n in names[-3:])
    else:
        txt = "<no literal push in window — indirect/register key>"
    print(f"0x{va:08x}  {fwd:18s}  {txt}")
