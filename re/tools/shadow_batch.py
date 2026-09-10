#!/usr/bin/env python3
"""shadow_batch.py -- boot groups of shadow A/B sites, bisect crashers, accumulate verdicts.

    py -3.12 re/tools/shadow_batch.py [--manifest re/parity/shadow_sites.tsv] [--group 16]
                                      [--hold 30] [--max-boots 12] [--rvas 0x..,0x..]
                                      [--out re/parity/shadow_results.tsv]

WHY A DRIVER
------------
scenario_launch.py --hooks <list> installs ONLY the listed hooks (MASHED_HOOK_ONLY) and arms
the shadow lane, so N sites sample in one boot. Two things make that unsafe to do by hand
over 100+ sites:
  * `--hooks all` wedges at phase 2 with 0 samples (2026-09-09), so groups must be bounded;
  * a C2 port that is WRONG in a crashing way (the thing this lane exists to find) takes the
    whole boot down with it, and then every other site in the group reports NO_SAMPLES.
This driver boots one group at a time, and when a boot dies before the race ran to its hold
it splits the group in two and retries each half, down to single sites. A site that crashes
alone is recorded CRASH -- that IS a finding about the port -- and never merged as clean.

PER BOOT
  1. rotate original/shadow_ab.log aside (keeps every raw log under log/shadow_ab/)
  2. run scenario_launch.py --hooks <group> --hold H    (mode 10, 1 player + AI by default)
  3. classify the boot: RACE_OK if the launcher printed its running-race verdict and the
     game was alive at the end of the hold; CRASH if "game exited." appeared; VOID otherwise
  4. shadow_ab_report.py over the rotated log -> per-site rows, merged into --out

The results TSV is the input to re-classify. This script promotes nothing.

PROCESS HYGIENE: scenario_launch spawns its own MASHED and kills only that PID; nothing here
touches other sessions' instances (CLAUDE.md, MASHED process hygiene).
"""
import argparse
import csv
import datetime as dt
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "re" / "parity" / "shadow_sites.tsv"
OUT = ROOT / "re" / "parity" / "shadow_results.tsv"
LOG = ROOT / "original" / "shadow_ab.log"
LOGDIR = ROOT / "log" / "shadow_ab"
LAUNCH = ROOT / "re" / "frida" / "scenario_launch.py"
REPORT = ROOT / "re" / "tools" / "shadow_ab_report.py"


