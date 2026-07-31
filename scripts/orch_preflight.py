#!/usr/bin/env python3
"""Pre-flight a hook BEFORE spending a game boot on it.

Twelve game boots were spent across orchestrator iters 12-18 to promote eight
rows. FOUR of those boots produced no verdict at all, and three of the four came
from a single class: the arg_type handler reads a CONFIG key that nothing
supplies, so the Frida agent dies before it runs a single test vector.
`observe_callee` vs `observe_callee_str` cost two boots by itself, and one of
those two was spent fixing the WRONG thing because the batch log truncated the
error at twenty characters.

Every one of those was detectable locally in milliseconds. This is that check.

THE LOAD-BEARING IDEA: a handler's required CONFIG keys are DERIVED from the
handler's own source, not from a hand-maintained list. Brace-match the
`if (CONFIG.arg_type === '<name>') { ... }` block in diff_template.js, collect
every `CONFIG.<key>` inside it, and classify each as required or optional by
whether it is ever read WITHOUT a `||` default / `?` guard. A hand-written list
would drift from the harness the first time somebody adds a handler; this cannot.

Then resolve each required key three ways, because run_diff.py sometimes BUILDS a
config key from a differently-named registry field:
  1. present in the registry entry            -> ok
  2. built by run_diff.py from hook['other']  -> ok if 'other' is in the entry
  3. neither                                  -> FAIL, with the fix named

THE EARLY-WINDOW LANE IS A SECOND HARNESS and is checked too (orch-iter21).
early_window_leaf_diff.py has its own dispatch gate (PURE_LEAF_ARGTYPES) and
builds its cfg from an EXPLICIT ALLOWLIST of `'k': h.get('k')` pairs. A registry
key absent from that allowlist is silently dropped: the handler sees `undefined`,
which reads as "feature off" rather than as an error, so the run still produces a
verdict - just not the one you think. `key_off` did exactly that and produced a
GREEN 4/4 that a port returning a constant 0 would equally have passed.

That check runs in the direction that actually bites: every key THIS ENTRY sets
must be delivered. The opposite direction ("every cfg.<key> the handler reads
must be forwarded") needs an accurate brace-match of a JS block embedded in a
Python string, which strip_js - written for a .js file - gets wrong; the first
attempt emitted ~20 false FAILs on already-green rows. A checker that cries wolf
is worse than no checker.

Also checks, cheapest first: library-band membership (never a port target),
callee hazards read out of the PORT SOURCE (a debug-printf callee makes a
function unverifiable by force-call, which cost a boot in iter18), sentinel
presence, and test-vector shape.

Usage:
  py -3.12 scripts/orch_preflight.py <hook_name> [<hook_name>...]
  py -3.12 scripts/orch_preflight.py --all-authored     # every impl row
Exit code 0 = clear to boot, 1 = at least one FAIL.
"""
import csv
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
AGENT = ROOT / "re/frida/diff_template.js"
EARLY_WINDOW = ROOT / "re/frida/early_window_leaf_diff.py"


def early_window_argtypes():
    """Names in PURE_LEAF_ARGTYPES — the early-window harness's dispatch gate.

    Parsed from source rather than imported: importing that module is not free and
    this checker must stay cheap enough to run before every boot."""
    try:
        src = EARLY_WINDOW.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return set()
    m = re.search(r"PURE_LEAF_ARGTYPES\s*=\s*\{(.*?)\n\}", src, re.S)
    if not m:
        return set()
    return set(re.findall(r"'([a-z0-9_]+)'", m.group(1)))


