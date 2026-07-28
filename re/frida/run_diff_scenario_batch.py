#!/usr/bin/env py -3.12
# run_diff_scenario_batch.py — MANY hooks per ONE booted+navigated game.
#
# WHY THIS EXISTS
# ---------------
# run_diff_scenario.py is the only lane that can diff live-state (STATE) hooks,
# and its usage line is `<hook_name>` — SINGULAR. One boot per hook. Against a
# spawn budget of roughly 15 boots before the d3d9/GPU driver wedges
# (memory: feedback_d3d9_shim_wedges_gpu_driver), that caps a session at ~15
# STATE verifications no matter how many candidates are ready. Measured
# 2026-07-28: 947 of the 1416-row callee_gate_cascade backlog classify STATE,
# so this cap — not candidate supply, not the filters — is the binding
# constraint on C2->C3 throughput.
#
# run_diff_warm.py already proved many-hooks-per-process works (slot_worker
# loops hook names against one attached session, create_script/unload per hook)
# but it never navigates, so live globals are still zero and STATE hooks come
# back degenerate. This runner is the composition of the two: ONE spawn,
# run_diff_scenario's proven drive_to_results navigation, then N hooks force-
# called sequentially against that single populated process.
#
# THE OPEN QUESTION THIS ANSWERS
# ------------------------------
# Does force-calling hook N leave the live state clean enough for hook N+1?
# Synthetic A/B calls the ORIGINAL RVA with seeded arguments; a target with a
# side effect can perturb the very globals a later target reads. So the run is
# instrumented rather than assumed:
#   * the sentinel words are re-read AFTER every hook and compared to the
#     snapshot taken at the diff point;
#   * nav phase/depth are re-sampled (a hook that knocks the game out of the
#     race changes phase);
#   * with --repeat-first, hook #1 is run a SECOND time as the final hook.
#     Hook #1 GREEN at position 1 and RED/degenerate at the end is direct
#     evidence of state degradation; GREEN both times over an unchanged
#     sentinel is evidence the process is reusable.
# Run it against known-GREEN controls (log/diff_scenario_*.csv) first — a
# control going RED is the signal, and that is only interpretable if it was
# GREEN in the one-boot-per-hook lane.
#
# Usage:
#   py -3.12 re/frida/run_diff_scenario_batch.py <hook1> <hook2> ... \
#       [--scenario race|results] [--round 130] [--repeat-first]
#       [--shot-dir verify/scenario_batch]
#
# Emits log/diff_scenario_batch_<hook>.csv per hook (run_diff.py schema) and a
# state-health table. Kills ONLY the pid it spawned (multi-session hygiene).

import csv
import json
import os
import shutil
import sys
import time
from pathlib import Path

import frida

sys.path.insert(0, str(Path(__file__).resolve().parent))

import statenav
import run_diff_scenario as RDS
from run_diff_scenario import drive_to_results, ROOT, ORIG, MASHED_EXE, ASI_PATH, LOG_DIR
from run_diff import build_config, value_bits
from hooks_registry import HOOKS

AGENT_JS = Path(__file__).resolve().parent / "diff_template.js"
HOOK_TIMEOUT_S = 45


def _flag(name, default=None, cast=str):
    if name in sys.argv:
        return cast(sys.argv[sys.argv.index(name) + 1])
    return default


def run_one_hook(sess, nav, name, shotdir):
    """Force-call the A/B for one hook against an ALREADY-navigated session.
    Mirrors run_diff_scenario's step 4 verbatim (same agent, same config), but
    leaves the process alive for the next hook."""
    hook = HOOKS[name]
    config = build_config(hook, asi_path=ASI_PATH)
    results, errors, done = [], [], {"v": False}

    def on_msg(message, data):
        if message.get("type") == "error":
            errors.append(message.get("description")); done["v"] = True; return
        p = message.get("payload", {})
        k = p.get("type")
        if k == "results":
            results.extend(p["data"]); done["v"] = True
        elif k == "error":
            errors.append(p.get("msg")); done["v"] = True

    text = AGENT_JS.read_text(encoding="utf-8").replace("$CONFIG$", json.dumps(config))
    scr = sess.create_script(text)
    scr.on("message", on_msg)
    scr.load()
    deadline = time.time() + HOOK_TIMEOUT_S
    while not done["v"] and time.time() < deadline:
        if not nav.alive():
            errors.append("process exited"); break
        time.sleep(0.1)
    try: scr.unload()
    except Exception: pass
    return results, (errors[0] if errors else None)