def live_sites(manifest, restrict):
    # A manifest row is a CLAIM that a shadow site exists; the TU on disk is the fact.
    # Three rows (0x005ad2e0, 0x0045cc50, 0x00426030) said GENERATED:lane2-decomp2port while
    # their .cpp did not exist -- decomp2port.py's --prune-failed removed the file without
    # updating the row. Booting such a row burns a boot and records NO_SAMPLES, which then
    # reads as "the scenario never calls it" when the truth is "there is nothing installed".
    # That is precisely the false conclusion the 2026-09-10 prescreen correction came from, so
    # check the file rather than trusting the status.
    src_root = ROOT / "mashedmod" / "src" / "mashed_re"
    on_disk = {p.name for p in src_root.rglob("*.cpp")} if src_root.is_dir() else set()
    rows, phantom = [], []
    with open(manifest, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            if not r["status"].startswith(("GENERATED", "ALREADY")):
                continue
            if restrict and r["rva"] not in restrict:
                continue
            tu = (r.get("file") or "").strip()
            if tu and on_disk and tu not in on_disk:
                phantom.append((r["rva"], r.get("name", ""), tu))
                continue
            rows.append(r)
    for rva, name, tu in phantom:
        print(f"  [skip] {rva} {name}: manifest says {tu} but no such TU on disk "
              f"-- fix the row's status, do not boot it")
    return rows


def rotate_log(tag):
    LOGDIR.mkdir(parents=True, exist_ok=True)
    if LOG.exists():
        dst = LOGDIR / f"shadow_ab_{dt.datetime.now():%Y%m%d_%H%M%S}_pre_{tag}.log"
        shutil.move(str(LOG), str(dst))


def boot(rvas, hold, extra):
    """One scenario_launch boot. Returns (state, stdout_tail, saved_log_path)."""
    hooks = ",".join("0x" + r for r in rvas)
    tag = f"{len(rvas)}x_{rvas[0]}"
    rotate_log(tag)
    cmd = [sys.executable, str(LAUNCH), "--hooks", hooks, "--hold", str(hold)] + extra
    env = dict(os.environ)
    env.setdefault("MASHED_MUTE", "1")            # feedback_always_launch_muted
    if os.environ.get("SHADOW_BATCH_NO_SHADOW"):
        env["MASHED_NO_SELFTEST"] = "1"          # control: hooks live, A/B disarmed
        env.pop("MASHED_SHADOW_AB", None)
    else:
        env["MASHED_SHADOW_AB"] = "1"
    t0 = time.time()
    # Snapshot MASHED PIDs before the launch: whatever is NEW and still alive afterwards was
    # spawned by this boot and may be killed. Never kill by name (other sessions' games).
    def mashed_pids():
        txt = subprocess.run(["tasklist", "/FI", "IMAGENAME eq MASHED.exe", "/NH", "/FO", "CSV"],
                             capture_output=True, text=True).stdout
        return {int(x.split(",")[1].strip('"')) for x in txt.splitlines() if x.startswith('"MASHED.exe"')}
    before = mashed_pids()
    try:
        p = subprocess.run(cmd, cwd=str(ROOT), env=env, capture_output=True, text=True,
                           timeout=hold + 300, errors="replace")
        out = p.stdout + p.stderr
    except subprocess.TimeoutExpired as ex:
        out = ((ex.stdout or "") + (ex.stderr or "")) if isinstance(ex.stdout, str) else ""
        out += "\n[shadow_batch] launcher TIMEOUT"
    # A HUNG game (deadlock under the tracker) outlives the launcher: kill ONLY the PID the
    # launcher printed for this boot (never by name -- other sessions' games coexist), then
    # classify. The rotate is tolerant of the log still being open for a moment.
    hung = False
    m_pid = re.search(r"scenario_launch\s+pid=(\d+)", out)
    new_alive = mashed_pids() - before
    if m_pid:
        new_alive.add(int(m_pid.group(1)))
    for pid in sorted(new_alive):
        alive = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/NH"], capture_output=True,
                               text=True).stdout
        if "MASHED.exe" in alive:
            hung = True
            subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True)
            time.sleep(1.0)
    saved = None
    stamp = f"{dt.datetime.now():%Y%m%d_%H%M%S}"
    if LOG.exists():
        saved = LOGDIR / f"shadow_ab_{stamp}_{tag}.log"
        for _try in range(5):
            try:
                shutil.move(str(LOG), str(saved))
                break
            except PermissionError:
                time.sleep(1.0)
        else:
            shutil.copy(str(LOG), str(saved))
    # keep the launcher's own output next to the shadow log: a VOID boot with no launcher
    # transcript cannot be diagnosed (prescreen_batch.py learned the same lesson).
    (LOGDIR / f"launch_{stamp}_{tag}.txt").write_text(out, encoding="utf-8", errors="replace")
    # "script has been destroyed" = the Frida agent's process died under it, i.e. the game
    # crashed before the launcher's own exit check ran (seen at ~10 s with region sites, run 4)
    # "game exited while waiting for menu" = died at boot with the hooks installed: a port that
    # runs at init is wrong (Lane 2 first batch, 2026-09-10). That is a CRASH, not a harness VOID.
    if hung:
        state = "HUNG"            # process alive but stuck after the launcher gave up
    elif ("game exited." in out or "TIMEOUT" in out or "script has been destroyed" in out
            or "game exited while waiting for" in out):
        state = "CRASH"           # the game died with these hooks installed
    elif "reached a running race" in out:
        state = "RACE_OK"
    else:
        state = "VOID"            # harness problem (attach failed, nav stalled): not about the hooks
    tail = "\n".join(out.strip().splitlines()[-6:])
    return state, tail, saved, round(time.time() - t0)


