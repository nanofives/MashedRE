"""RWC-THUNK-BIND-AUDIT run 44: 12_City control arms (PREREG_CITY13.md + amendment 1).

    py -3.12 verify/rwcthunk_20261113/city_arms.py <data_dir> ARM:exe [ARM:exe ...]

Each arm = one `run13.py run --tracks 12 --attempts 1` on the private root, wrapped test-slot outside and mashed_lock
inside, then `run13.py score --tracks 12`. Prints one line per arm: City rows, cam_dump md5, flag/cd at seq 3.
Spawns only through run13.py, which stops only its own PID.
"""
import hashlib
import os
import subprocess
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
ORCH = WT.parent / "orch-integration"
SLOT = Path(r"C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py")
LOCK = ORCH / "re" / "orchestrator" / "mashed_lock.py"
RUN13 = WT / "verify" / "rsky_20261016" / "run13.py"
CITE = WT / "verify" / "rres_20261016" / "cites_rres.json"
ROOT = Path("D:/MashedWT/RWC-THUNK-BIND-AUDIT_root")


def main():
    data = Path(sys.argv[1])
    arms = [a.split(":", 1) for a in sys.argv[2:]]
    env = dict(os.environ, PROYECTOS_TEST_HUB="off")
    n = len(arms)
    for i, (name, exe) in enumerate(arms, 1):
        out = data / f"{i:02d}_{name}"
        cmd = ["py", "-3.12", str(SLOT), "py", "-3.12", str(LOCK), "run", "--label", "RWC-THUNK-BIND-AUDIT", "--",
               "py", "-3.12", str(RUN13), "run", "--exe", exe, "--root", str(ROOT), "--out", str(out),
               "--tracks", "12", "--attempts", "1"]
        rc = subprocess.call(cmd, env=env)
        sc = subprocess.run(["py", "-3.12", str(RUN13), "score", "--out", str(out), "--cite", str(CITE), "--tracks", "12"],
                            capture_output=True, text=True)
        (out / "tracks_score.txt").write_text(sc.stdout + sc.stderr)
        rows = [l for l in sc.stdout.splitlines() if l.startswith("12_City")]
        cam = out / "12_City" / "stand" / "cam_dump.csv"
        md5 = hashlib.md5(cam.read_bytes()).hexdigest()[:8] if cam.exists() else "none"
        s3 = ""
        if cam.exists():
            ls = cam.read_text().splitlines()
            if len(ls) > 3:
                f = ls[3].split(",")
                s3 = f"seq{f[0]} flag={f[27]} cd={f[28]}"
        summ = " ".join("/".join(r.split()[3:6:2]) + "/rows" + r.split()[8] for r in rows)
        print(f"[{i}/{n}] {name} rc={rc} City {summ} cam={md5} {s3} {'PASS' if rows else 'FAIL'}", flush=True)


if __name__ == "__main__":
    main()
