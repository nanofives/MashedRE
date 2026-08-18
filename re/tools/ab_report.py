# ab_report.py — batch A/B capture comparison for the 16-shot verify set.
#
# WHY THIS EXISTS: every RESULT.md table under verify/ was assembled BY HAND from
# individual imgdiff.py runs (audited 2026-08-18). That is slow, error-prone, and
# it produced tables whose numbers nobody could reproduce with one command. This
# tool runs the comparison over a whole capture directory and emits the table.
#
# It is a REPORTER, not a verdict authority. Acceptance for composition work is
# still the draw-list diff (re/tools/drawlist_diff.py) per
# re/analysis/parity_tooling.md — pixel percentages localize a divergence, they
# do not accept a change.
#
# The percentage this reports as "diff%" is PIXELS OVER THRESHOLD, matching the
# figure quoted throughout verify/**/RESULT.md (e.g. "71.61%"), NOT the mean.
# imgdiff.py's own default threshold of 16 is preserved.
#
# Usage:
#   # compare two existing capture dirs
#   py -3.12 re/tools/ab_report.py --a verify/run_1234 --b verify/run_5678 \
#       --label-a d3d9 --label-b librw --out verify/d1_recheck
#
#   # capture both arms first, then compare (arm B adds MASHED_RENDER_LIBRW=1)
#   py -3.12 re/tools/ab_report.py --capture --out verify/d1_recheck
#
#   # same-build control pair: capture twice with IDENTICAL env, expect 0.00%
#   py -3.12 re/tools/ab_report.py --capture --control --out verify/d1_control
#
# Outputs, all under --out:
#   REPORT.md        the table, sorted by divergence descending
#   heat/<shot>.png  amplified abs-diff heatmap, only for shots over --heat-min
#   png/<shot>.{a,b}.png  PNG copies of the diverging shots, for viewing in chat
#
# --share additionally copies REPORT.md and the png/ set into the Happy share
# folder so they can be surfaced to a remote user.
import argparse
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

import numpy as np
from PIL import Image

REPO = Path(__file__).resolve().parents[2]

# The capture driver is compiled into the exe (mashedmod/src/mashed_re/exe_main.cpp);
# these env vars are the documented recipe. MASHED_GOTO=6 is a MANDATORY pairing
# with MASHED_RACE_DEMO=1 — either alone writes nothing (CHANGELOG 2026-08-xx
# records two wasted attempts from getting this wrong). MASHED_DET_FRAMES must
# stay a FRAME count; replacing it with a wall-clock kill reintroduced the
# nondeterminism R10b closed.
CAPTURE_ENV = {
    "MASHED_DETERMINISTIC": "1",
    "MASHED_DET_FRAMES": "3000",
    "MASHED_RACE_DEMO": "1",
    "MASHED_GOTO": "6",
    "MASHED_DRIVE_HOLD": "1",
    "MASHED_DRIVE_DEMO": "1",
    "MASHED_WIN_POS": "left-bl",
}

EXE = REPO / "mashedmod" / "build" / "mashed_re.exe"
SHARE = REPO / ".happy-share" / "cmsykiciq0b3dn51chwq1bssl"


def shots(root: Path) -> dict:
    """Map shot name -> path, over the three capture blocks the driver writes."""
    found = {}
    for block in ("race1", "r5", "r6"):
        d = root / block
        if not d.is_dir():
            continue
        for bmp in sorted(d.glob("*.bmp")):
            found[f"{block}/{bmp.stem}"] = bmp
    # Tolerate captures written flat rather than into blocks.
    for bmp in sorted(root.glob("*.bmp")):
        found.setdefault(bmp.stem, bmp)
    return found