def early_window_cfg_forwarded():
    """{cfg_key: registry_key} the early-window driver actually FORWARDS.

    That driver builds its cfg from an EXPLICIT ALLOWLIST of `'k': h.get('k')`
    pairs. A registry key absent from it is silently dropped and the handler sees
    `undefined`, which reads as "feature off" rather than as an error — so the run
    still produces a verdict, just not the one you think. This cost two false
    GREENs in orch-iter21: `key_off` was in the entry AND in the handler and did
    nothing, so esi_global_search seeded the key at entry+0 while the target
    compared at +0x44, every vector missed and BOTH SIDES RETURNED 0. That is the
    early-window twin of the run_diff.py forwarding bug this script was written
    for."""
    try:
        src = EARLY_WINDOW.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return {}
    # Brace-match the dict rather than regexing to the first `}` on its own line:
    # the cfg literal contains nested structures, so a non-greedy match truncates
    # it and silently under-reports what is forwarded — which would turn this
    # checker into a false-FAIL generator, the one failure mode that guarantees it
    # gets ignored.
    start = src.find("cfg = {")
    if start < 0:
        return {}
    brace = src.find("{", start)
    depth, i, n = 0, brace, len(src)
    while i < n:
        if src[i] == "{":
            depth += 1
        elif src[i] == "}":
            depth -= 1
            if depth == 0:
                break
        i += 1
    m = re.match(r"(?s)(.*)", src[brace:i + 1])
    out = {}
    for cfg_k, hook_k in re.findall(
            r"'([A-Za-z_][A-Za-z0-9_]*)'\s*:\s*h\.get\(\s*'([A-Za-z_][A-Za-z0-9_]*)'",
            m.group(1)):
        out[cfg_k] = hook_k
    # a few are taken straight off h[...] rather than h.get(...)
    for cfg_k, hook_k in re.findall(
            r"'([A-Za-z_][A-Za-z0-9_]*)'\s*:\s*h\[\s*'([A-Za-z_][A-Za-z0-9_]*)'\s*\]",
            m.group(1)):
        out.setdefault(cfg_k, hook_k)
    return out


RUNDIFF = ROOT / "re/frida/run_diff.py"
HOOKS_CSV = ROOT / "hooks.csv"

sys.path.insert(0, str(ROOT / "re/frida"))
sys.path.insert(0, str(ROOT / "scripts"))
from hooks_registry import HOOKS                      # noqa: E402
from orch_rank_gate import library_band               # noqa: E402

# A callee with one of these roles makes the target unverifiable, or its writes
# non-comparable, by a direct synthetic force-call. Matched against the callee's
# hooks.csv row. Each entry is (pattern, why).
CALLEE_HAZARDS = [
    (r"OutputDebugString", "debug-print SEH exception surfaces under Frida — "
                           "both sides error identically (iter18, 0x00482900)"),
    (r"\bdebug printf\b",  "debug-print wrapper; see OutputDebugString"),
    (r"QueryPerformance|\bQPC\b",
                           "timer read — value differs between the two calls"),
    (r"\bmalloc\b|\balloc\b|RwFreeList|freelist",
                           "returns a heap address — differs per run"),
    (r"DestroyWindow|ChangeDisplaySettings",
                           "destroys the window/device"),
]


def strip_js(src):
    """Blank out comments and string bodies so brace matching is reliable."""
    out, i, n = [], 0, len(src)
    while i < n:
        c = src[i]
        two = src[i:i + 2]
        if two == "//":
            j = src.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i)); i = j
        elif two == "/*":
            j = src.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join(ch if ch == "\n" else " " for ch in src[i:j]))
            i = j
        elif c in "'\"`":
            j = i + 1
            while j < n and src[j] != c:
                j += 2 if src[j] == "\\" else 1
            j = min(j + 1, n)
            out.append(src[i] + " " * (j - i - 2) + src[i] if j - i >= 2
                       else " " * (j - i))
            i = j
        else:
            out.append(c); i += 1
    return "".join(out)


def handler_block(arg_type):
    """Return (raw_block, start_line) for one arg_type, or (None, None)."""
    raw = AGENT.read_text(encoding="utf-8")
    blank = strip_js(raw)
    assert len(blank) == len(raw), "strip_js must preserve offsets"
    # Search RAW: strip_js blanks string BODIES, so the arg_type literal only
    # survives in the original text. Brace-matching still uses the blanked copy,
    # which is the half that needs braces-in-strings suppressed.
    m = re.search(r"CONFIG\.arg_type\s*===\s*'%s'" % re.escape(arg_type), raw)
    if not m:
        return None, None
    brace = blank.find("{", m.end())
    if brace < 0:
        return None, None
    depth, i, n = 0, brace, len(blank)
    while i < n:
        if blank[i] == "{":
            depth += 1
        elif blank[i] == "}":
            depth -= 1
            if depth == 0:
                break
        i += 1
    return raw[brace:i + 1], raw[:m.start()].count("\n") + 1


# CONFIG.<key> read with a default or a guard on the same line -> optional.
GUARDED = [
    r"CONFIG\.%s\s*\|\|", r"CONFIG\.%s\s*\?", r"CONFIG\.%s\s*!==\s*undefined",
    r"\(\s*CONFIG\.%s\s*\|\s*0\s*\)\s*\|\|", r"if\s*\(\s*CONFIG\.%s\s*\)",
]


