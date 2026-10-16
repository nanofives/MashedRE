"""Rewrite the registry entry menu_entry_array_init (0x0042d3e0) from void_write_observe on ONE dword to cache_setter_observe over every dword of six entries
(0, 1, 2, 28, 29 and 30 of the 52-byte array at 0x00898ac0) plus the dword before the array. Two fill patterns (all-ones, and a pattern with a distinct
byte per lane so a byte store that lands on the wrong lane shows). The original leaves entry +32 and entry 30 untouched, so a body that zeroes them is RED.
    py -3.12 registry_entry_array.py [--apply]"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
REG = ROOT / "re" / "frida" / "hooks_registry.py"
raw = REG.read_bytes()
nl = b"\r\n" if b"\r\n" in raw else b"\n"
ls = raw.split(nl)
i = [k for k, l in enumerate(ls) if l.startswith(b"    'menu_entry_array_init': {")]
assert len(i) == 1
i = i[0]
j = next(k for k in range(i + 1, len(ls)) if ls[k].rstrip().endswith(b"},") and ls[k].startswith(b"        'path2_tests'") or ls[k].rstrip() in (b"    },", b"    }"))
# the old entry ends with `'path2_tests': [...],    },` on one line; accept either shape
end = j
assert b"menu_entry_array_init" in ls[i]
# walk to the line that closes the entry (the first line at/after i whose stripped text ends with "}," at 4-space dict level)
for k in range(i + 1, len(ls)):
    if ls[k].rstrip().endswith(b"    },") or ls[k].rstrip() == b"    },":
        end = k
        break
base = 0x00898ac0
ents = [0, 1, 2, 28, 29, 30]
addrs = [base - 4] + [base + e * 0x34 + d * 4 for e in ents for d in range(13)]


def vec(fill):
    seeds = []
    for a in addrs:
        v = fill(a)
        seeds.append("{'addr': '0x%08x', 'val': 0x%08X}" % (a, v))
    rows = [", ".join(seeds[n:n + 3]) for n in range(0, len(seeds), 3)]
    seed_txt = ("[" + (",\n                      ").join(rows) + "]")
    obs = ["'0x%08x'" % a for a in addrs]
    orow = [", ".join(obs[n:n + 8]) for n in range(0, len(obs), 8)]
    obs_txt = "[" + (",\n                     ").join(orow) + "]"
    return "{'seed': " + seed_txt + ",\n             'args': [],\n             'obs':  " + obs_txt + "}"


v_ff = vec(lambda a: 0xFFFFFFFF)
v_pat = vec(lambda a: 0x80402010 ^ ((a >> 2) & 0xFF) * 0x01010101 & 0x7F7F7F7F | 0x01010101)
new = ("    # 0x0042d3e0  MenuEntryArrayInit  (rewritten 2026-10-16, dualcopy4). The original makes 14 stores per entry over 30 entries (disasm_42d3e0.txt):\n"
       "    # dwords entry +0 +4 +8, bytes +12..+15, dwords +16 +20 +24 +28 +36 +40 +44; entry +32 and entry 30 are NOT written. The old key observed only the first\n"
       "    # dword of entry 0 (void_write_observe), which the old body also cleared, so it could not see that body writing entry-4, bytes +9..+12, +32 and missing\n"
       "    # +13..15, +28, +44. This key fills entries 0 1 2 28 29 30 and the dword before the array and observes every one of them.\n"
       "    'menu_entry_array_init': {\n"
       "        'rva':            0x0042d3e0,\n"
       "        'export':         'MenuEntryArrayInit',\n"
       "        'signature':      {'ret': 'void', 'args': []},\n"
       "        'arg_type':       'cache_setter_observe',\n"
       "        'lut_root_delta': 0,\n"
       "        'path1_tests': [\n"
       "            # all dwords 0xFFFFFFFF\n"
       "            " + v_ff + ",\n"
       "            # a distinct non-zero byte per lane\n"
       "            " + v_pat + ",\n"
       "        ],\n"
       "        'path2_tests': [\n"
       "            " + v_ff + ",\n"
       "        ],\n"
       "    },")
# also swallow the 6 comment lines directly above the old entry
s = i
while s > 0 and ls[s - 1].startswith(b"    #"):
    s -= 1
print(f"old entry lines {s + 1}..{end + 1} ({end - s + 1} lines) -> new entry ({new.count(chr(10)) + 1} lines), {len(addrs)} addresses per vector")
ls[s:end + 1] = new.encode().split(b"\n")
if "--apply" in sys.argv:
    REG.write_bytes(nl.join(ls))
    print("written")
