# MATCH-S14 WIP (stopped run 5, 2026-11-02, on the lead's STOP)

Leg reached: **hand-match (static) in progress; no swap, no build, no gates, no tracker edits.**

- Slice 0x004840d0..0x0048cee0, 128 functions (`slice_rvas.txt`). decomp_pc.py --port --callees (slot Mashed_pool0,
  released by the tool), g2s raw converter: 35 at 100%, all relocheck OK (`raw100_check.txt`).
- Hand-matching by helpers h1..h7 (`BRIEF.md`, `helper_assign.txt`, `notes/h*.md`): 62 more rows self-reported 100%
  when stopped (h2 finished; h1, h3..h7 stopped mid-list). **Not yet re-verified by the lead** (match.py + relocheck
  + cxxcheck per file owed). Claimed total 97/128.
- Out of scope this run: 0x004854e0, 0x00486830, 0x00487280, 0x004883b0, 0x00489500 (>900 B, no port body).
- 45 slice RVAs have port bodies (`D:/MashedWT/MATCH-S14_data/bodies_pre.tsv`); exe bodies: Race/WorldRegistry.cpp
  RG_*, Race/WorldObjectLinks.cpp, Util/WorldObjectsBase.cpp, Physics/SmplFzxStateBlock.cpp.
- Gap row 0x00485c20 (gap_value.tsv): h3 note line is the status.
- Swap leg ready to run: `SWAP_BRIEF.md`, tools in `tools/` (rspadd anchor `Race\WorldObjectLinks.cpp`, x87add
  anchor `WarzonePontoon`). Partial files: some `.c` under `re/match/s14/` from stopped helpers may be below 100%.

Resume: re-verify every re/match/s14/*.c (collect.py -> results.tsv), finish h1/h3..h7 lists, then swap/build/lint.
Raw data: D:/MashedWT/MATCH-S14_data (decomp.json, g2s/, notes/, scratch/).
bdb693b22bbef88fc2d7c6830c7fe4ef098e97b33c3262477131716ebb21f462 *D:/MashedWT/MATCH-S14_data/decomp.json
a435f1d3a85427ac3014481aa09db4ac3de5f8094035a1e14e99f9d78372b4c5 *D:/MashedWT/MATCH-S14_data/g2s/results.tsv
