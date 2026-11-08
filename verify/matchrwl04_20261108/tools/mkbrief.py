V = 'verify/matchrwl04_20261108/'
s = open(V + 'BRIEF.md').read()
i = s.index('## Slice additions')
s = s[:i]
bs = chr(92)
s = s.replace('re/match/s24', 're/match/rwl04').replace('re' + bs + 'match' + bs + 's24', 're' + bs + 'match' + bs + 'rwl04')
s = s.replace('D:/MashedWT/MATCH-RWL04_data/tools/to_s24.py', 'verify/matchrwl04_20261108/tools/to_s24.py')
s = s.replace('D:' + bs + 'MashedWT' + bs + 'MATCH-RWL04_data' + bs + 'tools' + bs + 'cxxcheck.bat', 'verify' + bs + 'matchrwl04_20261108' + bs + 'tools' + bs + 'cxxcheck.bat')
s = s.replace('MATCH-S24', 'MATCH-RWL04')
s += '''## RWL04 additions (this overrides anything above that says S24)
- Slice 0x005412d0..0x005540d0, 126 rows of the RenderWare toolkit library (third-party-library[renderware]). Raw data dir `D:/MashedWT/MATCH-RWL04_data`
  (g2s/, decomp.json, tools/). Sources `re/match/rwl04/<va8>.c` already exist for every row (raw converter output in the
  required form, via verify/matchrwl04_20261108/tools/to_s24.py). 28 rows are already 100% with relocheck OK: leave them.
  Your list is in your task (smallest first). Where the text above says S24 / s24, read RWL04 / rwl04. Slices S22/S23/S24 data
  dirs are NOT yours: use D:/MashedWT/MATCH-RWL04_data.
- Flags: `// FLAGS: /O2 /Ob1 /G5` (28 of 28 raw 100% rows need it; /O2 /GS /Ob1 and /O1 win only on low raw forms). If a row stalls on
  prologue/cookie/byte-op residue, try the others once and record which won on line 1.
- Read `git show orch/MATCH-RWCORE-G5:verify/matchrwcoreg5_20261107/flags_brief.md` (last two sections) for residual classes
  that no flag or source form moves (x87 operand order, shrink-wrapped pushes, sibling loop reloads): class (a), move on.
- C++ check: `cmd /c "verify\\matchrwl04_20261108\\tools\\cxxcheck.bat re\\match\\rwl04\\<va8>.c"` from the PowerShell tool, run on
  every file you touch at the end (DDK score first; casts that do not change the DDK bytes are free: typedef `code`
  under `__cplusplus`, real function-pointer types, explicit casts, correct callee arity).
- No-size rows (not in re/console/pc_funcs.csv): score with `py -3.12 verify/matchrwl04_20261108/tools/nosize.py re/match/rwl04/<va8>.c 0x<va>`.
- Several rows are tiny (8-30 B): usually a thunk, a one-store setter or a tail jump (`mov r,[g]; jmp [r+k]`). Read `odis.py`
  first, then write the C that gives those bytes; many of the 25% rows are the raw converter adding a wrong prototype/arity.
- Scratch to `D:/MashedWT/MATCH-RWL04_data/scratch/<id>/`. Notes to `D:/MashedWT/MATCH-RWL04_data/notes/<id>.md`.
- Append one line to your notes file as you finish each function. Budget for your whole list: about 2 h wall; report what you have by then.
'''
open(V + 'BRIEF.md', 'w').write(s)