def config_keys(block):
    """{key: 'required'|'optional'} for one handler block."""
    keys = {}
    for k in sorted(set(re.findall(r"CONFIG\.([A-Za-z_][A-Za-z0-9_]*)", block))):
        if k in ("arg_type", "tests"):
            continue
        guarded_hits = sum(len(re.findall(p % re.escape(k), block))
                           for p in GUARDED)
        total = len(re.findall(r"CONFIG\.%s\b" % re.escape(k), block))
        keys[k] = "optional" if guarded_hits >= total else "required"
    return keys


def rundiff_mapping():
    """config['X'] = hook['Y']  ->  {X: Y}, including the identity loops."""
    src = RUNDIFF.read_text(encoding="utf-8")
    mapping = {}
    for x, y in re.findall(r"config\['([^']+)'\]\s*=\s*hook\['([^']+)'\]", src):
        mapping[x] = y
    for x, y in re.findall(
            r"config\['([^']+)'\]\s*=\s*f\"[^\"]*hook\['([^']+)'\]", src):
        mapping[x] = y
    # Transformed forwards: the assignment is an expression over hook['Y']
    # rather than a bare read — e.g. a list comprehension normalising ints to
    # hex strings (stub_at, iter20). Matching only the bare form reported those
    # as "never forwarded", which is a FALSE FAIL and exactly as expensive as a
    # missed one: it trains you to ignore the checker.
    # The bound + DOTALL is deliberate: such a forward is often wrapped across
    # lines, but an unbounded .*? would happily span half the file and invent a
    # mapping from one forward to an unrelated hook[...] far below it.
    for x, y in re.findall(
            r"config\['([^']+)'\]\s*=\s*(?!hook\['|f\").{0,200}?hook\['([^']+)'\]",
            src, re.S):
        mapping.setdefault(x, y)
    for k in re.findall(r"config\[_k\]\s*=\s*hook\[_k\]", src):
        pass  # identity loops handled below
    for grp in re.findall(r"for _k in \(([^)]*)\):", src, re.S):
        for k in re.findall(r"'([^']+)'", grp):
            mapping.setdefault(k, k)
    return mapping


def hooks_rows():
    rows = {}
    with HOOKS_CSV.open(newline="", encoding="utf-8", errors="replace") as f:
        for r in csv.DictReader(f):
            rva = (r.get("rva") or "").strip().lower()
            if rva:
                rows[rva] = r
    return rows


