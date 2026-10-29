"""Self-test for re/tools/uncertainty_resolve.py (runs against a fake repo root, never the real trackers)."""
import importlib.util, pathlib, pytest

TOOL = pathlib.Path(__file__).resolve().parents[1] / "uncertainty_resolve.py"

UNC = (
    "# U\r\n\r\n## Active uncertainties\r\n\r\n"
    "| ID | Type | Where | Statement | Evidence missing | Path to resolution | Blocks |\r\n"
    "|----|------|-------|-----------|------------------|--------------------|--------|\n"
    "| U-1 | structural | w1 | s1 | e1 | p1 | b1 |\r\n"
    "| U-2 | semantic | w2 | s2 | e2 | p2 | b2 |\n"
    "| U-3 | ~~environmental~~ | w3 | s3 | n/a | n/a | nothing |\n"
    "\n## Resolved (audit trail)\n\n| U-9 | x | y | z | a | b | c |\n"
)
CL = "# Changelog\n\n<!-- ENTRIES -->\n2026-01-01  old entry\n"


@pytest.fixture
def mod(tmp_path, monkeypatch):
    spec = importlib.util.spec_from_file_location("uncertainty_resolve", TOOL)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    (tmp_path / "re" / "analysis").mkdir(parents=True)
    (tmp_path / "ev.md").write_text("evidence", encoding="utf-8")
    (tmp_path / "UNCERTAINTIES.md").write_bytes(UNC.encode())
    (tmp_path / "re" / "analysis" / "CHANGELOG.md").write_bytes(CL.encode())
    monkeypatch.setattr(m, "ROOT", tmp_path)
    monkeypatch.setattr(m, "UNC", tmp_path / "UNCERTAINTIES.md")
    monkeypatch.setattr(m, "CHANGELOG", tmp_path / "re" / "analysis" / "CHANGELOG.md")
    return m


def run(m, monkeypatch, *argv):
    monkeypatch.setattr("sys.argv", ["uncertainty_resolve.py", *argv])
    m.main()


def test_resolve_strikes_row_keeps_eols_and_logs(mod, monkeypatch, tmp_path):
    run(mod, monkeypatch, "resolve", "--reason", "fixed it.", "--evidence", "ev.md", "--date", "2026-10-28", "U-1", "U-2")
    raw = (tmp_path / "UNCERTAINTIES.md").read_bytes().decode()
    r1 = next(l for l in raw.split("\n") if l.startswith("| U-1 |"))
    assert r1.endswith("|\r") and "~~structural~~" in r1
    assert "**RESOLVED 2026-10-28** (ev.md): fixed it. Prior statement: s1" in r1
    assert r1.count(" | ") == 6 and "| n/a | n/a | nothing |" in r1
    r2 = next(l for l in raw.split("\n") if l.startswith("| U-2 |"))
    assert not r2.endswith("\r")
    assert "| U-9 | x | y | z | a | b | c |" in raw          # Resolved section untouched
    cl = (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_text()
    assert cl.index("U-1  UNCERTAINTIES") < cl.index("U-2  UNCERTAINTIES") < cl.index("old entry")
    assert "U-1  UNCERTAINTIES  Active->struck  fixed it (evidence: ev.md)" in cl


def test_idempotent_second_run_refuses_and_writes_nothing(mod, monkeypatch, tmp_path):
    args = ("resolve", "--reason", "r", "--evidence", "ev.md", "U-1")
    run(mod, monkeypatch, *args)
    before = (tmp_path / "UNCERTAINTIES.md").read_bytes()
    cl_before = (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_bytes()
    with pytest.raises(SystemExit, match="already struck"):
        run(mod, monkeypatch, *args)
    assert (tmp_path / "UNCERTAINTIES.md").read_bytes() == before
    assert (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_bytes() == cl_before


@pytest.mark.parametrize("argv,msg", [
    (["--reason", "r", "--evidence", "ev.md", "U-404"], "no row"),
    (["--reason", "r", "--evidence", "nope.md", "U-1"], "evidence file not found"),
    (["--reason", " . ", "--evidence", "ev.md", "U-1"], "empty reason"),
    (["--reason", "r", "--evidence", "ev.md", "U-3"], "already struck"),
    (["--reason", "r", "--evidence", "ev.md", "U-9"], "no row"),
    (["--reason", "a | b", "--evidence", "ev.md", "U-1"], "' | '"),
])
def test_refusals(mod, monkeypatch, tmp_path, argv, msg):
    before = (tmp_path / "UNCERTAINTIES.md").read_bytes()
    with pytest.raises(SystemExit, match=msg):
        run(mod, monkeypatch, "resolve", *argv)
    assert (tmp_path / "UNCERTAINTIES.md").read_bytes() == before


def test_all_or_nothing(mod, monkeypatch, tmp_path):
    before = (tmp_path / "UNCERTAINTIES.md").read_bytes()
    with pytest.raises(SystemExit):
        run(mod, monkeypatch, "resolve", "--reason", "r", "--evidence", "ev.md", "U-1", "U-404")
    assert (tmp_path / "UNCERTAINTIES.md").read_bytes() == before


def test_partial_keeps_blocks(mod, monkeypatch, tmp_path):
    run(mod, monkeypatch, "resolve", "--partial", "--reason", "r", "--evidence", "ev.md", "--date", "2026-10-28", "U-2")
    raw = (tmp_path / "UNCERTAINTIES.md").read_text()
    row = next(l for l in raw.splitlines() if l.startswith("| U-2 |"))
    assert "**RESOLVED (PARTIAL) 2026-10-28**" in row and row.endswith("| b2 |")
