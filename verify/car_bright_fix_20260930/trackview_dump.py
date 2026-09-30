# A4 support: deterministic orbit-camera stills of a track's WORLD (no car), so
# the terrain / sea / prop surfaces can be diffed pre-fix vs post-fix at
# bit-identical poses.
#
# MASHED_TRACK_VIEW=<piz> is the R4 fly-through (exe_main.cpp:893) -- it renders
# the cracked RW world through TrackRenderer with an auto-orbit camera and no
# vehicle, which is exactly what A4's terrain/sea guard wants: the car (the
# surface the fix is SUPPOSED to change) is not in the frame at all, so any
# nonzero diff is a non-car regression.
#
# MASHED_DETERMINISTIC=1 makes the orbit a pure function of the frame index, so
# frame N in the pre-fix run and frame N in the post-fix run are the same pose.
# MASHED_DBG_BBDUMP=N dumps the backbuffer at frame N (exe_main.cpp:3959),
# MASHED_DBG_BBDUMP_OUT names the file, MASHED_DET_FRAMES quits after N frames.
#
# Usage: py -3.12 verify/car_bright_fix_20260930/trackview_dump.py <exe> <piz> <outdir> <frame,...>
import os, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
TIMEOUT = 120.0


def main():
    exe = Path(sys.argv[1]).resolve()
    piz = sys.argv[2]
    outdir = Path(sys.argv[3])
    frames = [int(v) for v in sys.argv[4].split(",")]
    outdir.mkdir(parents=True, exist_ok=True)
    for fr in frames:
        out = (outdir / f"f{fr:04d}.bmp").resolve()
        if out.exists():
            out.unlink()
        env = dict(os.environ)
        env.update({
            "MASHED_TRACK_VIEW": piz,
            "MASHED_DETERMINISTIC": "1",
            "MASHED_DBG_BBDUMP": str(fr),
            "MASHED_DBG_BBDUMP_OUT": str(out),
            "MASHED_DET_FRAMES": str(fr + 30),
            "MASHED_VERIFY_OUT": outdir.as_posix(),
            "MASHED_MUTE": "1",
            "MASHED_WIN_POS": "left-bl",
            "MASHED_TITLE": f"car-bright A4 trackview f{fr}",
        })
        env.pop("MASHED_NAV_DEMO", None)
        proc = subprocess.Popen([str(exe)], cwd=str(ROOT), env=env,
                                stdout=subprocess.DEVNULL,
                                stderr=subprocess.DEVNULL)
        t0 = time.time()
        while time.time() - t0 < TIMEOUT and proc.poll() is None:
            time.sleep(0.2)
        if proc.poll() is None:
            proc.kill()
            proc.wait(timeout=10)
            print(f"  f{fr}: TIMEOUT (killed pid {proc.pid})")
            continue
        print(f"  f{fr}: rc={proc.returncode} pid={proc.pid} "
              f"{'OK ' + str(out.stat().st_size) + 'B' if out.exists() else 'NO DUMP'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
