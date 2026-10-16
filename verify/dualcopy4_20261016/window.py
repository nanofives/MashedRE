"""Shared-.asi deploy window (parent standing rule: backup + no-foreign-MASHED check + restore in a finally), dualcopy4 copy of
verify/dupinstall2_20261016/window.py with the asi under test chosen on the command line.

    py -3.12 window.py <outdir> <steps.json> [--asi <path to the .asi to deploy>]

steps.json: [{"name": "...", "cmd": ["py", "-3.12", "script.py", ...], "env": {"K": "V"}, "timeout": 300}, ...]
`{OUT}` inside cmd/env values is replaced by the absolute output dir, `{ASI}` by the shared .asi path. Each step's stdout+stderr goes to
<outdir>/<name>.txt; <outdir>/window.log has one line per step.

  1. no MASHED.exe may exist when the window opens (polled up to WAIT_FOREIGN_S, then refuse; nothing is killed);
  2. original\\mashed_re_dev.asi is copied to ...\\mashed_re_dev.asi.dualcopy4_bak, SHA-256 recorded;
  3. the asi under test (default: this worktree's build) is copied over (a loaded DLL cannot be overwritten: a foreign MASHED -> abort);
  4. the steps run;
  5. finally: restore, compare SHA-256, delete the backup.
"""
import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
MAIN_ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original")
SHARED = MAIN_ORIG / "mashed_re_dev.asi"
BAK = MAIN_ORIG / "mashed_re_dev.asi.dualcopy4_bak"
WAIT_FOREIGN_S = int(os.environ.get("WINDOW_WAIT_S", "900"))


def sha(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def mashed_pids() -> list[int]:
    r = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe", "/FO", "CSV", "/NH"], capture_output=True, text=True)
    pids = []
    for ln in r.stdout.splitlines():
        parts = [x.strip('"') for x in ln.split('","')]
        if len(parts) >= 2 and parts[0].strip('"').lower() == "mashed.exe":
            try:
                pids.append(int(parts[1]))
            except ValueError:
                pass
    return pids


def sub(v, out: Path):
    return v.replace("{OUT}", str(out)).replace("{ASI}", str(SHARED)) if isinstance(v, str) else v


def main() -> int:
    out = Path(sys.argv[1]).resolve()
    steps = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
    build = WT / "mashedmod" / "build" / "mashed_re_dev.asi"
    if "--asi" in sys.argv:
        build = Path(sys.argv[sys.argv.index("--asi") + 1]).resolve()
    out.mkdir(parents=True, exist_ok=True)
    log = (out / "window.log").open("a", encoding="utf-8")

    def say(m: str) -> None:
        line = time.strftime("%H:%M:%S ") + m
        print(line, flush=True)
        log.write(line + "\n")
        log.flush()

    t0 = time.time()
    while mashed_pids():
        if time.time() - t0 > WAIT_FOREIGN_S:
            say(f"REFUSE: foreign MASHED.exe still running after {WAIT_FOREIGN_S}s: {mashed_pids()}")
            return 3
        say(f"waiting: foreign MASHED.exe {mashed_pids()}")
        time.sleep(15)
    if BAK.exists():
        say(f"REFUSE: stale backup {BAK} exists; inspect before continuing")
        return 4
    orig_hash = sha(SHARED)
    say(f"shared asi sha256 before = {orig_hash[:16]}  size={SHARED.stat().st_size}")
    shutil.copy2(SHARED, BAK)
    if sha(BAK) != orig_hash:
        say("REFUSE: backup hash mismatch")
        BAK.unlink()
        return 5
    rc = 0
    try:
        try:
            shutil.copy2(build, SHARED)
        except OSError as e:
            say(f"ABORT: cannot overwrite the shared asi (a foreign MASHED has it mapped?): {e}")
            return 6
        say(f"deployed {build.name} sha256 = {sha(SHARED)[:16]}")
        for s in steps:
            env = dict(os.environ)
            for k, v in (s.get("env") or {}).items():
                env[k] = sub(v, out)
            cmd = [sub(c, out) for c in s["cmd"]]
            t1 = time.time()
            try:
                p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=int(s.get("timeout", 300)))
                (out / f"{s['name']}.txt").write_text(p.stdout + "\n" + p.stderr, encoding="utf-8")
                say(f"{s['name']:42s} rc={p.returncode} ({time.time()-t1:.0f}s)")
            except subprocess.TimeoutExpired as e:
                (out / f"{s['name']}.txt").write_text(str(e.stdout or "") + "\nTIMEOUT", encoding="utf-8")
                say(f"{s['name']:42s} TIMEOUT ({time.time()-t1:.0f}s)")
    finally:
        for attempt in range(10):
            try:
                shutil.copy2(BAK, SHARED)
                break
            except OSError as e:
                say(f"restore attempt {attempt}: {e}")
                time.sleep(3)
        back = sha(SHARED)
        say(f"shared asi sha256 after restore = {back[:16]}  matches original: {back == orig_hash}")
        if back == orig_hash:
            BAK.unlink()
        else:
            say(f"RESTORE FAILED: backup kept at {BAK}")
            rc = 8
    return rc


if __name__ == "__main__":
    raise SystemExit(main())
