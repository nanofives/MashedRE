# HB-ARCTIC-NOLIGHT-CAP original-side driver. One MASHED.exe launch per pose, each preceded by the
# audio-endpoint check (Get-PnpDevice -Class AudioEndpoint -PresentOnly must list a VB-Audio device,
# else the run stops BLOCKED). Run the whole driver inside mashed_lock.py run (one hold for all
# poses, so each check is made right before its boot). Poses and settle are HB-ARCTIC-OPENSEA's
# (verify/hbarcticopensea_20261027/RESULT_HB-ARCTIC-OPENSEA.md section 2), captured with its
# capture_orig.py from the private root.
# usage: py -3.12 run_orig.py <root> <datadir> [pose ...]
import subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
W = HERE.parent.parent
CAP = W / "verify" / "hbarcticopensea_20261027" / "capture_orig.py"
POSES = {"e1": "10,6,20,90,25", "w1": "-28,6,14,270,25", "w2": "-30,8,0,270,35", "n1": "-5,6,30,0,25"}
SETTLE = "3"


def audio_check(log):
    out = subprocess.run(["pwsh", "-NoProfile", "-Command",
                          "Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.Status + ' ' + $_.FriendlyName }"],
                         capture_output=True, text=True).stdout
    ok = "VB-Audio" in out
    log.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} audio check {'OK' if ok else 'NONE'}\n{out}")
    log.flush()
    return ok


def main():
    root, data = sys.argv[1], Path(sys.argv[2])
    todo = sys.argv[3:] or list(POSES)
    data.mkdir(parents=True, exist_ok=True)
    with open(data / "audio_checks.txt", "a") as log:
        for i, p in enumerate(todo, 1):
            if not audio_check(log):
                print(f"[{i}/{len(todo)}] {p} BLOCKED no VB-Audio endpoint", flush=True)
                return 2
            rc = subprocess.call([sys.executable, str(CAP), root, str(data), f"{p}={SETTLE}#{POSES[p]}"])
            print(f"[{i}/{len(todo)}] {p} rc={rc}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
