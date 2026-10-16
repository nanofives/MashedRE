"""Insert registry key 'ai_powerup_seek' (0x00414f00) into re/frida/hooks_registry.py, just before 'ai_post_step_powerup_brake'.
Vectors from vectors.py.txt (make_vectors.py). Idempotent: refuses if the key exists. Keeps the file's own newline style."""
from pathlib import Path

HERE = Path(__file__).resolve().parent
REG = HERE.parent.parent / "re" / "frida" / "hooks_registry.py"

raw = REG.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in raw else "\n"
if "'ai_powerup_seek':" in raw:
    raise SystemExit("already present")
vec = [ln for ln in (HERE / "vectors.py.txt").read_text(encoding="utf-8").splitlines() if not ln.startswith("# ")]
vec_text = "\n".join(vec)
p2 = []
# path2: the first natural ret=1 vector, the first gate-2 vector and the first contrived one (3 vectors, 6 lines)
blocks = [vec[i:i + 3] for i in range(0, len(vec), 3)]
for b in (blocks[0], blocks[29], blocks[41]):
    p2 += b
entry = f"""    # [seek414f00 2026-10-16] FUN_00414f00, the AI Warzone platform seek (Ai/AiPowerupSeek.cpp, one body in both targets).
    # Reads 0x00684de0 / 0x0088ff50[v] / [0x00684dac]+0x30 (the platform object 0x00684d18, written by its state machine
    # FUN_00451d80) and the FUN_0046d4a0 rows of car v and car 0. Vectors from the ORIGINAL capture
    # verify/seek414f00_20261016/O3.msd.seek.csv (Warzone, rule 8: 29 natural ret=1 rows) plus contrived rows for the car-0
    # branch and the 0.5 / 30.0 / NaN gate edges (make_vectors.py). Observable = EAX (fold_ret) + the two cand dwords, which
    # start at the 0xaaaaaaaa sentinel, so a no-write is visible. Every seeded VA is restored.
    'ai_powerup_seek': {{
        'rva':            0x00414f00,
        'export':         'AiPowerupSeek',
        'signature':      {{'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32', 'int32']}},
        'arg_type':       'cache_setter_observe',
        'fold_ret':       True,
        'lut_root_delta': 0,
        'path1_tests': [
{vec_text}
        ],
        'path2_tests': [
{chr(10).join(p2)}
        ],
    }},
"""
anchor = "    'ai_post_step_powerup_brake': {"
i = raw.index(anchor)
new = raw[:i] + entry.replace("\n", nl) + raw[i:]
REG.write_bytes(new.encode("utf-8"))
print("inserted; vectors", sum(1 for ln in vec if "'args'" in ln), "path2", sum(1 for ln in p2 if "'args'" in ln))
