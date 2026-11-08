"""MATCH-PCFUNCS-REGEN: run DumpFunctions.java (the pc_funcs.csv generator) on a read-only pool clone.
    py -3.12 verify/matchpcfuncsregen_20261107/tools/dump.py <out_dir>
Writes <out_dir>/dump_pc_funcs.csv. Master project is not opened."""
import pathlib, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import decomp_pc
POOL_DIR = pathlib.Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool")
SRC = ROOT / "re" / "tools" / "console" / "ghidra_scripts" / "DumpFunctions.java"
out = pathlib.Path(sys.argv[1]).resolve(); out.mkdir(parents=True, exist_ok=True)
sdir = out / "scripts"; sdir.mkdir(exist_ok=True)
(sdir / SRC.name).write_bytes(SRC.read_bytes())
dump = out / "dump_pc_funcs.csv"
slot, idx = decomp_pc.acquire_slot()
print("slot", slot, flush=True)
try:
    cmd = [str(decomp_pc.GH), str(POOL_DIR), slot, "-process", decomp_pc.PROGRAM, "-noanalysis", "-readOnly",
           "-scriptPath", str(sdir), "-postScript", "DumpFunctions.java", str(dump)]
    res = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)
    (out / "headless.log").write_text(res.stdout + res.stderr, encoding="utf-8")
finally:
    decomp_pc.release_slot(idx); print("released", slot)
print(dump.exists())
