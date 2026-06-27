#!/usr/bin/env py -3.12
"""diag.py — Mashed RE diagnosis + self-heal environment.

ONE place to (a) auto-heal the recurring environment failures and (b) capture every
execution error with full context so it's never re-investigated. Catalog narrative:
re/diag/KNOWN_ISSUES.md. The SIGNATURES table below is the machine-readable source.

Subcommands:
  doctor                 all health checks; auto-heal the SAFE ones; diagnose the
                         rest; write re/diag/doctor_report.md . Use at session start
                         or any time the environment feels wrong.
  heal                   only the auto-heal actions (no full report)
  run --tag T -- CMD...  run CMD, capture rc/stdout/stderr/env/timing to
                         re/diag/runs/<tag>_<ts>.log, scan output vs the catalog,
                         print any matched known issue + its fix. Exits with CMD's rc.
  probe-render           spawn mashed_re.exe muted ~12 s; report if D3D9 can create a
                         device in THIS context (display/lock state)
  scan FILE              scan an existing log file against the signature catalog
  issues                 print the known-issue catalog

Usage:  py -3.12 scripts/diag.py doctor
        py -3.12 scripts/diag.py run --tag build -- cmd /c mashedmod\\build.bat
"""
import argparse
import datetime
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DIAG = os.path.join(ROOT, "re", "diag")
RUNS = os.path.join(DIAG, "runs")

# ── signature catalog (keep IDs in sync with re/diag/KNOWN_ISSUES.md) ──────────
# pattern: regex matched against captured output; heal: auto-fix hint (advisory —
# the actual fix lives in doctor's checks); fallback: manual recovery.
SIGNATURES = [
    dict(id="D3D9-NOTAVAIL",
         pattern=r"CreateDevice\(.*hr=0x8876086[AC]",
         cause="No presentable desktop (screen locked / display asleep / background spawn).",
         heal="none (cannot unlock screen in code)",
         fallback="run on an active unlocked desktop; or use offline numeric proof for renderer correctness."),
    dict(id="FRIDA-ATTACH-FLAKE",
         pattern=r"refused to load frida-agent|terminated during injection|VirtualAllocEx.*0x5",
         cause="Frida spawn perturbs MASHED layout -> transient boot AV.",
         heal="retry the diff up to 3x",
         fallback="attach-after-boot (re/frida/record_attach.py); never blanket-kill MASHED."),
    dict(id="FRIDA-TIMEOUT",
         pattern=r"\bTIMEOUT\b|rc=3\b.*6[0-9]\.\ds",
         cause="run_diff hung ~61s (state-touching/destructive fn, or spawn flake).",
         heal="retry once; if persistent, queue the candidate",
         fallback="exclude destructive/teardown fns; scenario-attach."),
    dict(id="GHIDRA-STALE-LOCK",
         pattern=r"LockException|\.lock~.*(?:busy|resource)|Slot \d+: LOCKED",
         cause="Crashed Ghidra MCP left a stale project lock.",
         heal="release locked slots when no java.exe runs",
         fallback="read via xtwin.py / dump_asm.py instead of Ghidra."),
    dict(id="WF-CRLF",
         pattern=r"contains control characters that would be hidden",
         cause="Workflow .js has CRLF; the \\r are control chars.",
         heal="normalize CRLF->LF in re/pipeline/*.js",
         fallback="py strip \\r then re-invoke Workflow."),
    dict(id="BUILD-FAIL",
         pattern=r"\berror C\d{4}\b|fatal error|LINK : fatal|: error LNK",
         cause="MSVC compile/link error in the standalone or .asi.",
         heal="none (real code error)",
         fallback="read the cited error C#### line; the .asi->original copy step failing in a worktree (no original/) is BENIGN."),
    dict(id="MASHED-BOOT-AV",
         pattern=r"MASHED\.exe.*\.dmp|ntdll.*\+0x542f0|ECX=0x5477",
         cause="Original boot AV (CRT static-init / fopen / joypad / EMULATEHEAP class).",
         heal="none (binary patches are not auto-applied)",
         fallback="scripts/parse_minidump.py; apply the relevant scripts/patch_mashed_*; setup_mashed_compat.ps1."),
]


def now():
    return datetime.datetime.now().strftime("%Y%m%d-%H%M%S")


