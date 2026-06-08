# Scenario-attach A/B diff harness (Pivot A).
#
# Runs the EXISTING synthetic A/B call-diff (orig RVA vs .asi export, hook BYPASSED)
# from re/frida/diff_template.js — but takes the diff while the game sits at a
# POPULATED + QUIESCENT scenario point instead of the empty main menu. Populated
# state makes a getter that reads a live array return varied, non-degenerate values
# across the registry's test vectors, so the bit-identity A/B is meaningful (not a
# false-GREEN where every vector returns the same constant).
#
# CONFIDENCE: this remains C3 evidence (synthetic A/B, inline-JMP bypassed). It is
# NOT a C4 claim — C4 still requires hook-installed canonical observation
# (re/CONFIDENCE.md). Do NOT overclaim C4 from this.
#
# Reuses, unchanged:
#   * statenav.py        — Nav class + the FUN_00497310 in-process return-override
#                          (control id 4=confirm, 11=up, 12=down) state-aware driver.
#                          NO OS input injection anywhere (hard user rule).
#   * diff_template.js   — the 98 arg_type A/B call/observe/compare engine.
#   * run_diff.build_config — maps a hooks_registry entry -> the agent CONFIG dict.
#
# Usage:
#   py -3.12 re/frida/run_diff_scenario.py <hook_name> [--scenario results]
#       [--sentinel 0xADDR[,0xADDR...]] [--sentinel-words N]
#       [--round 120] [--shot-dir verify/scenario]
#
#   <hook_name>            a key in hooks_registry.HOOKS (read-only getter ONLY).
#   --scenario results     scenario point to navigate to (default 'results';
#                          'race' = stop as soon as the round has started/track
#                          loaded; the diff runs once the sentinel is non-zero).
#   --sentinel 0xADDR      one or more live globals the target depends on; the run
#                          asserts at least one is NON-zero (over --sentinel-words
#                          dwords) before running the diff. Aborts with a clear
#                          message if every sentinel word is still zero (nav did not
#                          populate the state). If omitted, falls back to the hook's
#                          'scenario_sentinel' registry field if present.
#   --sentinel-words N     dwords to scan from each sentinel base (default 8).
#
# Emits log/diff_scenario_<hook_name>.csv (same schema as run_diff.py) and prints
# GREEN/RED + a degeneracy check (how many DISTINCT original values the vectors
# produced — 1 distinct == still degenerate; >1 == non-degenerate, the goal).
import csv
import json
import os
import shutil
import struct
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))

from hooks_registry import HOOKS
from run_diff import build_config, value_bits, _find_original
# Reuse statenav's Nav class + the in-process press driver agent verbatim.
import statenav

MASHED_EXE = _find_original(ROOT)
ORIG       = MASHED_EXE.parent
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'diff_template.js'
LOG_DIR    = ROOT / 'log'


def _arg(name, default=None, cast=str):
    if name in sys.argv:
        return cast(sys.argv[sys.argv.index(name) + 1])
    return default


