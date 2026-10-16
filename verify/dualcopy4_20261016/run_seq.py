"""run_seq.py <outdir> <steps.json> : run the steps one after the other WITHOUT touching the shared .asi (path1 only: run_diff.py
loads this worktree's mashedmod/build/mashed_re_dev.asi and spawns the original with MASHED_RE_NO_AUTO_HOOK). Same step format and
output layout as window.py. A step is retried once when the original dies on a piz-open collision with another session (U-9677:
`0x005509b4` / `0x004e66db` signatures or no CSV written)."""
import json, os, subprocess, sys, time
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
out = Path(sys.argv[1]).resolve()
steps = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
out.mkdir(parents=True, exist_ok=True)
log = (out / "seq.log").open("a", encoding="utf-8")


def say(m):
    line = time.strftime("%H:%M:%S ") + m
    print(line, flush=True)
    log.write(line + "\n")
    log.flush()


for s in steps:
    for attempt in (1, 2):
        env = dict(os.environ)
        for k, v in (s.get("env") or {}).items():
            env[k] = v.replace("{OUT}", str(out))
        cmd = [c.replace("{OUT}", str(out)) for c in s["cmd"]]
        t0 = time.time()
        try:
            p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, timeout=int(s.get("timeout", 300)))
            txt = p.stdout + "\n" + p.stderr
            rc = p.returncode
        except subprocess.TimeoutExpired as e:
            txt, rc = str(e.stdout or "") + "\nTIMEOUT", -9
        (out / f"{s['name']}.txt").write_text(txt, encoding="utf-8")
        say(f"{s['name']:44s} rc={rc} attempt={attempt} ({time.time()-t0:.0f}s)")
        if rc == 0 or attempt == 2 or ("GREEN" in txt) or ("RED" in txt and "mismatches" in txt):
            break
        say("   no verdict: retrying once (piz collision / harness start failure class)")
