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
# ANSWER (measured 2026-07-28) — REUSE HOLDS, WITH ONE FIXED HAZARD:
#   * INTEGER hooks reuse cleanly: 48 force-calls of 4 read-only zero-arg
#     getters, 48/48 GREEN, repeat control GREEN at first and last position.
#   * A float-returning hook batched after a SPECIFIC polluter false-REDs with
#     exactly 1 mismatch, always idx=0 (original side came back EMPTY for
#     heading_atan2 and 0x9F9C garbage for audio_vec_length, i.e.
#     NaN/indefinite, clean from vector 1 on). That is a DIRTY x87 STACK on
#     entry: the polluter leaves ST0 occupied, the next float hook's first FLD
#     overflows the x87 stack, and it drains by the second vector. Same hazard
#     class as memory feedback_x87_st0_float10_return_fnptr.
#     NOT a property of float hooks in general — heading_atan2 + audio_vec_length
#     alternating 3x with NO scrub is 6/6 GREEN. The polluter measured here was
#     camera_path_all_nodes_eq2 (0x0047c270), itself RED 8/8 for real.
#   FIXED (24fb2b69) by a CW-PRESERVING scrub between hooks: save CW -> FNINIT
#   -> restore CW. A bare FNINIT would reset the control word to 0x037F while
#   MASHED/MSVC run their own, trading a dirty-stack false-RED for a *rounding*
#   false-RED. On by default; --no-x87-scrub reproduces the fault (that is how
#   the fix was A/B'd: scrub OFF 1/5 GREEN -> scrub ON 3/5, same ordering).
#
# VERDICTS ARE NOT JUST GREEN/RED. Two ways a run can compare nothing at all:
#   * ZERO-ARG DEGENERACY — a single-shot getter returning its menu default;
#     caught by the zero_arg_baseline criterion (INCONCLUSIVE, never GREEN).
#   * BOTH SIDES ERRORED IDENTICALLY — every non-matching row has
#     err_original == err_reimpl (same AV, same fault address). Measured
#     2026-07-28 on all three hooks that read "RED" in the first real batch:
#     camera_path_all_nodes_eq2 8/8 at 0x0, camera_path_any_node_nonzero 8/8 at
#     0xc, smplfzx_stateblock_get_logged 10/10 at 0x10. Both sides died before
#     producing a value, i.e. the live state those hooks read was still null
#     when we called them (their own registry comments say the manager pointer
#     is null until deep into a race). Calling that RED manufactures a port
#     defect out of a run in which the function never executed. Now reported as
#     INCONCLUSIVE-BOTH-ERRORED — and still never GREEN, because identical
#     faults are consistent with identical code without demonstrating it.
#
# The `race` scenario is POPULATED but NOT quiescent (run_diff_scenario defaults
# to `results` for that reason). Prefer --scenario race only for hooks whose
# state is stable within a frame; a per-frame-varying getter can false-RED
# because the live global moves between the original and reimpl calls.
#
# Usage:
#   py -3.12 re/frida/run_diff_scenario_batch.py <hook1> <hook2> ... \
#       [--scenario race|results] [--round 130] [--repeat-first]
#       [--sentinel 0xADDR[,0xADDR...]] [--no-x87-scrub]
#       [--dwell 20] [--gate-wait 15] [--shot-dir verify/scenario_batch]
#
# --dwell is usually REQUIRED for anything but the simplest getters: `--scenario
# race` returns at race frame 0 (phase=0, t+0s), where much of the live state a
# STATE hook wants has not been built yet. Measured 2026-07-28 at that instant —
# camera node count already 2 but every sub-count still 0, SmplFzx manager
# pointer at 0x006e71cc still NULL. Per-hook 'state_gate' in hooks_registry.py
# then decides individually: a chain [base, off, ...] must resolve non-null, or
# {'any_nonzero': base, 'words': n} must find one live element; a hook whose gate
# stays unmet for --gate-wait seconds is SKIPPED with the failing link printed,
# instead of being called into a null deref and scored.
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


def run_one_hook(sess, nav, name, shotdir, tests_override=None):
    """Force-call the A/B for one hook against an ALREADY-navigated session.
    Mirrors run_diff_scenario's step 4 verbatim (same agent, same config), but
    leaves the process alive for the next hook.

    tests_override replaces the registry's path1_tests with values harvested
    from the live game (see capture_live_args). Both sides still receive the
    IDENTICAL list, so bit-identity is unaffected."""
    hook = HOOKS[name]
    config = build_config(hook, asi_path=ASI_PATH)
    if tests_override:
        config["tests"] = list(tests_override)
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


