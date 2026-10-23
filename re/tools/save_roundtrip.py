"""savelink round trip (verify/savelink_20261023/PREREG_savelink_roundtrip.md).

Arms C / P / S boot the ORIGINAL (private copy, MASHED_ORIG_DIR) to the mode-select menu with a given
gamesave.bin and dump the live 13x12 table at 0x007f0a40. Arm R makes the original write its own save
(SerializeToBuffer + SaveWrite) and parses it with the standalone's ParseImage (savefmt_cli).

    py -3.12 re/tools/save_roundtrip.py --root D:/MashedWT/savelink_root --data D:/MashedWT/savelink_data
"""
import argparse, ast, hashlib, os, re, shutil, subprocess, sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent.parent
SPAN, ROWS, COLS, STRIDE = 0x24A40, 13, 12, 0x30


def span_cells(path):
    d = Path(path).read_bytes()
    return [[int.from_bytes(d[SPAN + r * STRIDE + c * 4: SPAN + r * STRIDE + c * 4 + 4], "little")
             for c in range(COLS)] for r in range(ROWS)]


def run_probe(orig_dir, plan, log):
    env = dict(os.environ, MASHED_ORIG_DIR=str(orig_dir))
    p = subprocess.run([sys.executable, str(REPO / "re/frida/nav_champ_probe.py"),
                        "--plan", plan], env=env, capture_output=True, text=True, timeout=240)
    Path(log).write_text(p.stdout + "\n--stderr--\n" + p.stderr, encoding="utf-8")
    rows = {}
    for m in re.finditer(r"^\s{2}row\s*(\d+): (\[.*\])\s*$", p.stdout, re.M):
        rows[int(m.group(1))] = ast.literal_eval(m.group(2))
    return [rows.get(r) for r in range(ROWS)], p.stdout


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", required=True)
    ap.add_argument("--data", required=True)
    a = ap.parse_args()
    orig = Path(a.root) / "original"
    data = Path(a.data)
    gs = orig / "gamesave.bin"
    ship = data / "shipped.bin"
    if not ship.exists():
        shutil.copy2(gs, ship)
    cli = data / "savefmt_cli.exe"
    shipped_sha = hashlib.sha256(ship.read_bytes()).hexdigest()
    main_orig = REPO.parent.parent / "original" / "gamesave.bin"
    out = []
    P = lambda *x: (print(*x), out.append(" ".join(str(i) for i in x)))

    P("shipped sha256", shipped_sha[:16])
    # arm files
    subprocess.run([sys.executable, str(REPO / "re/tools/gamesave_edit.py"), str(ship), "-o", str(data / "arm_P.bin"),
                    "--rows", "2", "--set", "c1=1"], check=True, capture_output=True)
    subprocess.run([str(cli), "build", str(ship), str(data / "arm_S.bin"), "7", "2", "1", "1"], check=True)
    arms = {}
    for name in ("C", "P", "S"):
        shutil.copy2(data / ("shipped.bin" if name == "C" else f"arm_{name}.bin"), gs)
        cells, _ = run_probe(orig, "dumptable", data / f"arm_{name}.log")
        arms[name] = cells
        P(f"arm {name}: {'dumped' if all(cells) else 'INCOMPLETE'}  r2c1={cells[2][1] if cells[2] else None}")
    verdict = {}
    shipped_cells = span_cells(ship)
    verdict["P0"] = arms["C"] == shipped_cells
    diffs = [(r, c) for r in range(ROWS) for c in range(COLS) if arms["C"][r] and arms["P"][r] and arms["C"][r][c] != arms["P"][r][c]]
    verdict["P1"] = diffs == [(2, 1)] and arms["P"][2][1] == 1
    sdiff = [(r, c, arms["P"][r][c], arms["S"][r][c]) for r in range(ROWS) for c in range(COLS)
             if arms["P"][r] and arms["S"][r] and arms["P"][r][c] != arms["S"][r][c]]
    verdict["P2"] = not sdiff and all(arms["S"])
    P("P0 control dump == shipped span:", verdict["P0"])
    P("P1 positive control moved exactly r2c1:", verdict["P1"], "diff cells", diffs)
    P("P2 subject == positive control on all 156 cells:", verdict["P2"], "diff cells", sdiff)
    # P4: standalone ParseImage of S
    q = subprocess.run([str(cli), "parse", str(data / "arm_S.bin")], capture_output=True, text=True).stdout
    P("P4 parse S:", q.splitlines()[0], q.splitlines()[2])
    # arm R
    shutil.copy2(data / "arm_P.bin", gs)
    rows, stdout = run_probe(orig, "savewrite", data / "arm_R.log")
    written = data / "arm_R_written.bin"
    shutil.copy2(gs, written)
    r = subprocess.run([str(cli), "parse", str(written)], capture_output=True, text=True)
    P("arm R written size", written.stat().st_size, "| parse:", r.stdout.splitlines()[0])
    wc = span_cells(written)
    verdict["P3"] = (written.stat().st_size == 0x24FA0 and r.returncode == 0 and rows and all(rows) and wc == rows)
    P("P3 original-written file parses and span == live table:", verdict["P3"])
    # guard: the shared original/ save must be untouched
    if main_orig.exists():
        P("shared original/gamesave.bin sha256", hashlib.sha256(main_orig.read_bytes()).hexdigest()[:16])
    (data / "roundtrip_result.txt").write_text("\n".join(out) + "\n", encoding="utf-8")
    gs.write_bytes(ship.read_bytes())          # leave the private root pristine
    return 0 if all(verdict.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