def compare(pa: Path, pb: Path, threshold: int):
    """Return (diff_pct, mean_all, per-channel means, diff array uint8)."""
    a = Image.open(pa).convert("RGB")
    b = Image.open(pb).convert("RGB")
    resampled = False
    if b.size != a.size:
        b = b.resize(a.size, Image.BILINEAR)
        resampled = True
    aa = np.asarray(a, dtype=np.int16)
    bb = np.asarray(b, dtype=np.int16)
    diff = np.abs(aa - bb).astype(np.uint8)
    over = int((diff.max(axis=2) > threshold).sum())
    n = diff.shape[0] * diff.shape[1]
    means = diff.reshape(-1, 3).mean(axis=0)
    return {
        "pct": 100.0 * over / n,
        "mean": float(means.mean()),
        "means": [float(m) for m in means],
        "diff": diff,
        "size": a.size,
        "resampled": resampled,
    }


def run_capture(out_dir: Path, librw: bool, timeout: int) -> int:
    """Spawn one capture arm. Returns exit code.

    PROCESS HYGIENE (CLAUDE.md): we track only the PID we spawn and never kill by
    name — a blanket kill has destroyed another session's capture before. On
    timeout we terminate THIS pid only.
    """
    if not EXE.is_file():
        sys.exit(f"missing {EXE} — build first with mashedmod\\build.bat")
    env = dict(os.environ)
    env.update(CAPTURE_ENV)
    # The exe resolves MASHED_VERIFY_OUT against its own cwd (= REPO), so hand it
    # a repo-relative path whether the caller passed an absolute or relative one.
    try:
        rel = out_dir.resolve().relative_to(REPO)
    except ValueError:
        sys.exit(f"--out must live under the repo ({REPO}), got {out_dir}")
    env["MASHED_VERIFY_OUT"] = str(rel).replace("\\", "/")
    if librw:
        env["MASHED_RENDER_LIBRW"] = "1"
    else:
        env.pop("MASHED_RENDER_LIBRW", None)

    arm = "librw" if librw else "d3d9"
    print(f"[capture:{arm}] -> {out_dir}")
    t0 = time.time()
    proc = subprocess.Popen([str(EXE)], cwd=str(REPO), env=env)
    print(f"[capture:{arm}] pid {proc.pid}")
    try:
        # WaitForExit BEFORE reading the code: the boot AV reports 0xFFFFFFFF,
        # the same signature as a force-kill, so an early read misdiagnoses it.
        rc = proc.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        print(f"[capture:{arm}] TIMEOUT after {timeout}s, terminating pid {proc.pid}")
        proc.kill()
        proc.wait()
        return -2
    print(f"[capture:{arm}] exit {rc} in {time.time() - t0:.1f}s")
    return rc


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--a", type=Path, help="capture dir, arm A (reference)")
    ap.add_argument("--b", type=Path, help="capture dir, arm B (under test)")
    ap.add_argument("--label-a", default="A")
    ap.add_argument("--label-b", default="B")
    ap.add_argument("--out", type=Path, required=True, help="report output dir")
    ap.add_argument("--threshold", type=int, default=16,
                    help="per-pixel max-channel diff counted as different "
                         "(imgdiff.py default; the quoted verify/ figures use 16)")
    ap.add_argument("--capture", action="store_true",
                    help="run both arms first (arm B sets MASHED_RENDER_LIBRW=1)")
    ap.add_argument("--control", action="store_true",
                    help="with --capture, run arm B with IDENTICAL env — a "
                         "same-build control pair, which must come back 0.00%%")
    ap.add_argument("--timeout", type=int, default=300, help="per-arm seconds")
    ap.add_argument("--heat-min", type=float, default=0.5,
                    help="only emit heatmap/PNG for shots at or above this %%")
    ap.add_argument("--share", action="store_true",
                    help="copy report + PNGs into the Happy share folder")
    args = ap.parse_args()

    out = args.out
    out.mkdir(parents=True, exist_ok=True)

    dir_a, dir_b = args.a, args.b
    if args.capture:
        dir_a = out / "arm_a"
        dir_b = out / "arm_b"
        if run_capture(dir_a, librw=False, timeout=args.timeout) != 0:
            sys.exit("arm A capture failed")
        if run_capture(dir_b, librw=not args.control, timeout=args.timeout) != 0:
            sys.exit("arm B capture failed")
    if not dir_a or not dir_b:
        sys.exit("need --a and --b, or --capture")

    sa, sb = shots(dir_a), shots(dir_b)
    common = sorted(set(sa) & set(sb))
    only_a = sorted(set(sa) - set(sb))
    only_b = sorted(set(sb) - set(sa))
    if not common:
        sys.exit(f"no shots in common between {dir_a} and {dir_b}")

    (out / "heat").mkdir(exist_ok=True)
    (out / "png").mkdir(exist_ok=True)

    rows = []
    for name in common:
        r = compare(sa[name], sb[name], args.threshold)
        flat = name.replace("/", "_")
        if r["pct"] >= args.heat_min:
            Image.fromarray(np.minimum(r["diff"].astype(np.int16) * 4, 255)
                            .astype(np.uint8)).save(out / "heat" / f"{flat}.png")
            Image.open(sa[name]).convert("RGB").save(out / "png" / f"{flat}.a.png")
            Image.open(sb[name]).convert("RGB").save(out / "png" / f"{flat}.b.png")
        rows.append((name, r))

    rows.sort(key=lambda kv: kv[1]["pct"], reverse=True)
    worst = rows[0]
    n_par = sum(1 for _, r in rows if r["pct"] < args.heat_min)

    lines = []
    lines.append(f"# A/B capture report: {args.label_a} vs {args.label_b}")
    lines.append("")
    lines.append(f"- arm A (`{args.label_a}`): `{dir_a}`")
    lines.append(f"- arm B (`{args.label_b}`): `{dir_b}`")
    lines.append(f"- shots compared: **{len(rows)}**"
                 + (f" (A-only: {len(only_a)}, B-only: {len(only_b)})"
                    if only_a or only_b else ""))
    lines.append(f"- threshold: {args.threshold} "
                 f"(diff% = pixels over threshold, the figure quoted in verify/)")
    lines.append(f"- **worst: `{worst[0]}` at {worst[1]['pct']:.2f}%**")
    lines.append(f"- at or under {args.heat_min}%: {n_par} of {len(rows)}")
    if args.control:
        lines.append("")
        lines.append("**CONTROL PAIR** — identical env on both arms. Any nonzero "
                     "figure here is harness noise, not a renderer difference, "
                     "and invalidates a same-session A/B until explained.")
    lines.append("")
    lines.append("| shot | diff% | mean | R | G | B |")
    lines.append("|---|---:|---:|---:|---:|---:|")
    for name, r in rows:
        lines.append(f"| `{name}` | {r['pct']:.2f} | {r['mean']:.2f} | "
                     f"{r['means'][0]:.2f} | {r['means'][1]:.2f} | {r['means'][2]:.2f} |")
    if only_a or only_b:
        lines.append("")
        lines.append("## Unpaired shots")
        for n in only_a:
            lines.append(f"- only in A: `{n}`")
        for n in only_b:
            lines.append(f"- only in B: `{n}`")
    if any(r["resampled"] for _, r in rows):
        lines.append("")
        lines.append("Note: at least one pair differed in size and B was "
                     "bilinear-resampled to A, which inflates edge differences.")
    lines.append("")
    lines.append("Reminder: all pre-2026-08-16 `verify/` stills are horizontally "
                 "mirrored relative to current output. Diffing against them reads "
                 "30-45% and means nothing.")
    lines.append("")
    lines.append("Pixel percentages localize a divergence. They do not accept a "
                 "change: acceptance is `drawlist_diff.py` per "
                 "`re/analysis/parity_tooling.md`.")
    report = "\n".join(lines) + "\n"
    (out / "REPORT.md").write_text(report, encoding="utf-8")
    print(report)
    print(f"-> {out / 'REPORT.md'}")

    if args.share:
        SHARE.mkdir(parents=True, exist_ok=True)
        stamp = out.name
        shutil.copy2(out / "REPORT.md", SHARE / f"{stamp}_REPORT.md")
        for p in sorted((out / "png").glob("*.png")):
            shutil.copy2(p, SHARE / f"{stamp}_{p.name}")
        for p in sorted((out / "heat").glob("*.png")):
            shutil.copy2(p, SHARE / f"{stamp}_heat_{p.name}")
        print(f"-> shared into {SHARE}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
