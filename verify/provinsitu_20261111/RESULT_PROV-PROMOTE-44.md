# RESULT PROV-PROMOTE-44 (run 38): L-128 frida-replay C4 promotions

Source evidence: `verify/provinsitu_20261111/RESULT_PROV-INSITU.md` and `PROV_VERDICTS.tsv` (commit bf44aae7d).
Scenario: one race, track 0, QuickRace, 4 cars, 2,724 ticks, replay reference race, DDK body live in the
original through an inline JMP (`re/tools/prov/provlink.py`).

Legs, valid for every row in the table below (columns of PROV_VERDICTS.tsv):
IDENTICAL-FOR-2724: every compared tick of the race equals the reference with the body live (verdict column).
install-check: ok (install column = ok, the inline JMP was installed at the RVA).
body-executed: yes, calls_run column > 0 in the race window (counts in the table).
write-set: empty or covered (static capstone store list over the DDK code, L-38 / L-40 / L-54): 40 rows `empty`
  (no store outside the frame), 4 rows `covered` (stores only to absolute addresses inside the compared image).

| rva | source | install | calls_run | stores | write-set leg | verdict |
|---|---|---|---|---|---|---|
| 0x00414300 | re/match/s03/h4_00414300.c | ok | 3112 | frame 1 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0041d930 | re/match/s04/0041d930.c | ok | 2719 | frame 19 abs_in 1 abs_out 0 indirect 0 | covered | IDENTICAL-FOR-2724 |
| 0x0041fcc0 | re/match/s05/0041fcc0.c | ok | 8369 | frame 14 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00429310 | re/match/s06/00429310.c | ok | 2719 | frame 9 abs_in 6 abs_out 0 indirect 0 | covered | IDENTICAL-FOR-2724 |
| 0x00477e60 | re/match/s12/00477e60.c | ok | 35829 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00482ae0 | re/match/s13/00482ae0.c | ok | 10876 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00487020 | re/match/s14/00487020.c | ok | 14 | frame 4 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0048a4e0 | re/match/s14/0048a4e0.c | ok | 1 | frame 10 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0041f770 | re/match/s05/0041f770.c | ok | 10876 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0041faf0 | re/match/s05/0041faf0.c | ok | 8369 | frame 11 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00425ca0 | re/match/s05/00425ca0.c | ok | 21752 | frame 3 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00428450 | re/match/s06/00428450.c | ok | 107 | frame 12 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004584e0 | re/match/s09/004584e0.c | ok | 12090 | frame 3 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00458a40 | re/match/s09/00458a40.c | ok | 12090 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0045a190 | re/match/s09/0045a190.c | ok | 2719 | frame 11 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004647f0 | re/match/s10/004647f0.c | ok | 2885 | frame 0 abs_in 2 abs_out 0 indirect 0 | covered | IDENTICAL-FOR-2724 |
| 0x00474de0 | re/match/s11/00474de0.c | ok | 92453 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00474e70 | re/match/s12/00474e70.c | ok | 699 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004770a0 | re/match/s12/004770a0.c | ok | 127501 | frame 1 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00477f50 | re/match/s12/00477f50.c | ok | 294798 | frame 3 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0047cea0 | re/match/s13/0047cea0.c | ok | 145 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00480b70 | re/match/s13/00480b70.c | ok | 238 | frame 13 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0048aa50 | re/match/s14/0048aa50.c | ok | 98 | frame 2 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004c1a70 | re/match/s19/004c1a70.c | ok | 8166 | frame 2 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004c1b40 | re/match/s19/004c1b40.c | ok | 201522 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x004c4d20 | re/match/s19/004c4d20.c | ok | 226168 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00546b10 | re/match/s24/00546b10.c | ok | 11743 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005aa7a0 | re/match/s25/005aa7a0.c | ok | 16 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005ae3a0 | re/match/s26/005ae3a0.c | ok | 8 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00407be0 | re/match/s01/00407be0.c | ok | 10823 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00416060 | re/match/s03/h2_00416060.c | ok | 1331 | frame 1 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00458b10 | re/match/s09/00458b10.c | ok | 2721 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00484f70 | re/match/s14/00484f70.c | ok | 2719 | frame 4 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00556ca0 | re/match/s24/00556ca0.c | ok | 832 | frame 1 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005a6d60 | re/match/s25/005a6d60.c | ok | 135889 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005aaac0 | re/match/s25/005aaac0.c | ok | 4 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005aab00 | re/match/s25/005aab00.c | ok | 18431 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005aab70 | re/match/s25/005aab70.c | ok | 18396 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005acd10 | re/match/s26/005acd10.c | ok | 32 | frame 0 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005ad320 | re/match/s26/005ad320.c | ok | 92 | frame 8 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0045c550 | re/match/s10/0045c550.c | ok | 8046 | frame 7 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x005acda0 | re/match/s26/005acda0.c | ok | 10 | frame 3 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x0040c690 | re/match/racepath/0040c690.c | ok | 8288 | frame 22 abs_in 0 abs_out 0 indirect 0 | empty | IDENTICAL-FOR-2724 |
| 0x00411ae0 | re/match/racepath/00411ae0.c | ok | 2279 | frame 30 abs_in 2 abs_out 0 indirect 0 | covered | IDENTICAL-FOR-2724 |

## Outcome

- Command: `py.exe -3.12 re/tools/c4_reclass.py batch --tsv verify/provinsitu_20261111/promote44_batch.tsv --result verify/provinsitu_20261111/RESULT_PROV-PROMOTE-44.md --apply` (dry run first: 43 pass, 0 refused, 1 skip).
- **Replay-verified C4 (c4=frida-replay): 43 rows** (39 C2->C4, 1 C1->C4, 6 C3->C4 per the CHANGELOG lines). Write-set leg: 39 `empty` and the 4 `covered` rows (0x0041d930, 0x00429310, 0x004647f0, 0x00411ae0).
- Byte-matched (c4=match / c4=libid) promoted in this row: **0**, reported apart.
- Not promoted, listed with reason: 0x00546b10 stays as it was, already C4 by another route (hooks.csv row cites shadow_ab, no write). No row was refused by the gate; no STUB marker among the 43.
- Thin rows (fewer than 10 calls in the race window, verdict covers only those calls): 0x0048a4e0 (1), 0x005aaac0 (4), 0x005ae3a0 (8).
- Report per row: `promote44_report.tsv`; dry run: `promote44_dry.tsv`. hooks.csv notes carry `c4=frida-replay` and this RESULT path; CHANGELOG has 43 lines.
- No build, no gates (tracker row).
