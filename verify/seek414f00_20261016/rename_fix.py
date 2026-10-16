"""Redo the AiPowerupSeek -> AiTrackObjSeek rename on the two files a PowerShell one-liner corrupted (single-pair hashtable entries
flattened to strings, so .Replace(p[0], p[1]) swapped single characters). Restores each from HEAD first, keeps its newline style."""
import subprocess
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
PAIRS = [
    ("AiPowerupSeek_Exe", "AiTrackObjSeek_Exe"),
    ("AiPowerupSeek.h", "AiTrackObjSeek.h"),
    ("AiPowerupSeek.cpp", "AiTrackObjSeek.cpp"),
    ("AiPowerupSeek", "AiTrackObjSeek"),
    ("reads the Warzone platform words", "reads the course-0x21 track-object words"),
    ("AI Warzone platform seek FUN_00414f00", "AI course-0x21 track-object seek FUN_00414f00"),
]
for rel, head in (("mashedmod/src/mashed_re/Ai/AiStandalone.cpp", "mashedmod/src/mashed_re/Ai/AiStandalone.cpp"),
                  ("mashedmod/src/mashed_re/Ai/AiTrackObjSeek.h", "mashedmod/src/mashed_re/Ai/AiPowerupSeek.h")):
    data = subprocess.run(["git", "show", f"HEAD:{head}"], cwd=str(WT), capture_output=True, check=True).stdout.decode("utf-8")
    for a, b in PAIRS:
        data = data.replace(a, b)
    (WT / rel).write_bytes(data.encode("utf-8"))
    print(rel, "restored+renamed", data.count("AiTrackObjSeek"), "refs")