def sh(cmd, **kw):
    """Run a list-cmd, return (rc, combined_output). Never raises."""
    try:
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                           errors="replace", **kw)
        return p.returncode, (p.stdout or "") + (p.stderr or "")
    except Exception as e:
        return 1, f"[diag: failed to run {cmd}: {e}]"


def procs(name):
    rc, out = sh(["tasklist", "/fi", f"imagename eq {name}", "/fo", "csv", "/nh"])
    return [l for l in out.splitlines() if name.split(".")[0].lower() in l.lower()]


def git(*args):
    rc, out = sh(["git", *args])
    return out.strip()


def scan_text(text):
    """Return list of matched signature dicts for the given text."""
    hits = []
    for s in SIGNATURES:
        if re.search(s["pattern"], text, re.IGNORECASE):
            hits.append(s)
    return hits


# ── health checks: each returns (name, status, detail, healed) ────────────────
def check_crlf(heal):
    import glob
    targets = glob.glob(os.path.join(ROOT, "re", "pipeline", "*.js")) + \
              glob.glob(os.path.join(ROOT, "re", "pipeline", "*.workflow.js"))
    bad = []
    for p in targets:
        b = open(p, "rb").read()
        if b"\r" in b:
            bad.append(p)
            if heal:
                open(p, "wb").write(b.replace(b"\r\n", b"\n").replace(b"\r", b"\n"))
    if not targets:
        return ("crlf", "OK", "no pipeline .js", False)
    if bad:
        rel = ", ".join(os.path.relpath(p, ROOT) for p in bad)
        return ("crlf", "HEALED" if heal else "WARN", f"CRLF in {rel}", heal)
    return ("crlf", "OK", "all LF", False)


def check_worktrees(heal):
    # SAFETY (2026-06-27 incident): worktrees created by the `worktree` skill
    # SYMLINK `original/` (the immutable game install) into themselves. `git
    # worktree remove --force` follows that symlink and DELETES THE REAL original/
    # assets. This wiped the anchor + all .piz once. Worktree pruning is therefore
    # DIAGNOSE-ONLY here — NEVER auto-removed. Prune manually after FIRST deleting
    # the worktree's `original` symlink (see KNOWN_ISSUES ORPHAN-WORKTREE).
    heal = False
    out = git("worktree", "list", "--porcelain")
    wts = []
    cur = {}
    for line in out.splitlines():
        if line.startswith("worktree "):
            if cur:
                wts.append(cur)
            cur = {"path": line.split(" ", 1)[1]}
        elif line.startswith("branch "):
            cur["branch"] = line.split("/")[-1]
    if cur:
        wts.append(cur)
    main_path = os.path.normcase(os.path.normpath(ROOT))
    pruned, flagged = [], []
    for w in wts:
        if os.path.normcase(os.path.normpath(w["path"])) == main_path:
            continue
        br = w.get("branch", "")
        # commits beyond main + uncommitted changes in the worktree
        ahead = git("log", "--oneline", f"main..{br}") if br else ""
        n_ahead = len([x for x in ahead.splitlines() if x.strip()])
        dirty = ""
        if os.path.isdir(w["path"]):
            rc, dirty = sh(["git", "-C", w["path"], "status", "--porcelain"])
        safe = (n_ahead == 0) and not dirty.strip()
        if safe:
            if heal:
                sh(["git", "worktree", "remove", "--force", w["path"]])
                if br:
                    sh(["git", "branch", "-D", br])
            pruned.append(br or w["path"])
        else:
            flagged.append(f"{br}({n_ahead} ahead{'+dirty' if dirty.strip() else ''})")
    if heal:
        sh(["git", "worktree", "prune"])
    detail = []
    if pruned:
        detail.append(("pruned " if heal else "prunable ") + ", ".join(pruned))
    if flagged:
        detail.append("KEPT (unmerged work): " + ", ".join(flagged))
    if not pruned and not flagged:
        return ("worktrees", "OK", "none orphaned", False)
    status = "HEALED" if (heal and pruned) else ("WARN" if flagged else "WARN")
    return ("worktrees", status, "; ".join(detail), bool(heal and pruned))


