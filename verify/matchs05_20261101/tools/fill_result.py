import pathlib
p = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-S05\verify\matchs05_20261031\RESULT_MATCH-S05.md")
t = p.read_text(encoding="utf-8")
t = t.replace("@@VERDICT@@", "**GREEN.** 118 of 128 functions matched byte for byte; 53 matched sources are now the port body of their RVA\n"
              "(22 in both targets, 18 of them replacing an existing exe body); the default gates are unchanged on that exe.")
t = t.replace("@@GATES@@", "on the exe with all 53 swaps (commit `af19355a4`, exe sha256/16 `795de6dc9e7a5727`): G-CTRL\n"
              "  `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9`, equal to `merge_train.json` gctrl.ref / b.ref.\n"
              "  No gate hash moved, so no DECISION is needed for any swap.")
t = t.replace("@@GATETABLE@@",
              "| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs ref |\n"
              "|---|---|---|---|---|---|\n"
              "| 1 | all 53 swaps, commit af19355a4 | `795de6dc9e7a5727` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |\n\n"
              "H_DEF exit 0 self-exited after 520 s, each B arm exit 0 after 43-44 s.")
t = t.replace("S05's 44 entries sit in a sorted block", "S05's entries (16 in exe, 44 in asi) sit in a sorted block")
raw = """```
9cd0ca52067f90a6a5f11eb4468d5e255f95df0216477bcbb0433da8d3f19448  decomp.json (decomp_pc.py --port --callees, 128 functions)
e6a3d0a397406efc70e747fcf1c6ef5b20e27c84c56c25353aff082069b7340d  bodies.tsv (pre-swap body scan, tools/bodies.py)
795de6dc9e7a5727e73e1d9c0f21fea4afe3d3869701843854f8bd4a0987caf7  exe_g1/mashed_re.exe
80b4e002590de3141731f02db23e338d62b3d49a04e2c3b357e889913a2926b0  exe_g1/mashed_re_dev.asi
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K{0,1,26}.step.csv
8dd030119830441e7063f04b223a334786072bd793e662f0125aaa59e2592a7b  arms_g1/H_DEF.log
a8b84de4d871f5aa663cf7249267931414c79ddd66cab9e0097e16d97b0be88f  build1.log
98eda7c990c3c0f3e9eb0616fb259649a79c95b37c9249a12e56680c592e1253  gates_g1.log
```
Also there: `g2s/` (raw converter output for all 128), `dup_sources/` (14 duplicate pair files), `notes/`, `tools/`."""
t = t.replace("@@RAW@@", raw)
assert "@@" not in t
p.write_text(t, encoding="utf-8")
