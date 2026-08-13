#!/usr/bin/env python3
"""Decompile PC (MASHED.exe) functions via Ghidra headless — no MCP required.

Acquires a read-only pool slot, runs analyzeHeadless ONCE for all requested
addresses, prints the results, then releases the slot.

Examples:
  py -3.12 re\\tools\\decomp_pc.py 0x004a4541
  py -3.12 re\\tools\\decomp_pc.py 0x004a4541 0x00495870 --callees
  py -3.12 re\\tools\\decomp_pc.py --file rvas.txt --json -o out.json
  py -3.12 re\\tools\\decomp_pc.py 0x004a4541 --slot 3        # reuse a held slot

Notes:
  - Batch your addresses. One invocation costs ~30-60 s of project-open regardless
    of how many functions you ask for, so N-in-one is nearly free vs N runs.
  - Always -readOnly -noanalysis against a pool clone; never the master Mashed.gpr.
  - Addresses are absolute VAs as used throughout this repo (image base 0x400000),
    e.g. 0x004a4541 — not base-relative offsets.
"""
import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]          # re/tools -> re -> repo root
GH = Path(r"C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra"
          r"\ghidra_12.0.3_PUBLIC\support\analyzeHeadless.bat")
POOL_DIR = ROOT / "mashed_pool"
POOL_SH = ROOT / "scripts" / "ghidra_pool.sh"
SCRIPTS = Path(__file__).resolve().parent / "ghidra_scripts"
PROGRAM = "MASHED.exe"


def acquire_slot():
    """Return (slot_name, slot_index). Raises on failure."""
    res = subprocess.run(["bash", str(POOL_SH), "acquire"],
                         capture_output=True, text=True, timeout=300)
    lines = [ln.strip() for ln in res.stdout.splitlines() if ln.strip()]
    if res.returncode != 0 or not lines:
        sys.stderr.write(res.stdout + res.stderr)
        raise SystemExit("pool acquire failed")
    name = lines[-1]
    if not name.startswith("Mashed_pool"):
        sys.stderr.write(res.stdout + res.stderr)
        raise SystemExit(f"unexpected acquire output: {name!r}")
    return name, name[len("Mashed_pool"):]


def release_slot(idx):
    subprocess.run(["bash", str(POOL_SH), "release", str(idx)],
                   capture_output=True, text=True, timeout=300)


def run_headless(slot_name, vas, modes, verbose=False):
    tmp = Path(tempfile.mkdtemp(prefix="decomp_pc_"))
    manifest, out_json = tmp / "vas.txt", tmp / "out.json"
    manifest.write_text("\n".join(vas) + "\n", encoding="utf-8")

    # modes go as SEPARATE args: analyzeHeadless is a .bat and cmd.exe splits
    # arguments on commas, so a single "decomp,callees" string silently truncates
    # to "decomp". Never put a comma in a script arg.
    cmd = [str(GH), str(POOL_DIR), slot_name,
           "-process", PROGRAM, "-noanalysis", "-readOnly",
           "-scriptPath", str(SCRIPTS),
           "-postScript", "DecompPC.java", str(manifest), str(out_json), *modes]
    if verbose:
        sys.stderr.write(" ".join(cmd) + "\n")

    res = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)
    if not out_json.exists():
        # Ghidra reports script errors on stdout; surface the tail of both streams.
        sys.stderr.write(res.stdout[-4000:] + res.stderr[-4000:])
        raise SystemExit("headless produced no output (see trace above)")
    return json.loads(out_json.read_text(encoding="utf-8"))


def _oneline(s):
    """Keep list rows on one console line — RE strings often embed \\n (log formats)."""
    return (s.replace("\\", "\\\\").replace("\n", "\\n")
             .replace("\r", "\\r").replace("\t", "\\t"))