def drive_to_results(nav, scr, pid, shotdir, scenario, round_secs, sentinel_check):
    """Reuse statenav's proven nav sequence to reach a populated+quiescent point.
    Returns True once nav has descended into the race (track loaded -> live state
    populated). For scenario=='results' it additionally lets the arena round play
    out so the diff is taken on the static post-race results screen. The diff
    actually runs only after sentinel_check() reports non-zero, so 'race' is the
    earliest-populated variant and 'results' the most-quiescent."""
    print("  booting...")
    nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 18.0, "title up")
    print(f"  title: depth={nav.depth()} phase={nav.phase()}")
    nav.confirm_to_depth(2); time.sleep(0.3); nav.press(4); time.sleep(0.5)
    print(f"  after GTS+modal: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    statenav.shoot(pid, ROOT / shotdir / "sa_gts.png")
    nav.confirm_to_depth(3)
    print(f"  single player: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    nav.press(12)   # down -> Quick Battle (arena; round ends on its own)
    print(f"  mode sel (Quick Battle): sel={scr.exports_sync.sel()}")
    nav.confirm_to_depth(4, tries=4)
    print(f"  colour-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    nav.confirm_to_depth(5, tries=4)   # colour -> track select
    print(f"  track-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    statenav.shoot(pid, ROOT / shotdir / "sa_track.png")
    nav.press(4); time.sleep(1.5)      # confirm track -> Game Mode setup screen
    print(f"  after track confirm: depth={nav.depth()} phase={nav.phase()}")
    # Play Game -> START arena. Confirm until phase leaves the frontend (==0 in arena).
    for k in range(5):
        if nav.phase() != 3: break
        nav.press(4); time.sleep(1.5)
        print(f"  start-attempt {k}: depth={nav.depth()} phase={nav.phase()}")
        if nav.phase() == 0: break
    statenav.shoot(pid, ROOT / shotdir / "sa_race_enter.png")
    # Track is loading -> live arrays populate. Poll the sentinel; once non-zero the
    # state is live. For scenario=='race' we return at first populated frame; for
    # 'results' we keep waiting for the round to end (quiescent results screen) but
    # stop early if the sentinel is already populated AND the round has ended.
    end = time.time() + round_secs; t_shot = 0; populated_at = None
    while time.time() < end:
        if not nav.alive():
            print("   process exited during race"); break
        ph = nav.phase()
        pop = sentinel_check()
        if pop and populated_at is None:
            populated_at = int(round_secs - (end - time.time()))
            print(f"   >>> sentinel POPULATED at t+{populated_at}s (phase={ph})")
            if scenario == "race":
                return True
        if time.time() - t_shot > 10:
            t_shot = time.time()
            tnow = int(round_secs - (end - time.time()))
            print(f"   round t+{tnow}s phase={ph} populated={bool(pop)}")
            statenav.shoot(pid, ROOT / shotdir / f"sa_round_{tnow}.png")
        # 'results': a return to phase==3 with depth>=1 after the race == results/menu
        # screen reached and quiescent. Run the diff there if populated.
        if scenario == "results" and populated_at is not None and ph == 3 and nav.depth() >= 1:
            print(f"   >>> results/quiescent screen reached (phase=3) with state populated")
            return True
        time.sleep(0.5)
    # timed out — return True iff we ever saw the state populated (diff still useful)
    return populated_at is not None


def main():
    if len(sys.argv) < 2 or sys.argv[1].startswith("--"):
        sys.exit(f"usage: {sys.argv[0]} <hook_name> [--scenario results|race] "
                 f"[--sentinel 0xADDR[,..]] [--sentinel-words N] [--round 120] "
                 f"[--shot-dir verify/scenario]\n  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")
    hook = HOOKS[name]

    scenario   = _arg("--scenario", "results")
    round_secs = _arg("--round", 130, int)
    sent_words = _arg("--sentinel-words", 8, int)
    shotdir    = _arg("--shot-dir", "verify/scenario")
    Path(ROOT / shotdir).mkdir(parents=True, exist_ok=True)

    # sentinel addresses: --sentinel arg overrides registry 'scenario_sentinel'.
    sent_arg = _arg("--sentinel", None)
    if sent_arg:
        sentinels = [int(s, 16) for s in sent_arg.split(",") if s.strip()]
    elif "scenario_sentinel" in hook:
        s = hook["scenario_sentinel"]
        sentinels = s if isinstance(s, list) else [s]
    else:
        sys.exit("no sentinel given: pass --sentinel 0xADDR (the live global the "
                 "target reads) or add 'scenario_sentinel' to the hooks_registry entry")
    print(f"hook: {name}  rva=0x{hook['rva']:08x}  export={hook['export']}")
    print(f"scenario={scenario}  sentinels={[hex(s) for s in sentinels]} "
          f"x{sent_words} words  round={round_secs}s")

    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"build artifact not found at {ASI_PATH} — run mashedmod\\build.bat first")
    _shim = MASHED_EXE.parent / 'd3d9.dll'
    if not _shim.exists():
        sys.exit(f"FATAL: {_shim} missing (d3d9 windowed shim). "
                 f"Run `mashedmod\\build_d3d9_shim.bat`, then retry.")
    # windowed 800x600 so concurrent sessions don't fight over fullscreen.
    _canonical_cfg = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if _canonical_cfg.exists():
        shutil.copy2(str(_canonical_cfg), str(MASHED_EXE.parent / 'videocfg.bin'))

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    csv_out = LOG_DIR / f'diff_scenario_{name}.csv'

    # ── spawn + attach the NAV agent ────────────────────────────────────────
    # MASHED_RE_NO_AUTO_HOOK=1: the .asi must NOT auto-install its inline JMP,
    # because the synthetic A/B calls the ORIGINAL RVA directly (hook bypassed).
    # diff_template.js Module.load's the .asi itself to resolve the reimpl export.
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    nav_scr = sess.create_script(statenav.AGENT)
    nav_scr.on("message", lambda m, d: None)
    nav_scr.load()
    delta = nav_scr.exports_sync.init()   # returns base-0x400000 delta (0 if unrelocated)

    def read_sentinels():
        """Return list of (addr, [words]) read live via the nav agent. Reuses the
        nav agent's module-base delta so it's correct even if MASHED relocated."""
        out = []
        for base in sentinels:
            words = []
            for w in range(sent_words):
                v = nav_scr.exports_sync.peek(f"0x{base + w*4:08x}")
                words.append(v)
            out.append((base, words))
        return out

    def sentinel_nonzero():
        for base, words in read_sentinels():
            if any(isinstance(v, int) and v != 0 for v in words):
                return True
        return False

    dev.resume(pid)
    nav = statenav.Nav(nav_scr, pid)

    reached = drive_to_results(nav, nav_scr, pid, shotdir, scenario, round_secs, sentinel_nonzero)
    statenav.shoot(pid, ROOT / shotdir / "sa_diffpoint.png")

    # ── step 3: assert sentinel populated (clear abort if not) ───────────────
    snap = read_sentinels()
    print("  sentinel snapshot @ diff point:")
    for base, words in snap:
        print(f"    0x{base:08x}: " + " ".join(f"0x{(v & 0xffffffff):08x}" if isinstance(v, int) else str(v) for v in words))
    if not sentinel_nonzero():
        print("\nABORT: sentinel still ZERO at the diff point — nav did NOT populate "
              "the live state the target reads. Cannot produce a non-degenerate diff.")
        print(f"  (reached={reached} scenario={scenario}; the state may need a different "
              f"scenario point, or the round did not start.)")
        try: dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        return 5

    # ── step 4: run the EXISTING A/B diff at this live point ─────────────────
    # Load diff_template.js as a SECOND script on the SAME (already-navigated)
    # process. It Module.load's the .asi, resolves the reimpl export, and runs the
    # synthetic A/B once (pollLutThenRun -> runDiff). The original RVA + the live
    # state it reads are now populated, so a getter returns varied values.
    config = build_config(hook, asi_path=ASI_PATH)
    results_received, errors_received, done = [], [], {"v": False}

    def on_msg(message, data):
        if message.get("type") == "error":
            print("AGENT ERROR:", message.get("description"))
            errors_received.append(message); done["v"] = True; return
        p = message.get("payload", {}); k = p.get("type")
        if k == "results":
            results_received.extend(p["data"]); done["v"] = True
        elif k == "asi_loaded":
            print(f"  [diff] .asi loaded base={p['base']} {p['export_name']}@{p['reimpl_addr']}")
        elif k == "lut_ready":
            print(f"  [diff] LUT ready root={p['root']} attempts={p['attempts']}")
        elif k == "error":
            print(f"  [diff] ERROR: {p['msg']}"); errors_received.append(p); done["v"] = True
        else:
            print("  [diff]", p)

    script_text = AGENT_JS.read_text(encoding="utf-8").replace("$CONFIG$", json.dumps(config))
    diff_scr = sess.create_script(script_text)
    diff_scr.on("message", on_msg)
    diff_scr.load()

    deadline = time.time() + 45
    while not done["v"] and time.time() < deadline:
        if not nav.alive():
            print("  process exited before diff completed"); break
        time.sleep(0.1)

    try: diff_scr.unload()
    except Exception: pass
    try: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass

    if errors_received and not results_received:
        print("\nNO RESULTS — agent reported errors above.")
        return 2
    if not results_received:
        print("\nTIMEOUT — no results from the diff agent.")
        return 3

    # ── step 5: emit CSV (same schema as run_diff.py) + degeneracy check ─────
    ret_kind = hook["signature"]["ret"]
    mismatches = 0
    distinct_orig = set()
    with csv_out.open("w", newline="", encoding="utf-8") as fh:
        w = csv.writer(fh)
        w.writerow(["idx", "input", "original", "original_bits",
                    "reimpl", "reimpl_bits", "match", "err_original", "err_reimpl"])
        for r in results_received:
            ob, rb = value_bits(r["original"], ret_kind), value_bits(r["reimpl"], ret_kind)
            ob_s = f"0x{ob:08x}" if ob is not None else ""
            rb_s = f"0x{rb:08x}" if rb is not None else ""
            inp = r["input"]
            inp_repr = json.dumps(inp) if isinstance(inp, (list, dict)) else inp
            w.writerow([r["idx"], inp_repr, r["original"], ob_s,
                        r["reimpl"], rb_s, r["match"],
                        r.get("err_original") or "", r.get("err_reimpl") or ""])
            if not r["match"]:
                mismatches += 1
            if r.get("original") is not None:
                distinct_orig.add(str(r["original"]))

    print(f"\nResults written to {csv_out}")
    print(f"  total cases:        {len(results_received)}")
    print(f"  mismatches:         {mismatches}")
    print(f"  distinct orig vals: {len(distinct_orig)}  -> "
          f"{'NON-DEGENERATE (state is live)' if len(distinct_orig) > 1 else 'DEGENERATE (still constant — sentinel populated but vectors do not vary the read)'}")
    if mismatches == 0 and len(distinct_orig) > 1:
        print("\nGREEN + NON-DEGENERATE: bit-identical across varied live-state returns (C3 evidence; NOT C4).")
        return 0
    if mismatches == 0:
        print("\nGREEN but DEGENERATE: returns still constant across vectors — not promotable evidence.")
        return 6
    print("\nRED: at least one mismatch.")
    for r in results_received:
        if not r["match"]:
            print(f"  idx={r['idx']} input={r['input']} orig={r['original']} reimpl={r['reimpl']}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
