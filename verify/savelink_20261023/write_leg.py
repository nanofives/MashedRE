"""savelink write leg (PREREG_savelink_write.md): K1-K4. Run outside the sandbox.
    py -3.12 verify/savelink_20261023/write_leg.py
"""
import hashlib, os, shutil, subprocess, sys, time
from pathlib import Path
REPO = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(REPO / "re" / "tools"))
import save_roundtrip as rt

ROOT = Path("D:/MashedWT/savelink_root"); DATA = Path("D:/MashedWT/savelink_data")
GS = ROOT / "original" / "gamesave.bin"; BAK = ROOT / "original" / "gamesave.bin.preexe_bak"
SHIP = Path(os.environ.get("SAVELINK_SHIP", str(DATA / "shipped.bin"))); EXE = REPO / "mashedmod" / "build" / "mashed_re.exe"
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest()[:16]
out = []
def P(*a):
    s = " ".join(str(x) for x in a); print(s); out.append(s)

def run_exe(tag, env_extra, secs=12):
    for f in (ROOT / "gameflow_selftest.txt", ROOT / "mashed_re_gamesave.bin"):
        if f.exists(): f.unlink()
    env = dict(os.environ, MASHED_ROOT=str(ROOT), MASHED_SAVE_SELFTEST="1",
               MASHED_SAVE_SELFTEST_OUT2=str(DATA / f"selftest_{tag}.txt"))
    env.pop("MASHED_SAVE_PATH", None); env.pop("MASHED_SAVE_ORIGINAL", None)
    env.update(env_extra)
    p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(secs)
    alive = p.poll() is None
    if alive: p.kill()                      # our own PID only
    p.wait()
    t = DATA / f"selftest_{tag}.txt"
    return t.read_text() if t.exists() else "(no selftest output)"

def reset():
    shutil.copy2(SHIP, GS)
    if BAK.exists(): BAK.unlink()

# K4 / K0 file side: knob OFF
reset(); before = sha(GS)
txt_off = run_exe("off", {})
P("K4 knob OFF: gamesave.bin unchanged:", sha(GS) == before, "| .preexe_bak absent:", not BAK.exists())
P("K1(off) selftest:", txt_off.strip().splitlines()[-1] if txt_off.strip() else txt_off)

# K1 / K2: knob ON
reset()
txt_on = run_exe("on", {"MASHED_SAVE_ORIGINAL": "1", "MASHED_SAVE_SELFTEST_SHIPPED": str(SHIP)})
P(txt_on.strip())
last = txt_on.strip().splitlines()[-1] if txt_on.strip() else ""
P("K1 RESULT line:", last)
P("K2 .preexe_bak == shipped:", BAK.exists() and sha(BAK) == sha(SHIP))
merged = DATA / "merged_by_exe.bin"; shutil.copy2(GS, merged)
P("K2 merged file size", merged.stat().st_size, "magic", merged.read_bytes()[:4].hex())

# K3: the original loads the merged file
cells, _ = rt.run_probe(ROOT / "original", "dumptable", DATA / "arm_E.log")
file_cells = rt.span_cells(merged)
diff = [(r, c, file_cells[r][c], cells[r][c]) for r in range(13) for c in range(12) if cells[r] and cells[r][c] != file_cells[r][c]]
P("K3 original's table == merged file span on 156 cells:", all(cells) and not diff, "diff", diff[:6])
reset()                                     # leave the private root pristine
(DATA / "write_leg_result.txt").write_text("\n".join(out) + "\n", encoding="utf-8")
