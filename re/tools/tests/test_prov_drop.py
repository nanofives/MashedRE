"""Self-test for re/tools/match/prov_drop.py (MATCH-PROV-DROP-RULE). Fake repo root in a tmp dir; the real
PROVISIONAL.tsv is never touched and the compiler is never run (CHECKER is replaced by canned match/relocheck results)."""
import importlib.util
import pathlib

import pytest

TOOL = pathlib.Path(__file__).resolve().parents[1] / "match" / "prov_drop.py"

HDR = "rva\tname\tsource\tflags\tbest_pct\tequiv\tstatus\tevidence"
ROWS = [
    # rva, source, status
    ("0x00401000", "re/match/s1/00401000.c", "provisional"),   # byte-identical, included directly -> DROP
    ("0x00401100", "re/match/s1/00401100.c", "provisional"),   # byte-identical, included through a wrapper header -> DROP
    ("0x00401200", "re/match/s1/00401200.c", "provisional"),   # byte-identical, no TU includes it -> not-included
    ("0x00401300", "re/match/s1/00401300.c", "provisional"),   # included, 3 bytes differ -> not-byte-identical
    ("0x00401400", "re/match/s1/00401400.c", "refused"),       # included, relocheck BAD -> relocheck
    ("0x00401500", "RwCore/RwCore_Lifted_NN.cpp", "provisional"),  # lifted -> no-re-match-source
    ("0x00401600", "re/match/s1/00401600.c", "provisional"),   # in ADMITTED.tsv -> admitted
    ("0x00401700", "re/match/s1/00401700.c", "provisional"),   # naked -> naked
    ("0x00401800", "re/match/s1/00401800.c", "provisional"),   # TOOLCHAIN vc19 -> toolchain
    ("0x00401900", "re/match/s1/missing.c", "provisional"),    # file absent -> source-missing
    ("0x00401a00", "re/match/s1/00401a00.c", "provisional"),   # included only by a TU NOT in exe_sources.rsp
    ("0x00401b00", "re/match/s1/00401b00.c", "provisional"),   # included only on a commented-out line
    ("0x00401c00", "re/match/s1/00401c00.c", "provisional"),   # included, no MATCH symbol at the RVA -> no-symbol
]

CANNED = {
    "00401000.c": {0x00401000: (100.0, 40, 0, "OK", "checked=3 unchecked=0")},
    "00401100.c": {0x00401100: (100.0, 40, 0, "OK", "checked=2 unchecked=0")},
    "00401200.c": {0x00401200: (100.0, 40, 0, "OK", "checked=2 unchecked=0")},
    "00401300.c": {0x00401300: (98.0, 40, 3, "OK", "checked=2 unchecked=0")},
    "00401400.c": {0x00401400: (100.0, 40, 0, "BAD", "+0x4 _DAT_x")},
    "00401600.c": {0x00401600: (100.0, 40, 0, "OK", "")},
    "00401700.c": {0x00401700: (100.0, 40, 0, "OK", "")},
    "00401800.c": {0x00401800: (100.0, 40, 0, "OK", "")},
    "00401a00.c": {0x00401a00: (100.0, 40, 0, "OK", "")},
    "00401b00.c": {0x00401b00: (100.0, 40, 0, "OK", "")},
    "00401c00.c": {0x00401d00: (100.0, 40, 0, "OK", "")},
}


def load():
    spec = importlib.util.spec_from_file_location("prov_drop_ut", TOOL)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


def fake_checker(src):
    return CANNED[src.name]


@pytest.fixture
def repo(tmp_path):
    r = tmp_path
    mdir = r / "re" / "match"
    (mdir / "s1").mkdir(parents=True)
    lines = ["# L-126 provisional bodies. comment line kept by --apply", HDR]
    for rva, src, st in ROWS:
        lines.append("\t".join([rva, "FUN_" + rva[2:], src, "/O2", "50.0", "NOT-PROVEN", st, "ev.md"]))
    # CRLF on purpose: --apply must keep every byte of the surviving lines
    (mdir / "PROVISIONAL.tsv").write_bytes(("\r\n".join(lines) + "\r\n").encode())
    (mdir / "ADMITTED.tsv").write_text("# admitted\nrva\tname\tsource\n0x00401600\tF\tre/match/s1/00401600.c\n")
    for rva, src, _ in ROWS:
        if src.startswith("re/match/") and "missing" not in src:
            body = f"// FLAGS: /O2 /Ob1 /G5\n// MATCH: {rva} _FUN_{rva[2:]}\nint FUN_{rva[2:]}(void) {{ return 0; }}\n"
            if rva == "0x00401700":
                body = body.replace("int FUN_", "__declspec(naked) int FUN_")
            if rva == "0x00401800":
                body = "// TOOLCHAIN vc19\n" + body
            (r / src).write_text(body)
    sdir = r / "mashedmod" / "src" / "mashed_re"
    (sdir / "RwCore").mkdir(parents=True)
    (sdir / "Match").mkdir(parents=True)
    (sdir / "RwCore" / "RwCore_00401000.cpp").write_text(
        '// wrapper\n#define MATCH_PORT 1\n#include "../../../../re/match/s1/00401000.c"\n')
    (sdir / "Match" / "Wrap.h").write_text('#pragma once\n#include "../../../../re/match/s1/00401100.c"\n')
    (sdir / "Match" / "PortTop_00401100.cpp").write_text('#include "Wrap.h"\n')
    (sdir / "Match" / "Many.cpp").write_text(
        '#include "../../../../re/match/s1/00401300.c"\n#include "../../../../re/match/s1/00401400.c"\n'
        '#include "../../../../re/match/s1/00401600.c"\n#include "../../../../re/match/s1/00401700.c"\n'
        '#include "../../../../re/match/s1/00401800.c"\n#include "../../../../re/match/s1/00401c00.c"\n'
        '// #include "../../../../re/match/s1/00401b00.c"\n')
    (sdir / "Match" / "NotInExe.cpp").write_text('#include "../../../../re/match/s1/00401a00.c"\n')
    (r / "mashedmod" / "exe_sources.rsp").write_text(
        '"exe_main.cpp"\n"RwCore\\RwCore_00401000.cpp"\n"Match\\PortTop_00401100.cpp"\n"Match\\Many.cpp"\n')
    return r