def check_ghidra_locks(heal):
    rc, out = sh(["pwsh", os.path.join("scripts", "ghidra_pool.ps1"), "status"])
    locked = [m.group(1) for m in re.finditer(r"Slot (\d+): LOCKED", out)]
    jvm = procs("java.exe")
    if not locked:
        return ("ghidra_locks", "OK", "no locked slots", False)
    if jvm:
        return ("ghidra_locks", "WARN",
                f"slots {locked} LOCKED but {len(jvm)} java.exe running — left alone (may be live)", False)
    # stale: no JVM -> safe to release
    if heal:
        for s in locked:
            sh(["pwsh", os.path.join("scripts", "ghidra_pool.ps1"), "release", s])
    return ("ghidra_locks", "HEALED" if heal else "WARN",
            f"slots {locked} LOCKED, no JVM (stale){' -> released' if heal else ' -> releasable'}", heal)


def check_frida_pool(heal):
    if heal:
        rc, out = sh(["bash", os.path.join("scripts", "frida_pool.sh"), "cleanup"])
        if "[diag: failed" in out:
            return ("frida_pool", "SKIP", "bash unavailable; run scripts/frida_pool.sh cleanup manually", False)
        return ("frida_pool", "HEALED", "cleaned (locks + tracked zombie MASHED)", True)
    return ("frida_pool", "OK", "run `heal` to cleanup", False)


def check_verify_scratch(heal):
    out = git("status", "--porcelain", "--untracked-files=no")
    files = [l[3:] for l in out.splitlines() if l.strip()]
    if not files:
        return ("verify_scratch", "OK", "tree clean", False)
    nonverify = [f for f in files if not re.match(r"verify/.*\.(bmp|png)$", f)]
    if nonverify:
        return ("verify_scratch", "WARN",
                f"{len(files)} dirty incl non-verify ({nonverify[:3]}) — left for review", False)
    # only verify images dirty -> safe to restore
    if heal:
        sh(["git", "checkout", "--", "verify/"])
    return ("verify_scratch", "HEALED" if heal else "WARN",
            f"{len(files)} verify scratch image(s){' restored' if heal else ' restorable'}", heal)


def check_render_capability(_heal):
    """Diagnose-only: read the most recent standalone D3D9 result."""
    log = os.path.join(ROOT, "mashed_re.log")
    if not os.path.exists(log):
        return ("render", "UNKNOWN", "no mashed_re.log yet — run `diag.py probe-render`", False)
    txt = open(log, errors="replace").read()
    m = re.findall(r"CreateDevice\(.*hr=0x([0-9A-Fa-f]+)", txt)
    if not m:
        if "device" in txt.lower() and "ready" in txt.lower():
            return ("render", "OK", "last run created a device", False)
        return ("render", "UNKNOWN", "no CreateDevice line in last log", False)
    last = m[-1].upper()
    if last.startswith("8876086"):
        return ("render", "BLOCKED",
                f"D3DERR hr=0x{last} (D3D9-NOTAVAIL): display locked/asleep or background spawn. "
                "Renders need an ACTIVE UNLOCKED desktop; use offline proof otherwise.", False)
    return ("render", "OK", f"last CreateDevice hr=0x{last}", False)


def check_zombie_games(_heal):
    # diagnose-only: never auto-kill (multi-session). Report counts.
    m, mr = procs("MASHED.exe"), procs("mashed_re.exe")
    if not m and not mr:
        return ("game_procs", "OK", "no game processes", False)
    return ("game_procs", "WARN",
            f"{len(m)} MASHED.exe + {len(mr)} mashed_re.exe running — "
            "kill ONLY ones you spawned (multi-session rule)", False)


def check_original_intact(_heal):
    """Diagnose-only: is the immutable original/ install present? Catches the
    WORKTREE-SYMLINK-WIPE incident INSTANTLY instead of after asset-load failures."""
    od = os.path.join(ROOT, "original")
    anchor = os.path.join(od, "MASHED.exe.unpatched")
    toast = os.path.join(od, "TOASTART")
    if not os.path.isdir(od) or not os.listdir(od):
        return ("original", "BLOCKED",
                "original/ is EMPTY or missing — game install wiped/not present. "
                "Restore from the install archive, then re-apply scripts/patch_mashed_* + shim.", False)
    miss = [n for n, p in (("MASHED.exe.unpatched", anchor), ("TOASTART/", toast))
            if not os.path.exists(p)]
    if miss:
        return ("original", "BLOCKED", f"original/ present but MISSING {miss} — incomplete install.", False)
    return ("original", "OK", "anchor + TOASTART present", False)


