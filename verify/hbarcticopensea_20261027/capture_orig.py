# HB-ARCTIC-OPENSEA original-side captures. Arctic = Bronze Cup 1 challenge index 3
# (verify/arctic_ref/FINDINGS.md table). Runs race_draw_burst.py from the PRIVATE root copy
# (<root>/re/frida/), so its ROOT, EXE and the save swap of run_with_unlocked_save.py all
# resolve inside <root>/original and the shared install is never touched.
#
# usage: py -3.12 capture_orig.py <root> <datadir> <name>=<settle>[@<hold>][#<x>,<y>,<z>,<az>,<el>] ...
#   <hold> is race_draw_burst.py --hold, e.g. s3a=2@4:3 (accelerate 3 s, settle 2 s)
#   #... pins the camera (race_draw_burst.py --pose): eye (x,y,z), azimuth az deg (0 = +Z,
#   90 = +X), elevation el deg downward. Basis convention of the original's own frame
#   (measured on orig/s20: at=(sin az cos el, -sin el, cos az cos el), up=(sin az sin el,
#   cos el, cos az sin el), right = up x at).
#   writes <datadir>/orig/<name>/a.bmp + orig_cambasis.txt + orig_track.txt ...
import math, subprocess, sys, time
from pathlib import Path

root, data = Path(sys.argv[1]), Path(sys.argv[2])
settles = sys.argv[3:]
save = data / "save_unlock_0123.bin"
if not save.exists():
    subprocess.run([sys.executable, str(root / "re/tools/gamesave_edit.py"),
                    str(root / "original/gamesave.bin"), "-o", str(save),
                    "--rows", "0,1,2,3", "--set", "c1=1,c11=1"], check=True)


def basis(x, y, z, az, el):
    a, e = math.radians(az), math.radians(el)
    at = (math.sin(a) * math.cos(e), -math.sin(e), math.cos(a) * math.cos(e))
    up = (math.sin(a) * math.sin(e), math.cos(e), math.cos(a) * math.sin(e))
    rt = (up[1] * at[2] - up[2] * at[1], up[2] * at[0] - up[0] * at[2], up[0] * at[1] - up[1] * at[0])
    return ",".join(f"{v:.5f}" for v in (x, y, z) + rt + up + at)


for i, spec in enumerate(settles, 1):
    spec, _, cam = spec.partition("#")
    name, _, rest = spec.partition("=")
    s, _, hold = rest.partition("@")
    pose = basis(*map(float, cam.split(","))) if cam else ""
    out = data / "orig" / name
    out.mkdir(parents=True, exist_ok=True)
    t0 = time.time()
    with open(out / "capture.log", "w") as log:
        rc = subprocess.run([sys.executable, str(root / "re/tools/run_with_unlocked_save.py"), str(save), "--",
                             sys.executable, str(root / "re/frida/race_draw_burst.py"),
                             "--challenge", "3", "--settle", s, f"--hold={hold}", f"--pose={pose}", "--out", str(out / "a.bmp")],
                            stdout=log, stderr=subprocess.STDOUT).returncode
    trk = (out / "orig_track.txt").read_text().splitlines()[0] if (out / "orig_track.txt").exists() else "?"
    ok = (out / "a.bmp").exists() and (out / "orig_cambasis.txt").exists()
    print(f"[{i}/{len(settles)}] {name} hold={hold or '-'} settle={s} pose={cam or '-'} {'PASS' if ok else 'FAIL'} rc={rc} track={trk} {time.time() - t0:.0f}s", flush=True)