def run(m, repo, *extra):
    m.CHECKER = fake_checker
    return m.main(["--root", str(repo), "--jobs", "2", *extra])


EXPECT = {
    "0x00401000": ("DROP", ""), "0x00401100": ("DROP", ""),
    "0x00401200": ("KEEP", "not-included"), "0x00401300": ("KEEP", "not-byte-identical"),
    "0x00401400": ("KEEP", "relocheck"), "0x00401500": ("KEEP", "no-re-match-source"),
    "0x00401600": ("KEEP", "admitted"), "0x00401700": ("KEEP", "naked"), "0x00401800": ("KEEP", "toolchain"),
    "0x00401900": ("KEEP", "source-missing"), "0x00401a00": ("KEEP", "not-included"),
    "0x00401b00": ("KEEP", "not-included"), "0x00401c00": ("KEEP", "no-symbol"),
}


def test_classify_every_reason(repo, tmp_path):
    m = load()
    out = tmp_path / "out.tsv"
    before = (repo / "re/match/PROVISIONAL.tsv").read_bytes()
    assert run(m, repo, "--tsv", str(out)) == 0
    assert (repo / "re/match/PROVISIONAL.tsv").read_bytes() == before, "dry run must not write"
    rows = [ln.split("\t") for ln in out.read_text().splitlines()[1:]]
    got = {r[2]: (r[0], "" if r[1] == "-" else r[1]) for r in rows}
    assert got == EXPECT


def test_include_evidence_cites_file_line(repo, tmp_path):
    m = load()
    out = tmp_path / "out.tsv"
    run(m, repo, "--tsv", str(out))
    rows = {r.split("\t")[2]: r.split("\t") for r in out.read_text().splitlines()[1:]}
    assert rows["0x00401000"][9] == "mashedmod/src/mashed_re/RwCore/RwCore_00401000.cpp:3"
    assert rows["0x00401100"][9] == "mashedmod/src/mashed_re/Match/Wrap.h:2"
    assert "BYTE-IDENTICAL" in rows["0x00401000"][7] and rows["0x00401000"][8].startswith("OK")
    assert rows["0x00401000"][5] == "ddk" and rows["0x00401000"][6] == "/O2 /Ob1 /G5"
    # a kept not-included row still carries its byte evidence (the 0x00551a50 case)
    assert "BYTE-IDENTICAL" in rows["0x00401200"][7]


def test_apply_drops_only_drop_rows_and_keeps_bytes(repo):
    m = load()
    p = repo / "re/match/PROVISIONAL.tsv"
    before = p.read_bytes()
    assert run(m, repo, "--apply") == 0
    after = p.read_bytes()
    expect = b"".join(ln for ln in before.splitlines(keepends=True)
                      if not (ln.startswith(b"0x00401000\t") or ln.startswith(b"0x00401100\t")))
    assert after == expect
    assert after.startswith(b"# L-126") and b"\r\n" in after
    # idempotent: a second apply drops nothing
    run(m, repo, "--apply")
    assert p.read_bytes() == expect


def test_only_limits_rows(repo, tmp_path):
    m = load()
    out = tmp_path / "out.tsv"
    run(m, repo, "--only", "0x00401000,0x00401300", "--tsv", str(out))
    rows = out.read_text().splitlines()[1:]
    assert sorted(r.split("\t")[2] for r in rows) == ["0x00401000", "0x00401300"]


def test_match_error_is_kept(repo, tmp_path):
    m = load()

    def boom(src):
        if src.name == "00401000.c":
            raise RuntimeError("match-error: C2143")
        return CANNED[src.name]
    m.CHECKER = boom
    out = tmp_path / "out.tsv"
    m.main(["--root", str(repo), "--tsv", str(out)])
    rows = {r.split("\t")[2]: r.split("\t") for r in out.read_text().splitlines()[1:]}
    assert rows["0x00401000"][0] == "KEEP" and rows["0x00401000"][1] == "match-error"