# x87 stack scrub, run between hooks on the same Frida thread the force-calls
# use. Emitting a bare FNINIT would also reset the FPU CONTROL WORD to 0x037F,
# but MASHED/MSVC run with their own CW (precision + rounding bits), and
# changing it would alter results — trading the dirty-stack false-RED for a
# rounding false-RED. So: save CW, FNINIT (empties the register stack and
# clears exception flags), restore CW.
#
#   83 EC 04        sub  esp,4
#   D9 3C 24        fnstcw word [esp]
#   DB E3           fninit
#   D9 2C 24        fldcw  word [esp]
#   83 C4 04        add  esp,4
#   C3              ret
X87_SCRUB_JS = """
const code = Memory.alloc(Process.pageSize);
Memory.protect(code, Process.pageSize, 'rwx');
code.writeByteArray([0x83,0xEC,0x04, 0xD9,0x3C,0x24, 0xDB,0xE3,
                     0xD9,0x2C,0x24, 0x83,0xC4,0x04, 0xC3]);
new NativeFunction(code, 'void', [])();
send({type: 'x87_scrubbed'});
"""


def scrub_x87(sess):
    """Empty the x87 register stack between hooks, preserving the control word.
    Returns True if the stub ran. The dirt demonstrably crosses script
    boundaries (measured: a float hook at position>1 false-REDs on vector 0),
    which means these scripts share a thread — so scrubbing from a sibling
    script reaches the same FPU state the force-calls see."""
    ok = {"v": False}
    scr = sess.create_script(X87_SCRUB_JS)
    scr.on("message", lambda m, d: ok.__setitem__(
        "v", m.get("payload", {}).get("type") == "x87_scrubbed"))
    try:
        scr.load()
    except Exception:
        return False
    finally:
        try: scr.unload()
        except Exception: pass
    return ok["v"]


# ---------------------------------------------------------------------------
# LIVE ARGUMENT CAPTURE
#
# The blocker measured 2026-07-29 is not a missing attach point — it is that a
# large share of STATE candidates take a POINTER, and every synthetic value we
# can invent for one is wrong. Seeding a scratch buffer cannot fix it: fill it
# non-zero and a second-level deref faults, fill it zero and the target's own
# null check makes the run degenerate. Three hooks died this way in one batch,
# each looking like "live state absent" when the fault address was really
# argument + the first field offset (memory
# feedback_pointer_param_described_as_int).
#
# So don't invent the pointer — let the GAME produce it. Attach to the target,
# let it be called naturally for a short window, record the distinct argument
# values, detach, and replay those REAL values through the normal A/B. Both
# sides see the identical list, so bit-identity still holds, and distinct live
# objects give the non-degeneracy for free.
#
# This is the same instinct as the existing 'sprite_table_dispatch' arg_type,
# which patches the CALLEE with a NativeCallback to capture what was passed —
# generalised from one hook to any pointer-argument hook, and it needs no new
# diff_template.js handler: a captured pointer is just an int, so 'int_scalar'
# replays it verbatim.
#
# HOT-PATH HAZARD: Interceptor.attach on a >1000 calls/s function destabilises
# MASHED in about six seconds (CLAUDE.md). The window is therefore short and
# capped, one function at a time, and the listener is detached before the A/B
# runs — never left attached across the diff.
CAPTURE_JS = """
const tgt = ptr('$TARGET$');
const IDX = $IDX$, MAX = $MAX$;
const seen = [];
let li = null;
try {
    li = Interceptor.attach(tgt, {
        onEnter: function (args) {
            if (seen.length >= MAX) return;
            const v = args[IDX].toUInt32();
            if (seen.indexOf(v) === -1) seen.push(v);
        }
    });
} catch (e) {
    send({ type: 'captured', data: [], err: 'attach failed: ' + e.message });
}
setTimeout(function () {
    try { if (li) li.detach(); Interceptor.flush(); } catch (e) {}
    send({ type: 'captured', data: seen });
}, $WINDOW$);
"""


