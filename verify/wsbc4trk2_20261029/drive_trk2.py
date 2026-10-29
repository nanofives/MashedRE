"""D4-WSB-C4-SOLV-TRK2 (copy of verify/wsbc4trk1_20261028/drive_trk1.py from orch/D4-WSB-C4-SOLV-TRK1, data path, label, runner names changed; that file: driver (copy of verify/wsbsolvrgn_20261028/drive_rgn.py, data path, label, runner names changed): runs the PREREG legs one RVA per invocation of the committed runners
(run_shadow8.py / run_bvh8.py, unchanged) and adds the lead's run-20 audio rule around every invocation:
Get-PnpDevice -Class AudioEndpoint -PresentOnly before and after, and the Application log's MASHED.exe
crash events (Event 1000) since the invocation started. Endpoint missing or a fault at offset 0x001baf48
(0x005baf48) -> stop, BLOCKED audio endpoint, no retry. U-9677 piz-collision offsets (0x001509b4,
0x000e66db) -> the invocation is re-run (max 2 re-runs). Harness only: no read-out changes.

  py -3.12 verify/wsbc4trk2_20261029/drive_trk2.py shadow 0056bce0 ...
  py -3.12 verify/wsbc4trk2_20261029/drive_trk2.py bvh 0056bce0 ...
"""
import datetime, os, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
RUNNER = {"shadow": HERE / "run_shadow8.py", "bvh": HERE / "run_bvh8.py"}
LOG = Path("D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data/drive_trk2.log")
LOCK = WT.parent / "orch-integration" / "re" / "orchestrator" / "mashed_lock.py"


def note(s):
    line = f"[drive_trk2 {datetime.datetime.utcnow().strftime('%H:%M:%SZ')}] {s}"
    print(line, flush=True)
    with open(LOG, "a", encoding="utf-8") as f:
        f.write(line + "\n")


def endpoints():
    r = subprocess.run(["powershell", "-NoProfile", "-Command",
                        "Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.Status + ' ' + $_.FriendlyName }"],
                       capture_output=True, text=True)
    return [x for x in r.stdout.splitlines() if x.strip()]


def crashes(since):
    ps = ("Get-WinEvent -FilterHashtable @{LogName='Application'; Id=1000; StartTime=[datetime]'%s'} -ErrorAction SilentlyContinue"
          " | Where-Object { $_.Message -match 'MASHED.exe' } | ForEach-Object { (($_.Message -split \"`n\") | Select-Object -First 5 | ForEach-Object { $_.Trim() }) -join ' | ' }"
          % since.strftime("%Y-%m-%dT%H:%M:%S"))
    r = subprocess.run(["powershell", "-NoProfile", "-Command", ps], capture_output=True, text=True)
    return [x.strip() for x in r.stdout.splitlines() if x.strip()]


def main():
    if sys.argv[1] == "--inner":   # runs inside the solo window, right before the runner's boots
        leg, rva = sys.argv[2], sys.argv[3]
        ep = endpoints()
        note(f"{leg} {rva} pre-launch endpoints (inside the window): {ep or '<none>'}")
        if not ep:
            note("BLOCKED audio endpoint (none present before launch)"); return 3
        return subprocess.run([sys.executable, str(RUNNER[leg]), rva], cwd=str(WT)).returncode
    leg, rvas = sys.argv[1], sys.argv[2:]
    for i, rva in enumerate(rvas, 1):
        for attempt in range(1, 4):
            note(f"[{i}/{len(rvas)}] {leg} {rva} attempt {attempt}: queueing a solo window")
            # one solo window per row: the wait for other holders/windows happens here, outside the
            # runner's own 1800 s timer (row 3's first attempt spent its timer queued on the lock).
            # The runner still takes the lock per boot and is admitted by the inherited token.
            t0 = datetime.datetime.now() - datetime.timedelta(seconds=5)
            rc = subprocess.run([sys.executable, str(LOCK), "run", "--solo", "--label", "D4-WSB-C4-SOLV-TRK2", "--wait", "43200",
                                 "--", sys.executable, str(Path(__file__).resolve()), "--inner", leg, rva], cwd=str(WT)).returncode
            if rc == 3:
                return 3
            cr = crashes(t0)
            ep = endpoints()
            note(f"[{i}/{len(rvas)}] {leg} {rva} post endpoints: {ep or '<none>'} crash events: {cr or 'none'}")
            if any("1baf48" in c.lower() for c in cr):
                note("BLOCKED audio endpoint (AV at 0x005baf48)"); return 3
            if not ep:
                note("BLOCKED audio endpoint (dropped during the run)"); return 3
            if any(("1509b4" in c.lower() or "0e66db" in c.lower()) for c in cr):
                note(f"U-9677 piz collision on {rva}: re-run"); continue
            break
    return 0


if __name__ == "__main__":
    sys.exit(main())
