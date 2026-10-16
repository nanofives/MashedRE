"""menu_team_select_tick registry entry (0x0042fa00), dualcopy4:
  * two path1 vectors that discriminate the original (guard `!= 0`, no clamp) from the retired asi twin (guard `> 0`, clamp [0, 2]):
      v7  team[0] = -1 (0xFFFFFFFF) with UP pressed  -> original -2 (0xFFFFFFFE); the clamped twin gives 0
      v8  team[0] =  3 with no press                 -> original 3;               the clamped twin gives 2
  * path2_tests (the entry had none ON PURPOSE because the install was the other twin; now this body is the install).
    py -3.12 registry_team_tick.py [--apply]
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
REG = ROOT / "re" / "frida" / "hooks_registry.py"
raw = REG.read_bytes()
nl = b"\r\n" if b"\r\n" in raw else b"\n"
ls = raw.split(nl)
i = [k for k, l in enumerate(ls) if l.startswith(b"    'menu_team_select_tick': {")]
assert len(i) == 1
i = i[0]
j = next(k for k in range(i + 1, len(ls)) if ls[k].rstrip() == b"    },")
blk = ls[i:j + 1]

SEED_HEAD = ("{'seed': [{'addr': '0x007f1a14', 'val': 0}, {'addr': '0x007f1a24', 'val': 1}, {'addr': '0x007f1a34', 'val': 2}, {'addr': '0x007f1a44', 'val': 3},\n"
             "                      {'addr': '0x0067e938', 'val': %s}, {'addr': '0x0067e944', 'val': 0}, {'addr': '0x0067e950', 'val': 1}, {'addr': '0x0067e95c', 'val': 2},\n"
             "                      {'addr': '0x0067ea88', 'val': 0},\n"
             "                      {'addr': '0x007f1044', 'val': %s}, {'addr': '0x007f1504', 'val': 0},\n"
             "                      {'addr': '0x007f1090', 'val': 0}, {'addr': '0x007f1550', 'val': 0},\n"
             "                      {'addr': '0x007f10dc', 'val': 0}, {'addr': '0x007f159c', 'val': 0},\n"
             "                      {'addr': '0x007f1128', 'val': 0}, {'addr': '0x007f15e8', 'val': 0}],\n"
             "             'args': [],\n"
             "             'obs':  ['0x0067e938', '0x0067e944', '0x0067e950', '0x0067e95c', '0x0067ea88']}")
v7 = SEED_HEAD % ("0xFFFFFFFF", "0x00000001")
v8 = SEED_HEAD % ("3", "0")
v0 = SEED_HEAD % ("0", "0x00000100")
v2 = SEED_HEAD % ("1", "0x00000001")

# find the closing of path1_tests: the first line that is exactly 8 spaces + "],"
k = next(n for n, l in enumerate(blk) if l.rstrip() == b"        ],")
ind = "            "
ins = ("\n".join(ind + ln if n else ind + ln for n, ln in enumerate(
    ("# v7 dualcopy4: team -1 with UP pressed -> the original decrements (guard != 0), the retired clamped twin gave 0\n" + v7 + ",\n"
     "# v8 dualcopy4: team 3, no press -> the original leaves it, the retired clamped twin clamped to 2\n" + v8 + ",").split("\n")))).encode()
# the SEED_HEAD continuation lines carry their own 22-space indent; only the first line of each vector needs the 12
ins = ("            # v7 dualcopy4: team -1 with UP pressed -> the original decrements (guard != 0); the retired clamped twin gave 0\n"
       "            " + v7 + ",\n"
       "            # v8 dualcopy4: team 3, no press -> the original leaves it; the retired clamped twin clamped to 2\n"
       "            " + v8 + ",").encode()
blk[k:k] = ins.split(b"\n")

# replace the "NO path2_tests ON PURPOSE" comment block (from that line to the line before the closing "    },")
p2 = next(n for n, l in enumerate(blk) if b"NO path2_tests ON PURPOSE" in l)
new_p2 = ("        # path2 (dualcopy4): MenuTeamSelectTick is now the INSTALLED body of 0x0042fa00 (MenuNav.cpp RH_ScopedInstall), so the\n"
          "        # JMP at the RVA points at this export. The entry had no path2_tests while the installed hook was the asi twin.\n"
          "        'path2_tests': [\n"
          "            " + v0 + ",\n"
          "            " + v2 + ",\n"
          "            " + v7 + ",\n"
          "        ],").encode().split(b"\n")
blk[p2:len(blk) - 1] = new_p2
ls[i:j + 1] = blk
print("entry rewritten: +2 path1 vectors, +3 path2 vectors")
if "--apply" in sys.argv:
    REG.write_bytes(nl.join(ls))
    print("written")