def capture_live_args(sess, nav, spec):
    """Record distinct argument values the GAME passes to a target.

    spec: {'rva':…, 'arg_index':0, 'max':12, 'window_ms':1200}
    Returns (values, note). Values are raw u32s in call order, deduped.
    """
    rva = spec["rva"]
    js = (CAPTURE_JS
          .replace("$TARGET$", f"0x{rva:08x}")
          .replace("$IDX$", str(int(spec.get("arg_index", 0))))
          .replace("$MAX$", str(int(spec.get("max", 12))))
          .replace("$WINDOW$", str(int(spec.get("window_ms", 1200)))))
    got, done = {"v": []}, {"v": False}
    err = {"v": None}

    def on_msg(message, data):
        if message.get("type") == "error":
            err["v"] = message.get("description"); done["v"] = True; return
        p = message.get("payload", {})
        if p.get("type") == "captured":
            got["v"] = p.get("data") or []
            if p.get("err"):
                err["v"] = p["err"]
            done["v"] = True

    scr = sess.create_script(js)
    scr.on("message", on_msg)
    scr.load()
    deadline = time.time() + (int(spec.get("window_ms", 1200)) / 1000.0) + 8
    while not done["v"] and time.time() < deadline:
        if not nav.alive():
            err["v"] = "process exited during capture"; break
        time.sleep(0.05)
    try: scr.unload()
    except Exception: pass
    return got["v"], err["v"]


def validate_captured(peek, values, spec):
    """Drop captured values that are not usable NOW.

    A pointer harvested a second ago can be stale by replay time (freed, or a
    per-frame object recycled), and a stale pointer produces a both-sides AV
    that is indistinguishable from a bad port at a glance. So re-check each one
    immediately before use: it must be in the process's address range and every
    offset in spec['require_offsets'] must read back non-zero.
    """
    lo = int(spec.get("min_ptr", 0x00010000))
    req = spec.get("require_offsets") or [0]
    out = []
    for v in values:
        v &= 0xffffffff
        if v < lo:
            continue
        if all(isinstance(peek(f"0x{(v + off) & 0xffffffff:08x}"), int)
               and peek(f"0x{(v + off) & 0xffffffff:08x}") != 0 for off in req):
            out.append(v)
    return out


def walk_chain(peek, chain, minval=0):
    """Walk a pointer chain [base, off1, off2, ...] and report what we found.

    Semantics: v = *base; for each off: v must be non-null, then v = *(v + off).
    The FINAL value must be >= minval (default: merely non-zero). Returns
    (ok, human-readable trace) — the trace is printed on failure so a skipped
    hook says WHICH link was null rather than just "gate unmet".

    minval is for a link the target uses as a BASE ADDRESS and which is
    genuinely supposed to hold a pointer. USE IT SPARINGLY — the case that
    motivated it turned out not to be one. body_geometry_first_dword gated on
    [0x007dc8d8] != 0, passed, then faulted on both sides at 0xa4/0xa8/0xac, and
    the global held 0xa4; I concluded it was "not a usable base" and gated it
    out. Wrong: 0x0057c270 registers a RenderWare plugin (size 4, id 0x901) and
    stores the returned PLUGIN DATA OFFSET there, so 0xa4 is the correct healthy
    value and the real defect was passing an int where an RwObject* belongs. A
    min gate there would have permanently skipped a working hook. Confirm what a
    global HOLDS before demanding it look like a pointer.
    """
    addr = chain[0] & 0xffffffff
    v = peek(f"0x{addr:08x}")
    if not isinstance(v, int):
        return False, f"[0x{addr:08x}]=UNREADABLE"
    trace = f"[0x{addr:08x}]=0x{v & 0xffffffff:08x}"
    for off in chain[1:]:
        if (v & 0xffffffff) == 0:
            return False, trace + f" -> NULL before +0x{off:x}"
        nxt = peek(f"0x{(v + off) & 0xffffffff:08x}")
        if not isinstance(nxt, int):
            return False, trace + f" +0x{off:x}=UNREADABLE"
        trace += f" +0x{off:x}=0x{nxt & 0xffffffff:08x}"
        v = nxt
    ok = (v & 0xffffffff) >= max(1, minval)
    if not ok and minval:
        trace += f" (< min 0x{minval:08x} — not a usable base)"
    return ok, trace


