"""MATCH-S19-FU-PCFUNCS: add Ghidra-missed functions to re/console/pc_funcs.csv with the file's own generator.

One analyzeHeadless session on a read-only pool clone:
  1. DecompPC.java <rvas> <json> metadata create   -> defines each missing function transiently
  2. DumpFunctions.java <dump.csv>                 -> the generator of re/console/pc_funcs.csv
The clone is opened -readOnly, so nothing is persisted. Only the requested RVAs' rows are taken
from the dump (the rest of pc_funcs.csv is left as generated on 2026-06-12); the dump is kept
so the RESULT can say how far the full table has drifted.

    py -3.12 verify/matchs19fupcfuncs_20261106/tools/addrows.py <out_dir> 0x004c07b0 0x004c0830 ...
"""
import csv
import json
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import decomp_pc  # noqa: E402

FUNCS = ROOT / "re" / "console" / "pc_funcs.csv"
# pool clones live in the main checkout (scripts/ghidra_pool.sh PROJECT_ROOT); decomp_pc.POOL_DIR is worktree-relative
POOL_DIR = pathlib.Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool")
DUMP_SCRIPTS = ROOT / "re" / "tools" / "console" / "ghidra_scripts"


def main():
    out = pathlib.Path(sys.argv[1]).resolve()
    rvas = [f"0x{int(v, 16):08x}" for v in sys.argv[2:]]
    out.mkdir(parents=True, exist_ok=True)
    manifest, js, dump = out / "rvas.txt", out / "create.json", out / "dump_pc_funcs.csv"
    manifest.write_text("\n".join(rvas) + "\n", encoding="utf-8")
    # one script dir: a ';'-joined -scriptPath would be split by cmd.exe (analyzeHeadless is a .bat)
    sdir = out / "scripts"
    sdir.mkdir(exist_ok=True)
    for src in (decomp_pc.SCRIPTS / "DecompPC.java", DUMP_SCRIPTS / "DumpFunctions.java"):
        (sdir / src.name).write_bytes(src.read_bytes())

    slot, idx = decomp_pc.acquire_slot()
    print(f"[addrows] slot {slot}", flush=True)
    try:
        cmd = [str(decomp_pc.GH), str(POOL_DIR), slot,
               "-process", decomp_pc.PROGRAM, "-noanalysis", "-readOnly",
               "-scriptPath", str(sdir),
               "-postScript", "DecompPC.java", str(manifest), str(js), "metadata", "create",
               "-postScript", "DumpFunctions.java", str(dump)]
        print(" ".join(cmd), flush=True)
        res = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)
        (out / "headless.log").write_text(res.stdout + res.stderr, encoding="utf-8")
    finally:
        decomp_pc.release_slot(idx)
        print(f"[addrows] released {slot}", flush=True)
    if not js.exists() or not dump.exists():
        raise SystemExit(f"headless produced no output, see {out / 'headless.log'}")

    created = {f["requested"].lower(): f for f in json.loads(js.read_text(encoding="utf-8"))["functions"]}
    dumped = {r["entry"].lower(): r for r in csv.DictReader(open(dump, newline=""))}
    raw = FUNCS.read_bytes().decode("ascii")
    eol = "\r\n" if "\r\n" in raw else "\n"           # keep the checkout line endings
    lines = raw.splitlines()
    have = {ln.split(",")[0].lower() for ln in lines[1:] if ln.strip()}
    print(f"[addrows] dump rows {len(dumped)}, pc_funcs.csv rows {len(have)}, "
          f"in dump not in csv {len(set(dumped) - have)}, in csv not in dump {len(have - set(dumped))}")

    new = []
    for k in rvas:
        c = created.get(k, {})
        r = dumped.get(k)
        print(f"  {k} created_transiently={c.get('created_transiently')} exact={c.get('exact_entry')} "
              f"name={c.get('name')} size={c.get('size')} err={c.get('error') or c.get('create_failed')} "
              f"dump={'none' if r is None else (r['body_end'], r['body_size'], r['name'])}")
        if k in have:
            print(f"  {k} already in pc_funcs.csv, skipped")
            continue
        if r is None:
            raise SystemExit(f"{k}: no row in the dump; not adding anything")
        new.append(f"{r['entry']},{r['body_end']},{r['body_size']},{r['name']}")
    if not new:
        print("[addrows] nothing to add")
        return
    body = sorted(lines[1:] + new, key=lambda ln: int(ln.split(",")[0], 16))
    FUNCS.write_bytes((eol.join([lines[0]] + body) + eol).encode("ascii"))
    (out / "added_rows.csv").write_text("\n".join([lines[0]] + new) + "\n", encoding="utf-8")
    print(f"[addrows] added {len(new)} row(s) to {FUNCS}")


if __name__ == "__main__":
    main()
