"""Append the group-(b) block-1/2 note to the allowlist after allowlist_edit.py --apply."""
import pathlib
p = pathlib.Path(__file__).resolve().parent.parent.parent / "re" / "tools" / "dual_copy_allowlist.txt"
t = p.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in t else "\n"
note = [
    "# --- dualcopy4 (2026-10-16, ROADMAP D4 group b, blocks 1 and 2): 5 more keys gone, 27 -> 22.",
    "# 00420de0 (dead C float copy in Vehicle/Damage.cpp deleted; the naked x87 Accum420de0 is the body), 00405890 (Race/CollectDone.h is the one body, three thin readers),",
    "# 0042f6b0 (Race/SelectionToGameMode.h is the one table), 0042ad90 (Frontend/KvFind.cpp is the one body, both targets), 0042fa00 (the asi's clamped",
    "# PlayerSlotEdgeAdjust deleted; Frontend/MenuNav.cpp:MenuTeamSelectTick follows the original and is now the install). Evidence: verify/dualcopy4_20261016/RESULT_dualcopy4.md.",
]
p.write_bytes((t.rstrip("\r\n") + nl + nl.join(note) + nl).encode("utf-8"))
print("note appended")