def render(data):
    out = []
    for fn in data.get("functions", []):
        req = fn.get("requested", "?")
        if "error" in fn:
            out.append(f"// {req}: ERROR {fn['error']}")
            for k in ("probe_block", "probe_at", "probe_bytes"):
                if k in fn:
                    out.append(f"//   {k[6:]}: {fn[k]}")
            if "probe_xrefs" in fn:
                items = fn["probe_xrefs"]
                out.append(f"//   xrefs ({len(items)}):" if items else "//   xrefs: (none)")
                out.extend(f"//     {_oneline(it)}" for it in items)
            out.append("")
            continue
        hdr = f"// {fn['name']} @ {fn['entry']}  size={fn['size']}  ({data['program']})"
        if not fn.get("exact_entry", True):
            hdr += f"\n// NOTE: {req} is not a function entry; showing containing function"
        out.append(hdr)
        out.append(f"// {fn['signature']}")
        # Short graph lists read fine inline; xrefs/strings are long, so one per line.
        for key in ("callees", "callers"):
            if key in fn:
                out.append(f"// {key}: " + ", ".join(fn[key] or ["(none)"]))
        for key in ("xrefs", "strings"):
            if key in fn:
                items = fn[key]
                if not items:
                    out.append(f"// {key}: (none)")
                    continue
                out.append(f"// {key} ({len(items)}):")
                out.extend(f"//   {_oneline(it)}" for it in items)
        if fn.get("decomp"):
            out.append(fn["decomp"])
        elif "decomp_error" in fn:
            out.append(f"// DECOMPILE FAILED: {fn['decomp_error']}")
        out.append("")
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("vas", nargs="*", help="addresses, hex (0x optional)")
    ap.add_argument("--file", help="file with one address per line")
    ap.add_argument("--callees", action="store_true", help="include callee list")
    ap.add_argument("--callers", action="store_true", help="include caller list")
    ap.add_argument("--xrefs", action="store_true",
                    help="all refs TO the entry, incl. DATA refs (vtable / fn-ptr table) "
                         "that --callers cannot see")
    ap.add_argument("--strings", action="store_true",
                    help="string literals referenced from the function body")
    ap.add_argument("--no-decomp", action="store_true",
                    help="skip decompilation (fast; pairs with --callees/--callers)")
    ap.add_argument("--slot", help="reuse an already-held pool slot index; not released")
    ap.add_argument("--json", action="store_true", help="emit raw JSON")
    ap.add_argument("-o", "--out", help="write to file instead of stdout")
    ap.add_argument("-v", "--verbose", action="store_true", help="echo the headless command")
    a = ap.parse_args()

    vas = list(a.vas)
    if a.file:
        for line in Path(a.file).read_text(encoding="utf-8").splitlines():
            t = line.strip()
            if t and not t.startswith("#"):
                vas.append(t)
    if not vas:
        ap.error("no addresses given (positional args or --file)")
    if not GH.exists():
        raise SystemExit(f"analyzeHeadless not found: {GH}")

    modes = (([] if a.no_decomp else ["decomp"])
             + (["callees"] if a.callees else [])
             + (["callers"] if a.callers else [])
             + (["xrefs"] if a.xrefs else [])
             + (["strings"] if a.strings else [])) or ["decomp"]

    if a.slot is not None:
        slot_name, idx, owned = f"Mashed_pool{a.slot}", a.slot, False
    else:
        slot_name, idx = acquire_slot()
        owned = True
    sys.stderr.write(
        f"[decomp_pc] slot={slot_name} modes={'+'.join(modes)} n={len(vas)}\n")

    try:
        data = run_headless(slot_name, vas, modes, a.verbose)
    finally:
        if owned:
            release_slot(idx)

    text = json.dumps(data, indent=2) if a.json else render(data)
    if a.out:
        Path(a.out).write_text(text, encoding="utf-8")
        sys.stderr.write(f"[decomp_pc] wrote {a.out}\n")
    else:
        print(text)


if __name__ == "__main__":
    main()
