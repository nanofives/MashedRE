"""MATCH-RWT-S5 Fable attempt: write the 100% body (f4.py variant t, minus the unused pvb local) into
re/match/rwl01/004dcbb0.c. Keeps the file from `// FLAGS:` to the `// MATCH:` marker, replaces the comments above."""
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
SRC = ROOT / "re" / "match" / "rwl01" / "004dcbb0.c"
ns = {"__file__": str(HERE / "f4.py")}
exec((HERE / "f4.py").read_text(), ns)
body = ns["VARIANTS"]["t"]
assert "  int **pvb;\n" in body
body = body.replace("  int **pvb;\n", "")

COMMENT = """/* MATCH-RWT-S5 (Fable 5.2 high, one attempt) 2026-11-13: 100.0% byte match (119/119 ins, regs 100, ops 100) under
   ddk (cl 13.00.9176 / c2 13.00.9178) and rw9044, flags /O2 /Ob1 /G5. Source shape that closed it:
   (1) the slot index DAT_007d70c8 is used directly at every site, with no local copy. c2 then forms the index web
       itself: load at 0x004dcbbb (mov ebp,[0x7d70c8]), found path 0x004dcc27 mov ebp,ecx / 0x004dcc2c
       mov [0x7d70c8],ebp, reloads after the pn stores (0x004dcc7b) and after the call (0x004dcc96).
   (2) end is a variable (off + size, or size when off is 0: 0x004dcc4a mov ebx,edi), kept in ebx.
   (3) the pn loop key is a local loaded before the loop (0x004dcc5f mov ecx,[ebp*4+0x7d70ec] precedes the null
       test at 0x004dcc68); the tail uses DAT_007d70ec[DAT_007d70c8] inline in each branch (0x004dcc9f
       mov ebp,[ebp*4+0x7d70ec], then mov ecx,[ebp] / mov eax,[ebp] per branch).
   (4) the entry block reads param_1 once through `*(volatile uint *)&param_1` (0x004dcbb0 mov ecx,[esp+4]) and uses
       that copy for the size product and the first round-up; the loop and the tail read param_1 itself, which c2
       leaves in its home slot (0x004dcc00 mov ebx,[esp+0x14] per pass, 0x004dccf2 div dword ptr [esp+0x14]).
       param_1 is never written, so the copy holds the same value. With a plain copy or param_1 everywhere, c2 gives
       param_1 a callee-saved register and the index stays in memory (41.2%).
   History: RWL01 77.5% and the permuter's 77.5% measured a source that stored the OLD index on the found path
   (void, S3). S3 fixed the store (62.0%), S5 attempt 2 reached 83.2% with a local index copy.
   Probes: verify/match_20261113/MATCH-RWT-S5_probes/ (f1..f4), RESULT: verify/match_20261113/RESULT_MATCH-RWT-S5.md */
"""
text = SRC.read_text()
i0 = text.index("// FLAGS: /O2 /Ob1 /G5")
i1 = text.index("// MATCH: 0x004dcbb0")
mid = text[i0:i1]
old = [ln for ln in mid.splitlines(True) if ln.startswith("/* MATCH-RWL01 0x004dcbb0:")]
assert len(old) == 1
mid = mid.replace(old[0], "/* MATCH-RWL01 0x004dcbb0: real prototype is cdecl 5 args (the PC image reads [esp+4]..[esp+0x14]); "
                          "the vtable call at +0x2c is __stdcall (callee pops, no add esp after 0x004dccbb / 0x004dccde). */\n")
SRC.write_text(COMMENT + mid + "// MATCH: 0x004dcbb0 FUN_004dcbb0\n" + body, newline="\n")
print("written", SRC, len(body))
