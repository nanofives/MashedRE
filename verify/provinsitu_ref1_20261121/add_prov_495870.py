# PROV-INSITU-REF1 side item (lead, run 53): 0x00495870 (static helper in re/match/s16/00495fe0.c) gets its equiv.py
# verdict line; NOT-PROVEN -> a PROVISIONAL.tsv row citing the first difference. Appends one row (refuses a duplicate).
from pathlib import Path
P = Path(__file__).resolve().parents[2] / "re" / "match" / "PROVISIONAL.tsv"
txt = P.read_text(encoding="utf-8")
if "\n0x00495870\t" in txt:
    raise SystemExit("0x00495870 already listed")
row = ["0x00495870", "FUN_00495870", "re/match/s16/00495fe0.c", "/O2 /GS /Ob1", "98.9", "NOT-PROVEN",
       "indirect-call-esp-reset: orig: ESP restored from a register in a function with a call through "
       "ld32(M00(),(ld32(M00(),ld32(M00(),esi_in))+0x64)). equiv.py tool commit 1ed7ceb1a (= orch/integration a9e5b4c8a copy, no diff)",
       "NOT-WIRED (fix_joypad boot-patch detour site, MASHED.exe 0x00495870; the PROV-INSITU harness cannot place a JMP there)",
       "fxch residue (match.py 98.9%, 380/380 ins); DIFFERENT unit in ddkobj after RACE-BIND-C4-BULK cp1 (lead)",
       "not run", "provisional", "verify/provinsitu_ref1_20261121/RESULT_PROV-INSITU-REF1.md"]
if not txt.endswith("\n"):
    txt += "\n"
P.write_text(txt + "\t".join(row) + "\n", encoding="utf-8")
print("appended")