def eval_state_gate(peek, hook):
    """Per-hook liveness gate: every chain in hook['state_gate'] must resolve.

    The batch-wide --sentinel proves SOME live state came up; it says nothing
    about the specific globals a given hook dereferences. Measured 2026-07-28:
    smplfzx_stateblock_get_logged reads *(*(0x006e71cc)+0xc) and faulted 10/10
    on BOTH sides — its own registry comment predicted it ("null at menu,
    double deref"). Calling a hook whose chain is still null produces a
    both-sides-identical crash, i.e. a run that compares nothing. Gating turns
    that into an explicit SKIP instead of a fake verdict.
    """
    chains = hook.get("state_gate") or []
    if not chains:
        return True, ""
    traces = []
    for ch in chains:
        if isinstance(ch, dict) and "chain" in ch:
            # {'chain': [base, off, ...], 'min': N} — as below, but the final
            # value must be >= N. Use for any link the target DEREFERENCES.
            ok, tr = walk_chain(peek, ch["chain"], int(ch.get("min", 0)))
            traces.append(tr)
        elif isinstance(ch, dict):
            # {'any_nonzero': base, 'words': n} — at least one of n consecutive
            # dwords must be non-zero. A pointer chain cannot express "some
            # element of this array is live", which is exactly what the camera
            # predicates need: the outer scan only reaches the inner predicate
            # for nodes whose SUB-COUNT is non-zero, so a node count of 2 with
            # both sub-counts still 0 returns 0 for every input — a degenerate
            # run, not a broken one.
            base, n = ch["any_nonzero"] & 0xffffffff, int(ch.get("words", 8))
            vals = [peek(f"0x{(base + w*4) & 0xffffffff:08x}") for w in range(n)]
            ok = any(isinstance(v, int) and (v & 0xffffffff) != 0 for v in vals)
            traces.append(f"any_nonzero[0x{base:08x}..+{n*4}]=" + ",".join(
                f"0x{(v & 0xffffffff):08x}" if isinstance(v, int) else "??" for v in vals))
        else:
            ok, tr = walk_chain(peek, ch)
            traces.append(tr)
        if not ok:
            return False, " | ".join(traces)
    return True, " | ".join(traces)


def both_errored_identically(results):
    """True iff EVERY non-matching row failed the SAME way on BOTH sides.

    Measured 2026-07-28: camera_path_all_nodes_eq2, camera_path_any_node_nonzero
    and smplfzx_stateblock_get_logged each read 8/8 or 10/10 "RED" — and every
    single row had err_original == err_reimpl ('access violation accessing 0x0',
    '0xc', '0x10'). Both sides died at the same fault address before producing a
    value. That is NOT a port divergence; it is the live state the hook reads
    still being null at the point we called it (their registry comments say so
    outright — "the state-block manager pointer is null at menu (double deref)").
    Reporting it as RED manufactures a defect out of a run in which the function
    never executed, which is the same failure mode as the x87 false-RED that
    nearly filed a correct port as broken.

    The honest verdict is INCONCLUSIVE, never GREEN: identical faults are
    consistent with identical code, but do not demonstrate it — nothing was
    compared. hooks_registry.py's existing 'crash_equal_ok' flag is the opt-in
    that says the author intends equal crashes to count; without it this only
    downgrades RED to INCONCLUSIVE.
    """
    bad = [r for r in results if not r["match"]]
    if not bad:
        return False
    return all(r.get("err_original") and r.get("err_original") == r.get("err_reimpl")
               for r in bad)


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
        if both_errored_identically(results):
            err = next(r.get("err_original") for r in results if not r["match"])
            # A both-errored row compares NOTHING, so it must not outvote rows
            # that do. Measured 2026-07-29: slot_object_field8 returned NINE
            # distinct bit-identical values and both-faulted on one out-of-range
            # tail vector — and the whole run was labelled INCONCLUSIVE, throwing
            # away exactly the evidence it had gathered. Judge on the comparable
            # rows; report the excluded ones rather than letting them decide.
            ok = [r for r in results if r["match"]]
            ok_bits = {b for b in (value_bits(r["original"], ret_kind) for r in ok)
                       if b is not None}
            ok_bits |= {r["original"] for r in ok
                        if value_bits(r["original"], ret_kind) is None and r["original"]}
            if len(ok_bits) > 1:
                return (f"GREEN {len(ok)}/{total} ({len(ok_bits)} distinct; "
                        f"{mism} both-errored, excluded)")
            return (f"INCONCLUSIVE-BOTH-ERRORED {mism}/{total} "
                    f"(both sides: {err}) — nothing comparable was discriminating")
        return f"RED {mism}/{total}"
    # A spread of distinct ORIGINAL values IS the non-degeneracy proof, and it
    # outranks the zero_arg baseline check — the baseline exists only for hooks
    # where that spread is unavailable (a single-shot getter). Measured
    # 2026-07-28: RenderState_GetTexturingOverride carries zero_arg=True but is
    # driven with 12 varying inputs and echoes each one back, 12 distinct values
    # bit-identical; judging it by its FIRST row alone reported
    # "INCONCLUSIVE (value==0, gate unmet)" and threw away a perfectly good
    # control. Checking distinct spread first is strictly more evidence, never
    # less.
    if len(distinct_bits) > 1:
        return "GREEN"
    zero_arg = hook.get("zero_arg", False) or len(hook["signature"].get("args", [])) == 0
    if not zero_arg:
        return "GREEN-DEGENERATE"
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
            elif r["original"]:
                # COMPOSITE FINGERPRINTS count too. arg_types like out1_idx and
                # cache_roundtrip return a string ("<ret>:<out>"), for which
                # value_bits is None — so the distinct set stayed EMPTY and every
                # such run was reported GREEN-DEGENERATE no matter how
                # discriminating it was. Measured 2026-07-29:
                # vehicle_float_field_as_int produced three distinct
                # fingerprints, including 00000000:cccccccc — the out-of-range
                # path with the poison intact, i.e. positive proof that the guard
                # fires AND that it leaves *out untouched. That is the strongest
                # row in the run and it was being discarded.
                distinct.add(r["original"])
    return len(results), mism, distinct, out