def report_rows(saved, rvas):
    if not saved:
        return {}
    tmp = saved.with_suffix(".tsv")
    subprocess.run([sys.executable, str(REPORT), str(saved), "--only", ",".join(rvas),
                    "--tsv", str(tmp), "--quiet"], cwd=str(ROOT), capture_output=True, text=True)
    rows = {}
    if tmp.exists():
        with open(tmp, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                rows[r["rva"]] = r
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--manifest", default=str(MANIFEST))
    ap.add_argument("--out", default=str(OUT))
    ap.add_argument("--rvas", default="", help="restrict to these RVAs (comma list)")
    ap.add_argument("--group", type=int, default=16, help="sites per boot before bisection")
    ap.add_argument("--hold", type=int, default=30, help="seconds in-race per boot")
    ap.add_argument("--max-boots", type=int, default=12)
    ap.add_argument("--skip-done", action="store_true", help="skip RVAs already in --out with a non-VOID verdict")
    ap.add_argument("--launch-arg", action="append", default=[], help="extra arg passed to scenario_launch.py")
    ap.add_argument("--no-shadow", action="store_true",
                    help="CONTROL: install the hooks live but do not arm the A/B (MASHED_NO_SELFTEST=1). "
                         "Verdicts are then only RACE_OK/CRASH -- separates a crashing PORT from a crashing harness")
    args = ap.parse_args()
    if args.no_shadow:
        os.environ["SHADOW_BATCH_NO_SHADOW"] = "1"

    restrict = {x.strip().lower().replace("0x", "").zfill(8) for x in args.rvas.split(",") if x.strip()}
    sites = live_sites(args.manifest, restrict)
    done = {}
    outp = Path(args.out)
    if not outp.is_absolute():
        outp = ROOT / outp
    if outp.exists():
        with open(outp, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                done[r["rva"]] = r
    if args.skip_done:
        sites = [s for s in sites if done.get(s["rva"], {}).get("verdict", "VOID") in ("VOID", "NO_SAMPLES")]
    rvas = [s["rva"] for s in sites]
    names = {s["rva"]: s["name"] for s in sites}
    print(f"sites={len(rvas)} group={args.group} hold={args.hold}s max_boots={args.max_boots}")
    if not rvas:
        return 0

    queue = [rvas[i:i + args.group] for i in range(0, len(rvas), args.group)]
    boots = 0
    results = {}
    retried = {}
    while queue and boots < args.max_boots:
        grp = queue.pop(0)
        boots += 1
        print(f"\n[boot {boots}] {len(grp)} site(s): {', '.join(grp[:6])}{' ...' if len(grp) > 6 else ''}")
        state, tail, saved, secs = boot(grp, args.hold, args.launch_arg)
        print(f"  -> {state} in {secs}s   log={saved.name if saved else '-'}")
        rows = report_rows(saved, grp)
        stamp = dt.datetime.now().isoformat(timespec="seconds")
        if state == "RACE_OK":
            for r in grp:
                row = rows.get(r, {"rva": r, "name": names.get(r, ""), "verdict": "NO_SAMPLES",
                                   "n": 0, "ndiff": 0, "proof": "", "detail": ""})
                row.update(boot_state=state, group=len(grp), log=saved.name if saved else "", at=stamp)
                results[r] = row
                print(f"     {row['verdict']:<10} {r} {row['name']:<30} n={row['n']} ndiff={row['ndiff']}")
        elif state == "VOID" and retried.get(tuple(grp), 0) < 1:
            # a harness failure says nothing about the ports: retry the same group once
            retried[tuple(grp)] = 1
            queue.insert(0, grp)
            print(f"  VOID (harness) -> retry once   ({tail.splitlines()[-1] if tail else ''})")
        elif len(grp) > 1:
            half = len(grp) // 2
            queue[:0] = [grp[:half], grp[half:]]
            print(f"  bisect -> {half} + {len(grp) - half}   ({tail.splitlines()[-1] if tail else ''})")
            # whatever sampled before the crash is still informative for DIVERGENT rows
            for r, row in rows.items():
                if row["verdict"] == "DIVERGENT" and r not in results:
                    row.update(boot_state=state, group=len(grp), log=saved.name if saved else "", at=stamp)
                    results[r] = row
        else:
            r = grp[0]
            row = rows.get(r, {"rva": r, "name": names.get(r, ""), "verdict": "", "n": 0,
                               "ndiff": 0, "proof": "", "detail": ""})
            row["verdict"] = state if state in ("CRASH", "HUNG") else "VOID"
            row.update(boot_state=state, group=1, log=saved.name if saved else "", at=stamp,
                       detail=(row.get("detail") or "") + " | " + tail.replace("\n", " / ")[-160:])
            results[r] = row
            print(f"     {row['verdict']:<10} {r} {row['name']}")
    if queue:
        print(f"\nbudget exhausted: {sum(len(g) for g in queue)} site(s) not booted")

    # ── RE-READ the results file before merging: a long batch must not clobber edits
    #    made while it was running ────────────────────────────────────────────────────
    # `done` was loaded ONCE at startup. A 31-boot sweep takes ~25 minutes, and anything
    # written to the TSV in that window -- by a reviewer correcting a verdict, or by another
    # session's batch -- is silently reverted when this run writes its stale snapshot back.
    # Hit for real on 2026-09-10: three rows re-verdicted INVALID_PORT mid-sweep were back to
    # CRASH the moment the sweep finished, losing the diagnosis with them.
    # Re-read now and keep the on-disk row for anything this run did not touch.
    if outp.exists():
        try:
            with open(outp, newline="", encoding="utf-8") as f:
                fresh = {r["rva"]: r for r in csv.DictReader(f, delimiter="\t")}
            changed = [k for k, v in fresh.items()
                       if k not in results and done.get(k) != v]
            for k, v in fresh.items():
                if k not in results:       # untouched by this run -> disk wins
                    done[k] = v
            if changed:
                print(f"  [merge] {len(changed)} row(s) changed on disk during this run; "
                      f"kept the on-disk version")
        except Exception as e:            # never lose a whole sweep to a merge problem
            print(f"  [merge] WARNING could not re-read {outp.name} ({e}); "
                  f"writing this run's snapshot")

    # ── control boots must NOT clobber an armed verdict ──────────────────────────────
    # --no-shadow sets MASHED_NO_SELFTEST=1, so a control boot logs NO samples by
    # construction and every site comes back NO_SAMPLES. Merging that like a normal result
    # overwrote the armed verdict the control was run to EXPLAIN: on 2026-09-10 an armed
    # CRASH on 0056f0a0 (14:34) became NO_SAMPLES (14:35) the moment its control ran, which
    # is how a real crasher silently reads as "never fired". In control mode we therefore
    # record the outcome in its own `control` column and leave verdict/n/ndiff/proof alone.
    control_mode = bool(os.environ.get("SHADOW_BATCH_NO_SHADOW"))
    if control_mode:
        for r, row in results.items():
            prev = done.get(r)
            if prev:
                prev["control"] = row.get("boot_state", "")
                prev["control_at"] = row.get("at", "")
            else:
                row["control"] = row.get("boot_state", "")
                row["control_at"] = row.get("at", "")
                row["verdict"] = ""          # no armed verdict exists yet; do not invent one
                done[r] = row
        print("  [control] recorded in the `control` column; armed verdicts left untouched")
    else:
        for r, row in results.items():
            prev = done.get(r)
            if prev:                          # carry any earlier control result forward
                row.setdefault("control", prev.get("control", ""))
                row.setdefault("control_at", prev.get("control_at", ""))
        done.update(results)
    fields = ["rva", "name", "verdict", "n", "ndiff", "proof", "detail", "boot_state", "group",
              "log", "at", "control", "control_at"]
    with open(outp, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fields, delimiter="\t", extrasaction="ignore")
        w.writeheader()
        for k in sorted(done):
            w.writerow({fld: done[k].get(fld, "") for fld in fields})
    import collections
    # In control mode the per-site `verdict` is meaningless (no samples are logged), so tally
    # the boot outcome instead -- that IS the control's result.
    key = "boot_state" if control_mode else "verdict"
    tally = collections.Counter(r.get(key, "") for r in results.values())
    label = "control" if control_mode else "this run"
    print(f"\n{label}: {boots} boot(s)  " + "  ".join(f"{k}={v}" for k, v in sorted(tally.items())))
    print(f"results -> {outp.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
