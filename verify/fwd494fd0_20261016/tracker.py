"""hooks.csv + CHANGELOG transaction for the fwd494fd0 lane (re-classify side effects 1 and 4).

  0x00495080  C2 -> C3 RE-EARNED: the port now assembles the memory load, path1 carries a register-input (ESI) check
              that reads RED 8/8 on the unfixed build and GREEN 8/8 on the fixed one, path2 PASS 2/2, nav boot with the
              FULL hook set (1290 installed, no MASHED_HOOK_SKIP) reaches a race and survives 45 s.
  0x004f8660 / 0x004f8690 / 0x00496930: the three race-scenario path2 rows, re-run WITHOUT the skip workaround.

    py -3.12 tracker.py            # dry run
    py -3.12 tracker.py --apply
"""
import csv
import io
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
DATE = "2026-10-16"
EV = "verify/fwd494fd0_20261016"
W = ROOT / "verify" / "fwd494fd0_20261016" / "win1"


def entries(name: str) -> str:
    t = (W / f"path2_{name}.txt").read_text(encoding="utf-8", errors="replace")
    m = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
    op = re.search(r"opcode:\s+\S+\s+expected 0xe9\s+->\s+(\w+)", t)
    rel = re.search(r"rel32:\s+\S+\s+expected \S+\s+->\s+(\w+)", t)
    assert m and op.group(1) == "OK" and rel.group(1) == "OK" and m.group(1) == m.group(2), name
    return f"PASS (E9 + rel32 to the survivor, {m.group(1)}/{m.group(2)} entries)"


FIX = (f"C2->C3 RE-EARNED {DATE} (fwd494fd0): the one-line fix `mov esi, dword ptr ds:[771A0Ch]` (was the bare "
       "`[771A0Ch]`, which MSVC inline asm assembles as the immediate `mov esi, 0x771a0c`; built export now "
       "3E 8B 35 0C 1A 77 00). Register-input check added to path1: stub_esi records ESI at the stubbed callee "
       "FUN_00494fd0 (reads it as the DirectInput device pointer, mov eax,[esi] at 0x00494fee) with the source global "
       "0x00771a0c seeded to 0xD1D0C0DE. Negative control, UNFIXED build: RED 8/8, orig esi=0xd1d0c0de vs reimpl "
       "esi=0x771a0c with every stack argument equal (path1_before_fix.txt). Fixed build: path1 GREEN 8/8 "
       f"(path1_after_fix.txt), path2 {entries('input_axis_scale_bias_apply')}, nav boot to a race with the FULL hook "
       "set (1290 hooks installed incl. this one, no MASHED_HOOK_SKIP) reaches the race and the process stays alive, full "
       f"set survives 45 s. scripts/lint_inline_asm_abs.py now fails the build on the bare form. KNOWN_ISSUES "
       f"AUTOHOOK-INLINE-ASM-IMMEDIATE closed. {EV}/RESULT_fwd494fd0.md")

RACE = {0x004F8660: "plugin_data_dword_a", 0x004F8690: "plugin_data_dword_b", 0x00496930: "table_773030_get"}
LEVEL_ONLY_FOR = {0x00495080}

changelog = [(0x00495080, "Fwd494fd0_DoublePlusHalf", "C2->C3 (RE-EARNED)",
              "Fixed `mov esi, dword ptr ds:[771A0Ch]` (was an immediate). path1 now checks ESI: RED 8/8 on the unfixed build "
              f"(esi=0x771a0c vs 0xd1d0c0de), GREEN 8/8 fixed; path2 {entries('input_axis_scale_bias_apply')}; nav boot to a "
              "race with the full hook set (1290 installed, no skip) alive; full set 45 s. Inline-asm lint added to build.bat.")]
for rva, k in RACE.items():
    changelog.append((rva, "", "(no ladder move)",
                      f"path2 re-run with the FULL hook set, no MASHED_HOOK_SKIP: {entries(k)}."))


def main() -> int:
    apply = "--apply" in sys.argv
    p = ROOT / "hooks.csv"
    rows = list(csv.reader(io.StringIO(p.read_bytes().decode("utf-8"), newline="")))
    ix = {c: i for i, c in enumerate(rows[0])}
    by = {}
    for r in rows[1:]:
        try:
            by[int(r[0], 16)] = r
        except (ValueError, IndexError):
            pass
    r = by[0x00495080]
    assert r[ix["confidence"]] == "C2", r[ix["confidence"]]
    r[ix["confidence"]] = "C3"
    r[ix["notes"]] = r[ix["notes"]] + " | " + FIX
    print("0x00495080  C2 -> C3")
    for rva, k in RACE.items():
        r = by[rva]
        r[ix["notes"]] += (f" | RE-RUN {DATE} (fwd494fd0): path2 with the FULL hook set and NO MASHED_HOOK_SKIP: "
                           f"{entries(k)} (the nav drive no longer dies; {EV}/win1/path2_{k}.txt)")
        print(f"0x{rva:08x}  path2 note")
    buf = io.StringIO(newline="")
    csv.writer(buf, lineterminator="\r\n").writerows(rows)
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
    ct = cp.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in ct else "\n"
    L = ct.split(nl)
    at = [i for i, l in enumerate(L) if l == "<!-- ENTRIES -->"]
    assert len(at) == 1
    lines = []
    for rva, name, lvl, txt in changelog:
        nm = name or by[rva][ix["name"]]
        lines.append(f"{DATE}  {rva:08x}  {nm}  {lvl}  {txt} Evidence {EV}/RESULT_fwd494fd0.md.")
    L[at[0] + 1:at[0] + 1] = lines
    print(len(lines), "changelog lines")
    if apply:
        p.write_bytes(buf.getvalue().encode("utf-8"))
        cp.write_bytes(nl.join(L).encode("utf-8"))
        print("written")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