def check(name, mapping, csv_rows):
    fails, warns, notes = [], [], []
    if name not in HOOKS:
        return ["not in hooks_registry.HOOKS"], [], []
    h = HOOKS[name]
    at = h.get("arg_type")
    rva = h.get("rva")

    # 1. library band — cheapest disqualifier, and never a port target.
    band = library_band("0x%08x" % rva) if isinstance(rva, int) else None
    if band:
        fails.append("RVA 0x%08x is in library band '%s' — not a port target"
                     % (rva, band))

    # 2. handler exists at all (run_diff.py refuses at runtime; catch it here)
    block, line = handler_block(at) if at else (None, None)
    if at in (None, "none", "void"):
        notes.append("arg_type %r needs no handler" % at)
    elif block is None and at in early_window_argtypes():
        # The EARLY-WINDOW lane is a second harness with its own dispatch table
        # (PURE_LEAF_ARGTYPES in early_window_leaf_diff.py). Pre-flight only ever
        # looked in diff_template.js, so it FAILED every early-window row - including
        # ones already promoted through that lane, e.g. veh_tbl_8820a0_get3. Not a
        # defect in the row; a blind spot in this checker. (orch-iter21.)
        notes.append("arg_type %r is EARLY-WINDOW only - run it with "
                     "`py -3.12 re/frida/early_window_leaf_diff.py <hook>`, NOT run_diff.py "
                     "or run_diff_scenario_batch (they only know diff_template.js)" % at)
        # 3b. THE FORWARDING CHECK, in its high-signal direction: every key THIS
        # ENTRY sets must actually be delivered to the handler.
        #
        # Deliberately NOT "every cfg.<key> the handler reads must be forwarded".
        # That direction needs an accurate brace-match of a handler block living
        # inside a JS string inside a .py file, which strip_js (written for a .js
        # file) gets wrong - the first attempt at it emitted ~20 false FAILs on
        # already-green rows. A checker that cries wolf is worse than no checker;
        # this file's own history says so.
        #
        # The direction kept is the one that actually bit: a key present in the
        # entry and absent from the driver's allowlist is silently dropped, the
        # handler sees `undefined`, and that reads as "feature off" rather than as
        # an error - so the run still produces a verdict, just the wrong one.
        # `key_off` did exactly this in orch-iter21 and produced a GREEN 4/4 that a
        # port returning a constant 0 would also have passed.
        fwd = early_window_cfg_forwarded()          # {cfg_key: registry_key}
        delivered = set(fwd.values())
        STRUCTURAL = {"rva", "export", "signature", "arg_type", "path1_tests",
                      "path2_tests", "lut_root_delta", "scenario",
                      "scenario_sentinel", "notes"}
        for k in sorted(set(h) - STRUCTURAL - delivered):
            fails.append(
                "entry sets %r but the early-window driver never forwards it into cfg - "
                "it will be silently undefined and the handler will read that as "
                "'feature off', NOT as an error. Add \"'<cfg_key>': h.get('%s'),\" to the "
                "cfg dict in early_window_leaf_diff.py (or drop the key)." % (k, k))
    elif block is None:
        fails.append("arg_type %r has NO handler in EITHER harness "
                     "(diff_template.js or early_window_leaf_diff.py)" % at)
    else:
        notes.append("handler %r at diff_template.js:%d" % (at, line))
        # 3. every CONFIG key the handler reads must be resolvable
        for k, kind in sorted(config_keys(block).items()):
            src = mapping.get(k)
            # A registry field only reaches the agent if run_diff.py FORWARDS it.
            # Being present in the entry is NOT sufficient — that is precisely
            # the iter14 bug: the entry set 'observe_callee_str', which
            # run_diff.py never forwards (it builds that key from
            # 'observe_callee'), so CONFIG.observe_callee_str was undefined and
            # the agent died on ptr(undefined). Check the delivery path, not the
            # spelling.
            if src is not None and src in h:
                continue
            if k in h:
                msg = ("registry entry sets %r but run_diff.py never forwards "
                       "it to CONFIG — it will NOT reach the agent" % k)
                if src is not None:
                    msg += "; CONFIG.%s is built from hook[%r]" % (k, src)
            elif src is not None:
                msg = ("handler reads CONFIG.%s; run_diff.py builds it from "
                       "hook[%r], which the entry lacks — set %r"
                       % (k, src, src))
            else:
                msg = ("handler reads CONFIG.%s; run_diff.py does not forward "
                       "any such key" % k)
            (fails if kind == "required" else warns).append(msg)

    # 4. sentinel — the batch refuses without one; that cost a boot in iter12
    if "scenario_sentinel" not in h:
        warns.append("no scenario_sentinel — run_diff_scenario_batch will "
                     "refuse ('without a gate a 0-mismatch run proves nothing')")

    # 5. test-vector shape
    t1 = h.get("path1_tests") or []
    # Non-degeneracy can come from EITHER of two places, and only counting vectors
    # sees one of them:
    #   (a) ACROSS vectors  — distinct inputs produce distinct fingerprints;
    #   (b) WITHIN one observation — several observed slots carrying DIFFERENT
    #       expected values, so one call already discriminates.
    # (b) is a real and used pattern here. 0x0040ba60 observes out=[1,0,1,0], so a
    # port returning all-1 or all-0 fails on a single vector. 0x00477b40 uses a
    # boundary echo — the region goes to 0 while a byte past its end keeps a
    # sentinel — so zeroing too far AND not far enough both fail at once. Flagging
    # those as "cannot be non-degenerate" is a FALSE FAIL, and a checker that cries
    # wolf gets ignored.
    # The exemption is deliberately narrow: >= 2 observed slots. One observed slot
    # and one vector really is one observation, which is what eax_ecx_insert was
    # doing before cfg.eax_from_test (orch-iter21).
    obs_slots = 0
    for key in ("observe_addrs", "observe", "out_observe", "eax_observe",
                "ecx_observe", "abs_observe", "obs_globals"):
        v = h.get(key)
        if isinstance(v, (list, tuple)):
            obs_slots = max(obs_slots, len(v))
    for t in t1:                      # per-test obs lists (cache_setter_observe)
        if isinstance(t, dict) and isinstance(t.get("obs"), (list, tuple)):
            obs_slots = max(obs_slots, len(t["obs"]))
    if len(t1) < 2 and obs_slots >= 2:
        notes.append("only %d test vector(s), but %d observed slots — "
                     "non-degeneracy must come from DIFFERENT expected values across "
                     "those slots (e.g. a boundary echo, or a mixed out-vector). "
                     "Verify that from the evidence CSV; the vector count alone "
                     "cannot." % (len(t1), obs_slots))
    elif len(t1) < 2:
        fails.append("path1_tests has %d vector(s) and %d observed slots; cannot be "
                     "non-degenerate — one call observing one slot is a single "
                     "observation" % (len(t1), obs_slots))
    elif len({repr(x) for x in t1}) < 2:
        fails.append("all %d path1_tests are identical" % len(t1))

    # 6. callee hazards, read out of the PORT SOURCE. Preflight runs after
    #    authoring and before booting, which is exactly when the port's
    #    hardcoded callee addresses are available to check.
    row = csv_rows.get(("%08x" % rva).lower()) if isinstance(rva, int) else None
    src_path = (row or {}).get("file", "")
    port = None
    if src_path.endswith(".cpp") and (ROOT / src_path).exists():
        port = ROOT / src_path
    else:
        # hooks.csv only points `file` at the .cpp once the row is PROMOTED, but
        # preflight's whole purpose is to run before that. Fall back to locating
        # the port by its export name, which RH_ScopedInstall requires to be
        # unique anyway.
        exp = h.get("export")
        if exp:
            # Match the INSTALL SITE, not any mention of the name. A sibling
            # port that merely references the export — in a comment, or as a
            # typedef'd call target — used to win on rglob order, and then the
            # callee-hazard scan below read the wrong file's literals. Seen
            # iter20: replay_time_format resolved to ReplayGetTimeAtIdx.cpp.
            # Fall back to a plain name match only if no install site is found,
            # so a port not yet wired up still gets scanned.
            inst = re.compile(r"RH_ScopedInstall\s*\(\s*%s\s*," % re.escape(exp))
            name = re.compile(r"\b%s\b" % re.escape(exp))
            fallback = None
            for p in (ROOT / "mashedmod/src/mashed_re").rglob("*.cpp"):
                text = p.read_text(encoding="utf-8", errors="replace")
                if inst.search(text):
                    port = p
                    break
                if fallback is None and name.search(text):
                    fallback = p
            if port is None:
                port = fallback
    if port:
        notes.append("port %s" % port.relative_to(ROOT).as_posix())
        text = port.read_text(encoding="utf-8", errors="replace")
        for lit in sorted(set(re.findall(r"0x00([0-9a-fA-F]{6})\b", text))):
            if lit.lower() == ("%06x" % (rva & 0xffffff)):
                continue
            # hooks.csv keys are the full 8-hex RVA; the literal captured here
            # is its low 6 nibbles.
            crow = csv_rows.get(("00" + lit).lower())
            if not crow:
                continue
            note = " ".join(crow.get(k, "") or "" for k in ("name", "notes"))
            for pat, why in CALLEE_HAZARDS:
                if re.search(pat, note, re.I):
                    # A hazard the entry has already NEUTRALISED is not a
                    # warning. stub_at Interceptor.replaces the callee's entry,
                    # so it never executes and cannot raise anything. Reporting
                    # it anyway would leave the mitigated case looking identical
                    # to the unmitigated one — which trains you to skip the
                    # line, and the next real hazard with it.
                    stubbed = {("0x%08x" % a) if isinstance(a, int)
                               else str(a).lower()
                               for a in (h.get("stub_at") or [])}
                    if ("0x00" + lit).lower() in stubbed:
                        notes.append("callee 0x00%s (%s): %s — NEUTRALISED, "
                                     "it is in stub_at and never runs"
                                     % (lit, crow.get("name", "?"), why))
                        break
                    warns.append("callee 0x00%s (%s): %s"
                                 % (lit, crow.get("name", "?"), why))
                    break
    return fails, warns, notes


def main(argv):
    names = argv[1:]
    if not names:
        print(__doc__); return 2
    csv_rows = hooks_rows()
    if names == ["--all-authored"]:
        names = sorted(n for n, h in HOOKS.items()
                       if (csv_rows.get(("%08x" % h["rva"]).lower(), {})
                           .get("status") == "impl"))
    mapping = rundiff_mapping()
    bad = 0
    for n in names:
        fails, warns, notes = check(n, mapping, csv_rows)
        status = "FAIL" if fails else ("WARN" if warns else "OK")
        print("\n[%s] %s" % (status, n))
        for x in notes:
            print("     - %s" % x)
        for x in warns:
            print("  ~  %s" % x)
        for x in fails:
            print("  X  %s" % x)
        if fails:
            bad += 1
    print("\n%d checked, %d FAIL" % (len(names), bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