def main():
    names = [a for a in sys.argv[1:] if not a.startswith("--")]
    # strip values that belong to flags
    flagvals = set()
    for f in ("--scenario", "--round", "--shot-dir", "--sentinel-words", "--sentinel",
              "--gate-wait", "--dwell", "--inrace-probe"):
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
    # Seconds to keep polling a hook's own state_gate before skipping it.
    gate_wait = _flag("--gate-wait", 15.0, float)
    # Seconds to let the race run before taking the diff point.
    dwell = _flag("--dwell", 0.0, float)
    # RVAs only IN-RACE SIMULATION reaches, used to prove the diff point is inside
    # a running race. Pick these carefully — the first attempt got it wrong.
    # 0x00436810 / 0x0045ba00 / 0x00408a70 came from statenav's GAMEPLAY list, but
    # hooks.csv puts all three in the FRONTEND subsystem and statenav's own comment
    # calls them "the RESULTS/round-end subset". They fire when the results screen
    # appears (statenav: first_results_at=20s), so 0/0/0 from them means "the round
    # has not ENDED" — exactly what --scenario race is meant to produce, and it says
    # nothing about whether a race is running. Reading it as "never reaches a race"
    # was the third wrong call in a row on that question.
    #
    # These are per-frame SIMULATION instead:
    #   0x00470670 VehicleControlUpdate    (vehicle, C4)
    #   0x0047eb30 VehiclePhysicsWorldStep (vehicle, C3)
    #   0x004233e0 HeadingAtan2ToGameAngle (ai, C3 — on the physics chain)
    # VALIDATE A PROBE BEFORE TRUSTING ITS ZERO: a counter that has never been seen
    # to fire anywhere proves nothing when it reads 0.
    _probe_arg = _flag("--inrace-probe", None)
    inrace_probes = ([int(x, 16) for x in _probe_arg.split(",") if x.strip()]
                     if _probe_arg else [0x00470670, 0x0047eb30, 0x004233e0])
    shotdir    = _flag("--shot-dir", "verify/scenario_batch")
    repeat_first = "--repeat-first" in sys.argv
    # --no-x87-scrub: skip the between-hook FPU scrub, to reproduce the dirty
    # stack the scrub exists to fix (that is how the fix is A/B'd).
    no_scrub = "--no-x87-scrub" in sys.argv
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

    # x87 hazard: a float-returning hook scheduled after any other hook sees a
    # dirty x87 stack and false-REDs on its FIRST test vector only (measured
    # 2026-07-28; see header). Warn rather than silently reorder — the caller
    # may be deliberately probing the effect, as the measurement run was.
    late_floats = [(i, n) for i, n in enumerate(order, 1)
                   if i > 1 and HOOKS[n]["signature"]["ret"] in ("float", "double")]
    if late_floats and no_scrub:
        print("  WARNING x87: --no-x87-scrub with float-returning hook(s) after "
              "position 1 — expect a false RED on vector idx=0:")
        for i, n in late_floats:
            print(f"    #{i} {n}")
    elif late_floats:
        print(f"  x87 scrub ON (CW-preserving FNINIT between hooks); "
              f"{len(late_floats)} float hook(s) scheduled after position 1")
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
        # --dwell: let the race RUN before diffing. `--scenario race` returns
        # the instant the sentinel populates, which is phase=0 / t+0s — the
        # first frame of the race. Measured 2026-07-28 at that point: the
        # camera node COUNT is already 2 but every sub-count is still 0 (so the
        # path predicates return 0 for every input — degenerate, not broken),
        # and the SmplFzx manager pointer at 0x006e71cc is still NULL. Arriving
        # early is what made the state look absent. Dwelling costs in-race
        # window, which is cheap: the window fits ~28 verified hooks/min.
        if dwell > 0:
            # ACTIVE dwell — press controls, do not just sleep.
            #
            # Measured 2026-07-29 and this is the whole difference: a PASSIVE dwell
            # of 25s left the in-race probes at 0/0/0, while statenav.py — which
            # cycles a held control every ~6s through its round loop — reached a
            # race with 0x00436810 firing 1961 times. Same navigation up to race
            # entry; the only divergence after it is that statenav keeps driving
            # input and this runner did not. So the arena round does not advance
            # on its own from where `--scenario race` returns.
            #
            # Control cycle copied verbatim from statenav's round loop so the two
            # paths differ in as little as possible.
            drive = [4, 0, 1, 11, 12, 5, 6, 7]
            print(f"\n  dwelling {dwell:.0f}s, DRIVING input (a passive dwell leaves "
                  f"the round un-started — probes read 0)...")
            end = time.time() + dwell
            di, dt = 0, 0.0
            while time.time() < end and nav.alive():
                if time.time() - dt > 6:
                    di = (di + 1) % len(drive); dt = time.time()
                try: nav_scr.exports_sync.press(drive[di], 600)
                except Exception: pass
                time.sleep(0.5)
            if not nav.alive():
                print("ABORT: process exited during the dwell — lower --dwell.")
                return 4
        statenav.shoot(pid, ROOT / shotdir / "sb_diffpoint.png")

        # ── PROOF THE RACE IS ACTUALLY RUNNING ────────────────────────────────
        # Do NOT judge this from a screenshot. Measured 2026-07-29: the diff-point
        # capture is a BLANK WHITE WINDOW in-race even when the race is provably
        # running — a stock statenav control hit 0x00436810 1961 times and reached
        # results at t+20s while its own t+10s screenshot was equally white. The
        # menu captures fine, the race does not; the window grab misses the D3D9
        # backbuffer. A white shot is evidence of nothing either way, and I wrongly
        # called six boots' worth of runs "a hung game" on the strength of one.
        #
        # The instrument that DOES answer it is an entry counter on a function only
        # in-race code reaches (memory feedback_evidence_discipline §1: prove the
        # path ran). 0x00436810 is the reference — thousands of hits per race in the
        # stock control, and it is counted by RVA so our inline JMPs do not hide it.
        try:
            nav_scr.exports_sync.countthese([f"0x{r:08x}" for r in inrace_probes])
            time.sleep(1.5)
            probe = nav_scr.exports_sync.counts() or {}
            total = sum(v for v in probe.values() if isinstance(v, int))
            print("\n  in-race probe (1.5s): " + ", ".join(
                f"{k}={v}" for k, v in sorted(probe.items())))
            if total == 0:
                print("  WARNING: no in-race probe fired — the diff point is NOT inside a "
                      "running race. Live-state verdicts from here would be meaningless.")
            else:
                print(f"  => race IS running ({total} calls in 1.5s)")
        except Exception as e:
            print(f"  in-race probe unavailable: {e}")

        baseline_snap = read_sentinels()
        print("\n  sentinel snapshot @ diff point:")
        for b, ws in baseline_snap:
            print(f"    0x{b:08x}: " + " ".join(
                f"0x{(v & 0xffffffff):08x}" if isinstance(v, int) else str(v) for v in ws))
        if not sentinel_nonzero():
            print("\nABORT: sentinel still ZERO — nav did not populate live state "
                  f"(reached={reached}). No hook in this batch can diff non-degenerately.")
            return 5

        # t0 = the moment live state became usable. Every hook's cumulative
        # offset is measured from here, because the in-race window (the game
        # self-exits partway through a round) is what bounds hooks-per-boot.
        t0 = time.time()
        print(f"\n{'#':>2} {'hook':32s} {'verdict':20s} {'sec':>5s} {'t+':>6s} {'phase':>5s}")
        for i, name in enumerate(order, 1):
            tag = "repeat_" if (repeat_first and i == len(order) and len(order) > len(names)) else ""
            t_hook = time.time()

            # Per-hook liveness gate. Poll (don't just sample once) — the chain
            # a hook needs may come up later than the batch-wide sentinel did.
            gate_tr = ""
            if HOOKS[name].get("state_gate"):
                deadline = time.time() + gate_wait
                while True:
                    gate_ok, gate_tr = eval_state_gate(nav_scr.exports_sync.peek,
                                                       HOOKS[name])
                    if gate_ok or time.time() >= deadline or not nav.alive():
                        break
                    time.sleep(0.25)
                if not gate_ok:
                    verdict = "SKIPPED-GATE-UNMET"
                    rows.append((i, name, tag, verdict, 0, True,
                                 nav.phase() if nav.alive() else "DEAD",
                                 time.time() - t_hook, time.time() - t0))
                    print(f"{i:>2} {name[:32]:32s} {verdict[:20]:20s} "
                          f"{time.time() - t_hook:5.1f} {time.time() - t0:6.1f}")
                    print(f"   gate: {gate_tr}")
                    continue

            # Harvest REAL arguments from the game for pointer-taking hooks.
            tests_override = None
            if HOOKS[name].get("capture_args"):
                spec = dict(HOOKS[name]["capture_args"])
                spec.setdefault("rva", HOOKS[name]["rva"])
                raw, cerr = capture_live_args(sess, nav, spec)
                good = validate_captured(nav_scr.exports_sync.peek, raw, spec)
                print(f"   capture 0x{spec['rva']:08x}: {len(raw)} distinct, "
                      f"{len(good)} still valid"
                      + (f" [{cerr}]" if cerr else ""))
                if len(good) < 2:
                    # Fewer than two usable live arguments means the run could
                    # only ever be degenerate or a stale-pointer AV. Say so
                    # rather than emitting a verdict about the port.
                    verdict = f"SKIPPED-NO-LIVE-ARGS ({len(raw)} seen/{len(good)} valid)"
                    rows.append((i, name, tag, verdict, 0, True,
                                 nav.phase() if nav.alive() else "DEAD",
                                 time.time() - t_hook, time.time() - t0))
                    print(f"{i:>2} {name[:32]:32s} {verdict[:20]:20s} "
                          f"{time.time() - t_hook:5.1f} {time.time() - t0:6.1f}")
                    continue
                tests_override = good
                print(f"   replaying live args: " +
                      ", ".join(f"0x{v:08x}" for v in good[:6]) +
                      (" ..." if len(good) > 6 else ""))

            if not no_scrub:
                scrubbed = scrub_x87(sess)
            results, err = run_one_hook(sess, nav, name, shotdir, tests_override)
            dt = time.time() - t_hook
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
            rows.append((i, name, tag, verdict, distinct, same, phase, dt, time.time() - t0))
            print(f"{i:>2} {name[:32]:32s} {verdict[:20]:20s} {dt:5.1f} "
                  f"{time.time() - t0:6.1f} {str(phase):>5s}")
            if not alive:
                print(f"   process died after {i} hooks / t+{time.time() - t0:.1f}s "
                      f"— remaining {len(order) - i} cannot run")
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
    if ran:
        per = [r[7] for r in ran]
        window = rows[-1][8]
        print(f"  in-race window used: {window:.1f}s over {len(rows)} hooks")
        print(f"  per-hook seconds: min {min(per):.1f}  median "
              f"{sorted(per)[len(per)//2]:.1f}  max {max(per):.1f}")
        print(f"  => throughput {len(ran)/window*60:.1f} verified hooks/min of window"
              if window > 0 else "")
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