def verdict_for(hook, results, ret_kind, mism, total, distinct_bits):
    """Mirror run_diff_scenario's acceptance criterion, including ZERO-ARG mode.

    A zero-arg getter is called once, so "distinct values across vectors" is not
    available as the non-degeneracy signal. run_diff_scenario instead requires
    the observed ORIGINAL value to differ from 'zero_arg_baseline' — the value
    the getter returns when the live-state gate is NOT satisfied (menu/default).
    Equal to baseline means navigation did not actually populate the state, so
    the 0-mismatch run proves nothing: that is INCONCLUSIVE, never GREEN.
    """
    if mism:
        return f"RED {mism}/{total}"
    zero_arg = hook.get("zero_arg", False) or len(hook["signature"].get("args", [])) == 0
    if not zero_arg:
        return "GREEN" if len(distinct_bits) > 1 else "GREEN-DEGENERATE"
    baseline = hook.get("zero_arg_baseline")
    obs = next((value_bits(r["original"], ret_kind) for r in results), None)
    if obs is None:
        return "INCONCLUSIVE (no value)"
    if baseline is None:
        return "GREEN" if obs != 0 else "INCONCLUSIVE (value==0, gate unmet)"
    bl = value_bits(baseline, ret_kind) if not isinstance(baseline, int) else baseline
    return "GREEN" if obs != bl else f"INCONCLUSIVE (==baseline {bl})"


def write_csv(name, results, ret_kind, tag):
    out = LOG_DIR / f"diff_scenario_batch_{tag}{name}.csv"
    mism = 0
    distinct = set()
    with out.open("w", newline="", encoding="utf-8") as fh:
        w = csv.writer(fh)
        w.writerow(["idx", "input", "original", "original_bits",
                    "reimpl", "reimpl_bits", "match", "err_original", "err_reimpl"])
        for r in results:
            ob = value_bits(r["original"], ret_kind)
            rb = value_bits(r["reimpl"], ret_kind)
            inp = r["input"]
            w.writerow([r["idx"],
                        json.dumps(inp) if isinstance(inp, (list, dict)) else inp,
                        r["original"], f"0x{ob:08x}" if ob is not None else "",
                        r["reimpl"], f"0x{rb:08x}" if rb is not None else "",
                        r["match"], r.get("err_original") or "", r.get("err_reimpl") or ""])
            if not r["match"]:
                mism += 1
            if ob is not None:
                distinct.add(ob)
    return len(results), mism, distinct, out


