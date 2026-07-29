#!/usr/bin/env py -3.12
"""prescreen_batch.py — screen many STATE candidates for "is this exercised in
the target scenario", a chunk per boot, accumulating results.

WHY
---
Authoring a STATE port for a function the scenario never calls is pure waste:
2026-07-28 four candidates were disassembled, ported, built, registered and
diffed before anything revealed Quick Battle never touches any of them. A
pre-screen costs one boot per ~24 candidates and answers it first. Measured
2026-07-29 on the first 24: 14 exercised (58%), 5 of them race-gated.

THE RULE THIS SCRIPT ENFORCES
-----------------------------
Every chunk carries the three VALIDATED in-race probes. They read 0 at title,
track-confirm and start-attempt and 11,266 / 2,867 / 59,690 during a race, so a
chunk whose probes stay 0 never reached a race and its zeros mean nothing. Such
a chunk is recorded VOID and retried once — never merged into the results. The
first hand-run screen hit exactly this (phase=3 throughout, first_results_at=
None) and would have reported 33 candidates as "never exercised" when all it
measured was boot+menu.

Chunks are kept small: Interceptor.attach on a hot path destabilises MASHED in
about six seconds (CLAUDE.md) and a candidate's call rate is unknown until you
measure it.

VOID CHUNKS ARE AN INTERCEPTOR-OVERHEAD PROBLEM, NOT A WEDGED DRIVER.
I originally read two consecutive void chunks as the documented ~15-boot
d3d9/GPU wedge and told the user to reboot. That was WRONG, and the run order
already disproved it: a 48-probe screen failed, a 24-probe screen IMMEDIATELY
AFTER succeeded, three more 24-probe chunks succeeded, then two whose candidate
sets contain hot RenderWare functions failed the same way. A wedged driver does
not heal itself for the next boot. The failures track the probe SET, not elapsed
boots — statenav armed all counters before dev.resume, so their Interceptors ran
through the entire menu navigation and the nav's fixed waits timed out.
Fixed by MASHED_COUNT_LATE=1 (set below): the same chunk that "wedged" twice
then navigated fine with no reboot.
--max-void still stops the run, because repeated void chunks mean something is
wrong worth looking at — just not a driver to reboot.

Usage:
  py -3.12 scripts/prescreen_batch.py --candidates <tsv> --out <tsv>
      [--chunk 24] [--round 70] [--skip-done <tsv>] [--max-boots 8]
"""
import io
import os
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROBES = ["0x00470670", "0x0047eb30", "0x004233e0"]   # validated in-race, see prescreen_recipe.md

BASE_RE = re.compile(r"\[counts @start-attempt0\]\s+(.*)")
EXER_RE = re.compile(r"EXERCISED in-race:\s+\d+/\d+\s+->\s+\[(.*)\]")
PHASE_RE = re.compile(r"FINAL:.*first_results_at=(\S+)")


