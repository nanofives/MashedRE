"""Self-test for re/tools/c1_add.py (fake repo root, never the real trackers). HB-CHANGELOG-MARKER."""
import importlib.util, pathlib, pytest

TOOL = pathlib.Path(__file__).resolve().parents[1] / "c1_add.py"

# The header prose names the marker inline, exactly as the real CHANGELOG line 13 does.
CL = ("# Changelog\r\n\r\n**prepended immediately below the `<!-- ENTRIES -->` marker**, never appended to the end.\r\n"
      "\r\n<!-- ENTRIES -->\r\n2026-01-01  old entry\r\n")
HOOKS = "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes,exe_file,port\r\n00401000,A,boot,C2,mapped,,,,n,,\r\n"


@pytest.fixture
def mod(tmp_path, monkeypatch):
    spec = importlib.util.spec_from_file_location("c1_add", TOOL)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    (tmp_path / "re" / "analysis").mkdir(parents=True)
    (tmp_path / "R.md").write_text("r", encoding="utf-8")
    (tmp_path / "hooks.csv").write_bytes(HOOKS.encode())
    (tmp_path / "re" / "analysis" / "CHANGELOG.md").write_bytes(CL.encode())
    monkeypatch.setattr(m, "ROOT", tmp_path)
    monkeypatch.setattr(m, "HOOKS", tmp_path / "hooks.csv")
    monkeypatch.setattr(m, "CHANGELOG", tmp_path / "re" / "analysis" / "CHANGELOG.md")
    return m


ARGS = ["--rva", "0x0040BB10", "--subsystem", "gameplay", "--file", "re/match/s29/0040bb10.c", "--why", "callback.",
        "--result", "R.md", "--tag", "T", "--date", "2026-11-10"]


def test_inserts_below_the_marker_line_not_the_header_prose(mod, tmp_path):
    assert mod.main(ARGS) == 0
    lines = (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_bytes().decode().split("\r\n")
    assert lines[2] == "**prepended immediately below the `<!-- ENTRIES -->` marker**, never appended to the end."
    i = lines.index("<!-- ENTRIES -->")
    assert lines[i + 1] == "2026-11-10  0x0040bb10  FUN_0040bb10  (none)->C1  added (T): callback; evidence: R.md"
    assert lines[i + 2] == "2026-01-01  old entry"
    assert len(lines) == len(CL.split("\r\n")) + 1


def test_hooks_row_appended_with_crlf_and_11_fields(mod, tmp_path):
    mod.main(ARGS)
    raw = (tmp_path / "hooks.csv").read_bytes().decode()
    assert "\n" not in raw.replace("\r\n", "")
    last = raw.split("\r\n")[-2]
    assert last.startswith("0040bb10,FUN_0040bb10,gameplay,C1,mapped,re/match/s29/0040bb10.c,")
    assert len(next(iter(__import__("csv").reader([last])))) == 11


def test_refuses_existing_row(mod):
    with pytest.raises(SystemExit):
        mod.main(["--rva", "0x00401000"] + ARGS[2:])


@pytest.mark.parametrize("text", ["# c\n`<!-- ENTRIES -->` only in prose\n", "<!-- ENTRIES -->\nx\n<!-- ENTRIES -->\n"])
def test_refuses_missing_or_doubled_marker_line(mod, text):
    with pytest.raises(SystemExit):
        mod.prepend_changelog(text, ["e"])


def test_dry_run_writes_nothing(mod, tmp_path):
    mod.main(ARGS + ["--dry-run"])
    assert (tmp_path / "hooks.csv").read_bytes().decode() == HOOKS
    assert (tmp_path / "re" / "analysis" / "CHANGELOG.md").read_bytes().decode() == CL