def main():
    names = [a for a in sys.argv[1:] if not a.startswith("--")]
    # strip values that belong to flags
    flagvals = set()
    for f in ("--scenario", "--round", "--shot-dir", "--sentinel-words", "--sentinel"):
        if f in sys.argv:
            flagvals.add(sys.argv[sys.argv.index(f) + 1])
    names = [n for n in names if n not in flagvals]
    if not names:
        sys.exit(f"usage: {sys.argv[0]} <hook> [<hook> ...] [--scenario race|results] "
                 f"[--round 130] [--repeat-first]\n  registered: {len(HOOKS)} hooks")
    unknown = [n for n in names if n not in HOOKS]
    if unknown:
        sys.exit(f"unknown hook(s): {', '.join(unknown)}")

    scenario   = _flag("--scenario", "race")
    round_secs = _flag("--round", 130, int)
    sent_words = _flag("--sentinel-words", 8, int)
    shotdir    = _flag("--shot-dir", "verify/scenario_batch")
    repeat_first = "--repeat-first" in sys.argv
    (ROOT / shotdir).mkdir(parents=True, exist_ok=True)

    # Union of every candidate's sentinel: navigation must populate the state
    # that ANY hook in the batch reads, so the gate is the union, not per-hook.
    sent_arg = _flag("--sentinel", None)
    if sent_arg:
        sentinels = sorted({int(s, 16) for s in sent_arg.split(",") if s.strip()})
    else:
        sentinels = []
        for n in names:
            s = HOOKS[n].get("scenario_sentinel")
            if s is None:
                continue
            sentinels.extend(s if isinstance(s, list) else [s])
        sentinels = sorted(set(sentinels))
    if not sentinels:
        sys.exit("no sentinel: pass --sentinel 0xADDR[,..] (the live global(s) the "
                 "targets read) or add 'scenario_sentinel' to the hooks_registry "
                 "entries. Without a gate a 0-mismatch run proves nothing.")

    for p, label in ((MASHED_EXE, "MASHED.exe"), (ASI_PATH, "build artifact")):
        if not p.exists():
            sys.exit(f"{label} not found at {p}")
    if not (MASHED_EXE.parent / "d3d9.dll").exists():
        sys.exit("FATAL: original/d3d9.dll (windowed shim) missing — "
                 "run mashedmod\\build_d3d9_shim.bat")
    cfg = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if cfg.exists():
        shutil.copy2(str(cfg), str(MASHED_EXE.parent / "videocfg.bin"))
    LOG_DIR.mkdir(parents=True, exist_ok=True)

    order = names + ([names[0]] if repeat_first and len(names) > 1 else [])
    print(f"batch: {len(names)} hooks in ONE boot "
          f"(vs {len(names)} boots via run_diff_scenario.py)")
    print(f"  order: {' -> '.join(order)}")
    print(f"  scenario={scenario} sentinels={[hex(s) for s in sentinels]} round={round_secs}s")

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    print(f"  spawned pid={pid} (this session kills ONLY this pid)")
    sess = dev.attach(pid)
    nav_scr = sess.create_script(statenav.AGENT)
    nav_scr.on("message", lambda m, d: None)
    nav_scr.load()
    nav_scr.exports_sync.init()

    def read_sentinels():
        return [(b, [nav_scr.exports_sync.peek(f"0x{b + w*4:08x}") for w in range(sent_words)])
                for b in sentinels]

    def sentinel_nonzero():
        return any(isinstance(v, int) and v != 0
                   for _, ws in read_sentinels() for v in ws)

    rows = []
    try:
        dev.resume(pid)
        nav = statenav.Nav(nav_scr, pid)
        reached = drive_to_results(nav, nav_scr, pid, shotdir, scenario,
                                   round_secs, sentinel_nonzero)
        # The game can self-exit during the race (observed ~t+20-30s on
        # scenario=results, which waits for a round end that never arrives).
        # Reading the sentinel through a destroyed script raises
        # InvalidOperationError, so check liveness before touching the agent.
        if not nav.alive():
            print("\nABORT: process exited during navigation — no hook could run. "
                  "Use --scenario race (returns as soon as the sentinel populates) "
                  "to maximise the in-race window.")
            return 4
        statenav.shoot(pid, ROOT / shotdir / "sb_diffpoint.png")

        baseline_snap = read_sentinels()
        print("\n  sentinel snapshot @ diff point:")
        for b, ws in baseline_snap:
            print(f"    0x{b:08x}: " + " ".join(
                f"0x{(v & 0xffffffff):08x}" if isinstance(v, int) else str(v) for v in ws))
        if not sentinel_nonzero():
            print("\nABORT: sentinel still ZERO — nav did not populate live state "
                  f"(reached={reached}). No hook in this batch can diff non-degenerately.")
            return 5

        print(f"\n{'#':>2} {'hook':38s} {'verdict':22s} {'distinct':>8s} {'sentinel':10s} phase")
        for i, name in enumerate(order, 1):
            tag = "repeat_" if (repeat_first and i == len(order) and len(order) > len(names)) else ""
            results, err = run_one_hook(sess, nav, name, shotdir)
            ret_kind = HOOKS[name]["signature"]["ret"]
            if not results:
                verdict, total, mism, distinct = f"NO-RESULT ({err})", 0, 0, 0
            else:
                total, mism, dbits, _ = write_csv(name, results, ret_kind, tag)
                distinct = len(dbits)
                verdict = verdict_for(HOOKS[name], results, ret_kind, mism, total, dbits)
            alive = nav.alive()
            snap = read_sentinels() if alive else None
            same = (snap == baseline_snap) if alive else False
            phase = nav.phase() if alive else "DEAD"
            rows.append((i, name, tag, verdict, distinct, same, phase))
            print(f"{i:>2} {name[:38]:38s} {verdict:22s} {distinct:8d} "
                  f"{'unchanged' if same else 'CHANGED':10s} {phase}")
            if not alive:
                print("   process died — remaining hooks cannot run")
                break
    finally:
        try: dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        print(f"  killed pid={pid}")

    # ── state-reuse verdict ─────────────────────────────────────────────────
    print("\n=== STATE-REUSE VERDICT ===")
    ran = [r for r in rows if not r[3].startswith("NO-RESULT")]
    print(f"  hooks attempted: {len(rows)}  produced results: {len(ran)}")
    green = [r for r in ran if r[3].startswith("GREEN")]
    print(f"  GREEN: {len(green)}/{len(ran)}")
    changed_at = [r[0] for r in rows if not r[5]]
    if changed_at:
        print(f"  sentinel CHANGED after hook #{changed_at[0]} — state is perturbed by "
              f"force-calls; hooks after that point are suspect")
    else:
        print("  sentinel unchanged across every hook — no observed perturbation")
    if repeat_first and len(rows) == len(order):
        first, last = rows[0], rows[-1]
        if first[1] == last[1]:
            print(f"  repeat control '{first[1]}': first={first[3]}  last={last[3]}")
            print("  => REUSABLE" if first[3] == last[3] else
                  "  => DEGRADED — one boot per hook is required after all")
    return 0


if __name__ == "__main__":
    sys.exit(main())