def is_reparse(p):
    """True if p is a junction/symlink (reparse point) — must NOT be recursed into."""
    try:
        import stat as _stat
        st = os.lstat(p)
        return bool(getattr(st, "st_file_attributes", 0) & _stat.FILE_ATTRIBUTE_REPARSE_POINT) \
            or os.path.islink(p)
    except OSError:
        return False


CHECKS = [check_original_intact, check_crlf, check_worktrees, check_ghidra_locks,
          check_frida_pool, check_verify_scratch, check_render_capability,
          check_zombie_games]


def cmd_doctor(args):
    heal = not args.no_heal
    os.makedirs(DIAG, exist_ok=True)
    rows = [c(heal) for c in CHECKS]
    lines = [f"# diag doctor report — {now()}", "",
             f"mode: {'HEAL' if heal else 'DIAGNOSE-ONLY'}", ""]
    icon = {"OK": "ok", "HEALED": "HEAL", "WARN": "!!", "BLOCKED": "XX", "SKIP": "--", "UNKNOWN": "??"}
    for name, status, detail, _healed in rows:
        lines.append(f"- [{icon.get(status,'?')}] {name:14} {status:8} {detail}")
    report = "\n".join(lines) + "\n"
    open(os.path.join(DIAG, "doctor_report.md"), "w", encoding="utf-8").write(report)
    print(report)
    healed = [r for r in rows if r[1] == "HEALED"]
    blocked = [r for r in rows if r[1] in ("BLOCKED", "WARN")]
    print(f"summary: {len(healed)} healed, {len(blocked)} need attention. "
          f"report -> re/diag/doctor_report.md")
    return 0


def cmd_heal(args):
    args.no_heal = False
    for c in CHECKS:
        name, status, detail, healed = c(True)
        if status in ("HEALED", "WARN", "BLOCKED"):
            print(f"  {name}: {status} — {detail}")
    return 0


def cmd_run(args):
    os.makedirs(RUNS, exist_ok=True)
    cmd = args.cmd
    if not cmd:
        print("diag run: need -- <cmd...>", file=sys.stderr)
        return 2
    log = os.path.join(RUNS, f"{args.tag}_{now()}.log")
    start = datetime.datetime.now()
    env_snap = {k: v for k, v in os.environ.items() if k.startswith("MASHED")}
    try:
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, errors="replace")
        rc, out = p.returncode, (p.stdout or "") + (p.stderr or "")
    except Exception as e:
        rc, out = 127, f"[diag: exec failed: {e}]"
    dur = (datetime.datetime.now() - start).total_seconds()
    hits = scan_text(out)
    with open(log, "w", encoding="utf-8") as f:
        f.write(f"# diag run  tag={args.tag}  rc={rc}  dur={dur:.1f}s  at={now()}\n")
        f.write(f"# cmd: {' '.join(cmd)}\n")
        f.write(f"# cwd: {ROOT}\n")
        f.write(f"# env(MASHED_*): {env_snap}\n")
        if hits:
            f.write(f"# MATCHED KNOWN ISSUES: {', '.join(h['id'] for h in hits)}\n")
        f.write("# " + "-" * 70 + "\n")
        f.write(out)
    # index row
    with open(os.path.join(RUNS, "index.tsv"), "a", encoding="utf-8") as ix:
        ix.write(f"{now()}\t{args.tag}\t{rc}\t{dur:.1f}\t{','.join(h['id'] for h in hits)}\t{os.path.basename(log)}\n")
    sys.stdout.write(out)
    if hits:
        print("\n=== diag: matched known issue(s) ===", file=sys.stderr)
        for h in hits:
            print(f"  [{h['id']}] {h['cause']}\n     heal: {h['heal']}\n     fallback: {h['fallback']}", file=sys.stderr)
    print(f"\n[diag] captured -> {os.path.relpath(log, ROOT)} (rc={rc}, {dur:.1f}s)", file=sys.stderr)
    return rc


