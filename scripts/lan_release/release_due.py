"""release_due.py - is a new LAN release of mashed_re.exe due?

The roadmap loop runs this after every fast-forward of race/first-frame-parity (CLAUDE.md "LAN release",
.claude/skills/roadmap-loop/SKILL.md "Stop conditions") and publishes with publish.sh when it prints DUE. The build
on the mini-server is the truth: its manifest's label names the commit it was built from ("Mashed RE <sha>
(<date>)"), so no local state is kept.

Rule (Mariano, 2026-10-29): rebuild and upload after a few commits. A commit here is a first-parent commit on
race/first-frame-parity since the published one that changes what the build archives (mashedmod/), so one merged
roadmap step counts once however many commits its branch had. DUE at ROUNDS (3) such commits, or at 1 once the
published build is MAX_AGE (24) hours old.

  py -3.12 scripts/lan_release/release_due.py [--ref race/first-frame-parity] [--rounds 3] [--max-age-hours 24]

Prints one line, DUE / NOT_DUE / ERROR with the reason, then the counted commits. Exit 0 = due, 1 = not due, 2 = error.
"""
import argparse
import datetime
import json
import re
import subprocess
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SERVERS = ("http://mariano-server.local:8088/mashed", "http://192.168.68.112:8088/mashed")
BUILD_PATHS = ("mashedmod",)  # what publish.sh archives and builds


def published_manifest():
    for base in SERVERS:
        try:
            with urllib.request.urlopen(base + "/manifest.json", timeout=15) as r:
                return json.load(r), base
        except Exception:
            continue
    return None, None


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, check=True).stdout


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ref", default="race/first-frame-parity")
    ap.add_argument("--rounds", type=int, default=3)
    ap.add_argument("--max-age-hours", type=float, default=24.0)
    a = ap.parse_args()

    man, base = published_manifest()
    if man is None:
        print("DUE (nothing published yet: no server answered with manifest.json at %s)" % ", ".join(SERVERS))
        return 0
    m = re.search(r"Mashed RE ([0-9a-f]{7,40})", man.get("label", ""))
    if not m:
        print("ERROR the published label names no commit: %r" % man.get("label"))
        return 2
    sha = m.group(1)
    try:
        commits = git("log", "--first-parent", "--format=%h %s", "%s..%s" % (sha, a.ref), "--",
                      *BUILD_PATHS).splitlines()
    except subprocess.CalledProcessError as e:
        print("ERROR git log %s..%s failed: %s" % (sha, a.ref, e.stderr.strip()))
        return 2
    age_h = None
    try:
        gen = datetime.datetime.strptime(man.get("generated", ""), "%Y-%m-%d %H:%M")
        age_h = (datetime.datetime.now() - gen).total_seconds() / 3600
    except ValueError:
        pass

    n = len(commits)
    head = git("rev-parse", "--short=9", a.ref).strip()
    where = "published %s (%s, %s h old), %s %s, %d commit(s) since" % (
        sha, man.get("generated", "?"), "?" if age_h is None else "%.1f" % age_h, a.ref, head, n)
    if n >= a.rounds:
        verdict, rc = "DUE (%d >= %d commits)" % (n, a.rounds), 0
    elif n >= 1 and age_h is not None and age_h >= a.max_age_hours:
        verdict, rc = "DUE (%d commit(s), build older than %g h)" % (n, a.max_age_hours), 0
    else:
        verdict, rc = "NOT_DUE (need %d commits, or 1 after %g h)" % (a.rounds, a.max_age_hours), 1
    print("%s: %s" % (verdict, where))
    for c in commits:
        print("  " + c[:110])
    return rc


if __name__ == "__main__":
    sys.exit(main())
