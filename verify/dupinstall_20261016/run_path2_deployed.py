"""Deploy the worktree .asi to the SHARED original\\mashed_re_dev.asi under the safety rule, run the after-state
install observation and path2 (run_verify_hook.py) for every surviving registry key, then restore.

Safety rule (parent standing rule 3, 2026-10-16): backup + no-foreign-MASHED check + restore in a finally.

  1. no MASHED.exe process may exist when we start (poll up to WAIT_FOREIGN_S, then refuse; we never kill
     a process we did not spawn);
  2. original\\mashed_re_dev.asi is copied to original\\mashed_re_dev.asi.dupinstall_bak and its SHA-256 recorded;
  3. the worktree build is copied over (a loaded DLL cannot be overwritten, so a foreign MASHED that still has
     the .asi mapped makes this step fail -> abort, nothing changed);
  4. the runs;
  5. finally: restore from the backup, verify the SHA-256 equals the recorded one, delete the backup.

    py -3.12 run_path2_deployed.py <outdir> <rvas27.txt> <key> [<key> ...]
"""
import hashlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

WT =Path(__file__).resolve().parent.parent.parent
MAIN_ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original")
SHARED = MAIN_ORIG / "mashed_re_dev.asi"
BAK = MAIN_ORIG / "mashed_re_dev.asi.dupinstall_bak"
BUILD = WT / "mashedmod" / "build" / "mashed_re_dev.asi"
WAIT_FOREIGN_S = 600


def sha(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def foreign() -> list[int]:
    """PIDs of every MASHED.exe (this runner spawns none of its own while it checks)."""
    r = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe", "/FO", "CSV", "/NH"],
                       capture_output=True, text=True)
    pids = []
    for ln in r.stdout.splitlines():
        parts = [x.strip('"') for x in ln.split('","')]
        if len(parts) >= 2 and parts[0].strip('"').lower() == "mashed.exe":
            try:
                pids.append(int(parts[1]))
            except ValueError:
                pass
    return pids


def main() -> int:
    out = Path(sys.argv[1]).resolve()     # absolute: MASHED_HOOK_MANIFEST is opened by the GAME, whose cwd is original\
    rvas = Path(sys.argv[2]).resolve()
    keys = sys.argv[3:]
    out.mkdir(parents=True, exist_ok=True)
    log = (out / "deploy.log").open("a", encoding="utf-8")

    def say(m: str) -> None:
        line = time.strftime("%H:%M:%S ") + m
        print(line, flush=True)
        log.write(line + "\n")
        log.flush()

    t0 = time.time()
    while foreign():
        if time.time() - t0 > WAIT_FOREIGN_S:
            say(f"REFUSE: foreign MASHED.exe still running after {WAIT_FOREIGN_S}s: {foreign()}")
            return 3
        say(f"waiting: foreign MASHED.exe {foreign()}")
        time.sleep(15)
    if BAK.exists():
        say(f"REFUSE: stale backup {BAK} exists (another run, or a crashed one); inspect before continuing")
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
            shutil.copy2(BUILD, SHARED)
        except OSError as e:
            say(f"ABORT: cannot overwrite the shared asi (a foreign MASHED has it mapped?): {e}")
            return 6
        say(f"deployed worktree build sha256 = {sha(SHARED)[:16]}")
        env = dict(os.environ)
        env["DUPINSTALL_OUT"] = str(out / "install_after")
        (out / "install_after").mkdir(exist_ok=True)
        cmd = [sys.executable, str(WT / "verify" / "dupinstall_20261016" / "install_observe.py"),
               ",".join(rvas.read_text().split()), "--seconds", "30"]
        p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=240)
        (out / "install_after.log").write_text(p.stdout + p.stderr, encoding="utf-8")
        say(f"install_observe rc={p.returncode}")
        env2 = dict(os.environ)
        env2["MASHED_VERIFY_ASI"] = str(SHARED)          # absolute: a relative value resolves against the worktree
        for k in keys:
            q = subprocess.run([sys.executable, str(WT / "re" / "frida" / "run_verify_hook.py"), k],
                               cwd=str(WT), env=env2, capture_output=True, text=True, timeout=240)
            (out / f"path2_{k}.txt").write_text(q.stdout + q.stderr, encoding="utf-8")
            verdict = "PASS" if q.returncode == 0 else f"rc={q.returncode}"
            say(f"path2 {k:34s} {verdict}")
    except subprocess.TimeoutExpired as e:
        say(f"TIMEOUT: {e}")
        rc = 7
    finally:
        # restore no matter what
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