def _flag(name, default=None, cast=str):
    return cast(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else default


def run_chunk(rvas, round_s, shotdir):
    """One boot. Returns (baseline: dict, exercised: set, void: bool, note: str)."""
    env = dict(os.environ)
    env["MASHED_COUNT_RVAS"] = ",".join(rvas + PROBES)
    # Arm the probes AFTER the race is entered. Arming before resume leaves every
    # Interceptor live through the whole menu navigation, and that — not any
    # driver degradation — is what stalled the failing chunks. See below.
    env["MASHED_COUNT_LATE"] = "1"
    cmd = [sys.executable, str(ROOT / "re/frida/statenav.py"),
           "--round", str(round_s), "--shot-dir", shotdir]
    try:
        p = subprocess.run(cmd, cwd=str(ROOT), env=env, capture_output=True,
                           text=True, timeout=round_s + 240)
        out = p.stdout + p.stderr
    except subprocess.TimeoutExpired:
        return {}, set(), True, "TIMEOUT"

    base = {}
    m = BASE_RE.search(out)
    if m:
        for tok in m.group(1).split():
            if "=" in tok:
                k, v = tok.split("=", 1)
                try: base[k] = int(v)
                except ValueError: pass
    exer = set()
    m = EXER_RE.search(out)
    if m:
        exer = {x.strip().strip("'\"") for x in m.group(1).split(",") if x.strip()}

    # THE GATE: did this boot actually reach a race?
    probes_fired = sum(1 for p in PROBES if p in exer)
    if probes_fired == 0:
        fr = PHASE_RE.search(out)
        return base, exer, True, f"VOID no probe fired (first_results_at={fr.group(1) if fr else '?'})"
    return base, exer, False, f"ok ({probes_fired}/3 probes)"


def main():
    cand_path = _flag("--candidates", "re/analysis/plans/prescreen_candidates_all.txt")
    out_path  = _flag("--out", "re/analysis/plans/prescreen_result_all.tsv")
    chunk_n   = _flag("--chunk", 24, int)
    round_s   = _flag("--round", 70, int)
    max_boots = _flag("--max-boots", 8, int)
    max_void  = _flag("--max-void", 2, int)

    meta = {}
    order = []
    for line in io.open(ROOT / cand_path, encoding="utf-8"):
        if not line.strip(): continue
        rva, sz, sub = (line.rstrip("\n").split("\t") + ["", ""])[:3]
        meta[rva] = (sz, sub); order.append(rva)

    done = set()
    skip = _flag("--skip-done", None)
    if skip and (ROOT / skip).exists():
        for line in io.open(ROOT / skip, encoding="utf-8"):
            if line.startswith("0x"): done.add(line.split("\t")[0])
    todo = [r for r in order if r not in done]
    print(f"candidates={len(order)} already-screened={len(done)} todo={len(todo)} "
          f"chunk={chunk_n} -> {(len(todo)+chunk_n-1)//chunk_n} boots")

    outf = io.open(ROOT / out_path, "a", encoding="utf-8")
    if (ROOT / out_path).stat().st_size == 0:
        outf.write("rva\tclass\tsize\tsubsystem\tchunk\n"); outf.flush()

    voids = 0
    for i in range(0, len(todo), chunk_n):
        if (i // chunk_n) >= max_boots:
            print(f"stopping: --max-boots {max_boots} reached"); break
        rvas = todo[i:i + chunk_n]
        tag = f"c{i//chunk_n}"
        print(f"\n=== chunk {tag}: {len(rvas)} candidates", flush=True)
        base, exer, void, note = run_chunk(rvas, round_s, f"verify/prescreen_all_{tag}")
        print(f"    {note}", flush=True)
        if void:
            voids += 1
            print(f"    DISCARDED (void {voids}/{max_void}) — zeros from this boot mean nothing",
                  flush=True)
            if voids >= max_void:
                print("    stopping: consecutive void chunks. NOT a driver to reboot — "
                      "check probe overhead first (MASHED_COUNT_LATE, smaller --chunk); "
                      "resume with --skip-done", flush=True)
                break
            continue
        voids = 0
        for r in rvas:
            sz, sub = meta.get(r, ("", ""))
            if r in exer:
                # With MASHED_COUNT_LATE the probes are armed AFTER race entry, so
                # there is no pre-race baseline to compare against and everything
                # observed is in-race by construction. Do not label these
                # race_gated — that claim needs the baseline, and asserting it
                # from a run that cannot see the menu would be inventing evidence.
                cls = "exercised_inrace" if not base else (
                    "race_gated" if base.get(r, 0) == 0 else "exercised_prerace")
            else:
                cls = "never"
            outf.write(f"{r}\t{cls}\t{sz}\t{sub}\t{tag}\n")
        outf.flush()
        got = [r for r in rvas if r in exer]
        print(f"    exercised {len(got)}/{len(rvas)} "
              f"(race-gated {sum(1 for r in got if base.get(r,0)==0)})", flush=True)
    outf.close()
    print(f"\nwrote {out_path}")


if __name__ == "__main__":
    main()
