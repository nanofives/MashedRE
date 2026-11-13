"""IMG-4-GHIDRA-DATAREF (L-149 (a) step 4): Ghidra data-reference export of the file-backed image.

Runs re/tools/ghidra_scripts/DataRefExport.java headless on a read-only pool clone (-readOnly -noanalysis, nothing
persisted) over the image source ranges of RESULT_PORTSTATE-IMAGE.md section 3:
    0x005cc000..0x00637000  (.rdata + file-backed .data)
    0x00915000..0x00915008  (the file-backed tail)
and writes the raw export (every memory reference from those ranges into an executable block, with Ghidra's data
type at the source).

    py -3.12 re/tools/image/ghidra_dataref_export.py --raw re/tools/image/dataref_raw.tsv [--slot N]

The selection into CODEPTR_IMAGE.tsv is a separate step: re/tools/image/codeptr_image.py.
"""
import argparse
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import decomp_pc as dp  # noqa: E402  (pool acquire/release, analyzeHeadless path)

# In a git worktree the pool lives in the main checkout (mashed_pool/ is not tracked).
if not dp.POOL_DIR.exists():
    _common = subprocess.run(["git", "-C", str(HERE), "rev-parse", "--path-format=absolute", "--git-common-dir"],
                             capture_output=True, text=True).stdout.strip()
    if _common:
        _main = Path(_common).parent
        dp.POOL_DIR = _main / "mashed_pool"
        dp.POOL_SH_ARG = str(_main / "scripts" / "ghidra_pool.sh").replace("\\", "/")

RANGES = [(0x005CC000, 0x00637000), (0x00915000, 0x00915008)]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--raw", required=True, help="output TSV")
    ap.add_argument("--slot", help="reuse a held pool slot index (not released)")
    a = ap.parse_args()
    out = Path(a.raw).resolve()
    if a.slot is not None:
        name, idx, own = f"Mashed_pool{a.slot}", a.slot, False
    else:
        (name, idx), own = dp.acquire_slot(), True
    try:
        tmp = Path(tempfile.mkdtemp(prefix="dataref_")) / "raw.tsv"
        cmd = [str(dp.GH), str(dp.POOL_DIR), name, "-process", dp.PROGRAM, "-noanalysis", "-readOnly",
               "-scriptPath", str(dp.SCRIPTS), "-postScript", "DataRefExport.java", str(tmp)]
        for lo, hi in RANGES:
            cmd += [f"{lo:08x}", f"{hi:08x}"]
        res = subprocess.run(cmd, capture_output=True, text=True, timeout=3600)
        if not tmp.exists():
            sys.stderr.write(res.stdout[-4000:] + res.stderr[-4000:])
            raise SystemExit("headless produced no output")
        for ln in res.stdout.splitlines():
            if "DataRefExport:" in ln:
                print(ln.strip())
        out.write_bytes(tmp.read_bytes())
        print(f"slot {name}; wrote {out}")
    finally:
        if own:
            dp.release_slot(idx)


if __name__ == "__main__":
    main()
