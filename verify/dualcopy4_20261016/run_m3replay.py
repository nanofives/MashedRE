"""run_m3replay.py <exe> <tag> : the mode-7 M3 pickup-callback replay gate (verify/d3_mode7_m3_20261010/run_m3_replay.ps1) on an explicit exe image, outputs to
verify/dualcopy4_20261016/m3r/<tag>_<set>_pc<pc>.txt. The replay feeds recorded calls of FUN_00458920 (6 live, 3,888 synthetic) to the exe's naked copy
PK_458920 and compares the result; PK_458920 calls FUN_00405890, which this lane moved from the naked copy PK_405890 to Race/CollectDone.cpp:Pred405890.
Expected, from RESULT_M3.md: live rows=6 exact=6, synth rows=3888 exact=3888, at PC_24 and PC_53. Spawns and stops ONLY its own pid."""
import os, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
M3 = HERE.parent / "d3_mode7_m3_20261010"
exe, tag = str(Path(sys.argv[1]).resolve()), sys.argv[2]
out = HERE / "m3r"
out.mkdir(exist_ok=True)
sets = {"live": ["o_cb1.msd.cb.tsv", "o_cb2.msd.cb.tsv", "o_cb3.msd.cb.tsv"], "synth": ["synth_unicorn.cb.tsv"]}
bad = 0
for st, ins in sets.items():
    for pc in ("24", "53"):
        rep = out / f"{tag}_{st}_pc{pc}.txt"
        if rep.exists():
            rep.unlink()
        env = dict(os.environ)
        env.update({"MASHED_ROOT": str(ROOT), "MASHED_MUTE": "1", "MASHED_TRACK_VIEW": "Training", "MASHED_CAR": "1", "MASHED_ROUND": "1",
                    "MASHED_ROUND_RULE": "4", "MASHED_WIN_POS": "primary-bl", "MASHED_DETERMINISTIC": "1", "MASHED_DET_FRAMES": "60",
                    "MASHED_PATHPROG": "1", "MASHED_PICKUPS_PC": pc,
                    "MASHED_PKCB_REPLAY": ";".join(str(M3 / i) for i in ins), "MASHED_PKCB_REPLAY_OUT": str(rep)})
        p = subprocess.Popen([exe], cwd=str(ROOT), env=env)
        try:
            rc = p.wait(600)
            done = True
        except subprocess.TimeoutExpired:
            p.kill()
            rc = p.wait(15)
            done = False
        line = ""
        if rep.exists():
            line = next((l for l in rep.read_text(errors="replace").splitlines() if l.startswith("G-M3-REPLAY")), "")
        print(f"{tag} {st} PC{pc}: pid={p.pid} exit={rc} self_exited={done}  {line[:110]}", flush=True)
        bad += 0 if ("rows=" in line and line.split("rows=")[1].split()[0] == line.split("exact=")[1].split()[0]) else 1
print("REPLAY", "GREEN" if not bad else f"RED ({bad} bad)")
sys.exit(1 if bad else 0)