def cmd_probe_render(args):
    """Spawn mashed_re.exe muted briefly; report D3D9 device capability."""
    exe = os.path.join(ROOT, "mashedmod", "build", "mashed_re.exe")
    if not os.path.exists(exe):
        print("probe-render: build mashed_re.exe first (cmd /c mashedmod\\build.bat)")
        return 2
    log = os.path.join(ROOT, "mashed_re.log")
    try:
        os.remove(log)
    except OSError:
        pass
    env = dict(os.environ, MASHED_MUTE="1")
    print("probe-render: spawning mashed_re.exe muted ~12s ...")
    try:
        p = subprocess.Popen([exe], cwd=ROOT, env=env)
        import time
        time.sleep(12)
        p.terminate()
    except Exception as e:
        print(f"probe-render: spawn failed: {e}")
        return 1
    name, status, detail, _ = check_render_capability(False)
    print(f"RENDER: {status} — {detail}")
    return 0 if status == "OK" else 1


def cmd_scan(args):
    if not os.path.exists(args.file):
        print(f"scan: {args.file} not found")
        return 2
    hits = scan_text(open(args.file, errors="replace").read())
    if not hits:
        print("no known issues matched.")
        return 0
    for h in hits:
        print(f"[{h['id']}] {h['cause']}\n  heal: {h['heal']}\n  fallback: {h['fallback']}")
    return 0


def cmd_wt_remove(args):
    """The ONLY sanctioned way to remove a worktree. NEVER uses --force.

    A worktree may contain a junction/symlink to an out-of-tree immutable dir
    (e.g. `original/` -> the game install). `git worktree remove --force` follows
    that link and deletes the TARGET (the WORKTREE-SYMLINK-WIPE incident, 2026-06-27).
    This strips every reparse point in the worktree FIRST (rmdir removes the link
    ONLY, never recurses into the target), THEN `git worktree remove` WITHOUT --force
    (which refuses on a dirty tree — a feature; resolve manually, don't force)."""
    wt = os.path.abspath(args.path)
    if not os.path.isdir(wt):
        print(f"wt-remove: {wt} not a directory")
        return 2
    stripped = []
    for name in os.listdir(wt):
        p = os.path.join(wt, name)
        if os.path.isdir(p) and is_reparse(p):
            # rmdir on a junction removes the link only; it does NOT touch the target.
            rc, out = sh(["cmd", "/c", "rmdir", p])
            stripped.append(name + ("" if rc == 0 else f"(FAILED:{out.strip()[:40]})"))
    if stripped:
        print(f"wt-remove: stripped reparse points (link-only, target safe): {stripped}")
    rc, out = sh(["git", "worktree", "remove", wt])   # NO --force, ever
    if rc != 0:
        print(f"wt-remove: `git worktree remove` refused (likely dirty):\n{out.strip()}\n"
              f"Resolve the worktree's changes and retry. DO NOT use --force "
              f"(it wipes junction targets like original/).")
        return rc
    print(f"wt-remove: removed {wt} safely (no --force).")
    sh(["git", "worktree", "prune"])
    return 0


def cmd_issues(args):
    for s in SIGNATURES:
        print(f"## {s['id']}\n  signature: /{s['pattern']}/\n  cause: {s['cause']}\n"
              f"  heal: {s['heal']}\n  fallback: {s['fallback']}\n")
    print("narrative: re/diag/KNOWN_ISSUES.md")
    return 0


def main():
    # a diagnostics tool must never itself die on Windows console encoding
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except Exception:
            pass
    ap = argparse.ArgumentParser(description="Mashed RE diagnosis + self-heal")
    sub = ap.add_subparsers(dest="sub", required=True)
    d = sub.add_parser("doctor"); d.add_argument("--no-heal", action="store_true")
    sub.add_parser("heal")
    r = sub.add_parser("run"); r.add_argument("--tag", default="run")
    r.add_argument("cmd", nargs=argparse.REMAINDER)
    sub.add_parser("probe-render")
    sc = sub.add_parser("scan"); sc.add_argument("file")
    sub.add_parser("issues")
    wr = sub.add_parser("wt-remove"); wr.add_argument("path")
    args = ap.parse_args()
    fn = {"doctor": cmd_doctor, "heal": cmd_heal, "run": cmd_run,
          "probe-render": cmd_probe_render, "scan": cmd_scan, "issues": cmd_issues,
          "wt-remove": cmd_wt_remove}[args.sub]
    # strip the leading "--" REMAINDER leaves on `run`
    if args.sub == "run" and args.cmd and args.cmd[0] == "--":
        args.cmd = args.cmd[1:]
    return fn(args)


if __name__ == "__main__":
    sys.exit(main())
